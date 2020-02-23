#include "OVIACommon.h"

#pragma once

#ifndef OVIA_ID3Common_H
#define OVIA_ID3Common_H

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef struct ID3 {
        UTF32 *Title;
        UTF32 *Artist;
        UTF32 *Album;
    } ID3;
    
#ifdef __cplusplus
}
#endif

#endif /* OVIA_ID3Common_H */
