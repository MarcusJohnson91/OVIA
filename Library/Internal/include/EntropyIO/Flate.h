#include "../../../../Dependencies/FoundationIO/libFoundationIO/include/BitIO.h"

#pragma once

#ifndef OVIA_Flate_H
#define OVIA_Flate_H

#ifdef __cplusplus
extern "C" {
#endif
    
    /*
     Deflate uses MSBit first bit order and MSByte first byte order
     
     CMF   = Compression Method and Flags
     CINFO = Compression Info
     CM    = Compression Method
     
     FLG   = FLaGs
     
     CMF:
     CINFO: top 4 bits, if CM == 8 CINFO = base 2 logarithm of the LZ77 window size, minus 8; CINFO > 7 is not allowed.
     CM: bottom 4 bits, CM = 8 is DEFLATE, 15 is reserved.
     
     FLG:
     FLEVEL: top 2 bits, Compression level
     FDICT:  middle 1 bit, Preset Dictionary
     FCHECK: bottom 5 bits, check bits for CMF and FLG
     
     FCHECK: If you treat CMF and FLG as a uint16_t, the value should be  multiple of 31.
     
     if FDICT is set to 1, there is a DICT dictionary identifier right after the FLG byte.
     
     FLEVEL values: 0 = fastest, 1 = fast, 2 = default, 3 = best
     */
    
    typedef struct FlateHeader {
        uint32_t DictID;   // Adler32 of the tree used to compress the block.
        uint8_t  CINFO:4;  // CompressionInfo
        uint8_t  CM:4;     // CompressionMethod
        uint8_t  FLEVEL:2; // CompressionLevel
        uint8_t  FDICT:1;  // DictionaryPresent
        uint8_t  FCHECK:5; // CheckBits, X added to the other fields so that when the whole header is modulo'd with 31 the result is 0.
    } FlateHeader;
    
    typedef struct HuffmanCode {
        uint8_t  Bits:4;
        uint16_t Code:12;
    } HuffmanCode;
    
    typedef struct HuffmanTable {
        uint16_t     Entries;
        HuffmanCode *Codes;
    } HuffmanTable;
    
    void OVIA_PNG_DAT_ReadFlateHeader(FlateHeader *Header, BitBuffer *BitB);
    
    void OVIA_PNG_DAT_WriteFlateHeader(FlateHeader *Header, BitBuffer *BitB);
    
    void OVIA_PNG_DAT_ReadHuffmanTrees(FlateHeader *Header, BitBuffer *BitB);
    
    static const uint8_t FixedLengthAdditionalBits[]  = {
        0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 1, 2, 2, 2, 2,
        3, 3, 3, 3, 4, 4, 4, 4,
        5, 5, 5, 5, 0
    };
    
    static const uint8_t FixedDistanceAdditionalBits[] = {
        0,   0,  0,  0,  1,  1,  2,  2,
        3,   3,  4,  4,  5,  5,  6,  6,
        7,   7,  8,  8,  9,  9, 10, 10,
        11, 11, 12, 12, 13, 13
    }; // So, Read 5 bits for the Distance code, look it up here.
    
#ifdef __cplusplus
}
#endif

#endif /* OVIA_Flate_H */
