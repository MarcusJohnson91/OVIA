#include "../../include/CodecIO/FLACCodec.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h" /* Included for Assertions */
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/TextIO/LogIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    void FLAC_Frame_Read(FLACOptions *Options, BitBuffer *BitB, Audio2DContainer *Audio) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        AssertIO(Audio != NULL);
        uint8_t Reserved1                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 1); // 0
        AssertIO(Reserved1 == 0);
        Options->Frame->BlockType        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 1); // 0, Fixed
        Options->Frame->CodedBlockSize   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 4); // 5
        Options->Frame->CodedSampleRate  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 4); // 9
        Options->Frame->ChannelLayout    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 4); // 10
        Options->Frame->CodedBitDepth    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 3); // 8
        
        BitBuffer_Seek(BitB, 1); // Reserved2, should be 0
        
        uint8_t Frame_SampleIDSize    = BitBuffer_ReadUnary(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, UnaryType_Natural, UnaryTerminator_Zero) + 1; // 1
        if (Options->Frame->BlockType == BlockType_Fixed) {
            Options->Frame->FrameNumber  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, Bytes2Bits(Frame_SampleIDSize));
        } else if (Options->Frame->BlockType == BlockType_Variable) {
            Options->Frame->SampleNumber = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, Bytes2Bits(Frame_SampleIDSize));
        }
        
        if (Options->Frame->CodedBlockSize == 6) {
            Options->Frame->BlockSize    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 8);
        } else if (Options->Frame->CodedBlockSize == 7) {
            Options->Frame->BlockSize    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16);
        }
        
        if (Options->Frame->CodedSampleRate == 12) {
            Options->Frame->SampleRate   = 1000 * BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        } else if (Options->Frame->CodedSampleRate == 13) {
            Options->Frame->SampleRate   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 16);
        } else if (Options->Frame->CodedSampleRate == 14) {
            Options->Frame->SampleRate   = 10 * BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 16);
        }
        BitBuffer_Seek(BitB, 8); // Skip Frame CRC8
        
        for (uint8_t Channel = 0; Channel < Options->Frame->ChannelLayout; Channel++) { // 2 channels
            FLAC_SubFrame_Read(Options, BitB, Audio, Channel); // Channel 0 Coeff = 12
        }
        
        BitBuffer_Seek(BitB, 16); // Skip frame CRC 16
    }
    
    static uint8_t FLAC_GetBitDepth(FLACOptions *Options) {
        AssertIO(Options != NULL);
        uint8_t BitDepth = 0;
        if (Options->StreamInfo->CodedBitDepth == 1) {
            BitDepth = 8;
        } else if (Options->StreamInfo->CodedBitDepth == 2) {
            BitDepth = 12;
        } else if (Options->StreamInfo->CodedBitDepth == 4) {
            BitDepth = 16;
        } else if (Options->StreamInfo->CodedBitDepth == 5) {
            BitDepth = 20;
        } else if (Options->StreamInfo->CodedBitDepth == 6) {
            BitDepth = 24;
        }
        return BitDepth;
    }
    
    void FLAC_Parse_Subframe_LPC(FLACOptions *Options, BitBuffer *BitB, Audio2DContainer *Audio, uint8_t NumWarmupSamples) { // NumWarmupSamples=20 Max = 32
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        AssertIO(Audio != NULL);
        MediaIO_AudioTypes Type  = Audio2DContainer_GetType(Audio);
        if ((Type & AudioType_Unsigned) == AudioType_Unsigned) {
            if ((Type & AudioType_Integer32) == AudioType_Integer32) {
                uint32_t **Array = (uint32_t**) Audio2DContainer_GetArray(Audio);
                
                for (uint8_t WarmupSample = 0; WarmupSample < NumWarmupSamples; WarmupSample++) { // NumWarmupSamples = 8 in FLAC?
                    Array[0][WarmupSample] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, FLAC_GetBitDepth(Options));
                    // 28FE 264D 22E31E2E18D013E3E59B19B35A6AEBE79F3371AABE6300B080E303120688888928CF172A
                    // 2E1E, 2B59, 28F9, 264D, 22E3, 1E2E, 18D0, 13E3
                }
            } else if ((Type & AudioType_Integer16) == AudioType_Integer16) {
                uint16_t **Array = (uint16_t**) Audio2DContainer_GetArray(Audio);
                
                for (uint8_t WarmupSample = 0; WarmupSample < NumWarmupSamples; WarmupSample++) {
                    Array[0][WarmupSample] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, FLAC_GetBitDepth(Options));
                }
            } else if ((Type & AudioType_Integer8) == AudioType_Integer8) {
                uint8_t **Array  = (uint8_t**)  Audio2DContainer_GetArray(Audio);
                
                for (uint8_t WarmupSample = 0; WarmupSample < NumWarmupSamples; WarmupSample++) {
                    Array[0][WarmupSample] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, FLAC_GetBitDepth(Options));
                }
            }
        } else if ((Type & AudioType_Signed) == AudioType_Signed) {
            if ((Type & AudioType_Integer32) == AudioType_Integer32) {
                int32_t **Array  = (int32_t**) Audio2DContainer_GetArray(Audio);
                
                for (uint8_t WarmupSample = 0; WarmupSample < NumWarmupSamples; WarmupSample++) {
                    Array[0][WarmupSample] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, FLAC_GetBitDepth(Options));
                }
            } else if ((Type & AudioType_Integer16) == AudioType_Integer16) {
                int16_t **Array  = (int16_t**) Audio2DContainer_GetArray(Audio);
                
                for (uint8_t WarmupSample = 0; WarmupSample < NumWarmupSamples; WarmupSample++) {
                    Array[0][WarmupSample] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, FLAC_GetBitDepth(Options));
                }
            } else if ((Type & AudioType_Integer8) == AudioType_Integer8) {
                int8_t **Array   = (int8_t**)  Audio2DContainer_GetArray(Audio);
                
                for (uint8_t WarmupSample = 0; WarmupSample < NumWarmupSamples; WarmupSample++) {
                    Array[0][WarmupSample] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, FLAC_GetBitDepth(Options));
                }
            }
        }
        
        // Read QLP COEF PRECISION
        Options->LPC->LPCPrecision = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 4) + 1; // 6
        
        // Read QLP Shift
        Options->LPC->QLevel = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 5); // 20
        
        // Read Coeffs, there's NumWarmupSamples Coeffs
        for (uint8_t Coeff = 0; Coeff < NumWarmupSamples; Coeff++) {
            Options->LPC->LPCCoeff[Coeff] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, Options->LPC->LPCPrecision);
            // There's 20 of these 6 bit values left aka 120 bits aka 15 bytes
            // 7 bits of 0x0D, 0x8D, 0xEA, 0x86, 0xFE, 0x75, 0xBF, 0x5E, 0xCC, 0x36, 0x49, 0x0B, 0xB1, 0x79, 0xD9
            /*
             0b00011011
             0b00011011
             0b11010101
             0b00001101
             0b11111100
             0b11101011
             0b01111110
             0b10111101
             0b10011000
             0b01101100
             0b10010010
             0b00010111
             0b01100010
             0b11110011
             0b10110010
             0b1000101 = Remaining (byte = 0x45, top bit used)
             */
            /*
             0b000110 = 6
             0b110001 = 49
             0b101111 = 47
             0b010101 = 21
             0b000011 = 3
             0b011111 = 31
             0b110011 = 51
             0b101011 = 43
             0b011111 = 31
             0b101011 = 43
             0b110110 = 54
             0b011000 = 24
             0b011011 = 27
             0b001001 = 9
             0b001000 = 8
             0b010111 = 23
             0b011000 = 24
             0b101111 = 47
             0b001110 = 14
             0b110010 = 50
             */
        }
        
        // Read Entropy Coding Mode = 2 bits
        Options->LPC->RicePartitionType  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 2);
        AssertIO(Options->LPC->RicePartitionType != CodingType_Reserved1);
        AssertIO(Options->LPC->RicePartitionType != CodingType_Reserved2);
        FLAC_Decode_RICE(Options, BitB, Options->LPC->RicePartitionType);
    }
    
    void FLAC_SubFrame_Read(FLACOptions *Options, BitBuffer *BitB, Audio2DContainer *Audio, uint8_t Channel) { // 2 channels
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        AssertIO(Audio != NULL);
        bool Reserved1                             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 1); // 0
        AssertIO(Reserved1 == 0);
        Options->Frame->Sub->SubFrameType         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 6); // 39
        bool WastedBitsFlag                    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 1); // 0
        if (WastedBitsFlag == Yes) {
            Options->Frame->Sub->WastedBits       = BitBuffer_ReadUnary(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, UnaryType_Truncated, UnaryTerminator_One) + 1;
        }
        
        uint8_t  NumChannels                   = FLAC_GetNumChannels(Options); // 2
        uint32_t NumSamples                    = Options->Frame->SamplesInPartition;
        
        if (Audio2DContainer_GetType(Audio) == (AudioType_Signed | AudioType_Integer32)) {
            int32_t  **Array                   = (int32_t**) Audio2DContainer_GetArray(Audio);
            
            if (Options->Frame->Sub->SubFrameType == Subframe_Constant) {
                int32_t Constant               = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, Options->Frame->BitDepth);
                for (uint8_t Channel = 0; Channel < FLAC_GetNumChannels(Options); Channel++) {
                    for (uint16_t Sample = 0; Sample < FLAC_GetBlockSizeInSamples(Options); Sample++) {
                        Array[Channel][Sample] = Constant;
                    }
                }
            } else if (Options->Frame->Sub->SubFrameType == Subframe_Verbatim) {
                for (uint8_t Channel = 0; Channel < FLAC_GetNumChannels(Options); Channel++) {
                    for (uint16_t Sample = 0; Sample < FLAC_GetBlockSizeInSamples(Options); Sample++) {
                        Array[Channel][Sample] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, Options->Frame->BitDepth);
                    }
                }
            } else if (Options->Frame->Sub->SubFrameType <= Subframe_Fixed) { // Fixed
                Options->Frame->Sub->LPCFilterOrder = Options->Frame->Sub->SubFrameType & 0x7;
                uint16_t NumWarmUpSamples        = Options->Frame->CodedBitDepth * Options->Frame->Sub->LPCFilterOrder;
                uint8_t  RICEPartitionType       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 2);
                FLAC_Decode_RICE(Options, BitB, RICEPartitionType);
                for (uint8_t Channel = 0; Channel < FLAC_GetNumChannels(Options); Channel++) {
                    for (uint16_t Sample = 0; Sample < FLAC_GetBlockSizeInSamples(Options); Sample++) {
                        Array[Channel][Sample]   = Constant;
                    }
                }
            } else if (Options->Frame->Sub->SubFrameType <= Subframe_LPC) { // LPC
                FLAC_Parse_Subframe_LPC(Options, BitB, Audio, ((Options->Frame->Sub->SubFrameType >> 1) & 0x1F) + 1);
                
                Options->Frame->Sub->LPCFilterOrder = Options->Frame->Sub->SubFrameType & 0x1F; // 7
                uint16_t NumWarmUpSamples        = Options->Frame->CodedBitDepth * Options->Frame->Sub->LPCFilterOrder; // 8 * 7 = 56
                Options->Frame->Sub->LPCPrecision   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 4) + 1; // 2 + 1?
                Options->Frame->Sub->LPCShift       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 5); // 28
                Options->Frame->Sub->Coeff          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, Options->Frame->Sub->LPCPrecision * Options->Frame->Sub->LPCFilterOrder); // 2 or 3 * 7 = 14 or 21 = 0xF2B or 0x
                                                                                                                                                                                                               // 0x1E | (0x2B >> 1 = 0x15) | (0x59 >> 1 = 0x2C) = 0xF15 or 0x78AAC
                
            }
        }
    }
    
    void FLAC_Read_Residual(FLACOptions *Options, BitBuffer *BitB, Audio2DContainer *Audio) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        AssertIO(Audio != NULL);
        uint8_t  ResiducalCodingMethod    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 2);
        uint8_t  PartitionOrder           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 4);
        uint8_t  NumPartitions            = Exponentiate(2, PartitionOrder);
        uint8_t  RICEParameter            = 0;
        uint8_t  Partition                = 0;
        uint16_t NumSamples               = 0;
        AssertIO(ResiducalCodingMethod != CodingType_Reserved1);
        AssertIO(ResiducalCodingMethod != CodingType_Reserved2);
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
        }
        
        if (PartitionOrder == 0) {
            NumSamples                    = Options->Frame->BlockSize - Options->LPC->LPCOrder; // PredictorOrder
        } else if (PartitionOrder > 0) {
            NumSamples                    = Options->Frame->BlockSize / Exponentiate(2, PartitionOrder);
        } else {
            NumSamples                    = (Options->Frame->BlockSize / Exponentiate(2, PartitionOrder)) - Options->LPC->LPCOrder;
        }
    }
    
    void FLAC_Decode_RICE(FLACOptions *Options, BitBuffer *BitB, uint8_t RICEPartitionType) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        uint8_t     PartitionOrder   = 0;
        uint16_t    NumPartitions    = 0;
        uint8_t     NumSamplesInPart = 0;
        uint8_t     Parameter        = 0;
        
        uint16_t   *Partitions       = NULL;
        
        if (RICEPartitionType == CodingType_4Bit) {
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
        } else if (RICEPartitionType == CodingType_5Bit) {
            PartitionOrder           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 5); // 17
            NumPartitions            = Exponentiate(2, PartitionOrder);
            
            Partitions               = calloc(NumPartitions, sizeof(uint8_t));
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
            NumSamplesInPart         = Options->Frame->BlockSize - PartitionOrder; // Predictor Order not Partition Order
        } else if (PartitionOrder > 0) {
            NumSamplesInPart         = (Options->Frame->BlockSize / NumPartitions);
        } else {
            NumSamplesInPart         = (Options->Frame->BlockSize / NumPartitions) - PartitionOrder; // Predictor Order
        }
    }
    
    
    bool FLAC_Parse_Blocks(FLACOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        bool IsLastBlock                     = false;
        IsLastBlock                      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);  // 1
        uint8_t *PictureArray            = NULL;
        // Actual audio data starts at: 1056166
        if (IsLastBlock == false) {
            FLAC_BlockTypes BlockType         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 7);  // 6
            uint32_t BlockSize           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 24); // 865,236
            switch (BlockType) {
                case BlockType_StreamInfo:
                    FLAC_Parse_StreamInfo(Options, BitB);
                    break;
                case BlockType_Padding:
                    BitBuffer_Seek(BitB, Bytes2Bits(BlockSize));
                    break;
                case BlockType_Custom:
                    BitBuffer_Seek(BitB, Bytes2Bits(BlockSize));
                    break;
                case BlockType_SeekTable:
                    FLAC_Parse_SeekTable(Options, BitB, BlockSize);
                    break;
                case BlockType_Vorbis:
                    FLAC_Parse_Vorbis(Options, BitB);
                    break;
                case BlockType_Cuesheet:
                    FLAC_CUE_Parse(Options, BitB);
                    break;
                case BlockType_Picture:
                    PictureArray = FLAC_Pic_Read(Options, BitB);
                    break;
                default:
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Invalid Block Type: %d\n"), BlockType);
                    break;
            }
        } else {
            return IsLastBlock;
        }
        return IsLastBlock;
    }
    
    void FLAC_Parse_StreamInfo(FLACOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->StreamInfo->MinimumBlockSize = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16); // 4096
        Options->StreamInfo->MaximumBlockSize = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16); // 4096
        Options->StreamInfo->MinimumFrameSize = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 24); // 18
        Options->StreamInfo->MaximumFrameSize = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 24); // 15146
        Options->StreamInfo->CodedSampleRate  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 20); // 192000
        Options->StreamInfo->CodedChannels    = 1 + BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 3); // 2
        Options->StreamInfo->CodedBitDepth    = 1 + BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 5); // 24
        Options->StreamInfo->SamplesInStream  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 36);    // 428,342,094
        if (Options->StreamInfo->MD5 != NULL) {
            for (uint8_t MD5Byte = 0; MD5Byte < 16; MD5Byte++) { // 0xDC AE 07 EA E9 11 40 C1 F4 43 7B B0 72 18 9D 2E
                Options->StreamInfo->MD5[MD5Byte] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
            }
        } else {
            BitBuffer_Seek(BitB, 128);
        }
    }
    
    void FLAC_Parse_SeekTable(FLACOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) { // 3528
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SeekPoints->NumSeekPoints           = ChunkSize / 10;
        for (uint16_t SeekPoint = 0; SeekPoint < Options->SeekPoints->NumSeekPoints; SeekPoint++) {
            Options->SeekPoints->SampleInTargetFrame[SeekPoint] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 64);
            Options->SeekPoints->OffsetFrom1stSample[SeekPoint] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 64);
            Options->SeekPoints->TargetFrameSize[SeekPoint]     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16);
        }
    }
    
    void FLAC_Parse_Vorbis(FLACOptions *Options, BitBuffer *BitB) { // LITTLE ENDIAN, size = 393
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        uint32_t VendorTagSize = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32); // 32
        UTF8    *VendorTag     = BitBuffer_ReadUTF8(BitB, VendorTagSize); // reference libFLAC 1.3.2 20170101
        uint32_t NumUserTags   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32); // 13
        for (uint32_t Comment = 0; Comment < NumUserTags; Comment++) {
            uint32_t  TagSize                          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
            UTF8     *Tag                              = BitBuffer_ReadUTF8(BitB, TagSize);
            // 14; totaltracks=11
            // 12; totaldiscs=1
            // 13; BPM=00283 BPMb
            // 98; 6954756E 4E4F524D 3D303030 30303146 38203030 30303031 34322030 30303031 43453320 30303030 31344635 20303030 30393241 34203030 30303932 42342030 30303034 31434320 30303030 35314335 20303030 31324339 46203030 30313435 45460F
            // 15; 6974756E 65736761 706C6573 733D30
            //
        }
    }
    
    void FLAC_CUE_Parse(FLACOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        uint64_t CatalogIDSize    = BitBuffer_GetUTF8StringSize(BitB);
        Options->CueSheet->CatalogID = BitBuffer_ReadUTF8(BitB, CatalogIDSize);
        Options->CueSheet->LeadIn    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 64);
        Options->CueSheet->IsCD      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 1);
        BitBuffer_Seek(BitB, 2071); // Reserved
        
        Options->CueSheet->NumTracks = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 8);
        
        for (uint8_t Track = 0; Track < Options->CueSheet->NumTracks; Track++) {
            Options->CueSheet->Offset[Track] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 64);
            uint8_t   ISRCSize            = BitBuffer_GetUTF8StringSize(BitB);
            Options->CueSheet->ISRC          = BitBuffer_ReadUTF8(BitB, ISRCSize);
            Options->CueSheet->IsAudio       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
            Options->CueSheet->PreEmphasis   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
            
            BitBuffer_Seek(BitB, 152); // Reserved, all 0
            BitBuffer_Seek(BitB, 8); // NumIndexPoints
        }
        
        Options->CueSheet->IndexOffset    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 64);
        Options->CueSheet->IndexPointNum  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        BitBuffer_Seek(BitB, 24); // Reserved
    }
    
    uint8_t *FLAC_Pic_Read(FLACOptions *Options, BitBuffer *BitB) { // 1,047,358
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        uint8_t *PictureBuffer = NULL;
        
        uint32_t PicType            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32); // 0
        uint32_t MIMESize           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32); // 9
        UTF8    *MIMEType           = BitBuffer_ReadUTF8(BitB, MIMESize); // image/png
        
        uint32_t PicDescriptionSize = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32); // 0
        UTF8    *PicDescription     = BitBuffer_ReadUTF8(BitB, PicDescriptionSize); //
        
        uint32_t Width              = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32); // 1200
        uint32_t Height             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32); // 1200
        uint32_t BitDepth           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32); // 24
        uint32_t ColorsUsed         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32); // 0
        uint32_t PicSize            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32); // 000FFB15 aka 1,047,317
                                                                                                                         // Pop in the address of the start of the data, and skip over the data instead of buffering it.
                                                                                                                         // Ok so allocate a buffer
        PictureBuffer               = calloc(PicSize, sizeof(uint8_t));
        AssertIO(PictureBuffer != NULL);
        for (uint32_t Byte = 0; Byte < PicSize - 1; Byte++) {
            PictureBuffer[Byte] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, ByteOrder_LSByteIsFarthest, 8);
        }
        return PictureBuffer;
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
