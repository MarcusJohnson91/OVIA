#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "../../../Dependencies/libPCM/Dependencies/FoundationIO/libFoundationIO/include/StringIO.h"
#include "../../../Dependencies/libPCM/Dependencies/FoundationIO/libFoundationIO/include/Log.h"

#pragma once

#ifndef libModernPNG_Types_H
#define libModernPNG_Types_H

#ifdef __cplusplus
extern "C" {
#endif
    
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
        char      *ProfileName;
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
        char      *CalibrationName;
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
    
    struct PNGDecoder {
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
    };
    
    struct PNGEncoder {
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
    };
    
#ifdef __cplusplus
}
#endif

#endif /* libModernPNG_Types_H */
