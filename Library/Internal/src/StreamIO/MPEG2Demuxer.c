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

    static void ParseConditionalAccessDescriptor(MPEG2TransportStream *Transport, BitBuffer *BitB) { // CA_descriptor
        int N                                                 = 0;// TODO: what is N?
        uint8_t  DescriptorTag                                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);// descriptor_tag
        uint8_t  DescriptorSize                               = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);// descriptor_length
        uint16_t ConditionalAccessID                          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16);// CA_system_ID
        BitBuffer_Seek(BitB, 3); // reserved
        uint16_t  ConditionalAccessPID                        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        for (int i = 0; i < N; i++) {
            BitBuffer_Seek(BitB, 8); // private_data_byte
        }
    }

    static void ParseConditionalAccessSection(MPEG2TransportStream *Transport, BitBuffer *BitB) { // CA_section
        int N = 0; // TODO: find out what the hell N is
        Transport->Condition->TableID                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        Transport->Condition->SectionSyntaxIndicator = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
        BitBuffer_Seek(BitB, 3); // "0" + 2 bits reserved.
        Transport->Condition->SectionSize            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 12);
        BitBuffer_Seek(BitB, 18);
        Transport->Condition->VersionNum             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 5);
        Transport->Condition->CurrentNextIndicator   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
        Transport->Condition->SectionNumber          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        Transport->Condition->LastSectionNumber      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        for (int i = 0; i < N; i++) {
            ParseConditionalAccessDescriptor(BitB, Transport);
        }
        Transport->Condition->ConditionCRC32         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
    }

    static void ParseProgramAssociationTable(MPEG2TransportStream *Transport, BitBuffer *BitB) { // program_association_section
        Transport->Program->TableID                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        Transport->Program->SectionSyntaxIndicator = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
        BitBuffer_Seek(BitB, 3); // "0" + 2 bits reserved.
        Transport->Program->SectionSize            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 12);
        Transport->Program->TransportStreamID      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16);
        BitBuffer_Seek(BitB, 2); // Reserved
        Transport->Program->VersionNum             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 5);
        Transport->Program->CurrentNextIndicator   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
        Transport->Program->SectionNumber          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        Transport->Program->LastSectionNumber      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        Transport->Program->ProgramNumber          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16);
        Transport->Program->NetworkPID             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 13);
        Transport->Program->ProgramMapPID          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 13);
        Transport->Program->ProgramCRC32           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
    }

    static void ParsePackHeader(MPEG2ProgramStream *Program, BitBuffer *BitB) { // pack_header
        Program->PSP->PackStartCode           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        BitBuffer_Seek(BitB, 2); // 01
        Program->PSP->SystemClockRefBase1     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 3);
        BitBuffer_Seek(BitB, 1); // marker_bit
        Program->PSP->SystemClockRefBase2     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 15);
        BitBuffer_Seek(BitB, 1); // marker_bit
        Program->PSP->SystemClockRefBase3     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 15);
        BitBuffer_Seek(BitB, 1); // marker_bit
        Program->PSP->SystemClockRefBase      = Program->PSP->SystemClockRefBase1 << 30;
        Program->PSP->SystemClockRefBase     += Program->PSP->SystemClockRefBase2 << 15;
        Program->PSP->SystemClockRefBase     += Program->PSP->SystemClockRefBase3;
        
        Program->PSP->SystemClockRefExtension = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 9);
        BitBuffer_Seek(BitB, 1); // marker_bit
        Program->PSP->ProgramMuxRate          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 22);
        BitBuffer_Seek(BitB, 7); // marker_bit && reserved
        Program->PSP->PackStuffingSize        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 3);
        BitBuffer_Seek(BitB, Bytes2Bits(Program->PSP->PackStuffingSize));
        if (BitBuffer_PeekBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 0) == 0) { // MPEG2TSSystemHeaderStartCode
            // system_header();
        }
    }

    static void ParsePESPacket(PacketizedElementaryStream *Stream, BitBuffer *BitB) { // PES_packet
        int N3                                       = 0, N2 = 0, N1 = 0;// FIXME : WTF IS N3, N2, and N1?
        Stream->PacketStartCodePrefix                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 24);
        Stream->StreamID                             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);// 13
        Stream->PESPacketSize                        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16);//
        if (Stream->StreamID != ProgramStreamFolder &&
            Stream->StreamID != AnnexA_DSMCCStream &&
            Stream->StreamID != ProgramStreamMap &&
            Stream->StreamID != PrivateStream2 &&
            Stream->StreamID != PaddingStream &&
            Stream->StreamID != ECMStream &&
            Stream->StreamID != EMMStream &&
            Stream->StreamID != TypeEStream) {
            BitBuffer_Seek(BitB, 2);
            Stream->PESScramblingControl             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 2);
            Stream->PESPriority                      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
            Stream->AlignmentIndicator               = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
            Stream->CopyrightIndicator               = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
            Stream->OriginalOrCopy                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
            Stream->PTSDTSFlags                      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 2);
            Stream->ESCRFlag                         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
            Stream->ESRateFlag                       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
            Stream->DSMTrickModeFlag                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
            Stream->AdditionalCopyInfoFlag           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
            Stream->PESCRCFlag                       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
            Stream->PESExtensionFlag                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
            Stream->PESHeaderSize                    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
            if (Stream->PTSDTSFlags == 2) {
                BitBuffer_Seek(BitB, 4);
                uint8_t  PTS1                        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 3);
                BitBuffer_Seek(BitB, 1); // marker bit
                uint16_t PTS2                        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 15);
                BitBuffer_Seek(BitB, 1); // marker bit
                uint16_t PTS3                        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 15);
                BitBuffer_Seek(BitB, 1); // marker bit
                
                Stream->PTS                          = PTS1 << 30;
                Stream->PTS                         += PTS2 << 15;
                Stream->PTS                         += PTS3;
            }
            if (Stream->PTSDTSFlags == 3) {
                BitBuffer_Seek(BitB, 4);
                uint8_t  PTS1                        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 3);
                BitBuffer_Seek(BitB, 1); // marker bit
                uint16_t PTS2                        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 15);
                BitBuffer_Seek(BitB, 1); // marker bit
                uint16_t PTS3                        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 15);
                BitBuffer_Seek(BitB, 1); // marker bit
                
                Stream->PTS                          = PTS1 << 30;
                Stream->PTS                         += PTS2 << 15;
                Stream->PTS                         += PTS3;
                
                BitBuffer_Seek(BitB, 4);
                uint8_t  DTS1                        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 3);
                BitBuffer_Seek(BitB, 1); // marker bit
                uint16_t DTS2                        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 15);
                BitBuffer_Seek(BitB, 1); // marker bit
                uint16_t DTS3                        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 15);
                BitBuffer_Seek(BitB, 1); // marker bit
                
                Stream->DTS                          = DTS1 << 30;
                Stream->DTS                         += DTS2 << 15;
                Stream->DTS                         += DTS3;
            }
            if (Stream->ESCRFlag == true) {
                BitBuffer_Seek(BitB, 2);
                uint8_t  ESCR1                       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 3);
                BitBuffer_Seek(BitB, 1); // marker bit
                uint16_t ESCR2                       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 15);
                BitBuffer_Seek(BitB, 1); // marker bit
                uint16_t ESCR3                       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 15);
                BitBuffer_Seek(BitB, 1); // marker bit
                
                Stream->ESCR                         = ESCR1 << 30;
                Stream->ESCR                        += ESCR2 << 15;
                Stream->ESCR                        += ESCR3;
            }
            if (Stream->ESRateFlag == true) {
                BitBuffer_Seek(BitB, 1);
                Stream->ESRate                       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 22);
                BitBuffer_Seek(BitB, 1);
            }
            if (Stream->DSMTrickModeFlag == true) {
                Stream->TrickModeControl             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 3);
                if (Stream->TrickModeControl        == FastForward) {
                    Stream->FieldID                  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 2);
                    Stream->IntraSliceRefresh        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
                    Stream->FrequencyTruncation      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 2);
                } else if (Stream->TrickModeControl == SlowMotion) {
                    Stream->RepetitionControl        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 5);
                } else if (Stream->TrickModeControl == FreezeFrame) {
                    Stream->FieldID                  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 2);
                    BitBuffer_Seek(BitB, 3);
                } else if (Stream->TrickModeControl == FastRewind) {
                    Stream->FieldID                  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 2);
                    Stream->IntraSliceRefresh        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
                    Stream->FrequencyTruncation      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 2);
                } else if (Stream->TrickModeControl == SlowRewind) {
                    Stream->RepetitionControl        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 5);
                } else {
                    BitBuffer_Seek(BitB, 5);
                }
            }
            if (Stream->AdditionalCopyInfoFlag == true) {
                BitBuffer_Seek(BitB, 1);
                Stream->AdditionalCopyInfo           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 7);
            }
            if (Stream->PESCRCFlag == true) {
                Stream->PreviousPESPacketCRC         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16);
            }
            if (Stream->PESExtensionFlag == true) {
                Stream->PESPrivateDataFlag           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
                Stream->PackHeaderFieldFlag          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
                Stream->ProgramPacketSeqCounterFlag  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
                Stream->PSTDBufferFlag               = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
                Stream->PESExtensionFlag2            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
                if (Stream->PESPrivateDataFlag == true) {
                    BitBuffer_Seek(BitB, 128);
                }
                if (Stream->PackHeaderFieldFlag == true) {
                    Stream->PackFieldSize            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
                    //pack_header();
                }
                if (Stream->ProgramPacketSeqCounterFlag == true) {
                    BitBuffer_Seek(BitB, 1);
                    Stream->ProgramPacketSeqCounter  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 7);
                    BitBuffer_Seek(BitB, 1);
                    Stream->MPEGVersionIdentifier    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
                    Stream->OriginalStuffSize        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 6);
                }
                if (Stream->PSTDBufferFlag == true) {
                    BitBuffer_Seek(BitB, 2);
                    Stream->PSTDBufferScale         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
                    Stream->PSTDBufferSize          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 13);
                }
                if (Stream->PESExtensionFlag2 == true) {
                    BitBuffer_Seek(BitB, 1);
                    Stream->PESExtensionFieldSize    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 7);
                    Stream->StreamIDExtensionFlag    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
                    if (Stream->StreamIDExtensionFlag == false) {
                        Stream->StreamIDExtension    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 7);
                    } else {
                        BitBuffer_Seek(BitB, 6);
                        // tref_extension_flag
                        Stream->TREFFieldPresentFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
                        if (Stream->TREFFieldPresentFlag == false) {
                            BitBuffer_Seek(BitB, 4);
                            uint8_t  TREF1           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 3);
                            BitBuffer_Seek(BitB, 1); // marker bit
                            uint16_t TREF2           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 15);
                            BitBuffer_Seek(BitB, 1); // marker bit
                            uint16_t TREF3           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 15);
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

    static void ParseTransportStreamAdaptionField(MPEG2TransportStream *Transport, BitBuffer *BitB) { // adaptation_field
        Transport->Adaptation->AdaptationFieldSize                       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        Transport->Adaptation->DiscontinuityIndicator                    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
        Transport->Adaptation->RandomAccessIndicator                     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
        Transport->Adaptation->ElementaryStreamPriorityIndicator         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
        Transport->Adaptation->PCRFlag                                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
        Transport->Adaptation->OPCRFlag                                  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
        Transport->Adaptation->SlicingPointFlag                          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
        Transport->Adaptation->TransportPrivateDataFlag                  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
        Transport->Adaptation->AdaptationFieldExtensionFlag              = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1); // 16 bits read so far
        if (Transport->Adaptation->PCRFlag == true) { // Reads 48 bits
            Transport->Adaptation->ProgramClockReferenceBase             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 33);
            BitBuffer_Seek(BitB, 6);
            Transport->Adaptation->ProgramClockReferenceExtension        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 9);
        }
        if (Transport->Adaptation->OPCRFlag == true) { // Reads 48 bits
            Transport->Adaptation->OriginalProgramClockRefBase           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 33);
            BitBuffer_Seek(BitB, 6);
            Transport->Adaptation->OriginalProgramClockRefExt            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 9);
        }
        if (Transport->Adaptation->SlicingPointFlag == true) { // Reads 8 bits
            Transport->Adaptation->SpliceCountdown                       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        }
        if (Transport->Adaptation->TransportPrivateDataFlag == true) { // Reads up to 2056 bits
            Transport->Adaptation->TransportPrivateDataSize              = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
            for (uint8_t PrivateByte = 0; PrivateByte < Transport->Adaptation->TransportPrivateDataSize; PrivateByte++) {
                Transport->Adaptation->TransportPrivateData[PrivateByte] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
            }
        }
        if (Transport->Adaptation->AdaptationFieldExtensionFlag == true) { // Reads 12 bits
            Transport->Adaptation->AdaptationFieldExtensionSize          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
            Transport->Adaptation->LegalTimeWindowFlag                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
            Transport->Adaptation->PiecewiseRateFlag                     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
            Transport->Adaptation->SeamlessSpliceFlag                    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
            BitBuffer_Align(BitB, 1);
            if (Transport->Adaptation->LegalTimeWindowFlag == true) { // Reads 16 bits
                Transport->Adaptation->LegalTimeWindowValidFlag          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
                if (Transport->Adaptation->LegalTimeWindowValidFlag == true) {
                    Transport->Adaptation->LegalTimeWindowOffset         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 15);
                }
            }
            if (Transport->Adaptation->PiecewiseRateFlag == true) { // Reads 24 bits
                BitBuffer_Seek(BitB, 2);
                Transport->Adaptation->PiecewiseRateFlag                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 22);
            }
            if (Transport->Adaptation->SeamlessSpliceFlag == true) { // Reads 44 bits
                Transport->Adaptation->SpliceType                        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
                uint8_t  DecodeTimeStamp1                                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 3);
                BitBuffer_Seek(BitB, 1); // marker_bit
                uint16_t DecodeTimeStamp2                                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 15);
                BitBuffer_Seek(BitB, 1); // marker_bit
                uint16_t DecodeTimeStamp3                                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 15);
                BitBuffer_Seek(BitB, 1); // marker_bit
                
                Transport->Adaptation->DecodeTimeStampNextAU             = DecodeTimeStamp1 << 30;
                Transport->Adaptation->DecodeTimeStampNextAU            += DecodeTimeStamp2 << 15;
                Transport->Adaptation->DecodeTimeStampNextAU            += DecodeTimeStamp3;
            }
            for (int i = 0; i < 184 - Transport->Adaptation->AdaptationFieldSize; i++) { // So, the minimum number of bytes to read is 188 - 284, aka -96 lol
                BitBuffer_Seek(BitB, 8); // Reserved
            }
        }
        for (int i = 0; i < 184 - Transport->Adaptation->AdaptationFieldSize; i++) {
            BitBuffer_Seek(BitB, 8); // Stuffing so 0b11111111, throw it away.
        }
    }

    static void ParseTransportStreamPacket(MPEG2TransportStream *Transport, BitBuffer *BitB, uint8_t *TransportStream, size_t TransportStreamSize) { // transport_packet
        Transport->Packet->SyncByte                              = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        Transport->Packet->TransportErrorIndicator               = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1); // false
        Transport->Packet->StartOfPayloadIndicator               = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1); // true
        Transport->Packet->TransportPriorityIndicator            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1); // false
        Transport->Packet->PID                                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 13); // 0
        Transport->Packet->TransportScramblingControl            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 2); // 0
        Transport->Packet->AdaptationFieldControl                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 2); // 1
        Transport->Packet->ContinuityCounter                     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest,  4); // 0
        if (Transport->Packet->AdaptationFieldControl == 2 || Transport->Packet->AdaptationFieldControl == 3) {
            ParseTransportStreamAdaptionField(BitB, Transport);
        }
        if (Transport->Packet->AdaptationFieldControl == 1 || Transport->Packet->AdaptationFieldControl == 3) {
            for (int i = 0; i < 184; i++) {
                TransportStream[i] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest,  8); // data_byte, start copying data to the transport stream.
            }
        }
    }

    void DemuxMPEG2PESPackets(PacketizedElementaryStream *PESPacket, BitBuffer *BitB) {
		
    }

    void DemuxMPEG2ProgramStreamPacket(MPEG2ProgramStream *Program, BitBuffer *BitB) {

    }

    void DemuxMPEG2TransportStreamPacket(MPEG2TransportStream *Transport, BitBuffer *BitB) {

    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
