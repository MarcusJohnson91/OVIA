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
        MaxBitlengthJPG  = 16,
        MaxBitlengthPNG  = 15,
    } HuffmanConstants;

    /*!
     A Huffman Symbol is the actual bitstring that will be seen in the Huffman-encoded stream.
     A Value is the numeric value of the symbol.
     A Bitlength is how long the bitstring is.
     to get the Symbol you need to do ((2^Bitlength)-1) XOR Value
     */
    typedef struct HuffmanEntry {
        uint16_t Value;
        uint8_t  Bitlength;
    } HuffmanEntry;

    typedef struct HuffmanOptions {
        HuffmanEntry *Entries;
        size_t        NumEntries;
    } HuffmanOptions;

    HuffmanOptions *HuffmanOptions_Init(size_t NumSymbols, size_t MaxBitLength);

    void Huffman_ReadBitLengths(HuffmanOptions *Options, BitBuffer *BitB, size_t NumBitLengths);

    void Huffman_BuildTree_Bitlengths(HuffmanOptions *Options, uint16_t *BitLengths, size_t NumBitLengths, uint16_t *Symbols, size_t NumSymbols);

    void Huffman_Deinit(HuffmanOptions *Options);

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_EntropyIO_Huffman_H */
