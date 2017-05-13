#include "../libModernPNG.h"
#include "../PNGTypes.h"

#pragma once

#ifndef LIBMODERNPNG_READCHUNKS_H
#define LIBMODERNPNG_READCHUNKS_H

#ifdef __cplusplus
extern "C" {
#endif
    
    void       ParseIHDR(BitBuffer *BitB, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParsePLTE(BitBuffer *BitB, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseTRNS(BitBuffer *BitB, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseBKGD(BitBuffer *BitB, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseCHRM(BitBuffer *BitB, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseGAMA(BitBuffer *BitB, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseOFFS(BitBuffer *BitB, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParsePHYS(BitBuffer *BitB, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseSCAL(BitBuffer *BitB, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseSBIT(BitBuffer *BitB, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseSRGB(BitBuffer *BitB, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseSTER(BitBuffer *BitB, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseTEXt(BitBuffer *BitB, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseZTXt(BitBuffer *BitB, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseITXt(BitBuffer *BitB, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseTIME(BitBuffer *BitB, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseACTL(BitBuffer *BitB, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseFCTL(BitBuffer *BitB, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseIDAT(BitBuffer *BitB, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseHIST(BitBuffer *BitB, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseICCP(BitBuffer *BitB, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParsePCAL(BitBuffer *BitB, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseSPLT(BitBuffer *BitB, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseFDAT(BitBuffer *BitB, DecodePNG *Dec, uint32_t ChunkSize);
    
#ifdef __cplusplus
}
#endif

#endif /* LIBMODERNPNG_READCHUNKS_H */
