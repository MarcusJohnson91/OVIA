#include <stdlib.h>

#include "../../../Dependencies/FoundationIO/libFoundationIO/include/BitIO.h"
#include "../../../Dependencies/FoundationIO/libFoundationIO/include/Log.h"
#include "../../../Dependencies/FoundationIO/libFoundationIO/include/Math.h"

#include "../../include/libPCM.h"
#include "../../include/Private/libPCMTypes.h"
#include "../../include/Private/Image/PXMCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    static uint64_t PXMCheckForComment(BitBuffer *BitB) { // returns 0 if no comment was found, returns the number of bytes that make up the comment if it was.
        uint64_t CommentSize = 0;
        if (ReadBits(MSByteFirst, LSBitFirst, BitB, 8) == PXMCommentStart) {
            while (ReadBits(MSByteFirst, LSBitFirst, BitB, 8) != PXMEndField) {
                CommentSize += 1;
            }
        }
        return CommentSize;
    }
    
    static void PXMParsePNMASCIIHeader(PCMFile *PCM, BitBuffer *BitB) {
        uint64_t CommentSizeWidth = PXMCheckForComment(BitB);
        BitBuffer_Skip(BitB, Bytes2Bits(CommentSizeWidth));
        /* Read Width */
        uint64_t WidthStringSize = 0LLU;
        while (ReadBits(MSByteFirst, LSBitFirst, BitB, 8) != PXMFieldSeperator) {
            WidthStringSize += 1;
        }
        char *WidthString = calloc(1, WidthStringSize * sizeof(char));
        for (uint64_t WidthByte = 0; WidthByte < WidthStringSize; WidthByte++) {
            WidthString[WidthByte] = ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
        }
        PCM->Pic->Width = UTF32_String2Integer(UTF8_Decode(WidthString));
        free(WidthString);
        /* Read Width */
        
        uint64_t CommentSizeHeight = PXMCheckForComment(BitB);
        BitBuffer_Skip(BitB, Bytes2Bits(CommentSizeHeight));
        
        /* Read Height */
        uint64_t HeightStringSize = 0LLU;
        while (ReadBits(MSByteFirst, LSBitFirst, BitB, 8) != PXMEndField) {
            HeightStringSize += 1;
        }
        char *HeightString = calloc(1, HeightStringSize * sizeof(char));
        for (uint64_t HeightByte = 0; HeightByte < HeightStringSize; HeightByte++) {
            HeightString[HeightByte] = ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
        }
        PCM->Pic->Height = UTF32_String2Integer(UTF8_Decode(HeightString)); // Ok, so we read the Height.
        free(HeightString);
    }
    
    static void PXMParsePNMBinaryHeader(PCMFile *PCM, BitBuffer *BitB) {
        /* Read Width */
        uint64_t WidthStringSize = 0LLU;
        while (ReadBits(MSByteFirst, LSBitFirst, BitB, 8) != PXMFieldSeperator) {
            WidthStringSize += 1;
        }
        char *WidthString = calloc(1, WidthStringSize * sizeof(char));
        for (uint64_t WidthByte = 0; WidthByte < WidthStringSize; WidthByte++) {
            WidthString[WidthByte] = ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
        }
        PCM->Pic->Width = UTF32_String2Integer(UTF8_Decode(WidthString));
        free(WidthString);
        /* Read Width */
        
        /* Read Height */
        uint64_t HeightStringSize = 0LLU;
        while (ReadBits(MSByteFirst, LSBitFirst, BitB, 8) != PXMEndField) {
            HeightStringSize += 1;
        }
        char *HeightString = calloc(1, HeightStringSize * sizeof(char));
        for (uint64_t HeightByte = 0; HeightByte < HeightStringSize; HeightByte++) {
            HeightString[HeightByte] = ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
        }
        PCM->Pic->Height = UTF32_String2Integer(UTF8_Decode(HeightString)); // Ok, so we read the Height.
        free(HeightString);
        /* Read Height */
        
        /* Read MaxVal */
        uint8_t MaxValStringSize = 0;
        while (ReadBits(MSByteFirst, LSBitFirst, BitB, 8) != PXMEndField) {
            MaxValStringSize += 1;
        }
        char *MaxValString = calloc(1, MaxValStringSize * sizeof(char));
        for (uint8_t MaxValByte = 0; MaxValByte < MaxValStringSize; MaxValByte++) {
            MaxValString[MaxValByte] = ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
        }
        int64_t MaxVal     = UTF32_String2Integer(UTF8_Decode(MaxValString));
        PCM->BitDepth      = Logarithm(2, MaxVal + 1);
        free(MaxValString);
        /* Read MaxVal */
    }
    
    static void PXMParsePAMHeader(PCMFile *PCM, BitBuffer *BitB) {
        /* Read Width */
        BitBuffer_Skip(BitB, 48); // Skip "WIDTH " string
        uint64_t WidthStringSize = 0LLU;
        while (ReadBits(MSByteFirst, LSBitFirst, BitB, 8) != PXMFieldSeperator) {
            WidthStringSize += 1;
        }
        char *WidthString = calloc(1, WidthStringSize * sizeof(char));
        for (uint64_t WidthByte = 0; WidthByte < WidthStringSize; WidthByte++) {
            WidthString[WidthByte] = ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
        }
        PCM->Pic->Width = UTF32_String2Integer(UTF8_Decode(WidthString));
        free(WidthString);
        /* Read Width */
        
        /* Read Height */
        BitBuffer_Skip(BitB, 56); // Skip "HEIGHT " string
        uint64_t HeightStringSize = 0LLU;
        while (ReadBits(MSByteFirst, LSBitFirst, BitB, 8) != PXMEndField) {
            HeightStringSize += 1;
        }
        char *HeightString = calloc(1, HeightStringSize * sizeof(char));
        for (uint64_t HeightByte = 0; HeightByte < HeightStringSize; HeightByte++) {
            HeightString[HeightByte] = ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
        }
        PCM->Pic->Height = UTF32_String2Integer(UTF8_Decode(HeightString)); // Ok, so we read the Height.
        free(HeightString);
        /* Read Height */
        
        /* Read NumChannels */
        BitBuffer_Skip(BitB, 48); // Skip "DEPTH " string
        uint8_t DepthStringSize = 0;
        while (ReadBits(MSByteFirst, LSBitFirst, BitB, 8) != PXMEndField) {
            DepthStringSize += 1;
        }
        char *DepthString = calloc(1, DepthStringSize * sizeof(char));
        for (uint8_t DepthByte = 0; DepthByte < DepthStringSize; DepthByte++) {
            DepthString[DepthByte] = ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
        }
        PCM->NumChannels = UTF32_String2Integer(UTF8_Decode(DepthString));
        free(DepthString);
        /* Read NumChannels */
        
        /* Read MaxVal */
        BitBuffer_Skip(BitB, 56); // Skip "MAXVAL " string
        uint8_t MaxValStringSize = 0;
        while (ReadBits(MSByteFirst, LSBitFirst, BitB, 8) != PXMEndField) {
            MaxValStringSize += 1;
        }
        UTF8 *MaxValString = calloc(1, MaxValStringSize * sizeof(UTF8));
        for (uint8_t MaxValByte = 0; MaxValByte < MaxValStringSize; MaxValByte++) {
            MaxValString[MaxValByte] = ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
        }
        int64_t MaxVal    = UTF32_String2Integer(UTF8_Decode(MaxValString));
        PCM->BitDepth     = Logarithm(2, MaxVal + 1);
        free(MaxValString);
        /* Read MaxVal */
        
        /* Read TupleType */
        BitBuffer_Skip(BitB, 72); // Skip "TUPLETYPE " string
        uint8_t TupleTypeSize = 0;
        while (ReadBits(MSByteFirst, LSBitFirst, BitB, 8) != PXMEndField) {
            TupleTypeSize += 1;
        }
        char *TupleTypeString = calloc(1, TupleTypeSize * sizeof(char));
        for (uint8_t TupleByte = 0; TupleByte < TupleTypeSize; TupleByte++) {
            TupleTypeString[TupleByte] = ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
        }
        if (UTF8_Compare(TupleTypeString, U8("BLACKANDWHITE"), NormalizationFormKC, Yes) == Yes) {
            PCM->NumChannels = 1;
            PCM->Pic->PXMTupleType   = PXM_TUPLE_BnW;
        } else if (UTF8_Compare(TupleTypeString, U8("GRAYSCALE"), NormalizationFormKC, Yes) == Yes) {
            PCM->NumChannels = 1;
            PCM->Pic->PXMTupleType   = PXM_TUPLE_Gray;
        } else if (UTF8_Compare(TupleTypeString, U8("GRAYSCALE_ALPHA"), NormalizationFormKC, Yes) == Yes) {
            PCM->NumChannels = 2;
            PCM->Pic->PXMTupleType   = PXM_TUPLE_GrayAlpha;
        } else if (UTF8_Compare(TupleTypeString, U8("RGB"), NormalizationFormKC, Yes) == Yes) {
            PCM->NumChannels = 3;
            PCM->Pic->PXMTupleType   = PXM_TUPLE_RGB;
        } else if (UTF8_Compare(TupleTypeString, U8("RGB_ALPHA"), NormalizationFormKC, Yes) == Yes) {
            PCM->NumChannels = 4;
            PCM->Pic->PXMTupleType   = PXM_TUPLE_RGBAlpha;
        } else {
            PCM->NumChannels = 0;
            PCM->Pic->PXMTupleType   = PXM_TUPLE_Unknown;
            Log(Log_ERROR, __func__, U8("Unknown PXM Tuple: %s"), TupleTypeString);
        }
        free(TupleTypeString);
        /* Read TupleType */
        
        /* Skip ENDHDR */
        BitBuffer_Skip(BitB, 56); // ENDHDR
        /* Skip ENDHDR */
    }
    
    void PXMIdentifyFileType(PCMFile *PCM, BitBuffer *BitB) {
        char PXMMagicID[PXMMagicSize];
        for (uint8_t PXMMagicByte = 0; PXMMagicByte < PXMMagicSize; PXMMagicByte++) {
            PXMMagicID[PXMMagicByte] = ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
        }
        if (UTF8_Compare(PXMMagicID, U8("P1"), NormalizationFormKC, Yes) == Yes || UTF8_Compare(PXMMagicID, U8("P2"), NormalizationFormKC, Yes) == Yes || UTF8_Compare(PXMMagicID, U8("P3"), NormalizationFormKC, Yes) == Yes) {
            PCM->Pic->PXMType = ASCIIPXM;
        } else if (UTF8_Compare(PXMMagicID, U8("P4"), NormalizationFormKC, Yes) == Yes || UTF8_Compare(PXMMagicID, U8("P5"), NormalizationFormKC, Yes) == Yes || UTF8_Compare(PXMMagicID, U8("P6"), NormalizationFormKC, Yes) == Yes) {
            PCM->Pic->PXMType = BinaryPXM;
        } else if (UTF8_Compare(PXMMagicID, U8("P7"), NormalizationFormKC, Yes) == 0) {
            PCM->Pic->PXMType = PAMPXM;
        }
    }
    
    void PXMParseMetadata(PCMFile *PCM, BitBuffer *BitB) {
        uint8_t NumFieldsRead = 0;
        uint8_t Fields2Read = 0;
        if (PCM->Pic->PXMType == PAMPXM) {
            Fields2Read = 5;
        } else if (PCM->Pic->PXMType == ASCIIPXM) {
            Fields2Read = 2; // there is no MaxVal field
        } else if (PCM->Pic->PXMType == BinaryPXM) {
            Fields2Read = 3;
        }
        
        BitBuffer_Skip(BitB, 8); // Skip the LineFeed after the FileType marker
        // Before each field we need to check for Comments if the file is ASCII.
        if (PCM->Pic->PXMType == ASCIIPXM) {
            PXMParsePNMASCIIHeader(PCM, BitB);
        } else if (PCM->Pic->PXMType == BinaryPXM) {
            PXMParsePNMBinaryHeader(PCM, BitB);
        } else if (PCM->Pic->PXMType == PAMPXM) {
            PXMParsePAMHeader(PCM, BitB);
        }
    }
    
    uint16_t ***PXMExtractImage(PCMFile *PCM, BitBuffer *BitB) {
        uint16_t ***ExtractedImage = calloc(PCM->Pic->Width * PCM->Pic->Height * PCM->NumChannels, sizeof(uint16_t));
        if (PCM != NULL && BitB != NULL && ExtractedImage != NULL) {
            if (PCM->Pic->PXMType == ASCIIPXM) {
                for (uint64_t Width = 0; Width < PCM->Pic->Width; Width++) {
                    for (uint64_t Height = 0; Height < PCM->Pic->Height; Height++) {
                        for (uint8_t Channel = 0; Channel < PCM->NumChannels; Channel++) {
                            uint8_t SubPixelStringSize          = 0;
                            while (PeekBits(MSByteFirst, LSBitFirst, BitB, 8) != PXMFieldSeperator || PeekBits(MSByteFirst, LSBitFirst, BitB, 8) != PXMEndField) {
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
            } else if (PCM->Pic->PXMType == BinaryPXM || PCM->Pic->PXMType == PAMPXM) {
                for (uint64_t Width = 0; Width < PCM->Pic->Width; Width++) {
                    for (uint64_t Height = 0; Height < PCM->Pic->Height; Height++) {
                        for (uint8_t Channel = 0; Channel < PCM->NumChannels; Channel++) {
                            uint8_t CurrentPixel                = ReadBits(LSByteFirst, LSBitFirst, BitB, PCM->BitDepth);
                            if (PCM->Pic->PXMTupleType == PXM_TUPLE_BnW && PCM->Pic->PXMType != PAMPXM) {
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
