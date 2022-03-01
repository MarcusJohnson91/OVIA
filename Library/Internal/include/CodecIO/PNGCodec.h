/*!
 @header              PNGCodec.h
 @author              Marcus Johnson
 @copyright           2017+
 @version             1.0.0
 @brief               This header contains code for PNG (encoding and decoding).
 */

#pragma once

#ifndef OVIA_CodecIO_PNGCodec_h
#define OVIA_CodecIO_PNGCodec_h

#include "../../../OVIA/include/CodecIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
#ifndef PNGMagic
#define PNGMagic 0x89504E470D0A1A0A
#endif

    typedef enum PNGChunkMarkers {
        PNGMarker_acTL            = 0x6163544C,
        PNGMarker_bKGD            = 0x626B4744,
        PNGMarker_cHRM            = 0x6348524D,
        PNGMarker_fcTL            = 0x6663544C,
        PNGMarker_fDAT            = 0x66444154,
        PNGMarker_gAMA            = 0x67414D41,
        PNGMarker_hIST            = 0x68495354,
        PNGMarker_iCCP            = 0x69434350,
        PNGMarker_iDAT            = 0x49444154,
        PNGMarker_iEND            = 0x49454E44,
        PNGMarker_iHDR            = 0x49484452,
        PNGMarker_iTXt            = 0x69545874,
        PNGMarker_oFFs            = 0x6F464673,
        PNGMarker_pCAL            = 0x7043414C,
        PNGMarker_pHYs            = 0x70485973,
        PNGMarker_PLTE            = 0x504C5445,
        PNGMarker_sBIT            = 0x73424954,
        PNGMarker_sCAL            = 0x7343414c,
        PNGMarker_sPLT            = 0x73504c54,
        PNGMarker_sRGB            = 0x73524742,
        PNGMarker_sTER            = 0x73544552,
        PNGMarker_tEXt            = 0x74455874,
        PNGMarker_tIME            = 0x74494d45,
        PNGMarker_tRNS            = 0x74524e53,
        PNGMarker_zTXt            = 0x7A545874,
    } PNGChunkMarkers;

    typedef enum PNGColorTypes {
        PNGColor_Gray             = 0,
        PNGColor_RGB              = 2,
        PNGColor_Palette          = 3,
        PNGColor_GrayAlpha        = 4,
        PNGColor_RGBAlpha         = 6,
    } PNGColorTypes;

    typedef enum PNGFilterTypes {
        PNGFilter_Unfiltered      = 0,
        PNGFilter_Sub             = 1, // SubFilter
        PNGFilter_Up              = 2, // UpFilter
        PNGFilter_Average         = 3, // AverageFilter
        PNGFilter_Paeth           = 4, // PaethFilter
    } PNGFilterTypes;

    typedef enum PNGInterlaceTypes {
        PNGInterlace_Unspecified  = 0,
        PNGInterlace_Uninterlaced = 1, // PNGNotInterlaced
        PNGInterlace_Interlaced   = 2, // PNGInterlacedAdam7
    } PNGInterlaceTypes;

    typedef struct iHDR {
        uint32_t      Width;
        uint32_t      Height;
        uint8_t       BitDepth;
        uint8_t       Compression;
        uint8_t       FilterMethod;
        PNGColorTypes ColorType;
        bool          IsInterlaced:1;
        bool          CRCIsValid:1;
    } iHDR;

    typedef struct acTL {
        uint32_t   NumFrames;
        uint32_t   TimesToLoop;
        bool       CRCIsValid:1;
    } acTL;

    typedef struct fcTL {
        uint32_t   FrameNum;
        uint32_t   Width;
        uint32_t   Height;
        uint32_t   XOffset;
        uint32_t   YOffset;
        uint16_t   FrameDelayNumerator;
        uint16_t   FrameDelayDenominator;
        uint8_t    DisposeMethod;
        bool       BlendMethod:1;
        bool       CRCIsValid:1;
    } fcTL;

    typedef struct PLTE {
        uint8_t  **Palette;
        uint8_t    NumEntries;
        bool       CRCIsValid:1;
    } PLTE;

    typedef struct tRNS {
        uint8_t  **Palette;
        uint8_t    NumEntries;
        bool       CRCIsValid:1;
    } tRNS;

    typedef struct bkGD {
        uint8_t   *BackgroundPaletteEntry;
        bool       CRCIsValid:1;
    } bkGD;

    typedef struct sTER {
        bool       StereoType:1;
        bool       CRCIsValid:1;
    } sTER;

    typedef struct DAT {
        uint64_t   ImageSize;
        uint64_t   ImageOffset;
        uint32_t   DictID;
        uint32_t   Size;
        uint16_t   LengthLiteralTreeSize;
        uint8_t    DistanceTreeSize;
        uint8_t    CMF;
        uint8_t    FLG;
    } DATChunk;

    typedef struct fdAT {
        uint32_t   FrameNum;
        bool       CRCIsValid:1;
    } fdAT;

    typedef struct cHRM { // sRGB or iCCP overrides cHRM
        uint32_t   WhitePointX; // X = 0, Y = 1
        uint32_t   WhitePointY;
        uint32_t   RedX;
        uint32_t   RedY;
        uint32_t   GreenX;
        uint32_t   GreenY;
        uint32_t   BlueX;
        uint32_t   BlueY;
        bool       CRCIsValid:1;
    } cHRM;

    typedef struct gAMA { // sRGB or iCCP overrides gAMA
        uint32_t   Gamma;
        bool       CRCIsValid:1;
    } gAMA;

    typedef struct oFFs {
        int32_t    XOffset;
        int32_t    YOffset;
        bool       UnitSpecifier:1;
        bool       CRCIsValid:1;
    } oFFs;

    typedef struct iCCP {
        UTF8      *ProfileName;
        uint8_t   *CompressedICCPProfile;
        uint8_t    CompressionType;
        bool       CRCIsValid:1;
    } iCCP;

    typedef struct sBIT {
        uint8_t    Grayscale;
        uint8_t    Red;
        uint8_t    Green;
        uint8_t    Blue;
        uint8_t    Alpha;
        bool       CRCIsValid:1;
    } sBIT;

    typedef struct sRGB {
        uint8_t    RenderingIntent;
        bool       CRCIsValid:1;
    } sRGB;

    typedef struct pHYs {
        uint32_t   PixelsPerUnitXAxis;
        uint32_t   PixelsPerUnitYAxis;
        uint8_t    UnitSpecifier;
        bool       CRCIsValid:1;
    } pHYs;

    typedef struct pCAL {
        UTF8      *CalibrationName;
        uint8_t   *UnitName;
        double    *Parameters;
        int32_t    OriginalZero;
        int32_t    OriginalMax;
        uint8_t    CalibrationNameSize;
        uint8_t    UnitNameSize;
        uint8_t    EquationType;
        uint8_t    NumParams;
        bool       CRCIsValid:1;
    } pCAL;

    typedef struct sCAL {
        float      PixelWidth; // ASCII float
        float      PixelHeight; // ASCII float
        uint8_t    UnitSpecifier;
        bool       CRCIsValid:1;
    } sCAL;

    typedef struct hIST {
        uint32_t   NumColors;
        uint16_t  *Histogram; // For each PLTE entry, there needs to be 1 array element
        bool       CRCIsValid:1;
    } hIST;

    typedef enum PNGTextTypes {
        PNGTextType_tEXt = 0,
        PNGTextType_iTXt = 1,
        PNGTextType_zTXt = 2,
    } PNGTextTypes;

    typedef struct Text { // Replaces:  tEXt, iTXt, zTXt
        UTF8         *Keyword;
        UTF8         *Comment;
        PNGTextTypes  TextType;
        bool          CRCIsValid:1;
    } Text;

    typedef struct tIMe {
        uint16_t   Year;
        uint8_t    Month;
        uint8_t    Day;
        uint8_t    Hour;
        uint8_t    Minute;
        uint8_t    Second;
        bool       CRCIsValid:1;
    } tIMe;

    typedef struct PaletteEntry {
        uint16_t Red;
        uint16_t Green;
        uint16_t Blue;
        uint16_t Alpha;
    } PaletteEntry;

    typedef struct sPLT {
        UTF8         *PaletteName; // 1-79 CodeUnits, NULL terminator, size is number of colors minus PaletteName + Terminator + SampleDepth bytes
        PaletteEntry *Palette;
        uint64_t      NumEntries;
        uint8_t       SampleDepth;
    } sPLT;

    typedef struct PNGOptions {
        struct acTL   *acTL;
        struct bkGD   *bkGD;
        struct cHRM   *cHRM;
        struct DAT    *DAT;
        struct fcTL   *fcTL;
        struct fdAT   *fdAT;
        struct gAMA   *gAMA;
        struct hIST   *hIST;
        struct iCCP   *iCCP;
        struct iHDR   *iHDR;
        struct oFFs   *oFFs;
        struct pCAL   *pCAL;
        struct pHYs   *pHYs;
        struct PLTE   *PLTE;
        struct sBIT   *sBIT;
        struct sCAL   *sCAL;
        struct sPLT   *sPLT;
        struct sRGB   *sRGB;
        struct sTER   *sTER;
        struct Text   *Text;
        struct tIMe   *tIMe;
        struct tRNS   *tRNS;
        uint32_t       NumSPLTChunks;
        uint32_t       NumTextChunks;
        uint32_t       CurrentFrame;
        uint32_t       LineWidth;
        uint32_t       LinePadding;
        bool           PNGIsAnimated:1;
        bool           PNGIs3D:1;
        bool           acTLExists:1;
        bool           bkGDExists:1;
        bool           cHRMExists:1;
        bool           fcTLExists:1;
        bool           gAMAExists:1;
        bool           hISTExists:1;
        bool           iCCPExists:1;
        bool           oFFsExists:1;
        bool           pCALExists:1;
        bool           pHYsExists:1;
        bool           PLTEExists:1;
        bool           sBITExists:1;
        bool           sCALExists:1;
        bool           sPLTExists:1;
        bool           sRGBExists:1;
        bool           sTERExists:1;
        bool           TextExists:1;
        bool           tIMEExists:1;
        bool           tRNSExists:1;
    } PNGOptions;

    static const uint8_t PNGNumChannelsFromColorType[7] = {
        1, 0, 3, 3, 4, 0, 4
    };

    void *PNGOptions_Init(void);

    void  PNG_Parse(void *Options, BitBuffer *BitB);

    void  PNG_Extract(void *Options, BitBuffer *BitB, void *Container);
    
    bool PNGGetStereoscopicStatus(PNGOptions *Dec);

    void  PNGOptions_Deinit(void *Options);

