#include "../../include/Private/PNGCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void PNG_DAT_WriteZlibHeader(PNGOptions *PNG, BitBuffer *BitB) {
        if (PNG != NULL && BitB != NULL) {
            bool FDICT                = PNG->DAT->FLG & 0x4 >> 2;
            
            uint8_t CompressionInfo   = PNG->DAT->CMF & 0xF0 >> 4;
            uint8_t CompressionMethod = PNG->DAT->CMF & 0x0F;
            
            uint16_t FCHECK           = CompressionInfo << 12;
            FCHECK                   |= CompressionMethod << 8;
            FCHECK                   |= (PNG->DAT->FLG & 0x3) << 6;
            FCHECK                   |= FDICT << 5;
            
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 4, CompressionMethod);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 4, CompressionInfo);
            
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 5, FCHECK);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 1, FDICT);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 2, PNG->DAT->FLG & 0x3);
            
            if (FDICT == Yes) {
                BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, PNG->DAT->DictID);
            }
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_Flate_WriteLiteralBlock(PNGOptions *PNG, BitBuffer *BitB) {
        if (PNG != NULL && BitB != NULL) {
            BitBuffer_Align(BitB, 1);
            // How do we know how many bytes to copy?
            uint16_t Bytes2Copy  = 0;
            uint16_t Bytes2Copy2 = Bytes2Copy ^ 0xFFFF;
            
            BitBuffer_WriteBits(BitB, LSByteFirst, LSBitFirst, 16, Bytes2Copy);
            BitBuffer_WriteBits(BitB, LSByteFirst, LSBitFirst, 16, Bytes2Copy2);
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_Image_Insert(ImageContainer *Image, BitBuffer *BitB, bool OptimizePNG) {
        if (Image != NULL && BitB != NULL) {
            /*
             Loop over the image container, line by line, and try all the filters.
             */
        } else if (Image == NULL) {
            Log(Log_DEBUG, __func__, U8("ImageContainer Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_Filter_Image(ImageContainer *Image) {
        if (Image != NULL) {
            // Try each filter on each line, get the best by counting the diff between the symbols to decide which to use.
        } else if (Image == NULL) {
            Log(Log_DEBUG, __func__, U8("Image Pointer is NULL"));
        }
    }
    
    void WriteIHDRChunk(PNGOptions *PNG, BitBuffer *BitB) {
        if (PNG != NULL && BitB != NULL) {
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, 13);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, iHDRMarker);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, PNG->iHDR->Width);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, PNG->iHDR->Height);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8,  PNG->iHDR->BitDepth);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8,  PNG->iHDR->ColorType);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8,  PNG->iHDR->Compression);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8,  PNG->iHDR->FilterMethod);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8,  PNG->iHDR->Interlaced);
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteACTLChunk(PNGOptions *PNG, BitBuffer *BitB) {
        if (PNG != NULL && BitB != NULL) {
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, 8);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, acTLMarker);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, PNG->acTL->NumFrames);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, PNG->acTL->TimesToLoop);
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteFCTLChunk(PNGOptions *PNG, BitBuffer *BitB) {
        if (PNG != NULL && BitB != NULL) {
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, 29);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, fcTLMarker);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, PNG->fcTL->FrameNum);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, PNG->fcTL->Width);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, PNG->fcTL->Height);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, PNG->fcTL->XOffset);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, PNG->fcTL->YOffset);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 16, PNG->fcTL->FrameDelayNumerator);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 16, PNG->fcTL->FrameDelayDenominator);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8,  PNG->fcTL->DisposeMethod);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8,  PNG->fcTL->BlendMethod);
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteFDATChunk(PNGOptions *PNG, BitBuffer *BitB, uint8_t *DeflatedFrameData, uint32_t DeflatedFrameDataSize) {
        if (PNG != NULL && BitB != NULL) {
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, DeflatedFrameData + 8);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, fDATMarker);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, PNG->fcTL->FrameNum);
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteSTERChunk(PNGOptions *PNG, BitBuffer *BitB) {
        if (PNG != NULL && BitB != NULL) {
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, 1);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, sTERMarker);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8,  PNG->sTER->StereoType);
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteBKGDChunk(PNGOptions *PNG, BitBuffer *BitB) {
        if (PNG != NULL && BitB != NULL) {
            uint8_t  ColorType     = PNG->iHDR->ColorType;
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
            
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, Size);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, bKGDMarker);
            
            if (ColorType == PNG_PalettedRGB || ColorType == PNG_Grayscale || ColorType == PNG_GrayAlpha) {
                for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                    BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, BKGDEntrySize, PNG->bkGD->BackgroundPaletteEntry[Channel]);
                }
            } else if (ColorType == PNG_RGB || ColorType == PNG_RGBA) {
                for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                    BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, NumChannels * 16, PNG->bkGD->BackgroundPaletteEntry[Channel]);
                }
            }
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteCHRMChunk(PNGOptions *PNG, BitBuffer *BitB) {
        if (PNG != NULL && BitB != NULL) {
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, 32);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, cHRMMarker);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, PNG->cHRM->WhitePointX);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, PNG->cHRM->WhitePointY);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, PNG->cHRM->RedX);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, PNG->cHRM->RedY);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, PNG->cHRM->GreenX);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, PNG->cHRM->GreenY);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, PNG->cHRM->BlueX);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, PNG->cHRM->BlueY);
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteGAMAChunk(PNGOptions *PNG, BitBuffer *BitB) {
        if (PNG != NULL && BitB != NULL) {
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, 4);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, gAMAMarker);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, PNG->gAMA->Gamma);
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteOFFSChunk(PNGOptions *PNG, BitBuffer *BitB) {
        if (PNG != NULL && BitB != NULL) {
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, 9);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, oFFsMarker);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, PNG->oFFs->XOffset);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, PNG->oFFs->YOffset);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8,  PNG->oFFs->UnitSpecifier);
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteICCPChunk(PNGOptions *PNG, BitBuffer *BitB) {
        if (PNG != NULL && BitB != NULL) {
            UTF8     *ProfileName           = PNG->iCCP->ProfileName;
            uint64_t  ProfileNameSize       = UTF8_GetStringSizeInCodeUnits(ProfileName);
            uint64_t  CompressedProfileSize = PNG->iCCP->CompressedICCPProfileSize;
            
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, 8 + CompressedProfileSize + ProfileNameSize);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, iCCPMarker);
            BitBuffer_WriteUTF8(BitB, ProfileName);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, PNG->iCCP->CompressionType);
            for (uint64_t Byte = 0ULL; Byte < PNG->iCCP->CompressedICCPProfileSize; Byte++) {
                BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, PNG->iCCP->CompressedICCPProfile[Byte]);
            }
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteSBITChunk(PNGOptions *PNG, BitBuffer *BitB) {
        if (PNG != NULL && BitB != NULL) {
            uint8_t ChunkSize = 0;
            PNG_ColorTypes ColorType = PNG->iHDR->ColorType;
            if (ColorType == PNG_Grayscale) {
                ChunkSize = 1;
            } else if (ColorType == PNG_RGB || ColorType == PNG_PalettedRGB) {
                ChunkSize = 3;
            } else if (ColorType == PNG_GrayAlpha) {
                ChunkSize = 2;
            } else if (ColorType == PNG_RGBA) {
                ChunkSize = 4;
            }
            
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, ChunkSize);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, sBITMarker);
            
            if (ColorType == PNG_RGB || ColorType == PNG_PalettedRGB) {
                BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, PNG->sBIT->Red);
                BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, PNG->sBIT->Green);
                BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, PNG->sBIT->Blue);
            } else if (ColorType == PNG_RGBA) {
                BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, PNG->sBIT->Red);
                BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, PNG->sBIT->Green);
                BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, PNG->sBIT->Blue);
                BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, PNG->sBIT->Alpha);
            } else if (ColorType == PNG_Grayscale) {
                BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, PNG->sBIT->Grayscale);
            } else if (ColorType == PNG_GrayAlpha) {
                BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, PNG->sBIT->Grayscale);
                BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, PNG->sBIT->Alpha);
            }
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteSRGBChunk(PNGOptions *PNG, BitBuffer *BitB) {
        if (PNG != NULL && BitB != NULL) {
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, 1);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, sRGBMarker);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, PNG->sRGB->RenderingIntent);
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WritePHYSChunk(PNGOptions *PNG, BitBuffer *BitB) {
        if (PNG != NULL && BitB != NULL) {
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, 9);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, pHYsMarker);
            
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, PNG->pHYs->PixelsPerUnitXAxis);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, PNG->pHYs->PixelsPerUnitYAxis);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8,  PNG->pHYs->UnitSpecifier);
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WritePCALChunk(PNGOptions *PNG, BitBuffer *BitB) {
        if (PNG != NULL && BitB != NULL) {
            uint32_t ChunkSize = 0;
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, ChunkSize);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, pCALMarker);
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteSCALChunk(PNGOptions *PNG, BitBuffer *BitB) {
        if (PNG != NULL && BitB != NULL) {
            uint32_t ChunkSize = 0;
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
#ifdef __cplusplus
}
#endif
