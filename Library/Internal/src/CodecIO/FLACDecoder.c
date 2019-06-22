#include "../../include/Private/FLACCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    /*!
     @abstract Reads and OVIAs MIME Base64
     */
    void ReadBase64(BitBuffer *BitB, uint8_t *Buffer, uint64_t BufferSize, uint64_t LineLength) {
        
    }
    
    
    /* Start User facing functions */
    
    /*!
     @abstract          Copies frames from the stream pointed to by BitB, to OutputFrameBuffer (which needs to be freed by you)
     @param     StartFrame IS NOT zero indexed.
     */
    /*
     uint8_t *CopyFLACFrame(BitBuffer *BitB) { // for apps that don't care about metadata
     
     // scan stream for FrameMagic, once found, start counting until you hit StartFrame
     
     // See if there's a seektable, if so use that to get as close as possible, otherwise scan byte by byte.
     // Which means we need metadata flags.
     //
     for (size_t StreamByte = 0; StreamByte < BitB->FileSize; StreamByte++) {
     uint16_t Marker = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 14, true);
     if (Marker == FLACFrameMagic) {
     // Get frame size by reading ahead until you find either the end of the stream, or another FLACFrameMagic
     // then skip back, and read it all.
     // OR could we somehow just read it until we got to the end of the frame, and
     size_t FrameSizeInBits = 0;
     while (BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 14, true) != FLACFrameMagic || (BitB->FilePosition + BitB->BitsUnavailable) < BitB->FileSize) {
     FrameSizeInBits += 14;
     }
     BitBuffer_Seek(BitB, FrameSizeInBits);
     // Create buffer that's FrameSizeInBits, and then start copying each byte into the buffer
     uint8_t FrameBuffer[1];
     realloc(FrameBuffer, Bits2Bytes(FrameSizeInBits, RoundingType_Up));
     for (size_t FrameByte = 0; FrameByte < Bits2Bytes(FrameSizeInBits, true); FrameByte++) {
     FrameByte[FrameByte] = BitB->Buffer[FrameByte];
     }
     }
     }
     
     return NULL;
     }
     */
    
    /* End User Facing Functions */
    
    void OVIA_FLAC_Read(BitBuffer *BitB) {
        // Ok so here we start reading the file.
        if (Ovia != NULL && BitB != NULL) {
            uint32_t FileMarker = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 32);
            if (FileMarker == FLACMagic) {
                bool IsLastBlock = OVIA_FLAC_Parse_Blocks(Ovia, BitB);
                if (IsLastBlock) {
                    // Check for the FrameMarker
                    uint16_t FrameMarker = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 14);
                    if (FrameMarker == FLACFrameMagic) {
                        // Start reading the frames.
                        OVIA_FLAC_Stream_Read(Ovia, BitB);
                    }
                }
            } else {
                Log(Log_ERROR, __func__, U8("Not a FLAC file"));
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_Stream_Read(BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            uint16_t Marker = BitBuffer_PeekBits(BitB, MSByteFirst, LSBitFirst, 14);
            if (Marker == FLACFrameMagic) {
                BitBuffer_Seek(BitB, 14); //
                OVIA_FLAC_Frame_Read(Ovia, BitB);
            } else {
                OVIA_FLAC_Parse_Blocks(Ovia, BitB);
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_Frame_Read(BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            uint8_t Reserved                = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 2);
            if (Reserved == 0) {
                uint8_t CodedBlockSize      = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 4);
                uint8_t CodedSampleRate     = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 4);
                uint8_t ChannelLayout       = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 4) + 1;
                uint8_t CodedBitDepth       = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 3);
                
                OVIA_FLAC_Frame_SetBlockType(Ovia, Reserved);
                OVIA_FLAC_Frame_SetCodedBlockSize(Ovia, CodedBlockSize);
                OVIA_FLAC_Frame_SetCodedSampleRate(Ovia, CodedSampleRate);
                OVIA_FLAC_Frame_SetChannelLayout(Ovia, ChannelLayout);
                OVIA_FLAC_Frame_SetCodedBitDepth(Ovia, CodedBitDepth);
                
                if (OVIA_FLAC_Frame_GetBlockType(Ovia) == FixedBlockSize) {
                    uint8_t  Bytes2Read                 = 1 + BitBuffer_ReadUnary(BitB, MSByteFirst, LSBitFirst, CountUnary, StopBit_Zero);
                    uint64_t FrameNumber                = BitBuffer_ReadUTF8(BitB, Bytes2Read); // 0
                    OVIA_FLAC_Frame_SetFrameNumber(Ovia, FrameNumber);
                } else if (OVIA_FLAC_Frame_GetBlockType(Ovia) == VariableBlockSize) {
                    uint8_t  Bytes2Read                 = 1 + BitBuffer_ReadUnary(BitB, MSByteFirst, LSBitFirst, CountUnary, StopBit_Zero);
                    uint64_t SampleNumber               = BitBuffer_ReadUTF8(BitB, Bytes2Read);
                    OVIA_FLAC_Frame_SetSampleNumber(Ovia, SampleNumber);
                }
                
                if (OVIA_FLAC_Frame_GetCodedBlockSize(Ovia) == 6) {
                    OVIA_FLAC_Frame_SetBlockSize(Ovia, BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8));
                } else if (OVIA_FLAC_Frame_GetCodedBlockSize(Ovia) == 7) {
                    OVIA_FLAC_Frame_SetBlockSize(Ovia, BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 16));
                } else {
                    // What do we do?
                }
                
                if (OVIA_FLAC_Frame_GetCodedSampleRate(Ovia) == 12) {
                    OVIA_FLAC_Frame_SetSampleRate(Ovia, BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8) * 1000);
                } else if (OVIA_FLAC_Frame_GetCodedSampleRate(Ovia) == 13) {
                    OVIA_FLAC_Frame_SetSampleRate(Ovia, BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 16));
                } else if (OVIA_FLAC_Frame_GetCodedSampleRate(Ovia) == 14) {
                    OVIA_FLAC_Frame_SetSampleRate(Ovia, BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 16) * 10);
                } else {
                    // What do we do?
                }
                
                for (uint8_t Channel = 0; Channel < OVIA_GetNumChannels(Ovia); Channel++) { // 2 channels
                    OVIA_FLAC_SubFrame_Read(Ovia, BitB, Channel);
                }
            } else {
                Log(Log_ERROR, __func__, U8("BlockType %d is invalid"), Reserved);
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_SubFrame_Read(BitBuffer *BitB, uint8_t Channel) { // 2 channels
        if (Ovia != NULL && BitB != NULL) {
            BitBuffer_Seek(BitB, 1); // Reserved
            uint8_t SubFrameType = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 6);
            OVIA_FLAC_SubFrame_SetType(Ovia, SubFrameType);
            
            bool WastedBitsFlag = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 1);
            if (WastedBitsFlag == true) {
                uint8_t WastedBits = BitBuffer_ReadUnary(BitB, MSByteFirst, MSBitFirst, TruncatedCountUnary, StopBit_Zero);
                OVIA_FLAC_SubFrame_SetWastedBits(Ovia, WastedBitsFlag, WastedBits);
            }
            
            if (SubFrameType == Subframe_Verbatim) { // PCM
                OVIA_FLAC_SubFrame_Verbatim(Ovia, BitB);
            } else if (SubFrameType == Subframe_Constant) {
                OVIA_FLAC_SubFrame_Constant(Ovia, BitB);
            } else if (SubFrameType >= Subframe_Fixed && SubFrameType <= Subframe_LPC) { // Fixed
                OVIA_FLAC_SubFrame_Fixed(Ovia, BitB);
            } else if (SubFrameType >= Subframe_LPC) { // LPC
                OVIA_FLAC_SubFrame_LPC(Ovia, BitB, Channel);
            } else {
                Log(Log_ERROR, __func__, U8("Invalid Subframe type: %d"), SubFrameType);
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_SubFrame_Verbatim(BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            // First Raw  Sample L channel = 0x5E83
            // First Raw  Sample R channel = 0x7139
            // First FLAC Sample L channel = 0x5ED7, aka 87 more?
            uint64_t        NumSamples  = OVIA_FLAC_Frame_GetCodedBlockSize(Ovia);
            uint8_t         BitDepth    = Bits2Bytes(OVIA_GetBitDepth(Ovia), RoundingType_Up);
            uint8_t         NumChannels = OVIA_GetNumChannels(Ovia);
            Audio2DContainer *Audio       = OVIA_GetAudioContainerPointer(Ovia);
            Audio_Types     Type        = AudioContainer_GetType(Audio);
            if (Type == AudioType_Integer8) {
                uint8_t **Array         = (uint8_t**) AudioContainer_GetArray(Audio);
                for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                    for (uint16_t Sample = 0; Sample < NumSamples; Sample++) {
                        Array[Channel][Sample] = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, BitDepth);
                    }
                }
            } else if (Type == (AudioType_Signed | AudioType_Integer8)) {
                int8_t  **Array         = (int8_t**) AudioContainer_GetArray(Audio);
                for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                    for (uint16_t Sample = 0; Sample < NumSamples; Sample++) {
                        Array[Channel][Sample] = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, BitDepth);
                    }
                }
            } else if (Type == (AudioType_Unsigned | AudioType_Integer16)) {
                uint16_t **Array        = (uint16_t**) AudioContainer_GetArray(Audio);
                for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                    for (uint16_t Sample = 0; Sample < NumSamples; Sample++) {
                        Array[Channel][Sample] = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, BitDepth);
                    }
                }
            } else if (Type == (AudioType_Signed | AudioType_Integer16)) {
                int16_t  **Array        = (int16_t**) AudioContainer_GetArray(Audio);
                for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                    for (uint16_t Sample = 0; Sample < NumSamples; Sample++) {
                        Array[Channel][Sample] = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, BitDepth);
                    }
                }
            } else if (Type == (AudioType_Unsigned | AudioType_Integer32)) {
                uint32_t **Array        = (uint32_t**) AudioContainer_GetArray(Audio);
                for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                    for (uint16_t Sample = 0; Sample < NumSamples; Sample++) {
                        Array[Channel][Sample] = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, BitDepth);
                    }
                }
            } else if (Type == (AudioType_Signed | AudioType_Integer32)) {
                int32_t  **Array        = (int32_t**) AudioContainer_GetArray(Audio);
                for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                    for (uint16_t Sample = 0; Sample < NumSamples; Sample++) {
                        Array[Channel][Sample] = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, BitDepth);
                    }
                }
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_SubFrame_Constant(BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            uint64_t        NumSamples  = OVIA_FLAC_Frame_GetCodedBlockSize(Ovia);
            uint8_t         BitDepth    = Bits2Bytes(OVIA_GetBitDepth(Ovia), RoundingType_Up);
            uint8_t         NumChannels = OVIA_GetNumChannels(Ovia);
            Audio2DContainer *Audio       = OVIA_GetAudioContainerPointer(Ovia);
            Audio_Types     Type        = AudioContainer_GetType(Audio);
            int64_t         Constant    = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, Ovia, BitB_GetBitDepth(Ovia));
            
            if (Type == (AudioType_Unsigned | AudioType_Integer8)) {
                uint8_t **Array         = AudioContainer_GetArray(Audio);
                for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                    for (uint16_t Sample = 0; Sample < NumSamples; Sample++) {
                        Array[Channel][Sample] = Constant;
                    }
                }
            } else if (Type == (AudioType_Signed | AudioType_Integer8)) {
                int8_t  **Array         = AudioContainer_GetArray(Audio);
                for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                    for (uint16_t Sample = 0; Sample < NumSamples; Sample++) {
                        Array[Channel][Sample] = Constant;
                    }
                }
            } else if (Type == (AudioType_Unsigned | AudioType_Integer16)) {
                uint16_t **Array        = AudioContainer_GetArray(Audio);
                for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                    for (uint16_t Sample = 0; Sample < NumSamples; Sample++) {
                        Array[Channel][Sample] = Constant;
                    }
                }
            } else if (Type == (AudioType_Signed | AudioType_Integer16)) {
                int16_t  **Array        = AudioContainer_GetArray(Audio);
                for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                    for (uint16_t Sample = 0; Sample < NumSamples; Sample++) {
                        Array[Channel][Sample] = Constant;
                    }
                }
            } else if (Type == (AudioType_Unsigned | AudioType_Integer32)) {
                uint32_t **Array        = AudioContainer_GetArray(Audio);
                for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                    for (uint16_t Sample = 0; Sample < NumSamples; Sample++) {
                        Array[Channel][Sample] = Constant;
                    }
                }
            } else if (Type == (AudioType_Signed | AudioType_Integer32)) {
                int32_t  **Array        = AudioContainer_GetArray(Audio);
                for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                    for (uint16_t Sample = 0; Sample < NumSamples; Sample++) {
                        Array[Channel][Sample] = Constant;
                    }
                }
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_SubFrame_Fixed(BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            uint64_t        NumSamples  = OVIA_FLAC_Frame_GetCodedBlockSize(Ovia);
            uint8_t         BitDepth    = Bits2Bytes(OVIA_GetBitDepth(Ovia), RoundingType_Up);
            uint8_t         NumChannels = OVIA_GetNumChannels(Ovia);
            Audio2DContainer *Audio       = OVIA_GetAudioContainerPointer(Ovia);
            Audio_Types     Type        = AudioContainer_GetType(Audio);
            
            if (Type == (AudioType_Unsigned | AudioType_Integer8)) {
                uint8_t **Array         = AudioContainer_GetArray(Audio);
                for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                    for (uint16_t Sample = 0; Sample < NumSamples; Sample++) {
                        Array[Channel][Sample] = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, Ovia, BitB_GetBitDepth(Ovia));
                    }
                }
            } else if (Type == (AudioType_Signed | AudioType_Integer8)) {
                int8_t  **Array         = AudioContainer_GetArray(Audio);
                for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                    for (uint16_t Sample = 0; Sample < NumSamples; Sample++) {
                        Array[Channel][Sample] = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, Ovia, BitB_GetBitDepth(Ovia));
                    }
                }
            } else if (Type == (AudioType_Unsigned | AudioType_Integer16)) {
                uint16_t **Array        = AudioContainer_GetArray(Audio);
                for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                    for (uint16_t Sample = 0; Sample < NumSamples; Sample++) {
                        Array[Channel][Sample] = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, Ovia, BitB_GetBitDepth(Ovia));
                    }
                }
            } else if (Type == (AudioType_Signed | AudioType_Integer16)) {
                int16_t  **Array        = AudioContainer_GetArray(Audio);
                for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                    for (uint16_t Sample = 0; Sample < NumSamples; Sample++) {
                        Array[Channel][Sample] = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, Ovia, BitB_GetBitDepth(Ovia));
                    }
                }
            } else if (Type == (AudioType_Unsigned | AudioType_Integer32)) {
                uint32_t **Array        = AudioContainer_GetArray(Audio);
                for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                    for (uint16_t Sample = 0; Sample < NumSamples; Sample++) {
                        Array[Channel][Sample] = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, Ovia, BitB_GetBitDepth(Ovia));
                    }
                }
            } else if (Type == (AudioType_Signed | AudioType_Integer32)) {
                int32_t  **Array        = AudioContainer_GetArray(Audio);
                for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                    for (uint16_t Sample = 0; Sample < NumSamples; Sample++) {
                        Array[Channel][Sample] = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, Ovia, BitB_GetBitDepth(Ovia));
                    }
                }
            }
            OVIA_FLAC_Decode_Residual(Ovia, BitB);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_SubFrame_LPC(BitBuffer *BitB, uint8_t Channel) { // 4 0's
        if (Ovia != NULL && BitB != NULL) {
            uint64_t        NumSamples  = OVIA_FLAC_Frame_GetCodedBlockSize(Ovia);
            uint8_t         BitDepth    = Bits2Bytes(OVIA_GetBitDepth(Ovia), RoundingType_Up);
            uint8_t         NumChannels = OVIA_GetNumChannels(Ovia);
            Audio2DContainer *Audio       = OVIA_GetAudioContainerPointer(Ovia);
            Audio_Types     Type        = AudioContainer_GetType(Audio);
            
            if (Type == (AudioType_Unsigned | AudioType_Integer8)) {
                uint8_t **Array         = AudioContainer_GetArray(Audio);
                for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                    for (uint16_t Sample = 0; Sample < NumSamples; Sample++) {
                        Array[Channel][Sample] = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, Ovia, BitB_GetBitDepth(Ovia));
                    }
                }
            } else if (Type == (AudioType_Signed | AudioType_Integer8)) {
                int8_t  **Array         = AudioContainer_GetArray(Audio);
                for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                    for (uint16_t Sample = 0; Sample < NumSamples; Sample++) {
                        Array[Channel][Sample] = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, Ovia, BitB_GetBitDepth(Ovia));
                    }
                }
            } else if (Type == (AudioType_Unsigned | AudioType_Integer16)) {
                uint16_t **Array        = AudioContainer_GetArray(Audio);
                for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                    for (uint16_t Sample = 0; Sample < NumSamples; Sample++) {
                        Array[Channel][Sample] = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, Ovia, BitB_GetBitDepth(Ovia));
                    }
                }
            } else if (Type == (AudioType_Signed | AudioType_Integer16)) {
                int16_t  **Array        = AudioContainer_GetArray(Audio);
                for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                    for (uint16_t Sample = 0; Sample < NumSamples; Sample++) {
                        Array[Channel][Sample] = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, Ovia, BitB_GetBitDepth(Ovia));
                    }
                }
            } else if (Type == (AudioType_Unsigned | AudioType_Integer32)) {
                uint32_t **Array        = AudioContainer_GetArray(Audio);
                for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                    for (uint16_t Sample = 0; Sample < NumSamples; Sample++) {
                        Array[Channel][Sample] = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, Ovia, BitB_GetBitDepth(Ovia));
                    }
                }
            } else if (Type == (AudioType_Signed | AudioType_Integer32)) {
                int32_t  **Array        = AudioContainer_GetArray(Audio);
                for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                    for (uint16_t Sample = 0; Sample < OVIA_GetBitDepth(Ovia) * OVIA_FLAC_LPC_GetLPCOrder(Ovia); Sample++) {
                        Array[Channel][Sample] = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, Ovia, BitB_GetBitDepth(Ovia));
                    }
                }
            }
            
            OVIA_FLAC_LPC_SetLPCPrecision(Ovia, BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 4) + 1);
            OVIA_FLAC_LPC_SetLPCShift(Ovia, BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 5));
            OVIA_FLAC_LPC_SetNumLPCCoeffs(Ovia, OVIA_FLAC_LPC_GetLPCPrecision(Ovia) * OVIA_FLAC_LPC_GetLPCOrder(Ovia));
            
            for (uint16_t LPCCoefficent = 0; LPCCoefficent < OVIA_FLAC_LPC_GetNumLPCCoeffs(Ovia); LPCCoefficent++) {
                OVIA_FLAC_LPC_SetLPCCoeff(Ovia, LPCCoefficent, BitBuffer_ReadBits(MSByteFirst, MSBitFirst, Ovia, OVIA_FLAC_LPC_GetLPCPrecision(Ovia) + 1));
            }
            OVIA_FLAC_Decode_Residual(Ovia, BitB);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_Decode_Residual(BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            uint8_t RICEPartitionType = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 2);
            
            OVIA_FLAC_LPC_SetRICEPartitionType(Ovia, RICEPartitionType);
            if (RICEPartitionType == RICE1) {
                OVIA_FLAC_Decode_RICE1(Ovia, BitB);
            } else if (RICEPartitionType == RICE2) {
                OVIA_FLAC_Decode_RICE2(Ovia, BitB);
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_Decode_RICE1(BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            uint8_t PartitionOrder = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 4);
            OVIA_FLAC_LPC_SetRICEPartitionOrder(Ovia, PartitionOrder);
            
            for (uint8_t Partition = 0; Partition < PartitionOrder; Partition++) {
                uint8_t Parameter = 0;
                Parameter         = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 4) + 5;
                OVIA_FLAC_LPC_SetRICEParameter(Ovia, Partition, Parameter);
                if (Parameter == 20) {
                    // Escape code, meaning the partition is in unencoded binary form using n bits per sample; n follows as a 5-bit number.
                } else {
                    if (OVIA_FLAC_LPC_GetRICEPartitionOrder(Ovia) == 0) {
                        Parameter = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, Ovia, OVIA_GetBlockSize(Ovia) - OVIA_FLAC_LPC_GetLPCOrder(Ovia));
                        OVIA_FLAC_LPC_SetRICEParameter(Ovia, Partition, Parameter);
                    } else if (OVIA_FLAC_LPC_GetRICEPartitionOrder(Ovia) > 0) {
                        Parameter = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, (OVIA_GetBlockSize(Ovia) / pow(2, OVIA_FLAC_LPC_GetRICEPartitionOrder(Ovia))));
                        OVIA_FLAC_LPC_SetRICEParameter(Ovia, Partition, Parameter);
                    } else {
                        Parameter = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, (OVIA_GetBlockSize(Ovia) / pow(2, OVIA_FLAC_LPC_GetRICEPartitionOrder(Ovia) - OVIA_FLAC_LPC_GetLPCOrder(Ovia))));
                        OVIA_FLAC_LPC_SetRICEParameter(Ovia, Partition, Parameter);
                    }
                }
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_Decode_RICE2(BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            for (uint8_t Partition = 0; Partition < OVIA_FLAC_LPC_GetRICEPartitionOrder(Ovia); Partition++) {
                uint8_t Parameter = 0;
                Parameter         = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 5) + 5;
                
                OVIA_FLAC_LPC_SetRICEParameter(Ovia, Partition, Parameter);
                uint8_t RiceParameter = OVIA_FLAC_LPC_GetRICEParameter(Ovia, Partition);
                if (RiceParameter == 36) {
                    // Escape code, meaning the partition is in unencoded binary form using n bits per sample; n follows as a 5-bit number.
                } else {
                    if (OVIA_FLAC_LPC_GetRICEPartitionOrder(Ovia) == 0) {
                        Parameter = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, Ovia, BitB_GetBlockSize(Ovia) - OVIA_FLAC_LPC_GetLPCOrder(Ovia));
                        OVIA_FLAC_LPC_SetRICEParameter(Ovia, Partition, Parameter);
                    } else if (OVIA_FLAC_LPC_GetRICEPartitionOrder(Ovia) > 0) {
                        Parameter = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, Ovia, BitB_GetBlockSize(Ovia) / pow(2, OVIA_FLAC_LPC_GetRICEPartitionOrder(Ovia)));
                        OVIA_FLAC_LPC_SetRICEParameter(Ovia, Partition, Parameter);
                    } else {
                        Parameter = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, (OVIA_GetBlockSize(Ovia) / pow(2, OVIA_FLAC_LPC_GetRICEPartitionOrder(Ovia) - OVIA_FLAC_LPC_GetLPCOrder(Ovia))));
                        OVIA_FLAC_LPC_SetRICEParameter(Ovia, Partition, Parameter);
                    }
                }
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    uint8_t GetBlockSizeInSamples(uint8_t BlockSize) {
        uint16_t SamplesInBlock = 0;
        if (BlockSize == 1) {
            SamplesInBlock = 192;
        } else if (BlockSize >= 2 && BlockSize <= 5) {
            SamplesInBlock = (576 * pow(2, BlockSize - 2)); // 576/1152/2304/4608, pow(2, (BlockSize - 2))
        } else if (BlockSize == 6) {
            // get 8 bit block from the end of the header
        } else if (BlockSize == 7) {
            // get 16 bit block from the end of the header
        } else if (BlockSize >= 8 && BlockSize <= 15) {
            SamplesInBlock = (256 * pow(2, (BlockSize - 8))); // 256/512/1024/2048/4096/8192/16384/32768
                                                              // 256 * pow(2, 4)
                                                              // 256 * pow(2, 6) 256 * 64 = 16384
        } else {
            // Reserved
        }
        return SamplesInBlock;
    }
    
    void FLACOVIALPC(BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
        
        // Basically you use the warmup samples in Ovia->OVIAdSamples, along with the info in Ovia->LPC to deocde the file by using summation.
        // LPC is lossy, which is why you use filters to reduce the size of the residual.
        
        // I need 2 loops, one for the warmup samples, and one for the LPC encoded samples.
        
        
        // Original algorithm: X^
    }
    
    bool OVIA_FLAC_Parse_Blocks(BitBuffer *BitB) {
        bool IsLastBlock                     = false;
        if (Ovia != NULL && BitB != NULL) {
            IsLastBlock                      = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 1);  // 1
            uint8_t *PictureArray            = NULL;
            // Actual audio data starts at: 1056166
            if (IsLastBlock == false) {
                uint8_t  BlockType           = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 7);  // 1
                uint32_t BlockSize           = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 24); // 562
                switch (BlockType) {
                    case StreamInfo:
                        OVIA_FLAC_StreamInfo_Parse(Ovia, BitB);
                        break;
                    case Padding:
                        OVIA_FLAC_SkipPadding(Ovia, BitB, BlockSize);
                        break;
                    case Custom:
                        OVIA_FLAC_SkipCustom(Ovia, BitB, BlockSize);
                        break;
                    case SeekTable:
                        OVIA_FLAC_SeekTable_Read(Ovia, BitB, BlockSize);
                        break;
                    case VorbisComment:
                        OVIA_FLAC_Vorbis_Parse(Ovia, BitB);
                        break;
                    case Cuesheet:
                        OVIA_FLAC_CUE_Parse(Ovia, BitB);
                        break;
                    case Picture:
                        PictureArray = OVIA_FLAC_Pic_Read(Ovia, BitB);
                        break;
                    default:
                        Log(Log_ERROR, __func__, U8("Invalid Block Type: %d\n"), BlockType);
                        break;
                }
            } else {
                return IsLastBlock;
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
        return IsLastBlock;
    }
    
    void OVIA_FLAC_StreamInfo_Parse(BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            OVIA_FLAC_SetMinBlockSize(Ovia, BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 16));    // 4608
            OVIA_FLAC_SetMaxBlockSize(Ovia, BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 16));    // 4608
            OVIA_FLAC_SetMinFrameSize(Ovia, BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 24));    // 365
            OVIA_FLAC_SetMaxFrameSize(Ovia, BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 24));    // 15738
            OVIA_SetSampleRate(Ovia,        BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 20));    // 44100
            OVIA_SetNumChannels(Ovia,       BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 3) + 1); // 2
            OVIA_SetBitDepth(Ovia,          BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 5) + 1); // 16
            OVIA_SetNumSamples(Ovia,        BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 36));    // 0x000B0C508 = 11,584,776
            uint8_t *MD5 = calloc(16, sizeof(uint8_t));
            if (MD5 != NULL) {
                for (uint8_t MD5Byte = 0; MD5Byte < 16; MD5Byte++) { // 0x32540FAD8218FF34EC06836F389512DB
                    MD5[MD5Byte] = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8);
                }
                OVIA_FLAC_SetMD5(Ovia, MD5);
            } else {
                BitBuffer_Seek(BitB, 128);
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_SkipPadding(BitBuffer *BitB, uint32_t ChunkSize) { // 8192
        if (Ovia != NULL && BitB != NULL) {
            BitBuffer_Seek(BitB, Bytes2Bits(ChunkSize));
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_SkipCustom(BitBuffer *BitB, uint32_t ChunkSize) { // 134,775
        if (Ovia != NULL && BitB != NULL) {
            BitBuffer_Seek(BitB, Bytes2Bits(ChunkSize + 1));
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_SeekTable_Read(BitBuffer *BitB, uint32_t ChunkSize) { // 3528
        if (Ovia != NULL && BitB != NULL) {
            uint64_t NumSeekPoints = ChunkSize / 18; // 196
            OVIA_FLAC_SetNumSeekPoints(Ovia, NumSeekPoints);
            
            for (uint16_t SeekPoint = 0; SeekPoint < NumSeekPoints; SeekPoint++) {
                uint64_t Sample    = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 64);
                uint64_t Offset    = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 64);
                uint16_t FrameSize = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 16);
                OVIA_FLAC_Seek_SetSeekPoint(Ovia, SeekPoint, Sample, Offset, FrameSize);
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_Vorbis_Parse(BitBuffer *BitB) { // LITTLE ENDIAN, size = 562
        if (Ovia != NULL && BitB != NULL) {
            uint32_t VendorTagSize = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32); // 13
            UTF8    *VendorTag     = BitBuffer_ReadUTF8(BitB, VendorTagSize);
            OVIA_SetTag(Ovia, CreatingSoftware, VendorTag);
            
            uint32_t NumUserTags   = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
            for (uint32_t Comment = 0; Comment < NumUserTags; Comment++) {
                uint32_t  TagSize                          = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
                UTF8     *Tag                              = BitBuffer_ReadUTF8(BitB, TagSize);
                OVIA_SetTag(Ovia, UnknownTag, Tag);
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_CUE_Parse(BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            char *CatalogID = calloc(FLACMediaCatalogNumberSize, sizeof(char));
            for (uint8_t CatalogChar = 0; CatalogChar < FLACMediaCatalogNumberSize; CatalogChar++) {
                CatalogID[CatalogChar] = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8);
            }
            OVIA_FLAC_CUE_SetCatalogID(Ovia, CatalogID);
            OVIA_FLAC_CUE_SetLeadIn(Ovia, BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 64));
            OVIA_FLAC_CUE_SetIsCD(Ovia, BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 1));
            BitBuffer_Seek(BitB, 2071); // Reserved
            
            uint8_t NumTracks = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8);
            OVIA_FLAC_CUE_SetNumTracks(Ovia, NumTracks);
            
            for (uint8_t Track = 0; Track < NumTracks; Track++) {
                uint64_t  Offset      = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 64);
                uint8_t   ISRCSize    = BitBuffer_GetUTF8StringSize(BitB);
                UTF8     *ISRCString  = BitBuffer_ReadUTF8(BitB, ISRCSize);
                bool      IsAudio     = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 1);
                bool      PreEmphasis = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 1);
                
                OVIA_FLAC_CUE_SetTrack(Ovia, Offset, IsAudio, PreEmphasis, ISRCString);
                
                BitBuffer_Seek(BitB, 152); // Reserved, all 0
                BitBuffer_Seek(BitB, 8); // NumIndexPoints
            }
            /*
             Ovia->Meta->Cue->IndexOffset    = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 64);
             Ovia->Meta->Cue->IndexPointNum  = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8);
             */
            BitBuffer_Seek(BitB, 24); // Reserved
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    uint8_t *OVIA_FLAC_Pic_Read(BitBuffer *BitB) { // 1,047,358
        uint8_t *PictureBuffer = NULL;
        if (Ovia != NULL && BitB != NULL) {
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
                Log(Log_ERROR, __func__, U8("Couldn't allocate Picture Buffer"));
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
        return PictureBuffer;
    }
    
#ifdef __cplusplus
}
#endif
