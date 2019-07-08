#include "../../include/Private/PNMCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    static uint64_t PNMCheckForComment(BitBuffer *BitB) {
        uint64_t CommentSize = 0;
        if (BitB != NULL) {
            if (BitBuffer_PeekBits(BitB, MSByteFirst, LSBitFirst, 8) == PNMCommentStart) {
                BitBuffer_Seek(BitB, 8);
                do {
                    CommentSize += 1;
                } while (BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8) != PNMEndField);
            }
        } else {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
        return CommentSize;
    }
    
    static void PNMParse_ASCII(PNMOptions *PNM, BitBuffer *BitB) {
        if (BitB != NULL) {
            uint64_t CommentSizeWidth = PNMCheckForComment(BitB);
            BitBuffer_Seek(BitB, Bytes2Bits(CommentSizeWidth));
            /* Read Width */
            uint64_t WidthStringSize  = BitBuffer_GetUTF8StringSize(BitB);
            UTF8    *WidthString      = BitBuffer_ReadUTF8(BitB, WidthStringSize);
            PNM->Width                = UTF8_String2Integer(Base10, WidthString);
            free(WidthString);
            /* Read Width */
            
            uint64_t CommentSizeHeight = PNMCheckForComment(BitB);
            BitBuffer_Seek(BitB, Bytes2Bits(CommentSizeHeight));
            
            /* Read Height */
            uint64_t HeightStringSize = BitBuffer_GetUTF8StringSize(BitB);
            UTF8    *HeightString     = BitBuffer_ReadUTF8(BitB, HeightStringSize);
            PNM->Height               = UTF8_String2Integer(Base10, HeightString);
            free(HeightString);
            /* Read Height */
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void PNMParse_Binary(PNMOptions *PNM, BitBuffer *BitB) {
        if (BitB != NULL) {
            /* Read Width */
            uint64_t WidthStringSize  = BitBuffer_GetUTF8StringSize(BitB);
            UTF8    *WidthString      = BitBuffer_ReadUTF8(BitB, WidthStringSize);
            PNM->Width                = UTF8_String2Integer(Base10, WidthString);
            free(WidthString);
            /* Read Width */
            
            /* Read Height */
            uint64_t HeightStringSize = BitBuffer_GetUTF8StringSize(BitB);
            UTF8    *HeightString     = BitBuffer_ReadUTF8(BitB, HeightStringSize);
            PNM->Height               = UTF8_String2Integer(Base10, HeightString);
            free(HeightString);
            /* Read Height */
            
            /* Read MaxVal */
            uint64_t MaxValStringSize = BitBuffer_GetUTF8StringSize(BitB);
            UTF8    *MaxValString     = BitBuffer_ReadUTF8(BitB, MaxValStringSize);
            uint64_t MaxVal           = UTF8_String2Integer(Base10, MaxValString);
            PNM->BitDepth             = Logarithm(2, MaxVal + 1);
            free(MaxValString);
            /* Read MaxVal */
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void PNMParsePAMHeader(PNMOptions *PNM, BitBuffer *BitB) {
        if (PNM != NULL && BitB != NULL) {
            /* Read Width */
            BitBuffer_Seek(BitB, 48); // Skip "WIDTH " string
            
            uint64_t WidthStringSize  = BitBuffer_GetUTF8StringSize(BitB);
            UTF8    *WidthString      = BitBuffer_ReadUTF8(BitB, WidthStringSize);
            PNM->Width                = UTF8_String2Integer(Base10, WidthString);
            free(WidthString);
            /* Read Width */
            
            /* Read Height */
            BitBuffer_Seek(BitB, 56); // Skip "HEIGHT " string
            
            uint64_t HeightStringSize = BitBuffer_GetUTF8StringSize(BitB);
            UTF8    *HeightString     = BitBuffer_ReadUTF8(BitB, HeightStringSize);
            PNM->Height               = UTF8_String2Integer(Base10, HeightString);
            free(HeightString);
            /* Read Height */
            
            /* Read NumChannels */
            BitBuffer_Seek(BitB, 48); // Skip "DEPTH " string
            
            uint64_t NumChannelsStringSize  = BitBuffer_GetUTF8StringSize(BitB);
            UTF8    *NumChannelsString      = BitBuffer_ReadUTF8(BitB, NumChannelsStringSize);
            PNM->NumChannels                = UTF8_String2Integer(Base10, NumChannelsString);
            free(NumChannelsString);
            /* Read NumChannels */
            
            /* Read MaxVal */
            BitBuffer_Seek(BitB, 56); // Skip "MAXVAL " string
            
            uint64_t MaxValStringSize = BitBuffer_GetUTF8StringSize(BitB);
            UTF8    *MaxValString     = BitBuffer_ReadUTF8(BitB, MaxValStringSize);
            uint64_t MaxVal           = UTF8_String2Integer(Base10, MaxValString);
            PNM->BitDepth             = Logarithm(2, MaxVal + 1);
            free(MaxValString);
            /* Read MaxVal */
            
            /* Read TupleType */
            BitBuffer_Seek(BitB, 72); // Skip "TUPLETYPE " string
            uint64_t TupleTypeSize     = BitBuffer_GetUTF8StringSize(BitB);
            UTF8    *TupleTypeString   = BitBuffer_ReadUTF8(BitB, TupleTypeSize);
            
            if (UTF8_CompareSubString(TupleTypeString, U8("BLACKANDWHITE"), 0, 0) == true) {
                PNM->NumChannels       = 1;
                PNM->TupleType         = PNM_TUPLE_BnW;
            } else if (UTF8_CompareSubString(TupleTypeString, U8("GRAYSCALE"), 0, 0) == true) {
                PNM->NumChannels       = 1;
                PNM->TupleType         = PNM_TUPLE_Gray;
            } else if (UTF8_CompareSubString(TupleTypeString, U8("GRAYSCALE_ALPHA"), 0, 0) == true) {
                PNM->NumChannels       = 2;
                PNM->TupleType         = PNM_TUPLE_GrayAlpha;
            } else if (UTF8_CompareSubString(TupleTypeString, U8("RGB"), 0, 0) == true) {
                PNM->NumChannels       = 3;
                PNM->TupleType         = PNM_TUPLE_RGB;
            } else if (UTF8_CompareSubString(TupleTypeString, U8("RGB_ALPHA"), 0, 0) == true) {
                PNM->NumChannels       = 4;
                PNM->TupleType         = PNM_TUPLE_RGBAlpha;
            } else {
                Log(Log_DEBUG, __func__, U8("Unknown PNM Tuple: %s"), TupleTypeString);
            }
            free(TupleTypeString);
            /* Read TupleType */
            
            /* Skip ENDHDR */
            BitBuffer_Seek(BitB, 56); // ENDHDR
            /* Skip ENDHDR */
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNMExtractImage_ASCII(PNMOptions *PNM, BitBuffer *BitB, ImageContainer *Image) {
        if (PNM->BitDepth <= 8) {
            uint8_t ****Array        = (uint8_t****) ImageContainer_GetArray(Image);
            UTF8        Component[4] = {0, 0, 0, 0};
            for (uint64_t Width = 0; Width < PNM->Width; Width++) {
                for (uint64_t Height = 0; Height < PNM->Height; Height++) {
                    for (uint8_t Channel = 0; Channel < PNM->NumChannels; Channel++) {
                        for (uint8_t SubPixelByte = 0; SubPixelByte < 3; SubPixelByte++) {
                            Component[SubPixelByte]      = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8);
                        }
                        Array[0][Width][Height][Channel] = UTF8_String2Integer(Base10, Component);
                    }
                }
            }
        } else if (PNM->BitDepth <= 16) {
            uint16_t ****Array       = (uint16_t****) ImageContainer_GetArray(Image);
            UTF8        Component[6] = {0, 0, 0, 0, 0, 0};
            for (uint64_t Width = 0; Width < PNM->Width; Width++) {
                for (uint64_t Height = 0; Height < PNM->Height; Height++) {
                    for (uint8_t Channel = 0; Channel < PNM->NumChannels; Channel++) {
                        for (uint8_t SubPixelByte = 0; SubPixelByte < 3; SubPixelByte++) {
                            Component[SubPixelByte]      = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8);
                        }
                        Array[0][Width][Height][Channel] = UTF8_String2Integer(Base10, Component);
                    }
                }
            }
        } else {
            Log(Log_DEBUG, __func__, U8("16 bit ASCII PNM is invalid"));
        }
    }
    
    void PNMExtractImage_Binary(PNMOptions *PNM, BitBuffer *BitB, ImageContainer *Image) {
        if (PNM->BitDepth <= 8) {
            uint8_t ****Array  = (uint8_t****) ImageContainer_GetArray(Image);
            
            for (uint64_t Width = 0; Width < PNM->Width; Width++) {
                for (uint64_t Height = 0; Height < PNM->Height; Height++) {
                    for (uint8_t Channel = 0; Channel < PNM->NumChannels; Channel++) {
                        uint8_t CurrentPixel                 = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, PNM->BitDepth);
                        if (PNM->TupleType == PNM_TUPLE_BnW) {
                            Array[0][Width][Height][Channel] = ~CurrentPixel; // 1 = black, 0 = white
                        } else {
                            Array[0][Width][Height][Channel] = CurrentPixel; // 1 = white, 0 = black
                        }
                    }
                }
            }
        } else if (PNM->BitDepth <= 16) {
            uint16_t ****Array  = (uint16_t****) ImageContainer_GetArray(Image);
            
            for (uint64_t Width = 0; Width < PNM->Width; Width++) {
                for (uint64_t Height = 0; Height < PNM->Height; Height++) {
                    for (uint8_t Channel = 0; Channel < PNM->NumChannels; Channel++) {
                        uint16_t CurrentPixel                = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, PNM->BitDepth);
                        if (PNM->TupleType == PNM_TUPLE_BnW) {
                            Array[0][Width][Height][Channel] = ~CurrentPixel; // 1 = black, 0 = white
                        } else {
                            Array[0][Width][Height][Channel] = CurrentPixel; // 1 = white, 0 = black
                        }
                    }
                }
            }
        }
    }
    
    OVIADecoder PNMDecoder_BitASCII = {
        .MagicID               = {0x50, 0x31},
        .MagicIDSize           = 2,
        .MagicIDOffset         = 0,
        .MediaType             = MediaType_Image,
        .DecoderID             = CodecID_PBM_A,
        .Function_Initialize   = PNMOptions_Init,
        .Function_Parse        = PNMParse_ASCII,
        .Function_Decode       = PNMExtractImage_ASCII,
        .Function_Deinitialize = PNMOptions_Deinit,
    };
    
    OVIADecoder PNMDecoder_GrayASCII = {
        .MagicID               = {0x50, 0x32},
        .MagicIDSize           = 2,
        .MagicIDOffset         = 0,
        .MediaType             = MediaType_Image,
        .DecoderID             = CodecID_PGM_A,
        .Function_Initialize   = PNMOptions_Init,
        .Function_Parse        = PNMParse_ASCII,
        .Function_Decode       = PNMExtractImage_ASCII,
        .Function_Deinitialize = PNMOptions_Deinit,
    };
    
    OVIADecoder PNMDecoder_PixASCII = {
        .MagicID               = {0x50, 0x33},
        .MagicIDSize           = 2,
        .MagicIDOffset         = 0,
        .MediaType             = MediaType_Image,
        .DecoderID             = CodecID_PPM_A,
        .Function_Initialize   = PNMOptions_Init,
        .Function_Parse        = PNMParse_ASCII,
        .Function_Decode       = PNMExtractImage_ASCII,
        .Function_Deinitialize = PNMOptions_Deinit,
    };
    
    OVIADecoder PNMDecoder_BitBinary = {
        .MagicID               = {0x50, 0x34},
        .MagicIDSize           = 2,
        .MagicIDOffset         = 0,
        .MediaType             = MediaType_Image,
        .DecoderID             = CodecID_PBM_B,
        .Function_Initialize   = PNMOptions_Init,
        .Function_Parse        = PNMParse_Binary,
        .Function_Decode       = PNMExtractImage_Binary,
        .Function_Deinitialize = PNMOptions_Deinit,
    };
    
    OVIADecoder PNMDecoder_GrayBinary = {
        .MagicID               = {0x50, 0x35},
        .MagicIDSize           = 2,
        .MagicIDOffset         = 0,
        .MediaType             = MediaType_Image,
        .DecoderID             = CodecID_PGM_B,
        .Function_Initialize   = PNMOptions_Init,
        .Function_Parse        = PNMParse_Binary,
        .Function_Decode       = PNMExtractImage_Binary,
        .Function_Deinitialize = PNMOptions_Deinit,
    };
    
    OVIADecoder PNMDecoder_PixBinary = {
        .MagicID               = {0x50, 0x36},
        .MagicIDSize           = 2,
        .MagicIDOffset         = 0,
        .MediaType             = MediaType_Image,
        .DecoderID             = CodecID_PPM_B,
        .Function_Initialize   = PNMOptions_Init,
        .Function_Parse        = PNMParse_Binary,
        .Function_Decode       = PNMExtractImage_Binary,
        .Function_Deinitialize = PNMOptions_Deinit,
    };
    
    OVIADecoder PNMDecoder_Any = {
        .MagicID               = {0x50, 0x37},
        .MagicIDSize           = 2,
        .MagicIDOffset         = 0,
        .MediaType             = MediaType_Image,
        .DecoderID             = CodecID_PAM,
        .Function_Initialize   = PNMOptions_Init,
        .Function_Parse        = PNMParsePAMHeader,
        .Function_Decode       = PNMExtractImage_Binary,
        .Function_Deinitialize = PNMOptions_Deinit,
    };
    
#ifdef __cplusplus
}
#endif
