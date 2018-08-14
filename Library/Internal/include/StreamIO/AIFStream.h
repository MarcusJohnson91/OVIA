#include "../../libOVIA.h"

#include "../../../../Dependencies/FoundationIO/libFoundationIO/include/BitIO.h"
#include "../../../../Dependencies/FoundationIO/libFoundationIO/include/Log.h"
#include "../../../../Dependencies/FoundationIO/libFoundationIO/include/Math.h"
#include "../../../../Dependencies/FoundationIO/libFoundationIO/include/ContainerIO.h"

#pragma once

#ifndef OVIA_AIFCommon_H
#define OVIA_AIFCommon_H

#ifdef __cplusplus
extern "C" {
#endif
    
    enum AIFSpeakerMask {
        AIFFrontLeft          = 0x1,
        AIFFrontRight         = 0x2,
        AIFFrontCenter        = 0x4,
        AIFRearLeft           = 0x8,
        AIFRearRight          = 0x10,
        AIFSurround           = 0x20,
        AIFLeftCenter         = 0x40,
        AIFRightCenter        = 0x80,
    };
    
    enum AIFFTimeStamp {
        AIFFSecondsBeforeUNIX = 2082844800,
    };
    
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
    
    void AIFSkipPadding(BitBuffer *BitB, uint32_t SubChunkSize);
    
    void AIFParseMetadata(OVIA *Ovia, BitBuffer *BitB);
    
    void AIFExtractSamples(OVIA *Ovia, BitBuffer *BitB, uint64_t NumSamples2Extract, uint32_t **ExtractedSamples);
    
    void AIFInsertSamples(OVIA *Ovia, AudioContainer *Audio, BitBuffer *BitB);
    
    void AIFWriteHeader(OVIA *Ovia, BitBuffer *BitB);
    
#ifdef __cplusplus
}
#endif

#endif /* OVIA_AIFCommon_H */
