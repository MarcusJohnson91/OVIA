#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include "../../../Dependencies/BitIO/libBitIO/include/BitIO.h"

#include "../../include/libModernPNG.h"
#include "../../include/Decoder/DecodePNG.h"
#include "../../include/PNGTypes.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    
    
    /*
     bool VerifyChunkCRC(BitInput *BitI, uint32_t ChunkSize) {
     // So basically we need to read ChunkSize bytes into an array, then read the following 4 bytes as the CRC
     // then run VerifyCRC over the buffer, and finally compare the generated CRC with the extracted one, and return whether they match.
     // Then call SkipBits(BitI, Bytes2Bits(ChunkSize)); to reset to the beginning of the chunk
     uint8_t *Buffer2CRC = calloc(1, ChunkSize);
     for (uint32_t Byte = 0; Byte < ChunkSize; Byte++) {
     Buffer2CRC[Byte] = BitI->Buffer[Bits2Bytes(, false)];
     
     }
     uint32_t ChunkCRC = ReadBits(BitI, 32, true);
     bool CRCsMatch = VerifyCRC(Buffer2CRC, ChunkSize, 0x82608EDB, 32, 0xFFFFFFFF, ChunkCRC);
     SkipBits(BitI, -Bytes2Bits(ChunkSize));
     return CRCsMatch;
     }
     */
    
    enum Adam7Positions {
        Adam7Level1    =  0,
        Adam7Level2    =  5,
        Adam7Level3_1  = 32,
        Adam7Level3_2  = 36,
        Adam7Level4_1  =  3,
        Adam7Level4_2  =  7,
        Adam7Level4_3  = 35,
        Adam7Level4_4  = 39,
        Adam7Level5_1  = 17,
        Adam7Level5_2  = 19,
        Adam7Level5_3  = 21,
        Adam7Level5_4  = 23,
        Adam7Level5_5  = 49,
        Adam7Level5_6  = 51,
        Adam7Level5_7  = 53,
        Adam7Level5_8  = 55,
        Adam7Level6_1  =  2,
        Adam7Level6_2  =  4,
        Adam7Level6_3  =  6,
        Adam7Level6_4  =  8,
        Adam7Level6_5  = 18,
        Adam7Level6_6  = 20,
        Adam7Level6_7  = 22,
        Adam7Level6_8  = 24,
        Adam7Level6_9  = 34,
        Adam7Level6_10 = 36,
        Adam7Level6_11 = 38,
        Adam7Level6_12 = 40,
        Adam7Level6_13 = 50,
        Adam7Level6_14 = 52,
        Adam7Level6_15 = 54,
        Adam7Level6_16 = 56,
    };
    
    static const uint8_t Adam7Level5[8] = {
        17, 19, 21, 23, 49, 51, 53, 55
    };
    
    static const uint8_t Adam7Level6[16] = {
         2,  4,  6,  8, 18, 20, 22, 24,
        34, 36, 38, 40, 50, 52, 54, 56,
    };
    
    static const uint8_t Adam7Level7[32] = {
         9, 10, 11, 12, 13, 14, 15, 16,
        25, 26, 27, 28, 29, 30, 31, 32,
        41, 42, 43, 44, 45, 56, 47, 48,
        57, 58, 59, 60, 61, 62, 63, 64
    };
    
    void CalculateSTERPadding(DecodePNG *Dec) {
        Dec->LinePadding = 7 - ((Dec->iHDR->Width - 1) % 8);
        Dec->LineWidth   = (Dec->iHDR->Width * 2) + Dec->LinePadding;
    }
    
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
    
    void PNGDecodeFilter(DecodePNG *Dec, uint8_t ***InflatedBuffer) {
        char Error[BitIOStringSize];
        
        uint8_t DeFilteredData[Dec->iHDR->Height][Dec->iHDR->Width - 1];
        
        for (size_t Line = 0; Line < Dec->iHDR->Height; Line++) {
            uint8_t FilterType = *InflatedBuffer[Line][0];
            switch (FilterType) {
                case 0:
                    // copy the Line except byte 0 (the filter indication byte) to the output buffer.
                    PNGDecodeNonFilter(Dec, *InflatedBuffer, DeFilteredData, Line);
                    break;
                case 1:
                    // SubFilter
                    PNGDecodeSubFilter(Dec, *InflatedBuffer, DeFilteredData, Line);
                    break;
                case 2:
                    // UpFilter
                    PNGDecodeUpFilter(Dec, *InflatedBuffer, DeFilteredData, Line);
                    break;
                case 3:
                    // AverageFilter
                    PNGDecodeAverageFilter(Dec, *InflatedBuffer, DeFilteredData, Line);
                    break;
                case 4:
                    // PaethFilter
                    PNGDecodePaethFilter(Dec, *InflatedBuffer, DeFilteredData, Line);
                    break;
                default:
                    snprintf(Error, BitIOStringSize, "Filter type: %d is invalid\n", FilterType);
                    Log(LOG_ERR, "ModernPNG", "PNGDecodeFilteredLine", Error);
                    break;
            }
        }
    }
    
    void DecodePNGData(BitInput *BitI, DecodePNG *Dec) {
        // read the iDAT/fDAT chunk header, then do the other stuff.
        while (BytesRemainingInFile(BitI) > 0) { // 12 is the start of IEND
            uint32_t ChunkSize = ReadBits(BitI, 32, true);
            uint32_t ChunkID   = ReadBits(BitI, 32, true);
            
            if (strcasecmp(ChunkID, "iDAT") == 0) {
                ParseIDAT(BitI, Dec, ChunkSize);
            } else if (strcasecmp(ChunkID, "acTL") == 0) {
                
            } else if (strcasecmp(ChunkID, "fdAT") == 0) {
                ParseFDAT(BitI, Dec, ChunkSize);
            }
        }
    }
    
    void PNGDecodeImage(BitInput *BitI, DecodePNG *Dec, uint16_t *DecodedImage) {
        // Parse all chunks except iDAT, fDAT, and iEND first.
        // When you come across an iDAT or fDAT. you need to store the start address, then return to parsing the meta chunks, then at the end  decode the i/f DATs.
        ParsePNGMetadata(BitI, Dec);
        
        if (Dec->Is3D == true) {
            
        }
        while (BytesRemainingInFile(BitI) > 0) {
            
        }
    }
    
    uint16_t **DecodeAdam7(DecodePNG *Dec, uint16_t **DecodedImage) {
        // Break the image into 8x8 blocks.
        // MARK: if the image is not a multiple of 8, I assume you pad the edge blocks?
        for (uint32_t WidthBlock = 0; WidthBlock < Dec->iHDR->Width; WidthBlock += 8) {
            for (uint32_t HeightBlock = 0; HeightBlock < Dec->iHDR->Height; HeightBlock += 8) {
                for (uint8_t Adam7Level = 0; Adam7Level < 7; Adam7Level++) {
                    
                }
            }
        }
        
        return 0;
    }
    
    // Let's do this library right, by adding attach and delete functions for the various chunks, and let's also have a fancy function that applies color profiles to the pixels.
    // that's kinda a lot of work tho...
    
    void SeperateStereoImage(DecodePNG *Dec, uint16_t **DecodedImage) {
        
    }
    
    void PNGReadMetadata(BitInput *BitI, DecodePNG *Dec) {
        ParsePNGMetadata(BitI, Dec);
    }
    
#ifdef __cplusplus
}
#endif
