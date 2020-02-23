#include "../../include/Private/PNMCommon.h"


#ifdef __cplusplus
extern "C" {
#endif
    
    void PNMWriteHeader(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNMOptions *PNM = Options;
            if (PNM->Type == ASCIIPNM) {
                /* Write the Width */
                UTF8 *Width = UTF8_Integer2String(Base_Decimal_Radix10, PNM->Width);
                BitBuffer_WriteUTF8(BitB, Width, WriteType_Sized);
                BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, PNMEndField);
                free(Width);
                /* Write the Width */
                
                /* Write the Height */
                UTF8 *Height = UTF8_Integer2String(Base_Decimal_Radix10, PNM->Height);
                BitBuffer_WriteUTF8(BitB, Height, WriteType_Sized);
                BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, PNMEndField);
                free(Height);
                /* Write the Height */
            } else if (PNM->Type == BinaryPNM) {
                /* Write the Width */
                UTF8 *Width = UTF8_Integer2String(Base_Decimal_Radix10, PNM->Width);
                BitBuffer_WriteUTF8(BitB, Width, WriteType_Sized);
                BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, 0x0A);
                free(Width);
                /* Write the Width */
                
                /* Write the Height */
                UTF8 *Height = UTF8_Integer2String(Base_Decimal_Radix10, PNM->Height);
                BitBuffer_WriteUTF8(BitB, Height, WriteType_Sized);
                BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, 0x0A);
                free(Height);
                /* Write the Height */
                
                /* Write the BitDepth */
                uint64_t MaxVal    = Exponentiate(2, PNM->BitDepth) - 1;
                UTF8    *BitDepth  = UTF8_Integer2String(Base_Decimal_Radix10, MaxVal);
                BitBuffer_WriteUTF8(BitB, BitDepth, WriteType_Sized);
                BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, 0x0A);
                free(BitDepth);
                /* Write the BitDepth */
            } else if (PNM->Type == PAMPNM) {
                BitBuffer_WriteUTF8(BitB, UTF8String("P7"), WriteType_Sized);
                BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, 0x0A);
                
                /* Write the Width */
                UTF8 *Width         = UTF8_Integer2String(Base_Decimal_Radix10, PNM->Width);
                BitBuffer_WriteUTF8(BitB, UTF8String("WIDTH "), WriteType_Sized);
                BitBuffer_WriteUTF8(BitB, Width, WriteType_Sized);
                BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, 0x0A);
                free(Width);
                /* Write the Width */
                
                /* Write the Height */
                UTF8 *Height = UTF8_Integer2String(Base_Decimal_Radix10, PNM->Height);
                BitBuffer_WriteUTF8(BitB, UTF8String("HEIGHT "), WriteType_Sized);
                BitBuffer_WriteUTF8(BitB, Height, WriteType_Sized);
                BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, 0x0A);
                free(Height);
                /* Write the Height */
                
                /* Write the NumChannels */
                UTF8 *NumChannels = UTF8_Integer2String(Base_Decimal_Radix10, PNM->NumChannels);
                BitBuffer_WriteUTF8(BitB, UTF8String("DEPTH "), WriteType_Sized);
                BitBuffer_WriteUTF8(BitB, NumChannels, WriteType_Sized);
                BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, 0x0A);
                free(NumChannels);
                /* Write the NumChannels */
                
                /* Write the BitDepth */
                uint64_t MaxVal = Exponentiate(2, PNM->BitDepth) - 1;
                UTF8 *BitDepth  = UTF8_Integer2String(Base_Decimal_Radix10, MaxVal);
                BitBuffer_WriteUTF8(BitB, UTF8String("MAXVAL "), WriteType_Sized);
                BitBuffer_WriteUTF8(BitB, BitDepth, WriteType_Sized);
                BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, 0x0A);
                free(BitDepth);
                /* Write the BitDepth */
                
                /* Write the TUPLTYPE */
                BitBuffer_WriteUTF8(BitB, UTF8String("TUPLTYPE "), WriteType_Sized);
                PNMTupleTypes TupleType = PNM->TupleType;
                if (TupleType == PNM_TUPLE_BnW) {
                    BitBuffer_WriteUTF8(BitB, UTF8String("BLACKANDWHITE"), WriteType_Sized);
                } else if (TupleType == PNM_TUPLE_Gray) {
                    BitBuffer_WriteUTF8(BitB, UTF8String("GRAYSCALE"), WriteType_Sized);
                } else if (TupleType == PNM_TUPLE_GrayAlpha) {
                    BitBuffer_WriteUTF8(BitB, UTF8String("GRAYSCALE_ALPHA"), WriteType_Sized);
                } else if (TupleType == PNM_TUPLE_RGB) {
                    BitBuffer_WriteUTF8(BitB, UTF8String("RGB"), WriteType_Sized);
                } else if (TupleType == PNM_TUPLE_RGBAlpha) {
                    BitBuffer_WriteUTF8(BitB, UTF8String("RGB_ALPHA"), WriteType_Sized);
                }
                BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, 0x0A);
                /* Write the TUPLTYPE */
                
                /* Write the ENDHDR */
                BitBuffer_WriteUTF8(BitB, UTF8String("ENDHDR"), WriteType_Sized);
                BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, 0x0A);
                /* Write the ENDHDR */
            }
        } else if (Options == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNMInsertImage(void *Options, void *Container, BitBuffer *BitB) {
        if (Options != NULL && Container != NULL && BitB != NULL) {
            PNMOptions *PNM       = Options;
            ImageContainer *Image = Container;
            uint64_t ChannelCount = PNM->NumChannels;
            uint64_t Width        = ImageContainer_GetWidth(Image);
            uint64_t Height       = ImageContainer_GetHeight(Image);
            Image_Types Type      = ImageContainer_GetType(Image);
            if (Type == ImageType_Integer8) {
                uint8_t *Array  = (uint8_t*) ImageContainer_GetArray(Image);
                for (uint64_t W = 0ULL; W < Width; W++) {
                    for (uint64_t H = 0ULL; H < Height; H++) {
                        for (uint16_t Channel = 0; Channel < ChannelCount; Channel++) {
                            BitBuffer_WriteBits(BitB, MSByteFirst, MSBitFirst, ChannelCount, Array[W * H * Channel]);
                        }
                    }
                }
            } else if (Type == ImageType_Integer16) {
                uint16_t *Array  = (uint16_t*) ImageContainer_GetArray(Image);
                for (uint64_t W = 0ULL; W < Width; W++) {
                    for (uint64_t H = 0ULL; H < Height; H++) {
                        for (uint16_t Channel = 0; Channel < ChannelCount; Channel++) {
                            BitBuffer_WriteBits(BitB, MSByteFirst, MSBitFirst, ChannelCount, Array[W * H * Channel]);
                        }
                    }
                }
            }
        } else if (Options == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Options Pointer is NULL"));
        } else if (Container == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Container Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNMWriteFooter(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNMOptions *PNM = Options;
            if (PNM->Type == PAMPNM) {
                BitBuffer_WriteUTF8(BitB, UTF8String("ENDHDR"), WriteType_Sized);
            }
        } else if (Options == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
#define PNMNumExtensions 5
    
    static const UTF32 *PNMExtensions[PNMNumExtensions] = {
        [0] = UTF32String("pbm"),
        [1] = UTF32String("pgm"),
        [2] = UTF32String("ppm"),
        [3] = UTF32String("pnm"),
        [4] = UTF32String("pam"),
    };
    
    static const OVIAEncoder PNMEncoder = {
        .EncoderID             = CodecID_PNM,
        .MediaType             = MediaType_Image,
        .NumExtensions         = PNMNumExtensions,
        .Extensions            = PNMExtensions,
        .Function_Initialize   = PNMOptions_Init,
        .Function_WriteHeader  = PNMWriteHeader,
        .Function_Encode       = PNMInsertImage,
        .Function_WriteFooter  = PNMWriteFooter,
        .Function_Deinitialize = PNMOptions_Deinit,
    };
    
#ifdef __cplusplus
}
#endif
