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

    
    
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_AIFCommon_H */
