#include "/usr/local/Packages/libBitIO/include/BitIO.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif
    
    extern enum PNGConstants {
        PNGMagic = 0x89504E470D0A1A0A,
    } PNGConstants;
    
    extern enum PNGFlags {
        PNGIsStereoscopic       = 0,
        PNGNeedsColorCorrection = 0,
    } PNGFlags;
    
    typedef struct PNGDecoder PNGDecoder;
    
    typedef struct PNGEncoder PNGEncoder;
    
    void     InitPNGDecoder(PNGDecoder *PNG);
    
    void     DecodePNGImage(BitInput *BitI, PNGDecoder *PNG, uint8_t *DecodedImage);

#ifdef __cplusplus
}
#endif
