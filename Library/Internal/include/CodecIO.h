/*!
 @header              CodecIO.h
 @author              Marcus Johnson
 @copyright           2020+
 @version             1.0.0
 @brief               This header contains code for codecs (encoding and decoding).
 */

#include "Private/MediaIO.h"
#include "../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../Dependencies/FoundationIO/Library/include/MathIO.h"
#include "../../Dependencies/FoundationIO/Library/include/TextIO/LogIO.h"

#pragma once

#ifndef OVIA_CodecIO_H
#define OVIA_CodecIO_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef struct CodecIO_MIMETypes {
        const uint8_t                NumMIMETypes;
        PlatformIO_Immutable(UTF32*) MIMETypes[];
    } CodecIO_MIMETypes;

    typedef struct CodecIO_Extension {
        const uint8_t                Size;
        PlatformIO_Immutable(UTF32*) Extension;
    } CodecIO_Extension;

    typedef struct CodecIO_Extensions {
        const uint8_t           NumExtensions;
        const CodecIO_Extension Extensions[];
    } CodecIO_Extensions;

    typedef struct CodecIO_Signature {
        const uint64_t OffsetInBits;
        const uint8_t  SizeInBits;
        const uint8_t  *Signature;
    } CodecIO_Signature;

    typedef struct CodecIO_MagicIDs {
        const uint64_t          NumMagicIDs;
        const CodecIO_Signature MagicIDs[];
    } CodecIO_MagicIDs;


    typedef struct CodecIO_FileSignature {
        const uint64_t               OffsetInBits;
        const uint8_t                SizeInBits;
        const uint8_t                NumSignatures;
        const uint8_t               *Signature[];
    } CodecIO_FileSignature;

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
        const CodecIO_MagicIDs *MagicIDs;
        
    } CodecIO_Encoder;

    typedef struct CodecIO_Decoder {
        FunctionPointer_Init   Function_Initalize;
        FunctionPointer_Body   Function_Parse;
        FunctionPointer_Media  Function_Media;
        FunctionPointer_Deinit Function_Deinitalize;
    } CodecIO_Decoder;

    extern uint64_t        NumEncoders;
    extern CodecIO_Encoder GlobalEncoders[];

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

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_CodecIO_H */
