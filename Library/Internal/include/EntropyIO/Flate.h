/*!
 @header              Flate.h
 @author              Marcus Johnson
 @copyright           2017+
 @SPDX-License-Identifier: Apache-2.0
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

    typedef enum FlateConstants {
        MaxCodes                   = 288,
        MaxLiteralLengthCodes      = 286,
        MaxDistanceCodes           = 30,
        NumMetaCodes               = 19,
        MaxBitsPerSymbol           = 15,
        EndOfBlock                 = 257,
    } FlateConstants;
    
    typedef enum Flate_BlockTypes {
        BlockType_Literal = 0,
        BlockType_Fixed   = 1,
        BlockType_Dynamic = 2,
        BlockType_Invalid = 3,
    } Flate_BlockTypes;

    typedef struct FlateOptions FlateOptions;

    FlateOptions *FlateOptions_Init(void);

    void Flate_ReadDeflateBlock(FlateOptions *Options, BitBuffer *BitB);

    void Flate_ReadZlibHeader(FlateOptions *Options, BitBuffer *BitB);

    void Flate_Deinit(FlateOptions *Options);

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_EntropyIO_Flate_H */
