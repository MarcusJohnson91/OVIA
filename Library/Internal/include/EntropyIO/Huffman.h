/*!
 @header              Huffman.h
 @author              Marcus Johnson
 @copyright           2020+
 @SPDX-License-Identifier: Apache-2.0
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

    typedef struct HuffmanTree HuffmanTree; // Forward declare the Huffman-tree-like data structure

    typedef enum HuffmanConstants {
        MaxBitlengthJPG  = 16,
        MaxBitlengthPNG  = 15,
    } HuffmanConstants;

    /*!
     A Huffman Symbol is the actual bitstring that will be seen in the Huffman-encoded stream.
     A Value is the numeric value of the symbol.
     A Bitlength is how long the bitstring is.
     to get the Symbol you need to do ((2^Bitlength)-1) XOR Value
     */

    HuffmanTree *HuffmanTree_Init(size_t NumSymbols, size_t MaxBitLength);

    void Huffman_ReadBitLengths(HuffmanTree *Options, BitBuffer *BitB, size_t NumBitLengths);

    void Huffman_BuildTree_Bitlengths(HuffmanTree *Options, uint16_t *BitLengths, size_t NumBitLengths, uint16_t *Symbols, size_t NumSymbols);

    void Huffman_Deinit(HuffmanTree *Options);

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_EntropyIO_Huffman_H */
