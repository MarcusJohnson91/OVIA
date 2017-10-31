#include "../../libPCM.h"

// PGM, PBM, PPM, along with PAM.

#pragma once

#ifndef LIBPCM_PXMCommon_H
#define LIBPCM_PXMCommon_H

#ifdef __cplusplus
extern "C" {
#endif
    
    void       IdentifyPXMFileType(PCMFile *PCM, BitBuffer *BitB);
    
    void       ParsePXMHeader(PCMFile *PCM, BitBuffer *BitB);
    
    uint16_t **PXMExtractPixels(PCMFile *PCM, BitBuffer *BitB, uint64_t NumPixels2Read);
    
#ifdef __cplusplus
}
#endif

#endif /* LIBPCM_PXMCommon_H */
