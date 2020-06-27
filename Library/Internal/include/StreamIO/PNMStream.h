/*!
 @header              PNMCommon.h
 @author              Marcus Johnson
 @copyright           2017+
 @version             1.0.0
 @brief               This header contains code for reading and writing lossless PNM image files
 */

#include "OVIACommon.h"
#include "StreamIO.h"

#pragma once

#ifndef OVIA_PNMCommon_H
#define OVIA_PNMCommon_H

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
    
    typedef struct PNMOptions {
        uint64_t      Height;
        uint64_t      Width;
        uint8_t       BitDepth;
        uint8_t       NumChannels;
        PNMTupleTypes TupleType;
        PNMTypes      Type;
    } PNMOptions;
    
    void *PNMOptions_Init(void);
    
    void PNMReadHeader(void *Options, BitBuffer *BitB);
    
    void *PNMExtractImage(void *Options, BitBuffer *BitB);
    
    void PNMWriteHeader(void *Options, BitBuffer *BitB);
    
    void PNMInsertImage(void *Options, void *Container, BitBuffer *BitB);
    
    void PNMWriteFooter(void *Options, BitBuffer *BitB);
    
    void PNMOptions_Deinit(void *Options);

    static const OVIA_Extensions PNMExtensions = {
        .NumExtensions = 5,
        .Extensions    = {
            [0] = UTF32String("pam"),
            [1] = UTF32String("pbm"),
            [2] = UTF32String("pgm"),
            [3] = UTF32String("ppm"),
            [4] = UTF32String("pnm"),
        },
    };

    static const OVIA_MIMETypes PNMMIMETypes = {
        .NumMIMETypes = 6,
        .MIMETypes    = {
            [0]       = UTF32String("application/x-portable-anymap"), // pam, pnm
            [1]       = UTF32String("image/x-portable-anymap"), // pam, pnm
            [2]       = UTF32String("image/x-portable-bitmap"), // pbm
            [3]       = UTF32String("image/x-portable-graymap"), // pgm
            [4]       = UTF32String("image/x-portable-greymap"), // pgm
            [5]       = UTF32String("image/x-portable-pixmap"),  // ppm

        },
    };

    static const OVIA_MagicIDs PNMMagicIDs = {
        .NumMagicIDs         = 7,
        .MagicIDOffsetInBits = 0,
        .MagicIDSizeInBits   = 16,
        .MagicIDNumber = {
            [0] = (uint8_t[2]){0x50, 0x31},
            [1] = (uint8_t[2]){0x50, 0x32},
            [2] = (uint8_t[2]){0x50, 0x33},
            [3] = (uint8_t[2]){0x50, 0x34},
            [4] = (uint8_t[2]){0x50, 0x35},
            [5] = (uint8_t[2]){0x50, 0x36},
            [6] = (uint8_t[2]){0x50, 0x37},
        },
    };
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_PNMCommon_H */
