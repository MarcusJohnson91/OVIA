#include "../../include/Private/PNMCommon.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    static uint64_t PNMCheckForComment(BitBuffer *BitB) {
        uint64_t CommentSize = 0;
        if (BitB != NULL) {
            if (BitBuffer_PeekBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8) == PNMCommentStart) {
                BitBuffer_Seek(BitB, 8);
                do {
                    CommentSize += 1;
                } while (BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8) != PNMEndField);
            }
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
        return CommentSize;
    }
    
    void PNMReadHeader(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNMOptions *PNM           = Options;
            // Read the Marker, store it in PNMOptions
            BitBuffer_Seek(BitB, 8);
            uint8_t Type = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8);
            switch (Type) {
                case '1':
                case '2':
                case '3':
                    PNM->Type = ASCIIPNM;
                    break;
                case '4':
                case '5':
                case '6':
                    PNM->Type = BinaryPNM;
                    break;
                case '7':
                    PNM->Type = PAMPNM;
                    break;
            }
            if (PNM->Type == ASCIIPNM) {
                uint64_t CommentSizeWidth = PNMCheckForComment(BitB);
                BitBuffer_Seek(BitB, Bytes2Bits(CommentSizeWidth));
                /* Read Width */
                uint64_t WidthStringSize  = BitBuffer_GetUTF8StringSize(BitB);
                UTF8    *WidthString      = BitBuffer_ReadUTF8(BitB, WidthStringSize);
                PNM->Width                = UTF8_String2Integer(WidthString, Base_Integer | Base_Radix10);
                free(WidthString);
                /* Read Width */
                
                uint64_t CommentSizeHeight = PNMCheckForComment(BitB);
                BitBuffer_Seek(BitB, Bytes2Bits(CommentSizeHeight));
                
                /* Read Height */
                uint64_t HeightStringSize = BitBuffer_GetUTF8StringSize(BitB);
                UTF8    *HeightString     = BitBuffer_ReadUTF8(BitB, HeightStringSize);
                PNM->Height               = UTF8_String2Integer(HeightString, Base_Integer | Base_Radix10);
                free(HeightString);
                /* Read Height */
            } else if (PNM->Type == BinaryPNM) {
                /* Read Width */
                uint64_t WidthStringSize  = BitBuffer_GetUTF8StringSize(BitB);
                UTF8    *WidthString      = BitBuffer_ReadUTF8(BitB, WidthStringSize);
                PNM->Width                = UTF8_String2Integer(WidthString, Base_Integer | Base_Radix10);
                free(WidthString);
                /* Read Width */
                
                /* Read Height */
                uint64_t HeightStringSize = BitBuffer_GetUTF8StringSize(BitB);
                UTF8    *HeightString     = BitBuffer_ReadUTF8(BitB, HeightStringSize);
                PNM->Height               = UTF8_String2Integer(HeightString, Base_Integer | Base_Radix10);
                free(HeightString);
                /* Read Height */
                
                /* Read MaxVal */
                uint64_t MaxValStringSize = BitBuffer_GetUTF8StringSize(BitB);
                UTF8    *MaxValString     = BitBuffer_ReadUTF8(BitB, MaxValStringSize);
                uint64_t MaxVal           = UTF8_String2Integer(MaxValString, Base_Integer | Base_Radix10);
                PNM->BitDepth             = Logarithm(2, MaxVal + 1);
                free(MaxValString);
                /* Read MaxVal */
            } else if (PNM->Type == PAMPNM) {
                /* Read Width */
                BitBuffer_Seek(BitB, 48); // Skip "WIDTH " string
                
                uint64_t WidthStringSize  = BitBuffer_GetUTF8StringSize(BitB);
                UTF8    *WidthString      = BitBuffer_ReadUTF8(BitB, WidthStringSize);
                PNM->Width                = UTF8_String2Integer(WidthString, Base_Integer | Base_Radix10);
                free(WidthString);
                /* Read Width */
                
                /* Read Height */
                BitBuffer_Seek(BitB, 56); // Skip "HEIGHT " string
                
                uint64_t HeightStringSize = BitBuffer_GetUTF8StringSize(BitB);
                UTF8    *HeightString     = BitBuffer_ReadUTF8(BitB, HeightStringSize);
                PNM->Height               = UTF8_String2Integer(HeightString, Base_Integer | Base_Radix10);
                free(HeightString);
                /* Read Height */
                
                /* Read NumChannels */
                BitBuffer_Seek(BitB, 48); // Skip "DEPTH " string
                
                uint64_t NumChannelsStringSize  = BitBuffer_GetUTF8StringSize(BitB);
                UTF8    *NumChannelsString      = BitBuffer_ReadUTF8(BitB, NumChannelsStringSize);
                PNM->NumChannels                = UTF8_String2Integer(NumChannelsString, Base_Integer | Base_Radix10);
                free(NumChannelsString);
                /* Read NumChannels */
                
                /* Read MaxVal */
                BitBuffer_Seek(BitB, 56); // Skip "MAXVAL " string
                
                uint64_t MaxValStringSize = BitBuffer_GetUTF8StringSize(BitB);
                UTF8    *MaxValString     = BitBuffer_ReadUTF8(BitB, MaxValStringSize);
                uint64_t MaxVal           = UTF8_String2Integer(MaxValString, Base_Integer | Base_Radix10);
                PNM->BitDepth             = Logarithm(2, MaxVal + 1);
                free(MaxValString);
                /* Read MaxVal */
                
                /* Read TupleType */
                BitBuffer_Seek(BitB, 72); // Skip "TUPLETYPE " string
                uint64_t TupleTypeSize     = BitBuffer_GetUTF8StringSize(BitB);
                UTF8    *TupleTypeString   = BitBuffer_ReadUTF8(BitB, TupleTypeSize);
                
                if (UTF8_CompareSubString(TupleTypeString, UTF8String("BLACKANDWHITE"), 0, 0) == true) {
                    PNM->NumChannels       = 1;
                    PNM->TupleType         = PNM_TUPLE_BnW;
                } else if (UTF8_CompareSubString(TupleTypeString, UTF8String("GRAYSCALE"), 0, 0) == true) {
                    PNM->NumChannels       = 1;
                    PNM->TupleType         = PNM_TUPLE_Gray;
                } else if (UTF8_CompareSubString(TupleTypeString, UTF8String("GRAYSCALE_ALPHA"), 0, 0) == true) {
                    PNM->NumChannels       = 2;
                    PNM->TupleType         = PNM_TUPLE_GrayAlpha;
                } else if (UTF8_CompareSubString(TupleTypeString, UTF8String("RGB"), 0, 0) == true) {
                    PNM->NumChannels       = 3;
                    PNM->TupleType         = PNM_TUPLE_RGB;
                } else if (UTF8_CompareSubString(TupleTypeString, UTF8String("RGB_ALPHA"), 0, 0) == true) {
                    PNM->NumChannels       = 4;
                    PNM->TupleType         = PNM_TUPLE_RGBAlpha;
                } else {
                    Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Unknown PNM Tuple: %s"), TupleTypeString);
                }
                free(TupleTypeString);
                /* Read TupleType */
                
                /* Skip ENDHDR */
                BitBuffer_Seek(BitB, 56); // ENDHDR
                /* Skip ENDHDR */
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    /*
     if (PNM->Type == ASCIIPNM) {
     
     } else if (PNM->Type == BinaryPNM) {
     
     } else if (PNM->Type == PAMPNM) {
     
     }
     */
    
    void *PNMExtractImage(void *Options, BitBuffer *BitB) {
        ImageContainer *Image = NULL;
        if (Options != NULL && BitB != NULL && Image != NULL) {
            PNMOptions *PNM   = Options;
            if (PNM->Type == ASCIIPNM) {
                if (PNM->BitDepth <= 8) {
                    uint8_t ****Array        = (uint8_t****) ImageContainer_GetArray(Image);
                    UTF8        Component[4] = {0, 0, 0, 0};
                    for (uint64_t Width = 0; Width < PNM->Width; Width++) {
                        for (uint64_t Height = 0; Height < PNM->Height; Height++) {
                            for (uint8_t Channel = 0; Channel < PNM->NumChannels; Channel++) {
                                for (uint8_t SubPixelByte = 0; SubPixelByte < 3; SubPixelByte++) {
                                    Component[SubPixelByte]      = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8);
                                }
                                Array[0][Width][Height][Channel] = UTF8_String2Integer(Component, Base_Integer | Base_Radix10);
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
                                    Component[SubPixelByte]      = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8);
                                }
                                Array[0][Width][Height][Channel] = UTF8_String2Integer(Component, Base_Integer | Base_Radix10);
                            }
                        }
                    }
                } else {
                    Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("16 bit ASCII PNM is invalid"));
                }
            } else if (PNM->Type == BinaryPNM || PNM->Type == PAMPNM) {
                if (PNM->BitDepth <= 8) {
                    uint8_t ****Array  = (uint8_t****) ImageContainer_GetArray(Image);
                    
                    for (uint64_t Width = 0; Width < PNM->Width; Width++) {
                        for (uint64_t Height = 0; Height < PNM->Height; Height++) {
                            for (uint8_t Channel = 0; Channel < PNM->NumChannels; Channel++) {
                                uint8_t CurrentPixel                 = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_LSByte, BitIO_BitOrder_LSBit, PNM->BitDepth);
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
                                uint16_t CurrentPixel                = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_LSByte, BitIO_BitOrder_LSBit, PNM->BitDepth);
                                if (PNM->TupleType == PNM_TUPLE_BnW) {
                                    Array[0][Width][Height][Channel] = ~CurrentPixel; // 1 = black, 0 = white
                                } else {
                                    Array[0][Width][Height][Channel] = CurrentPixel; // 1 = white, 0 = black
                                }
                            }
                        }
                    }
                }
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (Image == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        }
        return Image;
    }
    
    static const OVIADecoder PNMDecoder = {
        .Function_Initialize   = PNMOptions_Init,
        .Function_Decode       = PNMExtractImage,
        .Function_Read         = PNMReadHeader,
        .Function_Deinitialize = PNMOptions_Deinit,
        .MagicIDs              = &PNMMagicIDs,
        .MediaType             = MediaType_Image,
        .DecoderID             = CodecID_PNG,
    };
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
