#include "../../../include/Private/Image/PNGCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    EncodePNG *EncodePNG_Init(void) {
        EncodePNG *Enc  = calloc(1, sizeof(EncodePNG));
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
            Log(Log_ERROR, __func__, U8("Failed to allocate enough memory for EncodePNG"));
        }
        return Enc;
    }
    
    void EncodePNG_Deinit(EncodePNG *Enc) {
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
    
    BitBuffer *EncodePNGImage(EncodePNG *Enc, void ****RawImage2Encode, bool InterlacePNG, bool OptimizePNG) {
        // RawImage2Encode is a void pointer, the contents of Enc->iHDR->BitDepth will define the data type, uint8_t or uint16_t
        
        // This is THE main function for encoding a buffer into a PNG file.
        return NULL;
    }
    
    BitBuffer *EncodeAdam7(EncodePNG *Enc, BitBuffer *ProgressiveImage) { // Returns the interlaced image
        /*
         Ok, so to interlace an image, we need to loop over every pixel in an 8x8 block?
         */
        BitBuffer *InterlacedImage = NULL;
        return InterlacedImage;
    }
    
    void PNGEncodeFilterNone(EncodePNG *Enc, uint8_t *DeEntropyedData, uint8_t *DeFilteredData, size_t Line) {
        
    }
    
    void PNGEncodeFilterPaeth(EncodePNG *Enc, uint8_t *Line, size_t LineSize) {
        // RawData is after decoding the I/f DATs, and after INFLAT'ing and De-LZ77'ing it.
        // Each line is preceded by a filter type byte, so decode it by a line by line basis.
        // Good candidate for multi-threading.
    }
    
    void PNGEncodeFilterSub(EncodePNG *Enc, uint8_t *Line, size_t NumPixels) {
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
    
    void ComposePNGCHRMWhitePoint() {
        
    }
    
#ifdef __cplusplus
}
#endif
