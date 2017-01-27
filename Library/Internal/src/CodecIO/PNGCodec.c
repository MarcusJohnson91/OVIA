#include "../../include/libModernPNG.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void CalculateSTERPadding(PNGDecoder *PNG) {
        PNG->LinePadding = 7 - ((PNG->iHDR->Width - 1) % 8);
        PNG->LineWidth   = (PNG->iHDR->Width * 2) + PNG->LinePadding;
    }
    
    void PNGEncodeFilterPaeth(PNGEncoder *PNG, uint8_t *ScanLine, size_t ScanLineSize) {
        // RawData is after decoding the I/f DATs, and after INFLAT'ing and De-LZ77'ing it.
        // Each line is preceded by a filter type byte, so decode it by a line by line basis.
        // Good candidate for multi-threading.
    }
    
    void PNGDecodeFilterPaeth(PNGDecoder *PNG, uint8_t *ScanLine, size_t ScanLineSize) {
        // Filtering is applied to bytes, not pixels
        uint8_t FilterType = ScanLine[0];
        for (size_t Byte = 1; Byte < ScanLineSize; Byte++) {
            /*
             if () {
             
             }
             */
        }
    }
    
    void PNGDecodeFilterSub(PNGDecoder *PNG, uint8_t *ScanLine, size_t NumPixels) {
        // NumPixel means whole pixel not sub pixel.
        uint8_t DecodedLine[NumPixels];
        for (size_t Pixel = 1; Pixel < NumPixels; Pixel++) {
            if (Pixel == 1) {
                DecodedLine[Pixel] = ScanLine[Pixel];
            } else {
                DecodedLine[Pixel] = ScanLine[Pixel] + ScanLine[Pixel - 1];
            }
        }
    }
    
    void PNGEncodeFilterSub(PNGEncoder *PNG, uint8_t *ScanLine, size_t NumPixels) {
        // NumPixel means whole pixel not sub pixel.
        uint8_t EncodedLine[NumPixels];
        for (size_t Pixel = 1; Pixel < NumPixels; Pixel++) {
            if (Pixel == 1) {
                EncodedLine[Pixel] = ScanLine[Pixel];
            } else {
                EncodedLine[Pixel] = ScanLine[Pixel] - ScanLine[Pixel - 1];
            }
        }
    }
    
    void PNGEncodeAdam7(PNGEncoder *PNG, uint8_t *RawFrame) {
        // Break image into 8x8 blocks, then break each chunk into 7 layers.
    }
    
    uint8_t *PNGDecodeFilterUp(PNGDecoder *PNG, uint8_t *CurrentScanLine, uint8_t *PreviousScanLine, size_t ScanLineSize) {
        uint8_t DecodedScanLine[ScanLineSize];
        for (uint8_t Byte = 1; Byte < ScanLineSize; Byte++) {
            DecodedScanLine[Byte] = CurrentScanLine[Byte] + PreviousScanLine[Byte] % 256;
        }
        return DecodedScanLine;
    }
    
    void PNGDecodeFilterAverage(PNGDecoder *PNG, uint8_t *CurrentScanLine, uint8_t *PreviousScanLine, size_t ScanLineSize) {
        uint8_t DecodedScanLine[ScanLineSize];
        for (uint8_t Byte = 1; Byte < ScanLineSize; Byte++) {
            //DecodedScanLine[Byte] = Average(x) + floor((Raw(-bpp) + PreviousScanLine[x]/2)
        }
    }
    
    // When Callocing a struct that contains a variable length array (like the text processing block) postpone initing that struct until it's been detected, AND when you've detected it, simply take the ChunkSize, and subtract the number of bytes for all the fields, and voila! you've got the size of the mufuckin array!!!
    
    void PNGDecodeFilteredLine(PNGDecoder *PNG, uint8_t *ScanLine, size_t ScanLineSize) {
        char Error[BitIOStringSize];
        uint8_t FilterType = ScanLine[0];
        switch (FilterType) {
            case 0:
                // return the scanline
                break;
            case 1:
                // SubFilter
                PNGDecodeFilterSub(PNG, ScanLine, ((ScanLineSize / ChannelsPerColorType[PNG->iHDR->ColorType]) * (PNG->iHDR->BitDepth / 8)));
                break;
            case 2:
                PNGDecodeFilterUp(PNG, NULL, NULL, ScanLineSize);
                break;
                
            case 3:
                
                break;
            default:
                snprintf(Error, BitIOStringSize, "Filter type: %d is invalid\n");
                Log(SYSError, "NewPNG", "PNGDecodeFilteredLine", Error);
                break;
        }
    }
    
#ifdef __cplusplus
}
#endif

