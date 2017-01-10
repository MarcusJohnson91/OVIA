#include "../include/DecodeFLAC.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void FLACReadStreamInfo(BitInput *BitI, FLACFile *FLAC) {
        FLAC->Meta->StreamInfo->MinimumBlockSize        = ReadBits(BitI, 16);
        FLAC->Meta->StreamInfo->MaximumBlockSize        = ReadBits(BitI, 16);
        FLAC->Meta->StreamInfo->MinimumFrameSize        = ReadBits(BitI, 24);
        FLAC->Meta->StreamInfo->MaximumFrameSize        = ReadBits(BitI, 24);
        FLAC->Meta->StreamInfo->SampleRate              = ReadBits(BitI, 20);
        FLAC->Meta->StreamInfo->Channels                = ReadBits(BitI, 3) + 1;
        FLAC->Meta->StreamInfo->BitDepth                = ReadBits(BitI, 5) + 1;
        FLAC->Meta->StreamInfo->SamplesInStream         = ReadBits(BitI, 36);
        for (uint8_t MD5Byte = 0; MD5Byte < 16; MD5Byte++) {
            FLAC->Meta->StreamInfo->MD5[MD5Byte] = ReadBits(BitI, 8); // MD5 = 0x58B4285B9CFD75081C9E6A3B4C9E0D28
        }
    }
    
    void SkipMetadataPadding(BitInput *BitI, FLACFile *FLAC) {
        SkipBits(BitI, Bytes2Bits(FLAC->Meta->MetadataSize));
    }
    
    void FLACReadCustom(BitInput *BitI, FLACFile *FLAC) { // 134,775
        SkipBits(BitI, Bytes2Bits(FLAC->Meta->MetadataSize + 1));
    }
    
    void FLACReadSeekTable(BitInput *BitI, FLACFile *FLAC) { // 378
        FLAC->Meta->Seek->NumSeekPoints = FLAC->Meta->MetadataSize / 18; // 21
        
        for (uint16_t SeekPoint = 0; SeekPoint < FLAC->Meta->Seek->NumSeekPoints; SeekPoint++) {
            FLAC->Meta->Seek->SampleInTargetFrame[SeekPoint]        = ReadBits(BitI, 64);
            FLAC->Meta->Seek->OffsetFrom1stSample[SeekPoint]        = ReadBits(BitI, 64);
            FLAC->Meta->Seek->TargetFrameSize[SeekPoint]            = ReadBits(BitI, 16);
        }
    }
    
    void FLACReadVorbisComment(BitInput *BitI, FLACFile *FLAC) { // LITTLE ENDIAN
        uint32_t UserCommentSize[FLACMaxVorbisComments] = {0};
        
        uint32_t VendorTagSize = SwapEndian32(ReadBits(BitI, 32));
        for (uint32_t TagByte = 0; TagByte < VendorTagSize; TagByte++) {
            FLAC->Meta->Vorbis->VendorString[TagByte]  = ReadBits(BitI, 8); // reference libFLAC 1.3.1 20141125
        }
        
        uint32_t NumUserComments = SwapEndian32(ReadBits(BitI, 32)); // 28
        for (uint32_t UserComment = 0; UserComment < NumUserComments; UserComment++) {
            UserCommentSize[UserComment] = SwapEndian32(ReadBits(BitI, 32)); // 28
            for (uint32_t TagByte = 0; TagByte < UserCommentSize[UserComment]; TagByte++) {
                FLAC->Meta->Vorbis->UserCommentString[UserComment][TagByte] = ReadBits(BitI, 8);
            }
        }
    }
    
    void FLACReadCuesheet(BitInput *BitI, FLACFile *FLAC) {
        for (uint8_t CatalogChar = 0; CatalogChar < FLACMedizCatalogNumberSize; CatalogChar++) {
            FLAC->Meta->Cue->CatalogID[CatalogChar] = ReadBits(BitI, 8);
        }
        FLAC->Meta->Cue->LeadIn = ReadBits(BitI, 64);
        FLAC->Meta->Cue->IsCD   = ReadBits(BitI, 1);
        SkipBits(BitI, 2071); // Reserved
        FLAC->Meta->Cue->NumTracks = ReadBits(BitI, 8);
        
        for (uint8_t Track = 0; Track < FLAC->Meta->Cue->NumTracks; Track++) {
            FLAC->Meta->Cue->TrackOffset[Track]  = ReadBits(BitI, 64);
            FLAC->Meta->Cue->TrackNum[Track]     = ReadBits(BitI, 64);
            
            for (uint8_t ISRCByte = 0; ISRCByte < FLACISRCSize; ISRCByte++) {
                FLAC->Meta->Cue->ISRC[Track][ISRCByte]  = ReadBits(BitI, 8);
            }
            FLAC->Meta->Cue->IsAudio[Track] = ReadBits(BitI, 1);
            FLAC->Meta->Cue->PreEmphasis[Track] = ReadBits(BitI, 1);
            SkipBits(BitI, 152); // Reserved, all 0
            FLAC->Meta->Cue->NumTrackIndexPoints[Track] = ReadBits(BitI, 8);
        }
        
        FLAC->Meta->Cue->IndexOffset    = ReadBits(BitI, 64);
        FLAC->Meta->Cue->IndexPointNum  = ReadBits(BitI, 8);
        SkipBits(BitI, 24); // Reserved
    }
    
    void FLACReadPicture(BitInput *BitI, FLACFile *FLAC) { // 17,151
        FLAC->Meta->Pic->PicType  = ReadBits(BitI, 32); // 3
        FLAC->Meta->Pic->MIMESize = ReadBits(BitI, 32); // 10
        for (uint32_t MIMEByte = 0; MIMEByte < FLAC->Meta->Pic->MIMESize; MIMEByte++) {
            FLAC->Meta->Pic->MIMEString[MIMEByte] = ReadBits(BitI, 8); // image/jpeg
        }
        FLAC->Meta->Pic->PicDescriptionSize = ReadBits(BitI, 32); // 0
        for (uint32_t Char = 0; Char < FLAC->Meta->Pic->PicDescriptionSize; Char++) {
            FLAC->Meta->Pic->PicDescriptionString[Char] = ReadBits(BitI, 8);
        }
        FLAC->Meta->Pic->Width       = ReadBits(BitI, 32); // 1144
        FLAC->Meta->Pic->Height      = ReadBits(BitI, 32); // 1144
        FLAC->Meta->Pic->BitDepth    = ReadBits(BitI, 32); // 24, PER PIXEL, NOT SUBPIXEL
        FLAC->Meta->Pic->ColorsUsed  = ReadBits(BitI, 32); // 0
        FLAC->Meta->Pic->PictureSize = ReadBits(BitI, 32); // 17,109
                                                           // Pop in the address of the start of the data, and skip over the data instead of buffering it.
    }
    
    void FLACDecodeMetadata(BitInput *BitI, FLACFile *FLAC) {
        bool     IsLastMetadataBlock     = ReadBits(BitI, 1);
        uint8_t  MetadataBlockType       = ReadBits(BitI, 7);
        FLAC->Meta->MetadataSize         = ReadBits(BitI, 24); // Does NOT count the 2 fields above.
        
        if (MetadataBlockType        == StreamInfo) {
            FLACReadStreamInfo(BitI, FLAC);
        } else if (MetadataBlockType == Padding) {
            SkipMetadataPadding(BitI, FLAC);
        } else if (MetadataBlockType == Custom) {
            FLACReadCustom(BitI, FLAC);
        } else if (MetadataBlockType == SeekTable) {
            FLACReadSeekTable(BitI, FLAC);
        } else if (MetadataBlockType == VorbisComment) {
            FLACReadVorbisComment(BitI, FLAC);
        } else if (MetadataBlockType == Cuesheet) {
            FLACReadCuesheet(BitI, FLAC);
        } else if (MetadataBlockType == Picture) {
            FLACReadPicture(BitI, FLAC);
        } else {
            char ErrorDescription[BitIOStringSize];
            snprintf(ErrorDescription, BitIOStringSize, "Invalid Block type: %d, Is last metadata block: %d", MetadataBlockType, IsLastMetadataBlock);
            Log(SYSError, "NewFLAC", "FLACReadMetadata", ErrorDescription);
        }
    }
    
    void InitDecodeFLACFile(FLACFile *FLAC) {
        FLAC->Meta->StreamInfo               = calloc(sizeof(FLACStreamInfo), 1);
        FLAC->Meta->Seek                     = calloc(sizeof(FLACSeekTable), 1);
        FLAC->Meta->Vorbis                   = calloc(sizeof(FLACVorbisComment), 1);
        FLAC->Meta->Cue                      = calloc(sizeof(FLACCueSheet), 1);
        FLAC->Meta->Pic                      = calloc(sizeof(FLACPicture), 1);
        
        FLAC->Data->Frame                    = calloc(sizeof(FLACFrame), 1);
        FLAC->Data->LPC                      = calloc(sizeof(FLACLPC), 1);
    }
    
    uint8_t FLACLPCOrder(uint8_t SubFrameType) {
        uint8_t LPCOrder = 0;
        if (((SubFrameType & 20) >> 5) == 1) { // LPC Type
            LPCOrder = SubFrameType & 31;
        }
        return LPCOrder;
    }
    
    void FLACDecodeSubFrameLPC(BitInput *BitI, FLACFile *FLAC, uint8_t SubFrameType, uint8_t Channel, int64_t *DecodedSamples[FLACMaxSamplesInBlock]) {
        uint8_t LPCOrder = 0, LPCPrecision = 0, LPCShift = 0, LPCCoeff[65535] = {0};
        
        LPCOrder              = FLACLPCOrder(SubFrameType);
        for (uint16_t Sample = 0; Sample < FLAC->Data->Frame->BitDepth * LPCOrder; Sample++) {
            DecodedSamples[Sample] = ReadExpGolomb(BitI, false, false);
            LPCPrecision           = ReadBits(BitI, 4) + 1;
            LPCShift               = ReadBits(BitI, 5);
            LPCCoeff[Sample]       = ReadBits(BitI, LPCPrecision * LPCOrder) + 1;
        }
        FLACDecodeResidual(BitI, FLAC, FLAC->Data->Frame->BlockSize, LPCOrder);
    }
    
    void FLACReadSubFrame(BitInput *BitI, FLACFile *FLAC, uint8_t Channel, int64_t *DecodedSamples[FLACMaxSamplesInBlock]) {
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
            WastedBits      = ReadRICE(BitI, false, 1); // 2
        }
        uint8_t  PredictorOrder = 0;
        
        
        if (SubFrameType == Subframe_Constant) {
            Constant                  = ReadBits(BitI, FLAC->Data->Frame->BitDepth);
        }
        for (uint16_t Sample = 0; Sample < FLAC->Data->Frame->BlockSize; Sample++) { // SamplesInBlock
            if (SubFrameType == Subframe_Constant) { // Subframe CONSTANT
                DecodedSamples[Sample] = Constant;
            } else if (((SubFrameType & 20) >> 5) == 1) { // SUBFRAME_LPC
                
            } else if (((SubFrameType & 8) >> 3) == 1) {
                LPCOrder = (SubFrameType & 0x7) - 1;
            } else if (SubFrameType == Subframe_Verbatim) {
                DecodedSamples[Sample] = ReadBits(BitI, FLAC->Meta->StreamInfo->BitDepth);
            }
        }
    }
    
    void FLACDecodeRICEPartition(BitInput *BitI, FLACFile *FLAC, uint32_t BlockSize, uint8_t PredictorOrder) {
        uint8_t RiceParameter[16];
        for (uint8_t Partition = 0; Partition < FLAC->Data->Frame->PartitionOrder; Partition++) {
            RiceParameter[Partition] = ReadBits(BitI, 4) + 5;
            if (RiceParameter[Partition] == 20) {
                // Escape code, meaning the partition is in unencoded binary form using n bits per sample; n follows as a 5-bit number.
            } else {
                if (FLAC->Data->Frame->PartitionOrder == 0) {
                    RiceParameter[Partition] = ReadBits(BitI, BlockSize - PredictorOrder);
                } else if (FLAC->Data->Frame->PartitionOrder > 0) {
                    RiceParameter[Partition] = ReadBits(BitI, (BlockSize / pow(2, FLAC->Data->Frame->PartitionOrder)));
                } else {
                    RiceParameter[Partition] = ReadBits(BitI, (BlockSize / pow(2, FLAC->Data->Frame->PartitionOrder)) - PredictorOrder);
                }
            }
        }
    }
    
    void FLACDecodeRICE2Partition(BitInput *BitI, FLACFile *FLAC, uint32_t BlockSize, uint8_t PredictorOrder) {
        uint8_t RiceParameter[16];
        for (uint8_t Partition = 0; Partition < FLAC->Data->Frame->PartitionOrder; Partition++) {
            RiceParameter[Partition] = ReadBits(BitI, 5) + 5;
            if (RiceParameter[Partition] == 36) {
                // Escape code, meaning the partition is in unencoded binary form using n bits per sample; n follows as a 5-bit number.
            } else {
                if (FLAC->Data->Frame->PartitionOrder == 0) {
                    RiceParameter[Partition] = ReadBits(BitI, BlockSize - PredictorOrder);
                } else if (FLAC->Data->Frame->PartitionOrder > 0) {
                    RiceParameter[Partition] = ReadBits(BitI, (BlockSize / pow(2, FLAC->Data->Frame->PartitionOrder)));
                } else {
                    RiceParameter[Partition] = ReadBits(BitI, (BlockSize / pow(2, FLAC->Data->Frame->PartitionOrder)) - PredictorOrder);
                }
            }
        }
    }
    
    void FLACDecodeResidual(BitInput *BitI, FLACFile *FLAC, uint32_t BlockSize, uint8_t PredictorOrder) {
        uint8_t PartitionType = ReadBits(BitI, 2);
        FLAC->Data->Frame->PartitionOrder  = ReadBits(BitI, 4);
        FLACDecodeRicePartition(BitI, FLAC, PartitionType);
    }
    
    void FLACDecodeRicePartition(BitInput *BitI, FLACFile *FLAC, uint8_t PartitionType) {
        uint8_t RICEType  = 0, BitsPerSample = 0;
        if (PartitionType == 0) { // RICE 1
            RICEType      = ReadBits(BitI, 4);
            if (RICEType  == 0xF) { // PCM using N bits per sample.
                BitsPerSample = ReadBits(BitI, 5);
            }
        } else if (PartitionType == 1) { // RICE2
            RICEType = ReadBits(BitI, 5);
            if (RICEType == 0x1F) {
                BitsPerSample = ReadBits(BitI, 5);
            }
        } else {
            char Error[BitIOStringSize];
            snprintf(Error, BitIOStringSize, "Invalid Rice partition %d at offset %llu\n", PartitionType, (BitI->FilePosition + Bits2Bytes(BitI->BitsUnavailable)));
            Log(SYSError, "NewFLAC", "FLACParseRicePartition", Error);
        }
        
        if (BitsPerSample > 0) {
            if (FLAC->Data->Frame->PartitionOrder == 0) {
                FLAC->Data->Frame->SamplesInPartition = FLAC->Data->Frame->BlockSize - FLAC->Data->Frame->PartitionOrder; // SamplesInBlock - PartitionOrder
            } else if ((FLAC->Data->Frame->PartitionOrder >= 1) && (FLAC->Data->Frame->CurrentPartition == 0)) {
                FLAC->Data->Frame->SamplesInPartition = (FLAC->Data->Frame->BlockSize / pow(2,FLAC->Data->Frame->PartitionOrder));
            } else {
                //else n = (frame's blocksize / (2^partition order)) - predictor order
                FLAC->Data->Frame->SamplesInPartition = ((FLAC->Data->Frame->BlockSize / pow(2,FLAC->Data->Frame->PartitionOrder)) - FLAC->Data->Frame->PartitionOrder);
            }
        }
    }
    
    void FLACReadFrame(BitInput *BitI, FLACFile *FLAC, int64_t *DecodedSamples[FLACMaxSamplesInBlock]) {
        uint8_t CodedSamplesInBlock = 0;
        
        SkipBits(BitI, 1); // 0
        FLAC->Data->Frame->BlockType            = ReadBits(BitI, 1); // 0
        CodedSamplesInBlock                     = ReadBits(BitI, 4);
        if (((CodedSamplesInBlock != 6) || (CodedSamplesInBlock != 7))) {
            FLAC->Data->Frame->BlockSize        = GetBlockSizeInSamples(CodedSamplesInBlock); // SamplesInBlock
        }
        FLAC->Data->Frame->CodedSampleRate      = ReadBits(BitI, 4); // 0x9
        if ((FLAC->Data->Frame->CodedSampleRate >= 0) && (FLAC->Data->Frame->CodedSampleRate <= 11)) {
            FLACSampleRate(BitI, FLAC);
        }
        FLAC->Data->Frame->ChannelLayout        = ReadBits(BitI, 4) + 1; // 0x1 aka stereo, right difference
        FLAC->Data->Frame->CodedBitDepth        = ReadBits(BitI, 3); // 6 aka 24 bits per sample
        if (FLAC->Data->Frame->CodedBitDepth != 0) {
            FLACBitDepth(FLAC); // 24
        }
        SkipBits(BitI, 1); // 0
        
        if (FLAC->Data->Frame->BlockType        == FixedBlockSize) { // variable blocktype
            FLAC->Data->Frame->FrameNumber      = ReadBits(BitI, 31); // 4,915,200
        } else if (FLAC->Data->Frame->BlockType == VariableBlockSize) {
            FLAC->Data->Frame->SampleNumber     = ReadBits(BitI, 36);
        }
        
        if (CodedSamplesInBlock == 6) {
            FLAC->Data->Frame->BlockSize = ReadBits(BitI, 8); // SamplesInBlock
        } else if (CodedSamplesInBlock == 7) {
            FLAC->Data->Frame->BlockSize = ReadBits(BitI, 16); // SamplesInBlock
        }
        
        
        if (FLAC->Data->Frame->CodedSampleRate == 12) {
            FLAC->Data->Frame->SampleRate = ReadBits(BitI, 8) * 1000;
        } else if (FLAC->Data->Frame->CodedSampleRate == 13) {
            FLAC->Data->Frame->SampleRate = ReadBits(BitI, 16);
        } else if (FLAC->Data->Frame->CodedSampleRate == 14) {
            FLAC->Data->Frame->SampleRate = ReadBits(BitI, 16) * 10;
        }
        
        FLAC->Data->Frame->FLACFrameCRC       = ReadBits(BitI, 8); // CRC, 0x00, 1 bit still remaining
        
        for (uint8_t Channel = 0; Channel < FLAC->Meta->StreamInfo->Channels; Channel++) {
            // read SubFrame
            FLACReadSubFrame(BitI, FLAC, Channel, DecodedSamples);
        }
    }
    
#ifdef __cplusplus
}
#endif
