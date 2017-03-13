#include "libModernPNG.h"
#include "PNGTypes.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif
    
    struct PNGDecoder {
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
    
    DecodePNG *InitDecodePNG(void) {
        DecodePNG *Dec  = calloc(sizeof(DecodePNG), 1);
        Dec->iHDR       = calloc(sizeof(iHDR), 1);
        Dec->acTL       = calloc(sizeof(acTL), 1);
        Dec->fdAT       = calloc(sizeof(fdAT), 1);
        Dec->tRNS       = calloc(sizeof(tRNS), 1);
        Dec->cHRM       = calloc(sizeof(cHRM), 1);
        Dec->sBIT       = calloc(sizeof(sBIT), 1);
        Dec->fcTL       = calloc(sizeof(fcTL), 1);
        Dec->Text       = calloc(sizeof(Text), 1);
        Dec->gAMA       = calloc(sizeof(gAMA), 1);
        Dec->oFFs       = calloc(sizeof(oFFs), 1);
        Dec->iCCP       = calloc(sizeof(iCCP), 1);
        Dec->sRGB       = calloc(sizeof(sRGB), 1);
        Dec->sTER       = calloc(sizeof(sTER), 1);
        Dec->PLTE       = calloc(sizeof(PLTE), 1);
        Dec->bkGD       = calloc(sizeof(bkGD), 1);
        Dec->pCAL       = calloc(sizeof(pCAL), 1);
        Dec->hIST       = calloc(sizeof(hIST), 1);
        return Dec;
    }
    
    typedef struct PNGDecoder DecodePNG;
    
    void CalculateSTERPadding(DecodePNG *Dec);
    
    void ParseIHDR(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void ParsePLTE(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void ParseTRNS(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void ParseBKGD(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void ParseCHRM(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void ParseGAMA(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void ParseOFFS(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void ParsePHYS(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void ParseSCAL(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void ParseSBIT(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void ParseSRGB(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void ParseSTER(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void ParseTEXt(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void ParseZTXt(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void ParseITXt(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void ParseTIME(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void ParseACTL(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void ParseFCTL(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void ParseIDAT(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void ParseHIST(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void ParseICCP(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void ParsePCAL(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void ParseSPLT(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void ParseFDAT(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    uint8_t ParsePNGMetadata(BitInput *BitI, DecodePNG *Dec);
    
    DecodePNG *InitPNGDecoder(void);
    
    void PNGDecodeSubFilter(DecodePNG *Dec, uint8_t **DeEntropyedData, uint8_t **DeFilteredData, size_t Line);
    
    void PNGDecodeNonFilter(DecodePNG *Dec, uint8_t *DeEntropyedData, uint8_t *DeFilteredData, size_t Line);
    
    void PNGDecodeUpFilter(DecodePNG *Dec, uint8_t **DeEntropyedData, uint8_t **DeFilteredData, size_t Line);
    
    void PNGDecodeAverageFilter(DecodePNG *Dec, uint8_t **DeEntropyedData, uint8_t **DeFilteredData, size_t Line);
    
    uint8_t PaethPredictor(int64_t Left, int64_t Above, int64_t UpperLeft);
    
    void PNGDecodePaethFilter(DecodePNG *Dec, uint8_t **DeEntropyedData, uint8_t **DeFilteredData, size_t Line);
    
    void PNGDecodeFilter(DecodePNG *Dec, uint8_t ***InflatedBuffer);
    
#ifdef __cplusplus
}
#endif
