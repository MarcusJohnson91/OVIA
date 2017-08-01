#include "../../Dependencies/libPCM/Dependencies/BitIO/libBitIO/include/BitIO.h"

#pragma once

#ifndef LIBMODERNPNG_LIBMODERNPNG_H
#define LIBMODERNPNG_LIBMODERNPNG_H

#ifdef __cplusplus
extern "C" {
#endif
    
#define PNGMagic             0x89504E470D0A1A0A
    
    enum ModernPNGChunkMarkers {
        iHDRMarker         = 0x49484452,
        acTLMarker         = 0x6163544C,
        fcTLMarker         = 0x6663544C,
        fDATMarker         = 0x66444154,
        sTERMarker         = 0x73544552,
        bKGDMarker         = 0x626b4744,
        cHRMMarker         = 0x6348524d,
        gAMAMarker         = 0x67414d41,
        oFFsMarker         = 0x6f464673,
        iCCPMarker         = 0x69434350,
        sBITMarker         = 0x73424954,
        sRGBMarker         = 0x73524742,
        pHYsMarker         = 0x70485973,
        pCALMarker         = 0x7043414C,
    };
    
    enum ModernPNGColorTypes {
        PNG_Grayscale      = 0,
        PNG_RGB            = 2,
        PNG_PalettedRGB    = 3,
        PNG_GrayAlpha      = 4,
        PNG_RGBA           = 6,
    };
    
    enum ModernPNGInterlaceType {
        PNGNotInterlaced   = 0,
        PNGInterlacedAdam7 = 1,
    };
    
    static const char ModernPNGNumber2Month[12][3] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
    };
    
    static const uint8_t ModernPNGChannelsPerColorType[7] = {
        1, 0, 3, 3, 4, 0, 4
    };
    
    typedef struct PNGDecoder DecodePNG;
    
    typedef struct PNGEncoder EncodePNG;
    
    /*!
     @abstract                     "Initializes the DecodePNG structure to start decoding this specific PNG file"
     @return                       "It takes no parameters, and returns a pointer to the PNGDecoder (typedef'd as DecodePNG) structure"
     */
    DecodePNG     *InitDecodePNG(void);
    
    /*!
     @abstract                     "Initializes the EncodePNG structure to start encoding this specific PNG file"
     @return                       "It takes no parameters, and returns a pointer to the PNGEncoder (typedef'd as EncodePNG) structure"
     */
    EncodePNG     *InitEncodePNG(void);
    
    /*!
     @abstract                     "Uninitializes the PNGDecoder (typedef'd as DecodePNG) structure after you're done decoding this specific PNG file"
     */
    void           DeinitDecodePNG(DecodePNG *Dec);
    
    /*!
     @abstract                     "Uninitializes the PNGEncoder (typedef'd as EncodePNG) structure after you're done decoding this specific PNG file"
     */
    void           DeinitEncodePNG(EncodePNG *Enc);
    
    /*!
     @abstract                     "Encodes a PNG from RawImage2Encode to a BitBuffer"
     @param        Enc             "Pointer to EncodePNG struct containing all the metadata about the image to be encoded"
     @param        RawImage2Encode "Pointer to raw array containing the image, supports 2D array containing stereoscopic frames"
     @param        InterlacePNG    "Should this PNG file be interlaced using the Adam7 algorithm for progressive download?"
     @param        OptimizePNG     "Should this PNG file be optimized by trying all filters? (Huffman optimization is enabled by default)"
     */
    BitBuffer     *EncodePNGImage(EncodePNG *Enc, void ****RawImage2Encode, bool InterlacePNG, bool OptimizePNG);
    
    /*!
     @abstract                     "Decodes a PNG from PNGImage2Decode to a BitBuffer"
     @param        Dec             "Pointer to DecodePNG struct containing all the metadata about the image to be decoded"
     @param        PNGImage2Decode "Pointer to raw array containing the image, supports 2D array containing stereoscopic frames"
     */
    BitBuffer     *DecodePNGImage(DecodePNG *Dec, void ****PNGImage2Decode);

#ifdef __cplusplus
}
#endif

#endif /* LIBMODERNPNG_LIBMODERNPNG_H */
