/*!
 @header              TIFStream.h
 @author              Marcus Johnson
 @copyright           2021+
 @version             1.0.0
 @brief               This header contains code for TIFF/EP streams.
 */

#pragma once

#ifndef OVIA_StreamIO_TIFStream_H
#define OVIA_StreamIO_TIFStream_H

#include "../../../OVIA/include/StreamIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

#ifdef OVIA_StreamIO_TIFF
    extern const OVIA_MagicIDs TIFFSignature;

    const OVIA_MagicIDs TIFFSignature = {
        .NumMagicIDs          = 2,
        .MagicIDs             = {
            [0]               = {
                .OffsetInBits = 0,
                .SizeInBits   = 16,
                .Signature    = (uint8_t[2]){0x49, 0x49}, // LL, little endian
            },
            [1]               = {
                .OffsetInBits = 0,
                .SizeInBits   = 16,
                .Signature    = (uint8_t[2]){0x4D, 0x4D}, // MM, big endian
            },
        },
    };

    extern const OVIA_Extensions TIFFExtensions;

    const OVIA_Extensions TIFFExtensions = {
        .NumExtensions     = 3,
        .Extensions        = {
            [0]            = {
                .Size      = 4,
                .Extension = UTF32String("tiff"),
            },
            [1]            = {
                .Size      = 3,
                .Extension = UTF32String("tif"),
            },
            [2]            = {
                .Size      = 3,
                .Extension = UTF32String("dng"),
            },
        },
    };

    extern const OVIA_MIMETypes TIFFMIMETypes;

    const OVIA_MIMETypes TIFFMIMETypes = {
        .NumMIMETypes     = 1,
        .MIMETypes        = {
            [0]           = {
                .Size     = 10,
                .MIMEType = UTF32String("image/tiff"),
            },
            [1]           = {
                .Size     = 10,
                .MIMEType = UTF32String("image/x-tiff"),
            },
            [2]           = {
                .Size     = 17,
                .MIMEType = UTF32String("image/x-adobe-dng"),
            },
        },
    };

#if defined(OVIA_StreamIO_Encode)
    extern const OVIA_Stream TIFFMuxer;

    const OVIA_Stream TIFFMuxer = {
        .MagicID = &TIFFSignature,
    };
#endif /* OVIA_StreamIO_Encode */

#if defined(OVIA_StreamIO_Decode)
    extern const OVIA_Stream TIFFDemuxer;

    const OVIA_Stream TIFFDemuxer = {
        .MagicID = &TIFFSignature,
    };
#endif /* OVIA_StreamIO_Decode */

