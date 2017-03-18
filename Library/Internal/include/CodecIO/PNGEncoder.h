#include "libModernPNG.h"
#include "PNGTypes.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef struct PNGEncoder EncodePNG;
    
    EncodePNG *InitPNGEncoder(void);
    
#ifdef __cplusplus
}
#endif
