#include "../include/DecodeFLAC.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void InitDecodeFLACFile(FLACDecoder *FLAC) {
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
        FLAC->Data->Frame->BlockType            = ReadBits(BitI, 1); // 0
        FLAC->Data->Frame->CodedSamplesInBlock  = ReadBits(BitI, 4);
        if (((FLAC->Data->Frame->CodedSamplesInBlock != 6) || (FLAC->Data->Frame->CodedSamplesInBlock != 7))) {
            FLAC->Data->Frame->BlockSize        = GetBlockSizeInSamples(FLAC->Data->Frame->CodedSamplesInBlock); // SamplesInBlock
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
        
        if (FLAC->Data->Frame->CodedSamplesInBlock == 6) {
            FLAC->Data->Frame->BlockSize        = ReadBits(BitI, 8); // SamplesInBlock
        } else if (FLAC->Data->Frame->CodedSamplesInBlock == 7) {
            FLAC->Data->Frame->BlockSize        = ReadBits(BitI, 16); // SamplesInBlock
        }
        
        
        if (FLAC->Data->Frame->CodedSampleRate == 12) {
            FLAC->Data->Frame->SampleRate       = ReadBits(BitI, 8) * 1000;
        } else if (FLAC->Data->Frame->CodedSampleRate == 13) {
            FLAC->Data->Frame->SampleRate       = ReadBits(BitI, 16);
        } else if (FLAC->Data->Frame->CodedSampleRate == 14) {
            FLAC->Data->Frame->SampleRate       = ReadBits(BitI, 16) * 10;
        }
        
        FLAC->Data->Frame->FLACFrameCRC         = ReadBits(BitI, 8); // CRC, 0x00, 1 bit still remaining
        
        for (uint8_t Channel = 0; Channel < FLAC->Meta->StreamInfo->Channels; Channel++) { // read SubFrame
            FLACReadSubFrame(BitI, FLAC, Channel);
        }
    }
    
    void FLACReadSubFrame(BitInput *BitI, FLACDecoder *FLAC, uint8_t Channel) {
        SkipBits(BitI, 1); // Reserved
        FLAC->Data->SubFrame->SubFrameType      = ReadBits(BitI, 6); // 0
        if (FLAC->Data->SubFrame->SubFrameType > 0) {
            FLAC->Data->LPC->LPCOrder = (FLAC->Data->SubFrame->SubFrameType & 0x1F) - 1;
        }
        FLAC->Data->SubFrame->WastedBitsFlag    = ReadBits(BitI, 1); // 1
        if (FLAC->Data->SubFrame->WastedBitsFlag == true) {
            FLAC->Data->SubFrame->WastedBits    = ReadRICE(BitI, false, 1); // 2
        }
        
        if (FLAC->Data->SubFrame->SubFrameType == Subframe_Verbatim) { // PCM
            FLACDecodeSubFrameVerbatim(BitI, FLAC);
        } else if (FLAC->Data->SubFrame->SubFrameType == Subframe_Constant) {
            FLACDecodeSubFrameConstant(BitI, FLAC);
        } else if (FLAC->Data->SubFrame->SubFrameType == Subframe_Fixed) {
            
        } else if (FLAC->Data->SubFrame->SubFrameType == Subframe_LPC) {
            FLACDecodeSubFrameLPC(BitI, FLAC, Channel);
        } else {
            char Description[BitIOStringSize];
            snprintf(Description, BitIOStringSize, "Invalid Subframe type: %d", FLAC->Data->SubFrame->SubFrameType);
            Log(SYSError, "ModernFLAC", "FLACReadSubframe", Description);
        }
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    void FLACDecodeResidual(BitInput *BitI, FLACDecoder *FLAC) {
        FLAC->Data->LPC->RicePartitionType      = ReadBits(BitI, 2);
        if (FLAC->Data->LPC->RicePartitionType == RICE1) {
            FLACDecodeRice1Partition(BitI, FLAC);
        } else if (FLAC->Data->LPC->RicePartitionType == RICE2) {
            FLACDecodeRice2Partition(BitI, FLAC);
        }
    }
    
    void FLACDecodeSubFrameFixed(BitInput *BitI, FLACDecoder *FLAC) {
        for (uint16_t WarmupSample = 0; WarmupSample < FLAC->Data->Frame->BitDepth * FLAC->Data->LPC->LPCOrder; WarmupSample++) {
            FLAC->DecodedSamples[WarmupSample]  = ReadBits(BitI, FLAC->Data->Frame->BitDepth);
        }
        FLACDecodeResidual(BitI, FLAC);
    }
    
    void FLACDecodeSubFrameLPC(BitInput *BitI, FLACDecoder *FLAC, uint8_t Channel) {
        FLAC->Data->LPC->LPCOrder  = FLAC->Data->SubFrame->SubFrameType & 31;
        
        for (uint16_t WarmupSample = 0; WarmupSample < FLAC->Data->Frame->BitDepth * FLAC->Data->LPC->LPCOrder; WarmupSample++) {
            FLAC->DecodedSamples[WarmupSample]  = ReadBits(BitI, FLAC->Data->Frame->BitDepth);
        }
        
        FLAC->Data->LPC->LPCPrecision           = ReadBits(BitI, 4) + 1;
        FLAC->Data->LPC->LPCShift               = ReadBits(BitI, 5);
        FLAC->Data->LPC->NumLPCCoeffs           = FLAC->Data->LPC->LPCPrecision * FLAC->Data->LPC->LPCOrder;
        
        
        for (uint16_t LPCCoefficent = 0; LPCCoefficent < FLAC->Data->LPC->NumLPCCoeffs; LPCCoefficent++) {
            FLAC->Data->LPC->LPCCoeff[LPCCoefficent] = ReadBits(BitI, FLAC->Data->LPC->NumLPCCoeffs) + 1;
        }
        FLACDecodeResidual(BitI, FLAC);
    }
    
    void FLACDecodeSubFrameConstant(BitInput *BitI, FLACDecoder *FLAC) {
        int64_t Constant = ReadBits(BitI, FLAC->Data->Frame->BitDepth);
        memset(FLAC->DecodedSamples, Constant, FLAC->Data->Frame->BlockSize);
    }
    
    void FLACDecodeSubFrameVerbatim(BitInput *BitI, FLACDecoder *FLAC) {
        for (uint16_t Sample = 0; Sample < FLAC->Data->Frame->BlockSize; Sample++) {
            FLAC->DecodedSamples[Sample] = ReadBits(BitI, FLAC->Data->Frame->BitDepth);
        }
    }
    
    void FLACDecodeRice1Partition(BitInput *BitI, FLACDecoder *FLAC) {
        FLAC->Data->LPC->PartitionOrder = ReadBits(BitI, 4);
        for (uint8_t Partition = 0; Partition < FLAC->Data->LPC->PartitionOrder; Partition++) {
            FLAC->Data->Rice->RICEParameter[Partition] = ReadBits(BitI, 4) + 5;
            if (FLAC->Data->Rice->RICEParameter[Partition] == 20) {
                // Escape code, meaning the partition is in unencoded binary form using n bits per sample; n follows as a 5-bit number.
            } else {
                if (FLAC->Data->LPC->PartitionOrder == 0) {
                    FLAC->Data->Rice->RICEParameter[Partition] = ReadBits(BitI, FLAC->Data->Frame->BlockSize - FLAC->Data->LPC->LPCOrder);
                } else if (FLAC->Data->LPC->PartitionOrder > 0) {
                    FLAC->Data->Rice->RICEParameter[Partition] = ReadBits(BitI, (FLAC->Data->Frame->BlockSize / pow(2, FLAC->Data->LPC->PartitionOrder)));
                } else {
                    FLAC->Data->Rice->RICEParameter[Partition] = ReadBits(BitI, (FLAC->Data->Frame->BlockSize / pow(2, FLAC->Data->LPC->PartitionOrder)) - FLAC->Data->LPC->LPCOrder);
                }
            }
        }
    }
    
    void FLACDecodeRice2Partition(BitInput *BitI, FLACDecoder *FLAC) {
        for (uint8_t Partition = 0; Partition < FLAC->Data->LPC->PartitionOrder; Partition++) {
            FLAC->Data->Rice->RICEParameter[Partition] = ReadBits(BitI, 5) + 5;
            if (FLAC->Data->Rice->RICEParameter[Partition] == 36) {
                // Escape code, meaning the partition is in unencoded binary form using n bits per sample; n follows as a 5-bit number.
            } else {
                if (FLAC->Data->LPC->PartitionOrder == 0) {
                    FLAC->Data->Rice->RICEParameter[Partition] = ReadBits(BitI, FLAC->Data->Frame->BlockSize - FLAC->Data->LPC->LPCOrder);
                } else if (FLAC->Data->LPC->PartitionOrder > 0) {
                    FLAC->Data->Rice->RICEParameter[Partition] = ReadBits(BitI, (FLAC->Data->Frame->BlockSize / pow(2, FLAC->Data->LPC->PartitionOrder)));
                } else {
                    FLAC->Data->Rice->RICEParameter[Partition] = ReadBits(BitI, (FLAC->Data->Frame->BlockSize / pow(2, FLAC->Data->LPC->PartitionOrder)) - FLAC->Data->LPC->LPCOrder);
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
        // I need 2 loops, one for the warmup samples, and one for the LPC encoded samples.
    }
    
#ifdef __cplusplus
}
#endif