#endif /* TIFF/EP Literals */

    typedef enum TIFFConstants {
        TIFF_ByteOrder_LE = 0x4949,
        TIFF_ByteOrder_BE = 0x4D4D,
    } TIFFConstants;

    /*!
     @constant                    TIFFType_Unspecified NULL
     @constant                    TIFFType_UInt8       BYTE
     @constant                    TIFFType_String      ASCII
     @constant                    TIFFType_UInt16      SHORT
     @constant                    TIFFType_UInt32      LONG
     @constant                    TIFFType_Fraction    RATIONAL
     @constant                    TIFFType_SInt8       SBYTE
     @constant                    TIFFType_Undefined   UNDEFINED, 8 bit byte
     @constant                    TIFFType_SInt16      SSHORT
     @constant                    TIFFType_SInt32      SLONG
     @constant                    TIFFType_SFraction   SRATIONAL
     @constant                    TIFFType_Decimal32   FLOAT
     @constant                    TIFFType_Decimal64   DOUBLE
     */
    typedef enum TIFFFieldTypes {
        TIFFType_Unspecified = 0,
        TIFFType_UInt8       = 1,
        TIFFType_String      = 2,
        TIFFType_UInt16      = 3,
        TIFFType_UInt32      = 4,
        TIFFType_Fraction    = 5,
        TIFFType_SInt8       = 6,
        TIFFType_Undefined   = 7,
        TIFFType_SInt16      = 8,
        TIFFType_SInt32      = 9,
        TIFFType_SFraction   = 10,
        TIFFType_Decimal32   = 11,
        TIFFType_Decimal64   = 12,
    } TIFFFieldTypes;

    typedef enum TIFFTagTypes {
        TIFFTag_NewSubFileType            = 254, // 1 = Thumbnail, 0 = Main image.
        TIFFTag_ImageWidth                = 256,
        TIFFTag_ImageLength               = 257,
        TIFFTag_BitsPerSample             = 258,
        TIFFTag_Compression               = 259,
        TIFFTag_PhotometricIntrepretation = 262,
        TIFFTag_ImageDescription          = 270,
        TIFFTag_Make                      = 271,
        TIFFTag_Model                     = 272,
        TIFFTag_StripOffsets              = 273,
        TIFFTag_Orientation               = 274,
        TIFFTag_SamplesPerPixel           = 277,
        TIFFTag_RowsPerStrip              = 278,
        TIFFTag_StripByteCounts           = 279,
        TIFFTag_XResolution               = 282,
        TIFFTag_YResolution               = 283,
        TIFFTag_PlanarConfiguration       = 284,
        TIFFTag_ResolutionUnit            = 296,
        TIFFTag_Software                  = 305,
        TIFFTag_DateTime                  = 306,
        TIFFTag_Artist                    = 315,
        TIFFTag_TileWidth                 = 322,
        TIFFTag_TileLength                = 323,
        TIFFTag_TileOffsets               = 324,
        TIFFTag_TileByteCounts            = 325,
        TIFFTag_SubIFDs                   = 330,
        TIFFTag_JPEGTables                = 347,
        TIFFTag_YCbCrCoefficients         = 529,
        TIFFTag_YCbCrSubsampling          = 530,
        TIFFTag_YCbCrPositioning          = 531,
        TIFFTag_ReferenceBlackWhite       = 532,
        TIFFTag_CFARepeatPatternDim       = 33421,
        TIFFTag_CFAPattern                = 33422,
        TIFFTag_BatteryLevel              = 33423,
        TIFFTag_Copyright                 = 33432,
        TIFFTag_ExposureTime              = 33434,
        TIFFTag_FNumber                   = 33437,
        TIFFTag_IPTC_NAA                  = 33723,
        TIFFTag_InterColorProfile         = 34675,
        TIFFTag_ExposureProgram           = 34850,
        TIFFTag_SpectralSensitivity       = 34852,
        TIFFTag_GPSInfo                   = 34853,
        TIFFTag_ISOSpeedRatings           = 34855,
        TIFFTag_OECF                      = 34856,
        TIFFTag_Interlace                 = 34857,
        TIFFTag_TimeZoneOffset            = 34858,
        TIFFTag_SelfTimerMode             = 34859,
        TIFFTag_DateTimeOriginal          = 36867,
        TIFFTag_CompressedBitsPerPixel    = 37122,
        TIFFTag_ShutterSpeedValue         = 37377,
        TIFFTag_ApartureValue             = 37378,
        TIFFTag_BrightnessValue           = 37379,
        TIFFTag_ExposureBiasValue         = 37380,
        TIFFTag_MaxApertureValue          = 37381,
        TIFFTag_SubjectDistance           = 37382,
        TIFFTag_MeteringMode              = 37383,
        TIFFTag_LightSource               = 37384,
        TIFFTag_Flash                     = 37385,
        TIFFTag_FocalLength               = 37386,
        TIFFTag_FlashEnergy               = 37387,
        TIFFTag_SpatialFrequencyResponse  = 37388,
        TIFFTag_Noise                     = 37389,
        TIFFTag_FocalPlaneXResolution     = 37390,
        TIFFTag_FocalPlaneYResolution     = 37391,
        TIFFTag_FocalPlaneResolutionUnit  = 37392,
        TIFFTag_ImageNumber               = 37393,
        TIFFTag_SecurityClassification    = 37394,
        TIFFTag_ImageHistory              = 37395,
        TIFFTag_SubjectLocation           = 37396,
        TIFFTag_ExposureIndex             = 37397,
        TIFFTag_TIFFStandardID          = 37398,
        TIFFTag_SensingMethod             = 37399,
    } TIFFTagTypes;

    typedef enum TIFFCompressionTypes {
        TIFFCompression_Uncompressed  = 1,
        TIFFCompression_JPEG          = 7,
        TIFFCompression_VendorGreater = 0x8000,
    } TIFFCompressionTypes;

    typedef struct TIFFIFDEntry {
        uint32_t       FieldCount;
        uint32_t       ValueOffset; // May be the value if it will fit within 4 bytes, if not it's an offset
        TIFFTagTypes   FieldTag;
        TIFFFieldTypes FieldType;
    } TIFFIFDEntry;

    typedef struct TIFFOptions {
        TIFFIFDEntry       *IFDEntries;
        uint32_t            IFD1Offset;
        uint16_t            Version;
        uint16_t            NumIFDEntries;
        BufferIO_ByteOrders ByteOrder;
    } TIFFOptions;

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_StreamIO_TIFStream_H */
