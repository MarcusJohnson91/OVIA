#include "../../include/Private/PNGCommon.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    /*
     For now, I'm just going to store the image in Deflate as literals.
     
     it will be a valid PNG so for now who cares?
     */
    
    void PNGWriteHeader(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            // Write the iHDR chunk, and then go through the list of other chunks to find out if other chunks should be written
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNGWriteFooter(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            uint32_t iENDSize = 0;
            uint32_t iENDCRC  = 0xAE426082;
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitIO_BitOrder_LSBit, 32, iENDSize);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitIO_BitOrder_LSBit, 32, iENDMarker);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitIO_BitOrder_LSBit, 32, iENDCRC);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
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
            
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 4, CompressionMethod);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 4, CompressionInfo);
            
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 5, FCHECK);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 1, FDICT);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 2, 0); // Huh?
            
            if (FDICT == Yes) {
                BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, PNG->DAT->DictID);
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_Flate_WriteLiteralBlock(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            BitBuffer_Align(BitB, 1);
            // How do we know how many bytes to copy?
            // Let's keep a offset in the PNGOptions struct saying the last written pixel so we can keep track.
            uint16_t Bytes2Copy  = 0;
            uint16_t Bytes2Copy2 = Bytes2Copy ^ 0xFFFF;
            
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitIO_BitOrder_LSBit, 16, Bytes2Copy);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitIO_BitOrder_LSBit, 16, Bytes2Copy2);
            
            // Now we simply copy bytes from the image container to the BitBuffer; the max num bytes is 65536
            
            /*
             Block max size is 65536.
             
             We need to find out the total number of bytes in the ImageContainer by counting the number of Views * the Width * Height * Num Channels * BitDepth
             */
            
            
            
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_Image_Insert(void *Options, void *Container, BitBuffer *BitB) {
        if (Options != NULL && Container != NULL && BitB != NULL) {
            /*
             Loop over the image container, line by line, and try all the filters.
             */
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (Container == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Container Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_Filter_Image(ImageContainer *Image) {
        if (Image != NULL) {
            // Try each filter on each line, get the best by counting the diff between the symbols to decide which to use.
        } else if (Image == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Image Pointer is NULL"));
        }
    }
    
    void WriteIHDRChunk(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG           = Options;
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, 13);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, iHDRMarker);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, PNG->iHDR->Width);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, PNG->iHDR->Height);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 8,  PNG->iHDR->BitDepth);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 8,  PNG->iHDR->ColorType);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 8,  PNG->iHDR->Compression);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 8,  PNG->iHDR->FilterMethod);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 8,  PNG->iHDR->Interlaced);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteACTLChunk(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG           = Options;
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, 8);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, acTLMarker);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, PNG->acTL->NumFrames);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, PNG->acTL->TimesToLoop);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteFCTLChunk(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG           = Options;
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, 29);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, fcTLMarker);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, PNG->fcTL->FrameNum);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, PNG->fcTL->Width);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, PNG->fcTL->Height);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, PNG->fcTL->XOffset);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, PNG->fcTL->YOffset);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 16, PNG->fcTL->FrameDelayNumerator);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 16, PNG->fcTL->FrameDelayDenominator);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 8,  PNG->fcTL->DisposeMethod);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 8,  PNG->fcTL->BlendMethod);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteFDATChunk(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG           = Options;
            // BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, DeflatedFrameData + 8);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, fDATMarker);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, PNG->fcTL->FrameNum);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteSTERChunk(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG           = Options;
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, 1);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, sTERMarker);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 8,  PNG->sTER->StereoType);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteBKGDChunk(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG        = Options;
            uint8_t  ColorType     = PNG->iHDR->ColorType;
            uint8_t  NumChannels   = PNG_NumChannelsPerColorType[ColorType];
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
            
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, Size);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, bKGDMarker);
            
            if (ColorType == PNG_PalettedRGB || ColorType == PNG_Grayscale || ColorType == PNG_GrayAlpha) {
                for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                    BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, BKGDEntrySize, PNG->bkGD->BackgroundPaletteEntry[Channel]);
                }
            } else if (ColorType == PNG_RGB || ColorType == PNG_RGBA) {
                for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                    BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, NumChannels * 16, PNG->bkGD->BackgroundPaletteEntry[Channel]);
                }
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteCHRMChunk(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG           = Options;
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, 32);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, cHRMMarker);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, PNG->cHRM->WhitePointX);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, PNG->cHRM->WhitePointY);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, PNG->cHRM->RedX);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, PNG->cHRM->RedY);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, PNG->cHRM->GreenX);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, PNG->cHRM->GreenY);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, PNG->cHRM->BlueX);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, PNG->cHRM->BlueY);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteGAMAChunk(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG           = Options;
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, 4);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, gAMAMarker);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, PNG->gAMA->Gamma);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteOFFSChunk(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG           = Options;
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, 9);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, oFFsMarker);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, PNG->oFFs->XOffset);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, PNG->oFFs->YOffset);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 8,  PNG->oFFs->UnitSpecifier);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteICCPChunk(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG                 = Options;
            UTF8     *ProfileName           = PNG->iCCP->ProfileName;
            uint64_t  ProfileNameSize       = UTF8_GetStringSizeInCodeUnits(ProfileName);
            uint64_t  CompressedProfileSize = PNG->iCCP->CompressedICCPProfileSize;
            
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, 8 + CompressedProfileSize + ProfileNameSize);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, iCCPMarker);
            BitBuffer_WriteUTF8(BitB, ProfileName, StringTerminator_NULL);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 8, PNG->iCCP->CompressionType);
            for (uint64_t Byte = 0ULL; Byte < PNG->iCCP->CompressedICCPProfileSize; Byte++) {
                BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 8, PNG->iCCP->CompressedICCPProfile[Byte]);
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteSBITChunk(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG          = Options;
            uint8_t ChunkSize        = 0;
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
            
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, ChunkSize);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, sBITMarker);
            
            if (ColorType == PNG_RGB || ColorType == PNG_PalettedRGB) {
                BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 8, PNG->sBIT->Red);
                BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 8, PNG->sBIT->Green);
                BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 8, PNG->sBIT->Blue);
            } else if (ColorType == PNG_RGBA) {
                BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 8, PNG->sBIT->Red);
                BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 8, PNG->sBIT->Green);
                BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 8, PNG->sBIT->Blue);
                BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 8, PNG->sBIT->Alpha);
            } else if (ColorType == PNG_Grayscale) {
                BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 8, PNG->sBIT->Grayscale);
            } else if (ColorType == PNG_GrayAlpha) {
                BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 8, PNG->sBIT->Grayscale);
                BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 8, PNG->sBIT->Alpha);
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteSRGBChunk(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG           = Options;
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 8, 1);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, sRGBMarker);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 8, PNG->sRGB->RenderingIntent);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void WritePHYSChunk(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG           = Options;
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 8, 9);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, pHYsMarker);
            
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, PNG->pHYs->PixelsPerUnitXAxis);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, PNG->pHYs->PixelsPerUnitYAxis);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 8,  PNG->pHYs->UnitSpecifier);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void WritePCALChunk(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG    = Options;
            uint32_t ChunkSize = 0;
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, ChunkSize);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitIO_BitOrder_LSBit, 32, pCALMarker);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void WriteSCALChunk(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG    = Options;
            uint32_t ChunkSize = 0;
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    static const OVIAEncoder PNGEncoder = {
        .EncoderID             = CodecID_PNG,
        .MediaType             = MediaType_Image,
        .Extensions            = &PNGExtensions,
        .Function_Initialize   = PNGOptions_Init,
        .Function_WriteHeader  = PNGWriteHeader,
        .Function_Encode       = PNG_Image_Insert,
        .Function_WriteFooter  = PNGWriteFooter,
        .Function_Deinitialize = PNGOptions_Deinit,
    };
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
