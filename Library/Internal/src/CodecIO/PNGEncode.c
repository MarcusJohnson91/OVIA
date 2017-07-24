#include <math.h>

#include "../../include/libModernPNG.h"
#include "../../include/Encode/EncodePNG.h"
#include "../../include/ModernPNGTypes.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void PNGEncodeFilterNone(DecodePNG *Dec, uint8_t *DeEntropyedData, uint8_t *DeFilteredData, size_t Line) {
        
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
    }
    
    void PNGEncodeAdam7(EncodePNG *Enc, uint8_t ****RawFrame) {
        uint8_t WidthPadding = 0, HeightPadding = 0, WidthStart = 0, HeightStart = 0;
        // Break image into 8x8 blocks, then break each chunk into 7 layers.
        if (Enc->iHDR->Width % 8 != 0) {
            WidthPadding = 8 - (Enc->iHDR->Width % 8);
        }
        if (Enc->iHDR->Height % 8 != 0) {
            HeightPadding = 8 - (Enc->iHDR->Height % 8);
        }
        // Pad the RawFrame by WidthPadding and HeightPadding
        // Then Split the image into 8x8 chunks, or maybe I should just have a few loops?
        
        // Create an array with size (Enc->iHDR->Width + WidthPadding) * (Enc->iHDR->Height + HeightPadding)
        uint16_t ****Image = calloc(((Enc->iHDR->Width + WidthPadding) * (Enc->iHDR->Height + HeightPadding)) * (Enc->Is3D * 2), 1);
        // Now copy the image into the new array, in the middle so the amount of padding is even on both sized
        // If the padding amount is odd, start at (Padding / 2)
        
        for (uint8_t StereoChannel = 0; StereoChannel < Enc->Is3D; StereoChannel++) {
            for (uint8_t Color = 0; Color < PNGChannelsPerColorType[Enc->iHDR->ColorType]; Color++) {
                for (uint64_t Height = HeightPadding / 2; Height < Enc->iHDR->Height; Height++) {
                    for (uint64_t Width = WidthPadding / 2; Width < Enc->iHDR->Width; Width++) {
                        Image[StereoChannel][Color][Height][Width] = RawFrame[StereoChannel][Color][Height][Width];
                    }
                }
            }
        }
        
        // Now I need to break up the image into 8x8 blocks
    }
    
    void OptimizePNG(EncodePNG *Enc, uint8_t ****Image) {
        // call PNGEncodeAdam7, then take the resulting image and try all the filters on an 8x8 block.
        // Actually, no. we should have a PNGFilterAdam7 function...
        // } else {
        // Just take the lines and try all the filters on each one, comparing them by comparing the difference between the bytes.
    }
    
    void PNGEncodeImage(EncodePNG *Enc, BitBuffer *BitB) {
        
    }
    
#ifdef __cplusplus
}
#endif
