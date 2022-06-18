#include "../../include/StreamIO/BMPStream.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h" /* Included for Assertions */
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    void BMPWriteHeader(BMPOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        uint32_t ImageSize     = Bits2Bytes(Options->Width * AbsoluteI(Options->Height) * Options->BitDepth, RoundingType_Up);
        uint32_t DIBHeaderSize = 40;
        uint64_t FileSize      = DIBHeaderSize + 2 + ImageSize;
        uint16_t NumPlanes     = 1; // Constant
        
        BitBuffer_WriteBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 16, BMP_BM);
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32, FileSize);
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 16, 0); // Reserved1
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 16, 0); // Reserved2
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32, 2 + 40);
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32, DIBHeaderSize);
        /* Write DIB Header */
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32, Options->Width);
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32, Options->Height);
        
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 16, NumPlanes);
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 16, Options->BitDepth);
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32, Options->CompressionType);
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32, ImageSize);
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32, Options->WidthPixelsPerMeter);
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32, Options->HeightPixelsPerMeter);
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32, Options->ColorsIndexed);
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32, Options->IndexedColorsUsed);
    }
    
    void BMPInsertImage(BMPOptions *Options, BitBuffer *BitB, ImageContainer *Image) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        AssertIO(Image != NULL);
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
                        BitBuffer_WriteBits(BitB, ByteOrder_Left2Right, BitOrder_Left2Right, BitDepth, Array[0][W][H][C]);
                    }
                }
            }
        } else if (Type == ImageType_Integer16) {
            uint16_t ****Array = (uint16_t****) ImageContainer_GetArray(Image);
            for (uint64_t W = 0; W < Width; W++) {
                for (uint64_t H = 0; H < Height; H++) {
                    for (uint16_t C = 0; C < NumChannels; C++) {
                        BitBuffer_WriteBits(BitB, ByteOrder_Left2Right, BitOrder_Left2Right, BitDepth, Array[0][W][H][C]);
                    }
                }
            }
        }
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

