#include "../../libModernAVC.h"

#pragma once

#ifndef LIBMODERNAVC_DecodeAVC_H
#define LIBMODERNAVC_DecodeAVC_H

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef struct Stream {
        uint64_t LastIDRFrame;
        uint64_t CurrentFrame;
    } Stream;
    
    typedef struct DecodedMacroBlock {
        uint16_t **R;
        uint16_t **G;
        uint16_t **B;
    } DecodedMacroBlock;
    
    size_t   FindNALSize(DecodeAVC *Dec, BitBuffer *BitB);
    
    void     ParseAVCFile(DecodeAVC *Dec, BitBuffer *BitB);
    
    void     ParseAVCHeader(DecodeAVC *Dec, BitBuffer *BitB);
    
    void     ExtractNALFromByteStream(DecodeAVC *Dec, BitBuffer *BitB, size_t NALSize); // FIXME: You don't really do that, so this'll prob have to go
    
    void     DecodeNAL(DecodeAVC *Dec, BitBuffer *BitB, uint8_t *NAL2Decode, size_t NALSize);
    
    uint64_t ScalingList(DecodeAVC *Dec, BitBuffer *BitB, uint8_t *scalingList, size_t ScalingListSize, bool UseDefaultScalingMatrixFlag);
    
    //void     ScalingList4x4();
    
    uint8_t  weightScale4x4(int i, int j);
    
    uint8_t  normAdjust4x4(int m, int i, int j);
    
    uint32_t LevelScale4x4(int m, int i, int j);
    
    uint8_t  weightScale8x8(int i, int j);
    
    uint8_t  normAdjust8x8(int m, int i, int j);
    
    uint8_t  levelScale8x8(int m, int i, int j);
    
    void     AcquisitionElement3DV(DecodeAVC *Dec, BitBuffer *BitB, uint8_t numViews, uint8_t DeltaFlag, uint8_t PredDirection, uint8_t precMode, uint8_t expLen, uint8_t OutSign[MVCMaxViews], uint8_t OutExp, uint8_t OutMantissa, uint8_t OutManLen[MVCMaxViews]);
    
    uint64_t DepthRanges(BitBuffer *BitB, uint64_t NumberOfViews, bool PredDirection, uint64_t Index);
    
    void     vsp_param(DecodeAVC *Dec, BitBuffer *BitB, uint8_t numViews, uint8_t Direction, uint8_t DepthPS);
    
    void     RefPicListMVCMod(DecodeAVC *Dec, BitBuffer *BitB);
    
    void     RefPicListMod(DecodeAVC *Dec, BitBuffer *BitB);
    
    void     pred_weight_table(DecodeAVC *Dec, BitBuffer *BitB);
    
    void     DecodeRefPicMarking(DecodeAVC *Dec, BitBuffer *BitB);
    
    void     sub_mb_pred(DecodeAVC *Dec, BitBuffer *BitB, uint8_t mb_type);
    
    void     ResidualLuma(int i16x16DClevel, int i16x16AClevel, int level4x4, int level8x8, int startIdx, int endIdx);
    
    void     Residual(DecodeAVC *Dec, BitBuffer *BitB, uint64_t StartIdx, uint64_t endIdx);
    
    void     residual_block_cavlc(DecodeAVC *Dec, BitBuffer *BitB, int coeffLevel, int startIdx, int endIdx, int maxNumCoeff);
    
    void     ScanNALUnits(DecodeAVC *Dec, BitBuffer *BitB);
    
    void     scaling_list(BitBuffer *BitB, int scalingList, size_t sizeOfScalingList, bool useDefaultScalingMatrixFlag);
    
    void     ParseReferenceBasePictureSyntax(DecodeAVC *Dec, BitBuffer *BitB);
    
    void     ParseSubMacroBlockPredictionInSVC(DecodeAVC *Dec, BitBuffer *BitB);
    
    void     rbsp_trailing_bits(DecodeAVC *Dec, BitBuffer *BitB);
    
    uint8_t  GetCodedBlockPattern(DecodeAVC *Dec, uint8_t CodeNum);
    
    void     ConvertRGB2YCgCo(DecodeAVC *Dec, uint8_t NumChannels, uint8_t NumPixels, uint16_t **DecodedMB);
    
    uint8_t  SubMacroBlockType(uint8_t SubMacroBlockType, uint8_t MacroBlockPartitionIndex);
    
#ifdef __cplusplus
}
#endif

#endif /* LIBMODERNAVC_DecodeAVC_H */
