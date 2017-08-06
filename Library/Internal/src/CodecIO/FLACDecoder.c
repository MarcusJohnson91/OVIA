#include <stdio.h>
#include <math.h>

#include "../../include/libModernFLAC.h"
#include "../../include/Private/ModernFLACTypes.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    /*!
     @abstract Reads and decodes MIME Base64
     */
    void ReadBase64(BitBuffer *InputFLAC, uint8_t *Buffer, uint64_t BufferSize, uint64_t LineLength) {
        
    }
    
    
    /* Start User facing functions */
    
    /*!
     @abstract          Copies frames from the stream pointed to by InputFLAC, to OutputFrameBuffer (which needs to be freed by you)
     @param     StartFrame IS NOT zero indexed.
     */
    /*
    uint8_t *CopyFLACFrame(BitBuffer *InputFLAC, DecodeFLAC *Dec) { // for apps that don't care about metadata
        
        // scan stream for FrameMagic, once found, start counting until you hit StartFrame
        
        // See if there's a seektable, if so use that to get as close as possible, otherwise scan byte by byte.
        // Which means we need metadata flags.
        //
        for (size_t StreamByte = 0; StreamByte < InputFLAC->FileSize; StreamByte++) {
            uint16_t Marker = ReadBits(InputFLAC, 14, true);
            if (Marker == FLACFrameMagic) {
                // Get frame size by reading ahead until you find either the end of the stream, or another FLACFrameMagic
                // then skip back, and read it all.
                // OR could we somehow just read it until we got to the end of the frame, and
                size_t FrameSizeInBits = 0;
                while (ReadBits(InputFLAC, 14, true) != FLACFrameMagic || (InputFLAC->FilePosition + InputFLAC->BitsUnavailable) < InputFLAC->FileSize) {
                    FrameSizeInBits += 14;
                }
                SkipBits(InputFLAC, FrameSizeInBits);
                // Create buffer that's FrameSizeInBits, and then start copying each byte into the buffer
                uint8_t FrameBuffer[1];
                realloc(FrameBuffer, Bits2Bytes(FrameSizeInBits, true));
                for (size_t FrameByte = 0; FrameByte < Bits2Bytes(FrameSizeInBits, true); FrameByte++) {
                    FrameByte[FrameByte] = InputFLAC->Buffer[FrameByte];
                }
            }
        }
        
        return NULL;
    }
     */
    
    /* End User Facing Functions */
    
    void FLACReadStream(BitBuffer *InputFLAC, DecodeFLAC *Dec) {
        uint16_t Marker = PeekBits(InputFLAC, 14, true);
        if (Marker == FLACFrameMagic) {
            FLACReadFrame(InputFLAC, Dec);
        } else {
            FLACParseMetadata(InputFLAC, Dec);
        }
    }
    
    void FLACReadFrame(BitBuffer *InputFLAC, DecodeFLAC *Dec) {
        SkipBits(InputFLAC, 1); // 0
        Dec->Data->Frame->BlockType            = ReadBits(InputFLAC, 1, true); // 0 aka Fixed
        Dec->Data->Frame->CodedSamplesInBlock  = ReadBits(InputFLAC, 4, true); // 12 aka 4096
        if (((Dec->Data->Frame->CodedSamplesInBlock != 6) || (Dec->Data->Frame->CodedSamplesInBlock != 7))) {
            Dec->Data->Frame->BlockSize        = GetBlockSizeInSamples(Dec->Data->Frame->CodedSamplesInBlock); // SamplesInBlock
        }
        Dec->Data->Frame->CodedSampleRate      = ReadBits(InputFLAC, 4, true); // 9 aka 44100
        if ((Dec->Data->Frame->CodedSampleRate >= 0) && (Dec->Data->Frame->CodedSampleRate <= 11)) {
            FLACSampleRate(InputFLAC, Dec);
        }
        Dec->Data->Frame->ChannelLayout        = ReadBits(InputFLAC, 4, true) + 1; // 2
        Dec->Data->Frame->CodedBitDepth        = ReadBits(InputFLAC, 3, true); // 4 aka 16 bits per sample
        if (Dec->Data->Frame->CodedBitDepth != 0) {
            FLACBitDepth(Dec);
        }
        SkipBits(InputFLAC, 1); // 0
        
        if (Dec->Data->Frame->BlockType        == FixedBlockSize) { // variable blocktype
            Dec->Data->Frame->FrameNumber      = ReadBits(InputFLAC, 31, true); // 6,367,232
        } else if (Dec->Data->Frame->BlockType == VariableBlockSize) {
            Dec->Data->Frame->SampleNumber     = ReadBits(InputFLAC, 36, true);
        }
        
        if (Dec->Data->Frame->CodedSamplesInBlock == 6) {
            Dec->Data->Frame->BlockSize        = ReadBits(InputFLAC, 8, true); // SamplesInBlock
        } else if (Dec->Data->Frame->CodedSamplesInBlock == 7) {
            Dec->Data->Frame->BlockSize        = ReadBits(InputFLAC, 16, true); // SamplesInBlock
        }
        
        
        if (Dec->Data->Frame->CodedSampleRate == 12) {
            Dec->Data->Frame->SampleRate       = ReadBits(InputFLAC, 8, true) * 1000;
        } else if (Dec->Data->Frame->CodedSampleRate == 13) {
            Dec->Data->Frame->SampleRate       = ReadBits(InputFLAC, 16, true);
        } else if (Dec->Data->Frame->CodedSampleRate == 14) {
            Dec->Data->Frame->SampleRate       = ReadBits(InputFLAC, 16, true) * 10;
        }
        
        Dec->Data->Frame->FLACFrameCRC         = ReadBits(InputFLAC, 8, true); // CRC, 0x4
        
        for (uint8_t Channel = 0; Channel < Dec->Meta->StreamInfo->Channels; Channel++) { // read SubFrame
            FLACReadSubFrame(InputFLAC, Dec, Channel);
        }
    }
    
    void FLACReadSubFrame(BitBuffer *InputFLAC, DecodeFLAC *Dec, uint8_t Channel) {
        SkipBits(InputFLAC, 1); // Reserved
        Dec->Data->SubFrame->SubFrameType      = ReadBits(InputFLAC, 6, true); // 127
        if (Dec->Data->SubFrame->SubFrameType > 0) {
            Dec->Data->LPC->LPCOrder = (Dec->Data->SubFrame->SubFrameType & 0x1F) - 1; // 30
        }
        Dec->Data->SubFrame->WastedBitsFlag    = ReadBits(InputFLAC, 1, true); // 1
        if (Dec->Data->SubFrame->WastedBitsFlag == true) {
            Dec->Data->SubFrame->WastedBits    = ReadRICE(InputFLAC, false, 0); // 11111 0 00000
        }
        
        if (Dec->Data->SubFrame->SubFrameType == Subframe_Verbatim) { // PCM
            FLACDecodeSubFrameVerbatim(InputFLAC, Dec);
        } else if (Dec->Data->SubFrame->SubFrameType == Subframe_Constant) {
            FLACDecodeSubFrameConstant(InputFLAC, Dec);
        } else if (Dec->Data->SubFrame->SubFrameType >= Subframe_Fixed && Dec->Data->SubFrame->SubFrameType <= Subframe_LPC) { // Fixed
            FLACDecodeSubFrameFixed(InputFLAC, Dec);
        } else if (Dec->Data->SubFrame->SubFrameType >= Subframe_LPC) { // LPC
            FLACDecodeSubFrameLPC(InputFLAC, Dec, Channel);
        } else {
            char Description[BitIOStringSize];
            snprintf(Description, BitIOStringSize, "Invalid Subframe type: %d", Dec->Data->SubFrame->SubFrameType);
            Log(LOG_ERR, "ModernFLAC", "FLACReadSubframe", Description);
        }
    }
    
    void FLACDecodeSubFrameVerbatim(BitBuffer *InputFLAC, DecodeFLAC *Dec) {
        for (uint16_t Sample = 0; Sample < Dec->Data->Frame->BlockSize; Sample++) {
            Dec->DecodedSamples[Sample] = ReadBits(InputFLAC, Dec->Data->Frame->BitDepth, true);
        }
    }
    
    void FLACDecodeSubFrameConstant(BitBuffer *InputFLAC, DecodeFLAC *Dec) {
        int64_t Constant = ReadBits(InputFLAC, Dec->Data->Frame->BitDepth, true);
        memset(Dec->DecodedSamples, Constant, Dec->Data->Frame->BlockSize);
    }
    
    void FLACDecodeSubFrameFixed(BitBuffer *InputFLAC, DecodeFLAC *Dec) {
        for (uint16_t WarmupSample = 0; WarmupSample < Dec->Data->Frame->BitDepth * Dec->Data->LPC->LPCOrder; WarmupSample++) {
            Dec->DecodedSamples[WarmupSample]  = ReadBits(InputFLAC, Dec->Data->Frame->BitDepth, true);
        }
        DecodeFLACesidual(InputFLAC, Dec);
    }
    
    void FLACDecodeSubFrameLPC(BitBuffer *InputFLAC, DecodeFLAC *Dec, uint8_t Channel) { // 4 0's
        for (uint16_t WarmupSample = 0; WarmupSample < Dec->Data->Frame->BitDepth * Dec->Data->LPC->LPCOrder; WarmupSample++) { // 16 * 30= 480 aka 960 bytes
            // 004F, FFB0, 004F, 
            Dec->DecodedSamples[WarmupSample]  = ReadBits(InputFLAC, Dec->Data->Frame->BitDepth, true);
        }
        
        Dec->Data->LPC->LPCPrecision           = ReadBits(InputFLAC, 4, true) + 1; // 0x1F aka 31
        Dec->Data->LPC->LPCShift               = ReadBits(InputFLAC, 5, true); // 0b01110 aka 14
        Dec->Data->LPC->NumLPCCoeffs           = Dec->Data->LPC->LPCPrecision * Dec->Data->LPC->LPCOrder;
        
        
        for (uint16_t LPCCoefficent = 0; LPCCoefficent < Dec->Data->LPC->NumLPCCoeffs; LPCCoefficent++) {
            Dec->Data->LPC->LPCCoeff[LPCCoefficent] = ReadBits(InputFLAC, Dec->Data->LPC->NumLPCCoeffs, true) + 1;
        }
        DecodeFLACesidual(InputFLAC, Dec);
    }
    
    void DecodeFLACesidual(BitBuffer *InputFLAC, DecodeFLAC *Dec) {
        Dec->Data->LPC->RicePartitionType      = ReadBits(InputFLAC, 2, true);
        if (Dec->Data->LPC->RicePartitionType == RICE1) {
            DecodeFLACice1Partition(InputFLAC, Dec);
        } else if (Dec->Data->LPC->RicePartitionType == RICE2) {
            DecodeFLACice2Partition(InputFLAC, Dec);
        }
    }
    
    void DecodeFLACice1Partition(BitBuffer *InputFLAC, DecodeFLAC *Dec) {
        Dec->Data->LPC->PartitionOrder = ReadBits(InputFLAC, 4, true);
        for (uint8_t Partition = 0; Partition < Dec->Data->LPC->PartitionOrder; Partition++) {
            Dec->Data->Rice->RICEParameter[Partition] = ReadBits(InputFLAC, 4, true) + 5;
            if (Dec->Data->Rice->RICEParameter[Partition] == 20) {
                // Escape code, meaning the partition is in unencoded binary form using n bits per sample; n follows as a 5-bit number.
            } else {
                if (Dec->Data->LPC->PartitionOrder == 0) {
                    Dec->Data->Rice->RICEParameter[Partition] = ReadBits(InputFLAC, Dec->Data->Frame->BlockSize - Dec->Data->LPC->LPCOrder, true);
                } else if (Dec->Data->LPC->PartitionOrder > 0) {
                    Dec->Data->Rice->RICEParameter[Partition] = ReadBits(InputFLAC, (Dec->Data->Frame->BlockSize / pow(2, Dec->Data->LPC->PartitionOrder)), true);
                } else {
                    Dec->Data->Rice->RICEParameter[Partition] = ReadBits(InputFLAC, (Dec->Data->Frame->BlockSize / pow(2, Dec->Data->LPC->PartitionOrder)) - Dec->Data->LPC->LPCOrder, true);
                }
            }
        }
    }
    
    void DecodeFLACice2Partition(BitBuffer *InputFLAC, DecodeFLAC *Dec) {
        for (uint8_t Partition = 0; Partition < Dec->Data->LPC->PartitionOrder; Partition++) {
            Dec->Data->Rice->RICEParameter[Partition] = ReadBits(InputFLAC, 5, true) + 5;
            if (Dec->Data->Rice->RICEParameter[Partition] == 36) {
                // Escape code, meaning the partition is in unencoded binary form using n bits per sample; n follows as a 5-bit number.
            } else {
                if (Dec->Data->LPC->PartitionOrder == 0) {
                    Dec->Data->Rice->RICEParameter[Partition] = ReadBits(InputFLAC, Dec->Data->Frame->BlockSize - Dec->Data->LPC->LPCOrder, true);
                } else if (Dec->Data->LPC->PartitionOrder > 0) {
                    Dec->Data->Rice->RICEParameter[Partition] = ReadBits(InputFLAC, (Dec->Data->Frame->BlockSize / pow(2, Dec->Data->LPC->PartitionOrder)), true);
                } else {
                    Dec->Data->Rice->RICEParameter[Partition] = ReadBits(InputFLAC, (Dec->Data->Frame->BlockSize / pow(2, Dec->Data->LPC->PartitionOrder)) - Dec->Data->LPC->LPCOrder, true);
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
    
    void FLACSampleRate(BitBuffer *InputFLAC, DecodeFLAC *Dec) {
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
    
    void FLACDecodeLPC(BitBuffer *InputFLAC, DecodeFLAC *Dec) {
        // Basically you use the warmup samples in Dec->DecodedSamples, along with the info in Dec->LPC to deocde the file by using summation.
        // LPC is lossy, which is why you use filters to reduce the size of the residual.
        
        // I need 2 loops, one for the warmup samples, and one for the LPC encoded samples.
        
        
        // Original algorithm: X^
    }
    
#ifdef __cplusplus
}
#endif
