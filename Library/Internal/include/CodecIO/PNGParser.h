#include "../../libModernPNG.h"
#include "../libModernPNGTypes.h"

#pragma once

#ifndef LIBMODERNPNG_LibModernPNGReadChunks_H
#define LIBMODERNPNG_LibModernPNGReadChunks_H

#ifdef __cplusplus
extern "C" {
#endif
    
    void       ParseIHDR(DecodePNG *Dec, BitBuffer *BitB, uint32_t ChunkSize);
    
    void       ParsePLTE(DecodePNG *Dec, BitBuffer *BitB, uint32_t ChunkSize);
    
    void       ParseTRNS(DecodePNG *Dec, BitBuffer *BitB, uint32_t ChunkSize);
    
    void       ParseBKGD(DecodePNG *Dec, BitBuffer *BitB, uint32_t ChunkSize);
    
    void       ParseCHRM(DecodePNG *Dec, BitBuffer *BitB, uint32_t ChunkSize);
    
    void       ParseGAMA(DecodePNG *Dec, BitBuffer *BitB, uint32_t ChunkSize);
    
    void       ParseOFFS(DecodePNG *Dec, BitBuffer *BitB, uint32_t ChunkSize);
    
    void       ParsePHYS(DecodePNG *Dec, BitBuffer *BitB, uint32_t ChunkSize);
    
    void       ParseSCAL(DecodePNG *Dec, BitBuffer *BitB, uint32_t ChunkSize);
    
    void       ParseSBIT(DecodePNG *Dec, BitBuffer *BitB, uint32_t ChunkSize);
    
    void       ParseSRGB(DecodePNG *Dec, BitBuffer *BitB, uint32_t ChunkSize);
    
    void       ParseSTER(DecodePNG *Dec, BitBuffer *BitB, uint32_t ChunkSize);
    
    void       ParseTEXt(DecodePNG *Dec, BitBuffer *BitB, uint32_t ChunkSize);
    
    void       ParseZTXt(DecodePNG *Dec, BitBuffer *BitB, uint32_t ChunkSize);
    
    void       ParseITXt(DecodePNG *Dec, BitBuffer *BitB, uint32_t ChunkSize);
    
    void       ParseTIME(DecodePNG *Dec, BitBuffer *BitB, uint32_t ChunkSize);
    
    void       ParseACTL(DecodePNG *Dec, BitBuffer *BitB, uint32_t ChunkSize);
    
    void       ParseFCTL(DecodePNG *Dec, BitBuffer *BitB, uint32_t ChunkSize);
    
    void       ParseIDAT(DecodePNG *Dec, BitBuffer *BitB, uint32_t ChunkSize);
    
    void       ParseHIST(DecodePNG *Dec, BitBuffer *BitB, uint32_t ChunkSize);
    
    void       ParseICCP(DecodePNG *Dec, BitBuffer *BitB, uint32_t ChunkSize);
    
    void       ParsePCAL(DecodePNG *Dec, BitBuffer *BitB, uint32_t ChunkSize);
    
    void       ParseSPLT(DecodePNG *Dec, BitBuffer *BitB, uint32_t ChunkSize);
    
    void       ParseFDAT(DecodePNG *Dec, BitBuffer *BitB, uint32_t ChunkSize);
    
#ifdef __cplusplus
}
#endif

#endif /* LIBMODERNPNG_LibModernPNGReadChunks_H */
