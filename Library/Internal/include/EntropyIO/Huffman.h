/*!
 @header              Huffman.h
 @author              Marcus Johnson
 @copyright           2020+
 @version             1.0.0
 @brief               This header contains code for a Canonical Huffman entropy coder.
 */

#pragma once

#ifndef OVIA_EntropyIO_Huffman_H
#define OVIA_EntropyIO_Huffman_H

#include "../../../OVIA/include/EntropyIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef enum HuffmanConstants {
        NumBitlengths = 16,
    } HuffmanConstants;

    typedef struct HuffmanOptions HuffmanOptions;

    HuffmanOptions *HuffmanOptions_Init(size_t NumSymbols, size_t MaxBitLength);

    void Huffman_ReadBitLengths(HuffmanOptions *Options, BitBuffer *BitB);

    void Huffman_BuildTreeFromBitlengths(HuffmanOptions *Options, uint16_t *BitLengths, size_t NumBitLengths, uint16_t *Symbols, size_t NumSymbols);

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_EntropyIO_Huffman_H */
