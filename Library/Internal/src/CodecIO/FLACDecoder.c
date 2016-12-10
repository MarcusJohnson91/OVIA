#include "../include/DecodeFLAC.h"

// When decoding FLAC, give the caller the option to skip extracting the images, to help embedded devices reduce memory requirements.

// I want 2 ways to handle this, the first is to just decode the whole file as quick as possible, and the second is for real time applications, which decodes all metadata, and a single frame.

// for the 2nd use case, I need to make a few functions, one that reads all metadata, and another that reads a frame of audio at a time.
// The fast as possible method will just loop through all the blocks

void FLACReadVorbisComment(BitInput *BitI, FLACFile *FLAC) { // LITTLE ENDIAN
    uint32_t UserCommentSize[FLACMaxVorbisComments] = {0};

    uint32_t VendorTagSize = SwapEndian32(ReadBits(BitI, 32));
    for (uint32_t TagByte = 0; TagByte < VendorTagSize; TagByte++) {
        FLAC->Vorbis->VendorString[TagByte]  = ReadBits(BitI, 8); // reference libFLAC 1.3.1 20141125
    }
    
    uint32_t NumUserComments = SwapEndian32(ReadBits(BitI, 32)); // 28
    for (uint32_t UserComment = 0; UserComment < NumUserComments; UserComment++) {
        UserCommentSize[UserComment] = SwapEndian32(ReadBits(BitI, 32)); // 28
        for (uint32_t TagByte = 0; TagByte < UserCommentSize[UserComment]; TagByte++) {
            FLAC->Vorbis->UserCommentString[UserComment][TagByte] = ReadBits(BitI, 8);
            // LC=BIS-SACD-1536
            // DISC=1/1
            // ITUNESCOMPILATION=0
            // PERFORMER=Sampson, Carolyn
            // PERFORMER=Cummings, Laurence
            // PERFORMER=Kenny, Elizabeth
            // PERFORMER=Lasla, Anne-Marie
            // MUSICIAN_CREDITS_LIST=ComposerPurcell, Henry
            // MUSICIAN_CREDITS_LIST=SopranoSampson, Carolyn
            // MUSICIAN_CREDITS_LIST=Harpsichord / SpinetCummings, Laurence
            // MUSICIAN_CREDITS_LIST=Archlute / TheorboKenny, Elizabeth
            // MUSICIAN_CREDITS_LIST=Bass ViolLasla, Anne-Marie
            // TAGGING_TIME=2010-12-16T12:41:28+01:00
            // COPYRIGHT=eClassical
            // Album=Victorious Love - Carolyn Sampson sings Purcell
            // Artist=Sampson, Carolyn
            // Artist=Cummings, Laurence
            // Artist=Kenny, Elizabeth
            // Artist=Lasla, Anne-Marie
            // Comment=Downloaded from eClassical.com. From album BIS-SACD-1536
            // Genre=(32)Vocal/Choral
            // Genre=(32)Baroque
            // Title=(Sweeter than roses, Z.585 No.1) - Sweeter than roses, Z.585 No.1
            // COMPOSER=Purcell, Henry
            // DATE=2007-08-30
            // ORGANIZATION=BIS
            // ORGANIZATION=eClassical
            // TRACKNUMBER=1/19
            // 28 comments
        }
    }
}

void FLACReadStreamInfo(BitInput *BitI, FLACFile *FLAC) { // size = 34
    FLAC->StreamInfo->MinimumBlockSize        = ReadBits(BitI, 16);    // 4096
    FLAC->StreamInfo->MaximumBlockSize        = ReadBits(BitI, 16);    // 4096
    FLAC->StreamInfo->MinimumFrameSize        = ReadBits(BitI, 24);    // 16
    FLAC->StreamInfo->MaximumFrameSize        = ReadBits(BitI, 24);    // 18,693
    FLAC->StreamInfo->SampleRate              = ReadBits(BitI, 20);    // 44,100
    FLAC->StreamInfo->Channels                = ReadBits(BitI, 3) + 1; // 2
    FLAC->StreamInfo->BitDepth                = ReadBits(BitI, 5) + 1; // 24
    FLAC->StreamInfo->SamplesInStream         = ReadBits(BitI, 36);    // 9,078,720
    for (uint8_t MD5Byte = 0; MD5Byte < 16; MD5Byte++) {
        FLAC->StreamInfo->MD5[MD5Byte] = ReadBits(BitI, 8); // MD5 = 0x58B4285B9CFD75081C9E6A3B4C9E0D28
    }
}

