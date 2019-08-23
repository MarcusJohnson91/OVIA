#include "../../include/Private/FLACCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void FLAC_ParseBlocks(void *Options, BitBuffer *BitB, Audio2DContainer *Audio) {
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
                LastMetadataBlock  = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 1);  // 0
                uint8_t  BlockType = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 7);  // 0
                uint32_t BlockSize = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 24); // 393
                switch (BlockType) {
                    case Block_StreamInfo:
                        FLAC_Parse_StreamInfo(FLAC, BitB);
                        break;
                    case Block_SeekTable:
                        FLAC_Parse_SeekTable(FLAC, BitB, BlockSize);
                        break;
                    case Block_Vorbis:
                        FLAC_Parse_Vorbis(FLAC, BitB);
                        break;
                    case Block_Cuesheet:
                        FLAC_CUE_Parse(FLAC, BitB);
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
                Marker             = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 14);
                FLAC_Frame_Read(FLAC, BitB, Audio);
            } while (LastMetadataBlock == Yes && Marker == FrameMagic);
        } else if (Options == NULL) {
            Log(Log_DEBUG, __func__, U8("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (Audio == NULL) {
            Log(Log_DEBUG, __func__, U8("Audio2DContainer Pointer is NULL"));
        }
    }
    
    void FLAC_Frame_Read(void *Options, BitBuffer *BitB, Audio2DContainer *Audio) {
        if (Options != NULL && BitB != NULL && Audio != NULL) {
            FLACOptions *FLAC                 = Options;
            uint8_t Reserved1                 = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 1); // 0
            if (Reserved1 == 0) {
                FLAC->Frame->BlockType        = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 1); // 0, Fixed
                FLAC->Frame->CodedBlockSize   = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 4); // 0b0110 aka 6
                FLAC->Frame->CodedSampleRate  = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 4); // 8
                FLAC->Frame->ChannelLayout    = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 4); // 8
                FLAC->Frame->CodedBitDepth    = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 3); // 6
                BitBuffer_Seek(BitB, 1); // Reserved2, should be 0
                uint8_t Frame_SampleIDSize    = 1 + BitBuffer_ReadUnary(BitB, MSByteFirst, MSBitFirst, CountUnary, 0); // 1
                BitBuffer_Seek(BitB, -Frame_SampleIDSize);
                if (FLAC->Frame->BlockType == BlockType_Fixed) {
                    UTF8 *FrameNumberString   = BitBuffer_ReadUTF8(BitB, Frame_SampleIDSize); // 1 byte
                    FLAC->Frame->FrameNumber  = UTF8_Decode(FrameNumberString); // Value = 0
                } else if (FLAC->Frame->BlockType == BlockType_Variable) {
                    UTF8 *SampleNumberString  = BitBuffer_ReadUTF8(BitB, Frame_SampleIDSize);
                    FLAC->Frame->SampleNumber = UTF8_Decode(SampleNumberString);
                }
                
                if (FLAC->Frame->CodedBlockSize == 6) {
                    FLAC->Frame->BlockSize    = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 8);
                } else if (FLAC->Frame->CodedBlockSize == 7) {
                    FLAC->Frame->BlockSize    = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 16);
                }
                
                if (FLAC->Frame->CodedSampleRate == 12) {
                    FLAC->Frame->SampleRate   = 1000 * BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8);
                } else if (FLAC->Frame->CodedSampleRate == 13) {
                    FLAC->Frame->SampleRate   = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 16);
                } else if (FLAC->Frame->CodedSampleRate == 14) {
                    FLAC->Frame->SampleRate   = 10 * BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 16);
                }
                
                uint8_t FrameHeaderCRC8       = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 8); // 0x8A
                
                for (uint8_t Channel = 0; Channel < FLAC->Frame->ChannelLayout; Channel++) { // 2 channels
                    FLAC_SubFrame_Read(FLAC, BitB, Audio, Channel); // Channel 0 Coeff = 12
                }
                
                uint16_t FrameCRC16             = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 16);
            } else {
                Log(Log_DEBUG, __func__, U8("BlockType %d is invalid"), FLAC->Frame->BlockType);
            }
        } else if (Options == NULL) {
            Log(Log_DEBUG, __func__, U8("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (Audio == NULL) {
            Log(Log_DEBUG, __func__, U8("Audio2DContainer Pointer is NULL"));
        }
    }
    
    void FLAC_SubFrame_Read(void *Options, BitBuffer *BitB, Audio2DContainer *Audio, uint8_t Channel) { // 2 channels
        if (Options != NULL && BitB != NULL && Audio != NULL) {
            FLACOptions *FLAC                          = Options;
            bool Reserved1                             = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 1); // 0
            if (Reserved1 == 0) {
                FLAC->Frame->Sub->SubFrameType         = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 6); // 0
                bool WastedBitsFlag                    = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 1); // 0
                if (WastedBitsFlag == Yes) {
                    FLAC->Frame->Sub->WastedBits       = 1 + BitBuffer_ReadUnary(BitB, MSByteFirst, MSBitFirst, TruncatedCountUnary, StopBit_One);
                }
                
                uint8_t  NumChannels                   = FLAC_GetNumChannels(FLAC); // 2
                uint32_t NumSamples                    = FLAC->Frame->SamplesInPartition;
                
                if (Audio2DContainer_GetType(Audio) == (AudioType_Unsigned | AudioType_Integer8)) {
                    uint8_t **Array                    = (uint8_t**) Audio2DContainer_GetArray(Audio);
                    
                } else if (Audio2DContainer_GetType(Audio) == (AudioType_Signed | AudioType_Integer8)) {
                    int8_t  **Array                    = (int8_t**) Audio2DContainer_GetArray(Audio);
                    
                } else if (Audio2DContainer_GetType(Audio) == (AudioType_Unsigned | AudioType_Integer16)) {
                    uint16_t **Array                   = (uint16_t**) Audio2DContainer_GetArray(Audio);
                    
                } else if (Audio2DContainer_GetType(Audio) == (AudioType_Signed | AudioType_Integer16)) {
                    int16_t  **Array                   = (int16_t**) Audio2DContainer_GetArray(Audio);
                    
                } else if (Audio2DContainer_GetType(Audio) == (AudioType_Unsigned | AudioType_Integer32)) {
                    uint32_t **Array                   = (uint32_t**) Audio2DContainer_GetArray(Audio);
                    
                } else if (Audio2DContainer_GetType(Audio) == (AudioType_Signed | AudioType_Integer32)) {
                    int32_t  **Array                   = (int32_t**) Audio2DContainer_GetArray(Audio);
                    
                    if (FLAC->Frame->Sub->SubFrameType == Subframe_Constant) {
                        int32_t Constant               = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, FLAC->Frame->BitDepth);
                        for (uint8_t Channel = 0; Channel < FLAC_GetNumChannels(FLAC); Channel++) {
                            for (uint16_t Sample = 0; Sample < FLAC_GetBlockSizeInSamples(FLAC); Sample++) {
                                Array[Channel][Sample] = Constant;
                            }
                        }
                    } else if (FLAC->Frame->Sub->SubFrameType == Subframe_Verbatim) {
                        for (uint8_t Channel = 0; Channel < FLAC_GetNumChannels(FLAC); Channel++) {
                            for (uint16_t Sample = 0; Sample < FLAC_GetBlockSizeInSamples(FLAC); Sample++) {
                                Array[Channel][Sample] = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, FLAC->Frame->BitDepth);
                            }
                        }
                    } else if (FLAC->Frame->Sub->SubFrameType >= 8 && FLAC->Frame->Sub->SubFrameType <= 15) { // Fixed
                        FLAC->Frame->Sub->LPCFilterOrder = FLAC->Frame->Sub->SubFrameType & 0x7;
                        uint16_t NumWarmUpSamples        = FLAC->Frame->CodedBitDepth * FLAC->Frame->Sub->LPCFilterOrder;
                        uint8_t  RICEPartitionType       = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 2);
                        FLAC_Decode_RICE(FLAC, BitB, RICEPartitionType);
                        for (uint8_t Channel = 0; Channel < FLAC_GetNumChannels(FLAC); Channel++) {
                            for (uint16_t Sample = 0; Sample < FLAC_GetBlockSizeInSamples(FLAC); Sample++) {
                                Array[Channel][Sample]   = Constant;
                            }
                        }
                    } else if (FLAC->Frame->Sub->SubFrameType >= 32 && FLAC->Frame->Sub->SubFrameType <= 63) { // LPC
                        FLAC->Frame->Sub->LPCFilterOrder = FLAC->Frame->Sub->SubFrameType & 0x1F; // 7
                        uint16_t NumWarmUpSamples        = FLAC->Frame->CodedBitDepth * FLAC->Frame->Sub->LPCFilterOrder; // 4 * 7 = 28
                        FLAC->Frame->Sub->LPCPrecision   = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 4) + 1; // 0 + 1?
                        FLAC->Frame->Sub->LPCShift       = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 5); // 0
                        FLAC->Frame->Sub->Coeff          = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, FLAC->Frame->Sub->LPCPrecision * FLAC->Frame->Sub->LPCFilterOrder); // 7 bits in the current byte, read 7 bits! value = 12
                        
                    }
                } else {
                    Log(Log_DEBUG, __func__, U8("Invalid Subframe type: %d"), FLAC->Frame->Sub->SubFrameType);
                }
            }
        } else if (Options == NULL) {
            Log(Log_DEBUG, __func__, U8("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (Audio == NULL) {
            Log(Log_DEBUG, __func__, U8("Audio2DContainer Pointer is NULL"));
        }
    }
    
    void FLAC_Read_Residual(void *Options, BitBuffer *BitB, Audio2DContainer *Audio) {
        if (Options != NULL && BitB != NULL && Audio != NULL) {
            FLACOptions *FLAC                 = Options;
            uint8_t  ResiducalCodingMethod    = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 2);
            uint8_t  PartitionOrder           = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 4);
            uint8_t  NumPartitions            = Exponentiate(2, PartitionOrder);
            uint8_t  RICEParameter            = 0;
            uint8_t  Partition                = 0;
            uint16_t NumSamples               = 0;
            if (ResiducalCodingMethod == 0) {
                RICEParameter                 = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 4);
                if (RICEParameter == 15) { // Escaped
                    Partition                 = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 5);
                }
            } else if (ResiducalCodingMethod == 1) {
                RICEParameter                 = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 5);
                if (RICEParameter == 31) { // Escaped
                    Partition                 = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 5);
                }
            } else {
                Log(Log_DEBUG, __func__, U8("Invalid Residual Coding Method %d"), ResiducalCodingMethod);
            }
            
            if (PartitionOrder == 0) {
                NumSamples                    = FLAC->Frame->BlockSize - FLAC->LPC->LPCOrder; // PredictorOrder
            } else if (PartitionOrder > 0) {
                NumSamples                    = FLAC->Frame->BlockSize / Exponentiate(2, PartitionOrder);
            } else {
                NumSamples                    = (FLAC->Frame->BlockSize / Exponentiate(2, PartitionOrder)) - FLAC->LPC->LPCOrder;
            }
        } else if (Options == NULL) {
            Log(Log_DEBUG, __func__, U8("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (Audio == NULL) {
            Log(Log_DEBUG, __func__, U8("Audio2DContainer Pointer is NULL"));
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
                PartitionOrder           = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 4);
                NumPartitions            = Exponentiate(2, PartitionOrder);
                
                Partitions               = calloc(NumPartitions, sizeof(uint16_t));
                if (Partitions != NULL) {
                    for (uint8_t Partition = 0; Partition < NumPartitions; Partition++) {
                        Parameter    = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 4);
                        if (Parameter == 15) {
                            Parameter    = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 5);
                        }
                    }
                }
            } else if (RICEPartitionType == RICE2) {
                PartitionOrder           = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 5);
                NumPartitions            = Exponentiate(2, PartitionOrder);
                
                Partitions               = calloc(NumPartitions, sizeof(uint16_t));
                if (Partitions != NULL) {
                    for (uint8_t Partition = 0; Partition < NumPartitions; Partition++) {
                        Parameter        = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 5);
                        if (Parameter == 31) {
                            Parameter    = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 5);
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
            Log(Log_DEBUG, __func__, U8("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    
    bool FLAC_Parse_Blocks(void *Options, BitBuffer *BitB) {
        FLACOptions *FLAC                    = Options;
        bool IsLastBlock                     = false;
        if (Options != NULL && BitB != NULL) {
            IsLastBlock                      = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 1);  // 1
            uint8_t *PictureArray            = NULL;
            // Actual audio data starts at: 1056166
            if (IsLastBlock == false) {
                BlockTypes BlockType         = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 7);  // 6
                uint32_t BlockSize           = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 24); // 865,236
                switch (BlockType) {
                    case Block_StreamInfo:
                        FLAC_Parse_StreamInfo(FLAC, BitB);
                        break;
                    case Block_Padding:
                        BitBuffer_Seek(BitB, Bytes2Bits(BlockSize));
                        break;
                    case Block_Custom:
                        BitBuffer_Seek(BitB, Bytes2Bits(BlockSize));
                        break;
                    case Block_SeekTable:
                        FLAC_Parse_SeekTable(FLAC, BitB, BlockSize);
                        break;
                    case Block_Vorbis:
                        FLAC_Parse_Vorbis(FLAC, BitB);
                        break;
                    case Block_Cuesheet:
                        FLAC_CUE_Parse(FLAC, BitB);
                        break;
                    case Block_Picture:
                        PictureArray = FLAC_Pic_Read(FLAC, BitB);
                        break;
                    default:
                        Log(Log_DEBUG, __func__, U8("Invalid Block Type: %d\n"), BlockType);
                        break;
                }
            } else {
                return IsLastBlock;
            }
        } else if (Options == NULL) {
            Log(Log_DEBUG, __func__, U8("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
        return IsLastBlock;
    }
    
    void FLAC_Parse_StreamInfo(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            FLACOptions *FLAC                  = Options;
            FLAC->StreamInfo->MinimumBlockSize = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 16); // 4096
            FLAC->StreamInfo->MaximumBlockSize = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 16); // 4096
            FLAC->StreamInfo->MinimumFrameSize = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 24); // 18
            FLAC->StreamInfo->MaximumFrameSize = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 24); // 15146
            FLAC->StreamInfo->CodedSampleRate  = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 20); // 192000
            FLAC->StreamInfo->CodedChannels    = 1 + BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 3); // 2
            FLAC->StreamInfo->CodedBitDepth    = 1 + BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 5); // 24
            FLAC->StreamInfo->SamplesInStream  = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 36);    // 428,342,094
            if (FLAC->StreamInfo->MD5 != NULL) {
                for (uint8_t MD5Byte = 0; MD5Byte < 16; MD5Byte++) { // 0xDC AE 07 EA E9 11 40 C1 F4 43 7B B0 72 18 9D 2E
                    FLAC->StreamInfo->MD5[MD5Byte] = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8);
                }
            } else {
                BitBuffer_Seek(BitB, 128);
            }
        } else if (Options == NULL) {
            Log(Log_DEBUG, __func__, U8("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void FLAC_Parse_SeekTable(void *Options, BitBuffer *BitB, uint32_t ChunkSize) { // 3528
        if (Options != NULL && BitB != NULL) {
            FLACOptions *FLAC                         = Options;
            FLAC->SeekPoints->NumSeekPoints           = ChunkSize / 10;
            for (uint16_t SeekPoint = 0; SeekPoint < FLAC->SeekPoints->NumSeekPoints; SeekPoint++) {
                FLAC->SeekPoints->SampleInTargetFrame = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 64);
                FLAC->SeekPoints->OffsetFrom1stSample = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 64);
                FLAC->SeekPoints->TargetFrameSize     = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 16);
            }
        } else if (Options == NULL) {
            Log(Log_DEBUG, __func__, U8("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void FLAC_Parse_Vorbis(void *Options, BitBuffer *BitB) { // LITTLE ENDIAN, size = 393
        if (Options != NULL && BitB != NULL) {
            uint32_t VendorTagSize = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32); // 32
            UTF8    *VendorTag     = BitBuffer_ReadUTF8(BitB, VendorTagSize); // reference libFLAC 1.3.2 20170101
            uint32_t NumUserTags   = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32); // 13
            for (uint32_t Comment = 0; Comment < NumUserTags; Comment++) {
                uint32_t  TagSize                          = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
                UTF8     *Tag                              = BitBuffer_ReadUTF8(BitB, TagSize);
                // 14; totaltracks=11
                // 12; totaldiscs=1
                // 13; BPM=00283 BPMb
                // 98; 6954756E 4E4F524D 3D303030 30303146 38203030 30303031 34322030 30303031 43453320 30303030 31344635 20303030 30393241 34203030 30303932 42342030 30303034 31434320 30303030 35314335 20303030 31324339 46203030 30313435 45460F
                // 15; 6974756E 65736761 706C6573 733D30
                //
            }
        } else if (Options == NULL) {
            Log(Log_DEBUG, __func__, U8("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void FLAC_CUE_Parse(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            FLACOptions *FLAC         = Options;
            uint64_t CatalogIDSize    = BitBuffer_GetUTF8StringSize(BitB);
            FLAC->CueSheet->CatalogID = BitBuffer_ReadUTF8(BitB, CatalogIDSize);
            FLAC->CueSheet->LeadIn    = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 64);
            FLAC->CueSheet->IsCD      = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 1);
            BitBuffer_Seek(BitB, 2071); // Reserved
            
            FLAC->CueSheet->NumTracks = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 8);
            
            for (uint8_t Track = 0; Track < FLAC->CueSheet->NumTracks; Track++) {
                FLAC->CueSheet->Offset[Track] = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 64);
                uint8_t   ISRCSize            = BitBuffer_GetUTF8StringSize(BitB);
                FLAC->CueSheet->ISRC          = BitBuffer_ReadUTF8(BitB, ISRCSize);
                FLAC->CueSheet->IsAudio       = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 1);
                FLAC->CueSheet->PreEmphasis   = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 1);
                
                BitBuffer_Seek(BitB, 152); // Reserved, all 0
                BitBuffer_Seek(BitB, 8); // NumIndexPoints
            }
            
             FLAC->CueSheet->IndexOffset    = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 64);
             FLAC->CueSheet->IndexPointNum  = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8);
            BitBuffer_Seek(BitB, 24); // Reserved
        } else if (Options == NULL) {
            Log(Log_DEBUG, __func__, U8("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    uint8_t *FLAC_Pic_Read(void *Options, BitBuffer *BitB) { // 1,047,358
        uint8_t *PictureBuffer = NULL;
        if (Options != NULL && BitB != NULL) {
            FLACOptions *FLAC           = Options;
            uint32_t PicType            = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 32); // 0
            uint32_t MIMESize           = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 32); // 9
            UTF8    *MIMEType           = BitBuffer_ReadUTF8(BitB, MIMESize); // image/png
            
            uint32_t PicDescriptionSize = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 32); // 0
            UTF8    *PicDescription     = BitBuffer_ReadUTF8(BitB, PicDescriptionSize); //
            
            uint32_t Width              = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 32); // 1200
            uint32_t Height             = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 32); // 1200
            uint32_t BitDepth           = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 32); // 24
            uint32_t ColorsUsed         = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 32); // 0
            uint32_t PicSize            = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 32); // 000FFB15 aka 1,047,317
                                                                                                 // Pop in the address of the start of the data, and skip over the data instead of buffering it.
                                                                                                 // Ok so allocate a buffer
            PictureBuffer               = calloc(PicSize, sizeof(uint8_t));
            if (PictureBuffer != NULL) {
                for (uint32_t Byte = 0; Byte < PicSize - 1; Byte++) {
                    PictureBuffer[Byte] = BitBuffer_ReadBits(MSByteFirst, MSByteFirst, BitB, 8);
                }
            } else {
                Log(Log_DEBUG, __func__, U8("Couldn't allocate Picture Buffer"));
            }
        } else if (Options == NULL) {
            Log(Log_DEBUG, __func__, U8("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
        return PictureBuffer;
    }
    
    static void RegisterDecoder_FLAC(OVIA *Ovia) {
        Ovia->NumDecoders                                 += 1;
        uint64_t DecoderIndex                              = Ovia->NumDecoders;
        Ovia->Decoders                                     = realloc(Ovia->Decoders, sizeof(OVIADecoder) * Ovia->NumDecoders);
        
        Ovia->Decoders[DecoderIndex].DecoderID             = CodecID_FLAC;
        Ovia->Decoders[DecoderIndex].MediaType             = MediaType_Audio2D;
        Ovia->Decoders[DecoderIndex].NumMagicIDs           = 1;
        Ovia->Decoders[DecoderIndex].MagicIDOffset[0]      = 0;
        Ovia->Decoders[DecoderIndex].MagicIDSize[0]        = 4;
        Ovia->Decoders[DecoderIndex].MagicID[0]            = (uint8_t[4]) {0x66, 0x4C, 0x61, 0x43};
        Ovia->Decoders[DecoderIndex].Function_Initialize   = FLACOptions_Init;
        Ovia->Decoders[DecoderIndex].Function_Parse        = FLAC_Parse_Blocks;
        Ovia->Decoders[DecoderIndex].Function_Decode       = NULL;
        Ovia->Decoders[DecoderIndex].Function_Deinitialize = FLACOptions_Deinit;
    }
    
    static OVIACodecRegistry Register_FLACDecoder = {
        .Function_RegisterEncoder[CodecID_FLAC - 1]   = RegisterDecoder_FLAC,
    };
    
#ifdef __cplusplus
}
#endif
