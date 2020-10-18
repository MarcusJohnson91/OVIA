#include "../../include/Private/StreamIO/PNMStream.h"
#include "../../../Dependencies/FoundationIO/Library/include/TextIO/StringIO.h"
#include "../../../Dependencies/FoundationIO/Library/include/TextIO/LogIO.h"
#include "../../../Dependencies/FoundationIO/Library/include/MathIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    static void PNMWriteHeader_ASCII(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNMOptions *PNM = Options;
            /* Write the Width */
            UTF8 *Width = UTF8_Integer2String(Base_Integer | Base_Radix10, PNM->Width);
            BitBuffer_WriteUTF8(BitB, Width, StringTerminator_Sized);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8, PNMEndField);
            free(Width);
            /* Write the Width */
            
            /* Write the Height */
            UTF8 *Height = UTF8_Integer2String(Base_Integer | Base_Radix10, PNM->Height);
            BitBuffer_WriteUTF8(BitB, Height, StringTerminator_Sized);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8, PNMEndField);
            free(Height);
            /* Write the Height */
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    static void PNMWriteHeader_Binary(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNMOptions *PNM = Options;
            /* Write the Width */
            UTF8 *Width = UTF8_Integer2String(Base_Integer | Base_Radix10, PNM->Width);
            BitBuffer_WriteUTF8(BitB, Width, StringTerminator_Sized);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8, 0x0A);
            free(Width);
            /* Write the Width */
            
            /* Write the Height */
            UTF8 *Height = UTF8_Integer2String(Base_Integer | Base_Radix10, PNM->Height);
            BitBuffer_WriteUTF8(BitB, Height, StringTerminator_Sized);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8, 0x0A);
            free(Height);
            /* Write the Height */
            
            /* Write the BitDepth */
            uint64_t MaxVal    = Exponentiate(2, PNM->BitDepth) - 1;
            UTF8    *BitDepth  = UTF8_Integer2String(Base_Integer | Base_Radix10, MaxVal);
            BitBuffer_WriteUTF8(BitB, BitDepth, StringTerminator_Sized);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8, 0x0A);
            free(BitDepth);
            /* Write the BitDepth */
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    static void PNMWriteHeader_PAM(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNMOptions *PNM = Options;
            BitBuffer_WriteUTF8(BitB, UTF8String("P7"), StringTerminator_Sized);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8, 0x0A);
            
            /* Write the Width */
            UTF8 *Width         = UTF8_Integer2String(Base_Integer | Base_Radix10, PNM->Width);
            BitBuffer_WriteUTF8(BitB, UTF8String("WIDTH "), StringTerminator_Sized);
            BitBuffer_WriteUTF8(BitB, Width, StringTerminator_Sized);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8, 0x0A);
            free(Width);
            /* Write the Width */
            
            /* Write the Height */
            UTF8 *Height = UTF8_Integer2String(Base_Integer | Base_Radix10, PNM->Height);
            BitBuffer_WriteUTF8(BitB, UTF8String("HEIGHT "), StringTerminator_Sized);
            BitBuffer_WriteUTF8(BitB, Height, StringTerminator_Sized);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8, 0x0A);
            free(Height);
            /* Write the Height */
            
            /* Write the NumChannels */
            UTF8 *NumChannels = UTF8_Integer2String(Base_Integer | Base_Radix10, PNM->NumChannels);
            BitBuffer_WriteUTF8(BitB, UTF8String("DEPTH "), StringTerminator_Sized);
            BitBuffer_WriteUTF8(BitB, NumChannels, StringTerminator_Sized);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8, 0x0A);
            free(NumChannels);
            /* Write the NumChannels */
            
            /* Write the BitDepth */
            uint64_t MaxVal = Exponentiate(2, PNM->BitDepth) - 1;
            UTF8 *BitDepth  = UTF8_Integer2String(Base_Integer | Base_Radix10, MaxVal);
            BitBuffer_WriteUTF8(BitB, UTF8String("MAXVAL "), StringTerminator_Sized);
            BitBuffer_WriteUTF8(BitB, BitDepth, StringTerminator_Sized);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8, 0x0A);
            free(BitDepth);
            /* Write the BitDepth */
            
            /* Write the TUPLTYPE */
            BitBuffer_WriteUTF8(BitB, UTF8String("TUPLTYPE "), StringTerminator_Sized);
            PNMTupleTypes TupleType = PNM->TupleType;
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
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNMInsertImage(void *Options, BitBuffer *BitB, ImageContainer *Image) {
        if (Options != NULL && BitB != NULL && Image != NULL) {
            PNMOptions *PNM         = Options;
            uint64_t ChannelCount   = PNM->NumChannels;
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
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (Image == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pixels2Write Pointer is NULL"));
        }
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
