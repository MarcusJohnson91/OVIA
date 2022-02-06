#include "../../../include/Private/CodecIO/PNGCodec.h"
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/TextIO/StringIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/TextIO/LogIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    void WriteIHDRChunk(PNGOptions *Enc, BitBuffer *OutputPNG) {
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, 13);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, PNGMarker_iHDR);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, Enc->iHDR->Width);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, Enc->iHDR->Height);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8,  Enc->iHDR->BitDepth);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8,  Enc->iHDR->ColorType);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8,  Enc->iHDR->Compression);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8,  Enc->iHDR->FilterMethod);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8,  Enc->iHDR->IsInterlaced);
    }

    void WriteACTLChunk(PNGOptions *Enc, BitBuffer *OutputPNG) {
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, 8);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, PNGMarker_acTL);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, Enc->acTL->NumFrames);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, Enc->acTL->TimesToLoop);
    }

    void WriteFCTLChunk(PNGOptions *Enc, BitBuffer *OutputPNG) {
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, 29);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, PNGMarker_fcTL);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, Enc->fcTL->FrameNum);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, Enc->fcTL->Width);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, Enc->fcTL->Height);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, Enc->fcTL->XOffset);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, Enc->fcTL->YOffset);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16, Enc->fcTL->FrameDelayNumerator);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16, Enc->fcTL->FrameDelayDenominator);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8,  Enc->fcTL->DisposeMethod);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8,  Enc->fcTL->BlendMethod);
    }

    void WriteFDATChunk(PNGOptions *Enc, BitBuffer *OutputPNG, uint8_t *DeflatedFrameData, uint32_t DeflatedFrameDataSize) {
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, DeflatedFrameData + 8);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, PNGMarker_fDAT);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, Enc->fdAT->FrameNum);
    }

    void WriteSTERChunk(PNGOptions *Enc, BitBuffer *OutputPNG) {
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, 1);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, PNGMarker_sTER);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8,  Enc->sTER->StereoType);
    }

    void WriteBKGDChunk(PNGOptions *Enc, BitBuffer *OutputPNG) {
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

        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, Size);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, PNGMarker_bKGD);

        if (Enc->iHDR->ColorType == PNGColor_Palette || Enc->iHDR->ColorType == PNGColor_Gray || Enc->iHDR->ColorType == PNGColor_GrayAlpha) {
            for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BKGDEntrySize, Enc->bkGD->BackgroundPaletteEntry[Channel]);
            }
        } else if (Enc->iHDR->ColorType == PNGColor_RGB || Enc->iHDR->ColorType == PNGColor_RGBAlpha) {
            for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, NumChannels * 16, Enc->bkGD->BackgroundPaletteEntry[Channel]);
            }
        }
    }

    void WriteCHRMChunk(PNGOptions *Enc, BitBuffer *OutputPNG) {
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, 32);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, PNGMarker_cHRM);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, Enc->cHRM->WhitePointX);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, Enc->cHRM->WhitePointY);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, Enc->cHRM->RedX);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, Enc->cHRM->RedY);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, Enc->cHRM->GreenX);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, Enc->cHRM->GreenY);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, Enc->cHRM->BlueX);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, Enc->cHRM->BlueY);
    }

    void WriteGAMAChunk(PNGOptions *Enc, BitBuffer *OutputPNG) {
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, 4);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, PNGMarker_gAMA);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, Enc->gAMA->Gamma);
    }

    void WriteOFFSChunk(PNGOptions *Enc, BitBuffer *OutputPNG) {
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, 9);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, PNGMarker_oFFs);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, Enc->oFFs->XOffset);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, Enc->oFFs->YOffset);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8,  Enc->oFFs->UnitSpecifier);
    }

    void WriteICCPChunk(PNGOptions *Enc, BitBuffer *OutputPNG) {
        uint32_t ProfileNameSize       = UTF8_GetStringSizeInCodeUnits(Enc->iCCP->ProfileName);
        uint32_t CompressedProfileSize = UTF8_GetStringSizeInCodeUnits(Enc->iCCP->CompressedICCPProfile);

        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, 8 + CompressedProfileSize + ProfileNameSize);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, PNGMarker_iCCP);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, Bytes2Bits(ProfileNameSize), Enc->iCCP->ProfileName);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8, Enc->iCCP->CompressionType);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, Bytes2Bits(CompressedProfileSize), Enc->iCCP->CompressedICCPProfile);
    }

    void WriteSBITChunk(PNGOptions *Enc, BitBuffer *OutputPNG) {
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

        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, ChunkSize);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, PNGMarker_sBIT);

        if (Enc->iHDR->ColorType == PNGColor_RGB || Enc->iHDR->ColorType == PNGColor_Palette) {
            BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8, Enc->sBIT->Red);
            BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8, Enc->sBIT->Green);
            BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8, Enc->sBIT->Blue);
        } else if (Enc->iHDR->ColorType == PNGColor_RGBAlpha) {
            BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8, Enc->sBIT->Red);
            BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8, Enc->sBIT->Green);
            BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8, Enc->sBIT->Blue);
            BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8, Enc->sBIT->Alpha);
        } else if (Enc->iHDR->ColorType == PNGColor_Gray) {
            BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8, Enc->sBIT->Grayscale);
        } else if (Enc->iHDR->ColorType == PNGColor_GrayAlpha) {
            BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8, Enc->sBIT->Grayscale);
            BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8, Enc->sBIT->Alpha);
        }
    }

    void WriteSRGBChunk(PNGOptions *Enc, BitBuffer *OutputPNG) {
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8, 1);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, PNGMarker_sRGB);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8,  Enc->sRGB->RenderingIntent);
    }

    void WritePHYSChunk(PNGOptions *Enc, BitBuffer *OutputPNG) {
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8, 9);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, PNGMarker_pHYs);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, Enc->pHYs->PixelsPerUnitXAxis);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, Enc->pHYs->PixelsPerUnitYAxis);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8,  Enc->pHYs->UnitSpecifier);
    }

    void WritePCALChunk(PNGOptions *Enc, BitBuffer *OutputPNG) {
        uint32_t ChunkSize = 0;
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, ChunkSize);
        BitBuffer_WriteBits(OutputPNG, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, PNGMarker_pCAL);

    }

    void WriteSCALChunk(PNGOptions *Enc, BitBuffer *OutputPNG) {
        uint32_t ChunkSize = 0;
    }

    void PNGIsAnimated(PNGOptions *Enc, const bool PNGIsAnimated) {
        if (Enc != NULL) {
            Enc->PNGIsAnimated = PNGIsAnimated;
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to PNGOptions is NULL"));
        }
    }

    void ComposeAPNGNumFrames(PNGOptions *Enc, const uint32_t NumFrames) {
        if (Enc != NULL) {
            Enc->acTL->NumFrames = NumFrames;
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to PNGOptions is NULL"));
        }
    }

    void ComposeAPNGTimes2Loop(PNGOptions *Enc, const uint32_t NumTimes2Loop) {
        if (Enc != NULL) {
            Enc->acTL->TimesToLoop = NumTimes2Loop;
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to PNGOptions is NULL"));
        }
    }

    void ComposeAPNGFrameDelay(PNGOptions *Enc, const uint32_t FrameDelayNumerator, const uint32_t FrameDelayDenominator) {
        if (Enc == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to PNGOptions is NULL"));
        } else {
            Enc->fcTL->FrameDelayNumerator   = FrameDelayNumerator;
            Enc->fcTL->FrameDelayDenominator = FrameDelayDenominator;
        }
    }

    void PNGSetIHDR(PNGOptions *Enc, const uint32_t Height, const uint32_t Width, const uint8_t BitDepth, PNGColorTypes ColorType, const bool Interlace) {
        if (Enc != NULL && Height > 0 && Width > 0 && (BitDepth > 0 || BitDepth > 16) && (ColorType <= 6 && ColorType != 1 && ColorType != 5) && Interlace >= 0 && Interlace <= 1) {
            Enc->iHDR->Height       = Height;
            Enc->iHDR->Width        = Width;
            Enc->iHDR->BitDepth     = BitDepth;
            Enc->iHDR->ColorType    = ColorType;
            Enc->iHDR->IsInterlaced = Interlace;
        } else if (Enc == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("PNGOptions Pointer is NULL"));
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

    void PNGSetSTER(PNGOptions *Enc, const bool StereoType) {
        if (Enc != NULL && StereoType >= 0 && StereoType <= 1) {
            Enc->sTER->StereoType = StereoType;
        } else if (Enc == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to PNGOptions is NULL"));
        } else if (StereoType < 0 || StereoType > 1) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("StereoType %d is invalid, valid values range from 0-1"), StereoType);
        }
    }

    void PNGSetACTL(PNGOptions *Enc, const uint32_t NumFrames, const uint32_t Times2Loop) {
        if (Enc != NULL && NumFrames > 0) {
            Enc->acTL->NumFrames   = NumFrames;
            Enc->acTL->TimesToLoop = Times2Loop;
        } else if (Enc == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to PNGOptions is NULL"));
        } else if (NumFrames == 0) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("NumFrames is 0, that isn't possible..."));
        }
    }

    void PNGSetFCTL(PNGOptions *Enc, const uint32_t FrameNum, const uint32_t FrameWidth, const uint32_t FrameHeight, uint32_t XOffset, uint32_t YOffset, uint16_t FrameDelayNumerator, uint16_t FrameDelayDenominator, uint8_t DisposalType, uint8_t BlendType) {

    }

    void ComposePNGCHRMWhitePoint() {

    }

    PNGOptions *EncodePNG_Init(void) {
        PNGOptions *Enc  = calloc(1, sizeof(PNGOptions));
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
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Failed to allocate enough memory for PNGOptions"));
        }
        return Enc;
    }

    void EncodePNG_Deinit(PNGOptions *Enc) {
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

    BitBuffer *EncodePNGImage(PNGOptions *Enc, void ****RawImage2Encode, bool InterlacePNG, bool OptimizePNG) {
        // RawImage2Encode is a void pointer, the contents of Enc->iHDR->BitDepth will define the data type, uint8_t or uint16_t

        // This is THE main function for encoding a buffer into a PNG file.
        return NULL;
    }

    BitBuffer *EncodeAdam7(PNGOptions *Enc, BitBuffer *ProgressiveImage) { // Returns the interlaced image
        /*
         Ok, so to interlace an image, we need to loop over every pixel in an 8x8 block?
         */
        BitBuffer *InterlacedImage = NULL;
        return InterlacedImage;
    }

    void PNGEncodeFilterNone(PNGOptions *Enc, uint8_t *DeEntropyedData, uint8_t *DeFilteredData, size_t Line) {

    }

    void PNGEncodeFilterPaeth(PNGOptions *Enc, uint8_t *Line, size_t LineSize) {
        // RawData is after decoding the I/f DATs, and after INFLAT'ing and De-LZ77'ing it.
        // Each line is preceded by a filter type byte, so decode it by a line by line basis.
        // Good candidate for multi-threading.
    }

    void PNGEncodeFilterSub(PNGOptions *Enc, uint8_t *Line, size_t NumPixels) {
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

    /*
     For now, I'm just going to store the image in Deflate as literals.

     it will be a valid PNG so for now who cares?
     */

    void PNGWriteHeader(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            // Write the iHDR chunk, and then go through the list of other chunks to find out if other chunks should be written
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void PNGWriteFooter(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            uint32_t iENDSize = 0;
            uint32_t iENDCRC  = 0xAE426082;
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32, iENDSize);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32, PNGMarker_iEND);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32, iENDCRC);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void PNG_DAT_WriteZlibHeader(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG           = Options;
            bool FDICT                = 0;

            uint8_t CompressionInfo   = 7;
            uint8_t CompressionMethod = 8;

            uint16_t FCHECK           = CompressionInfo << 12;
            FCHECK                   |= CompressionMethod << 8;
            FCHECK                   |= FDICT << 6;
            FCHECK                   |= FDICT << 5;

            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 4, CompressionMethod);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 4, CompressionInfo);

            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 5, FCHECK);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1, FDICT);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 2, 0); // Huh?

            if (FDICT == Yes) {
                BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, PNG->DAT->DictID);
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void PNG_Flate_WriteLiteralBlock(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            BitBuffer_Align(BitB, 1);
            // How do we know how many bytes to copy?
            // Let's keep a offset in the PNGOptions struct saying the last written pixel so we can keep track.
            uint16_t Bytes2Copy  = 0;
            uint16_t Bytes2Copy2 = Bytes2Copy ^ 0xFFFF;

            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16, Bytes2Copy);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16, Bytes2Copy2);

            // Now we simply copy bytes from the image container to the BitBuffer; the max num bytes is 65536

            /*
             Block max size is 65536.

             We need to find out the total number of bytes in the ImageContainer by counting the number of Views * the Width * Height * Num Channels * BitDepth
             */



        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void PNG_Image_Insert(ImageContainer *Image, BitBuffer *BitB, bool OptimizePNG) {
        if (Image != NULL && BitB != NULL) {
            /*
             Loop over the image container, line by line, and try all the filters.
             */
        } else if (Image == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void PNG_Filter_Image(ImageContainer *Image) {
        if (Image != NULL) {
            // Try each filter on each line, get the best by counting the diff between the symbols to decide which to use.
        } else if (Image == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Image Pointer is NULL"));
        }
    }

    void PNGIsAnimated(EncodePNG *Enc, const bool PNGIsAnimated) {
        if (Enc != NULL) {
            Enc->PNGIsAnimated = PNGIsAnimated;
        } else {
            Log(Log_ERROR, __func__, U8("Pointer to EncodePNG is NULL"));
        }
    }

    void ComposeAPNGNumFrames(EncodePNG *Enc, const uint32_t NumFrames) {
        if (Enc != NULL) {
            Enc->acTL->NumFrames = NumFrames;
        } else {
            Log(Log_ERROR, __func__, U8("Pointer to EncodePNG is NULL"));
        }
    }

    void ComposeAPNGTimes2Loop(EncodePNG *Enc, const uint32_t NumTimes2Loop) {
        if (Enc != NULL) {
            Enc->acTL->TimesToLoop = NumTimes2Loop;
        } else {
            Log(Log_ERROR, __func__, U8("Pointer to EncodePNG is NULL"));
        }
    }

    void ComposeAPNGFrameDelay(EncodePNG *Enc, const uint32_t FrameDelayNumerator, const uint32_t FrameDelayDenominator) {
        if (Enc == NULL) {
            Log(Log_ERROR, __func__, U8("Pointer to EncodePNG is NULL"));
        } else {
            Enc->fcTL->FrameDelayNumerator   = FrameDelayNumerator;
            Enc->fcTL->FrameDelayDenominator = FrameDelayDenominator;
        }
    }

    void PNGSetIHDR(EncodePNG *Enc, const uint32_t Height, const uint32_t Width, const uint8_t BitDepth, PNGColorTypes ColorType, const bool Interlace) {
        if (Enc != NULL && Height > 0 && Width > 0 && (BitDepth > 0 || BitDepth > 16) && (ColorType <= 6 && ColorType != 1 && ColorType != 5) && Interlace >= 0 && Interlace <= 1) {
            Enc->iHDR->Height       = Height;
            Enc->iHDR->Width        = Width;
            Enc->iHDR->BitDepth     = BitDepth;
            Enc->iHDR->ColorType    = ColorType;
            Enc->iHDR->IsInterlaced = Interlace;
        } else if (Enc == NULL) {
            Log(Log_ERROR, __func__, U8("EncodePNG Pointer is NULL"));
        } else if (Height == 0) {
            Log(Log_ERROR, __func__, U8("Height is 0, which is invalid"));
        } else if (Width == 0) {
            Log(Log_ERROR, __func__, U8("Width is 0, which is invalid"));
        } else if (BitDepth == 0 || BitDepth > 16) {
            Log(Log_ERROR, __func__, U8("BitDepth %d is invalid, valid values range from 1-16"), BitDepth);
        } else if (ColorType > 6 || ColorType == 1 || ColorType == 5) {
            Log(Log_ERROR, __func__, U8("ColorType %d is invalid, valid values range from 0-6, excluding 1 and 5"), ColorType);
        } else if (Interlace > 1) {
            Log(Log_ERROR, __func__, U8("Interlace %d is invalid, valid values range from 0-1"), Interlace);
        }
    }

    void PNGSetSTER(EncodePNG *Enc, const bool StereoType) {
        if (Enc != NULL && StereoType >= 0 && StereoType <= 1) {
            Enc->sTER->StereoType = StereoType;
        } else if (Enc == NULL) {
            Log(Log_ERROR, __func__, U8("Pointer to EncodePNG is NULL"));
        } else if (StereoType < 0 || StereoType > 1) {
            Log(Log_ERROR, __func__, U8("StereoType %d is invalid, valid values range from 0-1"));
        }
    }

    void PNGSetACTL(EncodePNG *Enc, const uint32_t NumFrames, const uint32_t Times2Loop) {
        if (Enc != NULL && NumFrames > 0) {
            Enc->acTL->NumFrames   = NumFrames;
            Enc->acTL->TimesToLoop = Times2Loop;
        } else if (Enc == NULL) {
            Log(Log_ERROR, __func__, U8("Pointer to EncodePNG is NULL"));
        } else if (NumFrames == 0) {
            Log(Log_ERROR, __func__, U8("NumFrames is 0, that isn't possible..."));
        }
    }

    void PNGSetFCTL(EncodePNG *Enc, const uint32_t FrameNum, const uint32_t FrameWidth, const uint32_t FrameHeight, uint32_t XOffset, uint32_t YOffset, uint16_t FrameDelayNumerator, uint16_t FrameDelayDenominator, uint8_t DisposalType, uint8_t BlendType) {

    }

    void WriteIHDRChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, 13);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, iHDRMarker);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, Enc->iHDR->Width);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, Enc->iHDR->Height);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 8,  Enc->iHDR->BitDepth);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 8,  Enc->iHDR->ColorType);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 8,  Enc->iHDR->Compression);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 8,  Enc->iHDR->FilterMethod);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 8,  Enc->iHDR->IsInterlaced);
    }

    void WriteACTLChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, 8);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, acTLMarker);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, Enc->acTL->NumFrames);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, Enc->acTL->TimesToLoop);
    }

    void WriteFCTLChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, 29);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, fcTLMarker);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, Enc->fcTL->FrameNum);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, Enc->fcTL->Width);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, Enc->fcTL->Height);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, Enc->fcTL->XOffset);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, Enc->fcTL->YOffset);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 16, Enc->fcTL->FrameDelayNumerator);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 16, Enc->fcTL->FrameDelayDenominator);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 8,  Enc->fcTL->DisposeMethod);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 8,  Enc->fcTL->BlendMethod);
    }

    void WriteFDATChunk(EncodePNG *Enc, BitBuffer *OutputPNG, uint8_t *DeflatedFrameData, uint32_t DeflatedFrameDataSize) {
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, DeflatedFrameData + 8);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, fDATMarker);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, Enc->fdAT->FrameNum);
    }

    void WriteSTERChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, 1);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, sTERMarker);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 8,  Enc->sTER->StereoType);
    }

    void WriteBKGDChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        uint8_t  NumChannels   = ModernPNGChannelsPerColorType[Enc->iHDR->ColorType];
        uint32_t Size          = 0;
        uint8_t  BKGDEntrySize = 0; // in bits

        if (Enc->iHDR->ColorType == PNG_PalettedRGB) {
            Size          = 1;
            BKGDEntrySize = Bytes2Bits(1);
        } else if (Enc->iHDR->ColorType == PNG_Grayscale || Enc->iHDR->ColorType == PNG_GrayAlpha) {
            Size          = 2;
            BKGDEntrySize = Bytes2Bits(2);
        } else if (Enc->iHDR->ColorType == PNG_RGB || Enc->iHDR->ColorType == PNG_RGBA) {
            Size          = NumChannels * 2;
            BKGDEntrySize = Bytes2Bits(NumChannels * 2);
        }

        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, Size);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, bKGDMarker);

        if (Enc->iHDR->ColorType == PNG_PalettedRGB || Enc->iHDR->ColorType == PNG_Grayscale || Enc->iHDR->ColorType == PNG_GrayAlpha) {
            for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                WriteBits(MSByteFirst, LSBitFirst, OutputPNG, BKGDEntrySize, Enc->bkGD->BackgroundPaletteEntry[Channel]);
            }
        } else if (Enc->iHDR->ColorType == PNG_RGB || Enc->iHDR->ColorType == PNG_RGBA) {
            for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                WriteBits(MSByteFirst, LSBitFirst, OutputPNG, NumChannels * 16, Enc->bkGD->BackgroundPaletteEntry[Channel]);
            }
        }
    }

    void WriteCHRMChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, 32);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, cHRMMarker);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, Enc->cHRM->WhitePointX);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, Enc->cHRM->WhitePointY);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, Enc->cHRM->RedX);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, Enc->cHRM->RedY);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, Enc->cHRM->GreenX);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, Enc->cHRM->GreenY);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, Enc->cHRM->BlueX);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, Enc->cHRM->BlueY);
    }

    void WriteGAMAChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, 4);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, gAMAMarker);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, Enc->gAMA->Gamma);
    }

    void WriteOFFSChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, 9);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, oFFsMarker);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, Enc->oFFs->XOffset);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, Enc->oFFs->YOffset);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 8,  Enc->oFFs->UnitSpecifier);
    }

    void WriteICCPChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        uint32_t ProfileNameSize       = strlen(Enc->iCCP->ProfileName);
        uint32_t CompressedProfileSize = strlen(Enc->iCCP->CompressedICCPProfile);

        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, 8 + CompressedProfileSize + ProfileNameSize);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, iCCPMarker);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, Bytes2Bits(ProfileNameSize), Enc->iCCP->ProfileName);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 8, Enc->iCCP->CompressionType);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, Bytes2Bits(CompressedProfileSize), Enc->iCCP->CompressedICCPProfile);
    }

    void WriteSBITChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        uint8_t ChunkSize = 0;
        uint8_t NumChannels = ModernPNGChannelsPerColorType[Enc->iHDR->ColorType];
        if (Enc->iHDR->ColorType == PNG_Grayscale) {
            ChunkSize = 1;
        } else if (Enc->iHDR->ColorType == PNG_RGB || Enc->iHDR->ColorType == PNG_PalettedRGB) {
            ChunkSize = 3;
        } else if (Enc->iHDR->ColorType == PNG_GrayAlpha) {
            ChunkSize = 2;
        } else if (Enc->iHDR->ColorType == PNG_RGBA) {
            ChunkSize = 4;
        }

        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, ChunkSize);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, sBITMarker);

        if (Enc->iHDR->ColorType == PNG_RGB || Enc->iHDR->ColorType == PNG_PalettedRGB) {
            WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 8, Enc->sBIT->Red);
            WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 8, Enc->sBIT->Green);
            WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 8, Enc->sBIT->Blue);
        } else if (Enc->iHDR->ColorType == PNG_RGBA) {
            WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 8, Enc->sBIT->Red);
            WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 8, Enc->sBIT->Green);
            WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 8, Enc->sBIT->Blue);
            WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 8, Enc->sBIT->Alpha);
        } else if (Enc->iHDR->ColorType == PNG_Grayscale) {
            WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 8, Enc->sBIT->Grayscale);
        } else if (Enc->iHDR->ColorType == PNG_GrayAlpha) {
            WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 8, Enc->sBIT->Grayscale);
            WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 8, Enc->sBIT->Alpha);
        }
    }

    void WriteSRGBChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 8, 1);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, sRGBMarker);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 8,  Enc->sRGB->RenderingIntent);
    }

    void WritePHYSChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 8, 9);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, pHYsMarker);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, Enc->pHYs->PixelsPerUnitXAxis);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, Enc->pHYs->PixelsPerUnitYAxis);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 8,  Enc->pHYs->UnitSpecifier);
    }

    void WritePCALChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        uint32_t ChunkSize = 0;
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, ChunkSize);
        WriteBits(MSByteFirst, LSBitFirst, OutputPNG, 32, pCALMarker);

    }

    void WriteSCALChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        uint32_t ChunkSize = 0;
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
