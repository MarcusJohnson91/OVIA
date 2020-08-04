#include "../../include/Private/BMPCommon.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

	/*
	Why don't I go ahead and set the registration functions up so that 



	why can't I set up the system so that it will take X
	*/
    
    void BMPReadMetadata(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            BMPOptions *BMP = Options;
            BitBuffer_Seek(BitB, 16);                           // Skip BMPMagic
            BMP->FileSize                                    = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
            BitBuffer_Seek(BitB, 32);                           // 2 16 bit Reserved fields
            BMP->Offset                                      = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
            if (BMP->Offset > 14) { // DIB Header
                BMP->DIBSize                                 = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
                if (BMP->DIBSize >= 40) {
                    BMP->Width                               = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
                    BMP->Height                              = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
                } else if (BMP->DIBSize == 12) {
                    BMP->Width                               = (uint16_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16);
                    BMP->Height                              = (uint16_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16);
                }
                BitBuffer_Seek(BitB, 16);                       // NumPlanes, always 1
                BMP->BitDepth                                = (uint16_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16);
                BMP->CompressionType                         = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
                BMP->NumBytesUsedBySamples                   = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
                BMP->WidthPixelsPerMeter                     = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
                BMP->HeightPixelsPerMeter                    = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
                BMP->ColorsIndexed                           = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
                BMP->IndexedColorsUsed                       = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
                if (BMP->DIBSize >= 56) {
                    BMP->ColorSpaceType                      = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
                    if (BMP->DIBSize >= 108) {
                        BMP->XCoordinate                     = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
                        BMP->YCoordinate                     = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
                        BMP->ZCoordinate                     = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
                        BMP->RGamma                          = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
                        BMP->GGamma                          = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
                        BMP->BGamma                          = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
                        if (BMP->DIBSize >= 124) {
                            BMP->ICCIntent                   = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
                            BMP->ICCSize                     = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
                            BMP->ICCPayload                  = calloc(BMP->ICCSize, sizeof(uint8_t));
                            if (BMP->ICCPayload != NULL) {
                                for (uint32_t ICCByte = 0; ICCByte < BMP->ICCSize; ICCByte++) {
                                    BMP->ICCPayload[ICCByte] = (uint8_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 8);
                                }
                            } else {
                                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate %d bytes for the ICC payload"), BMP->ICCSize);
                            }
                            BitBuffer_Seek(BitB, 32); // More Reserved data.
                                                      // Ok so when the Height is positive, the image is upside down, the bottom of the image is at the top of the file.
                        }
                    }
                }
                BitBuffer_Seek(BitB, BMP->DIBSize - BitBuffer_GetPosition(BitB)); // Skip the remaining bits.
            } else if (BMP->DIBSize == 40 && (BMP->CompressionType == BMP_BitFields || BMP->CompressionType == BMP_RGBABitFields)) {
                BMP->RMask     = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, BMP->BitDepth);
                BMP->GMask     = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, BMP->BitDepth);
                BMP->BMask     = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, BMP->BitDepth);
                if (BMP->CompressionType == BMP_RGBABitFields) {
                    BMP->AMask = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, BMP->BitDepth);
                }
            } else {
                BitBuffer_Seek(BitB, Bits2Bytes((BMP->Offset - 14) - BMP->DIBSize, RoundingType_Down));
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void *BMPExtractImage(void *Options, BitBuffer *BitB) {
        ImageContainer *Image = NULL;
        if (Options != NULL && BitB != NULL) {
            BMPOptions *BMP          = Options;
            uint16_t BitDepth        = BMP->BitDepth;
            uint32_t Width           = BMP->Width;
            bool     IsUpsideDown    = (BMP->Height & 0x80000000) >> 31;
            uint32_t Height          = (uint32_t) AbsoluteI(BMP->Height);
            
            ImageChannelMap *Map     = ImageContainer_GetChannelMap(Image);
            uint8_t NumViews         = ImageChannelMap_GetNumViews(Map);
            
            Image                    = ImageContainer_Init(BitDepth <= 8 ? ImageType_Integer8 : ImageType_Integer16, 0, Width, Height); // FIXME
            
            if (BMP->CompressionType == BMP_RGB) {
                if (BitDepth <= 8) {
                    uint8_t ****Array  = (uint8_t****) ImageContainer_GetArray(Image);
                    for (uint64_t View = 0ULL; View < NumViews; View++) {
                        for (uint64_t W = 0ULL; W < Width; W++) {
                            for (uint64_t H = 0ULL; H < Height; H++) {
                                for (uint64_t Channel = 0ULL; Channel < 3; Channel++) {
                                    Array[View][W][H][Channel] = (uint8_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, Bits2Bytes(BitDepth, RoundingType_Up));
                                }
                            }
                        }
                    }
                } else if (BMP->BitDepth > 8 && BMP->BitDepth <= 16) {
                    uint16_t ****Array = (uint16_t****) ImageContainer_GetArray(Image);
                    for (uint64_t View = 0ULL; View < NumViews; View++) {
                        for (uint64_t W = 0ULL; W < Width; W++) {
                            for (uint64_t H = 0ULL; H < Height; H++) {
                                for (uint64_t Channel = 0ULL; Channel < 3; Channel++) {
                                    Array[View][W][H][Channel] = (uint8_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, Bits2Bytes(BitDepth, RoundingType_Up));
                                }
                            }
                        }
                    }
                } else if (BMP->CompressionType == BMP_RLE_8Bit || BMP->CompressionType == BMP_RLE_4Bit) {
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("We don't support BMP's encoded with RLE"));
                } else if (BMP->CompressionType == BMP_BitFields || BMP->CompressionType == BMP_RGBABitFields) {
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("We don't support BMP's encoded with BitFields"));
                } else if (BMP->CompressionType == BMP_JPEG) {
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BMP Compression Type is JPEG, there's nothing we can do since JPEG is a lossy format"));
                } else if (BMP->CompressionType == BMP_PNG) {
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BMP Compression Type is PNG, Extract it to a plain PNG and re-run"));
                } else if (BMP->CompressionType == BMP_CMYK || BMP->CompressionType == BMP_CMYK_RLE_8Bit || BMP->CompressionType == BMP_CMYK_RLE_4Bit) {
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CMYK Images are unsupported"));
                }
            }
            if (IsUpsideDown) { // The Image is upside down, so we need to flip it
                ImageContainer_Flip(Image, FlipType_Vertical);
            }
            if (BMP->ColorsIndexed > 0) {
                // The image is palettized, so we need to go ahead and map the pixel bits to the actual colors.
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Palettized BMP's are currently unsupported"));
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BMPOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitB Pointer is NULL"));
        } else if (Image == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        }
        return Image;
    }
    
    static const OVIADecoder BMPDecoder = {
        .Function_Initialize   = BMPOptions_Init,
        .Function_Decode       = BMPExtractImage,
        .Function_Read         = BMPReadMetadata,
        .Function_Deinitialize = BMPOptions_Deinit,
        .MagicIDs              = &BMPMagicIDs,
        .MediaType             = MediaType_Image,
        .DecoderID             = CodecID_BMP,
    };
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
