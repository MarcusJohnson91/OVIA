#include "../../include/Private/CodecIO/PNGCodec.h"

#include "../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../Dependencies/FoundationIO/Library/include/TextIO/LogIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    void WriteIHDRChunk(PNGEncoder *Enc, BitBuffer *OutputPNG) {
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, 13);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, PNGMarker_iHDR);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, Enc->iHDR->Width);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, Enc->iHDR->Height);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 8,  Enc->iHDR->BitDepth);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 8,  Enc->iHDR->ColorType);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 8,  Enc->iHDR->Compression);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 8,  Enc->iHDR->FilterMethod);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 8,  Enc->iHDR->IsInterlaced);
    }

    void WriteACTLChunk(PNGEncoder *Enc, BitBuffer *OutputPNG) {
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, 8);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, PNGMarker_acTL);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, Enc->acTL->NumFrames);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, Enc->acTL->TimesToLoop);
    }

    void WriteFCTLChunk(PNGEncoder *Enc, BitBuffer *OutputPNG) {
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, 29);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, PNGMarker_fcTL);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, Enc->fcTL->FrameNum);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, Enc->fcTL->Width);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, Enc->fcTL->Height);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, Enc->fcTL->XOffset);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, Enc->fcTL->YOffset);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 16, Enc->fcTL->FrameDelayNumerator);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 16, Enc->fcTL->FrameDelayDenominator);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 8,  Enc->fcTL->DisposeMethod);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 8,  Enc->fcTL->BlendMethod);
    }

    void WriteFDATChunk(PNGEncoder *Enc, BitBuffer *OutputPNG, uint8_t *DeflatedFrameData, uint32_t DeflatedFrameDataSize) {
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, DeflatedFrameData + 8);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, PNGMarker_fDAT);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, Enc->fdAT->FrameNum);
    }

    void WriteSTERChunk(PNGEncoder *Enc, BitBuffer *OutputPNG) {
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, 1);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, PNGMarker_sTER);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 8,  Enc->sTER->StereoType);
    }

    void WriteBKGDChunk(PNGEncoder *Enc, BitBuffer *OutputPNG) {
        uint8_t  NumChannels   = PNGNumChannelsFromColorType[Enc->iHDR->ColorType];
        uint32_t Size          = 0;
        uint8_t  BKGDEntrySize = 0; // in bits

        if (Enc->iHDR->ColorType == PNGColor_Palette) {
            Size          = 1;
            BKGDEntrySize = Bytes2Bits(1);
        } else if (Enc->iHDR->ColorType == PNGColor_Gray || Enc->iHDR->ColorType == PNGColor_GrayAlpha) {
            Size          = 2;
            BKGDEntrySize = Bytes2Bits(2);
        } else if (Enc->iHDR->ColorType == PNGColor_RGB || Enc->iHDR->ColorType == PNGColor_RGBAlpha) {
            Size          = NumChannels * 2;
            BKGDEntrySize = Bytes2Bits(NumChannels * 2);
        }

        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, Size);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, PNGMarker_bKGD);

        if (Enc->iHDR->ColorType == PNGColor_Palette || Enc->iHDR->ColorType == PNGColor_Gray || Enc->iHDR->ColorType == PNGColor_GrayAlpha) {
            for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, BKGDEntrySize, Enc->bkGD->BackgroundPaletteEntry[Channel]);
            }
        } else if (Enc->iHDR->ColorType == PNGColor_RGB || Enc->iHDR->ColorType == PNGColor_RGBAlpha) {
            for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, NumChannels * 16, Enc->bkGD->BackgroundPaletteEntry[Channel]);
            }
        }
    }

    void WriteCHRMChunk(PNGEncoder *Enc, BitBuffer *OutputPNG) {
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, 32);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, PNGMarker_cHRM);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, Enc->cHRM->WhitePointX);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, Enc->cHRM->WhitePointY);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, Enc->cHRM->RedX);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, Enc->cHRM->RedY);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, Enc->cHRM->GreenX);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, Enc->cHRM->GreenY);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, Enc->cHRM->BlueX);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, Enc->cHRM->BlueY);
    }

    void WriteGAMAChunk(PNGEncoder *Enc, BitBuffer *OutputPNG) {
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, 4);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, PNGMarker_gAMA);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, Enc->gAMA->Gamma);
    }

    void WriteOFFSChunk(PNGEncoder *Enc, BitBuffer *OutputPNG) {
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, 9);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, PNGMarker_oFFs);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, Enc->oFFs->XOffset);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, Enc->oFFs->YOffset);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 8,  Enc->oFFs->UnitSpecifier);
    }

    void WriteICCPChunk(PNGEncoder *Enc, BitBuffer *OutputPNG) {
        uint32_t ProfileNameSize       = strlen(Enc->iCCP->ProfileName);
        uint32_t CompressedProfileSize = strlen(Enc->iCCP->CompressedICCPProfile);

        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, 8 + CompressedProfileSize + ProfileNameSize);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, PNGMarker_iCCP);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, Bytes2Bits(ProfileNameSize), Enc->iCCP->ProfileName);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 8, Enc->iCCP->CompressionType);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, Bytes2Bits(CompressedProfileSize), Enc->iCCP->CompressedICCPProfile);
    }

    void WriteSBITChunk(PNGEncoder *Enc, BitBuffer *OutputPNG) {
        uint8_t ChunkSize = 0;
        uint8_t NumChannels = PNGNumChannelsFromColorType[Enc->iHDR->ColorType];
        if (Enc->iHDR->ColorType == PNGColor_Gray) {
            ChunkSize = 1;
        } else if (Enc->iHDR->ColorType == PNGColor_RGB || Enc->iHDR->ColorType == PNGColor_Palette) {
            ChunkSize = 3;
        } else if (Enc->iHDR->ColorType == PNGColor_GrayAlpha) {
            ChunkSize = 2;
        } else if (Enc->iHDR->ColorType == PNGColor_RGBAlpha) {
            ChunkSize = 4;
        }

        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, ChunkSize);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, PNGMarker_sBIT);

        if (Enc->iHDR->ColorType == PNGColor_RGB || Enc->iHDR->ColorType == PNGColor_Palette) {
            BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 8, Enc->sBIT->Red);
            BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 8, Enc->sBIT->Green);
            BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 8, Enc->sBIT->Blue);
        } else if (Enc->iHDR->ColorType == PNGColor_RGBAlpha) {
            BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 8, Enc->sBIT->Red);
            BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 8, Enc->sBIT->Green);
            BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 8, Enc->sBIT->Blue);
            BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 8, Enc->sBIT->Alpha);
        } else if (Enc->iHDR->ColorType == PNGColor_Gray) {
            BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 8, Enc->sBIT->Grayscale);
        } else if (Enc->iHDR->ColorType == PNGColor_GrayAlpha) {
            BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 8, Enc->sBIT->Grayscale);
            BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 8, Enc->sBIT->Alpha);
        }
    }

    void WriteSRGBChunk(PNGEncoder *Enc, BitBuffer *OutputPNG) {
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 8, 1);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, PNGMarker_sRGB);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 8,  Enc->sRGB->RenderingIntent);
    }

    void WritePHYSChunk(PNGEncoder *Enc, BitBuffer *OutputPNG) {
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 8, 9);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, PNGMarker_pHYs);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, Enc->pHYs->PixelsPerUnitXAxis);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, Enc->pHYs->PixelsPerUnitYAxis);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 8,  Enc->pHYs->UnitSpecifier);
    }

    void WritePCALChunk(PNGEncoder *Enc, BitBuffer *OutputPNG) {
        uint32_t ChunkSize = 0;
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, ChunkSize);
        BitBuffer_WriteBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, OutputPNG, 32, PNGMarker_pCAL);

    }

    void WriteSCALChunk(PNGEncoder *Enc, BitBuffer *OutputPNG) {
        uint32_t ChunkSize = 0;
    }

    void PNGIsAnimated(PNGEncoder *Enc, const bool PNGIsAnimated) {
        if (Enc != NULL) {
            Enc->PNGIsAnimated = PNGIsAnimated;
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to PNGEncoder is NULL"));
        }
    }

    void ComposeAPNGNumFrames(PNGEncoder *Enc, const uint32_t NumFrames) {
        if (Enc != NULL) {
            Enc->acTL->NumFrames = NumFrames;
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to PNGEncoder is NULL"));
        }
    }

    void ComposeAPNGTimes2Loop(PNGEncoder *Enc, const uint32_t NumTimes2Loop) {
        if (Enc != NULL) {
            Enc->acTL->TimesToLoop = NumTimes2Loop;
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to PNGEncoder is NULL"));
        }
    }

    void ComposeAPNGFrameDelay(PNGEncoder *Enc, const uint32_t FrameDelayNumerator, const uint32_t FrameDelayDenominator) {
        if (Enc == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to PNGEncoder is NULL"));
        } else {
            Enc->fcTL->FrameDelayNumerator   = FrameDelayNumerator;
            Enc->fcTL->FrameDelayDenominator = FrameDelayDenominator;
        }
    }

    void PNGSetIHDR(PNGEncoder *Enc, const uint32_t Height, const uint32_t Width, const uint8_t BitDepth, PNGColorTypes ColorType, const bool Interlace) {
        if (Enc != NULL && Height > 0 && Width > 0 && (BitDepth > 0 || BitDepth > 16) && (ColorType <= 6 && ColorType != 1 && ColorType != 5) && Interlace >= 0 && Interlace <= 1) {
            Enc->iHDR->Height       = Height;
            Enc->iHDR->Width        = Width;
            Enc->iHDR->BitDepth     = BitDepth;
            Enc->iHDR->ColorType    = ColorType;
            Enc->iHDR->IsInterlaced = Interlace;
        } else if (Enc == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("PNGEncoder Pointer is NULL"));
        } else if (Height == 0) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Height is 0, which is invalid"));
        } else if (Width == 0) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Width is 0, which is invalid"));
        } else if (BitDepth == 0 || BitDepth > 16) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitDepth %d is invalid, valid values range from 1-16"), BitDepth);
        } else if (ColorType > 6 || ColorType == 1 || ColorType == 5) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("ColorType %d is invalid, valid values range from 0-6, excluding 1 and 5"), ColorType);
        } else if (Interlace > 1) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Interlace %d is invalid, valid values range from 0-1"), Interlace);
        }
    }

    void PNGSetSTER(PNGEncoder *Enc, const bool StereoType) {
        if (Enc != NULL && StereoType >= 0 && StereoType <= 1) {
            Enc->sTER->StereoType = StereoType;
        } else if (Enc == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to PNGEncoder is NULL"));
        } else if (StereoType < 0 || StereoType > 1) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("StereoType %d is invalid, valid values range from 0-1"));
        }
    }

    void PNGSetACTL(PNGEncoder *Enc, const uint32_t NumFrames, const uint32_t Times2Loop) {
        if (Enc != NULL && NumFrames > 0) {
            Enc->acTL->NumFrames   = NumFrames;
            Enc->acTL->TimesToLoop = Times2Loop;
        } else if (Enc == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to PNGEncoder is NULL"));
        } else if (NumFrames == 0) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("NumFrames is 0, that isn't possible..."));
        }
    }

    void PNGSetFCTL(PNGEncoder *Enc, const uint32_t FrameNum, const uint32_t FrameWidth, const uint32_t FrameHeight, uint32_t XOffset, uint32_t YOffset, uint16_t FrameDelayNumerator, uint16_t FrameDelayDenominator, uint8_t DisposalType, uint8_t BlendType) {

    }

    void ComposePNGCHRMWhitePoint() {

    }

    PNGEncoder *EncodePNG_Init(void) {
        PNGEncoder *Enc  = calloc(1, sizeof(PNGEncoder));
        if (Enc != NULL) {
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
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Failed to allocate enough memory for PNGEncoder"));
        }
        return Enc;
    }

    void EncodePNG_Deinit(PNGEncoder *Enc) {
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
            for (uint32_t TextChunk = 0UL; TextChunk < Enc->NumTextChunks; TextChunk++) {
                free(Enc->Text[TextChunk].Keyword);
                free(Enc->Text[TextChunk].Comment);
            }
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

    BitBuffer *EncodePNGImage(PNGEncoder *Enc, void ****RawImage2Encode, bool InterlacePNG, bool OptimizePNG) {
        // RawImage2Encode is a void pointer, the contents of Enc->iHDR->BitDepth will define the data type, uint8_t or uint16_t

        // This is THE main function for encoding a buffer into a PNG file.
        return NULL;
    }

    BitBuffer *EncodeAdam7(PNGEncoder *Enc, BitBuffer *ProgressiveImage) { // Returns the interlaced image
        /*
         Ok, so to interlace an image, we need to loop over every pixel in an 8x8 block?
         */
        BitBuffer *InterlacedImage = NULL;
        return InterlacedImage;
    }

    void PNGEncodeFilterNone(PNGEncoder *Enc, uint8_t *DeEntropyedData, uint8_t *DeFilteredData, size_t Line) {

    }

    void PNGEncodeFilterPaeth(PNGEncoder *Enc, uint8_t *Line, size_t LineSize) {
        // RawData is after decoding the I/f DATs, and after INFLAT'ing and De-LZ77'ing it.
        // Each line is preceded by a filter type byte, so decode it by a line by line basis.
        // Good candidate for multi-threading.
    }

    void PNGEncodeFilterSub(PNGEncoder *Enc, uint8_t *Line, size_t NumPixels) {
        // NumPixel means whole pixel not sub pixel.
        uint16_t *EncodedLine = calloc(1, Enc->iHDR->Width * Bits2Bytes(Enc->iHDR->BitDepth, Yes) * sizeof(uint16_t));
        for (size_t Pixel = 1; Pixel < NumPixels; Pixel++) {
            if (Pixel == 1) {
                EncodedLine[Pixel] = Line[Pixel];
            } else {
                EncodedLine[Pixel] = Line[Pixel] - Line[Pixel - 1];
            }
        }
        free(EncodedLine);
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
