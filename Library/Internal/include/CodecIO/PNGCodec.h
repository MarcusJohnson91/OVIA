#include "/usr/local/Packages/libBitIO/include/BitIO.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif
    
    extern enum PNGConstants {
        PNGMagic = 0x89504E470D0A1A0A,
    } PNGConstants;
    
    extern enum PNGFlags {
        PNGIsStereoscopic       = false,
        PNGNeedsColorCorrection = false,
    } PNGFlags;
    
    typedef struct GenericDecompressedImage {
        uint16_t   Height;
        uint16_t   Width;
        uint8_t    BitDepth:5;
        bool       Is3D:1;
        uint8_t    NumChannels;
        uint8_t ***ImageData; // Pointer to 3D, pointer to channels, pointer to sample.
    } GenericDecompressedImage;
    /*
    typedef struct iHDR iHDR;
    
    typedef struct acTL acTl;
    
    typedef struct fcTL fcTL;
    
    typedef struct PLTE PLTE;
    
    typedef struct tRNS tRNS;
    
    typedef struct bkGD bkGD;
    
    typedef struct sTER sTER;
    
    typedef struct fdAT fdAT;
    
    typedef struct cHRM cHRM;
    
    typedef struct gAMA gAMA;
    
    typedef struct iCCP iCCP;
    
    typedef struct sBIT sBIT;
    
    typedef struct sRGB sRGB;
    
    typedef struct Text Text;
    
    typedef struct PNGDecoder PNGDecoder;
    
    typedef struct PNGEncoder PNGEncoder;
    */
    
    static const uint8_t ChannelsPerColorType[8];
    
    void        CalculateSTERPadding(PNGDecoder *PNG);
    
    void        DecodePNGImage(BitInput *BitI, PNGDecoder *PNG, uint8_t *DecodedImage);

#ifdef __cplusplus
}
#endif
