/*!
 @header              AIFStream.h
 @author              Marcus Johnson
 @copyright           2020+
 @version             1.0.0
 @brief               This header contains code for AIF streams.
 */

#include "StreamIO.h"
#include "TagIO.h"

#pragma once

#ifndef OVIA_StreamIO_AIFStream_H
#define OVIA_StreamIO_AIFStream_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    typedef struct TagIO_Tags TagIO_Tags;
    
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
    } AIFOptions;
    
    void             *AIFOptions_Init(void);
    
    void              AIFSkipPadding(BitBuffer *BitB, uint32_t SubChunkSize);
    
    void              AIFParseMetadata(AIFOptions *AIF, BitBuffer *BitB);
    
    void              AIFExtractSamples(AIFOptions *AIF, BitBuffer *BitB, Audio2DContainer *Audio);
    
    void              AIFAppendSamples(AIFOptions *AIF, BitBuffer *BitB, Audio2DContainer *Audio);
    
    void              AIFWriteHeader(AIFOptions *AIF, BitBuffer *BitB);
    
    void              AIFOptions_Deinit(void *Options);

#ifdef OVIA_StreamIO_AIF
    extern OVIA_MagicIDs AIFSignature = {
        .NumMagicIDs         = 1,
        .MagicIDOffsetInBits = 0,
        .MagicIDSizeInBits   = 32,
        .MagicIDNumber       = {
            [0]              = (uint8_t[4]){0x46, 0x4F, 0x52, 0x4D},
        }
    };

    extern const OVIA_Extensions AIFExtensions = {
        .NumExtensions = 3,
        .Extensions    = {
            [0]        = UTF32String("aifc"),
            [1]        = UTF32String("aiff"),
            [2]        = UTF32String("aif"),
        },
    };

    extern const OVIA_MIMETypes AIFMIMETypes = {
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
            [0]              = (uint8_t[4]){0x46, 0x4F, 0x52, 0x4D},
        },
    };
#endif /* OVIA_StreamIO_AIF */
    
#if defined(OVIA_StreamIO_Encode) && defined(OVIA_StreamIO_AIF)
    extern OVIA_Stream AIFEncoder = {
        .MagicID = &AIFMagicIDs,
    };
#endif /* OVIA_StreamIO_Encode && OVIA_StreamIO_AIF */
    
#if defined(OVIA_StreamIO_Decode) && defined(OVIA_StreamIO_AIF)
    extern OVIA_Stream AIFDecoder = {
        .MagicID = &AIFMagicIDs,
    };
#endif /* OVIA_StreamIO_Decode && OVIA_StreamIO_AIF */
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_StreamIO_AIFStream_H */

