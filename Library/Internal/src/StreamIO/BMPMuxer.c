#include "../../include/StreamIO/BMPStream.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h" /* Included for Assertions */
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    void BMPWriteHeader(void *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
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
    }

    void BMPInsertImage(void *Options, BitBuffer *BitB, ImageContainer *Image) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        AssertIO(Image != NULL);
        BMPOptions *BMP         = Options;
        uint64_t Width          = ImageContainer_GetWidth(Image);
        uint64_t Height         = ImageContainer_GetHeight(Image);
        ImageChannelMap *Map    = ImageContainer_GetChannelMap(Image);
        MediaIO_ImageTypes Type = ImageContainer_GetType(Image);
        uint64_t NumChannels    = ImageChannelMap_GetNumChannels(Map);
        uint64_t BitDepth       = Bits2Bytes(ImageType_GetBitDepth(Type), RoundingType_Up);

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
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

