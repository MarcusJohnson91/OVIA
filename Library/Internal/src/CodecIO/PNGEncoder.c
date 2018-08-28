#include "../../../include/Private/Image/PNGCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
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
        // RawData is after Oviaoding the I/f DATs, and after INFLAT'ing and De-LZ77'ing it.
        // Each line is preceded by a filter type byte, so OVIA it by a line by line basis.
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
    
    void WriteIHDRChunk(EncodePNG *Enc, BitBuffer *BitB) {
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, 13);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, iHDRMarker);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->iHDR->Width);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->iHDR->Height);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8,  Enc->iHDR->BitDepth);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8,  Enc->iHDR->ColorType);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8,  Enc->iHDR->Compression);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8,  Enc->iHDR->FilterMethod);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8,  Enc->iHDR->Progressive);
    }
    
    void WriteACTLChunk(EncodePNG *Enc, BitBuffer *BitB) {
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, 8);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, acTLMarker);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->acTL->NumFrames);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->acTL->TimesToLoop);
    }
    
    void WriteFCTLChunk(EncodePNG *Enc, BitBuffer *BitB) {
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, 29);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, fcTLMarker);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->fcTL->FrameNum);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->fcTL->Width);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->fcTL->Height);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->fcTL->XOffset);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->fcTL->YOffset);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 16, Enc->fcTL->FrameDelayNumerator);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 16, Enc->fcTL->FrameDelayDenominator);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8,  Enc->fcTL->DisposeMethod);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8,  Enc->fcTL->BlendMethod);
    }
    
    void WriteFDATChunk(EncodePNG *Enc, BitBuffer *BitB, uint8_t *DeflatedFrameData, uint32_t DeflatedFrameDataSize) {
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, DeflatedFrameData + 8);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, fDATMarker);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->fdAT->FrameNum);
    }
    
    void WriteSTERChunk(EncodePNG *Enc, BitBuffer *BitB) {
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, 1);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, sTERMarker);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8,  Enc->sTER->StereoType);
    }
    
    void WriteBKGDChunk(EncodePNG *Enc, BitBuffer *BitB) {
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
        
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Size);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, bKGDMarker);
        
        if (Enc->iHDR->ColorType == PNG_PalettedRGB || Enc->iHDR->ColorType == PNG_Grayscale || Enc->iHDR->ColorType == PNG_GrayAlpha) {
            for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, BKGDEntrySize, Enc->bkGD->BackgroundPaletteEntry[Channel]);
            }
        } else if (Enc->iHDR->ColorType == PNG_RGB || Enc->iHDR->ColorType == PNG_RGBA) {
            for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, NumChannels * 16, Enc->bkGD->BackgroundPaletteEntry[Channel]);
            }
        }
    }
    
    void WriteCHRMChunk(EncodePNG *Enc, BitBuffer *BitB) {
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, 32);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, cHRMMarker);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->cHRM->WhitePointX);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->cHRM->WhitePointY);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->cHRM->RedX);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->cHRM->RedY);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->cHRM->GreenX);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->cHRM->GreenY);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->cHRM->BlueX);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->cHRM->BlueY);
    }
    
    void WriteGAMAChunk(EncodePNG *Enc, BitBuffer *BitB) {
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, 4);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, gAMAMarker);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->gAMA->Gamma);
    }
    
    void WriteOFFSChunk(EncodePNG *Enc, BitBuffer *BitB) {
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, 9);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, oFFsMarker);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->oFFs->XOffset);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->oFFs->YOffset);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8,  Enc->oFFs->UnitSpecifier);
    }
    
    void WriteICCPChunk(EncodePNG *Enc, BitBuffer *BitB) {
        uint32_t ProfileNameSize       = strlen(Enc->iCCP->ProfileName);
        uint32_t CompressedProfileSize = strlen(Enc->iCCP->CompressedICCPProfile);
        
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, 8 + CompressedProfileSize + ProfileNameSize);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, iCCPMarker);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, Bytes2Bits(ProfileNameSize), Enc->iCCP->ProfileName);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8, Enc->iCCP->CompressionType);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, Bytes2Bits(CompressedProfileSize), Enc->iCCP->CompressedICCPProfile);
    }
    
    void WriteSBITChunk(EncodePNG *Enc, BitBuffer *BitB) {
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
        
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, ChunkSize);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, sBITMarker);
        
        if (Enc->iHDR->ColorType == PNG_RGB || Enc->iHDR->ColorType == PNG_PalettedRGB) {
            WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8, Enc->sBIT->Red);
            WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8, Enc->sBIT->Green);
            WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8, Enc->sBIT->Blue);
        } else if (Enc->iHDR->ColorType == PNG_RGBA) {
            WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8, Enc->sBIT->Red);
            WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8, Enc->sBIT->Green);
            WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8, Enc->sBIT->Blue);
            WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8, Enc->sBIT->Alpha);
        } else if (Enc->iHDR->ColorType == PNG_Grayscale) {
            WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8, Enc->sBIT->Grayscale);
        } else if (Enc->iHDR->ColorType == PNG_GrayAlpha) {
            WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8, Enc->sBIT->Grayscale);
            WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8, Enc->sBIT->Alpha);
        }
    }
    
    void WriteSRGBChunk(EncodePNG *Enc, BitBuffer *BitB) {
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8, 1);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, sRGBMarker);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8,  Enc->sRGB->RenderingIntent);
    }
    
    void WritePHYSChunk(EncodePNG *Enc, BitBuffer *BitB) {
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8, 9);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, pHYsMarker);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->pHYs->PixelsPerUnitXAxis);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->pHYs->PixelsPerUnitYAxis);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8,  Enc->pHYs->UnitSpecifier);
    }
    
    void WritePCALChunk(EncodePNG *Enc, BitBuffer *BitB) {
        uint32_t ChunkSize = 0;
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, ChunkSize);
        WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, pCALMarker);
        
    }
    
    void WriteSCALChunk(EncodePNG *Enc, BitBuffer *BitB) {
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
    
    // Interlace is the wrong word, despite what the spec says, it's not interpolating fields, it's optimizing the image for streaming.
    // The word I'm looking for is "Progressive".
    void OVIA_PNG_SetIHDR(OVIA *Ovia, ImageContainer *Image, const bool Progressive) {
        if (Ovia != NULL && Image != NULL) {
            
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (Image == NULL) {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        
        
        if (Enc != NULL && Height > 0 && Width > 0 && (BitDepth > 0 || BitDepth > 16) && (ColorType <= 6 && ColorType != 1 && ColorType != 5) && Progressive >= 0 && Interlace <= 1) {
            Enc->iHDR->Height       = Height;
            Enc->iHDR->Width        = Width;
            Enc->iHDR->BitDepth     = BitDepth;
            Enc->iHDR->ColorType    = ColorType;
            Enc->iHDR->Progressive  = Progressive;
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
    
    void OVIA_PNG_SetSTER(EncodePNG *Enc, const bool StereoType) {
        if (Enc != NULL && StereoType >= 0 && StereoType <= 1) {
            Enc->sTER->StereoType = StereoType;
        } else if (Enc == NULL) {
            Log(Log_ERROR, __func__, U8("Pointer to EncodePNG is NULL"));
        } else if (StereoType < 0 || StereoType > 1) {
            Log(Log_ERROR, __func__, U8("StereoType %d is invalid, valid values range from 0-1"));
        }
    }
    
    void OVIA_PNG_SetACTL(EncodePNG *Enc, const uint32_t NumFrames, const uint32_t Times2Loop) {
        if (Enc != NULL && NumFrames > 0) {
            Enc->acTL->NumFrames   = NumFrames;
            Enc->acTL->TimesToLoop = Times2Loop;
        } else if (Enc == NULL) {
            Log(Log_ERROR, __func__, U8("Pointer to EncodePNG is NULL"));
        } else if (NumFrames == 0) {
            Log(Log_ERROR, __func__, U8("NumFrames is 0, that isn't possible..."));
        }
    }
    
    void OVIA_PNG_SetFCTL(EncodePNG *Enc, const uint32_t FrameNum, const uint32_t FrameWidth, const uint32_t FrameHeight, uint32_t XOffset, uint32_t YOffset, uint16_t FrameDelayNumerator, uint16_t FrameDelayDenominator, uint8_t DisposalType, uint8_t BlendType) {
        
    }
    
    void ComposePNGCHRMWhitePoint() {
        
    }
    
    void PNGInsertImage(OVIA *Ovia, BitBuffer *BitB, ImageContainer *Image) {
        
    }
    
#ifdef __cplusplus
}
#endif
