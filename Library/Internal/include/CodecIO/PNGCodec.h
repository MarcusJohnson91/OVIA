#include "/usr/local/Packages/libBitIO/include/BitIO.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif
    
    extern enum PNGConstants {
        PNGMagic  = 0x89504E470D0A1A0A,
    } PNGConstants;
    
    extern enum PNGFlags {
        PNGIsStereoscopic       = false,
        PNGNeedsColorCorrection = false,
    } PNGFlags;
    
    typedef struct PNGDecoder DecodePNG;
    
    typedef struct PNGEncoder EncodePNG;
    
    static const char Number2Month[12][3] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
    };
    
    static const uint8_t ChannelsPerColorType[8] = {
        1, 0, 3, 3, 4, 4
    };
    
    void        DecodePNGImage(BitInput *BitI, DecodePNG *PNG, uint8_t *DecodedImage);

#ifdef __cplusplus
}
#endif
