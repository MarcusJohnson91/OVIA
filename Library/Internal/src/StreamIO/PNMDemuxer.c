#include "../../include/libPCM.h"
#include "../../include/Private/libPCMTypes.h"
#include "../../include/Private/Image/PXMCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void ParsePXMHeader(PCMFile *PCM, BitBuffer *BitB, bool IsPAM) {
        BitBufferSkip(BitB, 8); // Skip the LineFeed
        /* Read Height and Width is for PPM, PBM, AND PGM */
        /* For PAM, We need to skip the WIDTH and HEIGHT strings first */
        if (IsPAM == Yes) {
            // Skip the WIDTH and HEIGHT strings
            BitBufferSkip(BitB, 48); // Skip "WIDTH " string
            BitBufferSkip(BitB, 56); // Skip "HEIGHT " string
        }
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
        /* Read Height */
        
        /* Read NumChannels */
        if (IsPAM == Yes) {
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
        }
        /* Read NumChannels */
        
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
        /* Read MaxVal */
        
        /* Read TupleType */
        if (IsPAM == Yes) {
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
            } else {
                PCM->PXM->TupleType = PXM_TUPLE_Unknown;
            }
            free(TupleTypeString);
        }
        /* Read TupleType */
        
        /* Skip ENDHDR */
        BitBufferSkip(BitB, 56); // ENDHDR
        /* Skip ENDHDR */
    }
    
#ifdef __cplusplus
}
#endif
