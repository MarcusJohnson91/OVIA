#include "../include/libModernPNG.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    static const uint8_t ChannelsPerColorType[8] = {
        1, 0, 3, 3, 4, 4
    };
    
    static const char Number2Month[12][3] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
    };
    
    void CalculateSTERPadding(PNGDecoder *PNG) {
        PNG->LinePadding = 7 - ((PNG->iHDR->Width - 1) % 8);
        PNG->LineWidth   = (PNG->iHDR->Width * 2) + PNG->LinePadding;
    }
    
    /* PNG Decode Filters, Filters operate on bytes, not pixels. */
    void PNGDecodeNonFilter(PNGDecoder *PNG, uint8_t *DeEntropyedData[], uint8_t *DeFilteredData[], size_t Line) {
        for (size_t Byte = 1; Byte < Bits2Bytes(PNG->iHDR->BitDepth); Byte++) {
            DeFilteredData[Byte - 1] = DeEntropyedData[Byte]; // Remove filter indicating byte
        }
    }
    
    void PNGDecodeSubFilter(PNGDecoder *PNG, uint8_t *DeEntropyedData[], uint8_t *DeFilteredData[], size_t Line) {
        for (size_t Byte = 1; Byte < Bits2Bytes(PNG->iHDR->BitDepth); Byte++) {
            DeFilteredData[Line][Byte - 1] = (DeEntropyedData[Line][Byte] + DeEntropyedData[Line][Byte+1]) & 0xFF;
        }
    }
    
    void PNGDecodeUpFilter(PNGDecoder *PNG, uint8_t *DeEntropyedData[], uint8_t *DeFilteredData[], size_t Line) {
        for (size_t Byte = 1; Byte < Bits2Bytes(PNG->iHDR->BitDepth); Byte++) {
            DeFilteredData[Line][Byte - 1] = DeEntropyedData[Line][Byte] + DeEntropyedData[Line - 1][Byte] & 0xFF;
        }
    }
    
    void PNGDecodeAverageFilter(PNGDecoder *PNG, uint8_t *DeEntropyedData[], uint8_t *DeFilteredData[], size_t Line) {
        uint8_t PixelSize = Bits2Bytes(PNG->iHDR->BitDepth);
        for (size_t Byte = 1; Byte < Bits2Bytes(PNG->iHDR->BitDepth); Byte++) {
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
        /*
         function PaethPredictor (a, b, c)
         begin
         ; a = left, b = above, c = upper left
         p := a + b - c        ; initial estimate
         pa := llabs(p - a)      ; distances to a, b, c
         pb := llabs(p - b)
         pc := llabs(p - c)
         ; return nearest of a,b,c,
         ; breaking ties in order a,b,c.
         if pa <= pb AND pa <= pc then return a
         else if pb <= pc then return b
         else return c
         end
         */
    }
    
    void PNGDecodePaethFilter(PNGDecoder *PNG, uint8_t *DeEntropyedData[], uint8_t *DeFilteredData[], size_t Line) {
        // Filtering is applied to bytes, not pixels
        uint8_t PixelSize = Bits2Bytes(PNG->iHDR->BitDepth);
        for (size_t Byte = 1; Byte < Bits2Bytes(PNG->iHDR->BitDepth); Byte++) {
            if (Line == 0) { // Assume top and top left = 0
                DeFilteredData[Line][Byte] = PaethPredictor(DeEntropyedData[Line][Byte], 0, 0);
            } else {
                DeFilteredData[Line][Byte] = PaethPredictor(DeEntropyedData[Line][Byte], DeEntropyedData[Line][Byte - PixelSize], DeEntropyedData[Line - 1][Byte - PixelSize]);
            }
        }
    }
    
    /* PNG Encode Filters */
    void PNGEncodeFilterPaeth(PNGEncoder *PNG, uint8_t *Line, size_t LineSize) {
        // RawData is after decoding the I/f DATs, and after INFLAT'ing and De-LZ77'ing it.
        // Each line is preceded by a filter type byte, so decode it by a line by line basis.
        // Good candidate for multi-threading.
    }
    
    void PNGEncodeFilterSub(PNGEncoder *PNG, uint8_t *Line, size_t NumPixels) {
        // NumPixel means whole pixel not sub pixel.
        uint8_t EncodedLine[NumPixels];
        for (size_t Pixel = 1; Pixel < NumPixels; Pixel++) {
            if (Pixel == 1) {
                EncodedLine[Pixel] = Line[Pixel];
            } else {
                EncodedLine[Pixel] = Line[Pixel] - Line[Pixel - 1];
            }
        }
    }
    
    void PNGEncodeAdam7(PNGEncoder *PNG, uint8_t *RawFrame) {
        // Break image into 8x8 blocks, then break each chunk into 7 layers.
    }
    
    
    
    // When Callocing a struct that contains a variable length array (like the text processing block) postpone initing that struct until it's been detected, AND when you've detected it, simply take the ChunkSize, and subtract the number of bytes for all the fields, and voila! you've got the size of the mufuckin array!!!
    
    void PNGDecodeFilter(PNGDecoder *PNG, uint8_t ***InflatedBuffer) {
        char Error[BitIOStringSize];
        
        uint8_t DeFilteredData[PNG->iHDR->Height][PNG->iHDR->Width - 1];
        
        for (size_t Line = 0; Line < PNG->iHDR->Height; Line++) {
            uint8_t FilterType = *InflatedBuffer[Line][0];
            switch (FilterType) {
                case 0:
                    // copy the Line except byte 0 (the filter indication byte) to the output buffer.
                    break;
                case 1:
                    // SubFilter
                    break;
                case 2:
                    // UpFilter
                    break;
                case 3:
                    // AverageFilter
                    break;
                case 4:
                    // PaethFilter
                    break;
                default:
                    snprintf(Error, BitIOStringSize, "Filter type: %d is invalid\n", FilterType);
                    Log(LOG_ERR, "NewPNG", "PNGDecodeFilteredLine", Error);
                    break;
            }
        }
    }
    
    void DecodePNGImage(BitInput *BitI, PNGDecoder *PNG, uint8_t *DecodedImage) {
        
    }
    
#ifdef __cplusplus
}
#endif
