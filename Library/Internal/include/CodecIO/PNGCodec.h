#include "/usr/local/Packages/libBitIO/include/BitIO.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif
    
    extern enum PNGConstants {
        PNGMagic = 0x89504E470D0A1A0A,
    } PNGConstants;
    
    static const uint8_t ChannelsPerColorType[8] = {
        1, 0, 3, 3, 4, 4
    };
    
    static const char Number2Month[12][3] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
    };
    
    extern enum PNGFlags {
        PNGIsStereoscopic       = 0,
        PNGNeedsColorCorrection = 0,
    } PNGFlags;
    
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
        uint32_t NumFrames;
        uint32_t Width;
        uint32_t Height;
        uint32_t XOffset;
        uint32_t YOffset;
        uint16_t Delay1;
        uint16_t Delay2;
        uint8_t  DisposeMethod;
        bool     BlendMethod;
        uint32_t CRC;
    } fcTL;
    
    typedef struct PLTE {
        uint8_t NumEntries;
        uint8_t  Palette[3][256];
        uint32_t CRC;
    } PLTE;
    
    typedef struct tRNS {
        uint8_t  NumEntries;
        uint8_t  Palette[3][256];
        uint32_t CRC;
    } tRNS;
    
    typedef struct bkGD {
        uint8_t  BackgroundPaletteEntry[3];
        uint32_t CRC;
    } bkGD;
    
    typedef struct sTER {
        bool     ChunkExists:1;
        bool     StereoType:1;
        uint32_t CRC;
    } sTER;
    
    typedef struct fdAT {
        uint32_t FrameNum;
    } fdAT;
    
    typedef struct cHRM { // sRGB or iCCP overrides cHRM
        uint32_t WhitePoint[2]; // X = 0, Y = 1
        uint32_t Red[2];
        uint32_t Green[2];
        uint32_t Blue[2];
    } cHRM;
    
    typedef struct gAMA { // sRGB or iCCP overrides gAMA
        uint32_t Gamma;
    } gAMA;
    
    typedef struct iCCP {
        uint8_t ProfileName[80];
        uint8_t NullSeperator; // Basically, once you've seen a null, subtract the bytes left in the chunk from the bytes read in order to find how large the compressed profile is.
        uint8_t CompressionType;
        uint8_t CompressedICCPProfile[];
    } iCCP;
    
    typedef struct sBIT {
        uint8_t Grayscale;
        uint8_t Red;
        uint8_t Green;
        uint8_t Blue;
        uint8_t Alpha;
    } sBIT;
    
    typedef struct sRGB {
        uint8_t RenderingIntent;
    } sRGB;
    
    typedef struct Text { // Replaces:  tEXt, iTXt, zTXt
        uint8_t TextType;
        uint8_t Keyword[80];
        uint8_t NullSeperator;
        uint8_t TextString[];
    } Text;
    
    typedef struct PNGDecoder {
        uint32_t      iHDRSize;
        uint32_t      CurrentFrame;
        uint32_t      LineWidth;
        uint32_t      LinePadding;
        
        bool          IsVideo:1;
        bool          Is3D:1;
        bool          acTLExists:1;
        bool          fcTLExists:1;
        bool          cHRMExists:1;
        bool          gAMAExists:1;
        bool          iCCPExists:1;
        bool          sRGBExists:1;
        iHDR          *iHDR;
        acTL          *acTL;
        fcTL          *fcTL;
        fdAT          *fdAT;
        tRNS          *tRNS;
        cHRM          *cHRM;
        sBIT          *sBIT;
        Text          *Text;
        gAMA          *gAMA;
        iCCP          *iCCP;
        sRGB          *sRGB;
        sTER          *sTER;
        PLTE          *PLTE;
        bkGD          *bkGD;
    } PNGDecoder;
    
    typedef struct PNGEncoder {
        uint32_t      iHDRSize;
        uint32_t      CurrentFrame;
        uint32_t      LineWidth;
        uint32_t      LinePadding;
        
        bool          IsVideo:1;
        bool          Is3D:1;
        bool          acTLExists:1;
        bool          fcTLExists:1;
        bool          cHRMExists:1;
        bool          gAMAExists:1;
        bool          iCCPExists:1;
        bool          sRGBExists:1;
        iHDR          *iHDR;
        acTL          *acTL;
        fcTL          *fcTL;
        fdAT          *fdAT;
        tRNS          *tRNS;
        cHRM          *cHRM;
        sBIT          *sBIT;
        Text          *Text;
        gAMA          *gAMA;
        iCCP          *iCCP;
        sRGB          *sRGB;
        sTER          *sTER;
        PLTE          *PLTE;
        bkGD          *bkGD;
    } PNGEncoder;
    
    void     InitPNGDecoder(PNGDecoder *PNG);
    
    uint8_t  ParsePNG(BitInput *BitI, PNGDecoder *PNG);

#ifdef __cplusplus
}
#endif
