#include "../../libModernAVC.h"

#pragma once

#ifndef LIBMODERNAVC_DecodeSlice_H
#define LIBMODERNAVC_DecodeSlice_H

#ifdef __cplusplus
extern "C" {
#endif
    
    void ParseSliceHeader(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSliceData(DecodeAVC *Dec, BitBuffer *BitB, uint8_t Category);
    
    void ParseAVC3DSlice(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseScalableSlice(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseNALAuxiliarySliceExtension(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseScalableSliceData(DecodeAVC *Dec, BitBuffer *BitB);
    
#ifdef __cplusplus
}
#endif

#endif /* LIBMODERNAVC_DecodeSlice_H */
