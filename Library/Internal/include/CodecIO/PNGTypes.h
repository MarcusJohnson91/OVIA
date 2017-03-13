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
    
    typedef struct acTL {
        uint32_t NumFrames;
        uint32_t TimesToLoop;
        uint32_t CRC;
    } acTL;
    
    typedef struct fcTL {
        uint32_t FrameNum;
        uint32_t Width;
        uint32_t Height;
        uint32_t XOffset;
        uint32_t YOffset;
        uint16_t FrameDelayNumerator;
        uint16_t FrameDelayDenominator;
        uint8_t  DisposeMethod;
        bool     BlendMethod;
        uint32_t CRC;
    } fcTL;
    
    typedef struct PLTE {
        uint8_t   NumEntries;
        uint8_t **Palette;
        uint32_t  CRC;
    } PLTE;
    
    typedef struct tRNS {
        uint8_t   NumEntries;
        uint8_t **Palette;
        uint32_t  CRC;
    } tRNS;
    
    typedef struct bkGD {
        uint8_t *BackgroundPaletteEntry;
        uint32_t CRC;
    } bkGD;
    
    typedef struct sTER {
        bool     StereoType:1;
        uint32_t CRC;
    } sTER;
    
    typedef struct fdAT {
        uint32_t FrameNum;
        uint32_t CRC;
    } fdAT;
    
    typedef struct cHRM { // sRGB or iCCP overrides cHRM
        uint32_t WhitePointX; // X = 0, Y = 1
        uint32_t WhitePointY;
        uint32_t RedX;
        uint32_t RedY;
        uint32_t GreenX;
        uint32_t GreenY;
        uint32_t BlueX;
        uint32_t BlueY;
        uint32_t CRC;
    } cHRM;
    
    typedef struct gAMA { // sRGB or iCCP overrides gAMA
        uint32_t Gamma;
        uint32_t CRC;
    } gAMA;
    
    typedef struct oFFs {
        int32_t  XOffset;
        int32_t  YOffset;
        bool     UnitSpecifier;
        uint32_t CRC;
    } oFFs;
    
    typedef struct iCCP {
        uint8_t  ProfileNameSize;
        uint32_t CompressedICCPProfileSize;
        
        uint8_t *ProfileName;
        uint8_t  CompressionType;
        uint8_t *CompressedICCPProfile;
        
        uint32_t CRC;
    } iCCP;
    
    typedef struct sBIT {
        uint8_t  Grayscale;
        uint8_t  Red;
        uint8_t  Green;
        uint8_t  Blue;
        uint8_t  Alpha;
        uint32_t CRC;
    } sBIT;
    
    typedef struct sRGB {
        uint8_t  RenderingIntent;
        uint32_t CRC;
    } sRGB;
    
    typedef struct pHYs {
        uint32_t PixelsPerUnitXAxis;
        uint32_t PixelsPerUnitYAxis;
        uint8_t  UnitSpecifier;
        uint32_t CRC;
    } pHYs;
    
    typedef struct pCAL {
        uint8_t  CalibrationNameSize;
        uint8_t  UnitNameSize;
        
        char    *CalibrationName;
        int32_t  OriginalZero;
        int32_t  OriginalMax;
        uint8_t  EquationType;
        uint8_t  NumParams;
        uint8_t *UnitName;
        
        
        uint32_t CRC;
    } pCAL;
    
    typedef struct sCAL {
        
        
        uint8_t  UnitSpecifier;
        char    *PixelWidth; // ASCII float
        char    *PixelHeight; // ASCII float
        uint32_t CRC;
    } sCAL;
    
    typedef struct gIFg {
        uint32_t CRC;
    } gIFg;
    
    typedef struct gIFs {
        uint32_t CRC;
    } gIFs;
    
    typedef struct hIST {
        uint32_t CRC;
    } hIST;
    
    typedef struct Text { // Replaces:  tEXt, iTXt, zTXt
        uint8_t  TextType;
        uint8_t *Keyword;
        uint8_t  NullSeperator;
        uint8_t *TextString;
        uint32_t CRC;
    } Text;
    
#ifdef __cplusplus
}
#endif
