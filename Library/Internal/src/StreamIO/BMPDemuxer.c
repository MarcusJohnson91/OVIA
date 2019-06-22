#include "../../include/Private/BMPCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    static uint64_t BMPGetRowSize(const uint16_t BitsPerPixel, const uint32_t ImageWidth) {
        return FloorF((BitsPerPixel * ImageWidth + 31) / 32) * 4; // Floor((4 * 1024 + 31) / 32) * 4
    }
    
    static uint64_t BMPGetPixelArraySize(const uint64_t RowSize, const int32_t ImageHeight) { // 5568, 3712, there's 54 extra bytes in the PixelArray...
        return RowSize * AbsoluteI(ImageHeight);
    }
    
    void BMPParseMetadata(BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            BitBuffer_Seek(BitB, 16);                           // Skip BM
            uint64_t DIBSize                                     = 0UL;
            uint32_t BMPCompressionType                          = 0;
            uint64_t FileSize                                    = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
            OVIA_SetFileSize(Ovia, FileSize);
            BitBuffer_Seek(BitB, 32);                           // 2 16 bit Reserved fields
            uint64_t Offset                                      = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
            OVIA_SetSampleOffset(Ovia, Offset);
            if (Offset > 14) { // DIB Header
                DIBSize                                         = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32); // 40
                if (DIBSize >= 40) {
                    uint64_t Width                              = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
                    OVIA_SetWidth(Ovia, Width);
                    int64_t Height                              = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
                    OVIA_SetHeight(Ovia, Height);
                } else if (DIBSize == 12) {
                    uint64_t Width                              = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16);
                    OVIA_SetWidth(Ovia, Width);
                    int64_t Height                              = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16);
                    OVIA_SetHeight(Ovia, Height);
                }
                BitBuffer_Seek(BitB, 16);                       // NumPlanes, always 1
                uint64_t BitDepth                                = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16);
                OVIA_SetBitDepth(Ovia, BitDepth);
                BMPCompressionType                               = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
                OVIA_BMP_SetCompressionType(Ovia, BMPCompressionType);
                OVIA_BMP_SetNumBytesUsedBySamples(Ovia, BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32));
                OVIA_BMP_SetWidthInMeters(Ovia, BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32));
                OVIA_BMP_SetHeightInMeters(Ovia, BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32));
                OVIA_BMP_SetColorsIndexed(Ovia, BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32));
                OVIA_BMP_SetIndexColorsUsed(Ovia, BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32));
                if (DIBSize >= 56) {
                    OVIA_BMP_SetColorSpaceType(Ovia, BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32));
                    if (DIBSize >= 108) {
                        OVIA_BMP_SetXCoordinate(Ovia, BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32));
                        OVIA_BMP_SetYCoordinate(Ovia, BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32));
                        OVIA_BMP_SetZCoordinate(Ovia, BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32));
                        OVIA_BMP_SetRGamma(Ovia, BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32));
                        OVIA_BMP_SetGGamma(Ovia, BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32));
                        OVIA_BMP_SetBGamma(Ovia, BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32));
                        if (DIBSize >= 124) {
                            uint32_t ICCIntent = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
                            uint32_t ICCSize   = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
                            uint8_t *ICCData   = calloc(ICCSize, sizeof(uint8_t));
                            if (ICCData != NULL) {
                                for (uint32_t ICCByte = 0; ICCByte < ICCSize; ICCByte++) {
                                    ICCData[ICCByte] = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 8);
                                }
                            } else {
                                Log(Log_ERROR, __func__, U8("Couldn't allocate %d bytes for the ICC payload"), ICCSize);
                            }
                            OVIA_BMP_SetICC(Ovia, ICCIntent, ICCSize, ICCData);
                            BitBuffer_Seek(BitB, 32); // More Reserved data.
                            // Ok so when the Height is positive, the image is upside down, the bottom of the image is at the top of the file.
                        }
                    }
                }
                BitBuffer_Seek(BitB, DIBSize - BitBuffer_GetPosition(BitB)); // Skip the remaining bits.
            } else if (DIBSize == 40 && (BMPCompressionType == BMP_BitFields || BMPCompressionType == BMP_RGBABitFields)) {
                OVIA_BMP_SetRMask(Ovia, BitBuffer_ReadBits(LSByteFirst, LSBitFirst, Ovia, OVIA_GetBitDepth(Ovia)));
                OVIA_BMP_SetGMask(Ovia, BitBuffer_ReadBits(LSByteFirst, LSBitFirst, Ovia, OVIA_GetBitDepth(Ovia)));
                OVIA_BMP_SetBMask(Ovia, BitBuffer_ReadBits(LSByteFirst, LSBitFirst, Ovia, OVIA_GetBitDepth(Ovia)));
                if (BMPCompressionType == BMP_RGBABitFields) {
                    OVIA_BMP_SetAMask(Ovia, BitBuffer_ReadBits(LSByteFirst, LSBitFirst, Ovia, OVIA_GetBitDepth(Ovia)));
                }
            } else {
                BitBuffer_Seek(BitB, Bits2Bytes((OVIA_GetSampleOffset(Ovia) - 14) - DIBSize, RoundingType_Down));
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    ImageContainer *BMPExtractImage(BitBuffer *BitB) {
        ImageContainer *Image = NULL;
        if (Ovia != NULL && BitB != NULL) {
            // We need to create a ImageContainer, based on the bit depth of the image.
            uint64_t BitDepth       = OVIA_GetBitDepth(Ovia);
            uint64_t NumChannels    = OVIA_GetNumChannels(Ovia);
            uint64_t Width          = OVIA_GetWidth(Ovia);
            int64_t  Height         = OVIA_GetHeight(Ovia);
            uint32_t BMPCompresType = OVIA_BMP_GetCompressionType(Ovia);
            if (BitDepth <= 8) {
                Image = ImageContainer_Init(ImageType_Integer8, ImageMask_Red | ImageMask_Green | ImageMask_Blue, Width, Height);
            } else if (BitDepth > 8 && BitDepth <= 16) {
                Image = ImageContainer_Init(ImageType_Integer16, ImageMask_Red | ImageMask_Green | ImageMask_Blue, Width, Height);
            }
            
            if (OVIA_GetHeight(Ovia) < 0) {
                // Read from the bottom to the top.
                if (OVIA_BMP_GetCompressionType(Ovia) == BMP_RGB) {
                    if (BitDepth <= 8) {
                        uint8_t *Array  = (uint8_t*) ImageContainer_GetArray(Image);
                        for (uint64_t View = 0ULL; View < ImageContainer_GetNumViews(Image); View++) {
                            for (uint64_t W = 0ULL; W < Width; W++) {
                                for (uint64_t H = 0ULL; H < Height; H++) {
                                    for (uint64_t Channel = 0ULL; Channel < NumChannels; Channel++) {
                                        Array[View * W * H * Channel] = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, Bits2Bytes(BitDepth, RoundingType_Up));
                                    }
                                }
                            }
                        }
                    } else if (BitDepth > 8 && BitDepth <= 16) {
                        uint16_t *Array = (uint16_t*) ImageContainer_GetArray(Image);
                        for (uint64_t View = 0ULL; View < ImageContainer_GetNumViews(Image); View++) {
                            for (uint64_t W = 0ULL; W < Width; W++) {
                                for (uint64_t H = 0ULL; H < Height; H++) {
                                    for (uint64_t Channel = 0ULL; Channel < NumChannels; Channel++) {
                                        Array[View * W * H * Channel] = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, Bits2Bytes(BitDepth, RoundingType_Up));
                                    }
                                }
                            }
                        }
                    }
                } else if (OVIA_BMP_GetCompressionType(Ovia) == BMP_RLE_8Bit || OVIA_BMP_GetCompressionType(Ovia) == BMP_RLE_4Bit) {
                    Log(Log_ERROR, __func__, U8("We don't support BMP's encoded with RLE"));
                } else if (OVIA_BMP_GetCompressionType(Ovia) == BMP_BitFields || OVIA_BMP_GetCompressionType(Ovia) == BMP_RGBABitFields) {
                    Log(Log_ERROR, __func__, U8("We don't support BMP's encoded with BitFields"));
                } else if (OVIA_BMP_GetCompressionType(Ovia) == BMP_JPEG) {
                    Log(Log_ERROR, __func__, U8("BMP Compression Type is JPEG, there's nothing we can do since JPEG is a lossy format"));
                } else if (OVIA_BMP_GetCompressionType(Ovia) == BMP_PNG) {
                    Log(Log_ERROR, __func__, U8("BMP Compression Type is PNG, extract it as is or OVIA it to uncompressed BMP"));
                } else if (OVIA_BMP_GetCompressionType(Ovia) == BMP_CMYK || OVIA_BMP_GetCompressionType(Ovia) == BMP_CMYK_RLE_8Bit || OVIA_BMP_GetCompressionType(Ovia) == BMP_CMYK_RLE_4Bit) {
                    Log(Log_ERROR, __func__, U8("CMYK Images are unsupported"));
                }
                if (Height < 0) { // The Image is upside down, so we need to flip it
                    ImageContainer_Flip(Image, true, false);
                }
            }
            if (OVIA_BMP_GetColorsIndexed(Ovia) > 0) {
                // The image is palettized, so we need to go ahead and map the pixel bits to the actual colors.
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitB Pointer is NULL"));
        }
        return Image;
    }
    
#ifdef __cplusplus
}
#endif
