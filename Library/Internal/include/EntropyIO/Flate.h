#include "../../../../Dependencies/FoundationIO/libFoundationIO/include/BitIO.h"
#include "../../../../Dependencies/FoundationIO/libFoundationIO/include/ContainerIO.h"
#include "../../libOVIA.h"

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
    
    typedef enum FlateConstants {
        MaxCodes                   = 288,
        MaxLiteralLengthCodes      = 286,
        MaxDistanceCodes           = 30,
        NumMetaCodes               = 19,
        MaxBitsPerSymbol           = 15,
    } FlateConstants;
    
    typedef enum OVIA_Flate_BlockTypes {
        Flate_LiteralBlock = 0,
        Flate_FixedBlock   = 1,
        Flate_DynamicBlock = 2,
        Flate_InvalidBlock = 3
    } OVIA_Flate_BlockTypes;
    
    typedef struct HuffmanTable {
        uint16_t  NumSymbols;
        uint16_t *Frequency; // Count
        uint16_t *Symbols;   // HuffmanCode
    } HuffmanTable;
    
    void OVIA_PNG_DAT_Decode(OVIA *Ovia, BitBuffer *BitB, ImageContainer *Image);
    
    HuffmanTable *OVIA_PNG_Huffman_BuildTree(uint64_t NumSymbols, const uint16_t *Lengths);
    
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
    
    static const uint8_t MetaCodeLengthOrder[] = {
        16, 17, 18, 0,  8, 7,  9, 6,
        10,  5, 11, 4, 12, 3, 13, 2,
        14,  1, 15
    };
    
    static uint16_t FixedLiteralTable[] = {
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
    
    static const uint16_t FixedDistanceTable[] = {
        5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5
    };
    
    void OVIA_PNG_Flate_ReadZlibHeader(OVIA *Ovia, BitBuffer *BitB);
    
    void OVIA_PNG_Flate_ReadLiteralBlock(OVIA *Ovia, BitBuffer *BitB);
    
    void OVIA_PNG_Flate_ReadFixedBlock(OVIA *Ovia, BitBuffer *BitB);
    
    void OVIA_PNG_Flate_ReadTreeDynamic(OVIA *Ovia, BitBuffer *BitB);
    
    uint64_t ReadHuffman(HuffmanTable *Tree, BitBuffer *BitB);
    
    void OVIA_PNG_Flate_ReadSymbol(OVIA *Ovia, BitBuffer *BitB);
    
#ifdef __cplusplus
}
#endif

#endif /* OVIA_Flate_H */
