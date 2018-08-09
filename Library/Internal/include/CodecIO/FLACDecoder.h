#include "../Common/libModernFLAC_Types.h"

#pragma once

#ifndef libModernFLAC_Decode_H
#define libModernFLAC_Decode_H

#ifdef __cplusplus
extern "C" {
#endif
    
    void DeinitFLACDecoder(DecodeFLAC *Dec);
    
    void FLACReadStream(BitBuffer *InputFLAC, DecodeFLAC *Dec);
    
    void FLACReadFrame(BitBuffer *InputFLAC, DecodeFLAC *Dec);
    
    void FLACReadSubFrame(BitBuffer *InputFLAC, DecodeFLAC *Dec, uint8_t Channel);
    
    void FLACDecodeSubFrameVerbatim(BitBuffer *InputFLAC, DecodeFLAC *Dec);
    
    void FLACDecodeSubFrameConstant(BitBuffer *InputFLAC, DecodeFLAC *Dec);
    
    void FLACDecodeSubFrameFixed(BitBuffer *InputFLAC, DecodeFLAC *Dec);
    
    void FLACDecodeSubFrameLPC(BitBuffer *InputFLAC, DecodeFLAC *Dec, uint8_t Channel);
    
    void DecodeFLACResidual(BitBuffer *InputFLAC, DecodeFLAC *Dec);
    
    void DecodeFLACRice1Partition(BitBuffer *InputFLAC, DecodeFLAC *Dec);
    
    void DecodeFLACRice2Partition(BitBuffer *InputFLAC, DecodeFLAC *Dec);
    
    uint8_t GetBlockSizeInSamples(uint8_t BlockSize);
    
    void FLACBitDepth(DecodeFLAC *Dec);
    
    void FLACSampleRate(BitBuffer *InputFLAC, DecodeFLAC *Dec);
    
    void FLACDecodeLPC(BitBuffer *InputFLAC, DecodeFLAC *Dec);
    
    
#ifdef __cplusplus
}
#endif

#endif /* libModernFLAC_Decode_H */