void FLACReadSeekTable(BitInput *BitI, FLACFile *FLAC) { // 378
    FLAC->Seek->NumSeekPoints = FLAC->MetadataSize / 18; // 21

    for (uint16_t SeekPoint = 0; SeekPoint < FLAC->Seek->NumSeekPoints; SeekPoint++) {
        // FIXME: Not sure if this is correct
        FLAC->Seek->SampleInTargetFrame[SeekPoint]        = ReadBits(BitI, 64);
        FLAC->Seek->OffsetFrom1stSample[SeekPoint]        = ReadBits(BitI, 64);
        FLAC->Seek->TargetFrameSize[SeekPoint]            = ReadBits(BitI, 16);
    }
}

void FLACReadCuesheet(BitInput *BitI, FLACFile *FLAC) {
    for (uint8_t CatalogChar = 0; CatalogChar < 128; CatalogChar++) {
        FLAC->Cue->CatalogID[CatalogChar] = ReadBits(BitI, 8);
    }
    FLAC->Cue->LeadIn = ReadBits(BitI, 64);
    FLAC->Cue->IsCD   = ReadBits(BitI, 1);
    SkipBits(BitI, 2071); // Reserved
    FLAC->Cue->NumTracks = ReadBits(BitI, 8);
    // struct to contain the info for each track in the cuesheet

     // CUESHEET_TRACK
    for (uint8_t Track = 0; Track < FLAC->Cue->NumTracks; Track++) {
        FLAC->Cue->TrackOffset[Track]  = ReadBits(BitI, 64);
        FLAC->Cue->TrackNum[Track]     = ReadBits(BitI, 64);

        for (uint8_t ISRCByte = 0; ISRCByte < FLACISRCSize; ISRCByte++) {
            FLAC->Cue->ISRC[Track][ISRCByte]  = ReadBits(BitI, 8);
        }
        FLAC->Cue->IsAudio[Track] = ReadBits(BitI, 1);
        FLAC->Cue->PreEmphasis[Track] = ReadBits(BitI, 1);
        SkipBits(BitI, 152); // Reserved, all 0
        FLAC->Cue->NumTrackIndexPoints[Track] = ReadBits(BitI, 8);
    }

    // CUESHEET_TRACK_INDEX
    FLAC->Cue->IndexOffset    = ReadBits(BitI, 64);
    FLAC->Cue->IndexPointNum  = ReadBits(BitI, 8);
    SkipBits(BitI, 24); // Reserved
}

void FLACReadPicture(BitInput *BitI, FLACFile *FLAC, int64_t MaxPicBuffer) { // 17,151
    FLAC->Pic->PicType  = ReadBits(BitI, 32); // 3
    FLAC->Pic->MIMESize = ReadBits(BitI, 32); // 10
    for (uint32_t MIMEByte = 0; MIMEByte < FLAC->Pic->MIMESize; MIMEByte++) {
        FLAC->Pic->MIMEString[MIMEByte] = ReadBits(BitI, 8); // image/jpeg
    }
    FLAC->Pic->PicDescriptionSize = ReadBits(BitI, 32); // 0
    for (uint32_t Char = 0; Char < FLAC->Pic->PicDescriptionSize; Char++) {
        FLAC->Pic->PicDescriptionString[Char] = ReadBits(BitI, 8);
    }
    FLAC->Pic->Width       = ReadBits(BitI, 32); // 1144
    FLAC->Pic->Height      = ReadBits(BitI, 32); // 1144
    FLAC->Pic->BitDepth    = ReadBits(BitI, 32); // 24, PER PIXEL, NOT SUBPIXEL
    FLAC->Pic->ColorsUsed  = ReadBits(BitI, 32); // 0
    FLAC->Pic->PictureSize = ReadBits(BitI, 32); // 17,109
    if ((FLAC->Pic->PictureSize <= FLACMaxPicBuffer) && (MaxPicBuffer > 0)) { // don't decode if it's bigger than the max size, or if it's 0.
        for (uint32_t Byte = 0; Byte < FLAC->Pic->PictureSize; Byte++) {
            FLAC->Pic->PictureBuffer[Byte] = ReadBits(BitI, 8);
        }
    } else {
        SkipBits(BitI, Bytes2Bits(FLAC->Pic->PictureSize));
    }
}

