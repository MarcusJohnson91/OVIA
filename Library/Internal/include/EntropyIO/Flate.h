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

    static const uint8_t MetaCodeLengthOrder[19] = {
        16, 17, 18, 0,  8, 7,  9, 6,
        10,  5, 11, 4, 12, 3, 13, 2,
        14,  1, 15
    };

    static const uint16_t LengthBase[29] = {
        3,   4,   5,   6,   7,  8,  9,  10,
        11,  13,  15,  17,  19, 23, 27,  31,
        35,  43,  51,  59,  67, 83, 99, 115,
        131, 163, 195, 227, 258
    };

    static const uint8_t LengthAdditionalBits[] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 1, 2, 2, 2, 2,
        3, 3, 3, 3, 4, 4, 4, 4,
        5, 5, 5, 5, 0
    };

    static const uint16_t DistanceBase[] = {
        1,    2,    3,     4,     5,     7,    9,   13,
        17,   25,   33,    49,    65,    97,  129,  193,
        257,  385,  513,   769,  1025,  1537, 2049, 3073,
        4097, 6145, 8193, 12289, 16385, 24577
    };

    static const uint8_t DistanceAdditionalBits[] = {
        0,   0,  0,  0,  1,  1,  2,  2,
        3,   3,  4,  4,  5,  5,  6,  6,
        7,   7,  8,  8,  9,  9, 10, 10,
        11, 11, 12, 12, 13, 13
    }; // So, Read 5 bits for the Distance code, look it up here.

    void Flate_Deinit(FlateOptions *Options);

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_EntropyIO_Flate_H */
