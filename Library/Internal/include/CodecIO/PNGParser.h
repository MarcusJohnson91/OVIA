#include <stdbool.h>
#include <stdint.h>
#include "../libModernPNG.h"
#include "../PNGTypes.h"

#pragma once

#ifndef LIBMODERNPNG_READCHUNKS_H
#define LIBMODERNPNG_READCHUNKS_H

#ifdef __cplusplus
extern "C" {
#endif
    
    void       ParseIHDR(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParsePLTE(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseTRNS(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseBKGD(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseCHRM(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseGAMA(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseOFFS(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParsePHYS(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseSCAL(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseSBIT(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseSRGB(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseSTER(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseTEXt(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseZTXt(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseITXt(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseTIME(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseACTL(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseFCTL(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseIDAT(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseHIST(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseICCP(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParsePCAL(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseSPLT(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
    void       ParseFDAT(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize);
    
#ifdef __cplusplus
}
#endif

#endif /* LIBMODERNPNG_READCHUNKS_H */
