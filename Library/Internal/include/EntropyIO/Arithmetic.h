#pragma once

#ifndef OVIA_EntropyIO_Arithmetic_h
#define OVIA_EntropyIO_Arithmetic_h

#include "EntropyIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef struct Arithmetic {
        uint8_t  TableType;
        uint8_t  TableID;
        uint8_t  CodeValue;
    } Arithmetic;

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_EntropyIO_Arithmetic_h */
