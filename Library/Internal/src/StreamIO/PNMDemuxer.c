#include "../../include/StreamIO/PNMStream.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h" /* Included for Assertions */
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/TextIO/StringIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    static uint64_t PNMCheckForComment(BitBuffer *BitB) {
        AssertIO(BitB != NULL);
        uint64_t CommentSize = 0;
        if (BitBuffer_PeekBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8) == PNMCommentStart) {
            BitBuffer_Seek(BitB, 8);
            do {
                CommentSize += 1;
            } while (BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8) != PNMEndField);
        }
        return CommentSize;
    }
    
    static void PNMParse_ASCII(PNMOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        uint64_t CommentSizeWidth = PNMCheckForComment(BitB);
        BitBuffer_Seek(BitB, Bytes2Bits(CommentSizeWidth));
        /* Read Width */
        uint64_t WidthStringSize  = BitBuffer_GetUTF8StringSize(BitB);
        UTF8    *WidthString      = BitBuffer_ReadUTF8(BitB, WidthStringSize);
        Options->Width            = UTF8_String2Integer(Base_Integer | Base_Radix10, WidthString);
        free(WidthString);
        /* Read Width */
        
        uint64_t CommentSizeHeight = PNMCheckForComment(BitB);
        BitBuffer_Seek(BitB, Bytes2Bits(CommentSizeHeight));
        
        /* Read Height */
        uint64_t HeightStringSize = BitBuffer_GetUTF8StringSize(BitB);
        UTF8    *HeightString     = BitBuffer_ReadUTF8(BitB, HeightStringSize);
        Options->Height               = UTF8_String2Integer(Base_Integer | Base_Radix10, HeightString);
        free(HeightString);
        /* Read Height */
    }
    
    static void PNMParse_Binary(PNMOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        /* Read Width */
        uint64_t WidthStringSize  = BitBuffer_GetUTF8StringSize(BitB);
        UTF8    *WidthString      = BitBuffer_ReadUTF8(BitB, WidthStringSize);
        Options->Width                = UTF8_String2Integer(Base_Integer | Base_Radix10, WidthString);
        free(WidthString);
        /* Read Width */
        
        /* Read Height */
        uint64_t HeightStringSize = BitBuffer_GetUTF8StringSize(BitB);
        UTF8    *HeightString     = BitBuffer_ReadUTF8(BitB, HeightStringSize);
        Options->Height               = UTF8_String2Integer(Base_Integer | Base_Radix10, HeightString);
        free(HeightString);
        /* Read Height */
        
        /* Read MaxVal */
        uint64_t MaxValStringSize = BitBuffer_GetUTF8StringSize(BitB);
        UTF8    *MaxValString     = BitBuffer_ReadUTF8(BitB, MaxValStringSize);
        uint64_t MaxVal           = UTF8_String2Integer(Base_Integer | Base_Radix10, MaxValString);
        Options->BitDepth             = Logarithm(2, MaxVal + 1);
        free(MaxValString);
        /* Read MaxVal */
    }
    
    static void PNMParse_PAM(PNMOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        /* Read Width */
        BitBuffer_Seek(BitB, 48); // Skip "WIDTH " string
        
        uint64_t WidthStringSize  = BitBuffer_GetUTF8StringSize(BitB);
        UTF8    *WidthString      = BitBuffer_ReadUTF8(BitB, WidthStringSize);
        Options->Width                = UTF8_String2Integer(Base_Integer | Base_Radix10, WidthString);
        free(WidthString);
        /* Read Width */
        
        /* Read Height */
        BitBuffer_Seek(BitB, 56); // Skip "HEIGHT " string
        
        uint64_t HeightStringSize = BitBuffer_GetUTF8StringSize(BitB);
        UTF8    *HeightString     = BitBuffer_ReadUTF8(BitB, HeightStringSize);
        Options->Height               = UTF8_String2Integer(Base_Integer | Base_Radix10, HeightString);
        free(HeightString);
        /* Read Height */
        
        /* Read NumChannels */
        BitBuffer_Seek(BitB, 48); // Skip "DEPTH " string
        
        uint64_t NumChannelsStringSize  = BitBuffer_GetUTF8StringSize(BitB);
        UTF8    *NumChannelsString      = BitBuffer_ReadUTF8(BitB, NumChannelsStringSize);
        Options->NumChannels                = UTF8_String2Integer(Base_Integer | Base_Radix10, NumChannelsString);
        free(NumChannelsString);
        /* Read NumChannels */
        
        /* Read MaxVal */
        BitBuffer_Seek(BitB, 56); // Skip "MAXVAL " string
        
        uint64_t MaxValStringSize = BitBuffer_GetUTF8StringSize(BitB);
        UTF8    *MaxValString     = BitBuffer_ReadUTF8(BitB, MaxValStringSize);
        uint64_t MaxVal           = UTF8_String2Integer(Base_Integer | Base_Radix10, MaxValString);
        Options->BitDepth             = Logarithm(2, MaxVal + 1);
        free(MaxValString);
        /* Read MaxVal */
        
        /* Read TupleType */
        BitBuffer_Seek(BitB, 72); // Skip "TUPLETYPE " string
        uint64_t TupleTypeSize     = BitBuffer_GetUTF8StringSize(BitB);
        UTF8    *TupleTypeString   = BitBuffer_ReadUTF8(BitB, TupleTypeSize);
        
        if (UTF8_CompareSubString(TupleTypeString, UTF8String("BLACKANDWHITE"), 0, 0) == true) {
            Options->NumChannels       = 1;
            Options->TupleType         = PNM_TUPLE_BnW;
        } else if (UTF8_CompareSubString(TupleTypeString, UTF8String("GRAYSCALE"), 0, 0) == true) {
            Options->NumChannels       = 1;
            Options->TupleType         = PNM_TUPLE_Gray;
        } else if (UTF8_CompareSubString(TupleTypeString, UTF8String("GRAYSCALE_ALPHA"), 0, 0) == true) {
            Options->NumChannels       = 2;
            Options->TupleType         = PNM_TUPLE_GrayAlpha;
        } else if (UTF8_CompareSubString(TupleTypeString, UTF8String("RGB"), 0, 0) == true) {
            Options->NumChannels       = 3;
            Options->TupleType         = PNM_TUPLE_RGB;
        } else if (UTF8_CompareSubString(TupleTypeString, UTF8String("RGB_ALPHA"), 0, 0) == true) {
            Options->NumChannels       = 4;
            Options->TupleType         = PNM_TUPLE_RGBAlpha;
        }
        free(TupleTypeString);
        /* Read TupleType */
        
        /* Skip ENDHDR */
        BitBuffer_Seek(BitB, 56); // ENDHDR
        /* Skip ENDHDR */
    }
    
    void PNMExtractImage_ASCII(PNMOptions *Options, BitBuffer *BitB, ImageContainer *Image) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        AssertIO(Image != NULL);
        if (Options->BitDepth <= 8) {
            uint8_t ****Array        = (uint8_t****) ImageContainer_GetArray(Image);
            UTF8        Component[4] = {0, 0, 0, 0};
            for (uint64_t Width = 0; Width < Options->Width; Width++) {
                for (uint64_t Height = 0; Height < Options->Height; Height++) {
                    for (uint8_t Channel = 0; Channel < Options->NumChannels; Channel++) {
                        for (uint8_t SubPixelByte = 0; SubPixelByte < 3; SubPixelByte++) {
                            Component[SubPixelByte]      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
                        }
                        Array[0][Width][Height][Channel] = UTF8_String2Integer(Base_Integer | Base_Radix10, Component);
                    }
                }
            }
        } else if (Options->BitDepth <= 16) {
            uint16_t ****Array       = (uint16_t****) ImageContainer_GetArray(Image);
            UTF8        Component[6] = {0, 0, 0, 0, 0, 0};
            for (uint64_t Width = 0; Width < Options->Width; Width++) {
                for (uint64_t Height = 0; Height < Options->Height; Height++) {
                    for (uint8_t Channel = 0; Channel < Options->NumChannels; Channel++) {
                        for (uint8_t SubPixelByte = 0; SubPixelByte < 3; SubPixelByte++) {
                            Component[SubPixelByte]      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
                        }
                        Array[0][Width][Height][Channel] = UTF8_String2Integer(Base_Integer | Base_Radix10, Component);
                    }
                }
            }
        }
    }
    
    void PNMExtractImage_Binary(PNMOptions *Options, BitBuffer *BitB, ImageContainer *Image) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        AssertIO(Image != NULL);
        
        if (Options->BitDepth <= 8) {
            uint8_t ****Array  = (uint8_t****) ImageContainer_GetArray(Image);
            
            for (uint64_t Width = 0; Width < Options->Width; Width++) {
                for (uint64_t Height = 0; Height < Options->Height; Height++) {
                    for (uint8_t Channel = 0; Channel < Options->NumChannels; Channel++) {
                        uint8_t CurrentPixel                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, Options->BitDepth);
                        if (Options->TupleType == PNM_TUPLE_BnW) {
                            Array[0][Width][Height][Channel] = ~CurrentPixel; // 1 = black, 0 = white
                        } else {
                            Array[0][Width][Height][Channel] = CurrentPixel; // 1 = white, 0 = black
                        }
                    }
                }
            }
        } else if (Options->BitDepth <= 16) {
            uint16_t ****Array  = (uint16_t****) ImageContainer_GetArray(Image);
            
            for (uint64_t Width = 0; Width < Options->Width; Width++) {
                for (uint64_t Height = 0; Height < Options->Height; Height++) {
                    for (uint8_t Channel = 0; Channel < Options->NumChannels; Channel++) {
                        uint16_t CurrentPixel                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, Options->BitDepth);
                        if (Options->TupleType == PNM_TUPLE_BnW) {
                            Array[0][Width][Height][Channel] = ~CurrentPixel; // 1 = black, 0 = white
                        } else {
                            Array[0][Width][Height][Channel] = CurrentPixel; // 1 = white, 0 = black
                        }
                    }
                }
            }
        }
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
