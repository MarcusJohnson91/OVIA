/*!
 @header              DNGCodec.h
 @author              Marcus Johnson
 @copyright           2020+
 @version             1.0.0
 @brief               This header contains code for DNG (encoding and decoding).
 */

#pragma once

#ifndef OVIA_CodecIO_DNGCodec_h
#define OVIA_CodecIO_DNGCodec_h

#include "../CodecIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef enum DNG_LightSourceTypes {
        LightSource_Unspecified            = 0,
        LightSource_Daylight               = 1,
        LightSource_Fluorescent            = 2,
        LightSource_Tungsten               = 3,
        LightSource_Flash                  = 4,
        LightSource_Fine_Shade             = 9,
        LightSource_Cloudy_Shade           = 10,
        LightSource_Shade                  = 11,
        LightSource_Daylight_Fluorescent   = 12,
        LightSource_Day_White_Fluorescent  = 13,
        LightSource_Cool_White_Fluorescent = 14,
        LightSource_White_Fluorescent      = 15,
        LightSource_Standard_Light_A       = 17,
        LightSource_Standard_Light_B       = 18,
        LightSource_Standard_Light_C       = 19,
        LightSource_D55                    = 20,
        LightSource_D65                    = 21,
        LightSource_D75                    = 22,
        LightSource_D50                    = 23,
        LightSource_ISO_Studio_Tungsten    = 24,
        LightSource_Other                  = 255,
    } DNG_LightSourceTypes;

    typedef enum DNG_CompressionTypes {
        Compression_Unspecified = 0,
        Compression_None        = 1,
        Compression_LZW         = 5,
        Compression_JPEG1       = 6,
        Compression_JPEG2       = 7,
        Compression_ZIP         = 8,
        Compression_NEF         = 34713,
    } DNG_CompressionTypes;

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_CodecIO_DNGCodec_h */
