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
    
#ifdef __cplusplus
}
#endif