#ifdef OVIA_CodecIO_PNG
    extern const CodecIO_ImageChannelConfig PNGChannelConfig;
    
    const CodecIO_ImageChannelConfig PNGChannelConfig = {
        .NumChannels = 3,
        .Channels    = {
            [0]      = ImageMask_2D | ImageMask_3D_L | ImageMask_3D_R | ImageMask_Luma,
            [1]      = ImageMask_2D | ImageMask_3D_L | ImageMask_3D_R | ImageMask_Red | ImageMask_Green | ImageMask_Blue,
            [2]      = ImageMask_2D | ImageMask_3D_L | ImageMask_3D_R | ImageMask_Red | ImageMask_Green | ImageMask_Blue | ImageMask_Alpha,
        },
    };

    extern const CodecIO_ImageLimitations PNGLimits;
    
    const CodecIO_ImageLimitations PNGLimits = {
        .MaxHeight      = 0xFFFFFFFF,
        .MaxWidth       = 0xFFFFFFFF,
        .MaxBitDepth    = 16,
        .ChannelConfigs = &PNGChannelConfig,
    };
    
    extern const OVIA_MIMETypes PNGMIMETypes;
    
    const OVIA_MIMETypes PNGMIMETypes = {
        .NumMIMETypes     = 5,
        .MIMETypes        = {
            [0]           = {
                .Size     = 9,
                .MIMEType = UTF32String("image/png"),
            },
            [1]           = {
                .Size     = 10,
                .MIMEType = UTF32String("image/apng"),
            },
            [2]           = {
                .Size     = 11,
                .MIMEType = UTF32String("image/x-mng"),
            },
            [3]           = {
                .Size     = 11,
                .MIMEType = UTF32String("video/x-mng"),
            },
            [4]           = {
                .Size     = 11,
                .MIMEType = UTF32String("image/x-jng"),
            },
        },
    };
    
    extern const OVIA_Extensions PNGExtensions;

    const OVIA_Extensions PNGExtensions = {
        .NumExtensions     = 4,
        .Extensions        = {
            [0]            = {
                .Size      = 4,
                .Extension = UTF32String("apng"),
            },
            [1]            = {
                .Size      = 3,
                .Extension = UTF32String("png"),
            },
            [2]            = {
                .Size      = 3,
                .Extension = UTF32String("mng"),
            },
            [3]            = {
                .Size      = 3,
                .Extension = UTF32String("jng"),
            }
        },
    };
    
    extern const OVIA_MagicIDs PNGMagicIDs;

    const OVIA_MagicIDs PNGMagicIDs = {
        .NumMagicIDs   = 3,
        .MagicIDs      = {
            [0]        = {
                .OffsetInBits = 0,
                .SizeInBits   = 64,
                .Signature    = (uint8_t[8]) {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A}, // PNG / APNG
            },
            [1]        = {
                .OffsetInBits = 0,
                .SizeInBits   = 64,
                .Signature    = (uint8_t[8]) {0x8A, 0x4D, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A}, // MNG
            },
            [2]        = {
                .OffsetInBits = 0,
                .SizeInBits   = 64,
                .Signature    = (uint8_t[8]) {0x8B, 0x4A, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A}, // JNG
            },
        },
    };
    
#if defined(OVIA_CodecIO_Encode)
    extern const CodecIO_Encoder PNGEncoder;
    
    const CodecIO_Encoder PNGEncoder = {
        .Function_Initalize   = PNGOptions_Init,
        .Function_Parse       = PNG_Parse,
        .Function_Media       = PNG_Extract,
        .Function_Deinitalize = PNGOptions_Deinit,
        .MagicIDs             = &PNGMagicIDs,
    };
#endif /* OVIA_CodecIO_Encode */
    
#if defined(OVIA_CodecIO_Decode)
    extern const CodecIO_Decoder PNGDecoder;

    const CodecIO_Decoder PNGDecoder = {
        .Function_Initalize   = PNGOptions_Init,
        .Function_Parse       = PNG_Parse,
        .Function_Media       = PNG_Extract,
        .Function_Deinitalize = PNGOptions_Deinit,
    };
#endif /* OVIA_CodecIO_Decode */
    
#endif /* PNG Literals */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_CodecIO_PNGCodec_h */
