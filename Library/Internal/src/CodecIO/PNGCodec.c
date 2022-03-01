#include "../../include/CodecIO/PNGCodec.h"

#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/TextIO/LogIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    uint32_t PNG_CalculateSTERPadding(const uint32_t Width) {
        return 7 - ((Width - 1) % 8);
    }

    void *PNGOptions_Init(void) {
        PNGOptions *Dec     = calloc(1, sizeof(PNGOptions));
        if (Dec != NULL) {
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
        }
        return Dec;
    }

    /* PNG Get functions */
    UTF8 *PNGGetTextChunk(PNGOptions *Dec, uint32_t Instance, UTF8 *Keyword) {
        UTF8 *Comment = NULL;
        if (Dec != NULL && Instance <= Dec->NumTextChunks - 1) {
            Keyword = Dec->Text[Instance].Keyword;
            Comment = Dec->Text[Instance].Comment;
        } else if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("PNGOptions Pointer is NULL"));
        } else if (Instance > Dec->NumTextChunks - 1) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Instance %ud is greater than there are Text chunks %ud"), Instance, Dec->NumTextChunks);
        }
        return Comment;
    }

    uint32_t PNGGetNumTextChunks(PNGOptions *Dec) {
        uint32_t NumTextChunks = 0UL;
        if (Dec != NULL) {
            NumTextChunks      = Dec->NumTextChunks;
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("PNGOptions Pointer is NULL"));
        }
        return NumTextChunks;
    }

    uint32_t PNGGetWidth(PNGOptions *Dec) {
        return Dec->iHDR->Width;
    }

    uint32_t PNGGetHeight(PNGOptions *Dec) {
        return Dec->iHDR->Height;
    }

    uint8_t PNGGetBitDepth(PNGOptions *Dec) {
        return Dec->iHDR->BitDepth;
    }

    uint8_t PNGGetColorType(PNGOptions *Dec) {
        return Dec->iHDR->ColorType;
    }

    bool PNGGetInterlaceStatus(PNGOptions *Dec) {
        return Dec->iHDR->IsInterlaced;
    }

    bool PNGGetStereoscopicStatus(PNGOptions *Dec) {
        return Dec->PNGIs3D;
    }

    uint32_t PNGGetWhitepointX(PNGOptions *Dec) {
        return Dec->cHRM->WhitePointX;
    }

    uint32_t PNGGetWhitepointY(PNGOptions *Dec) {
        return Dec->cHRM->WhitePointY;
    }

    uint32_t PNGGetGamma(PNGOptions *Dec) {
        return Dec->gAMA->Gamma;
    }

    UTF8 *PNGGetColorProfileName(PNGOptions *Dec) {
        return Dec->iCCP->ProfileName;
    }

    uint8_t *PNGGetColorProfile(PNGOptions *Dec) {
        //return DecompressDEFLATE(Dec->iCCP->CompressedICCPProfile);
        return NULL;
    }
    /* PNG Get functions */

    /* PNG Set functions */
    void PNGSetIHDRChunk(PNGOptions *Enc, uint32_t Width, uint32_t Height, uint8_t BitDepth, bool Interlaced) {
        if (Enc != NULL) {
            if (Enc->iHDR == NULL) {
                Enc->iHDR           = calloc(1, sizeof(iHDR));
            }
            Enc->iHDR->Width        = Width;
            Enc->iHDR->Height       = Height;
            Enc->iHDR->BitDepth     = BitDepth;
            Enc->iHDR->IsInterlaced = Interlaced;
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("PNGOptions Pointer is NULL"));
        }
    }

    void PNGSetACTLChunk(PNGOptions *Enc, uint32_t NumFrames, uint32_t Times2Loop) {
        if (Enc != NULL) {
            if (Enc->acTL == NULL) {
                Enc->acTL          = calloc(1, sizeof(acTL));
            }
            Enc->acTL->NumFrames   = NumFrames;
            Enc->acTL->TimesToLoop = Times2Loop;
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("PNGOptions Pointer is NULL"));
        }
    }

    void PNGSetFCTLChunk(PNGOptions *Enc, uint32_t FrameNum, uint32_t Width, uint32_t Height);

    void PNGSetTextChunk(PNGOptions *Enc, UTF8 *KeywordString, UTF8 *CommentString) {
        if (Enc != NULL && KeywordString != NULL && CommentString != NULL) {
            // Check to see if the current Comment field has been set, if it has increment Enc->NumTextChunks, and realloc, otherwise just set it.
            if (Enc->Text[Enc->NumTextChunks - 1].Comment != NULL) {
                Enc->NumTextChunks += 1;
            }
            Enc->Text[Enc->NumTextChunks - 1].Keyword = KeywordString;
            Enc->Text[Enc->NumTextChunks - 1].Comment = CommentString; // TODO: When writing these chunks out, make sure to scan the Comments for Unicode, if it is, you have to use iTXt
        } else if (Enc == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("PNGOptions Pointer is NULL"));
        } else if (KeywordString == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("KeywordString Pointer is NULL"));
        } else if (CommentString == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CommentString Pointer is NULL"));
        }
    }

    void PNGOptions_Deinit(void *Options) {
        PNGOptions *Dec = Options;
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
        free(Dec->tIMe);
        free(Dec->tRNS->Palette);
        free(Dec->tRNS);
        free(Dec->iHDR);
        for (uint32_t TextChunk = 0UL; TextChunk < Dec->NumTextChunks; TextChunk++) {
            free(Dec->Text[TextChunk].Keyword);
            free(Dec->Text[TextChunk].Comment);
        }
        free(Dec->Text);
        free(Dec);
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
