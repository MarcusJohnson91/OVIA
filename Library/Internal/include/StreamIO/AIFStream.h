/*!
 @header              AIFCommon.h
 @author              Marcus Johnson
 @copyright           2017+
 @version             1.0.0
 @brief               This header contains code for reading and writing lossless AIF audio files
 */

#include "OVIACommon.h"
#include "StreamIO.h"

#pragma once

#ifndef OVIA_AIFCommon_H
#define OVIA_AIFCommon_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    typedef enum AIFSpeakerMask {
        AIFFrontLeft          = 0x1,
        AIFFrontRight         = 0x2,
        AIFFrontCenter        = 0x4,
        AIFRearLeft           = 0x8,
        AIFRearRight          = 0x10,
        AIFSurround           = 0x20,
        AIFLeftCenter         = 0x40,
        AIFRightCenter        = 0x80,
    } AIFSpeakerMask;
    
    typedef enum AIFTimeStamp {
        AIFFSecondsBeforeUNIX = 2082844800,
    } AIFTimeStamp;
    
    typedef enum AIFChunkIDs {
        AIF_FORM              = 0x464F524D,
        AIF_AIFF              = 0x41494646,
        AIF_AIFC              = 0x41494643,
    } AIFChunkIDs;
    
    typedef enum AIFSubChunkIDs {
        AIF_AAPL              = 0x4150504c,
        AIF_AESD              = 0x41455344,
        AIF_ANNO              = 0x414e4e4f,
        AIF_AUTH              = 0x41555448,
        AIF_COMM              = 0x434F4D4D,
        AIF_COMT              = 0x434f4d54,
        AIF_ID3               = 0x49443320,
        AIF_INST              = 0x494e5354,
        AIF_MARK              = 0x4d41524b,
        AIF_MIDI              = 0x4d494449,
        AIF_NAME              = 0x4E414D45,
        AIF_SSND              = 0x53534E44,
    } AIFSubChunkIDs;
    
    typedef struct AIFOptions {
        UTF8          *Tags;
        OVIA_TagTypes *TagTypes;
        uint64_t       NumTags;
        uint64_t       SampleRate;
        uint64_t       SampleRate_Mantissa;
        uint32_t       FileSize;
        uint32_t       NumSamples;
        uint32_t       SampleOffset;
        uint32_t       BlockSize;
        int16_t        SampleRate_Exponent;
        uint16_t       NumChannels;
        uint16_t       BitDepth;
    } AIFOptions;
    
    void             *AIFOptions_Init(void);
    
    void              AIFSkipPadding(BitBuffer *BitB, uint32_t SubChunkSize);
    
    void              AIFReadMetadata(void *Options, BitBuffer *BitB);
    
    void             *AIFExtractSamples(void *Options, BitBuffer *BitB);
    
    void              AIFAppendSamples(void *Options, void *Container, BitBuffer *BitB);
    
    void              AIFWriteHeader(void *Options, BitBuffer *BitB);
    
    void              AIFOptions_Deinit(void *Options);

    static const OVIA_Extensions AIFExtensions = {
        .NumExtensions = 3,
        .Extensions    = {
            [0]        = UTF32String("aifc"),
            [1]        = UTF32String("aiff"),
            [2]        = UTF32String("aif"),
        },
    };

    static const OVIA_MIMETypes AIFMIMETypes = {
        .NumMIMETypes = 2,
        .MIMETypes    = {
            [0]       = UTF32String("audio/aiff"),
            [1]       = UTF32String("audio/x-aiff"),
        },
    };

    static const OVIA_MagicIDs AIFMagicIDs = {
        .NumMagicIDs         = 1,
        .MagicIDOffsetInBits = 0,
        .MagicIDSizeInBits   = 32,
        .MagicIDNumber = {
            [0] = (uint8_t[4]){0x46, 0x4F, 0x52, 0x4D},
        },
    };
    
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_AIFCommon_H */
