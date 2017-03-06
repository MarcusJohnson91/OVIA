#include "/usr/local/Packages/libBitIO/include/BitIO.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif
    
    extern enum PNGConstants {
        PNGMagic  = 0x89504E470D0A1A0A,
    } PNGConstants;
    
    extern enum PNGChunkMarkers {
        iHDRMarker = 0x49484452,
        acTLMarker = 0x6163544c,
        fcTLMarker = 0x6663544c,
        fDATMarker = 0x66444154,
        sTERMarker = 0x73544552,
        bKGDMarker = 0x626b4744,
    };
    
    extern enum PNGFlags {
        PNGIsStereoscopic       = false,
        PNGNeedsColorCorrection = false,
    } PNGFlags;
    
    extern enum PNGColorTypes {
        Grayscale   = 0,
        RGB         = 2,
        PalettedRGB = 3,
        GrayAlpha   = 4,
        RGBA        = 6,
    };
    
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
