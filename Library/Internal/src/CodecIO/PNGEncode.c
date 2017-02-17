#include "../include/EncodePNG.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    struct PNGEncoder {
        uint32_t      iHDRSize;
        uint32_t      CurrentFrame;
        uint32_t      LineWidth;
        uint32_t      LinePadding;
        bool          IsVideo:1;
        bool          Is3D:1;
        bool          acTLExists:1;
        bool          fcTLExists:1;
        bool          cHRMExists:1;
        bool          gAMAExists:1;
        bool          iCCPExists:1;
        bool          sRGBExists:1;
        struct iHDR   *iHDR;
        struct acTL   *acTL;
        struct fcTL   *fcTL;
        struct fdAT   *fdAT;
        struct tRNS   *tRNS;
        struct cHRM   *cHRM;
        struct sBIT   *sBIT;
        struct Text   *Text;
        struct gAMA   *gAMA;
        struct iCCP   *iCCP;
        struct sRGB   *sRGB;
        struct sTER   *sTER;
        struct PLTE   *PLTE;
        struct bkGD   *bkGD;
    };
    
    PNGEncoder *InitPNGEncoder(void) {
        PNGEncoder *Enc = calloc(sizeof(PNGEncoder), 1);
        Enc->iHDR       = calloc(sizeof(iHDR), 1);
        Enc->acTL       = calloc(sizeof(acTL), 1);
        Enc->fdAT       = calloc(sizeof(fdAT), 1);
        Enc->tRNS       = calloc(sizeof(tRNS), 1);
        Enc->cHRM       = calloc(sizeof(cHRM), 1);
        Enc->sBIT       = calloc(sizeof(sBIT), 1);
        Enc->fcTL       = calloc(sizeof(fcTL), 1);
        Enc->Text       = calloc(sizeof(Text), 1);
        Enc->gAMA       = calloc(sizeof(gAMA), 1);
        Enc->oFFs       = calloc(sizeof(oFFs), 1);
        Enc->iCCP       = calloc(sizeof(iCCP), 1);
        Enc->sRGB       = calloc(sizeof(sRGB), 1);
        Enc->sTER       = calloc(sizeof(sTER), 1);
        Enc->PLTE       = calloc(sizeof(PLTE), 1);
        Enc->bkGD       = calloc(sizeof(bkGD), 1);
        return Enc;
    }
    
#ifdef __cplusplus
}
#endif
