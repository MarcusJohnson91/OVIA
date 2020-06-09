#include "OVIACommon.h"

#pragma once

#ifndef OVIA_ID3Common_H
#define OVIA_ID3Common_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    typedef struct ID3 {
        UTF32 *Title;
        UTF32 *Artist;
        UTF32 *Album;
    } ID3;
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_ID3Common_H */
