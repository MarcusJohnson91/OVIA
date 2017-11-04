#include "../../libPCM.h"

// PGM, PBM, PPM, along with PAM.

#pragma once

#ifndef LIBPCM_PXMCommon_H
#define LIBPCM_PXMCommon_H

#ifdef __cplusplus
extern "C" {
#endif
    
    void       PXMIdentifyFileType(PCMFile *PCM, BitBuffer *BitB);
    
    void       PXMParseMetadata(PCMFile *PCM, BitBuffer *BitB);
    
    uint16_t **PXMExtractPixels(PCMFile *PCM, BitBuffer *BitB, uint64_t NumPixels2Read);
    
    void       PXMInsertPixels(PCMFile *PCM, BitBuffer *OutputPixels, uint32_t NumPixels2Write, uint16_t **Pixels2Write);
    
#ifdef __cplusplus
}
#endif

#endif /* LIBPCM_PXMCommon_H */
