#include "/usr/local/Packages/libBitIO/include/BitIO.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif
    
    extern enum PNGConstants {
        PNGMagic = 0x89504E470D0A1A0A,
    } PNGConstants;
    
    static const uint8_t ChannelsPerColorType[8] = {
        1, 0, 3, 3, 4, 4
    };
    
    static const char Number2Month[12][3] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
    };
    
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
