/*!
 @header              PNMStream.h
 @author              Marcus Johnson
 @copyright           2020+
 @version             1.0.0
 @brief               This header contains code for PNM streams.
 */

#include "StreamIO.h"

#pragma once

#ifndef OVIA_StreamIO_PNMStream_H
#define OVIA_StreamIO_PNMStream_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    typedef enum PNMTypes {
        UnknownPNM            = 0,
        BlackAndWhitePNM      = 1,
        BinaryPNM             = 2,
        ASCIIPNM              = 3,
        PAMPNM                = 4,
    } PNMTypes;
    
    typedef enum PNMTupleTypes {
        PNM_TUPLE_Unknown     = 0,
        PNM_TUPLE_BnW         = 1,
        PNM_TUPLE_Gray        = 2,
        PNM_TUPLE_GrayAlpha   = 3,
        PNM_TUPLE_RGB         = 4,
        PNM_TUPLE_RGBAlpha    = 5,
    } PNMTupleTypes;
    
    typedef enum PNMConstants {
        PNMCommentStart       = 0x23,
        PNMFieldSeperator     = 0x20,
        PNMEndField           = 0x0A,
    } PNMConstants;
    
    /*
     enum PNMMagic {
     PNM_PBMA = 0x5031, // PortableBitMapASCII
     PNM_PGMA = 0x5032, // PortableGrayMapASCII
     PNM_PPMA = 0x5033, // PortablePixMapASCII
     
     PNM_PBMB = 0x5034, // PortableBitMapBinary
     PNM_PGMB = 0x5035, // PortableGrayMapBinary
     PNM_PPMB = 0x5036, // PortablePixMapBinary
     
     PNM_PAMB = 0x5037, // PortableAnyMap
     } PNMMagic;
     */
    typedef struct PNMOptions {
        uint64_t      Height;
        uint64_t      Width;
        uint8_t       BitDepth;
        uint8_t       NumChannels;
        PNMTupleTypes TupleType;
        PNMTypes      Type;
    } PNMOptions;
    
    void *PNMOptions_Init(void);
    
    void PNMOptions_Deinit(void *Options);
    
#ifdef OVIA_StreamIO_PNM
    extern OVIA_MagicIDs PNMSignature = {
        .NumMagicIDs         = 7,
        .MagicIDOffsetInBits = 0,
        .MagicIDSizeInBits   = 16,
        .MagicIDNumber       = {
            [0]              = (uint8_t[2]){0x50, 0x31},
            [1]              = (uint8_t[2]){0x50, 0x34},
            [2]              = (uint8_t[2]){0x50, 0x32},
            [3]              = (uint8_t[2]){0x50, 0x35},
            [4]              = (uint8_t[2]){0x50, 0x33},
            [5]              = (uint8_t[2]){0x50, 0x36},
            [6]              = (uint8_t[2]){0x50, 0x37},
        }
    };

    extern const OVIA_Extensions PNMExtensions = {
        .NumExtensions = 4,
        .Extensions    = {
            [0]        = UTF32String("pbm"),
            [1]        = UTF32String("pgm"),
            [2]        = UTF32String("ppm"),
            [3]        = UTF32String("pnm"),
        },
    };

    extern const OVIA_MIMETypes PNMMIMETypes = {
        .NumMIMETypes = 4,
        .MIMETypes    = {
            [0]       = UTF32String("image/x-portable-bitmap"),
            [1]       = UTF32String("image/x-portable-graymap"),
            [0]       = UTF32String("image/x-portable-pixmap"),
            [1]       = UTF32String("image/x-portable-anymap"),
        },
    };
#endif /* OVIA_StreamIO_PNM */
    
#if defined(OVIA_StreamIO_Encode) && defined(OVIA_StreamIO_PNM)
    extern OVIA_Stream PNMEncoder = {
        .MagicID = &PNMMagicIDs,
    };
#endif /* OVIA_StreamIO_Encode && OVIA_StreamIO_PNM */
    
#if defined(OVIA_StreamIO_Decode) && defined(OVIA_StreamIO_PNM)
    extern OVIA_Stream PNMDecoder = {
        .MagicID = &PNMMagicIDs,
    };
#endif /* OVIA_StreamIO_Decode && OVIA_StreamIO_PNM */
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_StreamIO_PNMStream_H */

