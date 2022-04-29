#include "../../include/StreamIO/PNMStream.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h" /* Included for Assertions */
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/TextIO/StringIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    static void PNM_WriteWidth(BitBuffer *BitB, uint64_t Width) {
        UTF8 *WidthString = UTF8_Integer2String(Base_Integer | Base_Radix10, Width);
        BitBuffer_WriteUTF8(BitB, WidthString, StringTerminator_Sized);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8, PNMEndField);
        UTF8_Deinit(WidthString);
    }

    static void PNM_WriteHeight(BitBuffer *BitB, uint64_t Height) {
        UTF8 *HeightString = UTF8_Integer2String(Base_Integer | Base_Radix10, Height);
        BitBuffer_WriteUTF8(BitB, HeightString, StringTerminator_Sized);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8, PNMEndField);
        UTF8_Deinit(HeightString);
    }

    static void PNM_WriteBitDepth(BitBuffer *BitB, uint64_t BitDepth) {
        uint64_t MaxVal    = Exponentiate(2, BitDepth) - 1;
        UTF8    *BitDepthString = UTF8_Integer2String(Base_Integer | Base_Radix10, MaxVal);
        BitBuffer_WriteUTF8(BitB, BitDepthString, StringTerminator_Sized);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8, 0x0A);
        UTF8_Deinit(BitDepthString);
    }

    static void PNM_WriteNumChannels(BitBuffer *BitB, uint64_t NumChannels) {
        UTF8    *NumChannelsString  = UTF8_Integer2String(Base_Integer | Base_Radix10, NumChannels);
        BitBuffer_WriteUTF8(BitB, NumChannelsString, StringTerminator_Sized);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8, 0x0A);
        UTF8_Deinit(NumChannelsString);
    }
    
    static void PNMWriteHeader_ASCII(PNMOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        /* Write the Width */
        PNM_WriteWidth(BitB, Options->Width);
        /* Write the Width */

        /* Write the Height */
        PNM_WriteHeight(BitB, Options->Height);
        /* Write the Height */
    }
    
    static void PNMWriteHeader_Binary(PNMOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);

        /* Write the Width */
        PNM_WriteWidth(BitB, Options->Width);
        /* Write the Width */

        /* Write the Height */
        PNM_WriteHeight(BitB, Options->Height);
        /* Write the Height */

        /* Write the BitDepth */
        PNM_WriteBitDepth(BitB, Options->BitDepth);
        /* Write the BitDepth */
    }
    
    static void PNMWriteHeader_PAM(PNMOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);

        BitBuffer_WriteUTF8(BitB, UTF8String("P7"), StringTerminator_Sized);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8, 0x0A);

        /* Write the Width */
        BitBuffer_WriteUTF8(BitB, UTF8String("WIDTH "), StringTerminator_Sized);
        PNM_WriteWidth(BitB, Options->Width);
        /* Write the Width */

        /* Write the Height */
        BitBuffer_WriteUTF8(BitB, UTF8String("HEIGHT "), StringTerminator_Sized);
        PNM_WriteHeight(BitB, Options->Height);
        /* Write the Height */

        /* Write the NumChannels */
        BitBuffer_WriteUTF8(BitB, UTF8String("DEPTH "), StringTerminator_Sized);
        PNM_WriteNumChannels(BitB, Options->NumChannels);
        /* Write the NumChannels */

        /* Write the BitDepth */
        BitBuffer_WriteUTF8(BitB, UTF8String("MAXVAL "), StringTerminator_Sized);
        PNM_WriteBitDepth(BitB, Options->BitDepth);
        /* Write the BitDepth */

        /* Write the TUPLTYPE */
        BitBuffer_WriteUTF8(BitB, UTF8String("TUPLTYPE "), StringTerminator_Sized);
        PNMTupleTypes TupleType = Options->TupleType;
        if (TupleType == PNM_TUPLE_BnW) {
            BitBuffer_WriteUTF8(BitB, UTF8String("BLACKANDWHITE"), StringTerminator_Sized);
        } else if (TupleType == PNM_TUPLE_Gray) {
            BitBuffer_WriteUTF8(BitB, UTF8String("GRAYSCALE"), StringTerminator_Sized);
        } else if (TupleType == PNM_TUPLE_GrayAlpha) {
            BitBuffer_WriteUTF8(BitB, UTF8String("GRAYSCALE_ALPHA"), StringTerminator_Sized);
        } else if (TupleType == PNM_TUPLE_RGB) {
            BitBuffer_WriteUTF8(BitB, UTF8String("RGB"), StringTerminator_Sized);
        } else if (TupleType == PNM_TUPLE_RGBAlpha) {
            BitBuffer_WriteUTF8(BitB, UTF8String("RGB_ALPHA"), StringTerminator_Sized);
        }
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8, 0x0A);
        /* Write the TUPLTYPE */

        /* Write the ENDHDR */
        BitBuffer_WriteUTF8(BitB, UTF8String("ENDHDR"), StringTerminator_Sized);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8, 0x0A);
        /* Write the ENDHDR */
    }
    
    void PNMInsertImage(PNMOptions *Options, BitBuffer *BitB, ImageContainer *Image) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        AssertIO(Image != NULL);
        
        uint64_t ChannelCount   = Options->NumChannels;
        uint64_t Width          = ImageContainer_GetWidth(Image);
        uint64_t Height         = ImageContainer_GetHeight(Image);
        MediaIO_ImageTypes Type = ImageContainer_GetType(Image);
        if (Type == ImageType_Integer8) {
            uint8_t *Array  = (uint8_t*) ImageContainer_GetArray(Image);
            for (uint64_t W = 0ULL; W < Width; W++) {
                for (uint64_t H = 0ULL; H < Height; H++) {
                    for (uint16_t Channel = 0; Channel < ChannelCount; Channel++) {
                        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, ChannelCount, Array[W * H * Channel]);
                    }
                }
            }
        } else if (Type == ImageType_Integer16) {
            uint16_t *Array  = (uint16_t*) ImageContainer_GetArray(Image);
            for (uint64_t W = 0ULL; W < Width; W++) {
                for (uint64_t H = 0ULL; H < Height; H++) {
                    for (uint16_t Channel = 0; Channel < ChannelCount; Channel++) {
                        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, ChannelCount, Array[W * H * Channel]);
                    }
                }
            }
        }
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