void FLACReadCustom(BitInput *BitI, FLACFile *FLAC) { // 134,775

    // ALL ZERO! OVER 100 KILOBYTES OF NOTHING!
    SkipBits(BitI, Bytes2Bits(FLAC->MetadataSize + 1));
}

void FLACReadMetadata(BitInput *BitI, FLACFile *FLAC, int64_t MaxPicBuffer) { // FIXME: NEED TO LOOP UNTIL THERE IS NO METADATA LEFT
    // METADATA_BLOCK_HEADER
    //bool     IsLastMetadataBlock         = 0;
    // METADATA_BLOCK_DATA

    /*
     while ((BitI->FilePosition < BitI->FileSize) && (IsLastMetadataBlock == false)) { // Loops after it's done for some reason.
     // METADATA_BLOCK_HEADER
     */
    bool IsLastMetadataBlock         = ReadBits(BitI, 1);  // 1 YES!
    uint8_t  MetadataBlockType       = ReadBits(BitI, 7);  // 1
    FLAC->MetadataSize               = ReadBits(BitI, 24); // 134,775 // Does NOT count the 2 fields above.

    if (MetadataBlockType == StreamInfo) { // MetadataBlockType == StreamInfo // 0
        FLACReadStreamInfo(BitI, FLAC);
    } else if (MetadataBlockType == Padding) { // MetadataBlockType == Padding // 2
        SkipMetadataPadding(BitI, FLAC);
    } else if (MetadataBlockType == Custom) { // MetadataBlockType == Custom // Application specific chunk
        FLACReadCustom(BitI, FLAC);
    } else if (MetadataBlockType == SeekTable) { //  MetadataBlockType == SeekTable // 3
        FLACReadSeekTable(BitI, FLAC);
    } else if (MetadataBlockType == VorbisComment) { // MetadataBlockType == VorbisComment // 4
        FLACReadVorbisComment(BitI, FLAC);
    } else if (MetadataBlockType == Cuesheet) { // MetadataBlockType == Cuesheet // 5
        FLACReadCuesheet(BitI, FLAC);
    } else if (MetadataBlockType == Picture) { // MetadataBlockType == Picture // 6
        FLACReadPicture(BitI, FLAC, FLACMaxPicBuffer);
    } else {
        return;
        /*
         char ErrorDescription[BitIOStringSize];
         snprintf(ErrorDescription, BitIOStringSize, "Invalid Block type: %d", FLAC->MetadataBlockType);
         Log(SYSError, NULL, NULL, "NewFLAC", "FLACReadMetadata", ErrorDescription);
         */
    }

    /*
    // METADATA_BLOCK_HEADER
    bool     IsLastMetadataBlock     = ReadBits(BitI, 1); // 1 YES!
    char     MetadataBlockType       = ReadBits(BitI, 7); //
    FLAC->MetadataSize               = ReadBits(BitI, 24); // 17,151 // Does NOT count the 2 fields above.
                                                           //FLAC->MetadataSize               = SwapEndian32(FLAC->MetadataSize);
                                                           // METADATA_BLOCK_DATA

    while (IsLastMetadataBlock == false) {
        if (MetadataBlockType == StreamInfo) { // MetadataBlockType == StreamInfo // 0
            FLACReadStreamInfo(BitI, FLAC);
        } else if (MetadataBlockType == Custom) { // MetadataBlockType == Custom // Application specific chunk
            FLACReadCustom(BitI, FLAC);
        } else if (MetadataBlockType == Padding) { // MetadataBlockType == Padding // 2
            SkipMetadataPadding(BitI);
        } else if (MetadataBlockType == SeekTable) { //  MetadataBlockType == SeekTable // 3
            FLACReadSeekTable(BitI, FLAC);
        } else if (MetadataBlockType == VorbisComment) { // MetadataBlockType == VorbisComment // 4
            FLACReadVorbisComment(BitI, FLAC);
        } else if (MetadataBlockType == Cuesheet) { // MetadataBlockType == Cuesheet // 5
            FLACReadCuesheet(BitI, FLAC);
        } else if (MetadataBlockType == Picture) { // MetadataBlockType == Picture // 6
            FLACReadPicture(BitI, FLAC, MaxPicBuffer);
        } else {
            char ErrorDescription[BitIOStringSize];
            snprintf(ErrorDescription, BitIOStringSize, "Invalid Block type: %d", FLAC->MetadataBlockType);
            Log(SYSError, NULL, NULL, "NewFLAC", "FLACReadMetadata", ErrorDescription);
        }
    }
     */
}

