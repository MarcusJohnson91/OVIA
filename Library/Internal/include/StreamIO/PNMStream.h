/*!
 @header              PNMCommon.h
 @author              Marcus Johnson
 @copyright           2017+
 @version             1.0.0
 @brief               This header contains code for reading and writing lossless PNM image files
 */

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
    
    typedef struct PNMOptions {
        uint64_t      Height;
        uint64_t      Width;
        uint8_t       BitDepth;
        uint8_t       NumChannels;
        PNMTupleTypes TupleType;
        PNMTypes      Type;
    } PNMOptions;
    
    void *PNMOptions_Init(void);
    
    void PNMReadHeader(void *Options, BitBuffer *BitB);
    
    void *PNMExtractImage(void *Options, BitBuffer *BitB);
    
    void PNMWriteHeader(void *Options, BitBuffer *BitB);
    
    void PNMInsertImage(void *Options, void *Container, BitBuffer *BitB);
    
    void PNMWriteFooter(void *Options, BitBuffer *BitB);
    
    void PNMOptions_Deinit(void *Options);
    
#ifdef __cplusplus
}
#endif

#endif /* OVIA_PNMCommon_H */
