#include "../../../Dependencies/BitIO/libBitIO/include/BitIO.h"

#include "../libModernAVC.h"

#pragma once

#ifndef LIBMODERNAVC_DECODEMACROBLOCK_H
#define LIBMODERNAVC_DECODEMACROBLOCK_H

#ifdef __cplusplus
extern "C" {
#endif
    
    void     ParseMacroBlockLayerInSVC(DecodeAVC *Dec, BitBuffer *BitB);
    
    void     CalculateMacroBlockDimensions(DecodeAVC *Dec);
    
    void     ParseMBPredictionInSVC(DecodeAVC *Dec, BitBuffer *BitB);
    
    void     ParseAVC3DMacroBlockPredictionExtension(DecodeAVC *Dec, BitBuffer *BitB);
    
    uint64_t NextMacroBlockAddress(DecodeAVC *Dec, uint64_t CurrentMacroBlockAddress);
    
    uint8_t  NumMacroBlockPartitions(uint8_t MacroBlockType);
    
    void     MacroBlockLayer(DecodeAVC *Dec, BitBuffer *BitB);
    
    void     mb_pred(DecodeAVC *Dec, BitBuffer *BitB, uint8_t mb_type);
    
    void     DecodeMacroBlock(DecodeAVC *Dec, uint8_t *MacroBlock, size_t MacroBlockSize);
    
#ifdef __cplusplus
}
#endif

#endif /* LIBMODERNAVC_DECODEMACROBLOCK_H */
