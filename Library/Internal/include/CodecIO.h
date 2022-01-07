/*!
 @header              CodecIO.h
 @author              Marcus Johnson
 @copyright           2020+
 @version             1.0.0
 @brief               This header contains code for codecs (encoding and decoding).
 */

#pragma once

#ifndef OVIA_CodecIO_H
#define OVIA_CodecIO_H

#include "../MediaIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/TextIO/LogIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef struct CodecIO_ImageChannelConfig {
        const uint8_t           NumChannels;
        const Image_ChannelMask Channels[];
    } CodecIO_ImageChannelConfig;

    typedef struct CodecIO_ImageLimitations {
        const uint64_t                                    MaxWidth;
        const uint64_t                                    MaxHeight;
        const uint8_t                                     MaxBitDepth;
        PlatformIO_Immutable(CodecIO_ImageChannelConfig*) ChannelConfigs;
    } CodecIO_ImageLimitations;

    typedef struct CodecIO_AudioLimitations {
        const uint64_t           MaxSampleRate;
        const uint8_t            MaxBitDepth;
        const uint8_t            MaxNumChannels;
        const Audio_ChannelMask  SupportedChannels;
    } CodecIO_AudioLimitations;

    typedef void *(*FunctionPointer_Init)(void);   // Returns Options as void pointer

    typedef void  (*FunctionPointer_Deinit)(void *Options2Deinit);

    typedef void  (*FunctionPointer_Body)(void *Options, BitBuffer *BitB);

    typedef void  (*FunctionPointer_Media)(void *Options, BitBuffer *BitB, void *Container);

    typedef struct CodecIO_Encoder {
        FunctionPointer_Init    Function_Initalize;
        FunctionPointer_Body    Function_Parse;
        FunctionPointer_Media   Function_Media;
        FunctionPointer_Deinit  Function_Deinitalize;
        const OVIA_MagicIDs    *MagicIDs;
    } CodecIO_Encoder;

    typedef struct CodecIO_Decoder {
        FunctionPointer_Init   Function_Initalize;
        FunctionPointer_Body   Function_Parse;
        FunctionPointer_Media  Function_Media;
        FunctionPointer_Deinit Function_Deinitalize;
    } CodecIO_Decoder;
  
  typedef struct CodecIO_RegisteredCodecs {
      const uint64_t   NumEncoders;
      const uint64_t   NumDecoders;
      CodecIO_Encoder *Encoders;
      CodecIO_Decoder *Decoders;
  } CodecIO_RegisteredCodecs;
    
    extern CodecIO_RegisteredCodecs CodecRegistry;

    extern uint64_t        NumEncoders;
    extern uint64_t        NumDecoders;
    extern CodecIO_Encoder GlobalEncoders[];
    extern CodecIO_Decoder GlobalDecoders[];

    /*
     What information is needed to register a Codec?
     We need to know the Codecs limitations.
     We need to know the Codecs Type.
     Encoders we need to know their mime types and extensions and so on; this might need to be a StreamIO thing tho.
     */
    /*
     Let's just start at the top.

     A file is opened, OVIA is asked to identify this file's type.

     ovia goes through all registered codec file signatures and compare them with with opening bytes of the file.

     OVIA finds 1 or more matches for the StreamType, which may contain one or more sub-types contained within the stream.

     We need to know how to initalize the encoder and decoder, parsing functions, header reading/writing, etc
     */

    typedef struct CodecIO_CodecRegistry {
        uint64_t NumCodecs;

    } CodecIO_CodecRegistry;

#ifndef CodecIO_Internal_NumEncoders
#define CodecIO_Internal_NumEncoders 0
#endif /* CodecIO_Internal_NumEncoders */

#ifndef CodecIO_Internal_NumDecoders
#define CodecIO_Internal_NumDecoders 0
#endif /* CodecIO_Internal_NumDecoders */

#ifndef CodecIO_Internal_EncoderList
#define CodecIO_Internal_EncoderList
#endif /* CodecIO_Internal_EncoderList */

#ifndef CodecIO_Internal_DecoderList
#define CodecIO_Internal_DecoderList
#endif /* CodecIO_Internal_DecoderList */

    /*!
     @param CodecType    Encoder or Decoder?
     @param CodecStruct  CodecIO_Encoder/CodecIO_Decoder
     */
#ifndef CodecIO_Register
#define CodecIO_Register(CodecType, CodecStruct) \
/* Num */
#endif /* CodecIO_Register */

#ifndef CodecIO_RegisterEncoder
#define CodecIO_RegisterEncoder(CodecStruct) \
/* Increment CodecIO_Internal_NumEncoders */ \
_Redefine CodecIO_Internal_NumEncoders CodecIO_Internal_NumEncoders + 1 \
_Pragma("push_macro(\"CodecIO_Internal_EncoderList\")") \
_Redefine CodecIO_Internal_EncoderList CodecStruct
#endif /* CodecIO_RegisterEncoder */

#ifndef CodecIO_RegisterDecoder
#define CodecIO_RegisterDecoder(CodecStruct) \
/* Increment CodecIO_Internal_NumDecoders */ \
_Redefine CodecIO_Internal_NumDecoders CodecIO_Internal_NumDecoders + 1 \
_Pragma("push_macro(\"CodecIO_Internal_DecoderList\")") \
_Redefine CodecIO_Internal_DecoderList CodecStruct
#endif /* CodecIO_RegisterDecoder */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_CodecIO_H */
