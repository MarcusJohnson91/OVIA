#include "libModernPNG.h"
#include "PNGTypes.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif
    
    struct PNGDecoder {
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
    
    typedef struct PNGDecoder PNGDecoder;
    
    void ParseIHDR(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize);
    
    void ParsePLTE(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize);
    
    void ParseTRNS(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize);
    
    void ParseBKGD(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize);
    
    void ParseCHRM(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize);
    
    void ParseGAMA(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize);
    
    void ParseOFFS(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize);
    
    void ParsePHYS(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize);
    
    void ParseSCAL(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize);
    
    void ParseSBIT(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize);
    
    void ParseSRGB(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize);
    
    void ParseSTER(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize);
    
    void ParseTEXt(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize);
    
    void ParseZTXt(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize);
    
    void ParseITXt(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize);
    
    void ParseTIME(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize);
    
    void ParseACTL(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize);
    
    void ParseFCTL(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize);
    
    void ParseIDAT(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize);
    
    void ParseHIST(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize);
    
    void ParseICCP(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize);
    
    void ParsePCAL(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize);
    
    void ParseSPLT(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize);
    
    void ParseFDAT(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize);
    
    uint8_t ParsePNGMetadata(BitInput *BitI, PNGDecoder *PNG);
    
    PNGDecoder *InitPNGDecoder(void);
    
#ifdef __cplusplus
}
#endif
