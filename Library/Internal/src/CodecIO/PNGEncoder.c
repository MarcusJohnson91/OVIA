#include "../../../../Dependencies/FoundationIO/libFoundationIO/include/Macros.h"
#include "../../../include/Private/Image/PNGCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void EncodePNGImage(ImageContainer *Image, BitBuffer *BitB, bool InterlacePNG, bool OptimizePNG) {
        if (Image != NULL && BitB != NULL) {
            /*
             Loop over the image container, line by line, and try all the filters.
             */
        } else if (Image == NULL) {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_Encode_Adam7(OVIA *Ovia, ImageContainer *Image) { // Interlaces the ImageContainer, we should set a value somewhere indicating that it's been interlaced.
        if (Ovia != NULL && Image != NULL) {
            // Get the type of the array, and start encoding the image I guess.
            // What if the height or width is not a power of 8?
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (Image == NULL) {
            Log(Log_ERROR, __func__, U8("Image Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_EncodeFilterNone(OVIA *Ovia, ImageContainer *Image) {
        if (Ovia != NULL && Image != NULL) {
            
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNGEncodeFilterPaeth(OVIA *Ovia, ImageContainer *Image) {
        if (Ovia != NULL && Image != NULL) {
            
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
        // RawData is after Oviaoding the I/f DATs, and after INFLAT'ing and De-LZ77'ing it.
        // Each line is preceded by a filter type byte, so OVIA it by a line by line basis.
        // Good candidate for multi-threading.
    }
    
    void PNGEncodeFilterSub(OVIA *Ovia, ImageContainer *Image) {
        if (Ovia != NULL && Image != NULL) {
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
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
        
    }
    
    void WriteIHDRChunk(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, 13);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, iHDRMarker);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, OVIA_GetWidth(Ovia));
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, OVIA_GetHeight(Ovia));
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8,  OVIA_GetBitDepth(Ovia));
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8,  OVIA_PNG_GetColorType(Ovia));
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8,  OVIA_PNG_GetCompression(Ovia));
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8,  OVIA_PNG_GetFilterMethod(Ovia));
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8,  OVIA_PNG_GetInterlaceStatus(Ovia));
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteACTLChunk(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, 8);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, acTLMarker);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->acTL->NumFrames);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->acTL->TimesToLoop);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteFCTLChunk(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, 29);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, fcTLMarker);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->fcTL->FrameNum);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->fcTL->Width);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->fcTL->Height);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->fcTL->XOffset);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->fcTL->YOffset);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 16, Enc->fcTL->FrameDelayNumerator);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 16, Enc->fcTL->FrameDelayDenominator);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8,  Enc->fcTL->DisposeMethod);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8,  Enc->fcTL->BlendMethod);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteFDATChunk(OVIA *Ovia, BitBuffer *BitB, uint8_t *DeflatedFrameData, uint32_t DeflatedFrameDataSize) {
        if (Ovia != NULL && BitB != NULL) {
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, DeflatedFrameData + 8);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, fDATMarker);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->fdAT->FrameNum);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteSTERChunk(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, 1);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, sTERMarker);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8,  Enc->sTER->StereoType);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteBKGDChunk(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
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
            
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Size);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, bKGDMarker);
            
            if (Enc->iHDR->ColorType == PNG_PalettedRGB || Enc->iHDR->ColorType == PNG_Grayscale || Enc->iHDR->ColorType == PNG_GrayAlpha) {
                for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                    BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, BKGDEntrySize, Enc->bkGD->BackgroundPaletteEntry[Channel]);
                }
            } else if (Enc->iHDR->ColorType == PNG_RGB || Enc->iHDR->ColorType == PNG_RGBA) {
                for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                    BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, NumChannels * 16, Enc->bkGD->BackgroundPaletteEntry[Channel]);
                }
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteCHRMChunk(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, 32);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, cHRMMarker);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->cHRM->WhitePointX);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->cHRM->WhitePointY);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->cHRM->RedX);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->cHRM->RedY);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->cHRM->GreenX);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->cHRM->GreenY);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->cHRM->BlueX);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->cHRM->BlueY);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteGAMAChunk(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, 4);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, gAMAMarker);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->gAMA->Gamma);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteOFFSChunk(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, 9);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, oFFsMarker);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->oFFs->XOffset);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->oFFs->YOffset);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8,  Enc->oFFs->UnitSpecifier);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteICCPChunk(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            uint32_t ProfileNameSize       = strlen(Enc->iCCP->ProfileName);
            uint32_t CompressedProfileSize = strlen(Enc->iCCP->CompressedICCPProfile);
            
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, 8 + CompressedProfileSize + ProfileNameSize);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, iCCPMarker);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, Bytes2Bits(ProfileNameSize), Enc->iCCP->ProfileName);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8, Enc->iCCP->CompressionType);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, Bytes2Bits(CompressedProfileSize), Enc->iCCP->CompressedICCPProfile);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteSBITChunk(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
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
            
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, ChunkSize);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, sBITMarker);
            
            if (Enc->iHDR->ColorType == PNG_RGB || Enc->iHDR->ColorType == PNG_PalettedRGB) {
                BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8, Enc->sBIT->Red);
                BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8, Enc->sBIT->Green);
                BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8, Enc->sBIT->Blue);
            } else if (Enc->iHDR->ColorType == PNG_RGBA) {
                BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8, Enc->sBIT->Red);
                BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8, Enc->sBIT->Green);
                BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8, Enc->sBIT->Blue);
                BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8, Enc->sBIT->Alpha);
            } else if (Enc->iHDR->ColorType == PNG_Grayscale) {
                BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8, Enc->sBIT->Grayscale);
            } else if (Enc->iHDR->ColorType == PNG_GrayAlpha) {
                BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8, Enc->sBIT->Grayscale);
                BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8, Enc->sBIT->Alpha);
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteSRGBChunk(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8, 1);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, sRGBMarker);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8,  Enc->sRGB->RenderingIntent);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WritePHYSChunk(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8, 9);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, pHYsMarker);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->pHYs->PixelsPerUnitXAxis);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, Enc->pHYs->PixelsPerUnitYAxis);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 8,  Enc->pHYs->UnitSpecifier);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WritePCALChunk(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            uint32_t ChunkSize = 0;
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, ChunkSize);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitBuffer *BitB, 32, pCALMarker);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteSCALChunk(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            uint32_t ChunkSize = 0;
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void ComposeAPNGNumFrames(OVIA *Ovia, const uint32_t NumFrames) {
        if (Enc != NULL) {
            Enc->acTL->NumFrames = NumFrames;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void ComposeAPNGTimes2Loop(OVIA *Ovia, const uint32_t NumTimes2Loop) {
        if (Enc != NULL) {
            Enc->acTL->TimesToLoop = NumTimes2Loop;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void ComposeAPNGFrameDelay(OVIA *Ovia, const uint32_t FrameDelayNumerator, const uint32_t FrameDelayDenominator) {
        if (Enc == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else {
            Enc->fcTL->FrameDelayNumerator   = FrameDelayNumerator;
            Enc->fcTL->FrameDelayDenominator = FrameDelayDenominator;
        }
    }
    
    void ComposePNGCHRMWhitePoint() {
        if (Ovia != NULL && BitB != NULL) {
            
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNGInsertImage(OVIA *Ovia, BitBuffer *BitB, ImageContainer *Image) {
        if (Ovia != NULL && BitB != NULL) {
            
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
#ifdef __cplusplus
}
#endif