void FLACSampleRate(BitInput *BitI, FLACFile *FLAC) { // 9
    uint32_t SampleRate = 0;
    if (FLAC->StreamInfo->CodedSampleRate        == 0) {
        FLAC->Frame->SampleRate = FLAC->StreamInfo->SampleRate;
    } else if (FLAC->StreamInfo->CodedSampleRate == 1) {
        FLAC->Frame->SampleRate = 88200;
    } else if (FLAC->StreamInfo->CodedSampleRate == 2) {
        FLAC->Frame->SampleRate = 176400;
    } else if (FLAC->StreamInfo->CodedSampleRate == 3) {
        FLAC->Frame->SampleRate = 192000;
    } else if (FLAC->StreamInfo->CodedSampleRate == 4) {
        FLAC->Frame->SampleRate = 8000;
    } else if (FLAC->StreamInfo->CodedSampleRate == 5) {
        FLAC->Frame->SampleRate = 16000;
    } else if (FLAC->StreamInfo->CodedSampleRate == 6) {
        FLAC->Frame->SampleRate = 22050;
    } else if (FLAC->StreamInfo->CodedSampleRate == 7) {
        FLAC->Frame->SampleRate = 24000;
    } else if (FLAC->StreamInfo->CodedSampleRate == 8) {
        FLAC->Frame->SampleRate = 32000;
    } else if (FLAC->StreamInfo->CodedSampleRate == 9) {
        FLAC->Frame->SampleRate = 44100;
    } else if (FLAC->StreamInfo->CodedSampleRate == 10) {
        FLAC->Frame->SampleRate = 48000;
    } else if (FLAC->StreamInfo->CodedSampleRate == 11) {
        FLAC->Frame->SampleRate = 96000;
    }
}

void FLACEncodeLPC(FLACFile *FLAC, int64_t *RAWSample[FLAC->Frame->BlockSize], uint8_t PredictorCoefficents, int8_t Shift) {
    int32_t PreviousSample = 0, CurrentSample = 0, EncodedSample = 0;
    for (uint16_t Sample = 0; Sample < FLAC->Frame->BlockSize; Sample++) {
        CurrentSample    = RAWSample[Sample];
        EncodedSample    = (RAWSample[Sample] - PreviousSample) * PredictorCoefficents;
        PreviousSample   = RAWSample[Sample];
    }
}

void FLACDecodeLPC(FLACFile *FLAC, int64_t EncodedSample, uint8_t PredictorCoefficents, int8_t Shift) {

}

void FLACLPCOrder(uint8_t SubFrameType) {
    uint8_t LPCOrder = 0;
    if (((SubFrameType & 20) >> 5) == 1) { // LPC Type
        LPCOrder = SubFrameType & 31;
    }
}

