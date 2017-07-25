#include "../../include/libModernPNG.h"
#include "../../include/ModernPNGTypes.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void PNGEncodeFilterNone(EncodePNG *Enc, uint8_t *DeEntropyedData, uint8_t *DeFilteredData, size_t Line) {
        
    }
    
    void PNGEncodeFilterPaeth(EncodePNG *Enc, uint8_t *Line, size_t LineSize) {
        // RawData is after decoding the I/f DATs, and after INFLAT'ing and De-LZ77'ing it.
        // Each line is preceded by a filter type byte, so decode it by a line by line basis.
        // Good candidate for multi-threading.
    }
    
    void PNGEncodeFilterSub(EncodePNG *Enc, uint8_t *Line, size_t NumPixels) {
        // NumPixel means whole pixel not sub pixel.
        uint8_t *EncodedLine = (uint8_t*)calloc(1, Enc->iHDR->Width);
        for (size_t Pixel = 1; Pixel < NumPixels; Pixel++) {
            if (Pixel == 1) {
                EncodedLine[Pixel] = Line[Pixel];
            } else {
                EncodedLine[Pixel] = Line[Pixel] - Line[Pixel - 1];
            }
        }
        free(EncodedLine);
    }
    
#ifdef __cplusplus
}
#endif
