/*!
 @header              PNMStream.h
 @author              Marcus Johnson
 @copyright           2020+
 @version             1.0.0
 @brief               This header contains code for PNM streams.
 */

#pragma once

#ifndef OVIA_StreamIO_PNMStream_H
#define OVIA_StreamIO_PNMStream_H

#include "../../../OVIA/include/StreamIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    typedef enum PNMTypes {
        PNMType_Unspecified   = 0,
        PNMType_Grayscale     = 1,
        PNMType_Binary        = 2,
        PNMType_ASCII         = 3,
        PNMType_PAM           = 4,
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
    
    PNMOptions *PNMOptions_Init(void);
    
    void PNMOptions_Deinit(PNMOptions *Options);
    
#ifdef OVIA_StreamIO_PNM
    extern const OVIA_MagicIDs PNMSignatures;
    
    const OVIA_MagicIDs PNMSignatures = {
        .NumMagicIDs          = 7,
        .MagicIDs             = {
            [0]               = {
                .OffsetInBits = 0,
                .SizeInBits   = 16,
                .Signature    = (uint8_t[4]){0x42, 0x31},
            },
            [1]               = {
                .OffsetInBits = 0,
                .SizeInBits   = 16,
                .Signature    = (uint8_t[4]){0x42, 0x34},
            },
            [2]               = {
                .OffsetInBits = 0,
                .SizeInBits   = 16,
                .Signature    = (uint8_t[4]){0x42, 0x32},
            },
            [3]               = {
                .OffsetInBits = 0,
                .SizeInBits   = 16,
                .Signature    = (uint8_t[4]){0x42, 0x35},
            },
            [4]               = {
                .OffsetInBits = 0,
                .SizeInBits   = 16,
                .Signature    = (uint8_t[4]){0x50, 0x33},
            },
            [5]               = {
                .OffsetInBits = 0,
                .SizeInBits   = 16,
                .Signature    = (uint8_t[4]){0x50, 0x36},
            },
            [6]               = {
                .OffsetInBits = 0,
                .SizeInBits   = 16,
                .Signature    = (uint8_t[4]){0x50, 0x37},
            },
        },
    };
    
    extern const OVIA_Extensions PNMExtensions;
    
    const OVIA_Extensions PNMExtensions = {
        .NumExtensions     = 5,
        .Extensions        = {
            OVIA_RegisterExtension(0, UTF32String("pbm"))
            OVIA_RegisterExtension(1, UTF32String("pgm"))
            OVIA_RegisterExtension(2, UTF32String("ppm"))
            OVIA_RegisterExtension(3, UTF32String("pnm"))
            OVIA_RegisterExtension(4, UTF32String("pam"))
        }
    };

    extern const OVIA_MIMETypes PNMMIMETypes;
    
    const OVIA_MIMETypes PNMMIMETypes = {
        .NumMIMETypes     = 5,
        .MIMETypes        = {
            OVIA_RegisterMIMEType(0, UTF32String("image/x-portable-bitmap"))
            OVIA_RegisterMIMEType(1, UTF32String("image/x-portable-pixmap"))
            OVIA_RegisterMIMEType(2, UTF32String("image/x-portable-anymap"))
            OVIA_RegisterMIMEType(3, UTF32String("image/x-portable-graymap"))
            OVIA_RegisterMIMEType(4, UTF32String("image/x-portable-arbitrarymap"))
        }
    };
    
#if defined(OVIA_StreamIO_Encode)
    extern const OVIA_Stream PNMMuxer;
    
    const OVIA_Stream PNMMuxer = {
        .MagicID = &PNMSignatures,
    };
#endif /* OVIA_StreamIO_Encode */
    
#if defined(OVIA_StreamIO_Decode)
    extern const OVIA_Stream PNMDemuxer;
    
    const OVIA_Stream PNMDemuxer = {
        .MagicID = &PNMSignatures,
    };
#endif /* OVIA_StreamIO_Decode */
    
#endif /* PNM Literals */
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_StreamIO_PNMStream_H */

