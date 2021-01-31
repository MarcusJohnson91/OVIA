/*!
 @header              BMPStream.h
 @author              Marcus Johnson
 @copyright           2020+
 @version             1.0.0
 @brief               This header contains code for BMP streams.
 */

#include "StreamIO.h"

#pragma once

#ifndef OVIA_StreamIO_BMPStream_H
#define OVIA_StreamIO_BMPStream_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef enum BMPMagic {
        BMP_BM                = 0x424D,
        BMP_BitmapArray       = 0x4141,
        BMP_ColorIcon         = 0x4349,
        BMP_ColorPointer      = 0x4350,
        BMP_StructIcon        = 0x4943,
        BMP_Pointer           = 0x5054,
    } BMPMagic;

    typedef enum BMPCompressionMethods {
        BMP_RGB               = 0,
        BMP_RLE_8Bit          = 1,
        BMP_RLE_4Bit          = 2,
        BMP_BitFields         = 3,
        BMP_JPEG              = 4,
        BMP_PNG               = 5,
        BMP_RGBABitFields     = 6,
        BMP_CMYK              = 11,
        BMP_CMYK_RLE_8Bit     = 12,
        BMP_CMYK_RLE_4Bit     = 13,
    } BMPCompressionMethods;

    typedef struct BMPOptions {
        uint8_t     *ICCPayload;
        uint32_t     DIBSize;
        uint32_t     FileSize;
        uint32_t     Offset;
        uint32_t     Width;
        int32_t      Height;
        uint16_t     BitDepth;
        uint32_t     CompressionType;
        uint32_t     NumBytesUsedBySamples;
        uint32_t     WidthPixelsPerMeter;
        uint32_t     HeightPixelsPerMeter;
        uint32_t     ColorsIndexed;
        uint32_t     IndexedColorsUsed;
        uint32_t     ColorSpaceType;
        uint32_t     XCoordinate;
        uint32_t     YCoordinate;
        uint32_t     ZCoordinate;
        uint32_t     RGamma;
        uint32_t     GGamma;
        uint32_t     BGamma;
        uint32_t     ICCIntent;
        uint32_t     ICCSize;
        uint32_t     RMask;
        uint32_t     GMask;
        uint32_t     BMask;
        uint32_t     AMask;
    } BMPOptions;

    void           *BMPOptions_Init(void);

    static uint64_t BMPGetRowSize(const uint16_t BitsPerPixel, const uint32_t ImageWidth);

    static uint64_t BMPGetPixelArraySize(const uint64_t RowSize, const int32_t ImageHeight);

    void            BMPOptions_Deinit(void *Options);
    
#ifdef OVIA_StreamIO_BMP
    extern OVIA_MagicIDs BMPSignature = {
        .NumMagicIDs         = 1,
        .MagicIDOffsetInBits = 0,
        .MagicIDSizeInBits   = 16,
        .MagicIDNumber       = {
            [0]              = (uint8_t[2]){0x42, 0x4D},
        }
    };

    extern const OVIA_Extensions BMPExtensions = {
        .NumExtensions = 2,
        .Extensions    = {
            [0]        = UTF32String("bmp"),
            [1]        = UTF32String("dib"),
        },
    };

    extern const OVIA_MIMETypes BMPMIMETypes = {
        .NumMIMETypes = 2,
        .MIMETypes    = {
            [0]       = UTF32String("image/bmp"),
            [1]       = UTF32String("image/x-bmp"),
        },
    };
#endif /* OVIA_StreamIO_BMP */
    
#if defined(OVIA_StreamIO_Encode) && defined(OVIA_StreamIO_BMP)
    extern OVIA_Stream BMPEncoder = {
        .MagicID = &BMPMagicIDs,
    };
#endif /* OVIA_StreamIO_Encode && OVIA_StreamIO_BMP */
    
#if defined(OVIA_StreamIO_Decode) && defined(OVIA_StreamIO_BMP)
    extern OVIA_Stream BMPDecoder = {
        .MagicID = &BMPMagicIDs,
    };
#endif /* OVIA_StreamIO_Decode && OVIA_StreamIO_BMP */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_StreamIO_BMPStream_H */
