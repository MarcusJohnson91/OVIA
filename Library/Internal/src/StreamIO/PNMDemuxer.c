#include "../../include/libPCM.h"
#include "../../include/Private/libPCMTypes.h"
#include "../../include/Private/Image/PXMCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    static uint64_t PXMCheckForComment(BitBuffer *BitB) { // returns 0 if no comment was found, returns the number of bytes that make up the comment if it was.
        uint64_t CommentSize = 0;
        if (ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8) == PXMCommentStart) {
            while (ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8) != PXMEndField) {
                CommentSize += 1;
            }
        }
        return CommentSize;
    }
    
    void PXMIdentifyFileType(PCMFile *PCM, BitBuffer *BitB) {
        char PXMMagicID[PXMMagicSize];
        for (uint8_t PXMMagicByte = 0; PXMMagicByte < PXMMagicSize; PXMMagicByte++) {
            PXMMagicID[PXMMagicByte] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
        }
      	if (strncasecmp(PXMMagicID, "P1", PXMMagicSize) == 0 || strncasecmp(PXMMagicID, "P2", PXMMagicSize) == 0 || strncasecmp(PXMMagicID, "P3", PXMMagicSize) == 0) {
            PCM->PIC->PXMType = ASCIIPXM;
        } else if (strncasecmp(PXMMagicID, "P4", PXMMagicSize) == 0 || strncasecmp(PXMMagicID, "P5", PXMMagicSize) == 0 || strncasecmp(PXMMagicID, "P6", PXMMagicSize) == 0) {
            PCM->PIC->PXMType = BinaryPXM;
        } else if (strncasecmp(PXMMagicID, "P7", PXMMagicSize) == 0) {
            PCM->PIC->PXMType = PAMPXM;
        }
    }
    
    static void PXMParsePNMASCIIHeader(PCMFile *PCM, BitBuffer *BitB) {
        uint64_t CommentSizeWidth = PXMCheckForComment(BitB);
        BitBuffer_Skip(BitB, Bytes2Bits(CommentSizeWidth));
        /* Read Width */
        uint64_t WidthStringSize = 0ULL;
        while (ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8) != PXMFieldSeperator) {
            WidthStringSize += 1;
        }
        char *WidthString = calloc(1, WidthStringSize * sizeof(char));
        for (uint64_t WidthByte = 0; WidthByte < WidthStringSize; WidthByte++) {
            WidthString[WidthByte] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
        }
        PCM->PIC->Width = atoll(WidthString);
        free(WidthString);
        /* Read Width */
        
        uint64_t CommentSizeHeight = PXMCheckForComment(BitB);
        BitBuffer_Skip(BitB, Bytes2Bits(CommentSizeHeight));
        
        /* Read Height */
        uint64_t HeightStringSize = 0ULL;
        while (ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8) != PXMEndField) {
            HeightStringSize += 1;
        }
        char *HeightString = calloc(1, HeightStringSize * sizeof(char));
        for (uint64_t HeightByte = 0; HeightByte < HeightStringSize; HeightByte++) {
            HeightString[HeightByte] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
        }
        PCM->PIC->Height = atoll(HeightString); // Ok, so we read the Height.
        free(HeightString);
    }
    
    static void PXMParsePNMBinaryHeader(PCMFile *PCM, BitBuffer *BitB) {
        /* Read Width */
        uint64_t WidthStringSize = 0ULL;
        while (ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8) != PXMFieldSeperator) {
            WidthStringSize += 1;
        }
        char *WidthString = calloc(1, WidthStringSize * sizeof(char));
        for (uint64_t WidthByte = 0; WidthByte < WidthStringSize; WidthByte++) {
            WidthString[WidthByte] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
        }
        PCM->PIC->Width = atoll(WidthString);
        free(WidthString);
        /* Read Width */
        
        /* Read Height */
        uint64_t HeightStringSize = 0ULL;
        while (ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8) != PXMEndField) {
            HeightStringSize += 1;
        }
        char *HeightString = calloc(1, HeightStringSize * sizeof(char));
        for (uint64_t HeightByte = 0; HeightByte < HeightStringSize; HeightByte++) {
            HeightString[HeightByte] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
        }
        PCM->PIC->Height = atoll(HeightString); // Ok, so we read the Height.
        free(HeightString);
        /* Read Height */
        
        /* Read MaxVal */
        uint8_t MaxValStringSize = 0;
        while (ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8) != PXMEndField) {
            MaxValStringSize += 1;
        }
        char *MaxValString = calloc(1, MaxValStringSize * sizeof(char));
        for (uint8_t MaxValByte = 0; MaxValByte < MaxValStringSize; MaxValByte++) {
            MaxValString[MaxValByte] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
        }
        uint64_t MaxVal    = atoll(MaxValString);
        PCM->BitDepth = log2(MaxVal + 1);
        free(MaxValString);
        /* Read MaxVal */
    }
    
    static void PXMParsePAMHeader(PCMFile *PCM, BitBuffer *BitB) {
        /* Read Width */
        BitBuffer_Skip(BitB, 48); // Skip "WIDTH " string
        uint64_t WidthStringSize = 0ULL;
        while (ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8) != PXMFieldSeperator) {
            WidthStringSize += 1;
        }
        char *WidthString = calloc(1, WidthStringSize * sizeof(char));
        for (uint64_t WidthByte = 0; WidthByte < WidthStringSize; WidthByte++) {
            WidthString[WidthByte] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
        }
        PCM->PIC->Width = atoll(WidthString);
        free(WidthString);
        /* Read Width */
        
        /* Read Height */
        BitBuffer_Skip(BitB, 56); // Skip "HEIGHT " string
        uint64_t HeightStringSize = 0ULL;
        while (ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8) != PXMEndField) {
            HeightStringSize += 1;
        }
        char *HeightString = calloc(1, HeightStringSize * sizeof(char));
        for (uint64_t HeightByte = 0; HeightByte < HeightStringSize; HeightByte++) {
            HeightString[HeightByte] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
        }
        PCM->PIC->Height = atoll(HeightString); // Ok, so we read the Height.
        free(HeightString);
        /* Read Height */
        
        /* Read NumChannels */
        BitBuffer_Skip(BitB, 48); // Skip "DEPTH " string
        uint8_t DepthStringSize = 0;
        while (ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8) != PXMEndField) {
            DepthStringSize += 1;
        }
        char *DepthString = calloc(1, DepthStringSize * sizeof(char));
        for (uint8_t DepthByte = 0; DepthByte < DepthStringSize; DepthByte++) {
            DepthString[DepthByte] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
        }
        PCM->NumChannels = atoll(DepthString);
        free(DepthString);
        /* Read NumChannels */
        
        /* Read MaxVal */
        BitBuffer_Skip(BitB, 56); // Skip "MAXVAL " string
        uint8_t MaxValStringSize = 0;
        while (ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8) != PXMEndField) {
            MaxValStringSize += 1;
        }
        char *MaxValString = calloc(1, MaxValStringSize * sizeof(char));
        for (uint8_t MaxValByte = 0; MaxValByte < MaxValStringSize; MaxValByte++) {
            MaxValString[MaxValByte] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
        }
        uint64_t MaxVal    = atoll(MaxValString);
        PCM->BitDepth = log2(MaxVal + 1);
        free(MaxValString);
        /* Read MaxVal */
        
        /* Read TupleType */
        BitBuffer_Skip(BitB, 72); // Skip "TUPLETYPE " string
        uint8_t TupleTypeSize = 0;
        while (ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8) != PXMEndField) {
            TupleTypeSize += 1;
        }
        char *TupleTypeString = calloc(1, TupleTypeSize * sizeof(char));
        for (uint8_t TupleByte = 0; TupleByte < TupleTypeSize; TupleByte++) {
            TupleTypeString[TupleByte] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
        }
        if (strcasecmp(TupleTypeString, "BLACKANDWHITE") == 0) {
            PCM->NumChannels = 1;
            PCM->PIC->PXMTupleType   = PXM_TUPLE_BnW;
        } else if (strcasecmp(TupleTypeString, "GRAYSCALE") == 0) {
            PCM->NumChannels = 1;
            PCM->PIC->PXMTupleType   = PXM_TUPLE_Gray;
        } else if (strcasecmp(TupleTypeString, "GRAYSCALE_ALPHA") == 0) {
            PCM->NumChannels = 2;
            PCM->PIC->PXMTupleType   = PXM_TUPLE_GrayAlpha;
        } else if (strcasecmp(TupleTypeString, "RGB") == 0) {
            PCM->NumChannels = 3;
            PCM->PIC->PXMTupleType   = PXM_TUPLE_RGB;
        } else if (strcasecmp(TupleTypeString, "RGB_ALPHA") == 0) {
            PCM->NumChannels = 4;
            PCM->PIC->PXMTupleType   = PXM_TUPLE_RGBAlpha;
        } else {
            PCM->NumChannels = 0;
            PCM->PIC->PXMTupleType   = PXM_TUPLE_Unknown;
            BitIOLog(LOG_ERROR, "libPXM", __func__, "Unknown PXM Tuple: %s", TupleTypeString);
        }
        free(TupleTypeString);
        /* Read TupleType */
        
        /* Skip ENDHDR */
        BitBuffer_Skip(BitB, 56); // ENDHDR
        /* Skip ENDHDR */
    }
    
    void PXMParseMetadata(PCMFile *PCM, BitBuffer *BitB) {
        uint8_t NumFieldsRead = 0;
        uint8_t Fields2Read = 0;
        if (PCM->PIC->PXMType == PAMPXM) {
            Fields2Read = 5;
        } else if (PCM->PIC->PXMType == ASCIIPXM) {
            Fields2Read = 2; // there is no MaxVal field
        } else if (PCM->PIC->PXMType == BinaryPXM) {
            Fields2Read = 3;
        }
        
        BitBuffer_Skip(BitB, 8); // Skip the LineFeed after the FileType marker
                                // Before each field we need to check for Comments if the file is ASCII.
        if (PCM->PIC->PXMType == ASCIIPXM) {
            PXMParsePNMASCIIHeader(PCM, BitB);
        } else if (PCM->PIC->PXMType == BinaryPXM) {
            PXMParsePNMBinaryHeader(PCM, BitB);
        } else if (PCM->PIC->PXMType == PAMPXM) {
            PXMParsePAMHeader(PCM, BitB);
        }
    }
    
    uint16_t **PXMExtractPixels(PCMFile *PCM, BitBuffer *BitB, uint64_t NumPixels2Read) {
        uint64_t PixelArraySize = NumPixels2Read * PCM->NumChannels * Bits2Bytes(PCM->BitDepth, Yes);
        uint16_t **PixelArray = calloc(1, PixelArraySize * sizeof(uint16_t));
        if (PixelArray == NULL) {
            BitIOLog(LOG_ERROR, libPCMLibraryName, __func__, "Couldn't allocate %d bytes for the PixelArray", PixelArraySize);
        } else {
            if (PCM->PIC->PXMType == ASCIIPXM) {
                if (PCM->PIC->PXMTupleType == PXM_TUPLE_BnW) {
                    // 1 = black, 0 = white
                }
                for (uint64_t Pixel = 0ULL; Pixel < NumPixels2Read; Pixel++) {
                    for (uint8_t Channel = 0; Channel < PCM->NumChannels; Channel++) {
                        uint8_t SubPixelStringSize = 0;
                        while (PeekBits(BitIOMSByte, BitIOLSBit, BitB, 8) != PXMFieldSeperator || PeekBits(BitIOMSByte, BitIOLSBit, BitB, 8) != PXMEndField) {
                            SubPixelStringSize += 1;
                        }
                        char *SubPixelString = calloc(1, PCM->NumChannels * sizeof(char));
                        for (uint8_t SubPixelByte = 0; SubPixelByte < SubPixelStringSize; SubPixelByte++) {
                            SubPixelString[SubPixelByte] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
                        }
                        PixelArray[Channel][Pixel]       = atoi(SubPixelString);
                    }
                }
            } else if (PCM->PIC->PXMType == BinaryPXM || PCM->PIC->PXMType == PAMPXM) {
                for (uint64_t Pixel = 0ULL; Pixel < NumPixels2Read; Pixel++) {
                    for (uint8_t Channel = 0; Channel < PCM->NumChannels; Channel++) {
                        uint8_t CurrentPixel       = ReadBits(BitIOLSByte, BitIOLSBit, BitB, PCM->BitDepth);
                        if (PCM->PIC->PXMTupleType == PXM_TUPLE_BnW && PCM->PIC->PXMType != PAMPXM) {
                            PixelArray[Channel][Pixel] = ~CurrentPixel; // 1 = black, 0 = white
                        } else {
                            PixelArray[Channel][Pixel] = CurrentPixel; // 1 = white, 0 = black
                        }
                    }
                }
            }
        }
        return NULL;
    }
    
#ifdef __cplusplus
}
#endif
