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

    typedef enum FlateConstants {
        MaxCodes                   = 288,
        MaxLiteralLengthCodes      = 286,
        MaxDistanceCodes           = 30,
        NumMetaCodes               = 19,
        MaxBitsPerSymbol           = 15,
    } FlateConstants;
    
    typedef enum Flate_BlockTypes {
        BlockType_Literal = 0,
        BlockType_Fixed   = 1,
        BlockType_Dynamic = 2,
        BlockType_Invalid = 3,
    } Flate_BlockTypes;

    typedef struct FlateOptions FlateOptions;

    typedef struct HuffmanTree HuffmanTree;

    void Flate_ReadDeflateBlock(FlateOptions *Options, BitBuffer *BitB);

    void Flate_ReadZlibHeader(FlateOptions *Options, BitBuffer *BitB);

    void HuffmanTree_Deinit(HuffmanTree *Tree);

    static const uint8_t MetaCodeLengthOrder[19] = {
        16, 17, 18, 0,  8, 7,  9, 6,
        10,  5, 11, 4, 12, 3, 13, 2,
        14,  1, 15
    };

    static uint16_t FixedLiteralTable[288] = {
        8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8,
        8, 8, 8, 8, 8, 8, 8, 8,
        9, 9, 9, 9, 9, 9, 9, 9,
        9, 9, 9, 9, 9, 9, 9, 9,
        9, 9, 9, 9, 9, 9, 9, 9,
        9, 9, 9, 9, 9, 9, 9, 9,
        9, 9, 9, 9, 9, 9, 9, 9,
        9, 9, 9, 9, 9, 9, 9, 9,
        9, 9, 9, 9, 9, 9, 9, 9,
        9, 9, 9, 9, 9, 9, 9, 9,
        9, 9, 9, 9, 9, 9, 9, 9,
        9, 9, 9, 9, 9, 9, 9, 9,
        9, 9, 9, 9, 9, 9, 9, 9,
        9, 9, 9, 9, 9, 9, 9, 9,
        9, 9, 9, 9, 9, 9, 9, 9,
        9, 9, 9, 9, 9, 9, 9, 9,
        7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7,
        8, 8, 8, 8, 8, 8, 8, 8
    };

    static const uint16_t FixedDistanceTable[29] = {
        5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5
    };

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_EntropyIO_Flate_H */
