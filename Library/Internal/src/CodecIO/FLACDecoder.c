#include "../../include/Private/FLACCommon.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    void FLAC_ReadBlocks(void *Options, BitBuffer *BitB, Audio2DContainer *Audio) {
        if (Options != NULL && BitB != NULL && Audio != NULL) {
            /*
             Mandatory: FLAC Magic ID, StreamInfo header, FLACDATA.
             We've gotten here after the MAGICID has been parsed bare minimum.
             So, just check the byte for a
             
             So, read the highest bit, MSBit, as a bool to see if there are more blocks.
             then the the type n the remaining 7 bits
             then read the size of the block in the following 24 bits.
             */
            FLACOptions *FLAC      = Options;
            bool LastMetadataBlock = No;
            do {
                LastMetadataBlock  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 1);  // 0
                uint8_t  BlockType = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 7);  // 0
                uint32_t BlockSize = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 24); // 393
                switch (BlockType) {
                    case Block_StreamInfo:
                        FLAC_Read_StreamInfo(FLAC, BitB);
                        break;
                    case Block_SeekTable:
                        FLAC_Read_SeekTable(FLAC, BitB, BlockSize);
                        break;
                    case Block_Vorbis:
                        FLAC_Read_Vorbis(FLAC, BitB);
                        break;
                    case Block_Cuesheet:
                        FLAC_CUE_Read(FLAC, BitB);
                        break;
                    case Block_Picture:
                        FLAC_Pic_Read(FLAC, BitB);
                        break;
                    default:
                    case Block_Padding:
                    case Block_Custom:
                        BitBuffer_Seek(BitB, Bytes2Bits(BlockSize));
                        break;
                }
            } while (LastMetadataBlock == No);
            
            uint16_t Marker        = FrameMagic;
            
            do {
                Marker             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 14);
                FLAC_Frame_Read(FLAC, BitB, Audio);
            } while (LastMetadataBlock == Yes && Marker == FrameMagic);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (Audio == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Audio2DContainer Pointer is NULL"));
        }
    }
    
    static uint8_t FLAC_GetChannelLayoutFromFrame(uint8_t CodedChannelLayout) {
        return CodedChannelLayout + 1;
    }
    
    static uint32_t FLAC_GetSampleRateFromFrame(void *Options, uint8_t CodedSampleRate, uint16_t EndOfHeaderBits) {
        uint32_t SampleRate = 0; // Invalid
        if (Options != NULL) {
            if (CodedSampleRate == 0) {
                FLACOptions *FLAC = Options;
                SampleRate        = FLAC->StreamInfo->SampleRate;
            } else if (CodedSampleRate == 1) {
                SampleRate = 88200;
            } else if (CodedSampleRate == 2) {
                SampleRate = 176400;
            } else if (CodedSampleRate == 3) {
                SampleRate = 192000;
            } else if (CodedSampleRate == 4) {
                SampleRate = 8000;
            } else if (CodedSampleRate == 5) {
                SampleRate = 16000;
            } else if (CodedSampleRate == 6) {
                SampleRate = 22050;
            } else if (CodedSampleRate == 7) {
                SampleRate = 24000;
            } else if (CodedSampleRate == 8) {
                SampleRate = 32000;
            } else if (CodedSampleRate == 9) {
                SampleRate = 44100;
            } else if (CodedSampleRate == 10) {
                SampleRate = 48000;
            } else if (CodedSampleRate == 11) {
                SampleRate = 96000;
            } else if (CodedSampleRate == 12) {
                SampleRate = EndOfHeaderBits * 1000;
            } else if (CodedSampleRate == 13) {
                SampleRate = EndOfHeaderBits;
            } else if (CodedSampleRate == 14) {
                SampleRate = EndOfHeaderBits * 10;
            }
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("FLACOptions Pointer is NULL"));
        }
        return SampleRate;
    }
    
    static uint8_t FLAC_GetBitDepthFromFrame(void *Options, uint8_t CodedBitDepth) {
        uint8_t BitDepth = 0; // Invalid
        if (Options != NULL) {
            FLACOptions *FLAC = Options;
            if (CodedBitDepth == 0) {
                FLACOptions *FLAC = Options;
                BitDepth          = FLAC->StreamInfo->BitDepth;
            } else if (CodedBitDepth == 1) {
                BitDepth = 8;
            } else if (CodedBitDepth == 2) {
                BitDepth = 12;
            } else if (CodedBitDepth == 4) {
                BitDepth = 16;
            } else if (CodedBitDepth == 5) {
                BitDepth = 20;
            } else if (CodedBitDepth == 6) {
                BitDepth = 24;
            }
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("FLACOptions Pointer is NULL"));
        }
        return BitDepth;
    }
    
    static uint64_t FLAC_GetCodedNumber(BitBuffer *BitB, uint8_t CodedNumberSize) {
        uint64_t Value = 0ULL;
        if (BitB != NULL) {
            /*
             CodedNumberSize = 1: 7 bits;  0b0XXXXXXX
             CodedNumberSize = 2: 11 bits; 0b110XXXXX, 0b10XXXXXX
             CodedNumberSize = 3: 16 bits; 0b1110XXXX, 0b10XXXXXX, 0b10XXXXXX
             CodedNumberSize = 4: 21 bits; 0b11110XXX, 0b10XXXXXX, 0b10XXXXXX, 0b10XXXXXX
             CodedNumberSize = 5: 26 bits; 0b111110XX, 0b10XXXXXX, 0b10XXXXXX, 0b10XXXXXX, 0b10XXXXXX
             CodedNumberSize = 6: 31 bits; 0b1111110X, 0b10XXXXXX, 0b10XXXXXX, 0b10XXXXXX, 0b10XXXXXX, 0b10XXXXXX
             CodedNumberSize = 7: 36 bits; 0b11111110, 0b10XXXXXX, 0b10XXXXXX, 0b10XXXXXX, 0b10XXXXXX, 0b10XXXXXX, 0b10XXXXXX
             */
            if (CodedNumberSize == 1) {
                Value             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 7);
            } else {
                uint8_t Bits2Read = CodedNumberSize + 1;
                BitBuffer_Seek(BitB, Bits2Read);
                Value             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 8 - Bits2Read);
                Value            << (8 - Bits2Read);
                for (uint8_t Byte = 1; Byte < CodedNumberSize - 1; Byte++) {
                    BitBuffer_Seek(BitB, 2);
                    Value << 6;
                    Value |= BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 6);
                }
            }
        }
        return Value + 1;
    }
    
    void FLAC_Frame_Read(void *Options, BitBuffer *BitB, Audio2DContainer *Audio) {
        if (Options != NULL && BitB != NULL && Audio != NULL) {
            FLACOptions *FLAC                      = Options;
            uint8_t Reserved1                      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 1); // 0
            if (Reserved1 == 0) {
                FLAC->Frame->BlockType             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 1); // 0, Fixed
                FLAC->Frame->CodedBlockSize        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 4); // 0b1100 aka 12
                FLAC->Frame->CodedSampleRate       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 4); // 0b0011 aka 3
                FLAC->Frame->CodedChannelLayout    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 4); // 0b0000 aka 0
                FLAC->Frame->NumChannels           = FLAC_GetChannelLayoutFromFrame(FLAC->Frame->CodedChannelLayout); // 0b0000 aka 1 channel
                FLAC->Frame->CodedBitDepth         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 3); // 1
                FLAC->Frame->BitDepth              = FLAC_GetBitDepthFromFrame(Options, FLAC->Frame->CodedBitDepth); //  8 bits
                uint8_t Reserved2                  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 1); // 0
                if (Reserved2 == 0) {
                    uint8_t  CodedNumberSize       = BitBuffer_ReadUnary(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, UnaryType_Count, UnaryTerminator_Zero) + 1;
                    uint64_t CodedNumber           = FLAC_GetCodedNumber(BitB, CodedNumberSize);
                    
                    uint16_t HeaderBlockSize       = 0;
                    if (FLAC->Frame->CodedBlockSize == 6) {
                        HeaderBlockSize            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 8);
                    } else if (FLAC->Frame->CodedBlockSize == 7) {
                        HeaderBlockSize            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 16);
                    }
                    FLAC->Frame->BlockSize         = FLAC_GetBlockSizeInSamples(Options, HeaderBlockSize); // 4096
                    
                    if (FLAC->Frame->BlockType == BlockType_Fixed) {
                        FLAC->Frame->FrameNumber   = (uint32_t) CodedNumber * FLAC->Frame->BlockSize; // Framenumber * BlockSize
                    } else if (FLAC->Frame->BlockType == BlockType_Variable) {
                        FLAC->Frame->SampleNumber  = CodedNumber;
                    }
                    
                    uint16_t EndOfHeaderSampleRate = 0;
                    if (FLAC->Frame->CodedSampleRate == 12) {
                        EndOfHeaderSampleRate      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 8);
                    } else if (FLAC->Frame->CodedSampleRate >= 13 && FLAC->Frame->CodedSampleRate <= 14) {
                        EndOfHeaderSampleRate      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 16);
                    }
                    FLAC->Frame->SampleRate        = FLAC_GetSampleRateFromFrame(Options, FLAC->Frame->CodedSampleRate, EndOfHeaderSampleRate); // 192,000
                    
                    uint8_t FrameHeaderCRC8        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 8); // 0x90
                    
                    for (uint8_t Channel = 0; Channel < FLAC->Frame->NumChannels; Channel++) { // 2 channels
                        FLAC_SubFrame_Read(FLAC, BitB, Audio, Channel); // Channel 0 Coeff = 12
                    }
                    
                    uint16_t FrameCRC16            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 16);
                } else {
                    Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Reserved2 value is invalid"));
                }
            } else {
                Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BlockType %d is invalid"), FLAC->Frame->BlockType);
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (Audio == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Audio2DContainer Pointer is NULL"));
        }
    }
    
    static uint32_t FLAC_GetNumSamplesInPartition(uint8_t PartitionOrder, uint8_t PredictorOrder, uint16_t BlockSize) {
        uint32_t NumSamples = 0;
        if (PartitionOrder == 0) {
            NumSamples      = BlockSize - PredictorOrder;
        } else if (PartitionOrder > 0) {
            NumSamples      = (BlockSize / Exponentiate(2, PartitionOrder));
        } else {
            NumSamples      = (BlockSize / Exponentiate(2, PartitionOrder)) - PredictorOrder;
        }
        return NumSamples;
    }
    
    void FLAC_SubFrame_Read(void *Options, BitBuffer *BitB, Audio2DContainer *Audio, uint8_t Channel) { // 1 channels
        if (Options != NULL && BitB != NULL && Audio != NULL) {
            FLACOptions *FLAC                          = Options;
            bool Reserved1                             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 1); // 0
            if (Reserved1 == 0) {
                FLAC->Frame->Sub->SubFrameType         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 6); // 0b100111 = 39
                bool WastedBitsFlag                    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 1); // 0
                if (WastedBitsFlag == Yes) {
                    FLAC->Frame->Sub->WastedBits       = 1 + BitBuffer_ReadUnary(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, UnaryType_Truncated, UnaryTerminator_One);
                }
                
                uint8_t  NumChannels                   = FLAC->Frame->NumChannels;
                uint32_t NumSamples                    = FLAC->Frame->SamplesInPartition;
                
                if (FLAC->Frame->Sub->SubFrameType == Subframe_Constant) {
                    if ((Audio2DContainer_GetType(Audio) & AudioType_Integer8) == AudioType_Integer8) {
                        uint8_t **Array                = (uint8_t**) Audio2DContainer_GetArray(Audio);
                        
                        uint8_t Constant               = (uint8_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, FLAC->Frame->BitDepth);
                        for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                            for (uint16_t Sample = 0; Sample < FLAC->Frame->BlockSize; Sample++) {
                                Array[Channel][Sample] = Constant;
                            }
                        }
                    } else if ((Audio2DContainer_GetType(Audio) & AudioType_Integer16) == AudioType_Integer16) {
                        uint16_t **Array                = (uint16_t**) Audio2DContainer_GetArray(Audio);
                        
                        uint16_t Constant               = (uint16_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, FLAC->Frame->BitDepth);
                        for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                            for (uint16_t Sample = 0; Sample < FLAC->Frame->BlockSize; Sample++) {
                                Array[Channel][Sample] = Constant;
                            }
                        }
                    } else if ((Audio2DContainer_GetType(Audio) & AudioType_Integer32) == AudioType_Integer32) {
                        uint32_t **Array                = (uint32_t**) Audio2DContainer_GetArray(Audio);
                        
                        uint32_t Constant               = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, FLAC->Frame->BitDepth);
                        for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                            for (uint16_t Sample = 0; Sample < FLAC->Frame->BlockSize; Sample++) {
                                Array[Channel][Sample] = Constant;
                            }
                        }
                    }
                } else if (FLAC->Frame->Sub->SubFrameType == Subframe_Verbatim) {
                    if ((Audio2DContainer_GetType(Audio) & AudioType_Integer8) == AudioType_Integer8) {
                        uint8_t **Array                = (uint8_t**) Audio2DContainer_GetArray(Audio);
                        
                        for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                            for (uint16_t Sample = 0; Sample < FLAC->Frame->BlockSize; Sample++) {
                                Array[Channel][Sample] = (uint8_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, FLAC->Frame->BitDepth);
                            }
                        }
                    } else if ((Audio2DContainer_GetType(Audio) & AudioType_Integer16) == AudioType_Integer16) {
                        uint16_t **Array                = (uint16_t**) Audio2DContainer_GetArray(Audio);
                        
                        for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                            for (uint16_t Sample = 0; Sample < FLAC->Frame->BlockSize; Sample++) {
                                Array[Channel][Sample] = (uint16_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, FLAC->Frame->BitDepth);
                            }
                        }
                    } else if ((Audio2DContainer_GetType(Audio) & AudioType_Integer32) == AudioType_Integer32) {
                        uint32_t **Array                = (uint32_t**) Audio2DContainer_GetArray(Audio);
                        
                        for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                            for (uint16_t Sample = 0; Sample < FLAC->Frame->BlockSize; Sample++) {
                                Array[Channel][Sample] = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, FLAC->Frame->BitDepth);
                            }
                        }
                    }
                } else if (FLAC->Frame->Sub->SubFrameType >= 8 && FLAC->Frame->Sub->SubFrameType <= 15) { // Fixed
                    FLAC->Frame->Sub->LPCFilterOrder = FLAC->Frame->Sub->SubFrameType & 0x7;
                    uint16_t NumWarmUpSamples        = FLAC->Frame->CodedBitDepth * FLAC->Frame->Sub->LPCFilterOrder;
                    uint8_t  RICEPartitionType       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 2);
                    
                    if ((Audio2DContainer_GetType(Audio) & AudioType_Integer8) == AudioType_Integer8) {
                        uint8_t **Array                = (uint8_t**) Audio2DContainer_GetArray(Audio);
                        
                        for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                            for (uint16_t Sample = 0; Sample < FLAC->Frame->BlockSize; Sample++) {
                                Array[Channel][Sample] = (uint8_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, FLAC->Frame->BitDepth);
                            }
                        }
                    } else if ((Audio2DContainer_GetType(Audio) & AudioType_Integer16) == AudioType_Integer16) {
                        uint16_t **Array                = (uint16_t**) Audio2DContainer_GetArray(Audio);
                        
                        for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                            for (uint16_t Sample = 0; Sample < FLAC->Frame->BlockSize; Sample++) {
                                Array[Channel][Sample] = (uint16_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, FLAC->Frame->BitDepth);
                            }
                        }
                    } else if ((Audio2DContainer_GetType(Audio) & AudioType_Integer32) == AudioType_Integer32) {
                        uint32_t **Array                = (uint32_t**) Audio2DContainer_GetArray(Audio);
                        
                        for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                            for (uint16_t Sample = 0; Sample < FLAC->Frame->BlockSize; Sample++) {
                                Array[Channel][Sample] = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, FLAC->Frame->BitDepth);
                            }
                        }
                    }
                } else if (FLAC->Frame->Sub->SubFrameType >= 32 && FLAC->Frame->Sub->SubFrameType <= 63) { // LPC
                    FLAC->Frame->Sub->LPCFilterOrder         = (FLAC->Frame->Sub->SubFrameType & 0x1F) + 1; // 8
                    
                    if ((Audio2DContainer_GetType(Audio) & AudioType_Integer8) == AudioType_Integer8) { // Warmup Samples
                        uint8_t  **Array                     = (uint8_t**) Audio2DContainer_GetArray(Audio);
                        
                        for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                            for (uint8_t WarmupSample = 0; WarmupSample < FLAC->Frame->Sub->LPCFilterOrder; WarmupSample++) {
                                Array[Channel][WarmupSample] = (uint8_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, FLAC->Frame->BitDepth);
                                // C2, CD, B6, C4, 7A, 81, 80, 80
                            }
                        }
                    } else if ((Audio2DContainer_GetType(Audio) & AudioType_Integer16) == AudioType_Integer16) {
                        uint16_t **Array                     = (uint16_t**) Audio2DContainer_GetArray(Audio);
                        
                        for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                            for (uint8_t WarmupSample = 0; WarmupSample < FLAC->Frame->Sub->LPCFilterOrder; WarmupSample++) {
                                Array[Channel][WarmupSample] = (uint16_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, FLAC->Frame->BitDepth);
                            }
                        }
                    } else if ((Audio2DContainer_GetType(Audio) & AudioType_Integer32) == AudioType_Integer32) {
                        uint32_t **Array                     = (uint32_t**) Audio2DContainer_GetArray(Audio);
                        
                        for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                            for (uint8_t WarmupSample = 0; WarmupSample < FLAC->Frame->Sub->LPCFilterOrder; WarmupSample++) {
                                Array[Channel][WarmupSample] = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, FLAC->Frame->BitDepth);
                            }
                        }
                    }
                    
                    FLAC->Frame->Sub->LPCPrecision   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 4) + 1; // 6
                    FLAC->Frame->Sub->LPCShift       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 5); // 4 , 1 bit used from next byte
                    FLAC->Frame->Sub->NumCoeffs      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, FLAC->Frame->Sub->LPCFilterOrder); // Read 8 Coeffs
                    
                    // 11, 0, 30, 53, 13, 15, 56, 47, with 0b10 remaining
                    
                    if (FLAC->Frame->Sub->NumCoeffs <= 32) {
                        for (uint8_t Coeff = 0; Coeff < FLAC->Frame->Sub->NumCoeffs; Coeff++) {
                            FLAC->Frame->Sub->Coeffs[Coeff] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, FLAC->Frame->Sub->LPCPrecision);
                        }
                    } else {
                        Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("NumCoeffs: %u is invalid"), FLAC->Frame->Sub->NumCoeffs);
                    }
                    
                    FLAC->Frame->Sub->ResidualCoder  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 2); // 0b10 aka 2 aka RICE2
                    FLAC->Frame->Sub->PartitionOrder = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 4); // 0
                    NumSamples                       = FLAC_GetNumSamplesInPartition(FLAC->Frame->Sub->PartitionOrder, FLAC->LPC->LPCOrder, FLAC->Frame->BlockSize);
                    uint8_t  NumPartitions           = Exponentiate(2, FLAC->Frame->Sub->PartitionOrder);
                    uint8_t  Partition               = 0;
                    
                    if (FLAC->Frame->Sub->ResidualCoder == RICE1) {
                        FLAC->Frame->Sub->RICEParameter = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 4);
                        
                        if (FLAC->Frame->Sub->RICEParameter == 0xF) { // RAW samples
                            Partition                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 5);
                        } else {
                            
                        }
                    } else if (FLAC->Frame->Sub->ResidualCoder == RICE2) {
                        FLAC->Frame->Sub->RICEParameter = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 5);
                        if (FLAC->Frame->Sub->RICEParameter == 0x1F) { // RAW samples
                            Partition                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 5);
                        } else {
                            
                        }
                    } else {
                        Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ResidualCoder Coder: %u is invalid"), FLAC->Frame->Sub->ResidualCoder);
                    }
                    // FLAC_Read_Residual
                    
                    
                    
                    if ((Audio2DContainer_GetType(Audio) & AudioType_Integer8) == AudioType_Integer8) {
                        uint8_t **Array                = (uint8_t**) Audio2DContainer_GetArray(Audio);
                        
                        for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                            for (uint16_t Sample = 0; Sample < FLAC->Frame->Sub->LPCFilterOrder; Sample++) {
                                Array[Channel][Sample] = (uint8_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, FLAC->Frame->BitDepth);
                            }
                        }
                        
                        for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                            for (uint16_t Sample = 0; Sample < FLAC->Frame->BlockSize; Sample++) {
                                Array[Channel][Sample] = (uint8_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, FLAC->Frame->BitDepth);
                            }
                        }
                    } else if ((Audio2DContainer_GetType(Audio) & AudioType_Integer16) == AudioType_Integer16) {
                        uint16_t **Array                = (uint16_t**) Audio2DContainer_GetArray(Audio);
                        
                        for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                            for (uint16_t Sample = 0; Sample < FLAC->Frame->Sub->LPCFilterOrder; Sample++) {
                                Array[Channel][Sample] = (uint8_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, FLAC->Frame->BitDepth);
                            }
                        }
                        
                        for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                            for (uint16_t Sample = 0; Sample < FLAC->Frame->BlockSize; Sample++) {
                                Array[Channel][Sample] = (uint16_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, FLAC->Frame->BitDepth);
                            }
                        }
                    } else if ((Audio2DContainer_GetType(Audio) & AudioType_Integer32) == AudioType_Integer32) {
                        uint32_t **Array                = (uint32_t**) Audio2DContainer_GetArray(Audio);
                        
                        for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                            for (uint16_t Sample = 0; Sample < FLAC->Frame->Sub->LPCFilterOrder; Sample++) {
                                Array[Channel][Sample] = (uint8_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, FLAC->Frame->BitDepth);
                            }
                        }
                        
                        for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                            for (uint16_t Sample = 0; Sample < FLAC->Frame->BlockSize; Sample++) {
                                Array[Channel][Sample] = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, FLAC->Frame->BitDepth);
                            }
                        }
                    }
                }
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (Audio == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Audio2DContainer Pointer is NULL"));
        }
    }
    
    void FLAC_Read_Residual(void *Options, BitBuffer *BitB, Audio2DContainer *Audio) {
        if (Options != NULL && BitB != NULL && Audio != NULL) {
            FLACOptions *FLAC                 = Options;
            uint8_t  ResiducalCodingMethod    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 2);
            uint8_t  PartitionOrder           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 4);
            uint8_t  NumPartitions            = Exponentiate(2, PartitionOrder);
            uint8_t  RICEParameter            = 0;
            uint8_t  Partition                = 0;
            uint16_t NumSamples               = 0;
            if (ResiducalCodingMethod == 0) {
                RICEParameter                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 4);
                if (RICEParameter == 15) { // Escaped
                    Partition                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 5);
                }
            } else if (ResiducalCodingMethod == 1) {
                RICEParameter                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 5);
                if (RICEParameter == 31) { // Escaped
                    Partition                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 5);
                }
            } else {
                Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Invalid Residual Coding Method %d"), ResiducalCodingMethod);
            }
            
            if (PartitionOrder == 0) {
                NumSamples                    = FLAC->Frame->BlockSize - FLAC->LPC->LPCOrder; // PredictorOrder
            } else if (PartitionOrder > 0) {
                NumSamples                    = FLAC->Frame->BlockSize / Exponentiate(2, PartitionOrder);
            } else {
                NumSamples                    = (FLAC->Frame->BlockSize / Exponentiate(2, PartitionOrder)) - FLAC->LPC->LPCOrder;
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (Audio == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Audio2DContainer Pointer is NULL"));
        }
    }
    
    void FLAC_Decode_RICE(void *Options, BitBuffer *BitB, uint8_t RICEPartitionType) {
        if (Options != NULL && BitB != NULL) {
            FLACOptions *FLAC            = Options;
            uint8_t     PartitionOrder   = 0;
            uint16_t    NumPartitions    = 0;
            uint8_t     NumSamplesInPart = 0;
            uint8_t     Parameter        = 0;
            
            uint16_t   *Partitions       = NULL;
            
            if (RICEPartitionType == RICE1) {
                PartitionOrder           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 4);
                NumPartitions            = Exponentiate(2, PartitionOrder);
                
                Partitions               = calloc(NumPartitions, sizeof(uint16_t));
                if (Partitions != NULL) {
                    for (uint8_t Partition = 0; Partition < NumPartitions; Partition++) {
                        Parameter    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 4);
                        if (Parameter == 15) {
                            Parameter    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 5);
                        }
                    }
                }
            } else if (RICEPartitionType == RICE2) {
                PartitionOrder           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 5);
                NumPartitions            = Exponentiate(2, PartitionOrder);
                
                Partitions               = calloc(NumPartitions, sizeof(uint16_t));
                if (Partitions != NULL) {
                    for (uint8_t Partition = 0; Partition < NumPartitions; Partition++) {
                        Parameter        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 5);
                        if (Parameter == 31) {
                            Parameter    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 5);
                        }
                    }
                }
            }
            
            if (PartitionOrder == 0) {
                NumSamplesInPart         = FLAC->Frame->BlockSize - PartitionOrder; // Predictor Order not Partition Order
            } else if (PartitionOrder > 0) {
                NumSamplesInPart         = (FLAC->Frame->BlockSize / NumPartitions);
            } else {
                NumSamplesInPart         = (FLAC->Frame->BlockSize / NumPartitions) - PartitionOrder; // Predictor Order
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void FLAC_Read_StreamInfo(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            FLACOptions *FLAC                  = Options;
            FLAC->StreamInfo->MinimumBlockSize = (uint16_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16); // 4096
            FLAC->StreamInfo->MaximumBlockSize = (uint16_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16); // 4096
            FLAC->StreamInfo->MinimumFrameSize = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 24); // 18
            FLAC->StreamInfo->MaximumFrameSize = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 24); // 15146
            FLAC->StreamInfo->SampleRate       = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 20); // 192000
            FLAC->StreamInfo->CodedChannels    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 3) + 1; // 2
            FLAC->StreamInfo->BitDepth         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 5) + 1; // 24
            FLAC->StreamInfo->SamplesInStream  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 36);    // 428,342,094
            if (FLAC->StreamInfo->MD5 != NULL) {
                for (uint8_t MD5Byte = 0; MD5Byte < 16; MD5Byte++) { // 0xDC AE 07 EA E9 11 40 C1 F4 43 7B B0 72 18 9D 2E
                    FLAC->StreamInfo->MD5[MD5Byte] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
                }
            } else {
                BitBuffer_Seek(BitB, 128);
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void FLAC_Read_SeekTable(void *Options, BitBuffer *BitB, uint32_t ChunkSize) { // 3528
        if (Options != NULL && BitB != NULL) {
            FLACOptions *FLAC                         = Options;
            FLAC->SeekPoints->NumSeekPoints           = ChunkSize / 10;
            for (uint16_t SeekPoint = 0; SeekPoint < FLAC->SeekPoints->NumSeekPoints; SeekPoint++) {
                FLAC->SeekPoints->SampleInTargetFrame[SeekPoint] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 64);
                FLAC->SeekPoints->OffsetFrom1stSample[SeekPoint] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 64);
                FLAC->SeekPoints->TargetFrameSize[SeekPoint]     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16);
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void FLAC_Read_Vorbis(void *Options, BitBuffer *BitB) { // LITTLE ENDIAN, size = 393
        if (Options != NULL && BitB != NULL) {
            uint32_t VendorTagSize = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32); // 32
            UTF8    *VendorTag     = BitBuffer_ReadUTF8(BitB, VendorTagSize); // reference libFLAC 1.3.2 20170101
            uint32_t NumUserTags   = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32); // 13
            for (uint32_t Comment = 0; Comment < NumUserTags; Comment++) {
                uint32_t  TagSize                          = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
                UTF8     *Tag                              = BitBuffer_ReadUTF8(BitB, TagSize);
                // 14; totaltracks=11
                // 12; totaldiscs=1
                // 13; BPM=00283 BPMb
                // 98; 6954756E 4E4F524D 3D303030 30303146 38203030 30303031 34322030 30303031 43453320 30303030 31344635 20303030 30393241 34203030 30303932 42342030 30303034 31434320 30303030 35314335 20303030 31324339 46203030 30313435 45460F
                // 15; 6974756E 65736761 706C6573 733D30
                //
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void FLAC_CUE_Read(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            FLACOptions *FLAC         = Options;
            uint64_t CatalogIDSize    = BitBuffer_GetUTF8StringSize(BitB);
            FLAC->CueSheet->CatalogID = BitBuffer_ReadUTF8(BitB, CatalogIDSize);
            FLAC->CueSheet->LeadIn    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 64);
            FLAC->CueSheet->IsCD      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 1);
            BitBuffer_Seek(BitB, 2071); // Reserved
            
            FLAC->CueSheet->NumTracks = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 8);
            
            for (uint8_t Track = 0; Track < FLAC->CueSheet->NumTracks; Track++) {
                FLAC->CueSheet->Offset[Track]      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 64);
                uint8_t   ISRCSize                 = BitBuffer_GetUTF8StringSize(BitB);
                FLAC->CueSheet->ISRC[Track]        = BitBuffer_ReadUTF8(BitB, ISRCSize);
                FLAC->CueSheet->IsAudio[Track]     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
                FLAC->CueSheet->PreEmphasis[Track] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
                
                BitBuffer_Seek(BitB, 152); // Reserved, all 0
                BitBuffer_Seek(BitB, 8); // NumIndexPoints
            }
            
             FLAC->CueSheet->IndexOffset    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 64);
             FLAC->CueSheet->IndexPointNum  = (uint8_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
            BitBuffer_Seek(BitB, 24); // Reserved
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    uint8_t *FLAC_Pic_Read(void *Options, BitBuffer *BitB) { // 1,047,358
        uint8_t *PictureBuffer = NULL;
        if (Options != NULL && BitB != NULL) {
            FLACOptions *FLAC           = Options;
            uint32_t PicType            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32); // 0
            uint32_t MIMESize           = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32); // 9
            UTF8    *MIMEType           = BitBuffer_ReadUTF8(BitB, MIMESize); // image/png
            
            uint32_t PicDescriptionSize = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32); // 0
            UTF8    *PicDescription     = BitBuffer_ReadUTF8(BitB, PicDescriptionSize); //
            
            uint32_t Width              = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32); // 1200
            uint32_t Height             = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32); // 1200
            uint32_t BitDepth           = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32); // 24
            uint32_t ColorsUsed         = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32); // 0
            uint32_t PicSize            = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32); // 000FFB15 aka 1,047,317
                                                                                                 // Pop in the address of the start of the data, and skip over the data instead of buffering it.
                                                                                                 // Ok so allocate a buffer
            PictureBuffer               = calloc(PicSize, sizeof(uint8_t));
            if (PictureBuffer != NULL) {
                for (uint32_t Byte = 0; Byte < PicSize - 1; Byte++) {
                    PictureBuffer[Byte] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
                }
            } else {
                Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Couldn't allocate Picture Buffer"));
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
        return PictureBuffer;
    }
    
    static const OVIADecoder FLACDecoder = {
        .Function_Initialize   = FLACOptions_Init,
        .Function_Decode       = FLAC_Frame_Read,
        .Function_Read         = FLAC_ReadBlocks,
        .Function_Deinitialize = FLACOptions_Deinit,
        .MagicIDs              = &FLACMagicIDs,
        .MediaType             = MediaType_Audio2D,
        .DecoderID             = CodecID_FLAC,
    };
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
