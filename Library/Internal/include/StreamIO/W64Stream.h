/*!
 @header              W64Stream.h
 @author              Marcus Johnson
 @copyright           2020+
 @SPDX-License-Identifier: Apache-2.0
 @version             1.0.0
 @brief               This header contains code for W64 streams.
 */

#pragma once

#ifndef OVIA_StreamIO_W64Stream_H
#define OVIA_StreamIO_W64Stream_H

#include "../../../OVIA/include/StreamIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/GUUID.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef struct W64Options {
        uint32_t SpeakerMask;
        uint32_t SampleRate;
        uint32_t ByteRate;
        uint32_t BlockAlign;
        uint16_t CompressionFormat;
        uint16_t BlockAlignment;
        uint16_t NumChannels;
        uint16_t BitDepth;
        uint16_t ValidBitsPerSample;
    } W64Options;

    static const uint8_t W64_RIFF_GUIDString[GUUIDString_Size] = {
        0x72,0x69,0x66,0x66,0x2D,0x2E,0x91,0x2D,0xCF,0x11,0x2D,0xA5,0xD6,0x2D,0x28,0xDB,0x04,0xC1,0x00,0x00
    };

    static const uint8_t W64_WAVE_GUIDString[GUUIDString_Size] = {
        0x77,0x61,0x76,0x65,0x2D,0xF3,0xAC,0x2D,0xD3,0x11,0x2D,0x8C,0xD1,0x2D,0x00,0xC0,0x4F,0x8E,0xDB,0x8A
    };

    static const uint8_t W64_FMT_GUIDString[GUUIDString_Size] = {
        0x66,0x6D,0x74,0x20,0x2D,0xF3,0xAC,0x2D,0xD3,0x11,0x2D,0x8C,0xD1,0x2D,0x00,0xC0,0x4F,0x8E,0xDB,0x8A
    };

    static const uint8_t W64_DATA_GUIDString[GUUIDString_Size] = {
        0x64,0x61,0x74,0x61,0x2D,0xF3,0xAC,0x2D,0xD3,0x11,0x2D,0x8C,0xD1,0x2D,0x00,0xC0,0x4F,0x8E,0xDB,0x8A
    };

    static const uint8_t W64_LEVL_GUIDString[GUUIDString_Size] = { // aka Peak Envelope Chunk
        0x6A,0x75,0x6E,0x6B,0x2D,0xF3,0xAC,0x2D,0xD3,0x11,0x2D,0x8C,0xD1,0x2D,0x00,0xC0,0x4F,0x8E,0xDB,0x8A
    };

    static const uint8_t JUNK_UUID[GUUIDString_Size] = {
        0x6B,0x6E,0x75,0x6A,0x45,0xAC,0xF3,0x45,0x11,0xD3,0x45,0x8C,0xD1,0x45,0x00,0xC0,0x4F,0x8E,0xDB,0x8A};

    static const uint8_t W64_BEXT_GUIDString[GUUIDString_Size] = {
        0x62,0x65,0x78,0x74,0x2D,0xf3,0xAC,0x2D,0xD3,0xAA,0x2D,0xD1,0x8C,0x2D,0x00,0xC0,0x4F,0x8E,0xDB,0x8A
    };

    static const uint8_t W64_FACT_GUIDString[GUUIDString_Size] = {
        0x66,0x61,0x63,0x74,0x2D,0xF3,0xAC,0x2D,0xD3,0x11,0x2D,0x8C,0xD1,0x2D,0x00,0xC0,0x4F,0x8E,0xDB,0x8A
    };

    static const uint8_t W64_JUNK_GUIDString[GUUIDString_Size] = {
        0x6B,0x6E,0x75,0x6A,0x2D,0xAC,0xF3,0x2D,0x11,0xD3,0x2D,0x8C,0xD1,0x2D,0x00,0xC0,0x4F,0x8E,0xDB,0x8A
    };

    static const uint8_t W64_MRKR_GUIDString[GUUIDString_Size] = { // MARKER
        0xAB,0xF7,0x62,0x56,0x2D,0x39,0x45,0x2D,0x11,0xD2,0x2D,0x86,0xC7,0x2D,0x00,0xC0,0x4F,0x8E,0xDB,0x8A
    };

    static const uint8_t W64_SUMM_GUIDString[GUUIDString_Size] = { // SUMMARY LIST
        0x92,0x5F,0x94,0xBC,0x2D,0x52,0x5A,0x2D,0x11,0xD2,0x2D,0x86,0xDC,0x2D,0x00,0xC0,0x4F,0x8E,0xDB,0x8A
    };

    static const uint8_t W64_LIST_GUIDString[GUUIDString_Size] = {
        0x74,0x73,0x69,0x6C,0x2D,0x91,0x2F,0x2D,0x11,0xCF,0x2D,0xA5,0xD6,0x2D,0x28,0xDB,0x04,0xC1,0x00,0x00
    };

    uint64_t CalculateW64ByteRate(uint64_t NumChannels, uint8_t BitDepth, uint64_t SampleRate);

    uint64_t CalculateW64BlockAlign(uint64_t NumChannels, uint8_t BitDepth);

#ifdef OVIA_StreamIO_W64
    extern const OVIA_MagicIDs W64Signature;

    const OVIA_MagicIDs W64Signature = {
        .NumMagicIDs          = 1,
        .MagicIDs             = {
            OVIA_RegisterSignature(0, 128, &W64_RIFF_GUIDString)
        },
    };

    extern const OVIA_Extensions W64Extensions;

    const OVIA_Extensions W64Extensions = {
        .NumExtensions     = 1,
        .Extensions        = {
            OVIA_RegisterExtension("w64")
        }
    };

    extern const OVIA_MIMETypes W64MIMETypes;

    const OVIA_MIMETypes W64MIMETypes = {
        .NumMIMETypes     = 2,
        .MIMETypes        = {
            OVIA_RegisterMIMEType("audio/w64")
            OVIA_RegisterMIMEType("audio/x-w64")
        }
    };

#if defined(OVIA_StreamIO_Encode)
    extern const OVIA_Stream W64Muxer;

    const OVIA_Stream W64Muxer = {
        .MediaType             = MediaType_Audio2D,
        .MagicID               = &W64Signature,
        .Function_Initialize   = W64Options_Init,
        .Function_Parse        = W64ParseMetadata,
        .Function_Decode       = W64ExtractSamples,
        .Function_Deinitialize = W64Options_Deinit,
    };
#endif /* OVIA_StreamIO_Encode */

#if defined(OVIA_StreamIO_Decode)
    extern const StreamIO_Demuxer W64Demuxer;

    const StreamIO_Demuxer W64Demuxer = {
        .MediaType             = MediaType_Audio2D,
        .MagicID               = &W64Signature,
        .Function_Initialize   = W64Options_Init,
        .Function_Parse        = W64ParseMetadata,
        .Function_Decode       = W64ExtractSamples,
        .Function_Deinitialize = W64Options_Deinit,
    };
#endif /* OVIA_StreamIO_Decode */

#endif /* OVIA_StreamIO_W64 */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_StreamIO_W64Stream_H */

