#include "../../include/Private/PNMCommon.h"


#ifdef __cplusplus
extern "C" {
#endif
    
    static void PNMWritePAMHeader(BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            BitBuffer_WriteUTF8(BitB, U8("P7"));
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, 0x0A);
            
            /* Write the Width */
            UTF8 *Width = UTF8_Integer2String(10, OVIA_GetWidth(Ovia));
            BitBuffer_WriteUTF8(BitB, U8("WIDTH "));
            BitBuffer_WriteUTF8(BitB, Width);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, 0x0A);
            free(Width);
            /* Write the Width */
            
            /* Write the Height */
            UTF8 *Height = UTF8_Integer2String(10, OVIA_GetHeight(Ovia));
            BitBuffer_WriteUTF8(BitB, U8("HEIGHT "));
            BitBuffer_WriteUTF8(BitB, Height);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, 0x0A);
            free(Height);
            /* Write the Height */
            
            /* Write the NumChannels */
            UTF8 *NumChannels = UTF8_Integer2String(10, OVIA_GetNumChannels(Ovia));
            BitBuffer_WriteUTF8(BitB, U8("DEPTH "));
            BitBuffer_WriteUTF8(BitB, NumChannels);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, 0x0A);
            free(NumChannels);
            /* Write the NumChannels */
            
            /* Write the BitDepth */
            uint64_t MaxVal = Exponentiate(2, OVIA_GetBitDepth(Ovia)) - 1;
            UTF8 *BitDepth  = UTF8_Integer2String(10, MaxVal);
            BitBuffer_WriteUTF8(BitB, U8("MAXVAL "));
            BitBuffer_WriteUTF8(BitB, BitDepth);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, 0x0A);
            free(BitDepth);
            /* Write the BitDepth */
            
            /* Write the TUPLTYPE */
            BitBuffer_WriteUTF8(BitB, U8("TUPLTYPE "));
            PNMTupleTypes TupleType = OVIA_PNM_GetTupleType(Ovia);
            if (TupleType == PNM_TUPLE_BnW) {
                BitBuffer_WriteUTF8(BitB, U8("BLACKANDWHITE"));
            } else if (TupleType == PNM_TUPLE_Gray) {
                BitBuffer_WriteUTF8(BitB, U8("GRAYSCALE"));
            } else if (TupleType == PNM_TUPLE_GrayAlpha) {
                BitBuffer_WriteUTF8(BitB, U8("GRAYSCALE_ALPHA"));
            } else if (TupleType == PNM_TUPLE_RGB) {
                BitBuffer_WriteUTF8(BitB, U8("RGB"));
            } else if (TupleType == PNM_TUPLE_RGBAlpha) {
                BitBuffer_WriteUTF8(BitB, U8("RGB_ALPHA"));
            }
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, 0x0A);
            /* Write the TUPLTYPE */
            
            /* Write the ENDHDR */
            BitBuffer_WriteUTF8(BitB, U8("ENDHDR"));
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, 0x0A);
            /* Write the ENDHDR */
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void PNMWriteASCIIPNMHeader(BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            /* Write the Width */
            UTF8 *Width = UTF8_Integer2String(10, OVIA_GetWidth(Ovia));
            BitBuffer_WriteUTF8(BitB, Width);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, 0x0A);
            free(Width);
            /* Write the Width */
            
            /* Write the Height */
            UTF8 *Height = UTF8_Integer2String(10, OVIA_GetHeight(Ovia));
            BitBuffer_WriteUTF8(BitB, Height);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, 0x0A);
            free(Height);
            /* Write the Height */
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void PNMWriteBinaryPNMHeader(BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            /* Write the Width */
            UTF8 *Width = UTF8_Integer2String(10, OVIA_GetWidth(Ovia));
            BitBuffer_WriteUTF8(BitB, Width);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, 0x0A);
            free(Width);
            /* Write the Width */
            
            /* Write the Height */
            UTF8 *Height = UTF8_Integer2String(10, OVIA_GetHeight(Ovia));
            BitBuffer_WriteUTF8(BitB, Height);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, 0x0A);
            free(Height);
            /* Write the Height */
            
            /* Write the BitDepth */
            uint64_t MaxVal    = Exponentiate(2, OVIA_GetBitDepth(Ovia)) - 1;
            UTF8    *BitDepth  = UTF8_Integer2String(10, MaxVal);
            BitBuffer_WriteUTF8(BitB, BitDepth);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, 0x0A);
            free(BitDepth);
            /* Write the BitDepth */
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNMWriteHeader(BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            PNMTypes Type = OVIA_PNM_GetPNMType(Ovia);
            if (Type == UnknownPNM) {
                Type = PAMPNM;
            }
            
            if (Type == PAMPNM) {
                PNMWritePAMHeader(Ovia, BitB);
            } else if (Type == BinaryPNM) {
                PNMWriteBinaryPNMHeader(Ovia, BitB);
            } else if (Type == ASCIIPNM) {
                PNMWriteASCIIPNMHeader(Ovia, BitB);
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNMInsertImage(ImageContainer *Image, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL && Image != NULL) {
            uint64_t ChannelCount = OVIA_GetNumChannels(Ovia);
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
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (Image == NULL) {
            Log(Log_ERROR, __func__, U8("Pixels2Write Pointer is NULL"));
        }
    }
    
    OVIAEncoder PNMEncoder_BitASCII = {
        .EncoderID             = CodecID_PBM_A,
        .MediaType             = MediaType_Audio2D,
        .Function_Initialize   = PNMOptions_Init,
        .Function_WriteHeader  = PNMWriteHeader,
        .Function_Encode       = PNMInsertImage,
        .Function_WriteFooter  = NULL,
        .Function_Deinitialize = PNMOptions_Deinit,
    };
    
    OVIAEncoder PNMEncoder_GrayASCII = {
        .EncoderID             = CodecID_PGM_A,
        .MediaType             = MediaType_Image,
        .Function_Initialize   = PNMOptions_Init,
        .Function_WriteHeader  = PNMWriteHeader,
        .Function_Encode       = PNMInsertImage,
        .Function_WriteFooter  = NULL,
        .Function_Deinitialize = PNMOptions_Deinit,
    };
    
    OVIAEncoder PNMEncoder_PixASCII = {
        .EncoderID             = CodecID_PPM_A,
        .MediaType             = MediaType_Image,
        .Function_Initialize   = PNMOptions_Init,
        .Function_WriteHeader  = PNMWriteHeader,
        .Function_Encode       = PNMInsertImage,
        .Function_WriteFooter  = NULL,
        .Function_Deinitialize = PNMOptions_Deinit,
    };
    
    OVIAEncoder PNMEncoder_BitBinary = {
        .EncoderID             = CodecID_PBM_B,
        .MediaType             = MediaType_Image,
        .Function_Initialize   = PNMOptions_Init,
        .Function_WriteHeader  = PNMWriteHeader,
        .Function_Encode       = PNMInsertImage,
        .Function_WriteFooter  = NULL,
        .Function_Deinitialize = PNMOptions_Deinit,
    };
    
    OVIAEncoder PNMEncoder_GrayBinary = {
        .EncoderID             = CodecID_PGM_B,
        .MediaType             = MediaType_Image,
        .Function_Initialize   = PNMOptions_Init,
        .Function_WriteHeader  = PNMWriteHeader,
        .Function_Encode       = PNMInsertImage,
        .Function_WriteFooter  = NULL,
        .Function_Deinitialize = PNMOptions_Deinit,
    };
    
    OVIAEncoder PNMEncoder_PixBinary = {
        .EncoderID             = CodecID_PPM_B,
        .MediaType             = MediaType_Image,
        .Function_Initialize   = PNMOptions_Init,
        .Function_WriteHeader  = PNMWriteHeader,
        .Function_Encode       = PNMInsertImage,
        .Function_WriteFooter  = NULL,
        .Function_Deinitialize = PNMOptions_Deinit,
    };
    
    OVIAEncoder PNMEncoder_Any = {
        .EncoderID             = CodecID_PAM,
        .MediaType             = MediaType_Image,
        .Function_Initialize   = PNMOptions_Init,
        .Function_WriteHeader  = PNMWriteHeader,
        .Function_Encode       = PNMInsertImage,
        .Function_WriteFooter  = NULL,
        .Function_Deinitialize = PNMOptions_Deinit,
    };
    
#ifdef __cplusplus
}
#endif
