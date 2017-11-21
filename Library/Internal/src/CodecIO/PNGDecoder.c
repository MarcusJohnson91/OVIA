#include "../../include/libModernPNG.h"
#include "../../include/Private/libModernPNG_Types.h"

#include "../../include/Private/Decode/libModernPNG_Decode.h"
#include "../../include/Private/Decode/libModernPNG_ReadChunks.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#define strcasecmp _stricmp
#endif
    
    uint32_t CalculateSTERPadding(const uint32_t Width) {
        return 7 - ((Width - 1) % 8);
    }
    
    DecodePNG *DecodePNG_Init(void) {
        DecodePNG *Dec  = calloc(1, sizeof(DecodePNG));
        Dec->acTL       = calloc(1, sizeof(acTL));
        Dec->bkGD       = calloc(1, sizeof(bkGD));
        Dec->cHRM       = calloc(1, sizeof(cHRM));
        Dec->fcTL       = calloc(1, sizeof(fcTL));
        Dec->fdAT       = calloc(1, sizeof(fdAT));
        Dec->gAMA       = calloc(1, sizeof(gAMA));
        Dec->hIST       = calloc(1, sizeof(hIST));
        Dec->iCCP       = calloc(1, sizeof(iCCP));
        Dec->iHDR       = calloc(1, sizeof(iHDR));
        Dec->oFFs       = calloc(1, sizeof(oFFs));
        Dec->pCAL       = calloc(1, sizeof(pCAL));
        Dec->PLTE       = calloc(1, sizeof(PLTE));
        Dec->sBIT       = calloc(1, sizeof(sBIT));
        Dec->sRGB       = calloc(1, sizeof(sRGB));
        Dec->sTER       = calloc(1, sizeof(sTER));
        Dec->Text       = calloc(1, sizeof(Text));
        Dec->tIMe       = calloc(1, sizeof(tIMe));
        Dec->tRNS       = calloc(1, sizeof(tRNS));
        return Dec;
    }
    
    void DecodePNG_Deinit(DecodePNG *Dec) {
        if (Dec->acTLExists) {
            free(Dec->acTL);
        }
        if (Dec->bkGDExists) {
            free(Dec->bkGD->BackgroundPaletteEntry);
            free(Dec->bkGD);
        }
        if (Dec->cHRMExists) {
            free(Dec->cHRM);
        }
        if (Dec->fcTLExists) {
            free(Dec->fcTL);
            free(Dec->fdAT);
        }
        if (Dec->gAMAExists) {
            free(Dec->gAMA);
        }
        if (Dec->hISTExists) {
            free(Dec->hIST);
        }
        if (Dec->iCCPExists) {
            free(Dec->iCCP->CompressedICCPProfile);
            free(Dec->iCCP->ProfileName);
            free(Dec->iCCP);
        }
        if (Dec->oFFsExists) {
            free(Dec->oFFs);
        }
        if (Dec->pCALExists) {
            free(Dec->pCAL->CalibrationName);
            free(Dec->pCAL->UnitName);
            free(Dec->pCAL);
        }
        if (Dec->PLTEExists) {
            free(Dec->PLTE->Palette);
            free(Dec->PLTE);
        }
        if (Dec->sBITExists) {
            free(Dec->sBIT);
        }
        if (Dec->sRGBExists) {
            free(Dec->sRGB);
        }
        if (Dec->sTERExists) {
            free(Dec->sTER);
        }
        if (Dec->TextExists) {
            free(Dec->Text->Keyword);
            free(Dec->Text->TextString);
            free(Dec->Text);
        }
        if (Dec->tIMEExists) {
            free(Dec->tIMe);
        }
        if (Dec->tRNSExists) {
            free(Dec->tRNS->Palette);
            free(Dec->tRNS);
        }
        free(Dec->iHDR);
        free(Dec);
    }
    
    /*
    bool VerifyChunkCRC(BitBuffer *BitB, uint32_t ChunkSize) {
        // So basically we need to read ChunkSize bytes into an array, then read the following 4 bytes as the CRC
        // then run VerifyCRC over the buffer, and finally compare the generated CRC with the extracted one, and return whether they match.
        // Then call SkipBits(BitB, Bytes2Bits(ChunkSize)); to reset to the beginning of the chunk
        uint8_t *Buffer2CRC = calloc(1, ChunkSize * sizeof(uint8_t));
        for (uint32_t Byte = 0; Byte < ChunkSize; Byte++) {
            Buffer2CRC[Byte] = BitB->Buffer[Bits2Bytes(, false)];
            free(Buffer2CRC);
            
        }
        uint32_t ChunkCRC = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32);
        bool CRCsMatch = VerifyCRC(Buffer2CRC, ChunkSize, 0x82608EDB, 32, 0xFFFFFFFF, ChunkCRC);
        SkipBits(BitB, -Bytes2Bits(ChunkSize));
        return CRCsMatch;
    }
    */
    
    void PNGDecodeNonFilter(DecodePNG *Dec, uint8_t *DeEntropyedData, uint8_t *DeFilteredData, size_t Line) {
        for (size_t Byte = 1; Byte < Bits2Bytes(Dec->iHDR->BitDepth, true); Byte++) {
            DeFilteredData[Byte - 1] = DeEntropyedData[Byte]; // Remove filter indicating byte
        }
    }
    
    void PNGDecodeSubFilter(DecodePNG *Dec, uint8_t **DeEntropyedData, uint8_t **DeFilteredData, size_t Line) {
        for (size_t Byte = 1; Byte < Bits2Bytes(Dec->iHDR->BitDepth, true); Byte++) {
            DeFilteredData[Line][Byte - 1] = (DeEntropyedData[Line][Byte] + DeEntropyedData[Line][Byte+1]) & 0xFF;
        }
    }
    
    void PNGDecodeUpFilter(DecodePNG *Dec, uint8_t **DeEntropyedData, uint8_t **DeFilteredData, size_t Line) {
        for (size_t Byte = 1; Byte < Bits2Bytes(Dec->iHDR->BitDepth, true); Byte++) {
            DeFilteredData[Line][Byte - 1] = DeEntropyedData[Line][Byte] + DeEntropyedData[Line - 1][Byte] & 0xFF;
        }
    }
    
    void PNGDecodeAverageFilter(DecodePNG *Dec, uint8_t **DeEntropyedData, uint8_t **DeFilteredData, size_t Line) {
        uint8_t PixelSize = Bits2Bytes(Dec->iHDR->BitDepth, true);
        for (size_t Byte = 1; Byte < Bits2Bytes(Dec->iHDR->BitDepth, true); Byte++) {
            uint8_t Average = floor((DeEntropyedData[Line][Byte - (PixelSize)] + DeEntropyedData[Line - 1][Byte]) / 2);
            DeFilteredData[Line][Byte - 1] = DeEntropyedData[Line][Byte] + Average;
        }
    }
    
    uint8_t PaethPredictor(int64_t Left, int64_t Above, int64_t UpperLeft) {
        int64_t Guess     = Left + Above - UpperLeft;
        int64_t DistanceA = llabs(Guess - Left);
        int64_t DistanceB = llabs(Guess - Above);
        int64_t DistanceC = llabs(Guess - UpperLeft);
        
        uint8_t Output = 0;
        if (DistanceA <= DistanceB && DistanceA < DistanceC) {
            Output = DistanceA;
        } else if (DistanceB < DistanceC) {
            Output = DistanceB;
        } else {
            Output = DistanceC;
        }
        return Output;
    }
    
    void PNGDecodePaethFilter(DecodePNG *Dec, uint8_t **DeEntropyedData, uint8_t **DeFilteredData, size_t Line) {
        // Filtering is applied to bytes, not pixels
        uint8_t PixelSize = Bits2Bytes(Dec->iHDR->BitDepth, true);
        for (size_t Byte = 1; Byte < Bits2Bytes(Dec->iHDR->BitDepth, true); Byte++) {
            if (Line == 0) { // Assume top and top left = 0
                DeFilteredData[Line][Byte] = PaethPredictor(DeEntropyedData[Line][Byte], 0, 0);
            } else {
                DeFilteredData[Line][Byte] = PaethPredictor(DeEntropyedData[Line][Byte], DeEntropyedData[Line][Byte - PixelSize], DeEntropyedData[Line - 1][Byte - PixelSize]);
            }
        }
    }
    
    // These filters need to be operatable on every line in any order, so we need a main PNGDecodeFilteredImage function.
    // That function then needs to read the first byte of each line to see what filter is used
    // then decode each line.
    // ALSO keep in mind concurrency.
    
    void PNGDecodeFilteredImage(DecodePNG *Dec, uint8_t ***InflatedBuffer) {
        
		uint8_t *DeFilteredData = calloc(1, (Dec->iHDR->Height * Dec->iHDR->Width) * Bits2Bytes(Dec->iHDR->BitDepth, Yes));
        
        for (size_t Line = 0; Line < Dec->iHDR->Height; Line++) {
            uint8_t FilterType = *InflatedBuffer[Line][0];
            switch (FilterType) {
                case 0:
                    // copy the Line except byte 0 (the filter indication byte) to the output buffer.
                    PNGDecodeNonFilter(Dec, *InflatedBuffer, DeFilteredData, Line);
                    break;
                case 1:
                    // SubFilter
                    PNGDecodeSubFilter(Dec, *InflatedBuffer, &DeFilteredData, Line);
                    break;
                case 2:
                    // UpFilter
                    PNGDecodeUpFilter(Dec, *InflatedBuffer, &DeFilteredData, Line);
                    break;
                case 3:
                    // AverageFilter
                    PNGDecodeAverageFilter(Dec, *InflatedBuffer, &DeFilteredData, Line);
                    break;
                case 4:
                    // PaethFilter
                    PNGDecodePaethFilter(Dec, *InflatedBuffer, &DeFilteredData, Line);
                    break;
                default:
                    BitIOLog(BitIOLog_ERROR, "ModernPNG", __func__, "Filter type: %d is invalid\n", FilterType);
                    break;
            }
        }
        free(DeFilteredData);
    }
    
    void DecodePNGData(DecodePNG *Dec, BitBuffer *BitB) {
        // read the iDAT/fDAT chunk header, then do the other stuff.
        while (BitBuffer_GetSize(BitB) > 0) { // 12 is the start of IEND
            uint32_t ChunkSize = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32);
            uint32_t ChunkID   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32);
            
            if (strcasecmp(ChunkID, "iDAT") == 0) {
                ParseIDAT(Dec, BitB, ChunkSize);
            } else if (strcasecmp(ChunkID, "acTL") == 0) {
                
            } else if (strcasecmp(ChunkID, "fdAT") == 0) {
                ParseFDAT(Dec, BitB, ChunkSize);
            }
        }
    }
    
    // Let's do this library right, by adding attach and delete functions for the various chunks, and let's also have a fancy function that applies color profiles to the pixels.
    // that's kinda a lot of work tho...
    uint16_t ***DecodePNGImage(DecodePNG *Dec, BitBuffer *PNGFile) {
        return NULL;
    }
    
    BitBuffer *DecodeAdam7(DecodePNG *Dec, BitBuffer *InterlacedImage) { // Returns the interlaced image
        /*
         Ok, so to interlace an image, we need to loop over every pixel in an 8x8 block?
         */
        BitBuffer *ProgressiveImage = NULL;
        return ProgressiveImage;
    }
    
#ifdef __cplusplus
}
#endif
