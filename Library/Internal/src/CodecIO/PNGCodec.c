#include "../include/libModernPNG.h"
#include "../include/Private/libModernPNG_Types.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    uint32_t GetPNGWidth(DecodePNG *Dec) {
        return Dec->iHDR->Width;
    }
    
    uint32_t GetPNGHeight(DecodePNG *Dec) {
        return Dec->iHDR->Height;
    }
    
    uint8_t GetPNGBitDepth(DecodePNG *Dec) {
        return Dec->iHDR->BitDepth;
    }
    
    uint8_t GetPNGColorType(DecodePNG *Dec) {
        return Dec->iHDR->ColorType;
    }
    
    bool GetPNGInterlaceStatus(DecodePNG *Dec) {
        return Dec->iHDR->IsInterlaced;
    }
    
    bool IsPNGStereoscopic(DecodePNG *Dec) {
        return Dec->Is3D;
    }
    
    uint32_t GetPNGWhitepointX(DecodePNG *Dec) {
        return Dec->cHRM->WhitePointX;
    }
    
    uint32_t GetPNGWhitepointY(DecodePNG *Dec) {
        return Dec->cHRM->WhitePointY;
    }
    
    uint32_t GetPNGGamma(DecodePNG *Dec) {
        return Dec->gAMA->Gamma;
    }
    
    const char *GetPNGColorProfileName(DecodePNG *Dec) {
        return Dec->iCCP->ProfileName;
    }
    
    uint8_t *GetColorProfile(DecodePNG *Dec) {
        //return DecompressDEFLATE(Dec->iCCP->CompressedICCPProfile);
        return NULL;
    }
    
#ifdef __cplusplus
}
#endif
