#include "../../../include/Private/Image/PNMCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    static uint64_t PNMCheckForComment(BitBuffer *BitB) { // returns 0 if no comment was found, returns the number of bytes that make up the comment if it was.
        uint64_t CommentSize = 0;
        if (ReadBits(MSByteFirst, LSBitFirst, BitB, 8) == PNMCommentStart) {
            while (ReadBits(MSByteFirst, LSBitFirst, BitB, 8) != PNMEndField) {
                CommentSize += 1;
            }
        }
        return CommentSize;
    }
    
    static void PNMParsePNMASCIIHeader(PCMFile *PCM, BitBuffer *BitB) {
        uint64_t CommentSizeWidth = PNMCheckForComment(BitB);
        BitBuffer_Skip(BitB, Bytes2Bits(CommentSizeWidth));
        /* Read Width */
        uint64_t WidthStringSize = 0LLU;
        while (ReadBits(MSByteFirst, LSBitFirst, BitB, 8) != PNMFieldSeperator) {
            WidthStringSize += 1;
        }
        UTF8 *WidthString = calloc(1, WidthStringSize * sizeof(char));
        for (uint64_t WidthByte = 0; WidthByte < WidthStringSize; WidthByte++) {
            WidthString[WidthByte] = ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
        }
        PCM->Pic->Width = UTF8_String2Integer(WidthString);
        free(WidthString);
        /* Read Width */
        
        uint64_t CommentSizeHeight = PNMCheckForComment(BitB);
        BitBuffer_Skip(BitB, Bytes2Bits(CommentSizeHeight));
        
        /* Read Height */
        uint64_t HeightStringSize = 0LLU;
        while (ReadBits(MSByteFirst, LSBitFirst, BitB, 8) != PNMEndField) {
            HeightStringSize += 1;
        }
        UTF8 *HeightString = calloc(1, HeightStringSize * sizeof(char));
        for (uint64_t HeightByte = 0; HeightByte < HeightStringSize; HeightByte++) {
            HeightString[HeightByte] = ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
        }
        PCM->Pic->Height = UTF8_String2Integer(HeightString); // Ok, so we read the Height.
        free(HeightString);
    }
    
    static void PNMParsePNMBinaryHeader(PCMFile *PCM, BitBuffer *BitB) {
        /* Read Width */
        uint64_t WidthStringSize = 0LLU;
        while (ReadBits(MSByteFirst, LSBitFirst, BitB, 8) != PNMFieldSeperator) {
            WidthStringSize += 1;
        }
        UTF8 *WidthString = calloc(1, WidthStringSize * sizeof(char));
        for (uint64_t WidthByte = 0; WidthByte < WidthStringSize; WidthByte++) {
            WidthString[WidthByte] = ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
        }
        PCM->Pic->Width = UTF8_String2Integer(WidthString);
        free(WidthString);
        /* Read Width */
        
        /* Read Height */
        uint64_t HeightStringSize = 0LLU;
        while (ReadBits(MSByteFirst, LSBitFirst, BitB, 8) != PNMEndField) {
            HeightStringSize += 1;
        }
        UTF8 *HeightString = calloc(1, HeightStringSize * sizeof(char));
        for (uint64_t HeightByte = 0; HeightByte < HeightStringSize; HeightByte++) {
            HeightString[HeightByte] = ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
        }
        PCM->Pic->Height = UTF8_String2Integer(HeightString); // Ok, so we read the Height.
        free(HeightString);
        /* Read Height */
        
        /* Read MaxVal */
        uint8_t MaxValStringSize = 0;
        while (ReadBits(MSByteFirst, LSBitFirst, BitB, 8) != PNMEndField) {
            MaxValStringSize += 1;
        }
        UTF8 *MaxValString = calloc(1, MaxValStringSize * sizeof(char));
        for (uint8_t MaxValByte = 0; MaxValByte < MaxValStringSize; MaxValByte++) {
            MaxValString[MaxValByte] = ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
        }
        int64_t MaxVal     = UTF8_String2Integer(MaxValString);
        PCM->BitDepth      = Logarithm(2, MaxVal + 1);
        free(MaxValString);
        /* Read MaxVal */
    }
    
    static void PNMParsePAMHeader(PCMFile *PCM, BitBuffer *BitB) {
        /* Read Width */
        BitBuffer_Skip(BitB, 48); // Skip "WIDTH " string
        uint64_t WidthStringSize = 0LLU;
        while (ReadBits(MSByteFirst, LSBitFirst, BitB, 8) != PNMFieldSeperator) {
            WidthStringSize += 1;
        }
        UTF8 *WidthString = calloc(1, WidthStringSize * sizeof(char));
        for (uint64_t WidthByte = 0; WidthByte < WidthStringSize; WidthByte++) {
            WidthString[WidthByte] = ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
        }
        PCM->Pic->Width = UTF8_String2Integer(WidthString);
        free(WidthString);
        /* Read Width */
        
        /* Read Height */
        BitBuffer_Skip(BitB, 56); // Skip "HEIGHT " string
        uint64_t HeightStringSize = 0LLU;
        while (ReadBits(MSByteFirst, LSBitFirst, BitB, 8) != PNMEndField) {
            HeightStringSize += 1;
        }
        UTF8 *HeightString = calloc(1, HeightStringSize * sizeof(char));
        for (uint64_t HeightByte = 0; HeightByte < HeightStringSize; HeightByte++) {
            HeightString[HeightByte] = ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
        }
        PCM->Pic->Height = UTF8_String2Integer(HeightString); // Ok, so we read the Height.
        free(HeightString);
        /* Read Height */
        
        /* Read NumChannels */
        BitBuffer_Skip(BitB, 48); // Skip "DEPTH " string
        uint8_t DepthStringSize = 0;
        while (ReadBits(MSByteFirst, LSBitFirst, BitB, 8) != PNMEndField) {
            DepthStringSize += 1;
        }
        UTF8 *DepthString = calloc(1, DepthStringSize * sizeof(char));
        for (uint8_t DepthByte = 0; DepthByte < DepthStringSize; DepthByte++) {
            DepthString[DepthByte] = ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
        }
        PCM->NumChannels = UTF8_String2Integer(DepthString);
        free(DepthString);
        /* Read NumChannels */
        
        /* Read MaxVal */
        BitBuffer_Skip(BitB, 56); // Skip "MAXVAL " string
        uint8_t MaxValStringSize = 0;
        while (ReadBits(MSByteFirst, LSBitFirst, BitB, 8) != PNMEndField) {
            MaxValStringSize += 1;
        }
        UTF8 *MaxValString = calloc(1, MaxValStringSize * sizeof(UTF8));
        for (uint8_t MaxValByte = 0; MaxValByte < MaxValStringSize; MaxValByte++) {
            MaxValString[MaxValByte] = ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
        }
        int64_t MaxVal    = UTF8_String2Integer(MaxValString);
        PCM->BitDepth     = Logarithm(2, MaxVal + 1);
        free(MaxValString);
        /* Read MaxVal */
        
        /* Read TupleType */
        BitBuffer_Skip(BitB, 72); // Skip "TUPLETYPE " string
        uint8_t TupleTypeSize = 0;
        while (ReadBits(MSByteFirst, LSBitFirst, BitB, 8) != PNMEndField) {
            TupleTypeSize += 1;
        }
        UTF8 *TupleTypeString = calloc(1, TupleTypeSize * sizeof(char));
        for (uint8_t TupleByte = 0; TupleByte < TupleTypeSize; TupleByte++) {
            TupleTypeString[TupleByte] = ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
        }
        if (UTF8_Compare(TupleTypeString, U8("BLACKANDWHITE"), NormalizationFormKC, Yes) == Yes) {
            PCM->NumChannels = 1;
            PCM->Pic->PNMTupleType   = PNM_TUPLE_BnW;
        } else if (UTF8_Compare(TupleTypeString, U8("GRAYSCALE"), NormalizationFormKC, Yes) == Yes) {
            PCM->NumChannels = 1;
            PCM->Pic->PNMTupleType   = PNM_TUPLE_Gray;
        } else if (UTF8_Compare(TupleTypeString, U8("GRAYSCALE_ALPHA"), NormalizationFormKC, Yes) == Yes) {
            PCM->NumChannels = 2;
            PCM->Pic->PNMTupleType   = PNM_TUPLE_GrayAlpha;
        } else if (UTF8_Compare(TupleTypeString, U8("RGB"), NormalizationFormKC, Yes) == Yes) {
            PCM->NumChannels = 3;
            PCM->Pic->PNMTupleType   = PNM_TUPLE_RGB;
        } else if (UTF8_Compare(TupleTypeString, U8("RGB_ALPHA"), NormalizationFormKC, Yes) == Yes) {
            PCM->NumChannels = 4;
            PCM->Pic->PNMTupleType   = PNM_TUPLE_RGBAlpha;
        } else {
            PCM->NumChannels = 0;
            PCM->Pic->PNMTupleType   = PNM_TUPLE_Unknown;
            Log(Log_ERROR, __func__, U8("Unknown PNM Tuple: %s"), TupleTypeString);
        }
        free(TupleTypeString);
        /* Read TupleType */
        
        /* Skip ENDHDR */
        BitBuffer_Skip(BitB, 56); // ENDHDR
        /* Skip ENDHDR */
    }
    
    void PNMIdentifyFileType(PCMFile *PCM, BitBuffer *BitB) {
        UTF8 PNMMagicID[PNMMagicSize];
        for (uint8_t PNMMagicByte = 0; PNMMagicByte < PNMMagicSize; PNMMagicByte++) {
            PNMMagicID[PNMMagicByte] = ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
        }
        if (UTF8_Compare(PNMMagicID, U8("P1"), NormalizationFormKC, Yes) == Yes || UTF8_Compare(PNMMagicID, U8("P2"), NormalizationFormKC, Yes) == Yes || UTF8_Compare(PNMMagicID, U8("P3"), NormalizationFormKC, Yes) == Yes) {
            PCM->Pic->PNMType = ASCIIPNM;
        } else if (UTF8_Compare(PNMMagicID, U8("P4"), NormalizationFormKC, Yes) == Yes || UTF8_Compare(PNMMagicID, U8("P5"), NormalizationFormKC, Yes) == Yes || UTF8_Compare(PNMMagicID, U8("P6"), NormalizationFormKC, Yes) == Yes) {
            PCM->Pic->PNMType = BinaryPNM;
        } else if (UTF8_Compare(PNMMagicID, U8("P7"), NormalizationFormKC, Yes) == 0) {
            PCM->Pic->PNMType = PAMPNM;
        }
    }
    
    void PNMParseMetadata(PCMFile *PCM, BitBuffer *BitB) {
        uint8_t NumFieldsRead = 0;
        uint8_t Fields2Read = 0;
        if (PCM->Pic->PNMType == PAMPNM) {
            Fields2Read = 5;
        } else if (PCM->Pic->PNMType == ASCIIPNM) {
            Fields2Read = 2; // there is no MaxVal field
        } else if (PCM->Pic->PNMType == BinaryPNM) {
            Fields2Read = 3;
        }
        
        BitBuffer_Skip(BitB, 8); // Skip the LineFeed after the FileType marker
        // Before each field we need to check for Comments if the file is ASCII.
        if (PCM->Pic->PNMType == ASCIIPNM) {
            PNMParsePNMASCIIHeader(PCM, BitB);
        } else if (PCM->Pic->PNMType == BinaryPNM) {
            PNMParsePNMBinaryHeader(PCM, BitB);
        } else if (PCM->Pic->PNMType == PAMPNM) {
            PNMParsePAMHeader(PCM, BitB);
        }
    }
    
    uint16_t ***PNMExtractImage(PCMFile *PCM, BitBuffer *BitB) {
        uint16_t ***ExtractedImage = calloc(PCM->Pic->Width * PCM->Pic->Height * PCM->NumChannels, sizeof(uint16_t));
        if (PCM != NULL && BitB != NULL && ExtractedImage != NULL) {
            if (PCM->Pic->PNMType == ASCIIPNM) {
                for (uint64_t Width = 0; Width < PCM->Pic->Width; Width++) {
                    for (uint64_t Height = 0; Height < PCM->Pic->Height; Height++) {
                        for (uint8_t Channel = 0; Channel < PCM->NumChannels; Channel++) {
                            uint8_t SubPixelStringSize          = 0;
                            while (PeekBits(MSByteFirst, LSBitFirst, BitB, 8) != PNMFieldSeperator || PeekBits(MSByteFirst, LSBitFirst, BitB, 8) != PNMEndField) {
                                SubPixelStringSize             += 1;
                            }
                            char *SubPixelString                = calloc(1, PCM->NumChannels * sizeof(char));
                            for (uint8_t SubPixelByte = 0; SubPixelByte < SubPixelStringSize; SubPixelByte++) {
                                SubPixelString[SubPixelByte]    = ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
                            }
                            ExtractedImage[Width][Height][Channel] = atoi(SubPixelString);
                            free(SubPixelString);
                        }
                    }
                }
            } else if (PCM->Pic->PNMType == BinaryPNM || PCM->Pic->PNMType == PAMPNM) {
                for (uint64_t Width = 0; Width < PCM->Pic->Width; Width++) {
                    for (uint64_t Height = 0; Height < PCM->Pic->Height; Height++) {
                        for (uint8_t Channel = 0; Channel < PCM->NumChannels; Channel++) {
                            uint8_t CurrentPixel                = ReadBits(LSByteFirst, LSBitFirst, BitB, PCM->BitDepth);
                            if (PCM->Pic->PNMTupleType == PNM_TUPLE_BnW && PCM->Pic->PNMType != PAMPNM) {
                                ExtractedImage[Width][Height][Channel] = ~CurrentPixel; // 1 = black, 0 = white
                            } else {
                                ExtractedImage[Width][Height][Channel] = CurrentPixel; // 1 = white, 0 = black
                            }
                        }
                    }
                }
            }
        } else if (PCM == NULL) {
            Log(Log_ERROR, __func__, U8("PCM Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (ExtractedImage == NULL) {
            Log(Log_ERROR, __func__, U8("ExtractedImage Pointer is NULL"));
        }
        return ExtractedImage;
    }
    
#ifdef __cplusplus
}
#endif
