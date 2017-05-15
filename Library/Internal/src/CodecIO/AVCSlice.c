#include "../../../Dependencies/BitIO/libBitIO/include/BitIO.h"

#include "../../include/ModernAVCTypes.h"
#include "../../include/Common.h"
#include "../../include/Decode/DecodeAVC.h"
#include "../../include/Decode/DecodeMacroBlock.h"
#include "../../include/Decode/DecodeSlice.h"
#include "../../include/Decode/ParseNAL.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    // macroblock-adaptive frame/field decoding is in use, a sequence of macroblock pairs.
    // Slices contain only 1 color channel?
    
    void ParseSliceHeader(AVCFile *AVC, BitBuffer *BitB) {
        
    }
    
    // Slice group contains partitioned slices, partitioned by channel. Each slice may contain macroblocks or macroblock pairs (for MVC?).
    // A slice group contains a whole frame?
    
    /* Parse NAL Slice */
    void ParseAVC3DSliceDataExtension(AVCFile *AVC, BitBuffer *BitB) { // slice_data_in_3davc_extension
        if (AVC->PPS->EntropyCodingMode == Arithmetic) {
            AlignBitBuffer(BitB, 1);
            AVC->Slice->CurrentMacroBlockAddress = AVC->Slice->FirstMacroBlockInSlice * (AVC->Slice->MbaffFrameFlag + 1);
            bool     MoreDataFlag = true;
            AVC->Slice->PreviousMacroBlockSkipped = false;
            bool     RunLength = 0;
            while (MoreDataFlag == true) {
                if ((AVC->Slice->Type != SliceI1) || (AVC->Slice->Type != SliceI2) || (AVC->Slice->Type != SliceSI1) || (AVC->Slice->Type != SliceSI2)) {
                    if (AVC->PPS->EntropyCodingMode == ExpGolomb) {
                        AVC->Slice->MacroBlockSkipRun = ReadExpGolomb(BitB, false);
                        AVC->Slice->PreviousMacroBlockSkipped = (AVC->Slice->MacroBlockSkipRun > 0);
                        for (uint8_t SkippedMacroBlock = 0; SkippedMacroBlock < AVC->Slice->MacroBlockSkipRun; SkippedMacroBlock++) {
                            AVC->Slice->CurrentMacroBlockAddress = NextMacroBlockAddress(AVC, AVC->Slice->CurrentMacroBlockAddress);
                        }
                    }
                }
            }
        }
    }
    
    void ParseAVC3DSlice(AVCFile *AVC, BitBuffer *BitB) { // slice_header_in_3davc_extension
        AVC->Slice->FirstMacroBlockInSlice     = ReadExpGolomb(BitB, false);
        AVC->Slice->Type                       = ReadExpGolomb(BitB, false);
        AVC->PPS->PicParamSetID                = ReadExpGolomb(BitB, false);
        if ((AVC->NAL->AVC3DExtensionFlag == true) && (AVC->NAL->AVC3DExtensionFlag == true)) {
            AVC->Slice->PreSliceHeaderSrc      = ReadBits(BitB, 2, true);
            if ((AVC->Slice->Type == SliceP1) || (AVC->Slice->Type == SliceP2) || (AVC->Slice->Type == SliceSP1) || (AVC->Slice->Type == SliceSP2) || (AVC->Slice->Type == SliceB1) || (AVC->Slice->Type == SliceB2)) {
                AVC->Slice->PreRefListSrc      = ReadBits(BitB, 2, true);
                if (AVC->Slice->PreRefListSrc == 0) {
                    AVC->Slice->NumRefIDXActiveOverrideFlag= ReadBits(BitB, 1, true);
                    if (AVC->Slice->NumRefIDXActiveOverrideFlag == true) {
                        AVC->MacroBlock->NumRefIndexActiveLevel0 = ReadExpGolomb(BitB, false) + 1;
                    }
                    if ((AVC->Slice->Type == SliceB1) || (AVC->Slice->Type == SliceB2)) {
                        AVC->MacroBlock->NumRefIndexActiveLevel1 = ReadExpGolomb(BitB, false) + 1;
                    }
                    RefPicListMVCMod(AVC, BitB);
                }
            }
            if ((AVC->PPS->WeightedPrediction == true && ((AVC->Slice->Type == SliceP1) || (AVC->Slice->Type == SliceP2) || (AVC->Slice->Type == SliceSP1) || (AVC->Slice->Type == SliceSP2))) || ((AVC->PPS->WeightedBiPrediction == 1 && ((AVC->Slice->Type == SliceB1) || (AVC->Slice->Type == SliceB2))))) {
                
                AVC->Slice->PrePredictionWeightTableSrc = ReadBits(BitB, 2, true);
                if (AVC->Slice->PrePredictionWeightTableSrc == false) {
                    pred_weight_table(AVC, BitB);
                }
            }
            if (AVC->NAL->NALRefIDC != 0) {
                AVC->Slice->PreDecReferencePictureMarkingSrc = ReadBits(BitB, 2, true);
                if (AVC->Slice->PreDecReferencePictureMarkingSrc == 0) {
                    DecodeRefPicMarking(AVC, BitB);
                }
            }
            AVC->Slice->SliceQPDelta = ReadExpGolomb(BitB, true);
        } else {
            if (AVC->SPS->SeperateColorPlane == true) {
                AVC->Slice->ColorPlaneID   = ReadBits(BitB, 2, true);
            }
            AVC->Slice->FrameNumber     = ReadBits(BitB, 0, true); // FIXME: READ VARIABLE BITS frame_num
            if (AVC->SPS->OnlyMacroBlocksInFrame == false) {
                AVC->Slice->SliceIsInterlaced = ReadBits(BitB, 1, true);
                if (AVC->Slice->SliceIsInterlaced == true) {
                    AVC->Slice->SliceIsBottomField  = ReadBits(BitB, 1, true);
                }
            }
            if (AVC->Slice->IDRPicID == true) {
                AVC->Slice->IDRPicID = ReadExpGolomb(BitB, false);
            }
            if (AVC->SPS->PicOrderCount == false) {
                AVC->Slice->PictureOrderCountLSB = ReadBits(BitB, AVC->SPS->MaxPicOrder, true);
                if ((AVC->PPS->BottomPicFieldOrderInSliceFlag == true) && (AVC->Slice->SliceIsInterlaced == false)) {
                    AVC->Slice->DeltaPictureOrderCountBottom[1] = ReadExpGolomb(BitB, true);
                }
            }
            if ((AVC->SPS->PicOrderCount == true) && (AVC->SPS->DeltaPicOrder == false)) {
                AVC->Slice->DeltaPictureOrderCountBottom[0] = ReadExpGolomb(BitB, true);
                if ((AVC->PPS->BottomPicFieldOrderInSliceFlag == true) && (AVC->Slice->SliceIsInterlaced == false)) {
                    AVC->Slice->DeltaPictureOrderCountBottom[1] = ReadExpGolomb(BitB, true);
                }
            }
            if (AVC->PPS->RedundantPictureFlag == true) {
                AVC->PPS->RedundantPictureCount = ReadExpGolomb(BitB, true);
            }
            if ((AVC->Slice->Type == SliceB1) || (AVC->Slice->Type == SliceB2)) {
                AVC->Slice->DirectSpatialMVPredictionFlag = ReadBits(BitB, 1, true);
            }
            if ((AVC->Slice->Type == SliceP1) || (AVC->Slice->Type == SliceP2) || (AVC->Slice->Type == SliceSP1) || (AVC->Slice->Type == SliceSP2) || (AVC->Slice->Type == SliceB1) || (AVC->Slice->Type == SliceB2)) {
                AVC->Slice->NumRefIDXActiveOverrideFlag = ReadBits(BitB, 1, true);
                if (AVC->Slice->NumRefIDXActiveOverrideFlag == true) {
                    AVC->MacroBlock->NumRefIndexActiveLevel0 = ReadExpGolomb(BitB, false) + 1;
                    if ((AVC->Slice->Type == SliceB1) || (AVC->Slice->Type == SliceB2)) {
                        AVC->MacroBlock->NumRefIndexActiveLevel1 = ReadExpGolomb(BitB, false) + 1;
                    }
                }
            }
            if ((AVC->NAL->NALRefIDC == NAL_AuxiliarySliceExtension) || (AVC->NAL->NALRefIDC == NAL_MVCDepthView)) {
                RefPicListMVCMod(AVC, BitB); /* specified in Annex H */
            } else {
                RefPicListMod(AVC, BitB);
            }
            if (((AVC->Slice->DepthWeightedPredictionFlag == true) && ((AVC->Slice->Type == SliceP1) || (AVC->Slice->Type == SliceP2))) || ((AVC->PPS->WeightedBiPrediction == true && (AVC->Slice->Type == SliceB1)) || (AVC->Slice->Type == SliceB2))) {
                pred_weight_table(AVC, BitB);
            }
            if (AVC->NAL->NALRefIDC != NAL_Unspecified0) {
                DecodeRefPicMarking(AVC, BitB);
            }
            if ((AVC->PPS->EntropyCodingMode == ExpGolomb) && ((AVC->Slice->Type != SliceI1) || (AVC->Slice->Type != SliceI2))) {
                AVC->Slice->CabacInitIDC = ReadExpGolomb(BitB, false);
                AVC->Slice->SliceQPDelta = ReadExpGolomb(BitB, true);
            }
            // FIXME: Should SliceQPDelta be here?
            if ((AVC->Slice->Type == SliceSP1) || (AVC->Slice->Type == SliceSP2) || (AVC->Slice->Type == SliceSI1) || (AVC->Slice->Type == SliceSI2)) {
                if ((AVC->Slice->Type == SliceSP1) || (AVC->Slice->Type == SliceSP2)) {
                    AVC->Slice->DecodePMBAsSPSlice = ReadBits(BitB, 1, true);
                    AVC->Slice->SliceQSDelta = ReadExpGolomb(BitB, true);
                }
                if (AVC->PPS->DeblockingFilterFlag == true) {
                    AVC->Slice->DisableDeblockingFilterIDC = ReadExpGolomb(BitB, false);
                    if (AVC->Slice->DisableDeblockingFilterIDC == false) {
                        AVC->Slice->SliceAlphaOffsetC0 = ReadExpGolomb(BitB, true);
                        AVC->Slice->SliceBetaOffset    = ReadExpGolomb(BitB, true);
                    }
                }
                if ((AVC->PPS->SliceGroups > 0) && (AVC->PPS->SliceGroupMapType >= 3) && (AVC->PPS->SliceGroupMapType <= 5)) {
                    AVC->Slice->SliceGroupChangeCycle = ReadBits(BitB, Ceili(log2(AVC->PPS->PicSizeInMapUnits /  AVC->Slice->SliceGroupChangeRate)), true);
                }
                if (
                    (AVC->NAL->NALUnitType == NAL_MVCDepthView) &&
                    (
                     (AVC->Slice->Type == SliceI1)  ||
                     (AVC->Slice->Type == SliceI2)  ||
                     (AVC->Slice->Type == SliceSI1) ||
                     (AVC->Slice->Type == SliceSI2)
                     )
                    ) {
                    if (AVC->NAL->DepthFlag == true) {
                        AVC->Slice->DepthWeightedPredictionFlag = ReadBits(BitB, 1, true);
                    } else if (AVC->NAL->AVC3DExtensionFlag == true) {
                        AVC->Slice->DMVPFlag = ReadBits(BitB, 1, true);
                        if (AVC->Slice->SeqViewSynthesisFlag == true) {
                            AVC->Slice->SliceVSPFlag = ReadBits(BitB, 1, true);
                        }
                    }
                    if (AVC->SPS->AVC3DAcquisitionIDC == false && ((AVC->Slice->DepthWeightedPredictionFlag == true) || (AVC->Slice->DMVPFlag == true))) {
                        AVC->Slice->DepthParamSetID = ReadExpGolomb(BitB, false);
                    }
                }
            }
        }
    }
    
    /* Scalable Video Coding */
    void ParseScalableSlice(AVCFile *AVC, BitBuffer *BitB) { // slice_header_in_scalable_extension
        AVC->Slice->FirstMacroBlockInSlice = ReadExpGolomb(BitB, false);
        AVC->Slice->Type                   = ReadExpGolomb(BitB, false);
        AVC->PPS->PicParamSetID            = ReadExpGolomb(BitB, false);
        if (AVC->Slice->SeperateColorPlaneFlag == true) {
            AVC->Slice->ColorPlaneID       = ReadBits(BitB, 2, true);
        }
        AVC->Slice->FrameNumber            = ReadExpGolomb(BitB, false);
        if (AVC->SPS->OnlyMacroBlocksInFrame == true) {
            AVC->Slice->SliceIsInterlaced  = ReadBits(BitB, 1, true);
            if (AVC->Slice->SliceIsInterlaced == true) {
                AVC->Slice->SliceIsBottomField = ReadBits(BitB, 1, true);
            }
        }
        if (AVC->NAL->IDRFlag == NAL_NonIDRSlice) {
            AVC->Slice->IDRPicID = ReadExpGolomb(BitB, false);
        }
        if (AVC->SPS->PicOrderCount == 0) {
            AVC->Slice->PictureOrderCountLSB = ReadBits(BitB, AVC->SPS->MaxPicOrder, true);
            if ((AVC->PPS->BottomPicFieldOrderInSliceFlag == true) && (AVC->Slice->SliceIsInterlaced == false)) {
                AVC->Slice->DeltaPictureOrderCountBottom[0] = ReadExpGolomb(BitB, true);
            }
        } else if ((AVC->SPS->PicOrderCount == 1) && (AVC->SPS->DeltaPicOrder == 0)) {
            AVC->Slice->DeltaPicOrderCount[0] = ReadExpGolomb(BitB, true);
            if ((AVC->PPS->BottomPicFieldOrderInSliceFlag == true) && (AVC->Slice->SliceIsInterlaced == false)) {
                AVC->Slice->DeltaPicOrderCount[1] = ReadExpGolomb(BitB, true);
            }
        }
        if (AVC->PPS->RedundantPictureFlag == true) {
            AVC->PPS->RedundantPictureCount = ReadExpGolomb(BitB, false);
        }
        if (AVC->NAL->QualityID == 0) {
            if ((AVC->NAL->NALUnitType == NAL_AuxiliarySliceExtension) && ((AVC->Slice->Type == SliceEB1) || (AVC->Slice->Type == SliceEB2))) {
                AVC->Slice->DirectSpatialMVPredictionFlag = ReadBits(BitB, 1, true);
            }
            if ((AVC->NAL->NALUnitType == NAL_AuxiliarySliceExtension) && ((AVC->Slice->Type == SliceEP1) || (AVC->Slice->Type == SliceEP2) || (AVC->Slice->Type == SliceEB1) || (AVC->Slice->Type == SliceEB2))) {
                AVC->Slice->NumRefIDXActiveOverrideFlag = ReadBits(BitB, 1, true);
                if (AVC->Slice->NumRefIDXActiveOverrideFlag == true) {
                    AVC->MacroBlock->NumRefIndexActiveLevel0 = ReadExpGolomb(BitB, false) + 1;
                    if (AVC->Slice->Type == SliceEB1 || AVC->Slice->Type == SliceEB2) {
                        AVC->MacroBlock->NumRefIndexActiveLevel1 = ReadExpGolomb(BitB, false) + 1;
                    }
                }
            }
            RefPicListMod(AVC, BitB);
            if ((AVC->NAL->NALUnitType == NAL_AuxiliarySliceExtension) && (((AVC->PPS->WeightedPrediction == true) &&((AVC->Slice->Type == SliceEP1)  || (AVC->Slice->Type == SliceEP2))) || ((AVC->PPS->WeightedBiPrediction == 1) && ((AVC->Slice->Type == SliceEB1) || (AVC->Slice->Type == SliceEB2))))) {
                if (AVC->NAL->NoInterLayerPredictionFlag == true) {
                    AVC->Slice->BasePredictionWeightTable = ReadBits(BitB, 1, true);
                }
                if ((AVC->NAL->NoInterLayerPredictionFlag == false) || (AVC->Slice->BasePredictionWeightTable == false)) {
                    pred_weight_table(AVC, BitB);
                }
            }
            if (AVC->NAL->NALRefIDC != NAL_Unspecified0) {
                DecodeRefPicMarking(AVC, BitB);
                if (AVC->SVC->SliceHeaderRestricted == false) {
                    AVC->Slice->StoreRefBasePicFlag = ReadBits(BitB, 1, true);
                    if ((AVC->NAL->UseReferenceBasePictureFlag == true) || ((AVC->Slice->StoreRefBasePicFlag == true) && (AVC->NAL->IDRFlag == false))) {
                        ParseReferenceBasePictureSyntax(AVC, BitB);
                    }
                }
            }
        }
        if ((AVC->PPS->EntropyCodingMode == true) && ((AVC->Slice->Type != SliceEI1) || (AVC->Slice->Type != SliceEI2))) {
            AVC->Slice->CabacInitIDC = ReadExpGolomb(BitB, false);
        }
        AVC->Slice->SliceQPDelta = ReadExpGolomb(BitB, true);
        if (AVC->PPS->DeblockingFilterFlag == true) {
            AVC->Slice->DisableDeblockingFilterIDC = ReadExpGolomb(BitB, false);
            if (AVC->Slice->DisableDeblockingFilterIDC != 1) {
                AVC->Slice->SliceAlphaOffsetC0 = ReadExpGolomb(BitB, true);
                AVC->Slice->SliceBetaOffset    = ReadExpGolomb(BitB, true);
            }
        }
        if ((AVC->PPS->SliceGroups > 0) && ((AVC->PPS->SliceGroupMapType >= 3) && (AVC->PPS->SliceGroupMapType <= 5))) {
            AVC->Slice->SliceGroupChangeCycle = ReadBits(BitB, Ceili(log2(AVC->PPS->PicSizeInMapUnits /  AVC->Slice->SliceGroupChangeRate)), true);
        }
        if ((AVC->NAL->NoInterLayerPredictionFlag == true) && (AVC->NAL->QualityID == 0)) {
            AVC->Slice->RefLayerDQID = ReadExpGolomb(BitB, false);
            if (AVC->SVC->InterLayerDeblockingFilterPresent == true) {
                AVC->Slice->DisableInterLayerDeblocking = ReadExpGolomb(BitB, false);
                if (AVC->Slice->DisableInterLayerDeblocking != 1) {
                    AVC->Slice->InterLayerSliceAplhaC0Offset = ReadExpGolomb(BitB, true);
                    AVC->Slice->InterLayerSliceBetaOffset    = ReadExpGolomb(BitB, true);
                }
            }
            AVC->Slice->ConstrainedIntraResamplingFlag = ReadBits(BitB, 1, true);
            if (AVC->SVC->ExtendedSpatialScalabilityIDC == 2) {
                if (AVC->SPS->SeperateColorPlane == false) {
                    AVC->SPS->ChromaArrayType = AVC->SPS->ChromaFormatIDC;
                } else {
                    AVC->SPS->ChromaArrayType = ChromaBW;
                }
                if (AVC->SPS->ChromaArrayType > ChromaBW) {
                    AVC->Slice->RefLayerChromaPhaseXFlag = ReadBits(BitB, 1, true);
                    AVC->Slice->RefLayerChromaPhaseY     = ReadBits(BitB, 2, true) - 1;
                }
                AVC->Slice->ScaledRefLayerLeftOffset     = ReadExpGolomb(BitB, true);
                AVC->Slice->ScaledRefLayerTopOffset      = ReadExpGolomb(BitB, true);
                AVC->Slice->ScaledRefLayerRightOffset    = ReadExpGolomb(BitB, true);
                AVC->Slice->ScaledRefLayerBottomOffset   = ReadExpGolomb(BitB, true);
            }
        }
        if (AVC->NAL->NoInterLayerPredictionFlag == true) {
            AVC->Slice->SkipSliceFlag = ReadBits(BitB, 1, true);
            if (AVC->Slice->SkipSliceFlag == true) {
                AVC->Slice->MacroBlocksInSlice = ReadExpGolomb(BitB, false) + 1;
            } else {
                AVC->Slice->AdaptiveBaseModeFlag = ReadBits(BitB, 1, true);
                if (AVC->Slice->AdaptiveBaseModeFlag == false) {
                    AVC->Slice->DefaultBaseModeFlag = ReadBits(BitB, 1, true);
                }
                if (AVC->Slice->DefaultBaseModeFlag == false) {
                    AVC->Slice->AdaptiveMotionPredictionFlag = ReadBits(BitB, 1, true);
                    if (AVC->Slice->AdaptiveMotionPredictionFlag == false) {
                        AVC->Slice->DefaultMotionPredictionFlag = ReadBits(BitB, 1, true);
                    }
                }
                if (AVC->Slice->AdaptiveResidualPredictionFlag == false) {
                    AVC->Slice->DefaultResidualPredictionFlag = ReadBits(BitB, 1, true);
                }
            }
            if (AVC->SVC->AdaptiveCoeffsPresent == true) {
                AVC->Slice->TCoefficentPredictionFlag = ReadBits(BitB, 1, true);
            }
        }
        if ((AVC->SVC->SliceHeaderRestricted == false) && (AVC->Slice->SkipSliceFlag == false)) {
            AVC->Slice->ScanIndexStart = ReadBits(BitB, 4, true);
            AVC->Slice->ScanIndexEnd   = ReadBits(BitB, 4, true);
        }
    }
    
    void ParseNALAuxiliarySliceExtension(AVCFile *AVC, BitBuffer *BitB) { // slice_layer_extension_rbsp
        if (AVC->NAL->SVCExtensionFlag == true) {
            ParseScalableSlice(AVC, BitB); // slice_header_in_scalable_extension()
            if (AVC->Slice->SkipSliceFlag == true) {
                ParseScalableSliceData(AVC, BitB); // slice_data_in_scalable_extension()
            }
        } else if (AVC->NAL->AVC3DExtensionFlag == true) {
            ParseAVC3DSlice(AVC, BitB); // specified in Annex J
        } else {
            ParseSliceHeader(AVC, BitB); // slice_header();
            ParseSliceData(AVC, BitB, (2|3|4)); // slice_data(); FIXME: Unknown Category
        }
        AlignInput(BitB, 1); // rbsp_slice_trailing_bits()
    }
    
    void ParseScalableSliceData(AVCFile *AVC, BitBuffer *BitB) { // slice_data_in_scalable_extension
        bool moreDataFlag;
        if (AVC->PPS->EntropyCodingMode == Arithmetic) {
            AlignInput(BitB, 1);
        }
        AVC->Slice->MbaffFrameFlag = (AVC->SPS->AdaptiveFrameFlag && !AVC->Slice->SliceIsInterlaced);
        AVC->Slice->CurrentMacroBlockAddress = AVC->Slice->FirstMacroBlockInSlice * (AVC->Slice->MbaffFrameFlag + 1);
        moreDataFlag = 1;
        if ((AVC->Slice->Type != SliceEI1) || (AVC->Slice->Type != SliceEI2)) {
            if (AVC->PPS->EntropyCodingMode == ExpGolomb) {
                AVC->Slice->MacroBlockSkipRun = ReadExpGolomb(BitB, false);
                AVC->Slice->PreviousMacroBlockSkipped = (AVC->Slice->MacroBlockSkipRun > 0);
                for (uint8_t MB = 0; MB < AVC->Slice->MacroBlockSkipRun; MB++) {
                    AVC->Slice->CurrentMacroBlockAddress = NextMacroBlockAddress(AVC, AVC->Slice->CurrentMacroBlockAddress);
                }
                if (AVC->Slice->MacroBlockSkipRun > 0) {
                    moreDataFlag = more_rbsp_data();
                }
            } else {
                AVC->Slice->SkipMacroBlock = ReadArithmetic(BitB, NULL, NULL, NULL, NULL); // FIXME: Arthmetic encoded variable
                moreDataFlag = !AVC->Slice->SkipMacroBlock;
            }
        }
        if (moreDataFlag == true) {
            if (AVC->Slice->MbaffFrameFlag && ((AVC->Slice->CurrentMacroBlockAddress % 2) == 0 || (((AVC->Slice->CurrentMacroBlockAddress % 2) == 1) && (AVC->Slice->PreviousMacroBlockSkipped == true)))) {
                AVC->Slice->MacroBlockFieldDecodingFlag  = ReadBits(BitB, 1, true);
            }
            ParseMacroBlockLayerInSVC(AVC, BitB); // macroblock_layer_in_scalable_extension();
            if (AVC->PPS->EntropyCodingMode == ExpGolomb) {
                moreDataFlag = more_rbsp_data();
            } else {
                if ((AVC->Slice->Type != SliceEI1) || (AVC->Slice->Type != SliceEI2)) {
                    AVC->Slice->PreviousMacroBlockSkipped = AVC->Slice->SkipMacroBlock;
                }
                if (AVC->Slice->MbaffFrameFlag && (AVC->Slice->CurrentMacroBlockAddress % 2) == 0) {
                    moreDataFlag = 1;
                } else {
                    AVC->Slice->IsEndOfSlice = ReadArithmetic(BitB, NULL, NULL, NULL, NULL); // arthmetic coded variable
                    moreDataFlag = !AVC->Slice->IsEndOfSlice;
                    while (AVC->Slice->IsEndOfSlice == false) {
                        SkipBits(BitB, 1); // FIXME: Not sure f it should be 1 bit or 8 or what?
                    }
                }
            }
            AVC->Slice->CurrentMacroBlockAddress = NextMacroBlockAddress(AVC, AVC->Slice->CurrentMacroBlockAddress);
        }
    }
    
    void ParseSliceHeader(AVCFile *AVC, BitBuffer *BitB) { // slice_header
        
    }
    
    void ParseUnpartitionedSliceLayer(AVCFile *AVC, BitBuffer *BitB) { // slice_layer_without_partitioning_rbsp
        ParseSliceHeader(AVC, BitB);
        ParseSliceData(AVC, BitB, 0);
        rbsp_slice_trailing_bits();
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
