#include "../../../include/Private/Image/PNGCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void OVIA_PNG_Image_Insert(ImageContainer *Image, BitBuffer *BitB, bool InterlacePNG, bool OptimizePNG) {
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
    
    void OVIA_PNG_Filter_Image(OVIA *Ovia, ImageContainer *Image) {
        if (Ovia != NULL && Image != NULL) {
            // Try each filter on each line, get the best by counting the diff between the symbols to decide which to use.
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (Image == NULL) {
            Log(Log_ERROR, __func__, U8("Image Pointer is NULL"));
        }
    }
    
    void WriteIHDRChunk(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, 13);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, iHDRMarker);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, OVIA_GetWidth(Ovia));
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, OVIA_GetHeight(Ovia));
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 8,  OVIA_GetBitDepth(Ovia));
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 8,  OVIA_PNG_GetColorType(Ovia));
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 8,  OVIA_PNG_GetCompression(Ovia));
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 8,  OVIA_PNG_Filter_GetMethod(Ovia));
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 8,  OVIA_PNG_GetInterlaceStatus(Ovia));
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteACTLChunk(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, 8);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, acTLMarker);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, OVIA_PNG_ACTL_GetNumFrames(Ovia));
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, OVIA_PNG_ACTL_GetTimes2Loop(Ovia));
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteFCTLChunk(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, 29);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, fcTLMarker);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, OVIA_PNG_FCTL_GetFrameNum(Ovia));
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, OVIA_PNG_FCTL_GetWidth(Ovia));
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, OVIA_PNG_FCTL_GetHeight(Ovia));
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, OVIA_PNG_FCTL_GetXOffset(Ovia));
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, OVIA_PNG_FCTL_GetYOffset(Ovia));
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 16, OVIA_PNG_FCTL_GetFrameDelayNumerator(Ovia));
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 16, OVIA_PNG_FCTL_GetFrameDelayDenominator(Ovia));
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 8,  OVIA_PNG_FCTL_GetDisposeMethod(Ovia));
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 8,  OVIA_PNG_FCTL_GetBlendMethod(Ovia));
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteFDATChunk(OVIA *Ovia, BitBuffer *BitB, uint8_t *DeflatedFrameData, uint32_t DeflatedFrameDataSize) {
        if (Ovia != NULL && BitB != NULL) {
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, DeflatedFrameData + 8);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, fDATMarker);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, OVIA_PNG_FCTL_GetFrameNum(Ovia));
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteSTERChunk(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, 1);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, sTERMarker);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 8,  OVIA_PNG_STER_GetSterType(Ovia));
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteBKGDChunk(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            uint8_t  ColorType     = OVIA_PNG_iHDR_GetColorType(Ovia);
            uint8_t  NumChannels   = OVIA_GetNumChannels(Ovia);
            uint32_t Size          = 0;
            uint8_t  BKGDEntrySize = 0; // in bits
            
            if (ColorType == PNG_PalettedRGB) {
                Size          = 1;
                BKGDEntrySize = Bytes2Bits(1);
            } else if (ColorType == PNG_Grayscale || ColorType == PNG_GrayAlpha) {
                Size          = 2;
                BKGDEntrySize = Bytes2Bits(2);
            } else if (ColorType == PNG_RGB || ColorType == PNG_RGBA) {
                Size          = NumChannels * 2;
                BKGDEntrySize = Bytes2Bits(NumChannels * 2);
            }
            
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, Size);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, bKGDMarker);
            
            if (ColorType == PNG_PalettedRGB || ColorType == PNG_Grayscale || ColorType == PNG_GrayAlpha) {
                for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                    BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, BKGDEntrySize, OVIA_PNG_BKGD_GetBackgroundPaletteEntry(Ovia));
                }
            } else if (ColorType == PNG_RGB || ColorType == PNG_RGBA) {
                for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                    BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, NumChannels * 16, OVIA_PNG_BKGD_GetBackgroundPaletteEntry(Ovia));
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
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, 32);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, cHRMMarker);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, OVIA_PNG_CHRM_GetWhitePointX(Ovia));
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, OVIA_PNG_CHRM_GetWhitePointY(Ovia));
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, OVIA_PNG_CHRM_GetRedX(Ovia));
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, OVIA_PNG_CHRM_GetRedY(Ovia));
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, OVIA_PNG_CHRM_GetGreenX(Ovia));
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, OVIA_PNG_CHRM_GetGreenY(Ovia));
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, OVIA_PNG_CHRM_GetBlueX(Ovia));
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, OVIA_PNG_CHRM_GetBlueY(Ovia));
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteGAMAChunk(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, 4);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, gAMAMarker);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, OVIA_PNG_GetGamma(Ovia));
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteOFFSChunk(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, 9);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, oFFsMarker);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, OVIA_PNG_OFFS_GetXOffset(Ovia));
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, OVIA_PNG_OFFS_GetYOffset(Ovia));
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 8,  OVIA_PNG_OFFS_GetSpecifier(Ovia));
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteICCPChunk(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            UTF8     *ProfileName           = OVIA_PNG_ICCP_GetProfileName(Ovia);
            uint64_t  ProfileNameSize       = UTF8_GetStringSizeInCodeUnits(ProfileName);
            uint64_t  CompressedProfileSize = OVIA_PNG_ICCP_GetProfileDataSize(Ovia);
            
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, 8 + CompressedProfileSize + ProfileNameSize);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, iCCPMarker);
            BitBuffer_WriteUTF8(BitB, ProfileName);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 8, OVIA_PNG_ICCP_GetCompressionType(Ovia));
            for (uint64_t Byte = 0ULL; Byte < OVIA_PNG_ICCP_GetProfileDataSize(Ovia); Byte++) {
                BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 8, OVIA_PNG_ICCP_GetProfileData(Ovia));
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteSBITChunk(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            uint8_t ChunkSize = 0;
            OVIA_PNG_ColorTypes ColorType = OVIA_PNG_IHDR_GetColorType(Ovia);
            if (ColorType == PNG_Grayscale) {
                ChunkSize = 1;
            } else if (ColorType == PNG_RGB || ColorType == PNG_PalettedRGB) {
                ChunkSize = 3;
            } else if (ColorType == PNG_GrayAlpha) {
                ChunkSize = 2;
            } else if (ColorType == PNG_RGBA) {
                ChunkSize = 4;
            }
            
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, ChunkSize);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, sBITMarker);
            
            if (ColorType == PNG_RGB || ColorType == PNG_PalettedRGB) {
                BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 8, OVIA_PNG_SBIT_GetRed(Ovia));
                BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 8, OVIA_PNG_SBIT_GetGreen(Ovia));
                BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 8, OVIA_PNG_SBIT_GetBlue(Ovia));
            } else if (ColorType == PNG_RGBA) {
                BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 8, OVIA_PNG_SBIT_GetRed(Ovia));
                BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 8, OVIA_PNG_SBIT_GetGreen(Ovia));
                BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 8, OVIA_PNG_SBIT_GetBlue(Ovia));
                BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 8, OVIA_PNG_SBIT_GetAlpha(Ovia));
            } else if (ColorType == PNG_Grayscale) {
                BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 8, OVIA_PNG_SBIT_GetGray(Ovia));
            } else if (ColorType == PNG_GrayAlpha) {
                BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 8, OVIA_PNG_SBIT_GetGray(Ovia));
                BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 8, OVIA_PNG_SBIT_GetAlpha(Ovia));
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteSRGBChunk(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 8, 1);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, sRGBMarker);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 8, OVIA_PNG_SRGB_GetRenderingIntent(Ovia));
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WritePHYSChunk(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 8, 9);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, pHYsMarker);
            
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, OVIA_PNG_PHYS_GetPixelsPerUnitX(Ovia));
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, OVIA_PNG_PHYS_GetPixelsPerUnitY(Ovia));
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 8,  OVIA_PNG_PHYS_GetUnitSpecifier(Ovia));
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WritePCALChunk(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            uint32_t ChunkSize = 0;
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, ChunkSize);
            BitBuffer_WriteBits(MSByteFirst, LSBitFirst, BitB, 32, pCALMarker);
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
    
#ifdef __cplusplus
}
#endif
