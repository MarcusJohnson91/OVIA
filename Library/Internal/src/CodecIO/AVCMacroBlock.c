#include "../../../Dependencies/BitIO/libBitIO/include/BitIO.h"

#include "../../include/ModernAVCTypes.h"
#include "../../include/Common.h"
#include "../../include/Decode/DecodeAVC.h"
#include "../../include/Decode/DecodeMacroBlock.h"
#include "../../include/Decode/DecodeSlice.h"
#include "../../include/Decode/ParseNAL.h"
#include "../../include/Tables.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void ParseMacroBlockLayerInSVC(AVCFile *AVC, BitBuffer *BitB) { // macroblock_layer_in_scalable_extension
        if (AVC->PPS->EntropyCodingMode == ExpGolomb) {
            if (InCropWindow(AVC->Slice->CurrentMacroBlockAddress) && (AVC->Slice->AdaptiveBaseModeFlag == true)) {
                AVC->MacroBlock->BaseModeFlag = ReadBits(BitB, 1, true); // base_mode_flag
            }
            if (AVC->MacroBlock->BaseModeFlag == false) {
                AVC->MacroBlock->Type = ReadExpGolomb(BitB, false);
            }
            if (AVC->MacroBlock->Type == I_PCM) {
                SkipBits(BitB, 1); // pcm_alignment_zero_bit
                CalculateMacroBlockDimensions(AVC);
                for (uint16_t Sample = 0; Sample < MacroBlockMaxPixels; Sample++) {
                    AVC->MacroBlock->PCMLumaSample[Sample]   = ReadBits(BitB, AVC->SPS->LumaBitDepthMinus8, true);
                }
                for (uint8_t Channel = 0; Channel < 2; Channel++) {
                    for (uint8_t Sample = 0; Sample < (2 * (AVC->SPS->MacroBlockWidthChroma * AVC->SPS->MacroBlockHeightChroma)); Sample++) {
                        AVC->MacroBlock->PCMChromaSample[Channel][Sample] = ReadBits(BitB, AVC->SPS->ChromaBitDepthMinus8, true);
                    }
                }
            } else {
                if (AVC->MacroBlock->BaseModeFlag == false) {
                    AVC->MacroBlock->NoMBSmallerThan8x8Flag = 1;
                    if ((AVC->MacroBlock->Type != I_NxN) && (MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) != Intra_16x16) && NumMacroBlockPartitions(AVC->MacroBlock->Type) == 4) {
                        ParseSubMacroBlockPredictionInSVC(BitB, AVC);
                        for (uint8_t MBPartitionIndex = 0; MBPartitionIndex < 4; MBPartitionIndex++) {
                            if (AVC->MacroBlock->SubMacroBlockType[MBPartitionIndex] != B_Direct_8x8) {
                                if (NumberOfSubMacroBlockPartitions(AVC->MacroBlock->SubMacroBlockType[MBPartitionIndex]) > 1) {
                                    AVC->MacroBlock->NoMBSmallerThan8x8Flag = 0;
                                }
                            } else if (AVC->SPS->Inference8x8 == false) {
                                AVC->MacroBlock->NoMBSmallerThan8x8Flag = 0;
                            }
                        }
                    } else {
                        if (AVC->PPS->TransformIs8x8 == true && AVC->MacroBlock->Type == I_NxN) {
                            AVC->MacroBlock->TransformSizeIs8x8 = ReadBits(BitB, 1, true);
                        }
                        ParseMBPredictionInSVC(AVC, BitB);
                    }
                }
                if ((AVC->Slice->AdaptiveResidualPredictionFlag == true) && ((AVC->Slice->Type != SliceEI1) || (AVC->Slice->Type != SliceEI2)) && InCropWindow(AVC->Slice->CurrentMacroBlockAddress) && (AVC->MacroBlock->BaseModeFlag || (MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) != Intra_16x16 && MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) != Intra_8x8 && MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) != Intra_4x4))) {
                    AVC->Slice->ResidualPredictionFlag = ReadBits(BitB, 1, true);
                }
                if (AVC->Slice->ScanIndexEnd >= AVC->Slice->ScanIndexStart) {
                    if (AVC->MacroBlock->BaseModeFlag || MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) != Intra_16x16) {
                        AVC->MacroBlock->BlockPattern = ReadExpGolomb(BitB, false); // MappedExp
                        if (AVC->MacroBlock->BlockPatternLuma > 0 && AVC->PPS->TransformIs8x8 && (AVC->MacroBlock->BaseModeFlag || (AVC->MacroBlock->Type != I_NxN && AVC->MacroBlock->NoMBSmallerThan8x8Flag && (AVC->MacroBlock->Type != B_Direct_16x16 || AVC->SPS->Inference8x8)))) {
                            AVC->MacroBlock->TransformSizeIs8x8 = ReadBits(BitB, 1, true);
                        }
                    }
                    if (AVC->MacroBlock->BlockPatternLuma > 0 || AVC->MacroBlock->BlockPatternChroma > 0 || MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) == Intra_16x16) {
                        AVC->MacroBlock->QPDelta = ReadExpGolomb(BitB, true);
                        residual(AVC->Slice->ScanIndexStart, AVC->Slice->ScanIndexEnd);
                    }
                }
            }
        } else if (AVC->PPS->EntropyCodingMode == Arithmetic) {
            if (InCropWindow(AVC->Slice->CurrentMacroBlockAddress) && (AVC->Slice->AdaptiveBaseModeFlag == true)) {
                AVC->MacroBlock->BaseModeFlag = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
            }
            if (AVC->MacroBlock->BaseModeFlag == false) {
                AVC->MacroBlock->Type = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
            }
            if (AVC->MacroBlock->Type == I_PCM) {
                SkipBits(BitB, 1);
                CalculateMacroBlockDimensions(AVC);
                for (uint16_t Sample = 0; Sample < MacroBlockMaxPixels; Sample++) {
                    AVC->MacroBlock->PCMLumaSample[Sample]   = ReadBits(BitB, AVC->SPS->LumaBitDepthMinus8, true);
                }
                for (uint8_t Channel = 0; Channel < 2; Channel++) {
                    for (uint8_t Sample = 0; Sample < (2 * (AVC->SPS->MacroBlockWidthChroma * AVC->SPS->MacroBlockHeightChroma)); Sample++) {
                        AVC->MacroBlock->PCMChromaSample[Channel][Sample] = ReadBits(BitB, AVC->SPS->ChromaBitDepthMinus8, true);
                    }
                }
            } else {
                if (AVC->MacroBlock->BaseModeFlag == false) {
                    AVC->MacroBlock->NoMBSmallerThan8x8Flag = 1;
                    if ((AVC->MacroBlock->Type != I_NxN) && (MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) != Intra_16x16) && NumMacroBlockPartitions(AVC->MacroBlock->Type) == 4) {
                        ParseSubMacroBlockPredictionInSVC(BitB, AVC);
                        for (uint8_t MBPartitionIndex = 0; MBPartitionIndex < 4; MBPartitionIndex++) {
                            if (AVC->MacroBlock->SubMacroBlockType[MBPartitionIndex] != B_Direct_8x8) {
                                if (NumberOfSubMacroBlockPartitions(AVC->MacroBlock->SubMacroBlockType[MBPartitionIndex]) > 1) {
                                    AVC->MacroBlock->NoMBSmallerThan8x8Flag = 0;
                                }
                            } else if (AVC->SPS->Inference8x8 == false) {
                                AVC->MacroBlock->NoMBSmallerThan8x8Flag = 0;
                            }
                        }
                    } else {
                        if (AVC->PPS->TransformIs8x8 == true && AVC->MacroBlock->Type == I_NxN) {
                            AVC->MacroBlock->TransformSizeIs8x8 = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                        ParseMBPredictionInSVC(AVC, BitB);
                    }
                }
                if ((AVC->Slice->AdaptiveResidualPredictionFlag == true) && ((AVC->Slice->Type != SliceEI1) || (AVC->Slice->Type != SliceEI2)) && InCropWindow(AVC->Slice->CurrentMacroBlockAddress) && (AVC->MacroBlock->BaseModeFlag || (MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) != Intra_16x16 && MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) != Intra_8x8 && MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) != Intra_4x4))) {
                    AVC->Slice->ResidualPredictionFlag = AVC->PPS->EntropyCodingMode = ExpGolomb ? ReadBits(BitB, 1, true) : ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                }
                if (AVC->Slice->ScanIndexEnd >= AVC->Slice->ScanIndexStart) {
                    if (AVC->MacroBlock->BaseModeFlag || MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) != Intra_16x16) {
                        AVC->MacroBlock->BlockPattern = ReadArithmetic(BitB, NULL, NULL, NULL, NULL); // ME or Arithmetic
                        if (AVC->MacroBlock->BlockPatternLuma > 0 && AVC->PPS->TransformIs8x8 && (AVC->MacroBlock->BaseModeFlag || (AVC->MacroBlock->Type != I_NxN && AVC->MacroBlock->NoMBSmallerThan8x8Flag && (AVC->MacroBlock->Type != B_Direct_16x16 || AVC->SPS->Inference8x8)))) {
                            AVC->MacroBlock->TransformSizeIs8x8 = ReadBits(BitB, 1, true); // OR Arithmetic
                        }
                    }
                    if (AVC->MacroBlock->BlockPatternLuma > 0 || AVC->MacroBlock->BlockPatternChroma > 0 || MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) == Intra_16x16) {
                        AVC->MacroBlock->QPDelta = AVC->PPS->EntropyCodingMode = ExpGolomb ? ReadExpGolomb(BitB, true) : ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        residual(AVC->Slice->ScanIndexStart, AVC->Slice->ScanIndexEnd);
                    }
                }
            }
        }
    }
    
    void CalculateMacroBlockDimensions(AVCFile *AVC) {
        if ((AVC->SPS->ChromaFormatIDC == ChromaBW) && (AVC->Slice->SeperateColorPlaneFlag == false)) {
            AVC->SPS->MacroBlockWidthChroma  = 0;
            AVC->SPS->MacroBlockHeightChroma = 0;
        } else if ((AVC->SPS->ChromaFormatIDC == Chroma420) && (AVC->Slice->SeperateColorPlaneFlag == false)) {
            AVC->SPS->MacroBlockWidthChroma  = 8;
            AVC->SPS->MacroBlockHeightChroma = 8;
        } else if ((AVC->SPS->ChromaFormatIDC == Chroma422) && (AVC->Slice->SeperateColorPlaneFlag == false)) {
            AVC->SPS->MacroBlockWidthChroma  = 8;
            AVC->SPS->MacroBlockHeightChroma = 16;
        } else if ((AVC->SPS->ChromaFormatIDC == Chroma444) && (AVC->Slice->SeperateColorPlaneFlag == false)) {
            AVC->SPS->MacroBlockWidthChroma  = 16;
            AVC->SPS->MacroBlockHeightChroma = 16;
        } else if (AVC->Slice->SeperateColorPlaneFlag == true) {
            // Samples need to be handled differently.
        }
    }
    
    void ParseMBPredictionInSVC(AVCFile *AVC, BitBuffer *BitB) { // mb_pred_in_scalable_extension
        if (AVC->PPS->EntropyCodingMode == ExpGolomb) {
            if ((MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) == Intra_4x4) || (MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) == Intra_8x8) || (MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) == Intra_16x16)) {
                if (MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) == Intra_4x4) {
                    for (uint8_t luma4x4BlkIdx = 0; luma4x4BlkIdx < 16; luma4x4BlkIdx++) {
                        AVC->MacroBlock->PreviousIntra4x4PredictionFlag[luma4x4BlkIdx] = ReadBits(BitB, 1, true);
                        if (!AVC->MacroBlock->PreviousIntra4x4PredictionFlag[luma4x4BlkIdx]) {
                            AVC->MacroBlock->RemIntra4x4PredictionMode[luma4x4BlkIdx] = ReadBits(BitB, 3, true);
                        }
                    }
                }
                if (MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) == Intra_8x8) {
                    for (uint8_t luma8x8BlkIdx = 0; luma8x8BlkIdx < 4; luma8x8BlkIdx++) {
                        AVC->MacroBlock->PreviousIntra8x8PredictionFlag[luma8x8BlkIdx] = ReadBits(BitB, 1, true);
                        if (!AVC->MacroBlock->PreviousIntra8x8PredictionFlag[luma8x8BlkIdx]) {
                            AVC->MacroBlock->RemIntra8x8PredictionMode[luma8x8BlkIdx] = ReadBits(BitB, 3, true);
                        }
                    }
                }
                if (AVC->SPS->ChromaArrayType != 0) {
                    AVC->MacroBlock->IntraChromaPredictionMode = ReadExpGolomb(BitB, false);
                }
            } else if (MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) != Direct) {
                if (InCropWindow(AVC->Slice->CurrentMacroBlockAddress) && AVC->Slice->AdaptiveMotionPredictionFlag) {
                    for (uint32_t MBPartitionIndex = 0; MBPartitionIndex < NumMacroBlockPartitions(AVC->MacroBlock->Type); MBPartitionIndex++) {
                        if (MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) != Pred_L1) {
                            AVC->MacroBlock->MotionPredictionFlagLevel0[MBPartitionIndex] = ReadBits(BitB, 1, true);
                        }
                        if (MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) != Pred_L0) {
                            AVC->MacroBlock->MotionPredictionFlagLevel1[MBPartitionIndex] = ReadBits(BitB, 1, true);
                        }
                    }
                }
                for (uint32_t MBPartitionIndex = 0; MBPartitionIndex < NumMacroBlockPartitions(AVC->MacroBlock->Type); MBPartitionIndex++) {
                    if ((AVC->MacroBlock->NumRefIndexActiveLevel0 > 0 || AVC->Slice->MacroBlockFieldDecodingFlag != AVC->Slice->SliceIsInterlaced) && MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 1) != Pred_L1 && !AVC->MacroBlock->MotionPredictionFlagLevel0[MBPartitionIndex]) {
                        AVC->MacroBlock->RefIndexLevel0[MBPartitionIndex] = ReadExpGolomb(BitB, true);
                    }
                    if ((AVC->MacroBlock->NumRefIndexActiveLevel1 > 0 || AVC->Slice->MacroBlockFieldDecodingFlag != AVC->Slice->SliceIsInterlaced) && MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 1) != Pred_L0 && !AVC->MacroBlock->MotionPredictionFlagLevel1[MBPartitionIndex]) {
                        AVC->MacroBlock->RefIndexLevel1[MBPartitionIndex] = ReadExpGolomb(BitB, true);
                    }
                    if (MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, MBPartitionIndex) != Pred_L1) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            AVC->MacroBlock->MVDLevel0[MBPartitionIndex][0][compIdx] = ReadExpGolomb(BitB, true);
                        }
                    }
                    if (MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 1) != Pred_L0) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            AVC->MacroBlock->MVDLevel1[MBPartitionIndex][0][compIdx] = ReadExpGolomb(BitB, true);
                        }
                    }
                }
            }
        } else if (AVC->PPS->EntropyCodingMode == Arithmetic) {
            if ((MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) == Intra_4x4) || (MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) == Intra_8x8) || (MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) == Intra_16x16)) {
                if (MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) == Intra_4x4) {
                    for (uint8_t luma4x4BlkIdx = 0; luma4x4BlkIdx < 16; luma4x4BlkIdx++) {
                        AVC->MacroBlock->PreviousIntra4x4PredictionFlag[luma4x4BlkIdx] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        if (!AVC->MacroBlock->PreviousIntra4x4PredictionFlag[luma4x4BlkIdx]) {
                            AVC->MacroBlock->RemIntra4x4PredictionMode[luma4x4BlkIdx] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                }
                if (MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) == Intra_8x8) {
                    for (uint8_t luma8x8BlkIdx = 0; luma8x8BlkIdx < 4; luma8x8BlkIdx++) {
                        AVC->MacroBlock->PreviousIntra8x8PredictionFlag[luma8x8BlkIdx] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        if (AVC->MacroBlock->PreviousIntra8x8PredictionFlag[luma8x8BlkIdx] == false) {
                            AVC->MacroBlock->RemIntra8x8PredictionMode[luma8x8BlkIdx] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                }
                if (AVC->SPS->ChromaArrayType != 0) {
                    AVC->MacroBlock->IntraChromaPredictionMode = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                }
            } else if (MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) != Direct) {
                if (InCropWindow(AVC->Slice->CurrentMacroBlockAddress) && AVC->Slice->AdaptiveMotionPredictionFlag) {
                    for (uint32_t MBPartitionIndex = 0; MBPartitionIndex < NumMacroBlockPartitions(AVC->MacroBlock->Type); MBPartitionIndex++) {
                        if (MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 1) != Pred_L1) {
                            AVC->MacroBlock->MotionPredictionFlagLevel0[MBPartitionIndex] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                        if (MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 1) != Pred_L0) {
                            AVC->MacroBlock->MotionPredictionFlagLevel1[MBPartitionIndex] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                }
                for (uint32_t MBPartitionIndex = 0; MBPartitionIndex < NumMacroBlockPartitions(AVC->MacroBlock->Type); MBPartitionIndex++) {
                    if ((AVC->MacroBlock->NumRefIndexActiveLevel0 > 0 || AVC->Slice->MacroBlockFieldDecodingFlag != AVC->Slice->SliceIsInterlaced) && MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 1) != Pred_L1 && !AVC->MacroBlock->MotionPredictionFlagLevel0[MBPartitionIndex]) {
                        AVC->MacroBlock->RefIndexLevel0[MBPartitionIndex] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                    }
                    if ((AVC->MacroBlock->NumRefIndexActiveLevel1 > 0 || AVC->Slice->MacroBlockFieldDecodingFlag != AVC->Slice->SliceIsInterlaced) &&
                        MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 1) != Pred_L0 && !AVC->MacroBlock->MotionPredictionFlag[1][MBPartitionIndex]) {
                        AVC->MacroBlock->RefIndexLevel1[MBPartitionIndex] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                    }
                    if (MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, MBPartitionIndex) != Pred_L1) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            AVC->MacroBlock->MVDLevel0[MBPartitionIndex][0][compIdx] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                    if (MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) != Pred_L0) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            AVC->MacroBlock->MVDLevel1[MBPartitionIndex][0][compIdx] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                }
            }
        }
    }
    
    void ParseAVC3DMacroBlockPredictionExtension(AVCFile *AVC, BitBuffer *BitB) {
        if (MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) == Intra_4x4) {
            
        } else if (MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) == Intra_8x8) {
            
        } else if (MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) == Intra_16x16) {
            
        }
    }
    
    uint8_t NumMacroBlockPartitions(uint8_t MacroBlockType) { // NumMbPart
        if (MacroBlockType == 0) {
            return 1;
        } else if ((MacroBlockType == 1) || (MacroBlockType == 2)) {
            return 2;
        } else if ((MacroBlockType == 3) || (MacroBlockType == 4)) {
            return 4;
        } else {
            return 1;
        }
    }
    
    void MacroBlockLayer(AVCFile *AVC, BitBuffer *BitB) { // macroblock_layer
        if (AVC->PPS->EntropyCodingMode == ExpGolomb) {
            AVC->MacroBlock->Type = ReadExpGolomb(BitB, false);
            if (AVC->MacroBlock->Type == I_PCM) { // I_PCM
                AlignInput(BitB, 1);
                for (uint16_t i = 0; i < 256; i++) {
                    AVC->MacroBlock->PCMLumaSample[i] = ReadBits(BitB, AVC->SPS->LumaBitDepthMinus8, true);
                    for (uint8_t j = 0; j < 2; j++) { // FIXME: 2 needs to be a variable of the number of channels
                        AVC->MacroBlock->PCMChromaSample[j][i] = ReadBits(BitB, AVC->SPS->ChromaBitDepthMinus8, true);
                    }
                }
            } else { // NOT I_PCM
                AVC->MacroBlock->NoMBSmallerThan8x8Flag = true;
                uint8_t NumMacroBlockPart = NumMacroBlockPartitions(AVC->MacroBlock->Type);
                if ((AVC->MacroBlock->Type != I_NxN) && (MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) != Intra_16x16) && (NumMacroBlockPart == 4)) {
                    ParseSubMacroBlockPrediction(AVC, BitB, AVC->MacroBlock->Type);
                    for (uint8_t MBPartitionIndex = 0; MBPartitionIndex < 4; MBPartitionIndex++) {
                        if (AVC->MacroBlock->SubMacroBlockType[MBPartitionIndex] != B_Direct_8x8) {
                            if (NumberOfSubMacroBlockPartitions(AVC->MacroBlock->SubMacroBlockType[MBPartitionIndex]) > 1) {
                                AVC->MacroBlock->NoMBSmallerThan8x8Flag = 0;
                            }
                        } else if (AVC->SPS->Inference8x8 == false) {
                            AVC->MacroBlock->NoMBSmallerThan8x8Flag = 0;
                        }
                    }
                } else {
                    if (AVC->PPS->TransformIs8x8 && (AVC->MacroBlock->Type == I_NxN)) {
                        AVC->MacroBlock->TransformSizeIs8x8 = ReadBits(BitB, 1, true);
                    }
                    mb_pred(AVC, BitB, AVC->MacroBlock->Type);
                }
                if (MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) != Intra_16x16) {
                    AVC->MacroBlock->BlockPattern = ReadExpGolomb(BitB, false); // TODO: Add table lookup
                    if (AVC->MacroBlock->BlockPattern == true && AVC->PPS->TransformIs8x8 && (AVC->MacroBlock->Type != I_NxN) && AVC->MacroBlock->NoMBSmallerThan8x8Flag && (AVC->MacroBlock->Type != B_Direct_16x16 || AVC->SPS->Inference8x8)) {
                        AVC->MacroBlock->TransformSizeIs8x8 = ReadBits(BitB, 1, true);
                    }
                    if (AVC->MacroBlock->BlockPatternLuma > 0 || AVC->MacroBlock->BlockPatternChroma > 0 || MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) == Intra_16x16) {
                        AVC->MacroBlock->QPDelta = ReadExpGolomb(BitB, true);
                        residual(0, 15);
                    }
                }
            }
        } else if (AVC->PPS->EntropyCodingMode == Arithmetic) {
            AVC->MacroBlock->Type = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
            if (AVC->MacroBlock->Type == I_PCM) { // I_PCM
                AlignInput(BitB, 1);
                for (uint16_t i = 0; i < 256; i++) {
                    AVC->MacroBlock->PCMLumaSample[i] = ReadBits(BitB, AVC->SPS->LumaBitDepthMinus8, true);
                    for (uint8_t j = 0; j < 2; j++) { // FIXME: 2 needs to be a variable of the number of channels
                        AVC->MacroBlock->PCMChromaSample[j][i] = ReadBits(BitB, AVC->SPS->ChromaBitDepthMinus8, true);
                    }
                }
            } else { // NOT I_PCM
                AVC->MacroBlock->NoMBSmallerThan8x8Flag = true;
                uint8_t NumMacroBlockPart = NumMacroBlockPartitions(AVC->MacroBlock->Type);
                if ((AVC->MacroBlock->Type != I_NxN) && (MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) != Intra_16x16) && (NumMacroBlockPart == 4)) {
                    ParseSubMacroBlockPrediction(AVC, BitB, AVC->MacroBlock->Type);
                    for (uint8_t MBPartitionIndex = 0; MBPartitionIndex < 4; MBPartitionIndex++) {
                        if (AVC->MacroBlock->SubMacroBlockType[MBPartitionIndex] != B_Direct_8x8) {
                            if (NumberOfSubMacroBlockPartitions(AVC->MacroBlock->SubMacroBlockType[MBPartitionIndex]) > 1) {
                                AVC->MacroBlock->NoMBSmallerThan8x8Flag = 0;
                            }
                        } else if (AVC->SPS->Inference8x8 == false) {
                            AVC->MacroBlock->NoMBSmallerThan8x8Flag = 0;
                        }
                    }
                } else {
                    if (AVC->PPS->TransformIs8x8 && (AVC->MacroBlock->Type == I_NxN)) {
                        AVC->MacroBlock->TransformSizeIs8x8 = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                    }
                    mb_pred(AVC, BitB, AVC->MacroBlock->Type);
                }
                if (MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) != Intra_16x16) {
                    AVC->MacroBlock->BlockPattern = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                    if (AVC->MacroBlock->BlockPattern == true && AVC->PPS->TransformIs8x8 && (AVC->MacroBlock->Type != I_NxN) && AVC->MacroBlock->NoMBSmallerThan8x8Flag && (AVC->MacroBlock->Type != B_Direct_16x16 || AVC->SPS->Inference8x8)) {
                        AVC->MacroBlock->TransformSizeIs8x8 = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                    }
                    if (AVC->MacroBlock->BlockPatternLuma > 0 || AVC->MacroBlock->BlockPatternChroma > 0 || MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) == Intra_16x16) {
                        AVC->MacroBlock->QPDelta = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        residual(0, 15);
                    }
                }
            }
        }
    }
    
    void mb_pred(AVCFile *AVC, BitBuffer *BitB, uint8_t mb_type) { // mb_pred
        if (AVC->PPS->EntropyCodingMode == ExpGolomb) {
            uint8_t MacroBlockPredictionMode = MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0);
            if ((MacroBlockPredictionMode == Intra_4x4) || (MacroBlockPredictionMode == Intra_8x8) || (MacroBlockPredictionMode == Intra_16x16)) {
                if (MacroBlockPredictionMode == Intra_4x4) {
                    for (uint8_t Luma4x4Block = 0; Luma4x4Block < 16; Luma4x4Block++) { // luma4x4BlkIdx
                        AVC->MacroBlock->PreviousIntra4x4PredictionFlag[Luma4x4Block] = ReadBits(BitB, 1, true);
                        if (AVC->MacroBlock->PreviousIntra4x4PredictionFlag[Luma4x4Block] == false) {
                            AVC->MacroBlock->RemIntra4x4PredictionMode[Luma4x4Block] = ReadBits(BitB, 3, true);
                        }
                    }
                }
                if (MacroBlockPredictionMode == Intra_8x8) {
                    for (uint8_t Luma8x8Block = 0; Luma8x8Block < 4; Luma8x8Block++) { // luma8x8BlkIdx
                        AVC->MacroBlock->PreviousIntra8x8PredictionFlag[Luma8x8Block] = ReadBits(BitB, 1, true);
                        if (AVC->MacroBlock->PreviousIntra8x8PredictionFlag[Luma8x8Block] == false) {
                            AVC->MacroBlock->RemIntra8x8PredictionMode[Luma8x8Block] = ReadBits(BitB, 3, true);
                        }
                    }
                }
                if ((AVC->SPS->ChromaArrayType == Chroma420) || (AVC->SPS->ChromaArrayType == Chroma422)) {
                    AVC->MacroBlock->IntraChromaPredictionMode = ReadExpGolomb(BitB, false);
                }
            } else if (MacroBlockPredictionMode != Direct) {
                for (uint8_t MBPartitionIndex = 0; MBPartitionIndex < NumMacroBlockPartitions(AVC->MacroBlock->Type); MBPartitionIndex++) {
                    if ((AVC->MacroBlock->NumRefIndexActiveLevel0 > 0 || AVC->Slice->MacroBlockFieldDecodingFlag != AVC->Slice->SliceIsInterlaced) && MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) != Pred_L1) {
                        AVC->MacroBlock->RefIndexLevel0[MBPartitionIndex] = ReadRICE(BitB, true, 0);
                    }
                }
                for (uint32_t MBPartitionIndex = 0; MBPartitionIndex < NumMacroBlockPartitions(AVC->MacroBlock->Type); MBPartitionIndex++) {
                    if ((AVC->MacroBlock->NumRefIndexActiveLevel1 > 0 || AVC->Slice->MacroBlockFieldDecodingFlag != AVC->Slice->SliceIsInterlaced) && MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) != Pred_L0) {
                        AVC->MacroBlock->RefIndexLevel1[MBPartitionIndex] = ReadRICE(BitB, true, 0);
                    }
                }
                for (uint32_t MBPartitionIndex = 0; MBPartitionIndex < NumMacroBlockPartitions(AVC->MacroBlock->Type); MBPartitionIndex++) {
                    if (MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) != Pred_L1) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            AVC->MacroBlock->MVDLevel0[MBPartitionIndex][0][compIdx] = ReadExpGolomb(BitB, true);
                        }
                    }
                }
                for (uint32_t MBPartitionIndex = 0; MBPartitionIndex < NumMacroBlockPartitions(AVC->MacroBlock->Type); MBPartitionIndex++) {
                    if (MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) != Pred_L0) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            AVC->MacroBlock->MVDLevel1[MBPartitionIndex][0][compIdx] = ReadExpGolomb(BitB, true);
                        }
                    }
                }
            }
        } else if (AVC->PPS->EntropyCodingMode == Arithmetic) {
            uint8_t MacroBlockPredictionMode = MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0);
            if ((MacroBlockPredictionMode == Intra_4x4) || (MacroBlockPredictionMode == Intra_8x8) || (MacroBlockPredictionMode == Intra_16x16)) {
                if (MacroBlockPredictionMode == Intra_4x4) {
                    for (uint8_t Luma4x4Block = 0; Luma4x4Block < 16; Luma4x4Block++) { // luma4x4BlkIdx
                        AVC->MacroBlock->PreviousIntra4x4PredictionFlag[Luma4x4Block] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        if (AVC->MacroBlock->PreviousIntra4x4PredictionFlag[Luma4x4Block] == false) {
                            AVC->MacroBlock->RemIntra4x4PredictionMode[Luma4x4Block] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                }
                if (MacroBlockPredictionMode == Intra_8x8) {
                    for (uint8_t Luma8x8Block = 0; Luma8x8Block < 4; Luma8x8Block++) { // luma8x8BlkIdx
                        AVC->MacroBlock->PreviousIntra8x8PredictionFlag[Luma8x8Block] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        if (AVC->MacroBlock->PreviousIntra8x8PredictionFlag[Luma8x8Block] == false) {
                            AVC->MacroBlock->RemIntra8x8PredictionMode[Luma8x8Block] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                }
                if ((AVC->SPS->ChromaArrayType == Chroma420) || (AVC->SPS->ChromaArrayType == Chroma422)) {
                    AVC->MacroBlock->IntraChromaPredictionMode = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                }
            } else if (MacroBlockPredictionMode != Direct) {
                for (uint8_t MBPartitionIndex = 0; MBPartitionIndex < NumMacroBlockPartitions(AVC->MacroBlock->Type); MBPartitionIndex++) {
                    if ((AVC->MacroBlock->NumRefIndexActiveLevel0 > 0 || AVC->Slice->MacroBlockFieldDecodingFlag != AVC->Slice->SliceIsInterlaced) && MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) != Pred_L1) {
                        AVC->MacroBlock->RefIndexLevel0[MBPartitionIndex] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                    }
                }
                for (uint32_t MBPartitionIndex = 0; MBPartitionIndex < NumMacroBlockPartitions(AVC->MacroBlock->Type); MBPartitionIndex++) {
                    if ((AVC->MacroBlock->NumRefIndexActiveLevel1 > 0 || AVC->Slice->MacroBlockFieldDecodingFlag != AVC->Slice->SliceIsInterlaced) && MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) != Pred_L0) {
                        AVC->MacroBlock->RefIndexLevel1[MBPartitionIndex] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                    }
                }
                for (uint32_t MBPartitionIndex = 0; MBPartitionIndex < NumMacroBlockPartitions(AVC->MacroBlock->Type); MBPartitionIndex++) {
                    if (MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) != Pred_L1) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            AVC->MacroBlock->MVDLevel0[MBPartitionIndex][0][compIdx] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                }
                for (uint32_t MBPartitionIndex = 0; MBPartitionIndex < NumMacroBlockPartitions(AVC->MacroBlock->Type); MBPartitionIndex++) {
                    if (MacroBlockPartitionPredictionMode(AVC, AVC->MacroBlock->Type, 0) != Pred_L0) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            AVC->MacroBlock->MVDLevel1[MBPartitionIndex][0][compIdx] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                }
            }
        }
        AVC->Slice->PicHeightInMapUnits      = AVC->SPS->PicHeightInMapUnitsMinus1 + 1;
        AVC->Slice->FrameHeightInMacroBlocks = (2 - AVC->SPS->OnlyMacroBlocksInFrame) * AVC->Slice->PicHeightInMapUnits;
        AVC->Slice->PicWidthInMacroBlocks    = AVC->SPS->PicWidthInMacroBlocksMinus1 + 1;
        AVC->Slice->PicWidthInSamplesLuma    = AVC->Slice->PicWidthInMacroBlocks * 16;
        AVC->Slice->PicWidthInSamplesChroma  = AVC->Slice->PicWidthInMacroBlocks * AVC->SPS->MacroBlockWidthChroma;
        AVC->Slice->PicHeightInMacroBlocks   = AVC->Slice->FrameHeightInMacroBlocks / (AVC->Slice->SliceIsInterlaced + 1);
        AVC->Slice->RawMacroBlockSizeInBits  = 256 * AVC->SPS->BitDepthLuma + 2 * AVC->SPS->MacroBlockWidthChroma * AVC->SPS->MacroBlockHeightChroma * AVC->SPS->BitDepthChroma;
        AVC->SPS->QPBDOffsetChroma           = AVC->SPS->ChromaBitDepthMinus8 * 6;
    }
    
    void DecodeMacroBlock(AVCFile *AVC, uint8_t *MacroBlock, size_t MacroBlockSize) {
        if (AVC->SPS->ChromaFormatIDC == ChromaBW) { // black and white
            
        } else if (AVC->SPS->ChromaFormatIDC == Chroma420) { // 4:2:0
            
        } else if (AVC->SPS->ChromaFormatIDC == Chroma422) { // 4:2:2
            
        } else if (AVC->SPS->ChromaFormatIDC == Chroma444) { // 4:4:4
            
        }
    }
    
    void ParseSubMacroBlockPredictionInSVC(AVCFile *AVC, BitBuffer *BitB) { // sub_mb_pred_in_scalable_extension
        if (AVC->PPS->EntropyCodingMode == ExpGolomb) {
            for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) { // MBPartitionIndex
                AVC->MacroBlock->SubMacroBlockType[MacroBlockPiece] = ReadExpGolomb(BitB, false);
            }
            if ((InCropWindow(AVC->Slice->CurrentMacroBlockAddress) == true) && (AVC->Slice->AdaptiveMotionPredictionFlag == true)) {
                for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) {
                    if (SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Direct && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L1) {
                        AVC->MacroBlock->MotionPredictionFlagLevel0[MacroBlockPiece] = ReadBits(BitB, 1, true);
                    }
                    if (SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Direct && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L0) {
                        AVC->MacroBlock->MotionPredictionFlagLevel1[MacroBlockPiece] = ReadBits(BitB, 1, true);
                    }
                }
            }
            for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) { // MBPartitionIndex
                if ((AVC->MacroBlock->NumRefIndexActiveLevel0 > 0 || AVC->Slice->MacroBlockFieldDecodingFlag != AVC->Slice->SliceIsInterlaced) && AVC->MacroBlock->Type != P_8x8ref0 && SubMacroBlockType[MBPartitionIndex] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MBPartitionIndex]) != Pred_L1 && !motion_prediction_flag_l0[MBPartitionIndex]) {
                    
                    AVC->SPS->RefIndex[0][MacroBlockPiece] = ReadRICE(BitB, true, 0) + 1;
                }
                if ((AVC->MacroBlock->NumRefIndexActiveLevel1 > 0 || AVC->Slice->MacroBlockFieldDecodingFlag != AVC->Slice->SliceIsInterlaced) && SubMacroBlockType[MBPartitionIndex] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MBPartitionIndex]) != Pred_L0 && !motion_prediction_flag_l1[MBPartitionIndex]) {
                    
                    AVC->SPS->RefIndex[1][MacroBlockPiece] = ReadRICE(BitB, true, 0) + 1;
                    
                }
                
                if (SubMacroBlockType[MBPartitionIndex] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MBPartitionIndex]) != Pred_L1) {
                    for (uint32_t SubMBPartitionIndex = 0; SubMBPartitionIndex < NumSubMbPart(SubMacroBlockType[MBPartitionIndex]); SubMBPartitionIndex++) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            AVC->MacroBlock->MVDLevel0[MBPartitionIndex][SubMBPartitionIndex][compIdx] = ReadExpGolomb(BitB, true);
                        }
                    }
                }
                
                if (SubMacroBlockType[MBPartitionIndex] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MBPartitionIndex]) != Pred_L0) {
                    for (uint32_t SubMBPartitionIndex = 0; SubMBPartitionIndex < NumSubMbPart(SubMacroBlockType[MBPartitionIndex]); SubMBPartitionIndex++) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            AVC->MacroBlock->MVDLevel1[MBPartitionIndex][SubMBPartitionIndex][compIdx] = ReadExpGolomb(BitB, true);
                        }
                    }
                }
            }
        } else if (AVC->PPS->EntropyCodingMode == Arithmetic) {
            for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) { // MBPartitionIndex
                AVC->MacroBlock->SubMacroBlockType[MacroBlockPiece] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
            }
            if ((InCropWindow(AVC->Slice->CurrentMacroBlockAddress) == true) && (AVC->Slice->AdaptiveMotionPredictionFlag == true)) {
                for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) {
                    if (SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Direct && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L1) {
                        AVC->MacroBlock->MotionPredictionFlagLevel0[MacroBlockPiece] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                    }
                    if (SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Direct && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L0) {
                        AVC->MacroBlock->MotionPredictionFlagLevel1[MacroBlockPiece] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                    }
                }
            }
            for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) { // MBPartitionIndex
                if ((AVC->MacroBlock->NumRefIndexActiveLevel0 > 0 || AVC->Slice->MacroBlockFieldDecodingFlag != AVC->Slice->SliceIsInterlaced) && AVC->MacroBlock->Type != P_8x8ref0 && SubMacroBlockType[MBPartitionIndex] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MBPartitionIndex]) != Pred_L1 && !motion_prediction_flag_l0[MBPartitionIndex]) {
                    
                    AVC->SPS->RefIndex[0][MacroBlockPiece] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL) + 1;
                    
                }
                if ((AVC->MacroBlock->NumRefIndexActiveLevel1 > 0 || AVC->Slice->MacroBlockFieldDecodingFlag != AVC->Slice->SliceIsInterlaced) && SubMacroBlockType[MBPartitionIndex] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MBPartitionIndex]) != Pred_L0 && !motion_prediction_flag_l1[MBPartitionIndex]) {
                    
                    AVC->SPS->RefIndex[1][MacroBlockPiece] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL) + 1;
                    
                }
                
                if (SubMacroBlockType[MBPartitionIndex] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MBPartitionIndex]) != Pred_L1) {
                    for (uint32_t SubMBPartitionIndex = 0; SubMBPartitionIndex < NumSubMbPart(SubMacroBlockType[MBPartitionIndex]); SubMBPartitionIndex++) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            AVC->MacroBlock->MVDLevel0[MBPartitionIndex][SubMBPartitionIndex][compIdx] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                }
                
                if (SubMacroBlockType[MBPartitionIndex] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MBPartitionIndex]) != Pred_L0) {
                    for (uint32_t SubMBPartitionIndex = 0; SubMBPartitionIndex < NumSubMbPart(SubMacroBlockType[MBPartitionIndex]); SubMBPartitionIndex++) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            AVC->MacroBlock->MVDLevel1[MBPartitionIndex][SubMBPartitionIndex][compIdx] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                }
            }
        }
    }
    
    void ParseReferenceBasePictureSyntax(AVCFile *AVC, BitBuffer *BitB) { // dec_ref_base_pic_marking
        AVC->SVC->AdaptiveMarkingModeFlag = ReadBits(BitB, 1, true);
        if (AVC->SVC->AdaptiveMarkingModeFlag == true) {
            AVC->SVC->BaseControlOperation = ReadExpGolomb(BitB, false);
            if (AVC->SVC->BaseControlOperation == 1) {
                AVC->SVC->NumBasePicDiff = ReadExpGolomb(BitB, false) + 1;
            } else if (AVC->SVC->BaseControlOperation == 2) {
                AVC->SVC->LongTermBasePicNum = ReadExpGolomb(BitB, false);
            } while(AVC->SVC->BaseControlOperation != 0) {
                
            }
        }
    }
    
    void ParseSubMacroBlockPrediction(AVCFile *AVC, BitBuffer *BitB, uint8_t mb_type) { // sub_mb_pred
        if (AVC->PPS->EntropyCodingMode == ExpGolomb) {
            for (uint8_t MBPartitionIndex = 0; MBPartitionIndex < 4; MBPartitionIndex++) {
                SubMacroBlockType[MBPartitionIndex] = ReadExpGolomb(BitB, false);
            }
            for (uint8_t MBPartitionIndex = 0; MBPartitionIndex < 4; MBPartitionIndex++) {
                if ((AVC->MacroBlock->NumRefIndexActiveLevel0 > 0 || AVC->Slice->MacroBlockFieldDecodingFlag != AVC->Slice->SliceIsInterlaced) && AVC->MacroBlock->Type != P_8x8ref0 && SubMacroBlockType[MBPartitionIndex] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MBPartitionIndex]) != Pred_L1) {
                    AVC->MacroBlock->RefIndexLevel0[MBPartitionIndex] = ReadRICE(BitB, true, 0) + 1;
                }
            }
            for (uint8_t MBPartitionIndex = 0; MBPartitionIndex < 4; MBPartitionIndex++) {
                if ((AVC->MacroBlock->NumRefIndexActiveLevel1 > 0 || AVC->Slice->MacroBlockFieldDecodingFlag != AVC->Slice->SliceIsInterlaced) && SubMacroBlockType[MBPartitionIndex] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MBPartitionIndex]) != Pred_L0) {
                    AVC->MacroBlock->RefIndexLevel1[MBPartitionIndex] = ReadRICE(BitB, true, 0) + 1;
                }
            }
            for (uint8_t MBPartitionIndex = 0; MBPartitionIndex < 4; MBPartitionIndex++) {
                if (SubMacroBlockType[MBPartitionIndex] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MBPartitionIndex]) !=Pred_L1) {
                    for (uint32_t SubMBPartitionIndex = 0; SubMBPartitionIndex < NumSubMbPart(SubMacroBlockType[MBPartitionIndex]);SubMBPartitionIndex++) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            AVC->MacroBlock->MVDLevel0[MBPartitionIndex][SubMBPartitionIndex][compIdx] = ReadExpGolomb(BitB, true);
                        }
                    }
                }
            }
            for (uint8_t MBPartitionIndex = 0; MBPartitionIndex < 4; MBPartitionIndex++) {
                if (SubMacroBlockType[MBPartitionIndex] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MBPartitionIndex]) != Pred_L0) {
                    for (uint32_t SubMBPartitionIndex = 0; SubMBPartitionIndex < NumSubMbPart(SubMacroBlockType[MBPartitionIndex]);SubMBPartitionIndex++) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            AVC->MacroBlock->MVDLevel1[MBPartitionIndex][SubMBPartitionIndex][compIdx] = ReadExpGolomb(BitB, true);
                        }
                    }
                }
            }
        } else if (AVC->PPS->EntropyCodingMode == Arithmetic) {
            for (uint8_t MBPartitionIndex = 0; MBPartitionIndex < 4; MBPartitionIndex++) {
                SubMacroBlockType[MBPartitionIndex] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
            }
            for (uint8_t MBPartitionIndex = 0; MBPartitionIndex < 4; MBPartitionIndex++) {
                if ((AVC->MacroBlock->NumRefIndexActiveLevel0 > 0 || AVC->Slice->MacroBlockFieldDecodingFlag != AVC->Slice->SliceIsInterlaced) && AVC->MacroBlock->Type != P_8x8ref0 && SubMacroBlockType[MBPartitionIndex] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MBPartitionIndex]) != Pred_L1) {
                    AVC->MacroBlock->RefIndexLevel0[MBPartitionIndex] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL) + 1;
                }
            }
            for (uint8_t MBPartitionIndex = 0; MBPartitionIndex < 4; MBPartitionIndex++) {
                if ((AVC->MacroBlock->NumRefIndexActiveLevel1 > 0 || AVC->Slice->MacroBlockFieldDecodingFlag != AVC->Slice->SliceIsInterlaced) && SubMacroBlockType[MBPartitionIndex] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MBPartitionIndex]) != Pred_L0) {
                    AVC->MacroBlock->RefIndexLevel1[MBPartitionIndex] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL) + 1;
                }
            }
            for (uint8_t MBPartitionIndex = 0; MBPartitionIndex < 4; MBPartitionIndex++) {
                if (SubMacroBlockType[MBPartitionIndex] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MBPartitionIndex]) !=Pred_L1) {
                    for (uint32_t SubMBPartitionIndex = 0; SubMBPartitionIndex < NumSubMbPart(SubMacroBlockType[MBPartitionIndex]);SubMBPartitionIndex++) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            AVC->MacroBlock->MVDLevel0[MBPartitionIndex][SubMBPartitionIndex][compIdx] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                }
            }
            for (uint8_t MBPartitionIndex = 0; MBPartitionIndex < 4; MBPartitionIndex++) {
                if (SubMacroBlockType[MBPartitionIndex] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MBPartitionIndex]) != Pred_L0) {
                    for (uint32_t SubMBPartitionIndex = 0; SubMBPartitionIndex < NumSubMbPart(SubMacroBlockType[MBPartitionIndex]);SubMBPartitionIndex++) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            AVC->MacroBlock->MVDLevel1[MBPartitionIndex][SubMBPartitionIndex][compIdx] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                }
            }
        }
    }
    
#ifdef __cplusplus
}
#endif
