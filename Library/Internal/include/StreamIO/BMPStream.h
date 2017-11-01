#include "../../libPCM.h"

#pragma once

#ifndef LIBPCM_BMPCommon_H
#define LIBPCM_BMPCommon_H

#ifdef __cplusplus
extern "C" {
#endif
    
    void       BMPParseMetadata(PCMFile *PCM, BitBuffer *BitB);
    
    uint16_t **BMPExtractPixels(PCMFile *PCM, BitBuffer *BitB, uint64_t NumPixels2Extract);
    
    void       BMPInsertPixels(PCMFile *PCM, BitBuffer *OutputPixels, uint32_t NumPixels2Write, uint16_t **Pixels2Write);

#ifdef __cplusplus
}
#endif

#endif /* LIBPCM_BMPCommon_H */