void FLACReadSubFrame(BitInput *BitI, FLACFile *FLAC, uint8_t Channel) {
    bool    WastedBitsFlag                  = false;
    uint8_t SubFrameType                    = 0, LPCOrder = 0, WarmUpSamples = 0, LPCPrecision = 0, WastedBits = 0;
    int8_t  LPCShift                        = 0;
    int64_t LPCCoeff[FLACMaxLPCCoefficents] = {0};
    int32_t Constant                        = 0;

    SkipBits(BitI, 1); // Reserved
    SubFrameType        = ReadBits(BitI, 6); // 0
    if (SubFrameType > 0) {
        LPCOrder            = (SubFrameType & 0x1F) - 1;
    }
    WastedBitsFlag      = ReadBits(BitI, 1); // 1
    if (WastedBitsFlag == true) {
        WastedBits      = ReadRICE(BitI, 1); // 2
    }
    uint8_t  PredictorOrder = 0;


    if (SubFrameType == Subframe_Constant) {
        Constant         = ReadBits(BitI, FLAC->Frame->BitDepth);
    }
    for (uint16_t Sample = 0; Sample < FLAC->Frame->BlockSize; Sample++) { // SamplesInBlock
        if (SubFrameType == Subframe_Constant) { // Subframe CONSTANT
            FLAC->RAWAudio[Channel][Sample] = Constant;
        } else if (((SubFrameType & 20) >> 5) == 1) { // SUBFRAME_LPC
            LPCOrder = (SubFrameType & 0x1F) + 1;
            WarmUpSamples         = ReadBits(BitI, LPCOrder);
            LPCPrecision          = ReadBits(BitI, 4) + 1;
            LPCShift              = ReadBits(BitI, 5);
            LPCCoeff[Sample]      = ReadBits(BitI, LPCPrecision * LPCOrder) + 1;
            FLACDecodeResidual(BitI, FLAC, FLAC->Frame->BlockSize, LPCOrder);
        } else if (((SubFrameType & 8) >> 3) == 1) {
            LPCOrder = (SubFrameType & 0x7) - 1;
        } else if (SubFrameType == Subframe_Verbatim) {
            FLAC->RAWAudio[Channel][Sample] = ReadBits(BitI, FLAC->StreamInfo->BitDepth);
        }
    }
    /*
    if (SubFrameType == Subframe_Constant) { // Subframe CONSTANT
        int32_t Constant    = ReadBits(BitI, FLAC->Frame->BitDepth);
        memset(FLAC->RAWAudio[Channel], Constant, FLAC->Frame->SamplesInBlock);
    } else {
        for (uint16_t Sample = 0; Sample < FLAC->Frame->SamplesInBlock; Sample++) {
            if (((SubFrameType & 20) >> 5) == 1) { // LPC subframe
                LPCOrder = SubFrameType & 0x1F;
                FLAC->RAWAudio[Channel][Sample] = ReadBits(BitI, 0); // FIXME: 0 is bullshit
            } else if (((SubFrameType & 8) >> 3) == 1) { // LPC Fixed
                LPCOrder = SubFrameType & 0x7;
            } else if (SubFrameType == Subframe_Verbatim) { // Subframe VERBATIM aka raw PCM
                FLAC->RAWAudio[Channel][Sample] = ReadBits(BitI, FLAC->StreamInfo->BitDepth);
            }
        }
    }
     */
}

void FLACDecodeRICEPartition(BitInput *BitI, FLACFile *FLAC, uint32_t BlockSize, uint8_t PredictorOrder) {
    uint8_t RiceParameter[16];
    for (uint8_t Partition = 0; Partition < FLAC->Frame->PartitionOrder; Partition++) {
        RiceParameter[Partition] = ReadBits(BitI, 4) + 5;
        if (RiceParameter[Partition] == 20) {
            // Escape code, meaning the partition is in unencoded binary form using n bits per sample; n follows as a 5-bit number.
        } else {
            if (FLAC->Frame->PartitionOrder == 0) {
                RiceParameter[Partition] = ReadBits(BitI, BlockSize - PredictorOrder);
            } else if (FLAC->Frame->PartitionOrder > 0) {
                RiceParameter[Partition] = ReadBits(BitI, (BlockSize / pow(2, FLAC->Frame->PartitionOrder)));
            } else {
                RiceParameter[Partition] = ReadBits(BitI, (BlockSize / pow(2, FLAC->Frame->PartitionOrder)) - PredictorOrder);
            }
        }
    }
}

void FLACDecodeRICE2Partition(BitInput *BitI, FLACFile *FLAC, uint32_t BlockSize, uint8_t PredictorOrder) {
    uint8_t RiceParameter[16];
    for (uint8_t Partition = 0; Partition < FLAC->Frame->PartitionOrder; Partition++) {
        RiceParameter[Partition] = ReadBits(BitI, 5) + 5;
        if (RiceParameter[Partition] == 36) {
            // Escape code, meaning the partition is in unencoded binary form using n bits per sample; n follows as a 5-bit number.
        } else {
            if (FLAC->Frame->PartitionOrder == 0) {
                RiceParameter[Partition] = ReadBits(BitI, BlockSize - PredictorOrder);
            } else if (FLAC->Frame->PartitionOrder > 0) {
                RiceParameter[Partition] = ReadBits(BitI, (BlockSize / pow(2, FLAC->Frame->PartitionOrder)));
            } else {
                RiceParameter[Partition] = ReadBits(BitI, (BlockSize / pow(2, FLAC->Frame->PartitionOrder)) - PredictorOrder);
            }
        }
    }
}

