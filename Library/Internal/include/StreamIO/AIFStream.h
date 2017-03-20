#include "libPCM.h"

#ifndef LIBPCM_AIFCOMMON_H
#define LIBPCM_AIFCOMMON_H

#pragma once

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef struct AIFHeader {
        uint16_t     Channels;
        uint32_t     NumFrames; // More accurately described as number of channel agnostic samples.
        uint16_t     BitDepth; // Samples are padded to the next complete byte
        
        /* SNSD */
        uint32_t     Offset;
        uint32_t     BlockSize;
        
        PCMMetadata *Meta;
    };
    
    enum AIFChunkIDs {
        AIF_FORM = 0x464F524D,
        AIF_AIFF = 0x41494646,
        AIF_AIFC = 0x41494643,
        AIF_NAME = 0x4E414D45,
        AIF_COMM = 0x434F4D4D,
        AIF_SSND = 0x53534E44,
    };
    
#ifdef __cplusplus
}
#endif

#endif /* LIBPCM_AIFCOMMON_H */
