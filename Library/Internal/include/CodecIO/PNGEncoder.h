#include "libModernPNG.h"
#include "PNGTypes.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif
    
    struct PNGEncoder {
        uint32_t      CurrentFrame;
        uint32_t      LineWidth;
        uint32_t      LinePadding;
        bool          IsVideo:1;
        bool          Is3D:1;
        bool          acTLExists:1;
        bool          bkGDExists:1;
        bool          cHRMExists:1;
        bool          fcTLExists:1;
        bool          gAMAExists:1;
        bool          hISTExists:1;
        bool          iCCPExists:1;
        bool          oFFsExists:1;
        bool          pCALExists:1;
        bool          pHYsExists:1;
        bool          PLTEExists:1;
        bool          sBITExists:1;
        bool          sCALExists:1;
        bool          sPLTExists:1;
        bool          sRGBExists:1;
        bool          sTERExists:1;
        bool          TextExists:1;
        bool          tIMEExists:1;
        bool          tRNSExists:1;
        struct acTL   *acTL;
        struct bkGD   *bkGD;
        struct cHRM   *cHRM;
        struct fcTL   *fcTL;
        struct fdAT   *fdAT;
        struct gAMA   *gAMA;
        struct hIST   *hIST;
        struct iCCP   *iCCP;
        struct iHDR   *iHDR;
        struct oFFs   *oFFs;
        struct pCAL   *pCAL;
        struct pHYs   *pHYs;
        struct PLTE   *PLTE;
        struct sBIT   *sBIT;
        struct sCAL   *sCAL;
        struct sRGB   *sRGB;
        struct sTER   *sTER;
        struct Text   *Text;
        struct tRNS   *tRNS;
    };
    
    EncodePNG *InitPNGEncoder(void) {
        EncodePNG *Enc  = calloc(sizeof(EncodePNG), 1);
        Enc->acTL       = calloc(sizeof(acTL), 1);
        Enc->bkGD       = calloc(sizeof(bkGD), 1);
        Enc->cHRM       = calloc(sizeof(cHRM), 1);
        Enc->fcTL       = calloc(sizeof(fcTL), 1);
        Enc->fdAT       = calloc(sizeof(fdAT), 1);
        Enc->gAMA       = calloc(sizeof(gAMA), 1);
        Enc->iCCP       = calloc(sizeof(iCCP), 1);
        Enc->iHDR       = calloc(sizeof(iHDR), 1);
        Enc->oFFs       = calloc(sizeof(oFFs), 1);
        Enc->PLTE       = calloc(sizeof(PLTE), 1);
        Enc->sBIT       = calloc(sizeof(sBIT), 1);
        Enc->sRGB       = calloc(sizeof(sRGB), 1);
        Enc->sTER       = calloc(sizeof(sTER), 1);
        Enc->Text       = calloc(sizeof(Text), 1);
        Enc->tRNS       = calloc(sizeof(tRNS), 1);
        return Enc;
    }
    
#ifdef __cplusplus
}
#endif
