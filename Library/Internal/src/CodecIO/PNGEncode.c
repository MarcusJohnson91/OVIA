#include "../include/libModernPNG.h"
#include "../include/EncodePNG.h"
#include "../include/PNGTypes.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void PNGEncodeFilterPaeth(EncodePNG *Enc, uint8_t *Line, size_t LineSize) {
        // RawData is after decoding the I/f DATs, and after INFLAT'ing and De-LZ77'ing it.
        // Each line is preceded by a filter type byte, so decode it by a line by line basis.
        // Good candidate for multi-threading.
    }
    
    void PNGEncodeFilterSub(EncodePNG *Enc, uint8_t *Line, size_t NumPixels) {
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
    
    void PNGEncodeAdam7(EncodePNG *Enc, uint8_t *RawFrame) {
        // Break image into 8x8 blocks, then break each chunk into 7 layers.
    }
    
#ifdef __cplusplus
}
#endif
