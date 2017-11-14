#include "../../libPCM.h"

// PGM, PBM, PPM, along with PAM.

#pragma once

#ifndef LIBPCM_PXMCommon_H
#define LIBPCM_PXMCommon_H

#ifdef __cplusplus
extern "C" {
#endif
    
    enum PXMCommon {
        PXMMagicSize          = 2,
        PXMCommentStart       = 0x23,
        PXMFieldSeperator     = 0x20,
        PXMEndField           = 0x0A,
    };
    
    enum PXMMagic {
        PXM_PBMA = 0x5031, // PortableBitMapASCII
        PXM_PBMB = 0x5034, // PortableBitMapBinary
        PXM_PGMA = 0x5032, // PortableGrayMapASCII
        PXM_PGMB = 0x5035, // PortableGrayMapBinary
        PXM_PPMA = 0x5033, // PortablePixMapASCII
        PXM_PPMB = 0x5036, // PortablePixMapBinary
        PXM_PAMB = 0x5037, // PortableAnyMap
    };
    
    typedef enum PXMTupleTypes {
        PXM_TUPLE_Unknown   = 0,
        PXM_TUPLE_BnW       = 1,
        PXM_TUPLE_Gray      = 2,
        PXM_TUPLE_GrayAlpha = 3,
        PXM_TUPLE_RGB       = 4,
        PXM_TUPLE_RGBAlpha  = 5,
    } PXMTupleTypes;
    
    void PXMIdentifyFileType(PCMFile *PCM, BitBuffer *BitB);
    
    void PXMParseMetadata(PCMFile *PCM, BitBuffer *BitB);
    
    void PXMExtractPixels(PCMFile *PCM, BitBuffer *BitB, uint64_t NumPixels2Read, uint16_t **ExtractedPixels);
    
    void PXMInsertPixels(PCMFile *PCM, BitBuffer *OutputPixels, uint32_t NumPixels2Write, uint16_t **Pixels2Write);
    
#ifdef __cplusplus
}
#endif

#endif /* LIBPCM_PXMCommon_H */
