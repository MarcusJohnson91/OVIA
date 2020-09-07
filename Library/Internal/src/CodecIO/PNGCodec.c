#include "../../include/Private/CodecIO/PNGCodec.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    uint32_t CalculateSTERPadding(const uint32_t Width) {
        return 7 - ((Width - 1) % 8);
    }

    PNGDecoder *PNGDecoder_Init(void) {
        PNGDecoder *Dec     = calloc(1, sizeof(PNGDecoder));
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
    UTF8 *PNGGetTextChunk(PNGDecoder *Dec, uint32_t Instance, UTF8 *Keyword) {
        UTF8 *Comment = NULL;
        if (Dec != NULL && Instance <= Dec->NumTextChunks - 1) {
            Keyword = Dec->Text[Instance].Keyword;
            Comment = Dec->Text[Instance].Comment;
        } else if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("PNGDecoder Pointer is NULL"));
        } else if (Instance > Dec->NumTextChunks - 1) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Instance %ud is greater than there are Text chunks %ud"), Instance, Dec->NumTextChunks);
        }
        return Comment;
    }

    uint32_t PNGGetNumTextChunks(PNGDecoder *Dec) {
        uint32_t NumTextChunks = 0UL;
        if (Dec != NULL) {
            NumTextChunks      = Dec->NumTextChunks;
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("PNGDecoder Pointer is NULL"));
        }
        return NumTextChunks;
    }

    uint32_t PNGGetWidth(PNGDecoder *Dec) {
        return Dec->iHDR->Width;
    }

    uint32_t PNGGetHeight(PNGDecoder *Dec) {
        return Dec->iHDR->Height;
    }

    uint8_t PNGGetBitDepth(PNGDecoder *Dec) {
        return Dec->iHDR->BitDepth;
    }

    uint8_t PNGGetColorType(PNGDecoder *Dec) {
        return Dec->iHDR->ColorType;
    }

    bool PNGGetInterlaceStatus(PNGDecoder *Dec) {
        return Dec->iHDR->IsInterlaced;
    }

    bool PNGGetStereoscopicStatus(PNGDecoder *Dec) {
        return Dec->PNGIs3D;
    }

    uint32_t PNGGetWhitepointX(PNGDecoder *Dec) {
        return Dec->cHRM->WhitePointX;
    }

    uint32_t PNGGetWhitepointY(PNGDecoder *Dec) {
        return Dec->cHRM->WhitePointY;
    }

    uint32_t PNGGetGamma(PNGDecoder *Dec) {
        return Dec->gAMA->Gamma;
    }

    UTF8 *PNGGetColorProfileName(PNGDecoder *Dec) {
        return Dec->iCCP->ProfileName;
    }

    uint8_t *PNGGetColorProfile(PNGDecoder *Dec) {
        //return DecompressDEFLATE(Dec->iCCP->CompressedICCPProfile);
        return NULL;
    }
    /* PNG Get functions */

    /* PNG Set functions */
    void PNGSetIHDRChunk(PNGEncoder *Enc, uint32_t Width, uint32_t Height, uint8_t BitDepth, bool Interlaced) {
        if (Enc != NULL) {
            if (Enc->iHDR == NULL) {
                Enc->iHDR           = calloc(1, sizeof(iHDR));
            }
            Enc->iHDR->Width        = Width;
            Enc->iHDR->Height       = Height;
            Enc->iHDR->BitDepth     = BitDepth;
            Enc->iHDR->IsInterlaced = Interlaced;
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("PNGEncoder Pointer is NULL"));
        }
    }

    void PNGSetACTLChunk(PNGEncoder *Enc, uint32_t NumFrames, uint32_t Times2Loop) {
        if (Enc != NULL) {
            if (Enc->acTL == NULL) {
                Enc->acTL          = calloc(1, sizeof(acTL));
            }
            Enc->acTL->NumFrames   = NumFrames;
            Enc->acTL->TimesToLoop = Times2Loop;
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("PNGEncoder Pointer is NULL"));
        }
    }

    void PNGSetFCTLChunk(PNGEncoder *Enc, uint32_t FrameNum, uint32_t Width, uint32_t Height);

    void PNGSetTextChunk(PNGEncoder *Enc, UTF8 *KeywordString, UTF8 *CommentString) {
        if (Enc != NULL && KeywordString != NULL && CommentString != NULL) {
            // Check to see if the current Comment field has been set, if it has increment Enc->NumTextChunks, and realloc, otherwise just set it.
            if (Enc->Text[Enc->NumTextChunks - 1].Comment != NULL) {
                Enc->NumTextChunks += 1;
            }
            Enc->Text[Enc->NumTextChunks - 1].Keyword = KeywordString;
            Enc->Text[Enc->NumTextChunks - 1].Comment = CommentString; // TODO: When writing these chunks out, make sure to scan the Comments for Unicode, if it is, you have to use iTXt
        } else if (Enc == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("PNGEncoder Pointer is NULL"));
        } else if (KeywordString == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("KeywordString Pointer is NULL"));
        } else if (CommentString == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CommentString Pointer is NULL"));
        }
    }

    uint32_t GenerateCRC32(BitBuffer *BitB, const uint64_t ChunkSize) {
        uint64_t CRC32Polynomial = 0x104C11DB7;
        uint32_t CRC32PolyInit   = 0xFFFFFFFF;
        uint8_t  CurrentBit      = 0;
        bool     Reflect         = No;
        bool     Reverse         = No;
        /*
        for (uint64_t Bit = BitOffset; Bit < BitOffset + BitLength; Bit++) {
            // Now we need to select a bit from the buffer
            CurrentBit = BitBuffer_ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);

        }
         */
        /*
         Ok, so we already know the layout of the data hasn't been changed, and it's faster to iterate over the BitBuffer than copy variables back and forth.
         We also know the polynomial, size of the polynomial, and the init for the Poly, these are all constants.
         So, all we really have to do is Init the poly, and feed bits through the algorithm.
         Thanks to the PNG algorithm, all of our offsets should be at byte boundaries, so we should be able to loop over bytes.
         */

        return 0ULL;
    }

    bool VerifyCRC32(BitBuffer *BitB, uint64_t ChunkSize) {
        bool CRCsMatch = No;
        // Lets wrap Data2CRC in a BitBuffer

        uint32_t CalculatedCRC = 0UL;

        for (uint64_t Bit = 0; Bit < Bytes2Bits(ChunkSize); Bit++) {
            /*
             So, we read a bit, what do we do with it after that?
             Well, we need to build a XOR gate based on the CRC32 polynomial, x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x+1 aka 0x104C11DB7
             The highest bit is always 0, so the XOR'd value is always 1, so lets go ahead and remove that from the Polynomial.
             Polynomial: 0x4C11DB7
             */

        }
        uint32_t EmbeddedCRC = BitBuffer_ReadBits(ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, BitB, 32);
        if (CRCsMatch == EmbeddedCRC) {
            CRCsMatch  = Yes;
        }
        return CRCsMatch;
    }

    uint32_t GenerateAdler32(const uint8_t *Data, const uint64_t DataSize) {
        uint32_t Adler  = 1;
        uint32_t Sum1   = Adler & 0xFFFF;
        uint32_t Sum2   = (Adler >> 16) & 0xFFFF;

        for (uint64_t Byte = 0; Byte < DataSize; Byte++) {
            Sum1 += Data[Byte] % 65521;
            Sum2 += Sum1 % 65521;
        }
        return (Sum2 << 16) + Sum1;
    }

    bool VerifyAdler32(const uint8_t *Data, const uint64_t DataSize, const uint32_t EmbeddedAdler32) {
        uint32_t GeneratedAdler32 = GenerateAdler32(Data, DataSize);
        if (GeneratedAdler32 != EmbeddedAdler32) {
            return false;
        } else {
            return true;
        }
    }

    void PNGDecoder_Deinit(PNGDecoder *Dec) {
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

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
