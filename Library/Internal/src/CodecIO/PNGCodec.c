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
            Log(Log_ERROR, __func__, U8("DecodePNG Pointer is NULL"));
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
            Log(Log_ERROR, __func__, U8("EncodePNG Pointer is NULL"));
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
    
    /* PNG Get functions */
    void PNGGetTextChunk(DecodePNG *Dec, uint32_t Instance, UTF8 *Keyword, UTF8 *Comment) {
        if (Dec != NULL && Instance <= Dec->NumTextChunks - 1) {
            Keyword = Dec->Text[Instance].Keyword;
            Comment = Dec->Text[Instance].Comment;
        } else if (Dec == NULL) {
            Log(Log_ERROR, __func__, U8("DecodePNG Pointer is NULL"));
        } else if (Instance > Dec->NumTextChunks - 1) {
            Log(Log_ERROR, __func__, "Instance %d is greater than there are Text chunks %d", Instance, Dec->NumTextChunks);
        }
    }
    
    uint32_t PNGGetNumTextChunks(DecodePNG *Dec) {
        uint32_t NumTextChunks = 0UL;
        if (Dec != NULL) {
            NumTextChunks      = Dec->NumTextChunks;
        } else {
            Log(Log_ERROR, __func__, U8("DecodePNG Pointer is NULL"));
        }
        return NumTextChunks;
    }
    
    uint32_t PNGGetWidth(DecodePNG *Dec) {
        return Dec->iHDR->Width;
    }
    
    uint32_t PNGGetHeight(DecodePNG *Dec) {
        return Dec->iHDR->Height;
    }
    
    uint8_t PNGGetBitDepth(DecodePNG *Dec) {
        return Dec->iHDR->BitDepth;
    }
    
    uint8_t PNGGetColorType(DecodePNG *Dec) {
        return Dec->iHDR->ColorType;
    }
    
    bool PNGGetInterlaceStatus(DecodePNG *Dec) {
        return Dec->iHDR->IsInterlaced;
    }
    
    bool PNGGetStereoscopicStatus(DecodePNG *Dec) {
        return Dec->PNGIs3D;
    }
    
    uint32_t PNGGetWhitepointX(DecodePNG *Dec) {
        return Dec->cHRM->WhitePointX;
    }
    
    uint32_t PNGGetWhitepointY(DecodePNG *Dec) {
        return Dec->cHRM->WhitePointY;
    }
    
    uint32_t PNGGetGamma(DecodePNG *Dec) {
        return Dec->gAMA->Gamma;
    }
    
    UTF8 *PNGGetColorProfileName(DecodePNG *Dec) {
        return Dec->iCCP->ProfileName;
    }
    
    uint8_t *PNGGetColorProfile(DecodePNG *Dec) {
        //return DecompressDEFLATE(Dec->iCCP->CompressedICCPProfile);
        return NULL;
    }
    /* PNG Get functions */
    
    /* PNG Set functions */
    void PNGSetIHDRChunk(EncodePNG *Enc, uint32_t Width, uint32_t Height, uint8_t BitDepth, bool Interlaced) {
        if (Enc != NULL) {
            if (Enc->iHDR == NULL) {
                Enc->iHDR           = calloc(1, sizeof(iHDR));
            }
            Enc->iHDR->Width        = Width;
            Enc->iHDR->Height       = Height;
            Enc->iHDR->BitDepth     = BitDepth;
            Enc->iHDR->IsInterlaced = Interlaced;
        } else {
            Log(Log_ERROR, __func__, U8("EncodePNG Pointer is NULL"));
        }
    }
    
    void PNGSetACTLChunk(EncodePNG *Enc, uint32_t NumFrames, uint32_t Times2Loop) {
        if (Enc != NULL) {
            if (Enc->acTL == NULL) {
                Enc->acTL          = calloc(1, sizeof(acTL));
            }
            Enc->acTL->NumFrames   = NumFrames;
            Enc->acTL->TimesToLoop = Times2Loop;
        } else {
            Log(Log_ERROR, __func__, U8("EncodePNG Pointer is NULL"));
        }
    }
    
    void PNGSetFCTLChunk(EncodePNG *Enc, uint32_t FrameNum, uint32_t Width, uint32_t Height);
    
    void PNGSetTextChunk(EncodePNG *Enc, UTF8 *KeywordString, UTF8 *CommentString) {
        if (Enc != NULL && KeywordString != NULL && CommentString != NULL) {
            // Check to see if the current Comment field has been set, if it has increment Enc->NumTextChunks, and realloc, otherwise just set it.
            if (Enc->Text[Enc->NumTextChunks - 1].Comment != NULL) {
                Enc->NumTextChunks += 1;
            }
            Enc->Text[Enc->NumTextChunks - 1].Keyword = KeywordString;
            Enc->Text[Enc->NumTextChunks - 1].Comment = CommentString; // TODO: When writing these chunks out, make sure to scan the Comments for Unicode, if it is, you have to use iTXt
        } else if (Enc == NULL) {
            Log(Log_ERROR, __func__, U8("EncodePNG Pointer is NULL"));
        } else if (KeywordString == NULL) {
            Log(Log_ERROR, __func__, U8("KeywordString Pointer is NULL"));
        } else if (CommentString == NULL) {
            Log(Log_ERROR, __func__, U8("CommentString Pointer is NULL"));
        }
    }
    
    /* PNG Set functions */
    
    void DecodePNG_Deinit(DecodePNG *Dec) {
        free(Dec->acTL);
        free(Dec->bkGD->BackgroundPaletteEntry);
        free(Dec->bkGD);
        free(Dec->cHRM);
        free(Dec->fcTL);
        free(Dec->fdAT);
        free(Dec->gAMA);
        free(Dec->hIST);
        free(Dec->iCCP->CompressedICCPProfile);
        free(Dec->iCCP->ProfileName);
        free(Dec->iCCP);
        free(Dec->oFFs);
        free(Dec->pCAL->CalibrationName);
        free(Dec->pCAL->UnitName);
        free(Dec->pCAL);
        free(Dec->PLTE->Palette);
        free(Dec->PLTE);
        free(Dec->sBIT);
        free(Dec->sRGB);
        free(Dec->sTER);
        for (uint32_t TextChunk = 0UL; TextChunk < Dec->NumTextChunks; TextChunk++) {
            free(Dec->Text[TextChunk].Keyword);
            free(Dec->Text[TextChunk].Comment);
        }
        free(Dec->Text);
        free(Dec->tIMe);
        free(Dec->tRNS->Palette);
        free(Dec->tRNS);
        free(Dec->iHDR);
        free(Dec);
    }
    
#ifdef __cplusplus
}
#endif
