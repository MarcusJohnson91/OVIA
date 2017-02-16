#include "libModernPNG.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef struct iHDR {
        uint32_t Width;
        uint32_t Height;
        uint8_t  BitDepth;
        uint8_t  ColorType;
        uint8_t  Compression;
        uint8_t  FilterMethod;
        bool     IsInterlaced;
        uint32_t CRC;
    } iHDR;
    
    struct acTL {
        uint32_t NumFrames;
        uint32_t TimesToLoop;
        uint32_t CRC;
    };
    
    struct fcTL {
        uint32_t NumFrames;
        uint32_t Width;
        uint32_t Height;
        uint32_t XOffset;
        uint32_t YOffset;
        uint16_t Delay1;
        uint16_t Delay2;
        uint8_t  DisposeMethod;
        bool     BlendMethod;
        uint32_t CRC;
    };
    
    struct PLTE {
        uint8_t NumEntries;
        uint8_t  Palette[3][256];
        uint32_t CRC;
    };
    
    struct tRNS {
        uint8_t  NumEntries;
        uint8_t  Palette[3][256];
        uint32_t CRC;
    };
    
    struct bkGD {
        uint8_t  BackgroundPaletteEntry[3];
        uint32_t CRC;
    };
    
    struct sTER {
        bool     ChunkExists:1;
        bool     StereoType:1;
        uint32_t CRC;
    };
    
    struct fdAT {
        uint32_t FrameNum;
    };
    
    struct cHRM { // sRGB or iCCP overrides cHRM
        uint32_t WhitePoint[2]; // X = 0, Y = 1
        uint32_t Red[2];
        uint32_t Green[2];
        uint32_t Blue[2];
        uint32_t CRC;
    };
    
    struct gAMA { // sRGB or iCCP overrides gAMA
        uint32_t Gamma;
        uint32_t CRC;
    };
    
    struct oFFs {
        int32_t  XOffset;
        int32_t  YOffset;
        bool     UnitSpecifier;
        uint32_t CRC;
    };
    
    struct iCCP {
        uint8_t  ProfileName[80];
        uint8_t  NullSeperator; // Basically, once you've seen a null, subtract the bytes left in the chunk from the bytes read in order to find how large the compressed profile is.
        uint8_t  CompressionType;
        uint8_t *CompressedICCPProfile;
        uint32_t CRC;
    };
    
    struct sBIT {
        uint8_t  Grayscale;
        uint8_t  Red;
        uint8_t  Green;
        uint8_t  Blue;
        uint8_t  Alpha;
        uint32_t CRC;
    };
    
    struct sRGB {
        uint8_t  RenderingIntent;
        uint32_t CRC;
    };
    
    struct Text { // Replaces:  tEXt, iTXt, zTXt
        uint8_t TextType;
        uint8_t Keyword[80];
        uint8_t NullSeperator;
        uint8_t TextString[];
    };
    
#ifdef __cplusplus
}
#endif
