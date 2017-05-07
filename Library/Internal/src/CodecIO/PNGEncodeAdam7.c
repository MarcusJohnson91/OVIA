#include "../../include/libModernPNG.h"
#include "../../include/PNGTypes.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void       PadImageForAdam7(EncodePNG *Enc, uint16_t ***Image) {
        uint8_t WidthRows     = 8 - (Enc->iHDR->Width % 8); // if it's odd add the padding to the bottom, or right
        uint8_t HeightColumns = 8 - (Enc->iHDR->Height % 8);
        
        // Pad the image as you extract your 8x8 blocks.
        
    }
    
    void Extract8x8BlockForAdam7(EncodePNG *Enc, uint16_t ***Image) {
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
    
    uint16_t ***EncodeAdam7(EncodePNG *Enc, uint16_t **EncImage) {
        // Well, the first thing we need to do is pad the image if it's not a multiple of 8 wide or high.
        
        return 0;
    }

#ifdef __cplusplus
}
#endif
