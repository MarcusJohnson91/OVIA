#include "../../include/libModernPNG.h"
#include "../../include/ModernPNGTypes.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void       PadImageForAdam7(EncodePNG *Enc, uint16_t ***Image) {
        uint8_t WidthRows     = 8 - (Enc->iHDR->Width % 8); // if it's odd add the padding to the bottom, or right
        uint8_t HeightColumns = 8 - (Enc->iHDR->Height % 8);
        
        // Pad the image as you extract your 8x8 blocks.
        
    }
    
    void       Extract8x8BlockForAdam7(EncodePNG *Enc, uint16_t ***Image) {
        uint8_t PaddingRows    = 8 - (Enc->iHDR->Width % 8);  // Horizontal
        uint8_t PaddingColumns = 8 - (Enc->iHDR->Height % 8); // Vertical
        
        for (uint64_t Width = 0; Width < Enc->iHDR->Width; Width++) {
            for (uint64_t Height = 0; Height < Enc->iHDR->Height; Height++) {
                if ((Enc->iHDR->Width % 8 != 0) && (Width % Enc->iHDR->Width == 0)) { // if we need to pad the width, and we're at the start of a scanline
                    
                }
                if ((Enc->iHDR->Height % 8 != 0) && (Height % Enc->iHDR->Height == 0)) { // if we need to pad the height, and we're at the start of a scanline
                    
                }
            }
        }
        
    }
    
    uint16_t   ***EncodeAdam7(EncodePNG *Enc, uint16_t **EncImage) {
        // Well, the first thing we need to do is pad the image if it's not a multiple of 8 wide or high.
        
        return 0;
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

#ifdef __cplusplus
}
#endif
