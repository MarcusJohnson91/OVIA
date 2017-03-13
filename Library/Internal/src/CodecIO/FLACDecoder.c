#include "/usr/local/Packages/libBitIO/include/BitIO.h"
#include "../include/DecodeFLAC.h"

#ifdef __cplusplus
extern "C" {
#endif
    /* Start User facing functions */
    
    /*!
     @abstract          Copies frames from the stream pointed to by BitI, to OutputFrameBuffer (which needs to be freed by you)
     @param     StartFrame IS NOT zero indexed.
     */
    /*
    uint8_t *CopyFLACFrame(BitInput *BitI, FLACDecoder *FLAC) { // for apps that don't care about metadata
        
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
    void FLACParseMetadata(BitInput *BitI, FLACDecoder *FLAC) {
        FLAC->LastMetadataBlock          = ReadBits(BitI, 1, true);
        uint8_t  MetadataBlockType       = ReadBits(BitI, 7, true);  // 6
        FLAC->Meta->MetadataSize         = ReadBits(BitI, 24, true); // 3391 Does NOT count the 2 fields above.
        char Description[BitIOStringSize];
        
        switch (MetadataBlockType) {
            case StreamInfo:
                FLACParseStreamInfo(BitI, FLAC);
                break;
            case Padding:
                FLACSkipPadding(BitI, FLAC);
                break;
            case Custom:
                FLACSkipCustom(BitI, FLAC);
                break;
            case SeekTable:
                FLACParseSeekTable(BitI, FLAC);
                break;
            case VorbisComment:
                FLACParseVorbisComment(BitI, FLAC);
                break;
            case Cuesheet:
                FLACParseCuesheet(BitI, FLAC);
                break;
            case Picture:
                FLACParsePicture(BitI, FLAC);
                break;
            default:
                snprintf(Description, BitIOStringSize, "Invalid Metadata Type: %d\n", MetadataBlockType);
                Log(LOG_WARNING, "ModernFLAC", "FLACParseMetadata", Description);
                break;
        }
    }
    
    void FLACParseStreamInfo(BitInput *BitI, FLACDecoder *FLAC) {
        FLAC->Meta->StreamInfo->MinimumBlockSize        = ReadBits(BitI, 16, true); // 4096
        FLAC->Meta->StreamInfo->MaximumBlockSize        = ReadBits(BitI, 16, true); // 4096
        FLAC->Meta->StreamInfo->MinimumFrameSize        = ReadBits(BitI, 24, true); // 752
        FLAC->Meta->StreamInfo->MaximumFrameSize        = ReadBits(BitI, 24, true); // 13,594
        FLAC->Meta->StreamInfo->SampleRate              = ReadBits(BitI, 20, true); // 44,100
        FLAC->Meta->StreamInfo->Channels                = ReadBits(BitI, 3, true) + 1; // stereo
        FLAC->Meta->StreamInfo->BitDepth                = ReadBits(BitI, 5, true) + 1; // 16
        FLAC->Meta->StreamInfo->SamplesInStream         = ReadBits(BitI, 36, true); // 24,175,621
        for (uint8_t MD5Byte = 0; MD5Byte < 16; MD5Byte++) {
            FLAC->Meta->StreamInfo->MD5[MD5Byte] = ReadBits(BitI, 8, true); // MD5 = 0xF296D726DAED094F660800131C52CED4
        }
    }
    
    void FLACSkipPadding(BitInput *BitI, FLACDecoder *FLAC) { // 8192
        SkipBits(BitI, Bytes2Bits(FLAC->Meta->MetadataSize));
    }
    
    void FLACSkipCustom(BitInput *BitI, FLACDecoder *FLAC) { // 134,775
        SkipBits(BitI, Bytes2Bits(FLAC->Meta->MetadataSize + 1));
    }
    
    void FLACParseSeekTable(BitInput *BitI, FLACDecoder *FLAC) { // 18
        FLAC->Meta->Seek->NumSeekPoints = FLAC->Meta->MetadataSize / 18; // 21
        
        for (uint16_t SeekPoint = 0; SeekPoint < FLAC->Meta->Seek->NumSeekPoints; SeekPoint++) {
            FLAC->Meta->Seek->SampleInTargetFrame[SeekPoint]        = ReadBits(BitI, 64, true); // 0
            FLAC->Meta->Seek->OffsetFrom1stSample[SeekPoint]        = ReadBits(BitI, 64, true); // 0
            FLAC->Meta->Seek->TargetFrameSize[SeekPoint]            = ReadBits(BitI, 16, true); // 16384
        }
    }
    
    void FLACParseVorbisComment(BitInput *BitI, FLACDecoder *FLAC) { // LITTLE ENDIAN
        char Description[BitIOStringSize] = {0};
        FLAC->Meta->Vorbis->VendorTagSize = SwapEndian32(ReadBits(BitI, 32, true)); // 32
        FLAC->Meta->Vorbis->VendorTag     = calloc(FLAC->Meta->Vorbis->VendorTagSize, 1);
        for (uint32_t TagByte = 0; TagByte < FLAC->Meta->Vorbis->VendorTagSize; TagByte++) {
            FLAC->Meta->Vorbis->VendorTag[TagByte]  = ReadBits(BitI, 8, true); // reference libFLAC 1.3.1 20141125
        }
        FLAC->Meta->Vorbis->NumUserTags   = SwapEndian32(ReadBits(BitI, 32, true)); // 13
        FLAC->Meta->Vorbis->UserTagSize   = calloc(FLAC->Meta->Vorbis->NumUserTags, 1);
        FLAC->Meta->Vorbis->UserTagString = calloc(FLAC->Meta->Vorbis->NumUserTags, 1);
        for (uint32_t Comment = 0; Comment < FLAC->Meta->Vorbis->NumUserTags; Comment++) {
            FLAC->Meta->Vorbis->UserTagSize[Comment] = SwapEndian32(ReadBits(BitI, 32, true));
            FLAC->Meta->Vorbis->UserTagString[Comment] = calloc(FLAC->Meta->Vorbis->UserTagSize[Comment], 1);
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
            
            char UserTagString[FLAC->Meta->Vorbis->UserTagSize[Comment]];
            for (uint32_t CommentByte = 0; CommentByte < FLAC->Meta->Vorbis->UserTagSize[Comment]; CommentByte++) {
                UserTagString[CommentByte] = ReadBits(BitI, 8, true);
            }
            FLAC->Meta->Vorbis->UserTagString[Comment] = UserTagString;
        }
        
        for (uint32_t UserTag = 0; UserTag < FLAC->Meta->Vorbis->NumUserTags; UserTag++) {
            snprintf(Description, BitIOStringSize, "User Tag %d of %d: %c\n", UserTag, FLAC->Meta->Vorbis->NumUserTags, FLAC->Meta->Vorbis->UserTagString[UserTag]);
            //printf("%s", Description);
            Log(LOG_DEBUG, "ModernFLAC", "FLACParseVorbisComment", Description);
        }
    }
    
    void FLACParseCuesheet(BitInput *BitI, FLACDecoder *FLAC) {
        for (uint8_t CatalogChar = 0; CatalogChar < FLACMedizCatalogNumberSize; CatalogChar++) {
            FLAC->Meta->Cue->CatalogID[CatalogChar] = ReadBits(BitI, 8, true);
        }
        FLAC->Meta->Cue->LeadIn = ReadBits(BitI, 64, true);
        FLAC->Meta->Cue->IsCD   = ReadBits(BitI, 1, true);
        SkipBits(BitI, 2071); // Reserved
        FLAC->Meta->Cue->NumTracks = ReadBits(BitI, 8, true);
        
        for (uint8_t Track = 0; Track < FLAC->Meta->Cue->NumTracks; Track++) {
            FLAC->Meta->Cue->TrackOffset[Track]  = ReadBits(BitI, 64, true);
            FLAC->Meta->Cue->TrackNum[Track]     = ReadBits(BitI, 64, true);
            
            for (uint8_t ISRCByte = 0; ISRCByte < FLACISRCSize; ISRCByte++) {
                FLAC->Meta->Cue->ISRC[Track][ISRCByte]  = ReadBits(BitI, 8, true);
            }
            FLAC->Meta->Cue->IsAudio[Track] = ReadBits(BitI, 1, true);
            FLAC->Meta->Cue->PreEmphasis[Track] = ReadBits(BitI, 1, true);
            SkipBits(BitI, 152); // Reserved, all 0
            FLAC->Meta->Cue->NumTrackIndexPoints[Track] = ReadBits(BitI, 8, true);
        }
        
        FLAC->Meta->Cue->IndexOffset    = ReadBits(BitI, 64, true);
        FLAC->Meta->Cue->IndexPointNum  = ReadBits(BitI, 8, true);
        SkipBits(BitI, 24); // Reserved
    }
    
    void FLACParsePicture(BitInput *BitI, FLACDecoder *FLAC) { // 17,151
        FLAC->Meta->Pic->PicType  = ReadBits(BitI, 32, true); // 3
        FLAC->Meta->Pic->MIMESize = ReadBits(BitI, 32, true); // 10
        for (uint32_t MIMEByte = 0; MIMEByte < FLAC->Meta->Pic->MIMESize; MIMEByte++) {
            FLAC->Meta->Pic->MIMEString[MIMEByte] = ReadBits(BitI, 8, true); // image/jpeg
        }
        FLAC->Meta->Pic->PicDescriptionSize = ReadBits(BitI, 32, true); // 0
        for (uint32_t Char = 0; Char < FLAC->Meta->Pic->PicDescriptionSize; Char++) {
            FLAC->Meta->Pic->PicDescriptionString[Char] = ReadBits(BitI, 8, true);
        }
        FLAC->Meta->Pic->Width       = ReadBits(BitI, 32, true); // 1144
        FLAC->Meta->Pic->Height      = ReadBits(BitI, 32, true); // 1144
        FLAC->Meta->Pic->BitDepth    = ReadBits(BitI, 32, true); // 24, PER PIXEL, NOT SUBPIXEL
        FLAC->Meta->Pic->ColorsUsed  = ReadBits(BitI, 32, true); // 0
        FLAC->Meta->Pic->PictureSize = ReadBits(BitI, 32, true); // 17,109
                                                           // Pop in the address of the start of the data, and skip over the data instead of buffering it.
    }
    
    void FLACReadStream(BitInput *BitI, FLACDecoder *FLAC) {
        uint16_t Marker = PeekBits(BitI, 14, true);
        if (Marker == FLACFrameMagic) {
            FLACReadFrame(BitI, FLAC);
        } else {
            FLACParseMetadata(BitI, FLAC);
        }
    }
    
    void InitFLACDecoder(FLACDecoder *FLAC) {
        FLAC->Meta                           = calloc(sizeof(FLACMeta), 1);
        FLAC->Meta->StreamInfo               = calloc(sizeof(FLACStreamInfo), 1);
        FLAC->Meta->Seek                     = calloc(sizeof(FLACSeekTable), 1);
        FLAC->Meta->Vorbis                   = calloc(sizeof(FLACVorbisComment), 1);
        FLAC->Meta->Cue                      = calloc(sizeof(FLACCueSheet), 1);
        FLAC->Meta->Pic                      = calloc(sizeof(FLACPicture), 1);
        
        FLAC->Data                           = calloc(sizeof(FLACData), 1);
        FLAC->Data->Frame                    = calloc(sizeof(FLACFrame), 1);
        FLAC->Data->SubFrame                 = calloc(sizeof(FLACSubFrame), 1);
        FLAC->Data->LPC                      = calloc(sizeof(FLACLPC), 1);
        FLAC->Data->Rice                     = calloc(sizeof(RICEPartition), 1);
    }
    
    void FLACReadFrame(BitInput *BitI, FLACDecoder *FLAC) {
        SkipBits(BitI, 1); // 0
        FLAC->Data->Frame->BlockType            = ReadBits(BitI, 1, true); // 0 aka Fixed
        FLAC->Data->Frame->CodedSamplesInBlock  = ReadBits(BitI, 4, true); // 12 aka 4096
        if (((FLAC->Data->Frame->CodedSamplesInBlock != 6) || (FLAC->Data->Frame->CodedSamplesInBlock != 7))) {
            FLAC->Data->Frame->BlockSize        = GetBlockSizeInSamples(FLAC->Data->Frame->CodedSamplesInBlock); // SamplesInBlock
        }
        FLAC->Data->Frame->CodedSampleRate      = ReadBits(BitI, 4, true); // 9 aka 44100
        if ((FLAC->Data->Frame->CodedSampleRate >= 0) && (FLAC->Data->Frame->CodedSampleRate <= 11)) {
            FLACSampleRate(BitI, FLAC);
        }
        FLAC->Data->Frame->ChannelLayout        = ReadBits(BitI, 4, true) + 1; // 2
        FLAC->Data->Frame->CodedBitDepth        = ReadBits(BitI, 3, true); // 4 aka 16 bits per sample
        if (FLAC->Data->Frame->CodedBitDepth != 0) {
            FLACBitDepth(FLAC);
        }
        SkipBits(BitI, 1); // 0
        
        if (FLAC->Data->Frame->BlockType        == FixedBlockSize) { // variable blocktype
            FLAC->Data->Frame->FrameNumber      = ReadBits(BitI, 31, true); // 6,367,232
        } else if (FLAC->Data->Frame->BlockType == VariableBlockSize) {
            FLAC->Data->Frame->SampleNumber     = ReadBits(BitI, 36, true);
        }
        
        if (FLAC->Data->Frame->CodedSamplesInBlock == 6) {
            FLAC->Data->Frame->BlockSize        = ReadBits(BitI, 8, true); // SamplesInBlock
        } else if (FLAC->Data->Frame->CodedSamplesInBlock == 7) {
            FLAC->Data->Frame->BlockSize        = ReadBits(BitI, 16, true); // SamplesInBlock
        }
        
        
        if (FLAC->Data->Frame->CodedSampleRate == 12) {
            FLAC->Data->Frame->SampleRate       = ReadBits(BitI, 8, true) * 1000;
        } else if (FLAC->Data->Frame->CodedSampleRate == 13) {
            FLAC->Data->Frame->SampleRate       = ReadBits(BitI, 16, true);
        } else if (FLAC->Data->Frame->CodedSampleRate == 14) {
            FLAC->Data->Frame->SampleRate       = ReadBits(BitI, 16, true) * 10;
        }
        
        FLAC->Data->Frame->FLACFrameCRC         = ReadBits(BitI, 8, true); // CRC, 0x4
        
        for (uint8_t Channel = 0; Channel < FLAC->Meta->StreamInfo->Channels; Channel++) { // read SubFrame
            FLACReadSubFrame(BitI, FLAC, Channel);
        }
    }
    
    void FLACReadSubFrame(BitInput *BitI, FLACDecoder *FLAC, uint8_t Channel) {
        SkipBits(BitI, 1); // Reserved
        FLAC->Data->SubFrame->SubFrameType      = ReadBits(BitI, 6, true); // 127
        if (FLAC->Data->SubFrame->SubFrameType > 0) {
            FLAC->Data->LPC->LPCOrder = (FLAC->Data->SubFrame->SubFrameType & 0x1F) - 1; // 30
        }
        FLAC->Data->SubFrame->WastedBitsFlag    = ReadBits(BitI, 1, true); // 1
        if (FLAC->Data->SubFrame->WastedBitsFlag == true) {
            FLAC->Data->SubFrame->WastedBits    = ReadRICE(BitI, false, 0); // 11111 0 00000
        }
        
        if (FLAC->Data->SubFrame->SubFrameType == Subframe_Verbatim) { // PCM
            FLACDecodeSubFrameVerbatim(BitI, FLAC);
        } else if (FLAC->Data->SubFrame->SubFrameType == Subframe_Constant) {
            FLACDecodeSubFrameConstant(BitI, FLAC);
        } else if (FLAC->Data->SubFrame->SubFrameType >= Subframe_Fixed && FLAC->Data->SubFrame->SubFrameType <= Subframe_LPC) { // Fixed
            FLACDecodeSubFrameFixed(BitI, FLAC);
        } else if (FLAC->Data->SubFrame->SubFrameType >= Subframe_LPC) { // LPC
            FLACDecodeSubFrameLPC(BitI, FLAC, Channel);
        } else {
            char Description[BitIOStringSize];
            snprintf(Description, BitIOStringSize, "Invalid Subframe type: %d", FLAC->Data->SubFrame->SubFrameType);
            Log(LOG_ERR, "ModernFLAC", "FLACReadSubframe", Description);
        }
    }
    
    void FLACDecodeSubFrameVerbatim(BitInput *BitI, FLACDecoder *FLAC) {
        for (uint16_t Sample = 0; Sample < FLAC->Data->Frame->BlockSize; Sample++) {
            FLAC->DecodedSamples[Sample] = ReadBits(BitI, FLAC->Data->Frame->BitDepth, true);
        }
    }
    
    void FLACDecodeSubFrameConstant(BitInput *BitI, FLACDecoder *FLAC) {
        int64_t Constant = ReadBits(BitI, FLAC->Data->Frame->BitDepth, true);
        memset(FLAC->DecodedSamples, Constant, FLAC->Data->Frame->BlockSize);
    }
    
    void FLACDecodeSubFrameFixed(BitInput *BitI, FLACDecoder *FLAC) {
        for (uint16_t WarmupSample = 0; WarmupSample < FLAC->Data->Frame->BitDepth * FLAC->Data->LPC->LPCOrder; WarmupSample++) {
            FLAC->DecodedSamples[WarmupSample]  = ReadBits(BitI, FLAC->Data->Frame->BitDepth, true);
        }
        FLACDecodeResidual(BitI, FLAC);
    }
    
    void FLACDecodeSubFrameLPC(BitInput *BitI, FLACDecoder *FLAC, uint8_t Channel) { // 4 0's
        for (uint16_t WarmupSample = 0; WarmupSample < FLAC->Data->Frame->BitDepth * FLAC->Data->LPC->LPCOrder; WarmupSample++) { // 16 * 30= 480 aka 960 bytes
            // 004F, FFB0, 004F, 
            FLAC->DecodedSamples[WarmupSample]  = ReadBits(BitI, FLAC->Data->Frame->BitDepth, true);
        }
        
        FLAC->Data->LPC->LPCPrecision           = ReadBits(BitI, 4, true) + 1; // 0x1F aka 31
        FLAC->Data->LPC->LPCShift               = ReadBits(BitI, 5, true); // 0b01110 aka 14
        FLAC->Data->LPC->NumLPCCoeffs           = FLAC->Data->LPC->LPCPrecision * FLAC->Data->LPC->LPCOrder;
        
        
        for (uint16_t LPCCoefficent = 0; LPCCoefficent < FLAC->Data->LPC->NumLPCCoeffs; LPCCoefficent++) {
            FLAC->Data->LPC->LPCCoeff[LPCCoefficent] = ReadBits(BitI, FLAC->Data->LPC->NumLPCCoeffs, true) + 1;
        }
        FLACDecodeResidual(BitI, FLAC);
    }
    
    void FLACDecodeResidual(BitInput *BitI, FLACDecoder *FLAC) {
        FLAC->Data->LPC->RicePartitionType      = ReadBits(BitI, 2, true);
        if (FLAC->Data->LPC->RicePartitionType == RICE1) {
            FLACDecodeRice1Partition(BitI, FLAC);
        } else if (FLAC->Data->LPC->RicePartitionType == RICE2) {
            FLACDecodeRice2Partition(BitI, FLAC);
        }
    }
    
    void FLACDecodeRice1Partition(BitInput *BitI, FLACDecoder *FLAC) {
        FLAC->Data->LPC->PartitionOrder = ReadBits(BitI, 4, true);
        for (uint8_t Partition = 0; Partition < FLAC->Data->LPC->PartitionOrder; Partition++) {
            FLAC->Data->Rice->RICEParameter[Partition] = ReadBits(BitI, 4, true) + 5;
            if (FLAC->Data->Rice->RICEParameter[Partition] == 20) {
                // Escape code, meaning the partition is in unencoded binary form using n bits per sample; n follows as a 5-bit number.
            } else {
                if (FLAC->Data->LPC->PartitionOrder == 0) {
                    FLAC->Data->Rice->RICEParameter[Partition] = ReadBits(BitI, FLAC->Data->Frame->BlockSize - FLAC->Data->LPC->LPCOrder, true);
                } else if (FLAC->Data->LPC->PartitionOrder > 0) {
                    FLAC->Data->Rice->RICEParameter[Partition] = ReadBits(BitI, (FLAC->Data->Frame->BlockSize / pow(2, FLAC->Data->LPC->PartitionOrder)), true);
                } else {
                    FLAC->Data->Rice->RICEParameter[Partition] = ReadBits(BitI, (FLAC->Data->Frame->BlockSize / pow(2, FLAC->Data->LPC->PartitionOrder)) - FLAC->Data->LPC->LPCOrder, true);
                }
            }
        }
    }
    
    void FLACDecodeRice2Partition(BitInput *BitI, FLACDecoder *FLAC) {
        for (uint8_t Partition = 0; Partition < FLAC->Data->LPC->PartitionOrder; Partition++) {
            FLAC->Data->Rice->RICEParameter[Partition] = ReadBits(BitI, 5, true) + 5;
            if (FLAC->Data->Rice->RICEParameter[Partition] == 36) {
                // Escape code, meaning the partition is in unencoded binary form using n bits per sample; n follows as a 5-bit number.
            } else {
                if (FLAC->Data->LPC->PartitionOrder == 0) {
                    FLAC->Data->Rice->RICEParameter[Partition] = ReadBits(BitI, FLAC->Data->Frame->BlockSize - FLAC->Data->LPC->LPCOrder, true);
                } else if (FLAC->Data->LPC->PartitionOrder > 0) {
                    FLAC->Data->Rice->RICEParameter[Partition] = ReadBits(BitI, (FLAC->Data->Frame->BlockSize / pow(2, FLAC->Data->LPC->PartitionOrder)), true);
                } else {
                    FLAC->Data->Rice->RICEParameter[Partition] = ReadBits(BitI, (FLAC->Data->Frame->BlockSize / pow(2, FLAC->Data->LPC->PartitionOrder)) - FLAC->Data->LPC->LPCOrder, true);
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
    
    void FLACBitDepth(FLACDecoder *FLAC) {
        switch (FLAC->Meta->StreamInfo->CodedBitDepth) {
            case 0:
                FLAC->Data->Frame->BitDepth = FLAC->Meta->StreamInfo->BitDepth;
                break;
            case 1:
                FLAC->Data->Frame->BitDepth = 8;
                break;
            case 2:
                FLAC->Data->Frame->BitDepth = 12;
                break;
            case 4:
                FLAC->Data->Frame->BitDepth = 16;
                break;
            case 5:
                FLAC->Data->Frame->BitDepth = 20;
                break;
            case 6:
                FLAC->Data->Frame->BitDepth = 24;
                break;
            default:
                FLAC->Data->Frame->BitDepth = 0;
                break;
        }
    }
    
    void FLACSampleRate(BitInput *BitI, FLACDecoder *FLAC) {
        switch (FLAC->Meta->StreamInfo->CodedSampleRate) {
            case 0:
                FLAC->Data->Frame->SampleRate = FLAC->Meta->StreamInfo->SampleRate;
                break;
            case 1:
                FLAC->Data->Frame->SampleRate = 88200;
                break;
            case 2:
                FLAC->Data->Frame->SampleRate = 176400;
                break;
            case 3:
                FLAC->Data->Frame->SampleRate = 192000;
                break;
            case 4:
                FLAC->Data->Frame->SampleRate = 8000;
                break;
            case 5:
                FLAC->Data->Frame->SampleRate = 16000;
                break;
            case 6:
                FLAC->Data->Frame->SampleRate = 22050;
                break;
            case 7:
                FLAC->Data->Frame->SampleRate = 24000;
                break;
            case 8:
                FLAC->Data->Frame->SampleRate = 32000;
                break;
            case 9:
                FLAC->Data->Frame->SampleRate = 44100;
                break;
            case 10:
                FLAC->Data->Frame->SampleRate = 48000;
                break;
            case 11:
                FLAC->Data->Frame->SampleRate = 96000;
                break;
            default:
                break;
        }
    }
    
    void FLACDecodeLPC(BitInput BitI, FLACDecoder *FLAC) {
        // Basically you use the warmup samples in FLAC->DecodedSamples, along with the info in FLAC->LPC to deocde the file by using summation.
        // LPC is lossy, which is why you use filters to reduce the size of the residual.
        
        
        
        
        // I need 2 loops, one for the warmup samples, and one for the LPC encoded samples.
        
        
        // Original algorithm: X^
    }
    
#ifdef __cplusplus
}
#endif
