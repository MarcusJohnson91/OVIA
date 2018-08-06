#include "../../libModernPNG.h"

#pragma once

#ifndef libModernPNG_Decode_H
#define libModernPNG_Decode_H

#ifdef __cplusplus
extern "C" {
#endif
    
    uint8_t    PaethPredictor(int64_t Left, int64_t Above, int64_t UpperLeft);
    
    uint8_t    ParsePNGMetadata(DecodePNG *Dec, BitBuffer *BitB);
    
    void       PNGDecodeSubFilter(DecodePNG *Dec, uint8_t ***InflatedData, uint8_t ***DeFilteredData, size_t Line);
    
    void       PNGDecodeNonFilter(DecodePNG *Dec, uint8_t ***InflatedData, uint8_t ***DeFilteredData, size_t Line);
    
    void       PNGDecodeUpFilter(DecodePNG *Dec, uint8_t ***InflatedData, uint8_t ***DeFilteredData, size_t Line);
    
    void       PNGDecodeAverageFilter(DecodePNG *Dec, uint8_t ***InflatedData, uint8_t ***DeFilteredData, size_t Line);
    
    void       PNGDecodePaethFilter(DecodePNG *Dec, uint8_t ***InflatedData, uint8_t ***DeFilteredData, size_t Line);
    
    void       PNGDecodeFilter(DecodePNG *Dec, void ***InflatedData);
    
#ifdef __cplusplus
}
#endif

#endif /* libModernPNG_Decode_H */