void FLACDecodeResidual(BitInput *BitI, FLACFile *FLAC, uint32_t BlockSize, uint8_t PredictorOrder) {
    uint8_t PartitionType = ReadBits(BitI, 2);
    FLAC->Frame->PartitionOrder  = ReadBits(BitI, 4);

    FLACDecodeRicePartition(BitI, FLAC, PartitionType);
}

void FLACDecodeRicePartition(BitInput *BitI, FLACFile *FLAC, uint8_t PartitionType) {
    uint8_t RICEType = 0, BitsPerSample = 0;
    if (PartitionType == 0) { // RICE 1
        RICEType = ReadBits(BitI, 4);
        if (RICEType == 0xF) { // PCM using N bits per sample.
            BitsPerSample = ReadBits(BitI, 5);
        }
    } else if (PartitionType == 1) { // RICE2
        RICEType = ReadBits(BitI, 5);
        if (RICEType == 0x1F) {
            BitsPerSample = ReadBits(BitI, 5);
        }
    } else {
        char Error[BitIOStringSize];
        snprintf(Error, BitIOStringSize, "Invalid Rice partition %d at offset %d\n", PartitionType, (BitI->FilePosition + Bits2Bytes(BitI->BitsUnavailable)));
        Log(SYSError, NULL, NULL, "libFLAC2", "FLACParseRicePartition", Error);
    }

    if (BitsPerSample > 0) {
        if (FLAC->Frame->PartitionOrder == 0) {
            FLAC->Frame->SamplesInPartition = FLAC->Frame->BlockSize - FLAC->Frame->PartitionOrder; // SamplesInBlock - PartitionOrder
        } else if ((FLAC->Frame->PartitionOrder >= 1) && (FLAC->Frame->CurrentPartition == 0)) {
            FLAC->Frame->SamplesInPartition = (FLAC->Frame->BlockSize / pow(2,FLAC->Frame->PartitionOrder));
        } else {
            //else n = (frame's blocksize / (2^partition order)) - predictor order
            FLAC->Frame->SamplesInPartition = ((FLAC->Frame->BlockSize / pow(2,FLAC->Frame->PartitionOrder)) - FLAC->Frame->PartitionOrder);
        }
    }
}

void FLACReadFrame(BitInput *BitI, FLACFile *FLAC) {
    SkipBits(BitI, 1); // 0
    FLAC->Frame->BlockType            = ReadBits(BitI, 1); // 0
    uint8_t CodedSamplesInBlock       = ReadBits(BitI, 4);
    if (((CodedSamplesInBlock != 6) || (CodedSamplesInBlock != 7))) {
        FLAC->Frame->BlockSize        = GetBlockSizeInSamples(CodedSamplesInBlock); // SamplesInBlock
    }
    FLAC->Frame->CodedSampleRate      = ReadBits(BitI, 4); // 0x9
    if ((FLAC->Frame->CodedSampleRate >= 0) && (FLAC->Frame->CodedSampleRate <= 11)) {
        FLACSampleRate(BitI, FLAC);
    }
    FLAC->Frame->ChannelLayout        = ReadBits(BitI, 4) + 1; // 0x1 aka stereo, right difference
    FLAC->Frame->CodedBitDepth        = ReadBits(BitI, 3); // 6 aka 24 bits per sample
    if (FLAC->Frame->CodedBitDepth != 0) {
        FLAC->Frame->BitDepth         = FLACBitDepth(FLAC); // 24
    }
    SkipBits(BitI, 1); // 0

    if (FLAC->Frame->BlockType        == FixedBlockSize) { // variable blocktype
        FLAC->Frame->FrameNumber      = ReadBits(BitI, 31); // 4,915,200
    } else if (FLAC->Frame->BlockType == VariableBlockSize) {
        FLAC->Frame->SampleNumber     = ReadBits(BitI, 36);
    }

    if (CodedSamplesInBlock == 6) {
        FLAC->Frame->BlockSize = ReadBits(BitI, 8); // SamplesInBlock
    } else if (CodedSamplesInBlock == 7) {
        FLAC->Frame->BlockSize = ReadBits(BitI, 16); // SamplesInBlock
    }


    if (FLAC->Frame->CodedSampleRate == 12) {
        FLAC->Frame->SampleRate = ReadBits(BitI, 8) * 1000;
    } else if (FLAC->Frame->CodedSampleRate == 13) {
        FLAC->Frame->SampleRate = ReadBits(BitI, 16);
    } else if (FLAC->Frame->CodedSampleRate == 14) {
        FLAC->Frame->SampleRate = ReadBits(BitI, 16) * 10;
    }

    FLAC->Frame->FLACFrameCRC       = ReadBits(BitI, 8); // CRC, 0x00, 1 bit still remaining

    for (uint8_t Channel = 0; Channel < FLAC->StreamInfo->Channels; Channel++) {
        // read SubFrame
        FLACReadSubFrame(BitI, FLAC, Channel);
    }
}

