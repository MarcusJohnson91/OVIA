#include "../../include/libPCM.h"
#include "../../include/Private/libPCMTypes.h"
#include "../../include/Private/Image/PXMCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    static uint64_t CheckPXMForComment(BitBuffer *BitB) { // returns 0 if no comment was found, returns the number of bytes that make up the comment if it was.
        uint64_t CommentSize = 0;
        if (ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8) == 0x23) {
            while (ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8) != 0x0A) {
                CommentSize += 1;
            }
        }
        return CommentSize;
    }
    
    void IdentifyPXMFile(PCMFile *PCM, BitBuffer *BitB) {
        char PXMMagicID[PXMMagicSize];
        for (uint8_t PXMMagicByte = 0; PXMMagicByte < PXMMagicSize; PXMMagicByte++) {
            PXMMagicID[PXMMagicByte] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
        }
      	if (strncasecmp(PXMMagicID, "P1", PXMMagicSize) == 0 || strncasecmp(PXMMagicID, "P2", PXMMagicSize) == 0 || strncasecmp(PXMMagicID, "P3", PXMMagicSize) == 0) {
            PCM->PXM->PXMType = ASCIIPXM;
        } else if (strncasecmp(PXMMagicID, "P4", PXMMagicSize) == 0 || strncasecmp(PXMMagicID, "P5", PXMMagicSize) == 0 || strncasecmp(PXMMagicID, "P6", PXMMagicSize) == 0) {
            PCM->PXM->PXMType = BinaryPXM;
        } else if (strncasecmp(PXMMagicID, "P7", PXMMagicSize) == 0) {
            PCM->PXM->PXMType = PAMPXM;
        }
    }
    
    void ParsePXMHeader(PCMFile *PCM, BitBuffer *BitB, uint64_t FileSize, bool IsASCII, bool IsPAM) {
        uint8_t NumFieldsRead = 0;
        uint8_t Fields2Read = 0;
        if (IsPAM == Yes) {
            Fields2Read = 5;
        } else if (IsPAM == No && IsASCII == Yes) {
            Fields2Read = 2; // there is no MaxVal field
        } else if (IsPAM == No && IsASCII == No) {
            Fields2Read = 3;
        }
        
        /* What if we were to find out the size of the file, and loop over each byte of the file until we found the last element and while we did tht, we skipped over each comment? */
        /* You do not need to worry about comments being in the middle of fields, just between them. */
        
        BitBufferSkip(BitB, 8); // Skip the LineFeed after the FileType marker
        
        while (NumFieldsRead < Fields2Read) {
            // Before each field we need to check for Comments if the file is ASCII.
            if (PCM->PXM->PXMType == ASCIIPXM) {
                uint64_t CommentSizeWidth = CheckPXMForComment(BitB);
                BitBufferSkip(BitB, Bytes2Bits(CommentSizeWidth));
                /* Read Width */
                uint64_t WidthStringSize = 0ULL;
                while (ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8) != 0x20) {
                    WidthStringSize += 1;
                }
                char *WidthString = calloc(1, WidthStringSize);
                for (uint64_t WidthByte = 0; WidthByte < WidthStringSize; WidthByte++) {
                    WidthString[WidthByte] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
                }
                PCM->PXM->Width = atoll(WidthString);
                free(WidthString);
                NumFieldsRead += 1;
                /* Read Width */
                
                uint64_t CommentSizeHeight = CheckPXMForComment(BitB);
                BitBufferSkip(BitB, Bytes2Bits(CommentSizeHeight));
                
                /* Read Height */
                uint64_t HeightStringSize = 0ULL;
                while (ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8) != 0x0A) {
                    HeightStringSize += 1;
                }
                char *HeightString = calloc(1, HeightStringSize);
                for (uint64_t HeightByte = 0; HeightByte < HeightStringSize; HeightByte++) {
                    HeightString[HeightByte] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
                }
                PCM->PXM->Height = atoll(HeightString); // Ok, so we read the Height.
                free(HeightString);
                NumFieldsRead += 1;
                /* Read Height */
            } else if (PCM->PXM->PXMType == BinaryPXM) {
                /* Read Width */
                uint64_t WidthStringSize = 0ULL;
                while (ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8) != 0x20) {
                    WidthStringSize += 1;
                }
                char *WidthString = calloc(1, WidthStringSize);
                for (uint64_t WidthByte = 0; WidthByte < WidthStringSize; WidthByte++) {
                    WidthString[WidthByte] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
                }
                PCM->PXM->Width = atoll(WidthString);
                free(WidthString);
                NumFieldsRead += 1;
                /* Read Width */
                
                /* Read Height */
                uint64_t HeightStringSize = 0ULL;
                while (ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8) != 0x0A) {
                    HeightStringSize += 1;
                }
                char *HeightString = calloc(1, HeightStringSize);
                for (uint64_t HeightByte = 0; HeightByte < HeightStringSize; HeightByte++) {
                    HeightString[HeightByte] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
                }
                PCM->PXM->Height = atoll(HeightString); // Ok, so we read the Height.
                free(HeightString);
                NumFieldsRead += 1;
                /* Read Height */
                
                /* Read MaxVal */
                if (IsPAM == Yes) {
                    BitBufferSkip(BitB, 56); // Skip "MAXVAL " string
                }
                uint8_t MaxValStringSize = 0;
                while (ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8) != 0x0A) {
                    MaxValStringSize += 1;
                }
                char *MaxValString = calloc(1, MaxValStringSize);
                for (uint8_t MaxValByte = 0; MaxValByte < MaxValStringSize; MaxValByte++) {
                    MaxValString[MaxValByte] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
                }
                PCM->PXM->MaxVal = atoll(MaxValString);
                free(MaxValString);
                NumFieldsRead += 1;
                /* Read MaxVal */
            } else if (PCM->PXM->PXMType == PAMPXM) {
                /* Read Width */
                BitBufferSkip(BitB, 48); // Skip "WIDTH " string
                uint64_t WidthStringSize = 0ULL;
                while (ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8) != 0x20) {
                    WidthStringSize += 1;
                }
                char *WidthString = calloc(1, WidthStringSize);
                for (uint64_t WidthByte = 0; WidthByte < WidthStringSize; WidthByte++) {
                    WidthString[WidthByte] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
                }
                PCM->PXM->Width = atoll(WidthString);
                free(WidthString);
                NumFieldsRead += 1;
                /* Read Width */
                
                /* Read Height */
                BitBufferSkip(BitB, 56); // Skip "HEIGHT " string
                uint64_t HeightStringSize = 0ULL;
                while (ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8) != 0x0A) {
                    HeightStringSize += 1;
                }
                char *HeightString = calloc(1, HeightStringSize);
                for (uint64_t HeightByte = 0; HeightByte < HeightStringSize; HeightByte++) {
                    HeightString[HeightByte] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
                }
                PCM->PXM->Height = atoll(HeightString); // Ok, so we read the Height.
                free(HeightString);
                NumFieldsRead += 1;
                /* Read Height */
                
                /* Read NumChannels */
                BitBufferSkip(BitB, 48); // Skip "DEPTH " string
                uint8_t DepthStringSize = 0;
                while (ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8) != 0x0A) {
                    DepthStringSize += 1;
                }
                char *DepthString = calloc(1, DepthStringSize);
                for (uint8_t DepthByte = 0; DepthByte < DepthStringSize; DepthByte++) {
                    DepthString[DepthByte] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
                }
                PCM->PXM->NumChannels = atoll(DepthString);
                free(DepthString);
                NumFieldsRead += 1;
                /* Read NumChannels */
                
                /* Read MaxVal */
                BitBufferSkip(BitB, 56); // Skip "MAXVAL " string
                uint8_t MaxValStringSize = 0;
                while (ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8) != 0x0A) {
                    MaxValStringSize += 1;
                }
                char *MaxValString = calloc(1, MaxValStringSize);
                for (uint8_t MaxValByte = 0; MaxValByte < MaxValStringSize; MaxValByte++) {
                    MaxValString[MaxValByte] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
                }
                PCM->PXM->MaxVal = atoll(MaxValString);
                free(MaxValString);
                NumFieldsRead += 1;
                /* Read MaxVal */
                
                /* Read TupleType */
                BitBufferSkip(BitB, 72); // Skip "TUPLETYPE " string
                uint8_t TupleTypeSize = 0;
                while (ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8) != 0x0A) {
                    TupleTypeSize += 1;
                }
                char *TupleTypeString = calloc(1, TupleTypeSize);
                for (uint8_t TupleByte = 0; TupleByte < TupleTypeSize; TupleByte++) {
                    TupleTypeString[TupleByte] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
                }
                if (strcasecmp(TupleTypeString, "RGB") == 0) {
                    PCM->PXM->TupleType = PXM_TUPLE_RGB;
                } else if (strcasecmp(TupleTypeString, "RGB_ALPHA") == 0) {
                    PCM->PXM->TupleType = PXM_TUPLE_RGBAlpha;
                } else if (strcasecmp(TupleTypeString, "GRAYSCALE_ALPHA") == 0) {
                    PCM->PXM->TupleType = PXM_TUPLE_GrayAlpha;
                }
                
                else {
                    PCM->PXM->TupleType = PXM_TUPLE_Unknown;
                }
                free(TupleTypeString);
                NumFieldsRead += 1;
                /* Read TupleType */
                
                /* Skip ENDHDR */
                BitBufferSkip(BitB, 56); // ENDHDR
                /* Skip ENDHDR */
            }
        }
    }
    
    }
    
#ifdef __cplusplus
}
#endif
