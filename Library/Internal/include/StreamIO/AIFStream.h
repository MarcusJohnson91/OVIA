#include "../../libPCM.h"

#pragma once

#ifndef LIBPCM_AIFCommon_H
#define LIBPCM_AIFCommon_H

#ifdef __cplusplus
extern "C" {
#endif
    
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
    
    void AIFParseMetadata(PCMFile *PCM, BitBuffer *BitB);
    
    void AIFExtractSamples(PCMFile *PCM, BitBuffer *BitB, uint64_t NumSamples2Extract, uint32_t **ExtractedSamples);
    
    void AIFInsertSamples(PCMFile *PCM, BitBuffer *OutputSamples, uint32_t NumSamples2Write, uint32_t **Samples2Write);
    
#ifdef __cplusplus
}
#endif

#endif /* LIBPCM_AIFCommon_H */
