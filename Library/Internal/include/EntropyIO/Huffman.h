#pragma once

#ifndef OVIA_EntropyIO_Huffman_h
#define OVIA_EntropyIO_Huffman_h

#include "EntropyIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

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

#endif /* OVIA_EntropyIO_Huffman_h */
