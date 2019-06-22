#include "../../../Dependencies/FoundationIO/libFoundationIO/include/BitIO.h"
#include "../../../Dependencies/FoundationIO/libFoundationIO/include/ContainerIO.h"
#include "../../../Dependencies/FoundationIO/libFoundationIO/include/CryptographyIO.h"
#include "../../../Dependencies/FoundationIO/libFoundationIO/include/Log.h"
#include "../../../Dependencies/FoundationIO/libFoundationIO/include/Math.h"

#pragma once

#ifndef OVIA_PNMCommon_H
#define OVIA_PNMCommon_H

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef enum PNMTypes {
        UnknownPNM            = 0,
        BlackAndWhitePNM      = 1,
        BinaryPNM             = 2,
        ASCIIPNM              = 3,
        PAMPNM                = 4,
    } PNMTypes;
    
    typedef enum PNMTupleTypes {
        PNM_TUPLE_Unknown     = 0,
        PNM_TUPLE_BnW         = 1,
        PNM_TUPLE_Gray        = 2,
        PNM_TUPLE_GrayAlpha   = 3,
        PNM_TUPLE_RGB         = 4,
        PNM_TUPLE_RGBAlpha    = 5,
    } PNMTupleTypes;
    
    enum PNMCommon {
        PNMMagicSize          = 2,
        PNMCommentStart       = 0x23,
        PNMFieldSeperator     = 0x20,
        PNMEndField           = 0x0A,
    } PNMCommon;
    
    enum PNMMagic {
        PNM_PBMA = 0x5031, // PortableBitMapASCII
        PNM_PBMB = 0x5034, // PortableBitMapBinary
        PNM_PGMA = 0x5032, // PortableGrayMapASCII
        PNM_PGMB = 0x5035, // PortableGrayMapBinary
        PNM_PPMA = 0x5033, // PortablePixMapASCII
        PNM_PPMB = 0x5036, // PortablePixMapBinary
        PNM_PAMB = 0x5037, // PortableAnyMap
    } PNMMagic;
    
    typedef struct PNMOptions {
        PNMTupleTypes TupleType;
        PNMTypes      Type;
    } PNMOptions;
    
    void            PNMIdentifyFileType(BitBuffer *BitB);
    
    void            PNMParseMetadata(BitBuffer *BitB);
    
    ImageContainer *PNMExtractImage(BitBuffer *BitB);
    
    void            PNMInsertFrame(ImageContainer *Image, BitBuffer *BitB);
    
    void            PNMWriteHeader(BitBuffer *BitB);
    
    void                 OVIA_PNM_SetPNMType(PNMTypes PNMType);
    void                 OVIA_PNM_SetTupleType(PNMTupleTypes TupleType);
    PNMTupleTypes        OVIA_PNM_GetTupleType(OVIA *Ovia);
    PNMTypes             OVIA_PNM_GetPNMType(OVIA *Ovia);
    
#ifdef __cplusplus
}
#endif

#endif /* OVIA_PNMCommon_H */
