#include "../../include/libModernPNG.h"
#include "../../include/Private/libModernPNG_Types.h"

#include "../../include/Private/Encode/libModernPNG_Encode.h"


#ifdef __cplusplus
extern "C" {
#endif
    
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
    
    void DeinitEncodePNG(EncodePNG *Enc) {
        if (Enc->acTLExists) {
            free(Enc->acTL);
        }
        if (Enc->bkGDExists) {
            free(Enc->bkGD->BackgroundPaletteEntry);
            free(Enc->bkGD);
        }
        if (Enc->cHRMExists) {
            free(Enc->cHRM);
        }
        if (Enc->fcTLExists) {
            free(Enc->fcTL);
            free(Enc->fdAT);
        }
        if (Enc->gAMAExists) {
            free(Enc->gAMA);
        }
        if (Enc->hISTExists) {
            free(Enc->hIST);
        }
        if (Enc->iCCPExists) {
            free(Enc->iCCP->CompressedICCPProfile);
            free(Enc->iCCP->ProfileName);
            free(Enc->iCCP);
        }
        if (Enc->oFFsExists) {
            free(Enc->oFFs);
        }
        if (Enc->pCALExists) {
            free(Enc->pCAL->CalibrationName);
            free(Enc->pCAL->UnitName);
            free(Enc->pCAL);
        }
        if (Enc->PLTEExists) {
            free(Enc->PLTE->Palette);
            free(Enc->PLTE);
        }
        if (Enc->sBITExists) {
            free(Enc->sBIT);
        }
        if (Enc->sRGBExists) {
            free(Enc->sRGB);
        }
        if (Enc->sTERExists) {
            free(Enc->sTER);
        }
        if (Enc->TextExists) {
            free(Enc->Text->Keyword);
            free(Enc->Text->TextString);
            free(Enc->Text);
        }
        if (Enc->tIMEExists) {
            free(Enc->tIMe);
        }
        if (Enc->tRNSExists) {
            free(Enc->tRNS->Palette);
            free(Enc->tRNS);
        }
        free(Enc->iHDR);
        free(Enc);
    }
    
    BitBuffer *EncodePNGImage(EncodePNG *Enc, void ****RawImage2Encode, bool InterlacePNG, bool OptimizePNG) {
        // RawImage2Encode is a void pointer, the contents of Enc->iHDR->BitDepth will define the data type, uint8_t or uint16_t
        
        // This is THE main function for encoding a buffer into a PNG file.
        return NULL;
    }
    
#ifdef __cplusplus
}
#endif
