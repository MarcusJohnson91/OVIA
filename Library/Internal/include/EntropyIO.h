/*!
 @header              EntropyIO.h
 @author              Marcus Johnson
 @copyright           2020+
 @version             1.0.0
 @brief               This header contains code for reusable Entropy encoders and decoders.
 */

#include "MediaIO.h"
#include "../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../Dependencies/FoundationIO/Library/include/TextIO/LogIO.h"

#pragma once

#ifndef OVIA_EntropyIO_H
#define OVIA_EntropyIO_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    /*
     What all do we need to put here?

     for Canonical Huffman:
     Encode:
     Take a table of sorted frequencies and generate a Huffman tree that matches that frequency table; GenerateTable
     Take a Huffman table and encode the actual data and output it to a BitBuffer.

     Decode:
     Read the Canonical Huffman table and create a Huffman table from it.
     Decode the BitBuffer data into whatever it is, so return the actual encoded symbol.
     */

    //typedef struct HuffmanNode HuffmanNode;

    typedef struct HuffmanTable HuffmanTable;

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_EntropyIO_H */
