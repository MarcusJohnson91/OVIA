#include "../../libPCM.h"

// PGM, PBM, PPM, along with PAM.

#pragma once

#ifndef LIBPCM_PXMCommon_H
#define LIBPCM_PXMCommon_H

#ifdef __cplusplus
extern "C" {
#endif
    
    struct PXMHeader {
        uint64_t Width;
        uint64_t Height;
        uint64_t NumChannels;
        uint64_t MaxVal;
    };
    
    // The secton ender for all variants is 0x0A
    
#ifdef __cplusplus
}
#endif

#endif /* LIBPCM_PXMCommon_H */
