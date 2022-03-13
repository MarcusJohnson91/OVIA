/*!
 @header              Flate.h
 @author              Marcus Johnson
 @copyright           2022+
 @version             1.0.0
 @brief               This header contains code for a Deflate/Inflate entropy coder.
 */

#pragma once

#ifndef OVIA_EntropyIO_Flate_H
#define OVIA_EntropyIO_Flate_H

#include "../../../OVIA/include/EntropyIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    typedef enum Flate_FLAC_BlockTypes {
        BlockType_Literal = 0,
        BlockType_Fixed   = 1,
        BlockType_Dynamic = 2,
        BlockType_Invalid = 3,
    } Flate_FLAC_BlockTypes;

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_EntropyIO_Flate_H */
