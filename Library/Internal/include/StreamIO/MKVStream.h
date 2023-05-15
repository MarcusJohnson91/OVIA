/*!
 @header              MKVStream.h
 @author              Marcus Johnson
 @copyright           2022+
 @SPDX-License-Identifier: Apache-2.0
 @version             1.0.0
 @brief               This header contains code for MKV/WebM streams.
 */

#pragma once

#ifndef OVIA_StreamIO_MKVStream_H
#define OVIA_StreamIO_MKVStream_H

#include "../../../OVIA/include/StreamIO.h"
#include "../../../OVIA/include/TagIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    typedef struct MKVOptions {
        UTF8           *Tags;
        TagIO_TagTypes *TagTypes;
        uint64_t        NumTags;
        uint64_t        SampleRate;
        uint64_t        SampleRate_Mantissa;
        uint32_t        FileSize;
        uint32_t        NumSamples;
        uint32_t        SampleOffset;
        uint32_t        BlockSize;
        int16_t         SampleRate_Exponent;
        uint16_t        NumChannels;
        uint16_t        BitDepth;
    } MKVOptions;
    
    MKVOptions       *MKVOptions_Init(void);
    
    void              MKVOptions_Deinit(MKVOptions *Options);

#ifdef OVIA_StreamIO_MKV
    extern const OVIA_MagicIDs MKVSignature;
    
    const OVIA_MagicIDs MKVSignature = {
        .NumMagicIDs          = 2,
        .MagicIDs             = {
            OVIA_RegisterSignature(0, 0, 32, (uint8_t[4]) {0x1A, 0x45, 0xDF, 0xA3})
        }
    };
    
    extern const OVIA_Extensions MKVExtensions;
    
    const OVIA_Extensions MKVExtensions = {
        .NumExtensions     = 5,
        .Extensions        = {
            OVIA_RegisterExtension("mk3d")
            OVIA_RegisterExtension("mkv")
            OVIA_RegisterExtension("mka")
            OVIA_RegisterExtension("mks")
            OVIA_RegisterExtension("webm")
        }
    };

    extern const OVIA_MIMETypes MKVMIMETypes;
    
    const OVIA_MIMETypes MKVMIMETypes = {
        .NumMIMETypes     = 6,
        .MIMETypes        = {
            OVIA_RegisterMIMEType("video/mkv")
            OVIA_RegisterMIMEType("video/webm")
            OVIA_RegisterMIMEType("audio/webm")
            OVIA_RegisterMIMEType("audio/x-matroska")
            OVIA_RegisterMIMEType("video/x-matroska")
            OVIA_RegisterMIMEType("application/x-matroska")
        }
    };
    
#if defined(OVIA_StreamIO_Encode)
    extern const OVIA_Stream MKVMuxer;
    
    const OVIA_Stream MKVMuxer = {
        .MediaType             = MediaType_Container,
        .MagicID               = &MKVSignature,
        .Function_Initialize   = MKVOptions_Init,
        .Function_Parse        = MKVParseMetadata,
        .Function_Decode       = MKVExtractSamples,
        .Function_Deinitialize = MKVOptions_Deinit,
    };
#endif /* OVIA_StreamIO_Encode */
    
#if defined(OVIA_StreamIO_Decode)
    extern const StreamIO_Demuxer MKVDemuxer;
    
    const StreamIO_Demuxer MKVDemuxer = {
        .MediaType             = MediaType_Container,
        .MagicID               = &MKVSignature,
        .Function_Initialize   = MKVOptions_Init,
        .Function_Parse        = MKVParseMetadata,
        .Function_Decode       = MKVExtractSamples,
        .Function_Deinitialize = MKVOptions_Deinit,
    };
#endif /* OVIA_StreamIO_Decode */
    
#endif /* OVIA_StreamIO_MKV */
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_StreamIO_MKVStream_H */

