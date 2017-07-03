#include "../include/libModernPNG.h"
#include "../include/ModernPNGTypes.h"
#include "../include/ModernPNGShared.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    uint32_t CalculateSTERPadding(const uint32_t Width) {
        return 7 - ((Width - 1) % 8);
    }
    
    DecodePNG *InitDecodePNG(void) {
        DecodePNG *Dec  = calloc(1, sizeof(DecodePNG));
        Dec->acTL       = calloc(1, sizeof(acTL));
        Dec->bkGD       = calloc(1, sizeof(bkGD));
        Dec->cHRM       = calloc(1, sizeof(cHRM));
        Dec->fcTL       = calloc(1, sizeof(fcTL));
        Dec->fdAT       = calloc(1, sizeof(fdAT));
        Dec->gAMA       = calloc(1, sizeof(gAMA));
        Dec->hIST       = calloc(1, sizeof(hIST));
        Dec->iCCP       = calloc(1, sizeof(iCCP));
        Dec->iHDR       = calloc(1, sizeof(iHDR));
        Dec->oFFs       = calloc(1, sizeof(oFFs));
        Dec->pCAL       = calloc(1, sizeof(pCAL));
        Dec->PLTE       = calloc(1, sizeof(PLTE));
        Dec->sBIT       = calloc(1, sizeof(sBIT));
        Dec->sRGB       = calloc(1, sizeof(sRGB));
        Dec->sTER       = calloc(1, sizeof(sTER));
        Dec->Text       = calloc(1, sizeof(Text));
        Dec->tIMe       = calloc(1, sizeof(tIMe));
        Dec->tRNS       = calloc(1, sizeof(tRNS));
        return Dec;
    }
    
    EncodePNG *InitEncodePNG(void) {
        EncodePNG *Enc  = calloc(1, sizeof(EncodePNG));
        Enc->acTL       = calloc(1, sizeof(acTL));
        Enc->bkGD       = calloc(1, sizeof(bkGD));
        Enc->cHRM       = calloc(1, sizeof(cHRM));
        Enc->fcTL       = calloc(1, sizeof(fcTL));
        Enc->fdAT       = calloc(1, sizeof(fdAT));
        Enc->gAMA       = calloc(1, sizeof(gAMA));
        Enc->hIST       = calloc(1, sizeof(hIST));
        Enc->iCCP       = calloc(1, sizeof(iCCP));
        Enc->iHDR       = calloc(1, sizeof(iHDR));
        Enc->oFFs       = calloc(1, sizeof(oFFs));
        Enc->pCAL       = calloc(1, sizeof(pCAL));
        Enc->PLTE       = calloc(1, sizeof(PLTE));
        Enc->sBIT       = calloc(1, sizeof(sBIT));
        Enc->sRGB       = calloc(1, sizeof(sRGB));
        Enc->sTER       = calloc(1, sizeof(sTER));
        Enc->Text       = calloc(1, sizeof(Text));
        Enc->tIMe       = calloc(1, sizeof(tIMe));
        Enc->tRNS       = calloc(1, sizeof(tRNS));
        return Enc;
    }
    
#ifdef __cplusplus
}
#endif
