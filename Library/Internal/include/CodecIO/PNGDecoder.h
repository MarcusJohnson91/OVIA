#include <stdbool.h>
#include <stdint.h>
#include "../libModernPNG.h"

#pragma once

#ifndef LIBMODERNPNG_DECODEPNG_H
#define LIBMODERNPNG_DECODEPNG_H

#ifdef __cplusplus
extern "C" {
#endif
    
    uint8_t    ParsePNGMetadata(BitBuffer *BitB, DecodePNG *Dec);
    
    void       PNGDecodeSubFilter(DecodePNG *Dec, uint8_t **DeEntropyedData, uint8_t **DeFilteredData, size_t Line);
    
    void       PNGDecodeNonFilter(DecodePNG *Dec, uint8_t *DeEntropyedData, uint8_t *DeFilteredData, size_t Line);
    
    void       PNGDecodeUpFilter(DecodePNG *Dec, uint8_t **DeEntropyedData, uint8_t **DeFilteredData, size_t Line);
    
    void       PNGDecodeAverageFilter(DecodePNG *Dec, uint8_t **DeEntropyedData, uint8_t **DeFilteredData, size_t Line);
    
    uint8_t    PaethPredictor(int64_t Left, int64_t Above, int64_t UpperLeft);
    
    void       PNGDecodePaethFilter(DecodePNG *Dec, uint8_t **DeEntropyedData, uint8_t **DeFilteredData, size_t Line);
    
    void       PNGDecodeFilter(DecodePNG *Dec, uint8_t ***InflatedBuffer);
    
#ifdef __cplusplus
}
#endif

#endif /* LIBMODERNPNG_DECODEPNG_H */
