#include "../../libOVIA.h"

#include "../../../../Dependencies/FoundationIO/libFoundationIO/include/BitIO.h"
#include "../../../../Dependencies/FoundationIO/libFoundationIO/include/Log.h"
#include "../../../../Dependencies/FoundationIO/libFoundationIO/include/Math.h"
#include "../../../../Dependencies/FoundationIO/libFoundationIO/include/ContainerIO.h"

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
    
    void        BMPParseMetadata(PCMFile *PCM, BitBuffer *BitB);
    
    uint16_t ***BMPExtractImage(PCMFile *PCM, BitBuffer *BitB);
    
    void        BMPInsertImage(PCMFile *PCM, BitBuffer *BitB, uint16_t ***Image2Insert);
    
    void        BMPWriteHeader(PCMFile *PCM, BitBuffer *BitB, uint32_t NumPixels);
    
#ifdef __cplusplus
}
#endif

#endif /* OVIA_BMPCommon_H */
