#include "../../Dependencies/BitIO/libBitIO/include/BitIO.h"

#pragma once

#ifndef LIBMODERNPNG_LIBMODERNPNG_H
#define LIBMODERNPNG_LIBMODERNPNG_H

#ifdef __cplusplus
extern "C" {
#endif
    
#define PNGMagic 0x89504E470D0A1A0A
    
    enum PNGChunkMarkers {
        iHDRMarker = 0x49484452,
        acTLMarker = 0x6163544c,
        fcTLMarker = 0x6663544c,
        fDATMarker = 0x66444154,
        sTERMarker = 0x73544552,
        bKGDMarker = 0x626b4744,
        cHRMMarker = 0x6348524d,
        gAMAMarker = 0x67414d41,
        oFFsMarker = 0x6f464673,
        iCCPMarker = 0x69434350,
        sBITMarker = 0x73424954,
        sRGBMarker = 0x73524742,
        pHYsMarker = 0x70485973,
        pCALMarker = 0x7043414c,
    };
    
    enum PNGColorTypes {
        PNG_Grayscale   = 0,
        PNG_RGB         = 2,
        PNG_PalettedRGB = 3,
        PNG_GrayAlpha   = 4,
        PNG_RGBA        = 6,
    };
    
    static const char Number2Month[12][3] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
    };
    
    static const uint8_t ChannelsPerColorType[7] = {
        1, 0, 3, 3, 4, 0, 4
    };
    
    typedef struct PNGDecoder DecodePNG;
    
    typedef struct PNGEncoder EncodePNG;
    
    DecodePNG     *InitDecodePNG(void);
    
    EncodePNG     *InitEncodePNG(void);
    
    void           PNGDecodeImage(BitBuffer *BitB, DecodePNG *PNG, uint16_t *DecodedImage);
    
    void           PNGEncodeImage(EncodePNG *Enc, BitBuffer *BitB);

#ifdef __cplusplus
}
#endif

#endif /* LIBMODERNPNG_LIBMODERNPNG_H */
