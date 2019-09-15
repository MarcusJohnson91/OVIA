#include "../../include/Private/PNMCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    static uint64_t PNMCheckForComment(BitBuffer *BitB) {
        uint64_t CommentSize = 0;
        if (BitB != NULL) {
            if (BitBuffer_PeekBits(BitB, MSByteFirst, LSBitFirst, 8) == PNMCommentStart) {
                BitBuffer_Seek(BitB, 8);
                do {
                    CommentSize += 1;
                } while (BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8) != PNMEndField);
            }
        } else {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
        return CommentSize;
    }
    
    static void PNMParse_ASCII(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNMOptions *PNM           = Options; // Cast the void pointer Options to PNMOptions so that the warnings in the registration functions disappear
            uint64_t CommentSizeWidth = PNMCheckForComment(BitB);
            BitBuffer_Seek(BitB, Bytes2Bits(CommentSizeWidth));
            /* Read Width */
            uint64_t WidthStringSize  = BitBuffer_GetUTF8StringSize(BitB);
            UTF8    *WidthString      = BitBuffer_ReadUTF8(BitB, WidthStringSize);
            PNM->Width                = UTF8_String2Integer(Base_Decimal_Radix10, WidthString);
            free(WidthString);
            /* Read Width */
            
            uint64_t CommentSizeHeight = PNMCheckForComment(BitB);
            BitBuffer_Seek(BitB, Bytes2Bits(CommentSizeHeight));
            
            /* Read Height */
            uint64_t HeightStringSize = BitBuffer_GetUTF8StringSize(BitB);
            UTF8    *HeightString     = BitBuffer_ReadUTF8(BitB, HeightStringSize);
            PNM->Height               = UTF8_String2Integer(Base_Decimal_Radix10, HeightString);
            free(HeightString);
            /* Read Height */
        } else if (Options == NULL) {
            Log(Log_DEBUG, __func__, U8("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void PNMParse_Binary(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNMOptions *PNM           = Options; // Cast the void pointer Options to PNMOptions so that the warnings in the registration functions disappear
            /* Read Width */
            uint64_t WidthStringSize  = BitBuffer_GetUTF8StringSize(BitB);
            UTF8    *WidthString      = BitBuffer_ReadUTF8(BitB, WidthStringSize);
            PNM->Width                = UTF8_String2Integer(Base_Decimal_Radix10, WidthString);
            free(WidthString);
            /* Read Width */
            
            /* Read Height */
            uint64_t HeightStringSize = BitBuffer_GetUTF8StringSize(BitB);
            UTF8    *HeightString     = BitBuffer_ReadUTF8(BitB, HeightStringSize);
            PNM->Height               = UTF8_String2Integer(Base_Decimal_Radix10, HeightString);
            free(HeightString);
            /* Read Height */
            
            /* Read MaxVal */
            uint64_t MaxValStringSize = BitBuffer_GetUTF8StringSize(BitB);
            UTF8    *MaxValString     = BitBuffer_ReadUTF8(BitB, MaxValStringSize);
            uint64_t MaxVal           = UTF8_String2Integer(Base_Decimal_Radix10, MaxValString);
            PNM->BitDepth             = Logarithm(2, MaxVal + 1);
            free(MaxValString);
            /* Read MaxVal */
        } else if (Options == NULL) {
            Log(Log_DEBUG, __func__, U8("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void PNMParse_PAM(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNMOptions *PNM           = Options;  // Cast the void pointer Options to PNMOptions so that the warnings in the registration functions disappear
            /* Read Width */
            BitBuffer_Seek(BitB, 48); // Skip "WIDTH " string
            
            uint64_t WidthStringSize  = BitBuffer_GetUTF8StringSize(BitB);
            UTF8    *WidthString      = BitBuffer_ReadUTF8(BitB, WidthStringSize);
            PNM->Width                = UTF8_String2Integer(Base_Decimal_Radix10, WidthString);
            free(WidthString);
            /* Read Width */
            
            /* Read Height */
            BitBuffer_Seek(BitB, 56); // Skip "HEIGHT " string
            
            uint64_t HeightStringSize = BitBuffer_GetUTF8StringSize(BitB);
            UTF8    *HeightString     = BitBuffer_ReadUTF8(BitB, HeightStringSize);
            PNM->Height               = UTF8_String2Integer(Base_Decimal_Radix10, HeightString);
            free(HeightString);
            /* Read Height */
            
            /* Read NumChannels */
            BitBuffer_Seek(BitB, 48); // Skip "DEPTH " string
            
            uint64_t NumChannelsStringSize  = BitBuffer_GetUTF8StringSize(BitB);
            UTF8    *NumChannelsString      = BitBuffer_ReadUTF8(BitB, NumChannelsStringSize);
            PNM->NumChannels                = UTF8_String2Integer(Base_Decimal_Radix10, NumChannelsString);
            free(NumChannelsString);
            /* Read NumChannels */
            
            /* Read MaxVal */
            BitBuffer_Seek(BitB, 56); // Skip "MAXVAL " string
            
            uint64_t MaxValStringSize = BitBuffer_GetUTF8StringSize(BitB);
            UTF8    *MaxValString     = BitBuffer_ReadUTF8(BitB, MaxValStringSize);
            uint64_t MaxVal           = UTF8_String2Integer(Base_Decimal_Radix10, MaxValString);
            PNM->BitDepth             = Logarithm(2, MaxVal + 1);
            free(MaxValString);
            /* Read MaxVal */
            
            /* Read TupleType */
            BitBuffer_Seek(BitB, 72); // Skip "TUPLETYPE " string
            uint64_t TupleTypeSize     = BitBuffer_GetUTF8StringSize(BitB);
            UTF8    *TupleTypeString   = BitBuffer_ReadUTF8(BitB, TupleTypeSize);
            
            if (UTF8_CompareSubString(TupleTypeString, U8("BLACKANDWHITE"), 0, 0) == true) {
                PNM->NumChannels       = 1;
                PNM->TupleType         = PNM_TUPLE_BnW;
            } else if (UTF8_CompareSubString(TupleTypeString, U8("GRAYSCALE"), 0, 0) == true) {
                PNM->NumChannels       = 1;
                PNM->TupleType         = PNM_TUPLE_Gray;
            } else if (UTF8_CompareSubString(TupleTypeString, U8("GRAYSCALE_ALPHA"), 0, 0) == true) {
                PNM->NumChannels       = 2;
                PNM->TupleType         = PNM_TUPLE_GrayAlpha;
            } else if (UTF8_CompareSubString(TupleTypeString, U8("RGB"), 0, 0) == true) {
                PNM->NumChannels       = 3;
                PNM->TupleType         = PNM_TUPLE_RGB;
            } else if (UTF8_CompareSubString(TupleTypeString, U8("RGB_ALPHA"), 0, 0) == true) {
                PNM->NumChannels       = 4;
                PNM->TupleType         = PNM_TUPLE_RGBAlpha;
            } else {
                Log(Log_DEBUG, __func__, U8("Unknown PNM Tuple: %s"), TupleTypeString);
            }
            free(TupleTypeString);
            /* Read TupleType */
            
            /* Skip ENDHDR */
            BitBuffer_Seek(BitB, 56); // ENDHDR
            /* Skip ENDHDR */
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNMExtractImage_ASCII(void *Options, BitBuffer *BitB, ImageContainer *Image) {
        if (Options != NULL && BitB != NULL && Image != NULL) {
            PNMOptions *PNM              = Options;
            if (PNM->BitDepth <= 8) {
                uint8_t ****Array        = (uint8_t****) ImageContainer_GetArray(Image);
                UTF8        Component[4] = {0, 0, 0, 0};
                for (uint64_t Width = 0; Width < PNM->Width; Width++) {
                    for (uint64_t Height = 0; Height < PNM->Height; Height++) {
                        for (uint8_t Channel = 0; Channel < PNM->NumChannels; Channel++) {
                            for (uint8_t SubPixelByte = 0; SubPixelByte < 3; SubPixelByte++) {
                                Component[SubPixelByte]      = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8);
                            }
                            Array[0][Width][Height][Channel] = UTF8_String2Integer(Base_Decimal_Radix10, Component);
                        }
                    }
                }
            } else if (PNM->BitDepth <= 16) {
                uint16_t ****Array       = (uint16_t****) ImageContainer_GetArray(Image);
                UTF8        Component[6] = {0, 0, 0, 0, 0, 0};
                for (uint64_t Width = 0; Width < PNM->Width; Width++) {
                    for (uint64_t Height = 0; Height < PNM->Height; Height++) {
                        for (uint8_t Channel = 0; Channel < PNM->NumChannels; Channel++) {
                            for (uint8_t SubPixelByte = 0; SubPixelByte < 3; SubPixelByte++) {
                                Component[SubPixelByte]      = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8);
                            }
                            Array[0][Width][Height][Channel] = UTF8_String2Integer(Base_Decimal_Radix10, Component);
                        }
                    }
                }
            } else {
                Log(Log_DEBUG, __func__, U8("16 bit ASCII PNM is invalid"));
            }
        } else if (Options == NULL) {
            Log(Log_DEBUG, __func__, U8("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (Image == NULL) {
            Log(Log_DEBUG, __func__, U8("ImageContainer Pointer is NULL"));
        }
    }
    
    void PNMExtractImage_Binary(void *Options, BitBuffer *BitB, ImageContainer *Image) {
        if (Options != NULL && BitB != NULL && Image != NULL) {
            PNMOptions *PNM        = Options; 
            if (PNM->BitDepth <= 8) {
                uint8_t ****Array  = (uint8_t****) ImageContainer_GetArray(Image);
                
                for (uint64_t Width = 0; Width < PNM->Width; Width++) {
                    for (uint64_t Height = 0; Height < PNM->Height; Height++) {
                        for (uint8_t Channel = 0; Channel < PNM->NumChannels; Channel++) {
                            uint8_t CurrentPixel                 = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, PNM->BitDepth);
                            if (PNM->TupleType == PNM_TUPLE_BnW) {
                                Array[0][Width][Height][Channel] = ~CurrentPixel; // 1 = black, 0 = white
                            } else {
                                Array[0][Width][Height][Channel] = CurrentPixel; // 1 = white, 0 = black
                            }
                        }
                    }
                }
            } else if (PNM->BitDepth <= 16) {
                uint16_t ****Array  = (uint16_t****) ImageContainer_GetArray(Image);
                
                for (uint64_t Width = 0; Width < PNM->Width; Width++) {
                    for (uint64_t Height = 0; Height < PNM->Height; Height++) {
                        for (uint8_t Channel = 0; Channel < PNM->NumChannels; Channel++) {
                            uint16_t CurrentPixel                = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, PNM->BitDepth);
                            if (PNM->TupleType == PNM_TUPLE_BnW) {
                                Array[0][Width][Height][Channel] = ~CurrentPixel; // 1 = black, 0 = white
                            } else {
                                Array[0][Width][Height][Channel] = CurrentPixel; // 1 = white, 0 = black
                            }
                        }
                    }
                }
            }
        } else if (Options == NULL) {
            Log(Log_DEBUG, __func__, U8("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (Image == NULL) {
            Log(Log_DEBUG, __func__, U8("ImageContainer Pointer is NULL"));
        }
    }
    
    static void RegisterDecoder_PNM(OVIA *Ovia) {
        Ovia->NumDecoders                                    += 1;
        uint64_t DecoderIndex                                 = Ovia->NumDecoders - 1;
        Ovia->Decoders                                        = realloc(Ovia->Decoders, sizeof(OVIADecoder) * Ovia->NumDecoders);
        
        Ovia->Decoders[DecoderIndex].DecoderID                = CodecID_PNM;
        Ovia->Decoders[DecoderIndex].MediaType                = MediaType_Image;
        Ovia->Decoders[DecoderIndex].NumMagicIDs              = 6;
        
        /* MagicID 0 */
        Ovia->Decoders[DecoderIndex].MagicIDOffset[0]         = 0;
        Ovia->Decoders[DecoderIndex].MagicIDSize[0]           = 2;
        Ovia->Decoders[DecoderIndex].MagicID[0]               = (uint8_t[2]) {0x50, 0x31};
        Ovia->Decoders[DecoderIndex].Function_Initialize[0]   = PNMOptions_Init;
        Ovia->Decoders[DecoderIndex].Function_Parse[0]        = PNMParse_ASCII;
        Ovia->Decoders[DecoderIndex].Function_Decode[0]       = PNMExtractImage_ASCII;
        Ovia->Decoders[DecoderIndex].Function_Deinitialize[0] = PNMOptions_Deinit;
        /* MagicID 0 */
        
        /* MagicID 1 */
        Ovia->Decoders[DecoderIndex].MagicIDOffset[1]         = 0;
        Ovia->Decoders[DecoderIndex].MagicIDSize[1]           = 2;
        Ovia->Decoders[DecoderIndex].MagicID[1]               = (uint8_t[2]) {0x50, 0x32};
        Ovia->Decoders[DecoderIndex].Function_Initialize[1]   = PNMOptions_Init;
        Ovia->Decoders[DecoderIndex].Function_Parse[1]        = PNMParse_ASCII;
        Ovia->Decoders[DecoderIndex].Function_Decode[1]       = PNMExtractImage_ASCII;
        Ovia->Decoders[DecoderIndex].Function_Deinitialize[1] = PNMOptions_Deinit;
        /* MagicID 1 */
        
        /* MagicID 2 */
        Ovia->Decoders[DecoderIndex].MagicIDOffset[2]         = 0;
        Ovia->Decoders[DecoderIndex].MagicIDSize[2]           = 2;
        Ovia->Decoders[DecoderIndex].MagicID[2]               = (uint8_t[2]) {0x50, 0x33};
        Ovia->Decoders[DecoderIndex].Function_Initialize[2]   = PNMOptions_Init;
        Ovia->Decoders[DecoderIndex].Function_Parse[2]        = PNMParse_ASCII;
        Ovia->Decoders[DecoderIndex].Function_Decode[2]       = PNMExtractImage_ASCII;
        Ovia->Decoders[DecoderIndex].Function_Deinitialize[2] = PNMOptions_Deinit;
        /* MagicID 2 */ // Last ASCII
        
        /* MagicID 3 */
        Ovia->Decoders[DecoderIndex].MagicIDOffset[3]         = 0;
        Ovia->Decoders[DecoderIndex].MagicIDSize[3]           = 2;
        Ovia->Decoders[DecoderIndex].MagicID[3]               = (uint8_t[2]) {0x50, 0x34};
        Ovia->Decoders[DecoderIndex].Function_Initialize[3]   = PNMOptions_Init;
        Ovia->Decoders[DecoderIndex].Function_Parse[3]        = PNMParse_Binary;
        Ovia->Decoders[DecoderIndex].Function_Decode[3]       = PNMExtractImage_Binary;
        Ovia->Decoders[DecoderIndex].Function_Deinitialize[3] = PNMOptions_Deinit;
        /* MagicID 3 */
        
        /* MagicID 4 */
        Ovia->Decoders[DecoderIndex].MagicIDOffset[4]         = 0;
        Ovia->Decoders[DecoderIndex].MagicIDSize[4]           = 2;
        Ovia->Decoders[DecoderIndex].MagicID[4]               = (uint8_t[2]) {0x50, 0x35};
        Ovia->Decoders[DecoderIndex].Function_Initialize[4]   = PNMOptions_Init;
        Ovia->Decoders[DecoderIndex].Function_Parse[4]        = PNMParse_Binary;
        Ovia->Decoders[DecoderIndex].Function_Decode[4]       = PNMExtractImage_Binary;
        Ovia->Decoders[DecoderIndex].Function_Deinitialize[4] = PNMOptions_Deinit;
        /* MagicID 4 */
        
        /* MagicID 5 */
        Ovia->Decoders[DecoderIndex].MagicIDOffset[5]         = 0;
        Ovia->Decoders[DecoderIndex].MagicIDSize[5]           = 2;
        Ovia->Decoders[DecoderIndex].MagicID[5]               = (uint8_t[2]) {0x50, 0x36};
        Ovia->Decoders[DecoderIndex].Function_Initialize[5]   = PNMOptions_Init;
        Ovia->Decoders[DecoderIndex].Function_Parse[5]        = PNMParse_Binary;
        Ovia->Decoders[DecoderIndex].Function_Decode[5]       = PNMExtractImage_Binary;
        Ovia->Decoders[DecoderIndex].Function_Deinitialize[5] = PNMOptions_Deinit;
        /* MagicID 5 */
        
        /* MagicID 6 */
        Ovia->Decoders[DecoderIndex].MagicIDOffset[6]         = 0;
        Ovia->Decoders[DecoderIndex].MagicIDSize[6]           = 2;
        Ovia->Decoders[DecoderIndex].MagicID[6]               = (uint8_t[2]) {0x50, 0x37};
        Ovia->Decoders[DecoderIndex].Function_Initialize[6]   = PNMOptions_Init;
        Ovia->Decoders[DecoderIndex].Function_Parse[6]        = PNMParse_PAM;
        Ovia->Decoders[DecoderIndex].Function_Decode[6]       = PNMExtractImage_Binary;
        Ovia->Decoders[DecoderIndex].Function_Deinitialize[6] = PNMOptions_Deinit;
        /* MagicID 6 */
    }
    
    static OVIACodecRegistry Register_PNMDecoder = {
        .Function_RegisterDecoder[CodecID_PNM - 1] = RegisterDecoder_PNM,
    };
    
#ifdef __cplusplus
}
#endif
