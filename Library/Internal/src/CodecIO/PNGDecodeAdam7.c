#include "../../include/libModernPNG.h"
#include "../../include/PNGTypes.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    uint16_t **DecodeAdam7(DecodePNG *Dec, uint16_t **DecImage) {
        // Break the image into 8x8 blocks.
        // MARK: if the image is not a multiple of 8, I assume you pad the edge blocks?
        for (uint32_t WidthBlock = 0; WidthBlock < Dec->iHDR->Width; WidthBlock += 8) {
            
        }
        
        return 0;
    }
    
#ifdef __cplusplus
}
#endif
