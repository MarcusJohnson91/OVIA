/*!
 @header              Huffman.h
 @author              Marcus Johnson
 @copyright           2022+
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


    typedef struct Huffman { // Each Huffman Code aka binary string corresponds to a value that it represents
        uint32_t Code; // Binary string
        uint32_t Value; // Value represented by the string
    } Huffman;






















    // Old below

    /*!
     @constant BitString     The actual Huffman encoded value
     @constant BitStringSize The number of bits this BitString requires
     @constant RelativeFreq  How common is this symbol, compared to all the other symbols
     @constant Symbol        The actual value the BitString represents
     */
    typedef struct HuffmanData {
        uint16_t BitString;
        uint8_t  BitStringSize;
        uint8_t  RelativeFreq;
        uint8_t  Symbol;
    } HuffmanData;

    typedef struct HuffmanNode {
        HuffmanData         Symbol;
        struct HuffmanNode *Left;
        struct HuffmanNode *Right;
    } HuffmanNode;

    typedef struct HuffmanTree {
        HuffmanNode *Root;
    } HuffmanTree;

    HuffmanTree *HuffmanTree_Init(uint8_t BitLengths[16], uint8_t *Values);

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_EntropyIO_Huffman_H */
