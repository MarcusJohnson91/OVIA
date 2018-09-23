#include "../../../../Dependencies/FoundationIO/libFoundationIO/include/Macros.h"
#include "../../../include/Private/Image/PNMCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    static uint64_t PNMCheckForComment(BitBuffer *BitB) { // returns 0 if no comment was found, returns the number of bytes that make up the comment if it was.
        uint64_t CommentSize = 0;
        if (BitB != NULL) {
            if (BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8) == PNMCommentStart) {
                while (BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8) != PNMEndField) {
                    CommentSize += 1;
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
        return CommentSize;
    }
    
    static void PNMParsePNMASCIIHeader(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            uint64_t CommentSizeWidth = PNMCheckForComment(BitB);
            BitBuffer_Skip(BitB, Bytes2Bits(CommentSizeWidth));
            /* Read Width */
            uint64_t WidthStringSize = 0LLU;
            while (BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8) != PNMFieldSeperator) {
                WidthStringSize += 1;
            }
            UTF8 *WidthString = calloc(1, WidthStringSize * sizeof(char));
            for (uint64_t WidthByte = 0; WidthByte < WidthStringSize; WidthByte++) {
                WidthString[WidthByte] = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            }
            OVIA_SetWidth(Ovia, UTF8_String2Integer(10, WidthString));
            free(WidthString);
            /* Read Width */
            
            uint64_t CommentSizeHeight = PNMCheckForComment(BitB);
            BitBuffer_Skip(BitB, Bytes2Bits(CommentSizeHeight));
            
            /* Read Height */
            uint64_t HeightStringSize = 0LLU;
            while (BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8) != PNMEndField) {
                HeightStringSize += 1;
            }
            UTF8 *HeightString = calloc(1, HeightStringSize * sizeof(char));
            for (uint64_t HeightByte = 0; HeightByte < HeightStringSize; HeightByte++) {
                HeightString[HeightByte] = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            }
            OVIA_SetHeight(Ovia, UTF8_String2Integer(10, HeightString));
            free(HeightString);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void PNMParsePNMBinaryHeader(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            /* Read Width */
            uint64_t WidthStringSize = 0LLU;
            while (BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8) != PNMFieldSeperator) {
                WidthStringSize += 1;
            }
            UTF8 *WidthString = calloc(1, WidthStringSize * sizeof(char));
            for (uint64_t WidthByte = 0; WidthByte < WidthStringSize; WidthByte++) {
                WidthString[WidthByte] = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            }
            OVIA_SetWidth(Ovia, UTF8_String2Integer(10, WidthString));
            free(WidthString);
            /* Read Width */
            
            /* Read Height */
            uint64_t HeightStringSize = 0LLU;
            while (BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8) != PNMEndField) {
                HeightStringSize += 1;
            }
            UTF8 *HeightString = calloc(1, HeightStringSize * sizeof(char));
            for (uint64_t HeightByte = 0; HeightByte < HeightStringSize; HeightByte++) {
                HeightString[HeightByte] = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            }
            OVIA_SetHeight(Ovia, UTF8_String2Integer(10, HeightString));
            free(HeightString);
            /* Read Height */
            
            /* Read MaxVal */
            uint8_t MaxValStringSize = 0;
            while (BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8) != PNMEndField) {
                MaxValStringSize += 1;
            }
            UTF8 *MaxValString = calloc(1, MaxValStringSize * sizeof(char));
            for (uint8_t MaxValByte = 0; MaxValByte < MaxValStringSize; MaxValByte++) {
                MaxValString[MaxValByte] = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            }
            OVIA_SetBitDepth(Ovia, Logarithm(2, UTF8_String2Integer(10, MaxValString) + 1));
            free(MaxValString);
            /* Read MaxVal */
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void PNMParsePAMHeader(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            /* Read Width */
            BitBuffer_Skip(BitB, 48); // Skip "WIDTH " string
            uint64_t WidthStringSize = 0LLU;
            while (BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8) != PNMFieldSeperator) {
                WidthStringSize += 1;
            }
            UTF8 *WidthString = calloc(1, WidthStringSize * sizeof(char));
            for (uint64_t WidthByte = 0; WidthByte < WidthStringSize; WidthByte++) {
                WidthString[WidthByte] = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            }
            OVIA_SetWidth(Ovia, UTF8_String2Integer(10, WidthString));
            free(WidthString);
            /* Read Width */
            
            /* Read Height */
            BitBuffer_Skip(BitB, 56); // Skip "HEIGHT " string
            uint64_t HeightStringSize = 0LLU;
            while (BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8) != PNMEndField) {
                HeightStringSize += 1;
            }
            UTF8 *HeightString = calloc(1, HeightStringSize * sizeof(char));
            for (uint64_t HeightByte = 0; HeightByte < HeightStringSize; HeightByte++) {
                HeightString[HeightByte] = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            }
            OVIA_SetHeight(Ovia, UTF8_String2Integer(10, HeightString));
            free(HeightString);
            /* Read Height */
            
            /* Read NumChannels */
            BitBuffer_Skip(BitB, 48); // Skip "DEPTH " string
            uint8_t DepthStringSize = 0;
            while (BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8) != PNMEndField) {
                DepthStringSize += 1;
            }
            UTF8 *DepthString = calloc(1, DepthStringSize * sizeof(char));
            for (uint8_t DepthByte = 0; DepthByte < DepthStringSize; DepthByte++) {
                DepthString[DepthByte] = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            }
            OVIA_SetNumChannels(Ovia, UTF8_String2Integer(10, DepthString));
            free(DepthString);
            /* Read NumChannels */
            
            /* Read MaxVal */
            BitBuffer_Skip(BitB, 56); // Skip "MAXVAL " string
            uint8_t MaxValStringSize = 0;
            while (BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8) != PNMEndField) {
                MaxValStringSize += 1;
            }
            UTF8 *MaxValString = calloc(1, MaxValStringSize * sizeof(UTF8));
            for (uint8_t MaxValByte = 0; MaxValByte < MaxValStringSize; MaxValByte++) {
                MaxValString[MaxValByte] = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            }
            OVIA_SetBitDepth(Ovia, Logarithm(2, UTF8_String2Integer(10, MaxValString) + 1));
            free(MaxValString);
            /* Read MaxVal */
            
            /* Read TupleType */
            BitBuffer_Skip(BitB, 72); // Skip "TUPLETYPE " string
            uint8_t TupleTypeSize = 0;
            while (BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8) != PNMEndField) {
                TupleTypeSize += 1;
            }
            UTF8 *TupleTypeString = calloc(1, TupleTypeSize * sizeof(char));
            for (uint8_t TupleByte = 0; TupleByte < TupleTypeSize; TupleByte++) {
                TupleTypeString[TupleByte] = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            }
            if (UTF8_Compare(TupleTypeString, U8("BLACKANDWHITE")) == Yes) {
                OVIA_SetNumChannels(Ovia, 1);
                OVIA_PNM_SetTupleType(Ovia, PNM_TUPLE_BnW);
            } else if (UTF8_Compare(TupleTypeString, U8("GRAYSCALE")) == Yes) {
                OVIA_SetNumChannels(Ovia, 1);
                OVIA_PNM_SetTupleType(Ovia, PNM_TUPLE_Gray);
            } else if (UTF8_Compare(TupleTypeString, U8("GRAYSCALE_ALPHA")) == Yes) {
                OVIA_SetNumChannels(Ovia, 2);
                OVIA_PNM_SetTupleType(Ovia, PNM_TUPLE_GrayAlpha);
            } else if (UTF8_Compare(TupleTypeString, U8("RGB")) == Yes) {
                OVIA_SetNumChannels(Ovia, 3);
                OVIA_PNM_SetTupleType(Ovia, PNM_TUPLE_RGB);
            } else if (UTF8_Compare(TupleTypeString, U8("RGB_ALPHA")) == Yes) {
                OVIA_SetNumChannels(Ovia, 4);
                OVIA_PNM_SetTupleType(Ovia, PNM_TUPLE_RGBAlpha);
            } else {
                Log(Log_ERROR, __func__, U8("Unknown PNM Tuple: %s"), TupleTypeString);
            }
            free(TupleTypeString);
            /* Read TupleType */
            
            /* Skip ENDHDR */
            BitBuffer_Skip(BitB, 56); // ENDHDR
            /* Skip ENDHDR */
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNMIdentifyFileType(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            UTF8 PNMMagicID[PNMMagicSize];
            for (uint8_t PNMMagicByte = 0; PNMMagicByte < PNMMagicSize; PNMMagicByte++) {
                PNMMagicID[PNMMagicByte] = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            }
            if (UTF8_Compare(PNMMagicID, U8("P1")) == Yes || UTF8_Compare(PNMMagicID, U8("P2")) == Yes || UTF8_Compare(PNMMagicID, U8("P3")) == Yes) {
                OVIA_PNM_SetPNMType(Ovia, ASCIIPNM);
            } else if (UTF8_Compare(PNMMagicID, U8("P4")) == Yes || UTF8_Compare(PNMMagicID, U8("P5")) == Yes || UTF8_Compare(PNMMagicID, U8("P6")) == Yes) {
                OVIA_PNM_SetPNMType(Ovia, BinaryPNM);
            } else if (UTF8_Compare(PNMMagicID, U8("P7")) == Yes) {
                OVIA_PNM_SetPNMType(Ovia, PAMPNM);
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNMParseMetadata(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            uint8_t NumFieldsRead = 0;
            uint8_t Fields2Read = 0;
            if (OVIA_PNM_GetPNMType(Ovia) == PAMPNM) {
                Fields2Read = 5;
            } else if (OVIA_PNM_GetPNMType(Ovia) == ASCIIPNM) {
                Fields2Read = 2; // there is no MaxVal field
            } else if (OVIA_PNM_GetPNMType(Ovia) == BinaryPNM) {
                Fields2Read = 3;
            }
            
            BitBuffer_Skip(BitB, 8); // Skip the LineFeed after the FileType marker
            // Before each field we need to check for Comments if the file is ASCII.
            if (OVIA_PNM_GetTupleType(Ovia) == ASCIIPNM) {
                PNMParsePNMASCIIHeader(Ovia, BitB);
            } else if (OVIA_PNM_GetTupleType(Ovia) == BinaryPNM) {
                PNMParsePNMBinaryHeader(Ovia, BitB);
            } else if (OVIA_PNM_GetTupleType(Ovia) == PAMPNM) {
                PNMParsePAMHeader(Ovia, BitB);
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    ImageContainer *PNMExtractImage(OVIA *Ovia, BitBuffer *BitB) {
        ImageContainer *Image    = NULL;
        if (Ovia != NULL && BitB != NULL) {
            uint64_t Width       = OVIA_GetWidth(Ovia);
            uint64_t Height      = OVIA_GetHeight(Ovia);
            uint64_t NumChannels = OVIA_GetNumChannels(Ovia);
            uint8_t  BitDepth    = OVIA_GetBitDepth(Ovia);
            
            Image = ImageContainer_Init(BitDepth <= 8 ? ImageContainer_2DUInteger8 : ImageContainer_2DUInteger16, BitDepth, NumChannels, Width, Height);
            
            if (OVIA_PNM_GetPNMType(Ovia) == ASCIIPNM) {
                if (BitDepth <= 8) {
                    uint8_t ****Array  = (uint8_t****) ImageContainer_GetArray(Image);
                    
                    for (uint64_t W = 0; W < Width; W++) {
                        for (uint64_t H = 0; H < Height; H++) {
                            for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                                uint8_t SubPixelStringSize          = 0;
                                while (BitBuffer_PeekBits(MSByteFirst, LSBitFirst, BitB, 8) != PNMFieldSeperator || BitBuffer_PeekBits(MSByteFirst, LSBitFirst, BitB, 8) != PNMEndField) {
                                    SubPixelStringSize             += 1;
                                }
                                char *SubPixelString                = calloc(OVIA_GetNumChannels(Ovia), sizeof(char));
                                for (uint8_t SubPixelByte = 0; SubPixelByte < SubPixelStringSize; SubPixelByte++) {
                                    SubPixelString[SubPixelByte]    = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
                                }
                                Array[0][Width][Height][Channel]    = UTF8_String2Integer(10, SubPixelString);
                                free(SubPixelString);
                            }
                        }
                    }
                } else {
                    Log(Log_ERROR, __func__, U8("16 bit ASCII PNM is invalid"));
                }
            } else if (OVIA_PNM_GetPNMType(Ovia) == BinaryPNM || OVIA_PNM_GetPNMType(Ovia) == PAMPNM) {
                if (BitDepth <= 8) {
                    uint8_t ****Array  = (uint8_t****) ImageContainer_GetArray(Image);
                    
                    for (uint64_t W = 0; W < Width; W++) {
                        for (uint64_t H = 0; H < Height; H++) {
                            for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                                uint8_t CurrentPixel                = BitBuffer_ReadBits(LSByteFirst, LSBitFirst, BitB, BitDepth);
                                if (OVIA_PNM_GetTupleType(Ovia) == PNM_TUPLE_BnW && OVIA_PNM_GetTupleType(Ovia) != PAMPNM) {
                                    Array[0][Width][Height][Channel] = ~CurrentPixel; // 1 = black, 0 = white
                                } else {
                                    Array[0][Width][Height][Channel] = CurrentPixel; // 1 = white, 0 = black
                                }
                            }
                        }
                    }
                } else if (BitDepth <= 16) {
                    uint16_t ****Array  = (uint16_t****) ImageContainer_GetArray(Image);
                    
                    for (uint64_t W = 0; W < Width; W++) {
                        for (uint64_t H = 0; H < Height; H++) {
                            for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                                uint8_t CurrentPixel                = BitBuffer_ReadBits(LSByteFirst, LSBitFirst, BitB, BitDepth);
                                if (OVIA_PNM_GetTupleType(Ovia) == PNM_TUPLE_BnW && OVIA_PNM_GetTupleType(Ovia) != PAMPNM) {
                                    Array[0][Width][Height][Channel] = ~CurrentPixel; // 1 = black, 0 = white
                                } else {
                                    Array[0][Width][Height][Channel] = CurrentPixel; // 1 = white, 0 = black
                                }
                            }
                        }
                    }
                }
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
        return Image;
    }
    
#ifdef __cplusplus
}
#endif
