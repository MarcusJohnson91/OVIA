/*!
 @header              CodecIO.h
 @author              Marcus Johnson
 @copyright           2020+
 @SPDX-License-Identifier: Apache-2.0
 @version             1.0.0
 @brief               This header contains code for codecs (encoding and decoding).
 */

#pragma once

#ifndef OVIA_CodecIO_H
#define OVIA_CodecIO_H

#include "OVIATypes.h"
#include "MediaIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef struct CodecIO_ImageChannelConfig {
        const uint8_t           NumChannels;
        const MediaIO_ImageMask Channels;
    } CodecIO_ImageChannelConfig;

    typedef struct CodecIO_ImageLimitations {
        const uint64_t                                    MaxWidth;
        const uint64_t                                    MaxHeight;
        const uint8_t                                     MaxBitDepth;
        const uint8_t                                     MaxNumViews;
        PlatformIO_Immutable(CodecIO_ImageChannelConfig*) ChannelConfigs;
    } CodecIO_ImageLimitations;

    typedef struct CodecIO_AudioLimitations {
        const uint64_t           MaxSampleRate;
        const uint8_t            MaxBitDepth;
        const uint8_t            MaxNumChannels;
        const MediaIO_AudioMask  SupportedChannels;
    } CodecIO_AudioLimitations;

    typedef struct CodecIO_Encoder {
        OVIA_Function_Init    Function_Initalize;
        OVIA_Function_Parse   Function_Parse;
        OVIA_Function_Coder   Function_Media;
        OVIA_Function_Deinit  Function_Deinitalize;
        const OVIA_MagicIDs  *MagicIDs;
    } CodecIO_Encoder;

    typedef struct CodecIO_Decoder {
        OVIA_Function_Init   Function_Initalize;
        OVIA_Function_Parse  Function_Parse;
        OVIA_Function_Coder  Function_Media;
        OVIA_Function_Deinit Function_Deinitalize;
    } CodecIO_Decoder;

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
/*
#ifndef CodecIO_RegisterEncoder
#define CodecIO_RegisterEncoder(CodecStruct) \
_Pragma("redefine_macro(\"CodecIO_Internal_NumEncoders CodecIO_Internal_NumEncoders + 1\")") \
_Pragma("push_macro(\"CodecIO_Internal_EncoderList\")") \
_Pragma("redefine_macro(\"CodecIO_Internal_EncoderList CodecStruct\")") \
#endif */ /* CodecIO_RegisterEncoder */

/*
#ifndef CodecIO_RegisterDecoder
#define CodecIO_RegisterDecoder(CodecStruct) \
_Pragma("redefine_macro(\"CodecIO_Internal_NumDecoders CodecIO_Internal_NumDecoders + 1\")") \
_Pragma("push_macro(\"CodecIO_Internal_DecoderList\")") \
_Pragma("redefine_macro(\"CodecIO_Internal_DecoderList CodecStruct\")") \
#endif */ /* CodecIO_RegisterDecoder */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_CodecIO_H */
