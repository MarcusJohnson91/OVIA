#include "../../include/Private/PNMCommon.h"


#ifdef __cplusplus
extern "C" {
#endif
    
    static void PNMWriteHeader_ASCII(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNMOptions *PNM = Options;
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
        } else if (Options == NULL) {
            Log(Log_DEBUG, __func__, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    static void PNMWriteHeader_Binary(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNMOptions *PNM = Options;
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
        } else if (Options == NULL) {
            Log(Log_DEBUG, __func__, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    static void PNMWriteHeader_PAM(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNMOptions *PNM = Options;
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
        } else if (Options == NULL) {
            Log(Log_DEBUG, __func__, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNMInsertImage(void *Options, void *Container, BitBuffer *BitB) { // void *Options, void *Contanier, BitBuffer *BitB
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
            Log(Log_DEBUG, __func__, UTF8String("Options Pointer is NULL"));
        } else if (Container == NULL) {
            Log(Log_DEBUG, __func__, UTF8String("Container Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    static void RegisterEncoder_PNM(OVIA *Ovia) {
        Ovia->NumEncoders                                    += 1;
        uint64_t EncoderIndex                                 = Ovia->NumDecoders - 1;
        Ovia->Encoders                                        = realloc(Ovia->Encoders, sizeof(OVIAEncoder) * Ovia->NumEncoders);
        
        Ovia->Encoders[EncoderIndex].EncoderID                = CodecID_PNM;
        Ovia->Encoders[EncoderIndex].MediaType                = MediaType_Image;
        
        /* MagicID 0 */
        Ovia->Encoders[EncoderIndex].Function_Initialize[0]   = PNMOptions_Init;
        Ovia->Encoders[EncoderIndex].Function_WriteHeader[0]  = PNMWriteHeader_ASCII;
        Ovia->Encoders[EncoderIndex].Function_Encode[0]       = PNMInsertImage;
        Ovia->Encoders[EncoderIndex].Function_Deinitialize[0] = PNMOptions_Deinit;
        /* MagicID 0 */
        
        /* MagicID 1 */
        Ovia->Encoders[EncoderIndex].Function_Initialize[1]   = PNMOptions_Init;
        Ovia->Encoders[EncoderIndex].Function_WriteHeader[1]  = PNMWriteHeader_ASCII;
        Ovia->Encoders[EncoderIndex].Function_Encode[1]       = PNMInsertImage;
        Ovia->Encoders[EncoderIndex].Function_Deinitialize[1] = PNMOptions_Deinit;
        /* MagicID 1 */
        
        /* MagicID 2 */
        Ovia->Encoders[EncoderIndex].Function_Initialize[2]   = PNMOptions_Init;
        Ovia->Encoders[EncoderIndex].Function_WriteHeader[2]  = PNMWriteHeader_ASCII;
        Ovia->Encoders[EncoderIndex].Function_Encode[2]       = PNMInsertImage;
        Ovia->Encoders[EncoderIndex].Function_Deinitialize[2] = PNMOptions_Deinit;
        /* MagicID 2 */ // Last ASCII
        
        /* MagicID 3 */
        Ovia->Encoders[EncoderIndex].Function_Initialize[3]   = PNMOptions_Init;
        Ovia->Encoders[EncoderIndex].Function_WriteHeader[3]  = PNMWriteHeader_Binary;
        Ovia->Encoders[EncoderIndex].Function_Encode[3]       = PNMInsertImage;
        Ovia->Encoders[EncoderIndex].Function_Deinitialize[3] = PNMOptions_Deinit;
        /* MagicID 3 */
        
        /* MagicID 4 */
        Ovia->Encoders[EncoderIndex].Function_Initialize[4]   = PNMOptions_Init;
        Ovia->Encoders[EncoderIndex].Function_WriteHeader[4]  = PNMWriteHeader_Binary;
        Ovia->Encoders[EncoderIndex].Function_Encode[4]       = PNMInsertImage;
        Ovia->Encoders[EncoderIndex].Function_Deinitialize[4] = PNMOptions_Deinit;
        /* MagicID 4 */
        
        /* MagicID 5 */
        Ovia->Encoders[EncoderIndex].Function_Initialize[5]   = PNMOptions_Init;
        Ovia->Encoders[EncoderIndex].Function_WriteHeader[5]  = PNMWriteHeader_Binary;
        Ovia->Encoders[EncoderIndex].Function_Encode[5]       = PNMInsertImage;
        Ovia->Encoders[EncoderIndex].Function_Deinitialize[5] = PNMOptions_Deinit;
        /* MagicID 5 */
        
        /* MagicID 6 */
        Ovia->Encoders[EncoderIndex].Function_Initialize[6]   = PNMOptions_Init;
        Ovia->Encoders[EncoderIndex].Function_WriteHeader[6]  = PNMWriteHeader_PAM;
        Ovia->Encoders[EncoderIndex].Function_Encode[6]       = PNMInsertImage;
        Ovia->Encoders[EncoderIndex].Function_Deinitialize[6] = PNMOptions_Deinit;
        /* MagicID 6 */
    }
    
    static OVIACodecRegistry Register_PNMEncoder = {
        .Function_RegisterEncoder[CodecID_PNM - 1]            = RegisterEncoder_PNM,
    };
    
#ifdef __cplusplus
}
#endif
