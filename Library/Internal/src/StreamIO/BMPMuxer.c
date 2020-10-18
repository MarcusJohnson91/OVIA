#include "../../include/Private/StreamIO/BMPStream.h"
#include "../../../Dependencies/FoundationIO/Library/include/TextIO/LogIO.h"
#include "../../../Dependencies/FoundationIO/Library/include/MathIO.h"

#ifdef __cplusplus
extern "C" {
#endif

    void BMPWriteHeader(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            BMPOptions *BMP        = Options;
            uint32_t ImageSize     = Bits2Bytes(BMP->Width * AbsoluteI(BMP->Height) * BMP->BitDepth, RoundingType_Up);
            uint32_t DIBHeaderSize = 40;
            uint64_t FileSize      = DIBHeaderSize + 2 + ImageSize;
            uint16_t NumPlanes     = 1; // Constant

            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16, BMP_BM);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32, FileSize);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16, 0); // Reserved1
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16, 0); // Reserved2
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32, 2 + 40);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32, DIBHeaderSize);
            /* Write DIB Header */
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32, BMP->Width);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32, BMP->Height);

            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16, NumPlanes);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16, BMP->BitDepth);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32, BMP->CompressionType);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32, ImageSize);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32, BMP->WidthPixelsPerMeter);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32, BMP->HeightPixelsPerMeter);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32, BMP->ColorsIndexed);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32, BMP->IndexedColorsUsed);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void BMPInsertImage(void *Options, BitBuffer *BitB, ImageContainer *Image) {
        if (Options != NULL && BitB != NULL && Image != NULL) {
            BMPOptions *BMP         = Options;
            uint64_t Width          = ImageContainer_GetWidth(Image);
            uint64_t Height         = ImageContainer_GetHeight(Image);
            uint64_t NumChannels    = ImageContainer_GetNumChannels(Image);
            uint64_t BitDepth       = Bits2Bytes(ImageContainer_GetBitDepth(Image), RoundingType_Up);
            MediaIO_ImageTypes Type = ImageContainer_GetType(Image);

            if (Type == ImageType_Integer8) {
                uint8_t ****Array = (uint8_t****) ImageContainer_GetArray(Image);
                for (uint64_t W = 0; W < Width; W++) {
                    for (uint64_t H = 0; H < Height; H++) {
                        for (uint16_t C = 0; C < NumChannels; C++) {
                            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, BitDepth, Array[0][W][H][C]);
                        }
                    }
                }
            } else if (Type == ImageType_Integer16) {
                uint16_t ****Array = (uint16_t****) ImageContainer_GetArray(Image);
                for (uint64_t W = 0; W < Width; W++) {
                    for (uint64_t H = 0; H < Height; H++) {
                        for (uint16_t C = 0; C < NumChannels; C++) {
                            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, BitDepth, Array[0][W][H][C]);
                        }
                    }
                }
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (Image == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        }
    }

#ifdef __cplusplus
}
#endif

