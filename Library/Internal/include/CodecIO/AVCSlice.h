#include "../../../Dependencies/BitIO/libBitIO/include/BitIO.h"

#include "../libModernAVC.h"

#pragma once

#ifndef LIBMODERNAVC_DECODESLICE_H
#define LIBMODERNAVC_DECODESLICE_H

#ifdef __cplusplus
extern "C" {
#endif
    
    void ParseSliceHeader(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSliceData(AVCFile *AVC, BitBuffer *BitB, uint8_t Category);
    
    void ParseAVC3DSlice(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseScalableSlice(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseNALAuxiliarySliceExtension(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseScalableSliceData(AVCFile *AVC, BitBuffer *BitB);
    
#ifdef __cplusplus
}
#endif

#endif /* LIBMODERNAVC_DECODESLICE_H */
