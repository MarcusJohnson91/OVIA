#include <stdlib.h>

#include "../include/libModernPNG.h"
#include "../include/Private/libModernPNG_Types.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    uint32_t CalculateSTERPadding(const uint32_t Width) {
        return 7 - ((Width - 1) % 8);
    }
    
    void DecodePNG_Text_Init(struct PNGDecoder *Dec, uint8_t KeywordSize, uint32_t CommentSize) {
        if (Dec != NULL && KeywordSize > 0 && CommentSize > 0) {
            if (Dec->Text != NULL) {
                Dec->Text  = realloc(Dec->Text, sizeof(Text) * Dec->NumTextChunks);
            } else {
                Dec->Text  = calloc(Dec->NumTextChunks, sizeof(Text));
            }
            Dec->Text[Dec->NumTextChunks - 1].Keyword = calloc(1, KeywordSize);
            Dec->Text[Dec->NumTextChunks - 1].Comment = calloc(1, CommentSize);
        } else if (Dec == NULL) {
            BitIOLog(BitIOLog_ERROR, libModernPNGLibraryName, __func__, "DecodePNG Pointer is NULL");
        }
    }
    
    void EncodePNG_Text_Init(struct PNGEncoder *Enc, uint8_t KeywordSize, uint32_t CommentSize) {
        if (Enc != NULL && KeywordSize > 0 && CommentSize > 0) {
            if (Enc->Text != NULL) {
                Enc->Text  = realloc(Enc->Text, sizeof(Text) * Enc->NumTextChunks);
            } else {
                Enc->Text  = calloc(Enc->NumTextChunks, sizeof(Text));
            }
            Enc->Text[Enc->NumTextChunks - 1].Keyword = calloc(1, KeywordSize);
            Enc->Text[Enc->NumTextChunks - 1].Comment = calloc(1, CommentSize);
        } else if (Enc == NULL) {
            BitIOLog(BitIOLog_ERROR, libModernPNGLibraryName, __func__, "EncodePNG Pointer is NULL");
        }
    }
    
    DecodePNG *DecodePNG_Init(void) {
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
    
    /*
     Actually, fuck that, I'm making 1 function to set/get the info from a Text chunk.
     */
    
    void PNGSetTextChunk(EncodePNG *Enc, UTF8 *KeywordString, UTF8 *CommentString) {
        if (Enc != NULL && KeywordString != NULL && CommentString != NULL) {
            // Check to see if the current Comment field has been set, if it has increment Enc->NumTextChunks, and realloc, otherwise just set it.
            if (Enc->Text[Enc->NumTextChunks - 1].Comment != NULL) {
                Enc->NumTextChunks += 1;
            }
            Enc->Text[Enc->NumTextChunks - 1].Keyword = KeywordString;
            Enc->Text[Enc->NumTextChunks - 1].Comment = CommentString; // TODO: When writing these chunks out, make sure to scan the Comments for Unicode, if it is, you have to use iTXt
        } else if (Enc == NULL) {
            BitIOLog(BitIOLog_ERROR, libModernPNGLibraryName, __func__, "EncodePNG Pointer is NULL");
        } else if (KeywordString == NULL) {
            BitIOLog(BitIOLog_ERROR, libModernPNGLibraryName, __func__, "KeywordString Pointer is NULL");
        } else if (CommentString == NULL) {
            BitIOLog(BitIOLog_ERROR, libModernPNGLibraryName, __func__, "CommentString Pointer is NULL");
        }
    }
    
    void PNGGetTextChunk(DecodePNG *Dec, uint32_t Instance, UTF8 *Keyword, UTF8 *Comment) {
        if (Dec != NULL && Instance <= Dec->NumTextChunks - 1) {
            Keyword = Dec->Text[Instance].Keyword;
            Comment = Dec->Text[Instance].Comment;
        } else if (Dec == NULL) {
            BitIOLog(BitIOLog_ERROR, libModernPNGLibraryName, __func__, "DecodePNG Pointer is NULL");
        } else if (Instance > Dec->NumTextChunks - 1) {
            BitIOLog(BitIOLog_ERROR, libModernPNGLibraryName, __func__, "Instance %d is greater than there are Text chunks %d", Instance, Dec->NumTextChunks);
        }
    }
    
    uint32_t PNGGetNumTextChunks(DecodePNG *Dec) {
        uint32_t NumTextChunks = 0UL;
        if (Dec != NULL) {
            NumTextChunks      = Dec->NumTextChunks;
        } else {
            BitIOLog(BitIOLog_ERROR, libModernPNGLibraryName, __func__, "DecodePNG Pointer is NULL");
        }
        return NumTextChunks;
    }
    
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
        return Dec->PNGIs3D;
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
    
    void DecodePNG_Deinit(DecodePNG *Dec) {
        if (Dec->acTLExists) {
            free(Dec->acTL);
        }
        if (Dec->bkGDExists) {
            free(Dec->bkGD->BackgroundPaletteEntry);
            free(Dec->bkGD);
        }
        if (Dec->cHRMExists) {
            free(Dec->cHRM);
        }
        if (Dec->fcTLExists) {
            free(Dec->fcTL);
            free(Dec->fdAT);
        }
        if (Dec->gAMAExists) {
            free(Dec->gAMA);
        }
        if (Dec->hISTExists) {
            free(Dec->hIST);
        }
        if (Dec->iCCPExists) {
            free(Dec->iCCP->CompressedICCPProfile);
            free(Dec->iCCP->ProfileName);
            free(Dec->iCCP);
        }
        if (Dec->oFFsExists) {
            free(Dec->oFFs);
        }
        if (Dec->pCALExists) {
            free(Dec->pCAL->CalibrationName);
            free(Dec->pCAL->UnitName);
            free(Dec->pCAL);
        }
        if (Dec->PLTEExists) {
            free(Dec->PLTE->Palette);
            free(Dec->PLTE);
        }
        if (Dec->sBITExists) {
            free(Dec->sBIT);
        }
        if (Dec->sRGBExists) {
            free(Dec->sRGB);
        }
        if (Dec->sTERExists) {
            free(Dec->sTER);
        }
        if (Dec->TextExists) {
            for (uint32_t TextChunk = 0UL; TextChunk < Dec->NumTextChunks; TextChunk++) {
                free(Dec->Text[TextChunk].Keyword);
                free(Dec->Text[TextChunk].Comment);
            }
            free(Dec->Text);
        }
        if (Dec->tIMEExists) {
            free(Dec->tIMe);
        }
        if (Dec->tRNSExists) {
            free(Dec->tRNS->Palette);
            free(Dec->tRNS);
        }
        free(Dec->iHDR);
        free(Dec);
    }
    
#ifdef __cplusplus
}
#endif
