#include "../../../../Dependencies/FoundationIO/libFoundationIO/include/Macros.h"
#include "../../../include/Private/Image/PNMCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    static uint64_t PNMCheckForComment(BitBuffer *BitB) { // returns 0 if no comment was found, returns the number of bytes that make up the comment if it was.
        uint64_t CommentSize = 0;
        if (BitB != NULL) {
            if (BitBuffer_PeekBits(MSByteFirst, LSBitFirst, BitB, 8) == PNMCommentStart) {
                BitBuffer_Seek(BitB, 8);
                do {
                    CommentSize += 1;
                } while (BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8) != PNMEndField);
            }
        } else {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
        return CommentSize;
    }
    
    static void PNMParsePNMASCIIHeader(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            uint64_t CommentSizeWidth = PNMCheckForComment(BitB);
            BitBuffer_Seek(BitB, Bytes2Bits(CommentSizeWidth));
            /* Read Width */
            uint64_t WidthStringSize  = BitBuffer_GetUTF8StringSize(BitB);
            UTF8    *WidthString      = BitBuffer_ReadUTF8(BitB, WidthStringSize);
            OVIA_SetWidth(Ovia, UTF8_String2Integer(10, WidthString));
            free(WidthString);
            /* Read Width */
            
            uint64_t CommentSizeHeight = PNMCheckForComment(BitB);
            BitBuffer_Seek(BitB, Bytes2Bits(CommentSizeHeight));
            
            /* Read Height */
            uint64_t HeightStringSize = BitBuffer_GetUTF8StringSize(BitB);
            UTF8    *HeightString     = BitBuffer_ReadUTF8(BitB, HeightStringSize);
            OVIA_SetHeight(Ovia, UTF8_String2Integer(10, HeightString));
            free(HeightString);
            /* Read Height */
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void PNMParsePNMBinaryHeader(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            /* Read Width */
            uint64_t WidthStringSize  = BitBuffer_GetUTF8StringSize(BitB);
            UTF8    *WidthString      = BitBuffer_ReadUTF8(BitB, WidthStringSize);
            OVIA_SetWidth(Ovia, UTF8_String2Integer(10, WidthString));
            free(WidthString);
            /* Read Width */
            
            /* Read Height */
            uint64_t HeightStringSize = BitBuffer_GetUTF8StringSize(BitB);
            UTF8    *HeightString     = BitBuffer_ReadUTF8(BitB, HeightStringSize);
            OVIA_SetHeight(Ovia, UTF8_String2Integer(10, HeightString));
            free(HeightString);
            /* Read Height */
            
            /* Read MaxVal */
            uint64_t MaxValStringSize = BitBuffer_GetUTF8StringSize(BitB);
            UTF8    *MaxValString     = BitBuffer_ReadUTF8(BitB, MaxValStringSize);
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
            BitBuffer_Seek(BitB, 48); // Skip "WIDTH " string
            
            uint64_t WidthStringSize  = BitBuffer_GetUTF8StringSize(BitB);
            UTF8    *WidthString      = BitBuffer_ReadUTF8(BitB, WidthStringSize);
            
            OVIA_SetWidth(Ovia, UTF8_String2Integer(10, WidthString));
            free(WidthString);
            /* Read Width */
            
            /* Read Height */
            BitBuffer_Seek(BitB, 56); // Skip "HEIGHT " string
            
            uint64_t HeightStringSize = BitBuffer_GetUTF8StringSize(BitB);
            UTF8    *HeightString     = BitBuffer_ReadUTF8(BitB, HeightStringSize);
            
            OVIA_SetHeight(Ovia, UTF8_String2Integer(10, HeightString));
            free(HeightString);
            /* Read Height */
            
            /* Read NumChannels */
            BitBuffer_Seek(BitB, 48); // Skip "DEPTH " string
            
            uint64_t DepthStringSize  = BitBuffer_GetUTF8StringSize(BitB);
            UTF8    *DepthString      = BitBuffer_ReadUTF8(BitB, DepthStringSize);
            
            OVIA_SetNumChannels(Ovia, UTF8_String2Integer(10, DepthString));
            free(DepthString);
            /* Read NumChannels */
            
            /* Read MaxVal */
            BitBuffer_Seek(BitB, 56); // Skip "MAXVAL " string
            
            uint64_t MaxValStringSize = BitBuffer_GetUTF8StringSize(BitB);
            UTF8    *MaxValString     = BitBuffer_ReadUTF8(BitB, MaxValStringSize);
            
            OVIA_SetBitDepth(Ovia, Logarithm(2, UTF8_String2Integer(10, MaxValString) + 1));
            free(MaxValString);
            /* Read MaxVal */
            
            /* Read TupleType */
            BitBuffer_Seek(BitB, 72); // Skip "TUPLETYPE " string
            uint64_t TupleTypeSize     = BitBuffer_GetUTF8StringSize(BitB);
            UTF8    *TupleTypeString   = BitBuffer_ReadUTF8(BitB, TupleTypeSize);
            
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
            BitBuffer_Seek(BitB, 56); // ENDHDR
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
            
            BitBuffer_Seek(BitB, 8); // Skip the LineFeed after the FileType marker
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
            Image                = ImageContainer_Init(BitDepth <= 8 ? ImageType_Integer8 : ImageType_Integer16, BitDepth, 1, NumChannels, Width, Height);
            
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
