#include "../../include/Private/BMPCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void BMPParseMetadata(BMPOptions *BMP, BitBuffer *BitB) {
        if (BMP != NULL && BitB != NULL) {
            BitBuffer_Seek(BitB, 16);                            // Skip BMPMagic
            BMP->FileSize = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
            BitBuffer_Seek(BitB, 32);                           // 2 16 bit Reserved fields
            BMP->Offset                                      = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
            if (BMP->Offset > 14) { // DIB Header
                BMP->DIBSize = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
                if (BMP->DIBSize >= 40) {
                    BMP->Width = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
                    BMP->Height = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
                } else if (BMP->DIBSize == 12) {
                    BMP->Width = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16);
                    BMP->Height = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16);
                }
                BitBuffer_Seek(BitB, 16);                       // NumPlanes, always 1
                BMP->BitDepth                                = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16);
                BMP->CompressionType                               = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
                BMP->NumBytesUsedBySamples       = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
                BMP->WidthPixelsPerMeter         = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
                BMP->HeightPixelsPerMeter        = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
                BMP->ColorsIndexed               = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
                BMP->IndexedColorsUsed           = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
                if (BMP->DIBSize >= 56) {
                    BMP->ColorSpaceType          = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
                    if (BMP->DIBSize >= 108) {
                        BMP->XCoordinate         = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
                        BMP->YCoordinate         = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
                        BMP->ZCoordinate         = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
                        BMP->RGamma              = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
                        BMP->GGamma              = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
                        BMP->BGamma              = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
                        if (BMP->DIBSize >= 124) {
                            BMP->ICCIntent       = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
                            BMP->ICCSize         = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
                            BMP->ICCPayload      = calloc(BMP->ICCSize, sizeof(uint8_t));
                            if (BMP->ICCPayload != NULL) {
                                for (uint32_t ICCByte = 0; ICCByte < BMP->ICCSize; ICCByte++) {
                                    BMP->ICCPayload[ICCByte] = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 8);
                                }
                            } else {
                                Log(Log_DEBUG, __func__, U8("Couldn't allocate %d bytes for the ICC payload"), BMP->ICCSize);
                            }
                            BitBuffer_Seek(BitB, 32); // More Reserved data.
                                                      // Ok so when the Height is positive, the image is upside down, the bottom of the image is at the top of the file.
                        }
                    }
                }
                BitBuffer_Seek(BitB, BMP->DIBSize - BitBuffer_GetPosition(BitB)); // Skip the remaining bits.
            } else if (BMP->DIBSize == 40 && (BMP->CompressionType == BMP_BitFields || BMP->CompressionType == BMP_RGBABitFields)) {
                BMP->RMask = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, BMP->BitDepth);
                BMP->GMask = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, BMP->BitDepth);
                BMP->BMask = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, BMP->BitDepth);
                if (BMP->CompressionType == BMP_RGBABitFields) {
                    BMP->AMask = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, BMP->BitDepth);
                }
            } else {
                BitBuffer_Seek(BitB, Bits2Bytes((BMP->Offset - 14) - BMP->DIBSize, RoundingType_Down));
            }
        } else if (BMP == NULL) {
            Log(Log_DEBUG, __func__, U8("BMPOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void BMPExtractImage(BMPOptions *BMP, BitBuffer *BitB, ImageContainer *Image) {
        if (BMP != NULL && BitB != NULL && Image != NULL) {
            uint16_t BitDepth        = BMP->BitDepth;
            uint32_t Width           = BMP->Width;
            bool     IsUpsideDown    = (BMP->Height & 0x80000000) >> 31;
            uint32_t Height          = Absolute(BMP->Height);
            
            if (BMP->CompressionType == BMP_RGB) {
                if (BitDepth <= 8) {
                    uint8_t ****Array  = (uint8_t****) ImageContainer_GetArray(Image);
                    for (uint64_t View = 0ULL; View < ImageContainer_GetNumViews(Image); View++) {
                        for (uint64_t W = 0ULL; W < Width; W++) {
                            for (uint64_t H = 0ULL; H < Height; H++) {
                                for (uint64_t Channel = 0ULL; Channel < 3; Channel++) {
                                    Array[View * W * H * Channel] = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, Bits2Bytes(BitDepth, RoundingType_Up));
                                }
                            }
                        }
                    }
                } else if (BMP->BitDepth > 8 && BMP->BitDepth <= 16) {
                    uint16_t ****Array = (uint16_t****) ImageContainer_GetArray(Image);
                    for (uint64_t View = 0ULL; View < ImageContainer_GetNumViews(Image); View++) {
                        for (uint64_t W = 0ULL; W < Width; W++) {
                            for (uint64_t H = 0ULL; H < Height; H++) {
                                for (uint64_t Channel = 0ULL; Channel < 3; Channel++) {
                                    Array[View * W * H * Channel] = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, Bits2Bytes(BitDepth, RoundingType_Up));
                                }
                            }
                        }
                    }
                } else if (BMP->CompressionType == BMP_RLE_8Bit || BMP->CompressionType == BMP_RLE_4Bit) {
                    Log(Log_DEBUG, __func__, U8("We don't support BMP's encoded with RLE"));
                } else if (BMP->CompressionType == BMP_BitFields || BMP->CompressionType == BMP_RGBABitFields) {
                    Log(Log_DEBUG, __func__, U8("We don't support BMP's encoded with BitFields"));
                } else if (BMP->CompressionType == BMP_JPEG) {
                    Log(Log_DEBUG, __func__, U8("BMP Compression Type is JPEG, there's nothing we can do since JPEG is a lossy format"));
                } else if (BMP->CompressionType == BMP_PNG) {
                    Log(Log_DEBUG, __func__, U8("BMP Compression Type is PNG, Extract it to a plain PNG and re-run"));
                } else if (BMP->CompressionType == BMP_CMYK || BMP->CompressionType == BMP_CMYK_RLE_8Bit || BMP->CompressionType == BMP_CMYK_RLE_4Bit) {
                    Log(Log_DEBUG, __func__, U8("CMYK Images are unsupported"));
                }
            }
            if (IsUpsideDown) { // The Image is upside down, so we need to flip it
                ImageContainer_Flip(Image, FlipType_Vertical);
            }
            if (BMP->ColorsIndexed > 0) {
                // The image is palettized, so we need to go ahead and map the pixel bits to the actual colors.
                Log(Log_DEBUG, __func__, U8("Palettized BMP's are currently unsupported"));
            }
        } else if (BMP == NULL) {
            Log(Log_DEBUG, __func__, U8("BMPOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitB Pointer is NULL"));
        } else if (Image == NULL) {
            Log(Log_DEBUG, __func__, U8("ImageContainer Pointer is NULL"));
        }
    }
    
    OVIADecoder BMPDecoder = {
        .DecoderID             = CodecID_BMP,
        .MediaType             = MediaType_Image,
        .MagicIDOffset         = 0,
        .MagicIDSize           = 2,
        .MagicID               = {0x42, 0x4D},
        .Function_Initialize   = BMPOptions_Init,
        .Function_Parse        = BMPParseMetadata,
        .Function_Decode       = BMPExtractImage,
        .Function_Deinitialize = BMPOptions_Deinit,
    };
    
#ifdef __cplusplus
}
#endif
