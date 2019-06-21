#include "../InternalOVIA.h"

#pragma once

#ifndef OVIA_BMPCommon_H
#define OVIA_BMPCommon_H

#ifdef __cplusplus
extern "C" {
#endif
    
    enum BMPMagic {
        BMP_BM                = 0x424D,
        BMP_BitmapArray       = 0x4141,
        BMP_ColorIcon         = 0x4349,
        BMP_ColorPointer      = 0x4350,
        BMP_StructIcon        = 0x4943,
        BMP_Pointer           = 0x5054,
    };
    
    enum BMPCompressionMethods {
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
    };
    
    typedef struct BMPOptions {
        uint32_t     DIBSize;
        uint32_t     FileSize;
        uint32_t     Offset;
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
        uint8_t     *ICCPayload;
        uint32_t     RMask;
        uint32_t     GMask;
        uint32_t     BMask;
        uint32_t     AMask;
    } BMPOptions;
    
    void            BMPParseMetadata(OVIA *Ovia, BitBuffer *BitB);
    
    ImageContainer *BMPExtractImage(OVIA *Ovia, BitBuffer *BitB);
    
    void            BMPInsertImage(ImageContainer *Image, BitBuffer *BitB);
    
    void            BMPWriteHeader(OVIA *Ovia, BitBuffer *BitB);
    
    uint8_t             *OVIA_BMP_GetICCPayload(OVIA *Ovia);
    uint32_t             OVIA_BMP_GetAMask(OVIA *Ovia);
    uint32_t             OVIA_BMP_GetBGamma(OVIA *Ovia);
    uint32_t             OVIA_BMP_GetBMask(OVIA *Ovia);
    uint32_t             OVIA_BMP_GetColorsIndexed(OVIA *Ovia);
    uint32_t             OVIA_BMP_GetColorSpaceType(OVIA *Ovia);
    uint32_t             OVIA_BMP_GetCompressionType(OVIA *Ovia);
    uint32_t             OVIA_BMP_GetDIBSize(OVIA *Ovia);
    uint32_t             OVIA_BMP_GetFileSize(OVIA *Ovia);
    uint32_t             OVIA_BMP_GetGGamma(OVIA *Ovia);
    uint32_t             OVIA_BMP_GetGMask(OVIA *Ovia);
    uint32_t             OVIA_BMP_GetHeightInMeters(OVIA *Ovia);
    uint32_t             OVIA_BMP_GetICCIntent(OVIA *Ovia);
    uint32_t             OVIA_BMP_GetICCSize(OVIA *Ovia);
    uint32_t             OVIA_BMP_GetIndexColorsUsed(OVIA *Ovia);
    uint32_t             OVIA_BMP_GetNumBytesUsedBySamples(OVIA *Ovia);
    uint32_t             OVIA_BMP_GetOffset(OVIA *Ovia);
    uint32_t             OVIA_BMP_GetRGamma(OVIA *Ovia);
    uint32_t             OVIA_BMP_GetRMask(OVIA *Ovia);
    uint32_t             OVIA_BMP_GetWidthInMeters(OVIA *Ovia);
    uint32_t             OVIA_BMP_GetXCoordinate(OVIA *Ovia);
    uint32_t             OVIA_BMP_GetYCoordinate(OVIA *Ovia);
    uint32_t             OVIA_BMP_GetZCoordinate(OVIA *Ovia);
    void                 OVIA_BMP_SetAMask(OVIA *Ovia, uint32_t AlphaMask);
    void                 OVIA_BMP_SetBGamma(OVIA *Ovia, uint32_t BGamma);
    void                 OVIA_BMP_SetBMask(OVIA *Ovia, uint32_t BlueMask);
    void                 OVIA_BMP_SetColorsIndexed(OVIA *Ovia, uint32_t ColorsIndexed);
    void                 OVIA_BMP_SetColorSpaceType(OVIA *Ovia, uint32_t ColorSpaceType);
    void                 OVIA_BMP_SetCompressionType(OVIA *Ovia, uint32_t CompressionType);
    void                 OVIA_BMP_SetDIBSize(OVIA *Ovia, uint32_t DIBSize);
    void                 OVIA_BMP_SetFileSize(OVIA *Ovia, uint32_t FileSize);
    void                 OVIA_BMP_SetGGamma(OVIA *Ovia, uint32_t GGamma);
    void                 OVIA_BMP_SetGMask(OVIA *Ovia, uint32_t GreenMask);
    void                 OVIA_BMP_SetHeightInMeters(OVIA *Ovia, uint32_t HeightInMeters);
    void                 OVIA_BMP_SetICC(OVIA *Ovia, uint32_t ICCIntent, uint32_t ICCSize, uint8_t *ICCPayload);
    void                 OVIA_BMP_SetIndexColorsUsed(OVIA *Ovia, uint32_t IndexColorsUsed);
    void                 OVIA_BMP_SetNumBytesUsedBySamples(OVIA *Ovia, uint32_t NumBytesUsedBySamples);
    void                 OVIA_BMP_SetOffset(OVIA *Ovia, uint32_t Offset);
    void                 OVIA_BMP_SetRGamma(OVIA *Ovia, uint32_t RGamma);
    void                 OVIA_BMP_SetRMask(OVIA *Ovia, uint32_t RedMask);
    void                 OVIA_BMP_SetWidthInMeters(OVIA *Ovia, uint32_t WidthInMeters);
    void                 OVIA_BMP_SetXCoordinate(OVIA *Ovia, uint32_t XCoordinate);
    void                 OVIA_BMP_SetYCoordinate(OVIA *Ovia, uint32_t YCoordinate);
    void                 OVIA_BMP_SetZCoordinate(OVIA *Ovia, uint32_t ZCoordinate);
    
#ifdef __cplusplus
}
#endif

#endif /* OVIA_BMPCommon_H */
