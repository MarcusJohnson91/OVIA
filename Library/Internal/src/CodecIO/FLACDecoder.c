#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

#include "../../../Dependencies/libPCM/Dependencies/BitIO/libBitIO/include/BitIO.h"
#include "../../../Dependencies/libPCM/libPCM/include/libPCM.h"
#include "../../include/libModernFLAC.h"
#include "../../include/Decoder/DecodeFLAC.h"
#include "../../include/FLACTypes.h"

#ifdef __cplusplus
extern "C" {
#endif
    /* Start User facing functions */
    
    /*!
     @abstract          Copies frames from the stream pointed to by BitI, to OutputFrameBuffer (which needs to be freed by you)
     @param     StartFrame IS NOT zero indexed.
     */
    /*
    uint8_t *CopyFLACFrame(BitInput *BitI, DecodeFLAC *Dec) { // for apps that don't care about metadata
        
        // scan stream for FrameMagic, once found, start counting until you hit StartFrame
        
        // See if there's a seektable, if so use that to get as close as possible, otherwise scan byte by byte.
        // Which means we need metadata flags.
        //
        for (size_t StreamByte = 0; StreamByte < BitI->FileSize; StreamByte++) {
            uint16_t Marker = ReadBits(BitI, 14, true);
            if (Marker == FLACFrameMagic) {
                // Get frame size by reading ahead until you find either the end of the stream, or another FLACFrameMagic
                // then skip back, and read it all.
                // OR could we somehow just read it until we got to the end of the frame, and
                size_t FrameSizeInBits = 0;
                while (ReadBits(BitI, 14, true) != FLACFrameMagic || (BitI->FilePosition + BitI->BitsUnavailable) < BitI->FileSize) {
                    FrameSizeInBits += 14;
                }
                SkipBits(BitI, FrameSizeInBits);
                // Create buffer that's FrameSizeInBits, and then start copying each byte into the buffer
                uint8_t FrameBuffer[1];
                realloc(FrameBuffer, Bits2Bytes(FrameSizeInBits, true));
                for (size_t FrameByte = 0; FrameByte < Bits2Bytes(FrameSizeInBits, true); FrameByte++) {
                    FrameByte[FrameByte] = BitI->Buffer[FrameByte];
                }
            }
        }
        
        return NULL;
    }
     */
    
    /* End User Facing Functions */
    void FLACParseMetadata(BitInput *BitI, DecodeFLAC *Dec) {
        Dec->LastMetadataBlock          = ReadBits(BitI, 1, true);
        uint8_t  MetadataBlockType       = ReadBits(BitI, 7, true);  // 6
        Dec->Meta->MetadataSize         = ReadBits(BitI, 24, true); // 3391 Does NOT count the 2 fields above.
        char Description[BitIOStringSize];
        
        switch (MetadataBlockType) {
            case StreamInfo:
                FLACParseStreamInfo(BitI, Dec);
                break;
            case Padding:
                FLACSkipPadding(BitI, Dec);
                break;
            case Custom:
                FLACSkipCustom(BitI, Dec);
                break;
            case SeekTable:
                FLACParseSeekTable(BitI, Dec);
                break;
            case VorbisComment:
                FLACParseVorbisComment(BitI, Dec);
                break;
            case Cuesheet:
                FLACParseCuesheet(BitI, Dec);
                break;
            case Picture:
                FLACParsePicture(BitI, Dec);
                break;
            default:
                snprintf(Description, BitIOStringSize, "Invalid Metadata Type: %d\n", MetadataBlockType);
                Log(LOG_WARNING, "ModernFLAC", "FLACParseMetadata", Description);
                break;
        }
    }
    
    void FLACParseStreamInfo(BitInput *BitI, DecodeFLAC *Dec) {
        Dec->Meta->StreamInfo->MinimumBlockSize        = ReadBits(BitI, 16, true); // 4096
        Dec->Meta->StreamInfo->MaximumBlockSize        = ReadBits(BitI, 16, true); // 4096
        Dec->Meta->StreamInfo->MinimumFrameSize        = ReadBits(BitI, 24, true); // 752
        Dec->Meta->StreamInfo->MaximumFrameSize        = ReadBits(BitI, 24, true); // 13,594
        Dec->Meta->StreamInfo->SampleRate              = ReadBits(BitI, 20, true); // 44,100
        Dec->Meta->StreamInfo->Channels                = ReadBits(BitI, 3, true) + 1; // stereo
        Dec->Meta->StreamInfo->BitDepth                = ReadBits(BitI, 5, true) + 1; // 16
        Dec->Meta->StreamInfo->SamplesInStream         = ReadBits(BitI, 36, true); // 24,175,621
        for (uint8_t MD5Byte = 0; MD5Byte < 16; MD5Byte++) {
            Dec->Meta->StreamInfo->MD5[MD5Byte] = ReadBits(BitI, 8, true); // MD5 = 0xF296D726DAED094F660800131C52CED4
        }
    }
    
    void FLACSkipPadding(BitInput *BitI, DecodeFLAC *Dec) { // 8192
        SkipBits(BitI, Bytes2Bits(Dec->Meta->MetadataSize));
    }
    
    void FLACSkipCustom(BitInput *BitI, DecodeFLAC *Dec) { // 134,775
        SkipBits(BitI, Bytes2Bits(Dec->Meta->MetadataSize + 1));
    }
    
    void FLACParseSeekTable(BitInput *BitI, DecodeFLAC *Dec) { // 18
        Dec->Meta->Seek->NumSeekPoints = Dec->Meta->MetadataSize / 18; // 21
        
        for (uint16_t SeekPoint = 0; SeekPoint < Dec->Meta->Seek->NumSeekPoints; SeekPoint++) {
            Dec->Meta->Seek->SampleInTargetFrame[SeekPoint]        = ReadBits(BitI, 64, true); // 0
            Dec->Meta->Seek->OffsetFrom1stSample[SeekPoint]        = ReadBits(BitI, 64, true); // 0
            Dec->Meta->Seek->TargetFrameSize[SeekPoint]            = ReadBits(BitI, 16, true); // 16384
        }
    }
    
    void FLACParseVorbisComment(BitInput *BitI, DecodeFLAC *Dec) { // LITTLE ENDIAN
        char Description[BitIOStringSize] = {0};
        Dec->Meta->Vorbis->VendorTagSize = SwapEndian32(ReadBits(BitI, 32, true)); // 32
        Dec->Meta->Vorbis->VendorTag     = calloc(1, Dec->Meta->Vorbis->VendorTagSize);
        for (uint32_t TagByte = 0; TagByte < Dec->Meta->Vorbis->VendorTagSize; TagByte++) {
            Dec->Meta->Vorbis->VendorTag[TagByte]  = ReadBits(BitI, 8, true); // reference libFLAC 1.3.1 20141125
        }
        Dec->Meta->Vorbis->NumUserTags   = SwapEndian32(ReadBits(BitI, 32, true)); // 13
        Dec->Meta->Vorbis->UserTagSize   = calloc(1, Dec->Meta->Vorbis->NumUserTags);
        Dec->Meta->Vorbis->UserTagString = calloc(1, Dec->Meta->Vorbis->NumUserTags);
        for (uint32_t Comment = 0; Comment < Dec->Meta->Vorbis->NumUserTags; Comment++) {
            Dec->Meta->Vorbis->UserTagSize[Comment] = SwapEndian32(ReadBits(BitI, 32, true));
            Dec->Meta->Vorbis->UserTagString[Comment] = calloc(1, Dec->Meta->Vorbis->UserTagSize[Comment]);
            // 39   // ALBUM=My Beautiful Dark Twisted Fantasy
            // 17   // ARTIST=Kanye West
            // 6    // BPM=87
            // 13   // BPM=00000 BPM
            // 12   // DISCNUMBER=1
            // 9    // genre=Rap
            // 98   //iTunNORM=00001B05 00001A8D 0000D69A 0000E522 00026C39 00026C39 00008000 00008000 0000947A 0000947A
            // 13   // TITLE=Runaway
            // 12   // totaldiscs=1
            // 14   // totaltracks=13
            // 3044 // UNSYNCEDLYRICS=[Kanye West] (Look at You [X14]) (Look at You [X8]) (Ladies and Gentlemen, La-Ladies and Gentlemen) (And I Want to Show You How You All Look Like Beautiful Stars Tonight) (And I Want to Show You How You All Look Like Beautiful Stars Tonight) (And I Want to Show You How You All Look Like Beautiful Stars Tonight) And I Always Find, Yeah, I Always Find Somethin' Wrong You Been Puttin' up Wit' My Shit Just Way Too Long I'm So Gifted at Findin' What I Don't Like the Most So I Think It's Time for Us to Have a Toast Let's Have a Toast for the Douchebags, Let's Have a Toast for the Assholes, Let's Have a Toast for the Scumbags, Every One of Them That I Know Let's Have a Toast to the Jerkoffs That'll Never Take Work off Baby, I Got a Plan Run Away Fast As You Can She Find Pictures in My Email I Sent This Girl a Picture of My, Hey! I Don't Know What It Is with Females But I'm Not Too Good with That, Hey! See, I Could Have Me a Good Girl And Still Be Addicted to Them Hoodrats And I Just Blame Everything on You At Least You Know That's What I'm Good at See, I Always Find And I Always Find Yeah, I Always Find Somethin' Wrong You Been Puttin' up with My Shit Just Way Too Long I'm So Gifted at Findin' What I Don't Like the Most So I Think It's Time for Us to Have a Toast Let's Have a Toast for the Douchebags, Let's Have a Toast for the Assholes, Let's Have a Toast for the Scumbags, Every One of Them That I Know Let's Have a Toast to the Jerkoffs That'll Never Take Work off Baby, I Got a Plan Run Away Fast As You Can R-R-Ru-Ru-Ru-Run Away Run Away from Me, Baby (Look at, Look at, Look at, Look at You) Run Away from Me, Baby (Look at You, Look at You, Look at You) Run Away Run Away from Me, Baby [Pusha T] 24/7, 365, Jenny Stays on My Mind I-I-I-I Did It, All Right, All Right, I Admit It Now Pick Your Best Move, You Could Leave or Live Wit' It Ichabod Crane with That Lamborghini Top off Split and Go Where? Back to Wearin' Knockoffs, Huh? Knock It off, Neiman's, Shop It off Let's Talk Over Mai Tais, Waitress, Top It off (Inaudible) Wanna Fly in Your Freddy Loafers You Can't Blame 'em, They Ain't Never Seen Versace Sofas Every Bag, Every Blouse, Every Bracelet Comes with a Price Tag, Baby, Face It You Should Leave If You Can't Accept the Basics Plenty Bitches in the Baller-Player's Matrix Invisibly Set, the Rolex Is Faceless I'm Just Young, Rich, and Tasteless P! [Kanye] Oh I Haven't Fucked in So Long You Been Puttin' up with My Shit Just Way Too Long I'm So Gifted at Findin' What I Don't Like the Most So I Think It's Time for Us to Have a Toast (Ladies and Gentlemen! ) Yeah, I Always Find Somethin' Wrong You Been Puttin' up with My Shit Just Way Too Long I'm So Gifted at Findin' What I Don't Like the Most So I Think It's Time for Us to Have a Toast Let's Have a Toast for the Douchebags, Let's Have a Toast for the Assholes, Let's Have a Toast for the Scumbags, Every One of Them That I Know Let's Have a Toast to the Jerkoffs That'll Never Take Work off Baby, I Got a Plan Run Away Fast As You Can
            
            // 9  // DATE=2010
            // 13 // TRACKNUMBER=9
            
            char UserTagString[Dec->Meta->Vorbis->UserTagSize[Comment]];
            for (uint32_t CommentByte = 0; CommentByte < Dec->Meta->Vorbis->UserTagSize[Comment]; CommentByte++) {
                UserTagString[CommentByte] = ReadBits(BitI, 8, true);
            }
            Dec->Meta->Vorbis->UserTagString[Comment] = UserTagString;
        }
        
        for (uint32_t UserTag = 0; UserTag < Dec->Meta->Vorbis->NumUserTags; UserTag++) {
            snprintf(Description, BitIOStringSize, "User Tag %d of %d: %c\n", UserTag, Dec->Meta->Vorbis->NumUserTags, Dec->Meta->Vorbis->UserTagString[UserTag]);
            //printf("%s", Description);
            Log(LOG_DEBUG, "ModernFLAC", "FLACParseVorbisComment", Description);
        }
    }
    
    void FLACParseCuesheet(BitInput *BitI, DecodeFLAC *Dec) {
        for (uint8_t CatalogChar = 0; CatalogChar < FLACMedizCatalogNumberSize; CatalogChar++) {
            Dec->Meta->Cue->CatalogID[CatalogChar] = ReadBits(BitI, 8, true);
        }
        Dec->Meta->Cue->LeadIn = ReadBits(BitI, 64, true);
        Dec->Meta->Cue->IsCD   = ReadBits(BitI, 1, true);
        SkipBits(BitI, 2071); // Reserved
        Dec->Meta->Cue->NumTracks = ReadBits(BitI, 8, true);
        
        for (uint8_t Track = 0; Track < Dec->Meta->Cue->NumTracks; Track++) {
            Dec->Meta->Cue->TrackOffset[Track]  = ReadBits(BitI, 64, true);
            Dec->Meta->Cue->TrackNum[Track]     = ReadBits(BitI, 64, true);
            
            for (uint8_t ISRCByte = 0; ISRCByte < FLACISRCSize; ISRCByte++) {
                Dec->Meta->Cue->ISRC[Track][ISRCByte]  = ReadBits(BitI, 8, true);
            }
            Dec->Meta->Cue->IsAudio[Track] = ReadBits(BitI, 1, true);
            Dec->Meta->Cue->PreEmphasis[Track] = ReadBits(BitI, 1, true);
            SkipBits(BitI, 152); // Reserved, all 0
            Dec->Meta->Cue->NumTrackIndexPoints[Track] = ReadBits(BitI, 8, true);
        }
        
        Dec->Meta->Cue->IndexOffset    = ReadBits(BitI, 64, true);
        Dec->Meta->Cue->IndexPointNum  = ReadBits(BitI, 8, true);
        SkipBits(BitI, 24); // Reserved
    }
    
    void FLACParsePicture(BitInput *BitI, DecodeFLAC *Dec) { // 17,151
        Dec->Meta->Pic->PicType  = ReadBits(BitI, 32, true); // 3
        Dec->Meta->Pic->MIMESize = ReadBits(BitI, 32, true); // 10
        for (uint32_t MIMEByte = 0; MIMEByte < Dec->Meta->Pic->MIMESize; MIMEByte++) {
            Dec->Meta->Pic->MIMEString[MIMEByte] = ReadBits(BitI, 8, true); // image/jpeg
        }
        Dec->Meta->Pic->PicDescriptionSize = ReadBits(BitI, 32, true); // 0
        for (uint32_t Char = 0; Char < Dec->Meta->Pic->PicDescriptionSize; Char++) {
            Dec->Meta->Pic->PicDescriptionString[Char] = ReadBits(BitI, 8, true);
        }
        Dec->Meta->Pic->Width       = ReadBits(BitI, 32, true); // 1144
        Dec->Meta->Pic->Height      = ReadBits(BitI, 32, true); // 1144
        Dec->Meta->Pic->BitDepth    = ReadBits(BitI, 32, true); // 24, PER PIXEL, NOT SUBPIXEL
        Dec->Meta->Pic->ColorsUsed  = ReadBits(BitI, 32, true); // 0
        Dec->Meta->Pic->PictureSize = ReadBits(BitI, 32, true); // 17,109
                                                           // Pop in the address of the start of the data, and skip over the data instead of buffering it.
    }
    
    void FLACReadStream(BitInput *BitI, DecodeFLAC *Dec) {
        uint16_t Marker = PeekBits(BitI, 14, true);
        if (Marker == FLACFrameMagic) {
            FLACReadFrame(BitI, Dec);
        } else {
            FLACParseMetadata(BitI, Dec);
        }
    }
    
    DecodeFLAC *InitDecodeFLAC(void) {
        DecodeFLAC *Dec       = calloc(1, sizeof(DecodeFLAC));
        Dec->Meta             = calloc(1, sizeof(FLACMeta));
        Dec->Meta->StreamInfo = calloc(1, sizeof(FLACStreamInfo));
        Dec->Meta->Seek       = calloc(1, sizeof(FLACSeekTable));
        Dec->Meta->Vorbis     = calloc(1, sizeof(FLACVorbisComment));
        Dec->Meta->Cue        = calloc(1, sizeof(FLACCueSheet));
        Dec->Meta->Pic        = calloc(1, sizeof(FLACPicture));

        Dec->Data             = calloc(1, sizeof(FLACData));
        Dec->Data->Frame      = calloc(1, sizeof(FLACFrame));
        Dec->Data->SubFrame   = calloc(1, sizeof(FLACSubFrame));
        Dec->Data->LPC        = calloc(1, sizeof(FLACLPC));
        Dec->Data->Rice       = calloc(1, sizeof(RICEPartition));
        return Dec;
    }
    
    void FLACReadFrame(BitInput *BitI, DecodeFLAC *Dec) {
        SkipBits(BitI, 1); // 0
        Dec->Data->Frame->BlockType            = ReadBits(BitI, 1, true); // 0 aka Fixed
        Dec->Data->Frame->CodedSamplesInBlock  = ReadBits(BitI, 4, true); // 12 aka 4096
        if (((Dec->Data->Frame->CodedSamplesInBlock != 6) || (Dec->Data->Frame->CodedSamplesInBlock != 7))) {
            Dec->Data->Frame->BlockSize        = GetBlockSizeInSamples(Dec->Data->Frame->CodedSamplesInBlock); // SamplesInBlock
        }
        Dec->Data->Frame->CodedSampleRate      = ReadBits(BitI, 4, true); // 9 aka 44100
        if ((Dec->Data->Frame->CodedSampleRate >= 0) && (Dec->Data->Frame->CodedSampleRate <= 11)) {
            FLACSampleRate(BitI, Dec);
        }
        Dec->Data->Frame->ChannelLayout        = ReadBits(BitI, 4, true) + 1; // 2
        Dec->Data->Frame->CodedBitDepth        = ReadBits(BitI, 3, true); // 4 aka 16 bits per sample
        if (Dec->Data->Frame->CodedBitDepth != 0) {
            FLACBitDepth(Dec);
        }
        SkipBits(BitI, 1); // 0
        
        if (Dec->Data->Frame->BlockType        == FixedBlockSize) { // variable blocktype
            Dec->Data->Frame->FrameNumber      = ReadBits(BitI, 31, true); // 6,367,232
        } else if (Dec->Data->Frame->BlockType == VariableBlockSize) {
            Dec->Data->Frame->SampleNumber     = ReadBits(BitI, 36, true);
        }
        
        if (Dec->Data->Frame->CodedSamplesInBlock == 6) {
            Dec->Data->Frame->BlockSize        = ReadBits(BitI, 8, true); // SamplesInBlock
        } else if (Dec->Data->Frame->CodedSamplesInBlock == 7) {
            Dec->Data->Frame->BlockSize        = ReadBits(BitI, 16, true); // SamplesInBlock
        }
        
        
        if (Dec->Data->Frame->CodedSampleRate == 12) {
            Dec->Data->Frame->SampleRate       = ReadBits(BitI, 8, true) * 1000;
        } else if (Dec->Data->Frame->CodedSampleRate == 13) {
            Dec->Data->Frame->SampleRate       = ReadBits(BitI, 16, true);
        } else if (Dec->Data->Frame->CodedSampleRate == 14) {
            Dec->Data->Frame->SampleRate       = ReadBits(BitI, 16, true) * 10;
        }
        
        Dec->Data->Frame->FLACFrameCRC         = ReadBits(BitI, 8, true); // CRC, 0x4
        
        for (uint8_t Channel = 0; Channel < Dec->Meta->StreamInfo->Channels; Channel++) { // read SubFrame
            FLACReadSubFrame(BitI, Dec, Channel);
        }
    }
    
    void FLACReadSubFrame(BitInput *BitI, DecodeFLAC *Dec, uint8_t Channel) {
        SkipBits(BitI, 1); // Reserved
        Dec->Data->SubFrame->SubFrameType      = ReadBits(BitI, 6, true); // 127
        if (Dec->Data->SubFrame->SubFrameType > 0) {
            Dec->Data->LPC->LPCOrder = (Dec->Data->SubFrame->SubFrameType & 0x1F) - 1; // 30
        }
        Dec->Data->SubFrame->WastedBitsFlag    = ReadBits(BitI, 1, true); // 1
        if (Dec->Data->SubFrame->WastedBitsFlag == true) {
            Dec->Data->SubFrame->WastedBits    = ReadRICE(BitI, false, 0); // 11111 0 00000
        }
        
        if (Dec->Data->SubFrame->SubFrameType == Subframe_Verbatim) { // PCM
            FLACDecodeSubFrameVerbatim(BitI, Dec);
        } else if (Dec->Data->SubFrame->SubFrameType == Subframe_Constant) {
            FLACDecodeSubFrameConstant(BitI, Dec);
        } else if (Dec->Data->SubFrame->SubFrameType >= Subframe_Fixed && Dec->Data->SubFrame->SubFrameType <= Subframe_LPC) { // Fixed
            FLACDecodeSubFrameFixed(BitI, Dec);
        } else if (Dec->Data->SubFrame->SubFrameType >= Subframe_LPC) { // LPC
            FLACDecodeSubFrameLPC(BitI, Dec, Channel);
        } else {
            char Description[BitIOStringSize];
            snprintf(Description, BitIOStringSize, "Invalid Subframe type: %d", Dec->Data->SubFrame->SubFrameType);
            Log(LOG_ERR, "ModernFLAC", "FLACReadSubframe", Description);
        }
    }
    
    void FLACDecodeSubFrameVerbatim(BitInput *BitI, DecodeFLAC *Dec) {
        for (uint16_t Sample = 0; Sample < Dec->Data->Frame->BlockSize; Sample++) {
            Dec->DecodedSamples[Sample] = ReadBits(BitI, Dec->Data->Frame->BitDepth, true);
        }
    }
    
    void FLACDecodeSubFrameConstant(BitInput *BitI, DecodeFLAC *Dec) {
        int64_t Constant = ReadBits(BitI, Dec->Data->Frame->BitDepth, true);
        memset(Dec->DecodedSamples, Constant, Dec->Data->Frame->BlockSize);
    }
    
    void FLACDecodeSubFrameFixed(BitInput *BitI, DecodeFLAC *Dec) {
        for (uint16_t WarmupSample = 0; WarmupSample < Dec->Data->Frame->BitDepth * Dec->Data->LPC->LPCOrder; WarmupSample++) {
            Dec->DecodedSamples[WarmupSample]  = ReadBits(BitI, Dec->Data->Frame->BitDepth, true);
        }
        DecodeFLACesidual(BitI, Dec);
    }
    
    void FLACDecodeSubFrameLPC(BitInput *BitI, DecodeFLAC *Dec, uint8_t Channel) { // 4 0's
        for (uint16_t WarmupSample = 0; WarmupSample < Dec->Data->Frame->BitDepth * Dec->Data->LPC->LPCOrder; WarmupSample++) { // 16 * 30= 480 aka 960 bytes
            // 004F, FFB0, 004F, 
            Dec->DecodedSamples[WarmupSample]  = ReadBits(BitI, Dec->Data->Frame->BitDepth, true);
        }
        
        Dec->Data->LPC->LPCPrecision           = ReadBits(BitI, 4, true) + 1; // 0x1F aka 31
        Dec->Data->LPC->LPCShift               = ReadBits(BitI, 5, true); // 0b01110 aka 14
        Dec->Data->LPC->NumLPCCoeffs           = Dec->Data->LPC->LPCPrecision * Dec->Data->LPC->LPCOrder;
        
        
        for (uint16_t LPCCoefficent = 0; LPCCoefficent < Dec->Data->LPC->NumLPCCoeffs; LPCCoefficent++) {
            Dec->Data->LPC->LPCCoeff[LPCCoefficent] = ReadBits(BitI, Dec->Data->LPC->NumLPCCoeffs, true) + 1;
        }
        DecodeFLACesidual(BitI, Dec);
    }
    
    void DecodeFLACesidual(BitInput *BitI, DecodeFLAC *Dec) {
        Dec->Data->LPC->RicePartitionType      = ReadBits(BitI, 2, true);
        if (Dec->Data->LPC->RicePartitionType == RICE1) {
            DecodeFLACice1Partition(BitI, Dec);
        } else if (Dec->Data->LPC->RicePartitionType == RICE2) {
            DecodeFLACice2Partition(BitI, Dec);
        }
    }
    
    void DecodeFLACice1Partition(BitInput *BitI, DecodeFLAC *Dec) {
        Dec->Data->LPC->PartitionOrder = ReadBits(BitI, 4, true);
        for (uint8_t Partition = 0; Partition < Dec->Data->LPC->PartitionOrder; Partition++) {
            Dec->Data->Rice->RICEParameter[Partition] = ReadBits(BitI, 4, true) + 5;
            if (Dec->Data->Rice->RICEParameter[Partition] == 20) {
                // Escape code, meaning the partition is in unencoded binary form using n bits per sample; n follows as a 5-bit number.
            } else {
                if (Dec->Data->LPC->PartitionOrder == 0) {
                    Dec->Data->Rice->RICEParameter[Partition] = ReadBits(BitI, Dec->Data->Frame->BlockSize - Dec->Data->LPC->LPCOrder, true);
                } else if (Dec->Data->LPC->PartitionOrder > 0) {
                    Dec->Data->Rice->RICEParameter[Partition] = ReadBits(BitI, (Dec->Data->Frame->BlockSize / pow(2, Dec->Data->LPC->PartitionOrder)), true);
                } else {
                    Dec->Data->Rice->RICEParameter[Partition] = ReadBits(BitI, (Dec->Data->Frame->BlockSize / pow(2, Dec->Data->LPC->PartitionOrder)) - Dec->Data->LPC->LPCOrder, true);
                }
            }
        }
    }
    
    void DecodeFLACice2Partition(BitInput *BitI, DecodeFLAC *Dec) {
        for (uint8_t Partition = 0; Partition < Dec->Data->LPC->PartitionOrder; Partition++) {
            Dec->Data->Rice->RICEParameter[Partition] = ReadBits(BitI, 5, true) + 5;
            if (Dec->Data->Rice->RICEParameter[Partition] == 36) {
                // Escape code, meaning the partition is in unencoded binary form using n bits per sample; n follows as a 5-bit number.
            } else {
                if (Dec->Data->LPC->PartitionOrder == 0) {
                    Dec->Data->Rice->RICEParameter[Partition] = ReadBits(BitI, Dec->Data->Frame->BlockSize - Dec->Data->LPC->LPCOrder, true);
                } else if (Dec->Data->LPC->PartitionOrder > 0) {
                    Dec->Data->Rice->RICEParameter[Partition] = ReadBits(BitI, (Dec->Data->Frame->BlockSize / pow(2, Dec->Data->LPC->PartitionOrder)), true);
                } else {
                    Dec->Data->Rice->RICEParameter[Partition] = ReadBits(BitI, (Dec->Data->Frame->BlockSize / pow(2, Dec->Data->LPC->PartitionOrder)) - Dec->Data->LPC->LPCOrder, true);
                }
            }
        }
    }
    
    uint8_t GetBlockSizeInSamples(uint8_t BlockSize) {
        uint16_t SamplesInBlock = 0;
        if (BlockSize == 1) {
            SamplesInBlock = 192;
        } else if ((BlockSize >= 2) && (BlockSize <= 5)) {
            SamplesInBlock = (576 * pow(2, BlockSize - 2)); // 576/1152/2304/4608, pow(2, (BlockSize - 2))
        } else if (BlockSize == 6) {
            // get 8 bit block from the end of the header
        } else if (BlockSize == 7) {
            // get 16 bit block from the end of the header
        } else if ((BlockSize >= 8) && (BlockSize <= 15)) {
            SamplesInBlock = (256 * pow(2, (BlockSize - 8))); // 256/512/1024/2048/4096/8192/16384/32768
        } else {
            // Reserved
        }
        
        return SamplesInBlock;
    }
    
    void FLACBitDepth(DecodeFLAC *Dec) {
        switch (Dec->Meta->StreamInfo->CodedBitDepth) {
            case 0:
                Dec->Data->Frame->BitDepth = Dec->Meta->StreamInfo->BitDepth;
                break;
            case 1:
                Dec->Data->Frame->BitDepth = 8;
                break;
            case 2:
                Dec->Data->Frame->BitDepth = 12;
                break;
            case 4:
                Dec->Data->Frame->BitDepth = 16;
                break;
            case 5:
                Dec->Data->Frame->BitDepth = 20;
                break;
            case 6:
                Dec->Data->Frame->BitDepth = 24;
                break;
            default:
                Dec->Data->Frame->BitDepth = 0;
                break;
        }
    }
    
    void FLACSampleRate(BitInput *BitI, DecodeFLAC *Dec) {
        switch (Dec->Meta->StreamInfo->CodedSampleRate) {
            case 0:
                Dec->Data->Frame->SampleRate = Dec->Meta->StreamInfo->SampleRate;
                break;
            case 1:
                Dec->Data->Frame->SampleRate = 88200;
                break;
            case 2:
                Dec->Data->Frame->SampleRate = 176400;
                break;
            case 3:
                Dec->Data->Frame->SampleRate = 192000;
                break;
            case 4:
                Dec->Data->Frame->SampleRate = 8000;
                break;
            case 5:
                Dec->Data->Frame->SampleRate = 16000;
                break;
            case 6:
                Dec->Data->Frame->SampleRate = 22050;
                break;
            case 7:
                Dec->Data->Frame->SampleRate = 24000;
                break;
            case 8:
                Dec->Data->Frame->SampleRate = 32000;
                break;
            case 9:
                Dec->Data->Frame->SampleRate = 44100;
                break;
            case 10:
                Dec->Data->Frame->SampleRate = 48000;
                break;
            case 11:
                Dec->Data->Frame->SampleRate = 96000;
                break;
            default:
                break;
        }
    }
    
    void FLACDecodeLPC(BitInput *BitI, DecodeFLAC *Dec) {
        // Basically you use the warmup samples in Dec->DecodedSamples, along with the info in Dec->LPC to deocde the file by using summation.
        // LPC is lossy, which is why you use filters to reduce the size of the residual.
        
        
        
        
        // I need 2 loops, one for the warmup samples, and one for the LPC encoded samples.
        
        
        // Original algorithm: X^
    }
    
#ifdef __cplusplus
}
#endif