uint8_t GetBlockSizeInSamples(uint8_t BlockSize) {
    uint16_t SamplesInBlock = 0;
    if (BlockSize == 1) {
        SamplesInBlock = 192;
    } else if ((BlockSize >= 2 && BlockSize <= 5)) {
        SamplesInBlock = (576 * pow(2, (BlockSize - 2))); // 576/1152/2304/4608, pow(2, (BlockSize - 2))
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

uint32_t FLACBitDepth(FLACFile *FLAC) {
    uint32_t BitDepth = 0;
    if (FLAC->StreamInfo->CodedBitDepth == 0) {
        BitDepth = FLAC->StreamInfo->BitDepth;
    } else if (FLAC->StreamInfo->CodedBitDepth == 1) {
        BitDepth =  8;
    } else if (FLAC->StreamInfo->CodedBitDepth == 2) {
        BitDepth = 12;
    } else if (FLAC->StreamInfo->CodedBitDepth == 3) {
        BitDepth =  0;
    } else if (FLAC->StreamInfo->CodedBitDepth == 4) {
        BitDepth = 16;
    } else if (FLAC->StreamInfo->CodedBitDepth == 5) {
        BitDepth = 20;
    } else if (FLAC->StreamInfo->CodedBitDepth == 6) {
        BitDepth = 24;
    } else {
        BitDepth = 0;
    }
    return BitDepth;
}

void SkipMetadataPadding(BitInput *BitI, FLACFile *FLAC) {
    SkipBits(BitI, Bytes2Bits(FLAC->MetadataSize));
    /*
    while (ReadBits(BitI, 1) == 0) {
        AlignInput(BitI, 1);
    }
     */
}

void InitFLACFile(FLACFile *FLAC) {
    FLAC->StreamInfo                     = calloc(sizeof(FLACStreamInfo), 1);
    FLAC->Cue                            = calloc(sizeof(FLACCueSheet), 1);
    FLAC->Frame                          = calloc(sizeof(FLACFrame), 1);
    FLAC->Vorbis                         = calloc(sizeof(FLACVorbisComment), 1);
    FLAC->Seek                           = calloc(sizeof(FLACSeekTable), 1);
    FLAC->Pic                            = calloc(sizeof(FLACPicture), 1);
}

void FLACDecodeFile(int argc, const char *argv[]) {
    BitInput    *BitI  = calloc(sizeof(BitInput), 1);
    BitOutput   *BitO  = calloc(sizeof(BitOutput), 1);
    ErrorStatus *Error = calloc(sizeof(ErrorStatus), 1);
    FLACFile    *FLAC  = calloc(sizeof(FLACFile), 1);
    InitBitInput(BitI, Error, argc, argv);
    InitBitOutput(BitO, Error, argc, argv);
    InitFLACFile(FLAC);

    if (ReadBits(BitI, 32) != FLACMagic) {
        // Not a FLAC file
    } else {
        SkipBits(BitI, 32);
        FLACReadMetadata(BitI, FLAC, 0xFFFFFFFF);
        while (BitI->FileSize > 0) {
            FLACReadFrame(BitI, FLAC);
        }
    }
}
