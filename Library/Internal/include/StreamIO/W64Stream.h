#include "libPCM.h"

#pragma once

#ifndef LIBPCM_W64COMMON_H
#define LIBPCM_W64COMMON_H

#ifdef __cplusplus
extern "C" {
#endif
    
    static const uint8_t RIFF_UUID[BitIOUUIDStringSize] = {
        0x66,0x66,0x69,0x72,0x45,0x91,0x2E,0x45,0x11,0xCF,0x45,0xA5,0xD6,0x45,0x28,0xDB,0x04,0xC1,0x00,0x00,0x00
    };
    
    static const uint8_t WAVE_UUID[BitIOUUIDStringSize] = {
        0x77,0x61,0x76,0x65,0x45,0xF3,0xAC,0x45,0xD3,0x11,0x45,0x8C,0xD1,0x45,0x00,0xC0,0x4F,0x8E,0xDB,0x8A,0x00
    };
    
    static const uint8_t FMT_UUID[BitIOUUIDStringSize] = {
        0x66,0x6D,0x74,0x20,0x45,0xF3,0xAC,0x45,0xD3,0x11,0x45,0x8C,0xD1,0x45,0x00,0xC0,0x4F,0x8E,0xDB,0x8A,0x00
    };
    
    static const uint8_t DATA_UUID[BitIOUUIDStringSize] = {
        0x64,0x61,0x74,0x61,0x45,0xF3,0xAC,0x45,0xD3,0x11,0x45,0x8C,0xD1,0x45,0x00,0xC0,0x4F,0x8E,0xDB,0x8A,0x00
    };
    
    static const uint8_t LEVL_UUID[BitIOUUIDStringSize] = { // aka Peak Envelope Chunk
        0x6C,0x76,0x65,0x6C,0x45,0xAC,0xF3,0x45,0x11,0xD3,0x45,0x8C,0xD1,0x45,0x00,0xC0,0x4F,0x8E,0xDB,0x8A,0x00
    };
    
    static const uint8_t JUNK_UUID[BitIOUUIDStringSize] = {
        0x6B,0x6E,0x75,0x6A,0x45,0xAC,0xF3,0x45,0x11,0xD3,0x45,0x8C,0xD1,0x45,0x00,0xC0,0x4F,0x8E,0xDB,0x8A,0x00
    };
    
    static const uint8_t BEXT_UUID[BitIOUUIDStringSize] = {
        0x74,0x78,0x65,0x62,0x45,0xAC,0xF3,0x45,0x11,0xD3,0x45,0x8C,0xD1,0x45,0x00,0xC0,0x4F,0x8E,0xDB,0x8A,0x00
    };
    
    static const uint8_t FACT_UUID[BitIOUUIDStringSize] = {
        0x74,0x63,0x61,0x66,0x45,0xAC,0xF3,0x45,0x11,0xD3,0x45,0x8C,0xD1,0x45,0x00,0xC0,0x4F,0x8E,0xDB,0x8A,0x00
    };
    
    static const uint8_t LIST_UUID[BitIOUUIDStringSize] = {
        0x74,0x73,0x69,0x6C,0x45,0x91,0x2F,0x45,0x11,0xCF,0x45,0xA5,0xD6,0x45,0x28,0xDB,0x04,0xC1,0x00,0x00,0x00
    };
    
    static const uint8_t MARKER_UUID[BitIOUUIDStringSize] = { // MARKER
        0xAB,0xF7,0x62,0x56,0x45,0x39,0x2D,0x45,0x11,0xD2,0x45,0x86,0xC7,0x45,0x00,0xC0,0x4F,0x8E,0xDB,0x8A,0x00
    };
    
    static const uint8_t SUMMARYLIST_UUID[BitIOUUIDStringSize] = { // SUMMARY LIST
        0x92,0x5F,0x94,0xBC,0x45,0x52,0x5A,0x45,0x11,0xD2,0x45,0x86,0xDC,0x45,0x00,0xC0,0x4F,0x8E,0xDB,0x8A,0x00
    };
    
    void ParseW64FMTChunk(W64Header *W64, BitBuffer *BitB);
    
    void ParseW64BEXTChunk(W64Header *W64, BitBuffer *BitB);
    
#ifdef __cplusplus
}
#endif

#endif /* LIBPCM_W64COMMON_H */
