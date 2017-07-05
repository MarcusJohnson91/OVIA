#include "libPCM.h"

#pragma once

#ifndef LIBPCM_PORTABLEBITMAPCOMMON_H
#define LIBPCM_PORTABLEBITMAPCOMMON_H

#ifdef __cplusplus
extern "C" {
#endif
    
    enum PortableBitmapMagic {
        PortableBitMap  = 0x5031, // P1
        PortableGrayMap = 0x5032, // P2
        PortablePixMap  = 0x5033, // P3
        
    };
    
    // Supports both Portable Bitmats, and Portable Arbitrary Map formats.
    
#ifdef __cplusplus
}
#endif

#endif  LIBPCM_PORTABLEBITMAPCOMMON_H
