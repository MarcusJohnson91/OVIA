#include "../../libModernPNG.h"

#pragma once

#ifndef LIBMODERNPNG_LibModernPNGEntropyCoders_H
#define LIBMODERNPNG_LibModernPNGEntropyCoders_H

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef HuffmanNode HuffmanNode;
    
    typedef HuffmanTree HuffmanTree;
    
    /*!
     @abstract                                    "Generates CRC from data".
     @param             Data2CRC                  "Pointer to a BitBuffer containing data to CRC".
     @param             BitOffset                 "Which bit should we start CRCing?".
     @param             BitLength                 "How many bits should we CRC?"
     @param             Polynomial                "The Polynomial in Normal representation".
     @param             PolySize                  "The size of the polynomial in bits".
     @param             PolyInit                  "Initialization value".
     */
    uint64_t            GenerateCRC(BitBuffer *Data2CRC, const uint64_t BitOffset, const uint64_t BitLength, const uint64_t Polynomial, const uint8_t PolySize, const uint64_t PolyInit);
    
    /*!
     @abstract                                    "Computes the CRC of DataBuffer, and compares it to the submitted CRC".
     @param             Data2CRC                  "Pointer to a BitBuffer containing data to CRC".
     @param             BitOffset                 "Which bit should we start CRCing?".
     @param             BitLength                 "How many bits should we CRC?"
     @param             Polynomial                "The Polynomial in Normal representation".
     @param             PolySize                  "The size of the polynomial in bits".
     @param             PolyInit                  "Initialization value".
     @param             PrecomputedCRC            "The precomputed resulting CRC of Data2CRC, to compare the generated CRC with".
     */
    bool                VerifyCRC(BitBuffer *Data2CRC, const uint64_t BitOffset, const uint64_t BitLength, const uint64_t Polynomial, const uint8_t PolySize, const uint64_t PolyInit, const uint64_t PrecomputedCRC);
    
    /*!
     @abstract                                    "Creates Adler32 checksum from input data".
     @return                                      "Returns the Adler32 data from the data input".
     @param             Data                      "Pointer to the data to generate the Adler hash from".
     @param             DataSize                  "Size of data".
     */
    uint32_t            GenerateAdler32(const uint8_t *Data, const uint64_t DataSize);
    
    /*!
     @abstract                                    "Generates Adler32 from the input data, and compares it to the submitted checksum".
     @return                                      "Returns whether the input data matched the provided checksum or not".
     @param             Data                      "Pointer to the data to generate the Adler hash from".
     @param             DataSize                  "Size of data".
     @param             EmbeddedAdler32           "Embedded Adler32 to compare the generated one to".
     */
    bool                VerifyAdler32(const uint8_t *Data, const uint64_t DataSize, const uint32_t EmbeddedAdler32);
    
#ifdef __cplusplus
}
#endif

#endif /* LIBMODERNPNG_LibModernPNGEntropyCoders_H */
