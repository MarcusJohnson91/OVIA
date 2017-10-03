#include "../../include/Private/Common/libModernAVC_Types.h"
#include "../../include/Private/Common/libModernAVC_Common.h"
#include "../../include/Private/Decode/libModernAVC_Decode.h"
#include "../../include/Private/Decode/libModernAVC_ParseMacroBlock.h"
#include "../../include/Private/Decode/libModernAVC_ParseSlice.h"
#include "../../include/Private/Decode/libModernAVC_ParseNAL.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    // macroblock-adaptive frame/field decoding is in use, a sequence of macroblock pairs.
    // Slices contain only 1 color channel?
    
    void ParseSliceHeader(DecodeAVC *Dec, BitBuffer *BitB) {
        
    }
    
    // Slice group contains partitioned slices, partitioned by channel. Each slice may contain macroblocks or macroblock pairs (for MVC?).
    // A slice group contains a whole frame?
    
    /* Parse NAL Slice */
    void ParseAVC3DSliceDataExtension(DecodeAVC *Dec, BitBuffer *BitB) { // slice_data_in_3davc_extension
        if (Dec->PPS->EntropyCodingMode == Arithmetic) {
            AlignBitBuffer(BitB, 1);
            Dec->Slice->CurrentMacroBlockAddress = Dec->Slice->FirstMacroBlockInSlice * (Dec->Slice->MbaffFrameFlag + 1);
            bool     MoreDataFlag = true;
            Dec->Slice->PreviousMacroBlockSkipped = false;
            bool     RunLength = 0;
            while (MoreDataFlag == true) {
                if ((Dec->Slice->Type != SliceI1) || (Dec->Slice->Type != SliceI2) || (Dec->Slice->Type != SliceSI1) || (Dec->Slice->Type != SliceSI2)) {
                    if (Dec->PPS->EntropyCodingMode == ExpGolomb) {
                        Dec->Slice->MacroBlockSkipRun = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                        Dec->Slice->PreviousMacroBlockSkipped = (Dec->Slice->MacroBlockSkipRun > 0);
                        for (uint8_t SkippedMacroBlock = 0; SkippedMacroBlock < Dec->Slice->MacroBlockSkipRun; SkippedMacroBlock++) {
                            Dec->Slice->CurrentMacroBlockAddress = NextMacroBlockAddress(Dec, Dec->Slice->CurrentMacroBlockAddress);
                        }
                    }
                }
            }
        }
    }
    
    void ParseAVC3DSlice(DecodeAVC *Dec, BitBuffer *BitB) { // slice_header_in_3davc_extension
        Dec->Slice->FirstMacroBlockInSlice     = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
        Dec->Slice->Type                       = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
        Dec->PPS->PicParamSetID                = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
        if ((Dec->NAL->AVC3DExtensionFlag == true) && (Dec->NAL->AVC3DExtensionFlag == true)) {
            Dec->Slice->PreSliceHeaderSrc      = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 2);
            if ((Dec->Slice->Type == SliceP1) || (Dec->Slice->Type == SliceP2) || (Dec->Slice->Type == SliceSP1) || (Dec->Slice->Type == SliceSP2) || (Dec->Slice->Type == SliceB1) || (Dec->Slice->Type == SliceB2)) {
                Dec->Slice->PreRefListSrc      = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 2);
                if (Dec->Slice->PreRefListSrc == 0) {
                    Dec->Slice->NumRefIDXActiveOverrideFlag= ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                    if (Dec->Slice->NumRefIDXActiveOverrideFlag == true) {
                        Dec->MacroBlock->NumRefIndexActiveLevel0 = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false) + 1;
                    }
                    if ((Dec->Slice->Type == SliceB1) || (Dec->Slice->Type == SliceB2)) {
                        Dec->MacroBlock->NumRefIndexActiveLevel1 = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false) + 1;
                    }
                    RefPicListMVCMod(Dec, BitB);
                }
            }
            if ((Dec->PPS->WeightedPrediction == true && ((Dec->Slice->Type == SliceP1) || (Dec->Slice->Type == SliceP2) || (Dec->Slice->Type == SliceSP1) || (Dec->Slice->Type == SliceSP2))) || ((Dec->PPS->WeightedBiPrediction == 1 && ((Dec->Slice->Type == SliceB1) || (Dec->Slice->Type == SliceB2))))) {
                
                Dec->Slice->PrePredictionWeightTableSrc = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 2);
                if (Dec->Slice->PrePredictionWeightTableSrc == false) {
                    pred_weight_table(Dec, BitB);
                }
            }
            if (Dec->NAL->NALRefIDC != 0) {
                Dec->Slice->PreDecReferencePictureMarkingSrc = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 2);
                if (Dec->Slice->PreDecReferencePictureMarkingSrc == 0) {
                    DecodeRefPicMarking(Dec, BitB);
                }
            }
            Dec->Slice->SliceQPDelta = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, true);
        } else {
            if (Dec->SPS->SeperateColorPlane == true) {
                Dec->Slice->ColorPlaneID   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 2);
            }
            Dec->Slice->FrameNumber     = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 0); // FIXME: READ VARIABLE BITS frame_num
            if (Dec->SPS->OnlyMacroBlocksInFrame == false) {
                Dec->Slice->SliceIsInterlaced = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                if (Dec->Slice->SliceIsInterlaced == true) {
                    Dec->Slice->SliceIsBottomField  = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                }
            }
            if (Dec->Slice->IDRPicID == true) {
                Dec->Slice->IDRPicID = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            }
            if (Dec->SPS->PicOrderCount == false) {
                Dec->Slice->PictureOrderCountLSB = ReadBits(BitIOMSByte, BitIOLSBit, BitB, Dec->SPS->MaxPicOrder);
                if ((Dec->PPS->BottomPicFieldOrderInSliceFlag == true) && (Dec->Slice->SliceIsInterlaced == false)) {
                    Dec->Slice->DeltaPictureOrderCountBottom[1] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, true);
                }
            }
            if ((Dec->SPS->PicOrderCount == true) && (Dec->SPS->DeltaPicOrder == false)) {
                Dec->Slice->DeltaPictureOrderCountBottom[0] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, true);
                if ((Dec->PPS->BottomPicFieldOrderInSliceFlag == true) && (Dec->Slice->SliceIsInterlaced == false)) {
                    Dec->Slice->DeltaPictureOrderCountBottom[1] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, true);
                }
            }
            if (Dec->PPS->RedundantPictureFlag == true) {
                Dec->PPS->RedundantPictureCount = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, true);
            }
            if ((Dec->Slice->Type == SliceB1) || (Dec->Slice->Type == SliceB2)) {
                Dec->Slice->DirectSpatialMVPredictionFlag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            }
            if ((Dec->Slice->Type == SliceP1) || (Dec->Slice->Type == SliceP2) || (Dec->Slice->Type == SliceSP1) || (Dec->Slice->Type == SliceSP2) || (Dec->Slice->Type == SliceB1) || (Dec->Slice->Type == SliceB2)) {
                Dec->Slice->NumRefIDXActiveOverrideFlag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                if (Dec->Slice->NumRefIDXActiveOverrideFlag == true) {
                    Dec->MacroBlock->NumRefIndexActiveLevel0 = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false) + 1;
                    if ((Dec->Slice->Type == SliceB1) || (Dec->Slice->Type == SliceB2)) {
                        Dec->MacroBlock->NumRefIndexActiveLevel1 = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false) + 1;
                    }
                }
            }
            if ((Dec->NAL->NALRefIDC == NAL_AuxiliarySliceExtension) || (Dec->NAL->NALRefIDC == NAL_MVCDepthView)) {
                RefPicListMVCMod(Dec, BitB); /* specified in Annex H */
            } else {
                RefPicListMod(Dec, BitB);
            }
            if (((Dec->Slice->DepthWeightedPredictionFlag == true) && ((Dec->Slice->Type == SliceP1) || (Dec->Slice->Type == SliceP2))) || ((Dec->PPS->WeightedBiPrediction == true && (Dec->Slice->Type == SliceB1)) || (Dec->Slice->Type == SliceB2))) {
                pred_weight_table(Dec, BitB);
            }
            if (Dec->NAL->NALRefIDC != NAL_Unspecified0) {
                DecodeRefPicMarking(Dec, BitB);
            }
            if ((Dec->PPS->EntropyCodingMode == ExpGolomb) && ((Dec->Slice->Type != SliceI1) || (Dec->Slice->Type != SliceI2))) {
                Dec->Slice->CabacInitIDC = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                Dec->Slice->SliceQPDelta = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, true);
            }
            // FIXME: Should SliceQPDelta be here?
            if ((Dec->Slice->Type == SliceSP1) || (Dec->Slice->Type == SliceSP2) || (Dec->Slice->Type == SliceSI1) || (Dec->Slice->Type == SliceSI2)) {
                if ((Dec->Slice->Type == SliceSP1) || (Dec->Slice->Type == SliceSP2)) {
                    Dec->Slice->DecodePMBAsSPSlice = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                    Dec->Slice->SliceQSDelta = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, true);
                }
                if (Dec->PPS->DeblockingFilterFlag == true) {
                    Dec->Slice->DisableDeblockingFilterIDC = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    if (Dec->Slice->DisableDeblockingFilterIDC == false) {
                        Dec->Slice->SliceAlphaOffsetC0 = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, true);
                        Dec->Slice->SliceBetaOffset    = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, true);
                    }
                }
                if ((Dec->PPS->SliceGroups > 0) && (Dec->PPS->SliceGroupMapType >= 3) && (Dec->PPS->SliceGroupMapType <= 5)) {
                    Dec->Slice->SliceGroupChangeCycle = ReadBits(BitIOMSByte, BitIOLSBit, BitB, Ceili(log2(Dec->PPS->PicSizeInMapUnits /  Dec->Slice->SliceGroupChangeRate)));
                }
                if (
                    (Dec->NAL->NALUnitType == NAL_MVCDepthView) &&
                    (
                     (Dec->Slice->Type == SliceI1)  ||
                     (Dec->Slice->Type == SliceI2)  ||
                     (Dec->Slice->Type == SliceSI1) ||
                     (Dec->Slice->Type == SliceSI2)
                     )
                    ) {
                    if (Dec->NAL->DepthFlag == true) {
                        Dec->Slice->DepthWeightedPredictionFlag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                    } else if (Dec->NAL->AVC3DExtensionFlag == true) {
                        Dec->Slice->DMVPFlag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                        if (Dec->Slice->SeqViewSynthesisFlag == true) {
                            Dec->Slice->SliceVSPFlag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                        }
                    }
                    if (Dec->SPS->AVC3DAcquisitionIDC == false && ((Dec->Slice->DepthWeightedPredictionFlag == true) || (Dec->Slice->DMVPFlag == true))) {
                        Dec->Slice->DepthParamSetID = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    }
                }
            }
        }
    }
    
    /* Scalable Video Coding */
    void ParseScalableSlice(DecodeAVC *Dec, BitBuffer *BitB) { // slice_header_in_scalable_extension
        Dec->Slice->FirstMacroBlockInSlice = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
        Dec->Slice->Type                   = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
        Dec->PPS->PicParamSetID            = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
        if (Dec->Slice->SeperateColorPlaneFlag == true) {
            Dec->Slice->ColorPlaneID       = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 2);
        }
        Dec->Slice->FrameNumber            = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
        if (Dec->SPS->OnlyMacroBlocksInFrame == true) {
            Dec->Slice->SliceIsInterlaced  = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->Slice->SliceIsInterlaced == true) {
                Dec->Slice->SliceIsBottomField = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            }
        }
        if (Dec->NAL->IDRFlag == NAL_NonIDRSlice) {
            Dec->Slice->IDRPicID = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
        }
        if (Dec->SPS->PicOrderCount == 0) {
            Dec->Slice->PictureOrderCountLSB = ReadBits(BitIOMSByte, BitIOLSBit, BitB, Dec->SPS->MaxPicOrder);
            if ((Dec->PPS->BottomPicFieldOrderInSliceFlag == true) && (Dec->Slice->SliceIsInterlaced == false)) {
                Dec->Slice->DeltaPictureOrderCountBottom[0] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, true);
            }
        } else if ((Dec->SPS->PicOrderCount == 1) && (Dec->SPS->DeltaPicOrder == 0)) {
            Dec->Slice->DeltaPicOrderCount[0] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, true);
            if ((Dec->PPS->BottomPicFieldOrderInSliceFlag == true) && (Dec->Slice->SliceIsInterlaced == false)) {
                Dec->Slice->DeltaPicOrderCount[1] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, true);
            }
        }
        if (Dec->PPS->RedundantPictureFlag == true) {
            Dec->PPS->RedundantPictureCount = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
        }
        if (Dec->NAL->QualityID == 0) {
            if ((Dec->NAL->NALUnitType == NAL_AuxiliarySliceExtension) && ((Dec->Slice->Type == SliceEB1) || (Dec->Slice->Type == SliceEB2))) {
                Dec->Slice->DirectSpatialMVPredictionFlag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            }
            if ((Dec->NAL->NALUnitType == NAL_AuxiliarySliceExtension) && ((Dec->Slice->Type == SliceEP1) || (Dec->Slice->Type == SliceEP2) || (Dec->Slice->Type == SliceEB1) || (Dec->Slice->Type == SliceEB2))) {
                Dec->Slice->NumRefIDXActiveOverrideFlag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                if (Dec->Slice->NumRefIDXActiveOverrideFlag == true) {
                    Dec->MacroBlock->NumRefIndexActiveLevel0 = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false) + 1;
                    if (Dec->Slice->Type == SliceEB1 || Dec->Slice->Type == SliceEB2) {
                        Dec->MacroBlock->NumRefIndexActiveLevel1 = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false) + 1;
                    }
                }
            }
            RefPicListMod(Dec, BitB);
            if ((Dec->NAL->NALUnitType == NAL_AuxiliarySliceExtension) && (((Dec->PPS->WeightedPrediction == true) &&((Dec->Slice->Type == SliceEP1)  || (Dec->Slice->Type == SliceEP2))) || ((Dec->PPS->WeightedBiPrediction == 1) && ((Dec->Slice->Type == SliceEB1) || (Dec->Slice->Type == SliceEB2))))) {
                if (Dec->NAL->NoInterLayerPredictionFlag == true) {
                    Dec->Slice->BasePredictionWeightTable = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                }
                if ((Dec->NAL->NoInterLayerPredictionFlag == false) || (Dec->Slice->BasePredictionWeightTable == false)) {
                    pred_weight_table(Dec, BitB);
                }
            }
            if (Dec->NAL->NALRefIDC != NAL_Unspecified0) {
                DecodeRefPicMarking(Dec, BitB);
                if (Dec->SVC->SliceHeaderRestricted == false) {
                    Dec->Slice->StoreRefBasePicFlag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                    if ((Dec->NAL->UseReferenceBasePictureFlag == true) || ((Dec->Slice->StoreRefBasePicFlag == true) && (Dec->NAL->IDRFlag == false))) {
                        ParseReferenceBasePictureSyntax(Dec, BitB);
                    }
                }
            }
        }
        if ((Dec->PPS->EntropyCodingMode == true) && ((Dec->Slice->Type != SliceEI1) || (Dec->Slice->Type != SliceEI2))) {
            Dec->Slice->CabacInitIDC = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
        }
        Dec->Slice->SliceQPDelta = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, true);
        if (Dec->PPS->DeblockingFilterFlag == true) {
            Dec->Slice->DisableDeblockingFilterIDC = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            if (Dec->Slice->DisableDeblockingFilterIDC != 1) {
                Dec->Slice->SliceAlphaOffsetC0 = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, true);
                Dec->Slice->SliceBetaOffset    = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, true);
            }
        }
        if ((Dec->PPS->SliceGroups > 0) && ((Dec->PPS->SliceGroupMapType >= 3) && (Dec->PPS->SliceGroupMapType <= 5))) {
            Dec->Slice->SliceGroupChangeCycle = ReadBits(BitIOMSByte, BitIOLSBit, BitB, Ceili(log2(Dec->PPS->PicSizeInMapUnits /  Dec->Slice->SliceGroupChangeRate)));
        }
        if ((Dec->NAL->NoInterLayerPredictionFlag == true) && (Dec->NAL->QualityID == 0)) {
            Dec->Slice->RefLayerDQID = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            if (Dec->SVC->InterLayerDeblockingFilterPresent == true) {
                Dec->Slice->DisableInterLayerDeblocking = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                if (Dec->Slice->DisableInterLayerDeblocking != 1) {
                    Dec->Slice->InterLayerSliceAplhaC0Offset = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, true);
                    Dec->Slice->InterLayerSliceBetaOffset    = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, true);
                }
            }
            Dec->Slice->ConstrainedIntraResamplingFlag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->SVC->ExtendedSpatialScalabilityIDC == 2) {
                if (Dec->SPS->SeperateColorPlane == false) {
                    Dec->SPS->ChromaArrayType = Dec->SPS->ChromaFormatIDC;
                } else {
                    Dec->SPS->ChromaArrayType = ChromaBW;
                }
                if (Dec->SPS->ChromaArrayType > ChromaBW) {
                    ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                    Dec->Slice->RefLayerChromaPhaseXFlag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                    Dec->Slice->RefLayerChromaPhaseY     = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 2) - 1;
                }
                Dec->Slice->ScaledRefLayerLeftOffset     = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, true);
                Dec->Slice->ScaledRefLayerTopOffset      = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, true);
                Dec->Slice->ScaledRefLayerRightOffset    = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, true);
                Dec->Slice->ScaledRefLayerBottomOffset   = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, true);
            }
        }
        if (Dec->NAL->NoInterLayerPredictionFlag == true) {
            Dec->Slice->SkipSliceFlag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->Slice->SkipSliceFlag == true) {
                Dec->Slice->MacroBlocksInSlice = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false) + 1;
            } else {
                Dec->Slice->AdaptiveBaseModeFlag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                if (Dec->Slice->AdaptiveBaseModeFlag == false) {
                    Dec->Slice->DefaultBaseModeFlag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                }
                if (Dec->Slice->DefaultBaseModeFlag == false) {
                    Dec->Slice->AdaptiveMotionPredictionFlag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                    if (Dec->Slice->AdaptiveMotionPredictionFlag == false) {
                        Dec->Slice->DefaultMotionPredictionFlag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                    }
                }
                if (Dec->Slice->AdaptiveResidualPredictionFlag == false) {
                    Dec->Slice->DefaultResidualPredictionFlag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                }
            }
            if (Dec->SVC->AdaptiveCoeffsPresent == true) {
                Dec->Slice->TCoefficentPredictionFlag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            }
        }
        if ((Dec->SVC->SliceHeaderRestricted == false) && (Dec->Slice->SkipSliceFlag == false)) {
            Dec->Slice->ScanIndexStart = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 4);
            Dec->Slice->ScanIndexEnd   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 4);
        }
    }
    
    void ParseNALAuxiliarySliceExtension(DecodeAVC *Dec, BitBuffer *BitB) { // slice_layer_extension_rbsp
        if (Dec->NAL->SVCExtensionFlag == true) {
            ParseScalableSlice(Dec, BitB); // slice_header_in_scalable_extension()
            if (Dec->Slice->SkipSliceFlag == true) {
                ParseScalableSliceData(Dec, BitB); // slice_data_in_scalable_extension()
            }
        } else if (Dec->NAL->AVC3DExtensionFlag == true) {
            ParseAVC3DSlice(Dec, BitB); // specified in Annex J
        } else {
            ParseSliceHeader(Dec, BitB); // slice_header();
            ParseSliceData(Dec, BitB, (2|3|4)); // slice_data(); FIXME: Unknown Category
        }
        AlignBitBuffer(BitB, 1); // rbsp_slice_trailing_bits()
    }
    
    void ParseScalableSliceData(DecodeAVC *Dec, BitBuffer *BitB) { // slice_data_in_scalable_extension
        bool moreDataFlag;
        if (Dec->PPS->EntropyCodingMode == Arithmetic) {
            AlignBitBuffer(BitB, 1);
        }
        Dec->Slice->MbaffFrameFlag = (Dec->SPS->AdaptiveFrameFlag && !Dec->Slice->SliceIsInterlaced);
        Dec->Slice->CurrentMacroBlockAddress = Dec->Slice->FirstMacroBlockInSlice * (Dec->Slice->MbaffFrameFlag + 1);
        moreDataFlag = 1;
        if ((Dec->Slice->Type != SliceEI1) || (Dec->Slice->Type != SliceEI2)) {
            if (Dec->PPS->EntropyCodingMode == ExpGolomb) {
                Dec->Slice->MacroBlockSkipRun = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                Dec->Slice->PreviousMacroBlockSkipped = (Dec->Slice->MacroBlockSkipRun > 0);
                for (uint8_t MB = 0; MB < Dec->Slice->MacroBlockSkipRun; MB++) {
                    Dec->Slice->CurrentMacroBlockAddress = NextMacroBlockAddress(Dec, Dec->Slice->CurrentMacroBlockAddress);
                }
                if (Dec->Slice->MacroBlockSkipRun > 0) {
                    moreDataFlag = more_rbsp_data();
                }
            } else {
                Dec->Slice->SkipMacroBlock = ReadArithmetic(BitB, NULL, NULL, NULL, NULL); // FIXME: Arthmetic encoded variable
                moreDataFlag = !Dec->Slice->SkipMacroBlock;
            }
        }
        if (moreDataFlag == true) {
            if (Dec->Slice->MbaffFrameFlag && ((Dec->Slice->CurrentMacroBlockAddress % 2) == 0 || (((Dec->Slice->CurrentMacroBlockAddress % 2) == 1) && (Dec->Slice->PreviousMacroBlockSkipped == true)))) {
                Dec->Slice->MacroBlockFieldDecodingFlag  = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            }
            ParseMacroBlockLayerInSVC(Dec, BitB); // macroblock_layer_in_scalable_extension();
            if (Dec->PPS->EntropyCodingMode == ExpGolomb) {
                moreDataFlag = more_rbsp_data();
            } else {
                if ((Dec->Slice->Type != SliceEI1) || (Dec->Slice->Type != SliceEI2)) {
                    Dec->Slice->PreviousMacroBlockSkipped = Dec->Slice->SkipMacroBlock;
                }
                if (Dec->Slice->MbaffFrameFlag && (Dec->Slice->CurrentMacroBlockAddress % 2) == 0) {
                    moreDataFlag = 1;
                } else {
                    Dec->Slice->IsEndOfSlice = ReadArithmetic(BitB, NULL, NULL, NULL, NULL); // arthmetic coded variable
                    moreDataFlag = !Dec->Slice->IsEndOfSlice;
                    while (Dec->Slice->IsEndOfSlice == false) {
                        SkipBits(BitB, 1); // FIXME: Not sure f it should be 1 bit or 8 or what?
                    }
                }
            }
            Dec->Slice->CurrentMacroBlockAddress = NextMacroBlockAddress(Dec, Dec->Slice->CurrentMacroBlockAddress);
        }
    }
    
    void ParseUnpartitionedSliceLayer(DecodeAVC *Dec, BitBuffer *BitB) { // slice_layer_without_partitioning_rbsp
                                                                         //ParseSliceHeader(Dec, BitB);
        ParseSliceData(Dec, BitB, 0);
        rbsp_slice_trailing_bits(Dec, BitB);
    }
    
    // Parse input file to find the start of the AVC stream, parse AVC stream to find NALs, Parse NALs to find metadata and data, then extract SliceGroups into Slices, Slices into MacroBlocks, then decode each MacroBlock.
    
    // ALSO, I *REALLY* want to learn how to implement multi-threading to speed this up, cuz there's a fuck ton of data and it may not be fast enough.
    
    // So, from Slice on down, I should have a main Decode thread, that dispatches threads for each macroblock.
    // But, that means I'll have to readjust the Slice and MacroBlock structs, because they're centralized whiich would massively slow down the threads, those variables pretty much all need to be local in scope.
    
    void DemuxSliceGroup() {
        
    }
    
    void DecodeSlice() {
        // Rearrange SliceGroups
        // Extract a slice from each Slice Group
        // Extract a MacroBlock from each Slice.
        // Do the reverse DCT, colorspace conversion, and block-smoothing.
        // Output to an array to contain the picture.
    }
    
#ifdef __cplusplus
}
#endif
