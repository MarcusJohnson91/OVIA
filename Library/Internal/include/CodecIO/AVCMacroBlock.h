#include "../../../Dependencies/BitIO/libBitIO/include/BitIO.h"

#include "../libModernAVC.h"

#pragma once

#ifndef LIBMODERNAVC_DECODEMACROBLOCK_H
#define LIBMODERNAVC_DECODEMACROBLOCK_H

#ifdef __cplusplus
extern "C" {
#endif
    
    void     ParseMacroBlockLayerInSVC(AVCFile *AVC, BitBuffer *BitB);
    
    void     CalculateMacroBlockDimensions(AVCFile *AVC);
    
    void     ParseMBPredictionInSVC(AVCFile *AVC, BitBuffer *BitB);
    
    void     ParseAVC3DMacroBlockPredictionExtension(AVCFile *AVC, BitBuffer *BitB);
    
    uint64_t NextMacroBlockAddress(AVCFile *AVC, uint64_t CurrentMacroBlockAddress);
    
    uint8_t  NumMacroBlockPartitions(uint8_t MacroBlockType);
    
    void     MacroBlockLayer(AVCFile *AVC, BitBuffer *BitB);
    
    void     mb_pred(AVCFile *AVC, BitBuffer *BitB, uint8_t mb_type);
    
    void     DecodeMacroBlock(AVCFile *AVC, uint8_t *MacroBlock, size_t MacroBlockSize);
    
#ifdef __cplusplus
}
#endif

#endif /* LIBMODERNAVC_DECODEMACROBLOCK_H */
