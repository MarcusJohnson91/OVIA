#include "../../include/CodecIO/PNGCodec.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h" /* Included for Assertions */
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    uint32_t PNG_CalculateSTERPadding(const uint32_t Width) {
        return 7 - ((Width - 1) % 8);
    }

    void *PNGOptions_Init(void) {
        PNGOptions *Options     = calloc(1, sizeof(PNGOptions));
        AssertIO(Options != NULL);
        Options->acTL       = calloc(1, sizeof(acTL));
        Options->bkGD       = calloc(1, sizeof(bkGD));
        Options->cHRM       = calloc(1, sizeof(cHRM));
        Options->fcTL       = calloc(1, sizeof(fcTL));
        Options->fdAT       = calloc(1, sizeof(fdAT));
        Options->gAMA       = calloc(1, sizeof(gAMA));
        Options->hIST       = calloc(1, sizeof(hIST));
        Options->iCCP       = calloc(1, sizeof(iCCP));
        Options->iHDR       = calloc(1, sizeof(iHDR));
        Options->oFFs       = calloc(1, sizeof(oFFs));
        Options->pCAL       = calloc(1, sizeof(pCAL));
        Options->PLTE       = calloc(1, sizeof(PLTE));
        Options->sBIT       = calloc(1, sizeof(sBIT));
        Options->sRGB       = calloc(1, sizeof(sRGB));
        Options->sTER       = calloc(1, sizeof(sTER));
        Options->Text       = calloc(1, sizeof(Text));
        Options->tIMe       = calloc(1, sizeof(tIMe));
        Options->tRNS       = calloc(1, sizeof(tRNS));
        return Options;
    }

    /* PNG Get functions */
    UTF8 *PNGGetTextChunk(PNGOptions *Options, uint32_t Instance, UTF8 *Keyword) {
        AssertIO(Options != NULL);
        AssertIO(Keyword != NULL);
        Keyword = Options->Text[Instance].Keyword;
        return Options->Text[Instance].Comment;
    }

    uint32_t PNGGetNumTextChunks(PNGOptions *Options) {
        AssertIO(Options != NULL);
        return Options->NumTextChunks;
    }

    uint32_t PNGGetWidth(PNGOptions *Options) {
        AssertIO(Options != NULL);
        return Options->iHDR->Width;
    }

    uint32_t PNGGetHeight(PNGOptions *Options) {
        AssertIO(Options != NULL);
        return Options->iHDR->Height;
    }

    uint8_t PNGGetBitDepth(PNGOptions *Options) {
        AssertIO(Options != NULL);
        return Options->iHDR->BitDepth;
    }

    uint8_t PNGGetColorType(PNGOptions *Options) {
        AssertIO(Options != NULL);
        return Options->iHDR->ColorType;
    }

    bool PNGGetInterlaceStatus(PNGOptions *Options) {
        AssertIO(Options != NULL);
        return Options->iHDR->IsInterlaced;
    }

    bool PNGGetStereoscopicStatus(PNGOptions *Options) {
        AssertIO(Options != NULL);
        return Options->PNGIs3D;
    }

    uint32_t PNGGetWhitepointX(PNGOptions *Options) {
        AssertIO(Options != NULL);
        return Options->cHRM->WhitePointX;
    }

    uint32_t PNGGetWhitepointY(PNGOptions *Options) {
        AssertIO(Options != NULL);
        return Options->cHRM->WhitePointY;
    }

    uint32_t PNGGetGamma(PNGOptions *Options) {
        AssertIO(Options != NULL);
        return Options->gAMA->Gamma;
    }

    UTF8 *PNGGetColorProfileName(PNGOptions *Options) {
        AssertIO(Options != NULL);
        return Options->iCCP->ProfileName;
    }

    uint8_t *PNGGetColorProfile(PNGOptions *Options) {
        AssertIO(Options != NULL);
        //return DecompressDEFLATE(Options->iCCP->CompressedICCPProfile);
        return NULL;
    }
    /* PNG Get functions */

    /* PNG Set functions */
    void PNGSetIHDRChunk(PNGOptions *Options, uint32_t Width, uint32_t Height, uint8_t BitDepth, bool Interlaced) {
        AssertIO(Options != NULL);
        if (Options->iHDR == NULL) {
            Options->iHDR           = calloc(1, sizeof(iHDR));
        }
        Options->iHDR->Width        = Width;
        Options->iHDR->Height       = Height;
        Options->iHDR->BitDepth     = BitDepth;
        Options->iHDR->IsInterlaced = Interlaced;
    }

    void PNGSetACTLChunk(PNGOptions *Options, uint32_t NumFrames, uint32_t Times2Loop) {
        AssertIO(Options != NULL);
        if (Options->acTL == NULL) {
            Options->acTL          = calloc(1, sizeof(acTL));
        }
        Options->acTL->NumFrames   = NumFrames;
        Options->acTL->TimesToLoop = Times2Loop;
    }

    void PNGSetFCTLChunk(PNGOptions *Options, uint32_t FrameNum, uint32_t Width, uint32_t Height) {
        AssertIO(Options != NULL);
    }

    void PNGSetTextChunk(PNGOptions *Options, UTF8 *KeywordString, UTF8 *CommentString) {
        AssertIO(Options != NULL);
        AssertIO(KeywordString != NULL);
        AssertIO(CommentString != NULL);
        // Check to see if the current Comment field has been set, if it has increment Options->NumTextChunks, and realloc, otherwise just set it.
        if (Options->Text[Options->NumTextChunks - 1].Comment != NULL) {
            Options->NumTextChunks += 1;
        }
        Options->Text[Options->NumTextChunks - 1].Keyword = KeywordString;
        Options->Text[Options->NumTextChunks - 1].Comment = CommentString; // TODO: When writing these chunks out, make sure to scan the Comments for Unicode, if it is, you have to use iTXt
    }

    void PNGOptions_Deinit(PNGOptions *Options) {
        AssertIO(Options != NULL);
        free(Options->acTL);
        free(Options->bkGD->BackgroundPaletteEntry);
        free(Options->bkGD);
        free(Options->cHRM);
        free(Options->fcTL);
        free(Options->fdAT);
        free(Options->gAMA);
        free(Options->hIST);
        free(Options->iCCP->CompressedICCPProfile);
        free(Options->iCCP->ProfileName);
        free(Options->iCCP);
        free(Options->oFFs);
        free(Options->pCAL->CalibrationName);
        free(Options->pCAL->UnitName);
        free(Options->pCAL);
        free(Options->PLTE->Palette);
        free(Options->PLTE);
        free(Options->sBIT);
        free(Options->sRGB);
        free(Options->sTER);
        free(Options->tIMe);
        free(Options->tRNS->Palette);
        free(Options->tRNS);
        free(Options->iHDR);
        for (uint32_t TextChunk = 0UL; TextChunk < Options->NumTextChunks; TextChunk++) {
            free(Options->Text[TextChunk].Keyword);
            free(Options->Text[TextChunk].Comment);
        }
        free(Options->Text);
        free(Options);
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
