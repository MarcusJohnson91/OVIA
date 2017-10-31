#include "../../libPCM.h"

#pragma once

#ifndef LIBPCM_AIFCommon_H
#define LIBPCM_AIFCommon_H

#ifdef __cplusplus
extern "C" {
#endif
    
    void AIFParseMetadata(PCMFile *PCM, BitBuffer *BitB);
    
    uint32_t **AIFExtractSamples(PCMFile *PCM, BitBuffer *BitB, uint64_t Samples2Extract);
    
#ifdef __cplusplus
}
#endif

#endif /* LIBPCM_AIFCommon_H */
