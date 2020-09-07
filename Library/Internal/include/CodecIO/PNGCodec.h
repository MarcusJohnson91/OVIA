/*!
 @header              PNGCodec.h
 @author              Marcus Johnson
 @copyright           2020+
 @version             1.0.0
 @brief               This header contains code for PNG (encoding and decoding).
 */

#include "CodecIO.h"

#pragma once

#ifndef OVIA_PNGCodec_H
#define OVIA_PNGCodec_H

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
        PNGMarker_iHDR            = 0x49484452,
        PNGMarker_iTXt            = 0x69545874,
        PNGMarker_oFFs            = 0x6F464673,
        PNGMarker_pCAL            = 0x7043414C,
        PNGMarker_pHYs            = 0x70485973,
        PNGMarker_PLTE            = 0x504C5445,
        PNGMarker_sBIT            = 0x73424954,
        PNGMarker_sCAL            = 0x7343414c,
        PNGMarker_sRGB            = 0x73524742,
        PNGMarker_sTER            = 0x73544552,
        PNGMarker_tEXt            = 0x74455874,
        PNGMarker_zTXt            = 0x7A545874,
        PNGMarker_tIME            = 0x74494d45,
        PNGMarker_tRNS            = 0x74524e53,
        PNGMarker_sPLT            = 0x73504c54,
    } PNGChunkMarkers;

    typedef enum PNGColorTypes {
        PNGColor_Gray             = 0, // PNG_Grayscale
        PNGColor_RGB              = 2, // PNG_RGB
        PNGColor_Palette          = 3, // PNG_PalettedRGB
        PNGColor_GrayAlpha        = 4, // PNG_GrayAlpha
        PNGColor_RGBAlpha         = 6, // PNG_RGBA
    } PNGColorTypes;

    typedef enum PNGFilterTypes {
        PNGFilter_Unfiltered      = 0, // NotFiltered
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

    typedef struct Text { // Replaces:  tEXt, iTXt, zTXt
        UTF8      *Keyword;
        UTF8      *Comment;
        uint8_t    TextType;
        bool       CRCIsValid:1;
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

    typedef struct PNGDecoder {
        uint16_t    ***DecodedImage;
        struct acTL   *acTL;
        struct bkGD   *bkGD;
        struct cHRM   *cHRM;
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
        struct sRGB   *sRGB;
        struct sTER   *sTER;
        struct Text   *Text;
        struct tIMe   *tIMe;
        struct tRNS   *tRNS;
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
    } PNGDecoder;

    typedef struct PNGEncoder {
        struct acTL   *acTL;
        struct bkGD   *bkGD;
        struct cHRM   *cHRM;
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
        struct sRGB   *sRGB;
        struct sTER   *sTER;
        struct Text   *Text;
        struct tIMe   *tIMe;
        struct tRNS   *tRNS;
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
    } PNGEncoder;

    static const uint8_t PNGNumChannelsFromColorType[7] = {
        1, 0, 3, 3, 4, 0, 4
    };

    uint32_t GenerateCRC32(BitBuffer *BitB, const uint64_t ChunkSize);

    bool VerifyCRC32(BitBuffer *BitB, uint64_t ChunkSize);

    uint32_t GenerateAdler32(const uint8_t *Data, const uint64_t DataSize);

    bool VerifyAdler32(const uint8_t *Data, const uint64_t DataSize, const uint32_t EmbeddedAdler32);

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_PNGCodec_H */
