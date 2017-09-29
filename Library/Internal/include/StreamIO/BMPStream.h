#include "../../libPCM.h"

#pragma once

#ifndef LIBPCM_BMPCommon_H
#define LIBPCM_BMPCommon_H

#ifdef __cplusplus
extern "C" {
#endif
    
    struct BMPHeader {
        uint16_t Magic;
        uint32_t FileSize;
        uint16_t Reserved1;
        uint16_t Reserved2;
        uint32_t DataStart;
    };

#ifdef __cplusplus
}
#endif

#endif /* LIBPCM_BMPCommon_H */
