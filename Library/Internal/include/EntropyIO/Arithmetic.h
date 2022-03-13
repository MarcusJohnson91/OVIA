/*!
 @header              Arithmetic.h
 @author              Marcus Johnson
 @copyright           2022+
 @version             1.0.0
 @brief               This header contains code for an Arithmetic entropy coder.
 */

#pragma once

#ifndef OVIA_EntropyIO_Arithmetic_H
#define OVIA_EntropyIO_Arithmetic_H

#include "../../../OVIA/include/EntropyIO.h"

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

#endif /* OVIA_EntropyIO_Arithmetic_H */
