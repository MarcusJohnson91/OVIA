#include "OVIACommon.h"

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
    
    typedef enum PNMConstants {
        PNMCommentStart       = 0x23,
        PNMFieldSeperator     = 0x20,
        PNMEndField           = 0x0A,
    } PNMConstants;
    
    /*
    enum PNMMagic {
        PNM_PBMA = 0x5031, // PortableBitMapASCII
        PNM_PGMA = 0x5032, // PortableGrayMapASCII
        PNM_PPMA = 0x5033, // PortablePixMapASCII
        
        PNM_PBMB = 0x5034, // PortableBitMapBinary
        PNM_PGMB = 0x5035, // PortableGrayMapBinary
        PNM_PPMB = 0x5036, // PortablePixMapBinary
        
        PNM_PAMB = 0x5037, // PortableAnyMap
    } PNMMagic;
    */
    typedef struct PNMOptions {
        uint64_t      Height;
        uint64_t      Width;
        uint8_t       BitDepth;
        uint8_t       NumChannels;
        PNMTupleTypes TupleType;
        PNMTypes      Type;
    } PNMOptions;
    
    void *PNMOptions_Init(void);
    
    void PNMOptions_Deinit(void *Options);
    
#ifdef __cplusplus
}
#endif

#endif /* OVIA_PNMCommon_H */
