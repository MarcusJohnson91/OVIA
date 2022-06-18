#include "../../include/StreamIO/MPEG2Stream.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    // Each elementary stream needs it's own PES packet.
    // A Program Stream can contain multiple elementary streams encapsulated into PES packets.
    // Transport Streams and Program streams are like brothers, they do the same things, but some are more capable than others.
    
    // Access Unit = A coded representation of a Presentation Unit. In audio, an AU = a frame. in video, an AU = a picture + all padding and metadata.
    // PacketID    = an int that identifies elementary streams (audio or video) in a program.
    // Program     = Elementary streams that are to be played synchronized with the same time base.
    
    // Transport Streams CAN CONTAIN PROGRAM STREAMS, OR ELEMENTARY STREAMS.
    
    // So, for Demuxing, the general idea is to accumulate PES packets until you've got a whole NAL or whateve?
    // Also, we need a way to identify the stream type
    
    
    
    
    /* REAL INFO */
    // Transport streams have no global header.
    // Packet size is 188 bytes, M2ts adds 4 bytes for copyright and timestamp.
    
    static void ParseConditionalAccessDescriptor(MPEG2Options *Options, BitBuffer *BitB) { // CA_descriptor
        int N                                                 = 0;// TODO: what is N?
        uint8_t  DescriptorTag                                = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);// descriptor_tag
        uint8_t  DescriptorSize                               = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);// descriptor_length
        uint16_t ConditionalAccessID                          = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 16);// CA_system_ID
        BitBuffer_Seek(BitB, 3); // reserved
        uint16_t  ConditionalAccessPID                        = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        for (int i = 0; i < N; i++) {
            BitBuffer_Seek(BitB, 8); // private_data_byte
        }
    }
    
    static void ParseConditionalAccessSection(MPEG2Options *Options, BitBuffer *BitB) { // CA_section
        int N = 0; // TODO: find out what the hell N is
        Options->Condition->TableID                = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        Options->Condition->SectionSyntaxIndicator = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1);
        BitBuffer_Seek(BitB, 3); // "0" + 2 bits reserved.
        Options->Condition->SectionSize            = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 12);
        BitBuffer_Seek(BitB, 18);
        Options->Condition->VersionNum             = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 5);
        Options->Condition->CurrentNextIndicator   = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1);
        Options->Condition->SectionNumber          = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        Options->Condition->LastSectionNumber      = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        for (int i = 0; i < N; i++) {
            ParseConditionalAccessDescriptor(Options, BitB);
        }
        Options->Condition->ConditionCRC32         = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
    }
    
    static void ParseProgramAssociationTable(MPEG2Options *Options, BitBuffer *BitB) { // program_association_section
        Options->Program->TableID                = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        Options->Program->SectionSyntaxIndicator = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1);
        BitBuffer_Seek(BitB, 3); // "0" + 2 bits reserved.
        Options->Program->SectionSize            = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 12);
        Options->Program->TransportStreamID      = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 16);
        BitBuffer_Seek(BitB, 2); // Reserved
        Options->Program->VersionNum             = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 5);
        Options->Program->CurrentNextIndicator   = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1);
        Options->Program->SectionNumber          = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        Options->Program->LastSectionNumber      = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        Options->Program->ProgramNumber          = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 16);
        Options->Program->NetworkPID             = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 13);
        Options->Program->ProgramMapPID          = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 13);
        Options->Program->ProgramCRC32           = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
    }
    
    static void ParsePackHeader(MPEG2Options *Program, BitBuffer *BitB) { // pack_header
        Program->PSP->PackStartCode           = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        BitBuffer_Seek(BitB, 2); // 01
        Program->PSP->SystemClockRefBase1     = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 3);
        BitBuffer_Seek(BitB, 1); // marker_bit
        Program->PSP->SystemClockRefBase2     = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 15);
        BitBuffer_Seek(BitB, 1); // marker_bit
        Program->PSP->SystemClockRefBase3     = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 15);
        BitBuffer_Seek(BitB, 1); // marker_bit
        Program->PSP->SystemClockRefBase      = Program->PSP->SystemClockRefBase1 << 30;
        Program->PSP->SystemClockRefBase     += Program->PSP->SystemClockRefBase2 << 15;
        Program->PSP->SystemClockRefBase     += Program->PSP->SystemClockRefBase3;
        
        Program->PSP->SystemClockRefExtension = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 9);
        BitBuffer_Seek(BitB, 1); // marker_bit
        Program->PSP->ProgramMuxRate          = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 22);
        BitBuffer_Seek(BitB, 7); // marker_bit && reserved
        Program->PSP->PackStuffingSize        = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 3);
        BitBuffer_Seek(BitB, Bytes2Bits(Program->PSP->PackStuffingSize));
        if (BitBuffer_PeekBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 0) == 0) { // MPEG2TSSystemHeaderStartCode
                                                                                                     // system_header();
        }
    }
    
    static void ParsePESPacket(PacketizedElementaryStream *Stream, BitBuffer *BitB) { // PES_packet
        int N3                                       = 0, N2 = 0, N1 = 0;// FIXME : WTF IS N3, N2, and N1?
        Stream->PacketStartCodePrefix                = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 24);
        Stream->StreamID                             = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);// 13
        Stream->PESPacketSize                        = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 16);//
        if (Stream->StreamID != ProgramStreamFolder &&
            Stream->StreamID != AnnexA_DSMCCStream &&
            Stream->StreamID != ProgramStreamMap &&
            Stream->StreamID != PrivateStream2 &&
            Stream->StreamID != PaddingStream &&
            Stream->StreamID != ECMStream &&
            Stream->StreamID != EMMStream &&
            Stream->StreamID != TypeEStream) {
            BitBuffer_Seek(BitB, 2);
            Stream->PESScramblingControl             = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 2);
            Stream->PESPriority                      = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1);
            Stream->AlignmentIndicator               = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1);
            Stream->CopyrightIndicator               = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1);
            Stream->OriginalOrCopy                   = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1);
            Stream->PTSDTSFlags                      = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 2);
            Stream->ESCRFlag                         = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1);
            Stream->ESRateFlag                       = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1);
            Stream->DSMTrickModeFlag                 = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1);
            Stream->AdditionalCopyInfoFlag           = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1);
            Stream->PESCRCFlag                       = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1);
            Stream->PESExtensionFlag                 = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1);
            Stream->PESHeaderSize                    = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
            if (Stream->PTSDTSFlags == 2) {
                BitBuffer_Seek(BitB, 4);
                uint8_t  PTS1                        = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 3);
                BitBuffer_Seek(BitB, 1); // marker bit
                uint16_t PTS2                        = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 15);
                BitBuffer_Seek(BitB, 1); // marker bit
                uint16_t PTS3                        = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 15);
                BitBuffer_Seek(BitB, 1); // marker bit
                
                Stream->PTS                          = PTS1 << 30;
                Stream->PTS                         += PTS2 << 15;
                Stream->PTS                         += PTS3;
            }
            if (Stream->PTSDTSFlags == 3) {
                BitBuffer_Seek(BitB, 4);
                uint8_t  PTS1                        = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 3);
                BitBuffer_Seek(BitB, 1); // marker bit
                uint16_t PTS2                        = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 15);
                BitBuffer_Seek(BitB, 1); // marker bit
                uint16_t PTS3                        = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 15);
                BitBuffer_Seek(BitB, 1); // marker bit
                
                Stream->PTS                          = PTS1 << 30;
                Stream->PTS                         += PTS2 << 15;
                Stream->PTS                         += PTS3;
                
                BitBuffer_Seek(BitB, 4);
                uint8_t  DTS1                        = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 3);
                BitBuffer_Seek(BitB, 1); // marker bit
                uint16_t DTS2                        = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 15);
                BitBuffer_Seek(BitB, 1); // marker bit
                uint16_t DTS3                        = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 15);
                BitBuffer_Seek(BitB, 1); // marker bit
                
                Stream->DTS                          = DTS1 << 30;
                Stream->DTS                         += DTS2 << 15;
                Stream->DTS                         += DTS3;
            }
            if (Stream->ESCRFlag == true) {
                BitBuffer_Seek(BitB, 2);
                uint8_t  ESCR1                       = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 3);
                BitBuffer_Seek(BitB, 1); // marker bit
                uint16_t ESCR2                       = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 15);
                BitBuffer_Seek(BitB, 1); // marker bit
                uint16_t ESCR3                       = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 15);
                BitBuffer_Seek(BitB, 1); // marker bit
                
                Stream->ESCR                         = ESCR1 << 30;
                Stream->ESCR                        += ESCR2 << 15;
                Stream->ESCR                        += ESCR3;
            }
            if (Stream->ESRateFlag == true) {
                BitBuffer_Seek(BitB, 1);
                Stream->ESRate                       = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 22);
                BitBuffer_Seek(BitB, 1);
            }
            if (Stream->DSMTrickModeFlag == true) {
                Stream->TrickModeControl             = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 3);
                if (Stream->TrickModeControl        == FastForward) {
                    Stream->FieldID                  = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 2);
                    Stream->IntraSliceRefresh        = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1);
                    Stream->FrequencyTruncation      = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 2);
                } else if (Stream->TrickModeControl == SlowMotion) {
                    Stream->RepetitionControl        = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 5);
                } else if (Stream->TrickModeControl == FreezeFrame) {
                    Stream->FieldID                  = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 2);
                    BitBuffer_Seek(BitB, 3);
                } else if (Stream->TrickModeControl == FastRewind) {
                    Stream->FieldID                  = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 2);
                    Stream->IntraSliceRefresh        = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1);
                    Stream->FrequencyTruncation      = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 2);
                } else if (Stream->TrickModeControl == SlowRewind) {
                    Stream->RepetitionControl        = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 5);
                } else {
                    BitBuffer_Seek(BitB, 5);
                }
            }
            if (Stream->AdditionalCopyInfoFlag == true) {
                BitBuffer_Seek(BitB, 1);
                Stream->AdditionalCopyInfo           = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 7);
            }
            if (Stream->PESCRCFlag == true) {
                Stream->PreviousPESPacketCRC         = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 16);
            }
            if (Stream->PESExtensionFlag == true) {
                Stream->PESPrivateDataFlag           = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1);
                Stream->PackHeaderFieldFlag          = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1);
                Stream->ProgramPacketSeqCounterFlag  = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1);
                Stream->PSTDBufferFlag               = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1);
                Stream->PESExtensionFlag2            = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1);
                if (Stream->PESPrivateDataFlag == true) {
                    BitBuffer_Seek(BitB, 128);
                }
                if (Stream->PackHeaderFieldFlag == true) {
                    Stream->PackFieldSize            = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
                    //pack_header();
                }
                if (Stream->ProgramPacketSeqCounterFlag == true) {
                    BitBuffer_Seek(BitB, 1);
                    Stream->ProgramPacketSeqCounter  = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 7);
                    BitBuffer_Seek(BitB, 1);
                    Stream->MPEGVersionIdentifier    = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1);
                    Stream->OriginalStuffSize        = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 6);
                }
                if (Stream->PSTDBufferFlag == true) {
                    BitBuffer_Seek(BitB, 2);
                    Stream->PSTDBufferScale         = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1);
                    Stream->PSTDBufferSize          = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 13);
                }
                if (Stream->PESExtensionFlag2 == true) {
                    BitBuffer_Seek(BitB, 1);
                    Stream->PESExtensionFieldSize    = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 7);
                    Stream->StreamIDExtensionFlag    = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1);
                    if (Stream->StreamIDExtensionFlag == false) {
                        Stream->StreamIDExtension    = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 7);
                    } else {
                        BitBuffer_Seek(BitB, 6);
                        // tref_extension_flag
                        Stream->TREFFieldPresentFlag = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1);
                        if (Stream->TREFFieldPresentFlag == false) {
                            BitBuffer_Seek(BitB, 4);
                            uint8_t  TREF1           = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 3);
                            BitBuffer_Seek(BitB, 1); // marker bit
                            uint16_t TREF2           = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 15);
                            BitBuffer_Seek(BitB, 1); // marker bit
                            uint16_t TREF3           = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 15);
                            BitBuffer_Seek(BitB, 1); // marker bit
                            
                            Stream->TREF             = TREF1 << 30;
                            Stream->TREF             = TREF2 << 15;
                            Stream->TREF             = TREF3;
                        }
                    }
                    for (int i = 0; i < N3; i++) {
                        BitBuffer_Seek(BitB, 8); // Reserved
                    }
                }
                for (int i = 0; i < Stream->PESExtensionFieldSize; i++) {
                    BitBuffer_Seek(BitB, 8);
                }
            }
            for (int i = 0; i < N1; i++) {
                BitBuffer_Seek(BitB, 8); // stuffing_byte so 0b11111111, throw it away.
            }
            for (int i = 0; i < N2; i++) {
                BitBuffer_Seek(BitB, 8); // PES_packet_data_byte
            }
        } else if (Stream->StreamID == ProgramStreamFolder ||
                   Stream->StreamID == AnnexA_DSMCCStream ||
                   Stream->StreamID == ProgramStreamMap ||
                   Stream->StreamID == PrivateStream2 ||
                   Stream->StreamID == ECMStream ||
                   Stream->StreamID == EMMStream ||
                   Stream->StreamID == TypeEStream) {
            for (int i = 0; i < Stream->PESPacketSize; i++) {
                BitBuffer_Seek(BitB, 8); // PES_packet_data_byte
            }
        } else if (Stream->StreamID == PaddingStream) {
            for (int i = 0; i < Stream->PESPacketSize; i++) {
                BitBuffer_Seek(BitB, 8); // padding_byte
            }
        }
    }
    
    static void ParseTransportStreamAdaptionField(MPEG2Options *Options, BitBuffer *BitB) { // adaptation_field
        Options->Adaptation->AdaptationFieldSize                       = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        Options->Adaptation->DiscontinuityIndicator                    = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1);
        Options->Adaptation->RandomAccessIndicator                     = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1);
        Options->Adaptation->ElementaryStreamPriorityIndicator         = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1);
        Options->Adaptation->PCRFlag                                   = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1);
        Options->Adaptation->OPCRFlag                                  = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1);
        Options->Adaptation->SlicingPointFlag                          = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1);
        Options->Adaptation->TransportPrivateDataFlag                  = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1);
        Options->Adaptation->AdaptationFieldExtensionFlag              = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1); // 16 bits read so far
        if (Options->Adaptation->PCRFlag == true) { // Reads 48 bits
            Options->Adaptation->ProgramClockReferenceBase             = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 33);
            BitBuffer_Seek(BitB, 6);
            Options->Adaptation->ProgramClockReferenceExtension        = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 9);
        }
        if (Options->Adaptation->OPCRFlag == true) { // Reads 48 bits
            Options->Adaptation->OriginalProgramClockRefBase           = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 33);
            BitBuffer_Seek(BitB, 6);
            Options->Adaptation->OriginalProgramClockRefExt            = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 9);
        }
        if (Options->Adaptation->SlicingPointFlag == true) { // Reads 8 bits
            Options->Adaptation->SpliceCountdown                       = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        }
        if (Options->Adaptation->TransportPrivateDataFlag == true) { // Reads up to 2056 bits
            Options->Adaptation->TransportPrivateDataSize              = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
            for (uint8_t PrivateByte = 0; PrivateByte < Options->Adaptation->TransportPrivateDataSize; PrivateByte++) {
                Options->Adaptation->TransportPrivateData[PrivateByte] = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
            }
        }
        if (Options->Adaptation->AdaptationFieldExtensionFlag == true) { // Reads 12 bits
            Options->Adaptation->AdaptationFieldExtensionSize          = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
            Options->Adaptation->LegalTimeWindowFlag                   = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1);
            Options->Adaptation->PiecewiseRateFlag                     = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1);
            Options->Adaptation->SeamlessSpliceFlag                    = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1);
            BitBuffer_Align(BitB, 1);
            if (Options->Adaptation->LegalTimeWindowFlag == true) { // Reads 16 bits
                Options->Adaptation->LegalTimeWindowValidFlag          = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1);
                if (Options->Adaptation->LegalTimeWindowValidFlag == true) {
                    Options->Adaptation->LegalTimeWindowOffset         = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 15);
                }
            }
            if (Options->Adaptation->PiecewiseRateFlag == true) { // Reads 24 bits
                BitBuffer_Seek(BitB, 2);
                Options->Adaptation->PiecewiseRateFlag                 = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 22);
            }
            if (Options->Adaptation->SeamlessSpliceFlag == true) { // Reads 44 bits
                Options->Adaptation->SpliceType                        = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
                uint8_t  DecodeTimeStamp1                                = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 3);
                BitBuffer_Seek(BitB, 1); // marker_bit
                uint16_t DecodeTimeStamp2                                = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 15);
                BitBuffer_Seek(BitB, 1); // marker_bit
                uint16_t DecodeTimeStamp3                                = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 15);
                BitBuffer_Seek(BitB, 1); // marker_bit
                
                Options->Adaptation->DecodeTimeStampNextAU             = DecodeTimeStamp1 << 30;
                Options->Adaptation->DecodeTimeStampNextAU            += DecodeTimeStamp2 << 15;
                Options->Adaptation->DecodeTimeStampNextAU            += DecodeTimeStamp3;
            }
            for (int i = 0; i < 184 - Options->Adaptation->AdaptationFieldSize; i++) { // So, the minimum number of bytes to read is 188 - 284, aka -96 lol
                BitBuffer_Seek(BitB, 8); // Reserved
            }
        }
        for (int i = 0; i < 184 - Options->Adaptation->AdaptationFieldSize; i++) {
            BitBuffer_Seek(BitB, 8); // Stuffing so 0b11111111, throw it away.
        }
    }
    
    static void ParseTransportStreamPacket(MPEG2Options *Options, BitBuffer *BitB, uint8_t *TransportStream, size_t TransportStreamSize) { // transport_packet
        Options->Packet->SyncByte                              = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        Options->Packet->TransportErrorIndicator               = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1); // false
        Options->Packet->StartOfPayloadIndicator               = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1); // true
        Options->Packet->TransportPriorityIndicator            = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1); // false
        Options->Packet->PID                                   = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 13); // 0
        Options->Packet->TransportScramblingControl            = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 2); // 0
        Options->Packet->AdaptationFieldControl                = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 2); // 1
        Options->Packet->ContinuityCounter                     = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left,  4); // 0
        if (Options->Packet->AdaptationFieldControl == 2 || Options->Packet->AdaptationFieldControl == 3) {
            ParseTransportStreamAdaptionField(Options, BitB);
        }
        if (Options->Packet->AdaptationFieldControl == 1 || Options->Packet->AdaptationFieldControl == 3) {
            for (int i = 0; i < 184; i++) {
                TransportStream[i] = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left,  8); // data_byte, start copying data to the transport stream.
            }
        }
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
