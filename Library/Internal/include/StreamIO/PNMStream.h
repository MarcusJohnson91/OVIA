#include "libPCM.h"

// PGM, PBM, PPM, along with PAM.

#pragma once

#ifndef LIBPCM_PORTABLEBITMAPCOMMON_H
#define LIBPCM_PORTABLEBITMAPCOMMON_H

#ifdef __cplusplus
extern "C" {
#endif
    
    enum PXMMagic {
        PortableBitMap1  = 0x5031, // P1 or P4
        PortableBitMap2  = 0x5034,
        PortableGrayMap1 = 0x5032, // P2 or P5
        PortableGrayMap2 = 0x5035,
        PortablePixMap1  = 0x5033, // P3 or P6
        PortablePixMap2  = 0x5036,
        PortableAnyMap   = 0x5037, // P7
    };
    
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

#endif  LIBPCM_PORTABLEBITMAPCOMMON_H
