#include "../../include/libModernAVC.h"
#include "../../include/Private/ModernAVCTypes.h"
#include "../../include/Private/Decode/DecodeAVCCommon.h"
#include "../../include/Private/Decode/DecodeAVC.h"
#include "../../include/Private/Decode/DecodeMacroBlock.h"
#include "../../include/Private/Decode/DecodeSlice.h"
#include "../../include/Private/Decode/ParseNAL.h"
#include "../../include/Private/ModernAVCTables.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    BitBuffer *ExtractNAL2Packet(DecodeAVC *Dec, BitBuffer *AVCStream) { // NAL here means any NAL, packet here means NAL with any substituting required performed.
        /*
         Within a NAL, we need to check the size of the NAL, then scan through the input stream to make sure there are no bytes set to 0x000003, 0x000004, or 0x000005?
         */
        
        /*
         NAL rules:
         
         Can not end with a zero byte
         */
        return NULL;
    }
    
    /* Sequence Parameter Set */
    void ParseSequenceParameterSetData(DecodeAVC *Dec, BitBuffer *BitB) { // seq_parameter_set_data
        Dec->SPS->ProfileIDC                                                        = ReadBits(BitB, 8, true); // 100
        Dec->SPS->ConstraintFlag0                                                   = ReadBits(BitB, 1, true); // 0
        Dec->SPS->ConstraintFlag1                                                   = ReadBits(BitB, 1, true); // 0
        Dec->SPS->ConstraintFlag2                                                   = ReadBits(BitB, 1, true); // 0
        Dec->SPS->ConstraintFlag3                                                   = ReadBits(BitB, 1, true); // 0
        Dec->SPS->ConstraintFlag4                                                   = ReadBits(BitB, 1, true); // 0
        Dec->SPS->ConstraintFlag5                                                   = ReadBits(BitB, 1, true); // 0
        SkipBits(BitB, 2); // Zero bits.
        Dec->SPS->LevelIDC[0]                                                       = ReadBits(BitB, 8, true); // 51
        Dec->SPS->SeqParamSetID                                                     = ReadExpGolomb(BitB, false); // 1
        
        if ((Dec->SPS->ProfileIDC == 44)  ||
            (Dec->SPS->ProfileIDC == 83)  ||
            (Dec->SPS->ProfileIDC == 86)  ||
            (Dec->SPS->ProfileIDC == 100) ||
            (Dec->SPS->ProfileIDC == 110) ||
            (Dec->SPS->ProfileIDC == 118) ||
            (Dec->SPS->ProfileIDC == 122) ||
            (Dec->SPS->ProfileIDC == 128) ||
            (Dec->SPS->ProfileIDC == 134) ||
            (Dec->SPS->ProfileIDC == 138) ||
            (Dec->SPS->ProfileIDC == 139) ||
            (Dec->SPS->ProfileIDC == 244)) {
            Dec->SPS->ChromaFormatIDC                                               = ReadExpGolomb(BitB, false); // 1 aka 420
            if (Dec->SPS->ChromaFormatIDC == Chroma444) {
                Dec->SPS->SeperateColorPlane                                        = ReadBits(BitB, 1, true);
            }
            Dec->SPS->LumaBitDepthMinus8                                            = ReadExpGolomb(BitB, false); // 8
            Dec->SPS->ChromaBitDepthMinus8                                          = ReadExpGolomb(BitB, false);
            Dec->SPS->QPPrimeBypassFlag                                             = ReadBits(BitB, 1, true); // qpprime_y_zero_transform_bypass_flag
            Dec->SPS->ScalingMatrixFlag                                             = ReadBits(BitB, 1, true);
            
            if (Dec->SPS->ScalingMatrixFlag == true) {
                for (uint8_t i = 0; i < ((Dec->SPS->ChromaFormatIDC != Chroma444) ? 8 : 12); i++) {
                    Dec->SPS->ScalingListFlag[i]                                    = ReadBits(BitB, 1, true);
                    if (Dec->SPS->ScalingListFlag[i] == true) {
                        if (i < 6) {
                            ScalingList(Dec, BitB, ScalingList4x4[i], 16, Dec->PPS->DefaultScalingMatrix4x4[i]);
                        } else {
                            ScalingList(Dec, BitB, ScalingList8x8[i - 6], 64, Dec->PPS->DefaultScalingMatrix8x8[i - 6]);
                        }
                    }
                }
            }
        }
        Dec->SPS->MaxFrameNumMinus4                                                 = ReadExpGolomb(BitB, false) + 4; // 3
        Dec->SPS->PicOrderCount                                                     = ReadExpGolomb(BitB, false);
        if (Dec->SPS->PicOrderCount == 0) {
            Dec->SPS->MaxPicOrder                                                   = ReadExpGolomb(BitB, false) + 4;
        } else {
            Dec->SPS->DeltaPicOrder                                                 = ReadBits(BitB, 1, true);
            Dec->SPS->OffsetNonRefPic                                               = ReadExpGolomb(BitB, true);
            Dec->SPS->OffsetTop2Bottom                                              = ReadExpGolomb(BitB, true);
            Dec->SPS->RefFramesInPicOrder                                           = ReadExpGolomb(BitB, false);
            for (uint8_t i = 0; i < Dec->SPS->RefFramesInPicOrder; i++) {
                Dec->SPS->RefFrameOffset[i]                                         = ReadExpGolomb(BitB, true);
            }
            
        }
        Dec->SPS->MaxRefFrames                                                      = ReadExpGolomb(BitB, false);
        Dec->SPS->GapsInFrameNumAllowed                                             = ReadBits(BitB, 1, true);
        Dec->SPS->PicWidthInMacroBlocksMinus1                                       = ReadExpGolomb(BitB, false);
        Dec->SPS->PicHeightInMapUnitsMinus1                                         = ReadExpGolomb(BitB, false);
        Dec->SPS->OnlyMacroBlocksInFrame                                            = ReadBits(BitB, 1, true);
        if (Dec->SPS->OnlyMacroBlocksInFrame == false) {
            Dec->SPS->AdaptiveFrameFlag                                             = ReadBits(BitB, 1, true);
        }
        Dec->SPS->Inference8x8                                                      = ReadBits(BitB, 1, true);
        Dec->SPS->FrameCroppingFlag                                                 = ReadBits(BitB, 1, true);
        if (Dec->SPS->FrameCroppingFlag == true) {
            Dec->SPS->FrameCropLeft                                                 = ReadExpGolomb(BitB, false);
            Dec->SPS->FrameCropRight                                                = ReadExpGolomb(BitB, false);
            Dec->SPS->FrameCropTop                                                  = ReadExpGolomb(BitB, false);
            Dec->SPS->FrameCropBottom                                               = ReadExpGolomb(BitB, false);
        }
        Dec->SPS->VUIPresent                                                        = ReadBits(BitB, 1, true);
        if (Dec->SPS->VUIPresent == true) {
            ParseVideoUsabilityInformation(Dec, BitB);
        }
    }
    
    void ParseNALSequenceParameterSetExtended(DecodeAVC *Dec, BitBuffer *BitB) { // seq_parameter_set_extension_rbsp?
        Dec->SPS->SeqParamSetID                                                     = ReadExpGolomb(BitB, false);
        Dec->SPS->AuxiliaryFormatID                                                 = ReadExpGolomb(BitB, false);
        if (Dec->SPS->AuxiliaryFormatID != 0) {
            Dec->SPS->AuxiliaryBitDepth                                             = ReadExpGolomb(BitB, false) + 8;
            Dec->SPS->AlphaIncrFlag                                                 = ReadBits(BitB, 1, true);
            Dec->SPS->AlphaOpaqueValue                                              = ReadBits(BitB, Dec->SPS->AuxiliaryBitDepth + 9, true);
            Dec->SPS->AlphaTransparentValue                                         = ReadBits(BitB, Dec->SPS->AuxiliaryBitDepth + 9, true);
        }
        Dec->SPS->AdditionalExtensionFlag                                           = ReadBits(BitB, 1, true);
        AlignBitBuffer(BitB, 1); // rbsp_trailing_bits
    }
    
    void ParseNALSubsetSPS(DecodeAVC *Dec, BitBuffer *BitB) { // subset_seq_parameter_set_rbsp
        ReadSequenceParameterSetData(Dec, BitB);
        if ((Dec->SPS->ProfileIDC == Scalable_Constrained_Baseline_Profile) || (Dec->SPS->ProfileIDC == Scalable_High_Intra_Profile)) { // Scalable Video mode
            ParseNALSequenceParameterSetSVC(Dec, BitB);
            if (Dec->SPS->VUIPresent == true) {
                ParseSVCVUIExtension(Dec, BitB);
            }
        } else if ((Dec->SPS->ProfileIDC == MultiView_High_Profile) || (Dec->SPS->ProfileIDC == Stereo_High_Profile) || (Dec->SPS->ProfileIDC == 134)) {
            // Multi View Coding
            SkipBits(BitB, 1);
            ParseSPSMultiViewCodingExtension(Dec, BitB); // seq_parameter_set_mvc_extension
            Dec->SPS->MVCVUIParamsPresent                                           = ReadBits(BitB, 1, true);
            if (Dec->SPS->MVCVUIParamsPresent == true) {
                ParseMVCVUI(Dec, BitB);
            }
        } else if (Dec->SPS->ProfileIDC == MultiView_Depth_High_Profile) {
            // MVCD
            SkipBits(BitB, 1);
            ParseSPSMVCDExtension(Dec, BitB);
        } else if (Dec->SPS->ProfileIDC == 139) {
            // MVCD && 3DAVC
            SkipBits(BitB, 1);
            ParseSPSMVCDExtension(Dec, BitB);
            ParseSPS3DAVCExtension(Dec, BitB);
        }
        Dec->SPS->AdditionalExtension2                                              = ReadBits(BitB, 1, true);
        if (Dec->SPS->AdditionalExtension2 == true) {
            while (more_rbsp_data()) {
                Dec->SPS->AdditionalExtension2DataFlag                              = ReadBits(BitB, 1, true);
            }
        }
        AlignBitBuffer(BitB, 1); // rbsp_trailing_bits
    }
    
    void ParseNALSequenceParameterSet(DecodeAVC *Dec, BitBuffer *BitB) { // seq_parameter_set_rbsp
        ParseSequenceParameterSetData(Dec, BitB);                        // seq_parameter_set_data
        AlignBitBuffer(BitB, 1);                                             // rbsp_trailing_bits();
    }
    
    /* Video Usability Information */
    void ParseVideoUsabilityInformation(DecodeAVC *Dec, BitBuffer *BitB) { // Video Usability Information; ParseVUIParameters
        Dec->VUI->AspectRatioInfoPresent                                            = ReadBits(BitB, 1, true);
        if (Dec->VUI->AspectRatioInfoPresent == true) {
            Dec->VUI->AspectRatioIDC                                                = ReadBits(BitB, 8, true);
            if (Dec->VUI->AspectRatioIDC == 255) { // Extended_SAR = 255
                Dec->VUI->SARWidth                                                  = ReadBits(BitB, 16, true);
                Dec->VUI->SAWHeight                                                 = ReadBits(BitB, 16, true);
            }
        }
        Dec->VUI->OverscanInfoPresent                                               = ReadBits(BitB, 1, true);
        if (Dec->VUI->OverscanInfoPresent == true) {
            Dec->VUI->DisplayInOverscan                                             = ReadBits(BitB, 1, true);
        }
        Dec->VUI->VideoSignalTypePresent                                            = ReadBits(BitB, 1, true);
        if (Dec->VUI->VideoSignalTypePresent == true) {
            Dec->VUI->VideoType                                                     = ReadBits(BitB, 3, true);
            Dec->VUI->FullRange                                                     = ReadBits(BitB, 1, true);
            Dec->VUI->ColorDescriptionPresent                                       = ReadBits(BitB, 1, true);
            if (Dec->VUI->ColorDescriptionPresent == true) {
                Dec->VUI->ColorPrimaries                                            = ReadBits(BitB, 8, true);
                Dec->VUI->TransferProperties                                        = ReadBits(BitB, 8, true);
                Dec->VUI->MatrixCoefficients                                        = ReadBits(BitB, 8, true);
            }
        }
        Dec->VUI->ChromaLocationPresent                                             = ReadBits(BitB, 1, true);
        if (Dec->VUI->ChromaLocationPresent == true) {
            Dec->VUI->ChromaSampleLocationTop                                       = ReadExpGolomb(BitB, false);
            Dec->VUI->ChromaSampleLocationBottom                                    = ReadExpGolomb(BitB, false);
        }
        Dec->VUI->TimingInfoPresent[0]                                              = ReadBits(BitB, 1, true);
        if (Dec->VUI->TimingInfoPresent[0] == true) {
            Dec->VUI->UnitsPerTick[0]                                               = ReadBits(BitB, 32, true);
            Dec->VUI->TimeScale[0]                                                  = ReadBits(BitB, 32, true);
            Dec->VUI->FixedFrameRateFlag[0]                                         = ReadBits(BitB, 1, true);
        }
        Dec->VUI->NALHrdParamsPresent[0]                                            = ReadBits(BitB, 1, true);
        if (Dec->VUI->NALHrdParamsPresent[0] == true) {
            ParseHypotheticalReferenceDecoder(Dec, BitB);
        }
        Dec->VUI->VCLHrdParamsPresent[0]                                            = ReadBits(BitB, 1, true);
        if (Dec->VUI->VCLHrdParamsPresent[0] == true) {
            ParseHypotheticalReferenceDecoder(Dec, BitB); // wat
        }
        if ((Dec->VUI->NALHrdParamsPresent[0] || Dec->VUI->VCLHrdParamsPresent[0]) == true) {
            //Dec->StreamIsLowDelay                                                   = ReadBits(BitB, 1, true);
        }
        Dec->VUI->PicStructPresent[0]                                               = ReadBits(BitB, 1, true);
        Dec->VUI->BitStreamRestricted                                               = ReadBits(BitB, 1, true);
        if (Dec->VUI->BitStreamRestricted == true) {
            Dec->VUI->MotionVectorsOverPicBoundaries[0]                             = ReadBits(BitB, 1, true);
            Dec->VUI->MaxBytesPerPicDenom[0]                                        = ReadExpGolomb(BitB, false);
            Dec->VUI->MaxBitsPerMarcoBlockDenom[0]                                  = ReadExpGolomb(BitB, false);
            Dec->VUI->MaxMotionVectorLength[0]                                      = ReadExpGolomb(BitB, false);
            Dec->VUI->MaxMotionVectorHeight[0]                                      = ReadExpGolomb(BitB, false);
            Dec->VUI->MaxReorderFrames[0]                                           = ReadExpGolomb(BitB, false);
            Dec->VUI->MaxFrameBuffer[0]                                             = ReadExpGolomb(BitB, false);
        }
    }
    
    void ParseMVCDVUIParametersExtension(DecodeAVC *Dec, BitBuffer *BitB) { // mvcd_vui_parameters_extension
        Dec->VUI->VUIMVCDNumOpPoints                                                = ReadExpGolomb(BitB, false) + 1;
        for (uint8_t MVCDOpPoint = 0; MVCDOpPoint < Dec->VUI->VUIMVCDNumOpPoints; MVCDOpPoint++) {
            Dec->VUI->VUIMVCDTemporalID[MVCDOpPoint]                                = ReadBits(BitB, 3, true);
            Dec->VUI->VUIMVCDNumTargetOutputViews[MVCDOpPoint]                      = ReadExpGolomb(BitB, false) + 1;
            for (uint16_t VUIMVCDView = 0; VUIMVCDView < Dec->VUI->VUIMVCDNumTargetOutputViews[MVCDOpPoint]; VUIMVCDView++) {
                Dec->VUI->VUIMVCDViewID[MVCDOpPoint][VUIMVCDView]                   = ReadExpGolomb(BitB, false);
                Dec->VUI->VUIMVCDDepthFlag[MVCDOpPoint][VUIMVCDView]                = ReadExpGolomb(BitB, false);
                Dec->VUI->VUIMVCDTextureFlag[MVCDOpPoint][VUIMVCDView]              = ReadExpGolomb(BitB, false);
            }
            Dec->VUI->VUIMVCDTimingInfoPresent[MVCDOpPoint]                         = ReadBits(BitB, 1, true);
            if (Dec->VUI->VUIMVCDTimingInfoPresent[MVCDOpPoint] == true) {
                Dec->VUI->VUIMVCDNumUnitsInTick[MVCDOpPoint]                        = ReadBits(BitB, 32, true);
                Dec->VUI->VUIMVCDTimeScale[MVCDOpPoint]                             = ReadBits(BitB, 32, true);
                Dec->VUI->VUIMVCDFixedFrameRateFlag[MVCDOpPoint]                    = ReadBits(BitB, 1, true);
            }
            Dec->VUI->VUIMVCDNalHRDParametersPresent[MVCDOpPoint]                   = ReadBits(BitB, 1, true);
            if (Dec->VUI->VUIMVCDNalHRDParametersPresent[MVCDOpPoint] == true) {
                hrd_parameters();
            }
            Dec->VUI->VUIMVCDVclHRDParametersPresent[MVCDOpPoint]                   = ReadBits(BitB, 1, true);
            if (Dec->VUI->VUIMVCDVclHRDParametersPresent[MVCDOpPoint] == true) {
                hrd_parameters();
            }
            if ((Dec->VUI->VUIMVCDNalHRDParametersPresent[MVCDOpPoint] || Dec->VUI->VUIMVCDVclHRDParametersPresent[MVCDOpPoint]) == true) {
                Dec->VUI->VUIMVCDHRDLowDelayPresent[MVCDOpPoint]                    = ReadBits(BitB, 1, true);
            }
            Dec->VUI->VUIMVCDPicStructPresent[MVCDOpPoint]                          = ReadBits(BitB, 1, true);
        }
    }
    
    void ParseMVCVUIParametersExtension(DecodeAVC *Dec, BitBuffer *BitB) { // mvc_vui_parameters_extension
        Dec->VUI->MVCNumOpertionPoints                                              = ReadExpGolomb(BitB, false) + 1;
        for (uint16_t Operation = 0; Operation < Dec->VUI->MVCNumOpertionPoints; Operation++) {
            Dec->VUI->MVCTemporalID[0][Operation]                                   = ReadBits(BitB, 3, true);
            Dec->VUI->MVCNumTargetViews[Operation]                                  = ReadExpGolomb(BitB, false) + 1;
            for (uint16_t OutputView = 0; OutputView < Dec->VUI->MVCNumTargetViews[Operation]; OutputView++) {
                Dec->VUI->MVCViewID[Operation][OutputView]                          = ReadExpGolomb(BitB, false);
            }
            Dec->VUI->TimingInfoPresent[Operation]                                  = ReadBits(BitB, 1, true);
            if (Dec->VUI->TimingInfoPresent[Operation] == true) {
                Dec->VUI->MVCUnitsInTick[Operation]                                 = ReadBits(BitB, 32, true);
                Dec->VUI->MVCTimeScale[Operation]                                   = ReadBits(BitB, 32, true);
                Dec->VUI->MVCFixedFrameRate[Operation]                              = ReadBits(BitB, 1, true);
            }
            Dec->VUI->MVCNALHRDParamsPresent[Operation]                             = ReadBits(BitB, 1, true);
            if (Dec->VUI->MVCNALHRDParamsPresent[Operation] == true) {
                ParseHypotheticalReferenceDecoder(Dec, BitB);
            }
            Dec->VUI->MVCVCLHRDParamsPresent[Operation]                             = ReadBits(BitB, 1, true);
            if (Dec->VUI->MVCVCLHRDParamsPresent[Operation] == true) {
                ParseHypotheticalReferenceDecoder(Dec, BitB);
            }
            if ((Dec->VUI->MVCNALHRDParamsPresent[Operation] || Dec->VUI->MVCVCLHRDParamsPresent[Operation]) == true) {
                Dec->VUI->MVCLowDelayFlag[Operation]                                = ReadBits(BitB, 1, true);
            }
            Dec->VUI->PicStructPresent[Operation]                                   = ReadBits(BitB, 1, true);
        }
    }
    
    void ParseSVCVUIExtension(DecodeAVC *Dec, BitBuffer *BitB) { // svc_vui_parameters_extension
        Dec->VUI->VUIExtNumEntries                                                  = ReadExpGolomb(BitB, false) + 1;
        for (uint8_t VUIExtEntry = 0; VUIExtEntry < Dec->VUI->VUIExtNumEntries; VUIExtEntry++) {
            Dec->VUI->VUIExtDependencyID[VUIExtEntry]                               = ReadBits(BitB, 3, true);
            Dec->VUI->VUIExtQualityID[VUIExtEntry]                                  = ReadBits(BitB, 4, true);
            Dec->VUI->VUIExtTemporalID[VUIExtEntry]                                 = ReadBits(BitB, 3, true);
            Dec->VUI->VUIExtTimingInfoPresentFlag[VUIExtEntry]                      = ReadBits(BitB, 1, true);
            if (Dec->VUI->VUIExtTimingInfoPresentFlag[VUIExtEntry] == true) {
                Dec->VUI->VUIExtNumUnitsInTick[VUIExtEntry]                         = ReadBits(BitB, 32, true);
                Dec->VUI->VUIExtTimeScale[VUIExtEntry]                              = ReadBits(BitB, 32, true);
                Dec->VUI->VUIExtFixedFrameRateFlag[VUIExtEntry]                     = ReadBits(BitB, 1, true);
            }
            Dec->VUI->VUIExtNALHRDPresentFlag[VUIExtEntry]                          = ReadBits(BitB, 1, true);
            if (Dec->VUI->VUIExtNALHRDPresentFlag[VUIExtEntry] == true) {
                ParseHypotheticalReferenceDecoder(Dec, BitB);
            }
            Dec->VUI->VUIExtVCLHRDPresentFlag[VUIExtEntry]                          = ReadBits(BitB, 1, true);
            if (Dec->VUI->VUIExtVCLHRDPresentFlag[VUIExtEntry] == true) {
                ParseHypotheticalReferenceDecoder(Dec, BitB);
            }
            if ((Dec->VUI->VUIExtNALHRDPresentFlag[VUIExtEntry] == true) || (Dec->VUI->VUIExtVCLHRDPresentFlag[VUIExtEntry] == true)) {
                Dec->VUI->VUIExtLowDelayHRDFlag[VUIExtEntry]                        = ReadBits(BitB, 1, true);
            }
            Dec->VUI->VUIExtPicStructPresentFlag[VUIExtEntry]                       = ReadBits(BitB, 1, true);
        }
    }
    
    /* Picture Parameter Set */
    void ParseNALPictureParameterSet(DecodeAVC *Dec, BitBuffer *BitB) { // pic_parameter_set_rbsp
        Dec->PPS->PicParamSetID                                                     = ReadExpGolomb(BitB, false); // 3?
        Dec->SPS->SeqParamSetID                                                     = ReadExpGolomb(BitB, false); // 2
        Dec->PPS->EntropyCodingMode                                                 = ReadBits(BitB, 1, true); // bit 9, 0
        Dec->PPS->BottomPicFieldOrderInSliceFlag                                    = ReadBits(BitB, 1, true); // bit 8, 0
        Dec->PPS->SliceGroups                                                       = ReadExpGolomb(BitB, false) + 1; // 2
        if (Dec->PPS->SliceGroups > 0) {
            Dec->PPS->SliceGroupMapType                                             = ReadExpGolomb(BitB, false); // 48?
            if (Dec->PPS->SliceGroupMapType == 0) {
                for (uint8_t SliceGroup = 0; SliceGroup <= Dec->PPS->SliceGroups; SliceGroup++) {
                    Dec->PPS->RunLength[SliceGroup]                                 = ReadExpGolomb(BitB, false) + 1;
                }
            } else if (Dec->PPS->SliceGroupMapType == 2) {
                for (uint8_t SliceGroup = 0; SliceGroup <= Dec->PPS->SliceGroups; SliceGroup++) {
                    Dec->PPS->TopLeft[SliceGroup]                                   = ReadExpGolomb(BitB, false);
                    Dec->PPS->BottomRight[SliceGroup]                               = ReadExpGolomb(BitB, false);
                }
            } else if ((Dec->PPS->SliceGroupMapType == 3) || (Dec->PPS->SliceGroupMapType == 4) || (Dec->PPS->SliceGroupMapType == 5)) {
                for (uint8_t SliceGroup = 0; SliceGroup <= Dec->PPS->SliceGroups; SliceGroup++) {
                    Dec->PPS->SliceGroupChangeDirection                             = ReadBits(BitB, 1, true);
                    Dec->PPS->SliceGroupChangeRate                                  = ReadExpGolomb(BitB, false) + 1;
                }
            } else if (Dec->PPS->SliceGroupMapType == 6) {
                Dec->PPS->PicSizeInMapUnits                                         = ReadExpGolomb(BitB, false) + 1;
                for (uint64_t MapUnit = 0; MapUnit <= Dec->PPS->PicSizeInMapUnits; MapUnit++) {
                    Dec->PPS->SliceGroupID[MapUnit]                                 = ReadBits(BitB, Ceili(log2(Dec->PPS->SliceGroups)), true);
                }
            }
        }
        Dec->PPS->RefIndex[0]                                                       = ReadExpGolomb(BitB, false) + 1;
        Dec->PPS->RefIndex[1]                                                       = ReadExpGolomb(BitB, false) + 1;
        Dec->PPS->WeightedPrediction                                                = ReadBits(BitB, 1, true);
        Dec->PPS->WeightedBiPrediction                                              = ReadBits(BitB, 1, true);
        Dec->PPS->InitialSliceQP                                                    = ReadExpGolomb(BitB, true) + 26;
        Dec->PPS->InitialSliceQS                                                    = ReadExpGolomb(BitB, true) + 26;
        Dec->PPS->ChromaQPOffset                                                    = ReadExpGolomb(BitB, true);
        Dec->PPS->DeblockingFilterFlag                                              = ReadBits(BitB, 1, true);
        Dec->PPS->ConstrainedIntraFlag                                              = ReadBits(BitB, 1, true);
        Dec->PPS->RedundantPictureFlag                                              = ReadBits(BitB, 1, true);
        if (more_rbsp_data() == true) {
            Dec->PPS->TransformIs8x8                                                = ReadBits(BitB, 1, true);
            Dec->PPS->SeperateScalingMatrix                                         = ReadBits(BitB, 1, true);
            if (Dec->PPS->SeperateScalingMatrix == true) {
                for (uint8_t i = 0; i < 6 + ((Dec->SPS->ChromaFormatIDC != Chroma444) ? 2 : 6) * Dec->PPS->TransformIs8x8; i++) {
                    Dec->PPS->PicScalingList[i]                                     = ReadBits(BitB, 1, true);
                    if (Dec->PPS->PicScalingList[i] == true) {
                        if (i < 6) {
                            ScalingList(Dec, BitB, ScalingList4x4[i], 16, Dec->PPS->DefaultScalingMatrix4x4[i]);
                        } else {
                            ScalingList(Dec, BitB, ScalingList8x8[i - 6], 64, Dec->PPS->DefaultScalingMatrix8x8[i - 6]);
                        }
                    }
                }
                Dec->PPS->ChromaQPOffset                                            = ReadExpGolomb(BitB, true);
            }
            AlignBitBuffer(BitB, 1);
        }
    }
    
    /* Scalable Video Coding */
    void ParseNALSVCExtension(DecodeAVC *Dec, BitBuffer *BitB) { // nal_unit_header_svc_extension
        Dec->NAL->IDRFlag                                                           = ReadBits(BitB, 1, true);
        Dec->NAL->PriorityID[0]                                                     = ReadBits(BitB, 6, true);
        Dec->NAL->NoInterLayerPredictionFlag                                        = ReadBits(BitB, 1, true);
        Dec->NAL->DependencyID[0]                                                   = ReadBits(BitB, 3, true);
        Dec->NAL->QualityID[0][0]                                                   = ReadBits(BitB, 4, true);
        Dec->NAL->TemporalID[0]                                                     = ReadBits(BitB, 3, true);
        Dec->NAL->UseReferenceBasePictureFlag                                       = ReadBits(BitB, 1, true);
        Dec->NAL->IsDisposable[0]                                                   = ReadBits(BitB, 1, true);
        Dec->NAL->OutputFlag                                                        = ReadBits(BitB, 1, true);
        SkipBits(BitB, 2); // reserved_three_2bits
    }
    
    void ParseNALSequenceParameterSetSVC(DecodeAVC *Dec, BitBuffer *BitB) { // seq_parameter_set_svc_extension
        Dec->SVC->InterLayerDeblockingFilterPresent                                 = ReadBits(BitB, 1, true);
        Dec->SVC->ExtendedSpatialScalabilityIDC                                     = ReadBits(BitB, 2, true);
        if ((Dec->SPS->ChromaFormatIDC == Chroma420) || (Dec->SPS->ChromaFormatIDC == Chroma422)) {
            Dec->SVC->ChromaPhaseXFlag                                              = ReadBits(BitB, 1, true);
        }
        if (Dec->SPS->ChromaFormatIDC == Chroma420) {
            Dec->SVC->ChromaPhaseY                                                  = ReadBits(BitB, 2, true);
        }
        if (Dec->SVC->ExtendedSpatialScalabilityIDC == 1) {
            if (Dec->SPS->ChromaFormatIDC != ChromaBW) {
                Dec->SVC->SeqRefLayerChromaPhaseX                                   = ReadBits(BitB, 1, true);
                Dec->SVC->SeqRefLayerChromaPhaseY                                   = ReadBits(BitB, 2, true);
            }
            Dec->SVC->RefLayerLeftOffset                                            = ReadExpGolomb(BitB, true);
            Dec->SVC->RefLayerTopOffset                                             = ReadExpGolomb(BitB, true);
            Dec->SVC->RefLayerRightOffset                                           = ReadExpGolomb(BitB, true);
            Dec->SVC->RefLayerBottomOffset                                          = ReadExpGolomb(BitB, true);
        }
        Dec->SVC->SequenceCoeffLevelPresent                                         = ReadBits(BitB, 1, true);
        if (Dec->SVC->SequenceCoeffLevelPresent == true) {
            Dec->SVC->AdaptiveCoeffsPresent                                         = ReadBits(BitB, 1, true);
        }
        Dec->SVC->SliceHeaderRestricted                                             = ReadBits(BitB, 1, true);
    }
    
    void ParseNALPrefixUnitSVC(DecodeAVC *Dec, BitBuffer *BitB) { // prefix_nal_unit_svc
        if (Dec->NAL->NALRefIDC != 0) {
            Dec->Slice->StoreRefBasePicFlag                                         = ReadBits(BitB, 1, true);
            if (((Dec->NAL->UseReferenceBasePictureFlag) || (Dec->Slice->StoreRefBasePicFlag)) && (!Dec->NAL->IDRFlag)) {
                ParseReferenceBasePictureSyntax(Dec, BitB); // dec_ref_base_pic_marking();
            }
            Dec->NAL->AdditionalPrefixNALExtensionFlag                              = ReadBits(BitB, 1, true);
            if (Dec->NAL->AdditionalPrefixNALExtensionFlag == true) {
                while (more_rbsp_data()) {
                    Dec->NAL->AdditionalPrefixNALExtensionDataFlag                  = ReadBits(BitB, 1, true);
                }
            }
            AlignBitBuffer(BitB, 1); // rbsp_trailing_bits()
        } else if (more_rbsp_data()) {
            while (more_rbsp_data()) {
                Dec->NAL->AdditionalPrefixNALExtensionDataFlag                      = ReadBits(BitB, 1, true);
            }
        }
        AlignBitBuffer(BitB, 1); // rbsp_trailing_bits()
    }
    
    /* Multi-View Coding */
    void ParseNALMVCExtension(DecodeAVC *Dec, BitBuffer *BitB) { // nal_unit_header_mvc_extension
        Dec->NAL->NonIDRFlag                                                        = ReadBits(BitB, 1, true);
        Dec->NAL->PriorityID[0]                                                     = ReadBits(BitB, 6, true);
        Dec->SPS->ViewID[0][0]                                                      = ReadBits(BitB, 10, true);
        Dec->NAL->TemporalID[0]                                                     = ReadBits(BitB, 3, true);
        Dec->NAL->IsAnchorPicture                                                   = ReadBits(BitB, 1, true);
        Dec->NAL->InterViewFlag                                                     = ReadBits(BitB, 1, true);
        SkipBits(BitB, 1);
    }
    
    void ParseSPSMVCDExtension(DecodeAVC *Dec, BitBuffer *BitB) { // seq_parameter_set_mvcd_extension
        Dec->SPS->ViewCount                                                         = ReadExpGolomb(BitB, false) + 1;
        for (uint16_t View = 0; View < Dec->SPS->ViewCount; View++) {
            Dec->SPS->ViewID[View][0]                                               = ReadExpGolomb(BitB, false);
            Dec->SPS->DepthViewPresent[View]                                        = ReadBits(BitB, 1, true);
            Dec->SPS->DepthViewID[View]                                             = Dec->SPS->ViewID[View][0];
            Dec->DPS->NumDepthViews                                                += Dec->SPS->DepthViewPresent[View];
            Dec->SPS->TextureViewPresent[View]                                      = ReadBits(BitB, 1, true);
        }
        for (uint16_t View = 1; View < Dec->SPS->ViewCount; View++) {
            if (Dec->SPS->DepthViewPresent[View] == true) {
                Dec->SPS->AnchorRefsCount[0][View]                                  = ReadExpGolomb(BitB, false);
                for (uint8_t AnchorRef = 0; AnchorRef < Dec->SPS->AnchorRefsCount[0][View]; AnchorRef++) {
                    Dec->SPS->AnchorRef[0][View][AnchorRef]                         = ReadExpGolomb(BitB, false);
                }
                Dec->SPS->AnchorRefsCount[1][View]                                  = ReadExpGolomb(BitB, false);
                for (uint8_t AnchorRef = 0; AnchorRef < Dec->SPS->AnchorRefsCount[1][View]; AnchorRef++) {
                    Dec->SPS->AnchorRef[1][View][AnchorRef]                         = ReadExpGolomb(BitB, false);
                }
            }
        }
        for (uint16_t View = 1; View < Dec->SPS->ViewCount; View++) {
            if (Dec->SPS->DepthViewPresent[View] == true) {
                Dec->SPS->NonAnchorRefCount[0][View]                                = ReadExpGolomb(BitB, false);
                for (uint16_t NonAnchorRef = 0; NonAnchorRef < Dec->SPS->NonAnchorRefCount[0][View]; NonAnchorRef++) {
                    Dec->SPS->NonAnchorRef[0][View][NonAnchorRef]                   = ReadExpGolomb(BitB, false);
                }
                Dec->SPS->NonAnchorRefCount[1][View]                                = ReadExpGolomb(BitB, false);
                for (uint16_t NonAnchorRef = 0; NonAnchorRef < Dec->SPS->NonAnchorRefCount[0][View]; NonAnchorRef++) {
                    Dec->SPS->NonAnchorRef[1][View][NonAnchorRef]                   = ReadExpGolomb(BitB, false);
                }
            }
        }
        Dec->SPS->NumLevelValues                                                    = ReadExpGolomb(BitB, false) + 1;
        for (uint8_t Level = 0; Level < Dec->SPS->NumLevelValues; Level++) {
            Dec->SPS->LevelIDC[Level]                                               = ReadExpGolomb(BitB, false);
            Dec->SPS->NumApplicableOps[Level]                                       = ReadExpGolomb(BitB, false) + 1;
            for (uint16_t AppOp = 0; AppOp < Dec->SPS->NumApplicableOps[Level]; AppOp++) {
                Dec->SPS->AppOpTemporalID[Level][AppOp]                             = ReadExpGolomb(BitB, false);
                Dec->SPS->AppOpNumTargetViews[Level][AppOp]                         = ReadExpGolomb(BitB, false) + 1;
                for (uint16_t AppOpTargetView = 0; AppOpTargetView < Dec->SPS->AppOpNumTargetViews[Level][AppOp]; AppOpTargetView++) {
                    Dec->SPS->AppOpTargetViewID[Level][AppOp][AppOpTargetView]      = ReadExpGolomb(BitB, false);
                    Dec->SPS->AppOpDepthFlag[Level][AppOp][AppOpTargetView]         = ReadExpGolomb(BitB, false);
                    Dec->SPS->AppOpTextureFlag[Level][AppOp][AppOpTargetView]       = ReadExpGolomb(BitB, false);
                }
                Dec->SPS->AppOpTextureViews[Level][AppOp]                           = ReadExpGolomb(BitB, false) + 1;
                Dec->SPS->AppOpNumDepthViews[Level][AppOp]                          = ReadExpGolomb(BitB, false);
            }
        }
        Dec->SPS->MVCDVUIParametersPresent                                          = ReadBits(BitB, 1, true);
        if (Dec->SPS->MVCDVUIParametersPresent == true) {
            ParseMVCDVUIParametersExtension(Dec, BitB); // mvcd_vui_parameters_extension();
        }
        Dec->SPS->MVCDTextureVUIParametersPresent                                   = ReadBits(BitB, 1, true);
        if (Dec->SPS->MVCDTextureVUIParametersPresent == true) {
            ParseMVCVUIParametersExtension(Dec, BitB); //mvc_vui_parameters_extension();
        }
    }
    
    void ParseNALDepthParameterSet(DecodeAVC *Dec, BitBuffer *BitB) { // depth_parameter_set_rbsp
        Dec->DPS->DepthParameterSetID                                               = ReadExpGolomb(BitB, false);
        Dec->DPS->PredictionDirection                                               = ReadExpGolomb(BitB, false);
        if ((Dec->DPS->PredictionDirection == 0) || (Dec->DPS->PredictionDirection == 1)) {
            Dec->DPS->ReferenceDPSID[0]                                             = ReadExpGolomb(BitB, false);
            Dec->DPS->PredictedWeight0                                              = 64;
        }
        if (Dec->DPS->PredictionDirection == 0) {
            Dec->DPS->ReferenceDPSID[1]                                             = ReadExpGolomb(BitB, false);
            Dec->DPS->PredictedWeight0                                              = ReadBits(BitB, 6, true);
        }
        Dec->DPS->NumDepthViews                                                     = ReadExpGolomb(BitB, false) + 1;
        depth_ranges(BitB, Dec->DPS->NumDepthViews, Dec->DPS->PredictionDirection, Dec->DPS->DepthParameterSetID);
        Dec->DPS->VSPParamFlag                                                      = ReadBits(BitB, 1, true);
        if (Dec->DPS->VSPParamFlag == true) {
            vsp_param(Dec, BitB, Dec->DPS->NumDepthViews, Dec->DPS->PredictionDirection, Dec->DPS->DepthParameterSetID);
        }
        Dec->DPS->AdditionalExtensionFlag                                           = ReadBits(BitB, 1, true);
        Dec->DPS->DepthMappingValues                                                = ReadExpGolomb(BitB, false);
        for (uint8_t i = 1; i <= Dec->DPS->DepthMappingValues; i++) {
            Dec->DPS->DepthRepresentationModel[i]                                   = ReadExpGolomb(BitB, false);
        }
        if (Dec->DPS->AdditionalExtensionFlag == true) {
            while (more_rbsp_data() == true) {
                Dec->DPS->AdditionalExtensionFlag                                   = ReadBits(BitB, 1, true);
            }
        }
        AlignBitBuffer(BitB, 1); // rbsp_trailing_bits
    }
    
    void ParseSPS3DAVCExtension(DecodeAVC *Dec, BitBuffer *BitB) { // seq_parameter_set_3davc_extension
        if (Dec->DPS->NumDepthViews > 0) {
            Dec->SPS->AVC3DAcquisitionIDC                                           = ReadExpGolomb(BitB, false);
            for (uint8_t View = 0; View < Dec->DPS->NumDepthViews; View++) {
                Dec->SPS->AVC3DViewID[View]                                         = ReadExpGolomb(BitB, false);
            }
            if (Dec->SPS->AVC3DAcquisitionIDC > 0) {
                DepthRanges(BitB, Dec->DPS->NumDepthViews, 2, 0);
                vsp_param(Dec, BitB, Dec->DPS->NumDepthViews, 2, 0);
            }
            Dec->SPS->ReducedResolutionFlag                                         = ReadBits(BitB, 1, true);
            if (Dec->SPS->ReducedResolutionFlag == true) {
                Dec->SPS->DepthPicWidthInMacroBlocks                                = ReadExpGolomb(BitB, false) + 1;
                Dec->SPS->DepthPicHeightInMapUnits                                  = ReadExpGolomb(BitB, false) + 1;
                Dec->SPS->DepthHorizontalDisparity                                  = ReadExpGolomb(BitB, false) + 1;
                Dec->SPS->DepthVerticalDisparity                                    = ReadExpGolomb(BitB, false) + 1;
                Dec->SPS->DepthHorizontalRSH                                        = ReadExpGolomb(BitB, false);
                Dec->SPS->DepthVerticalRSH                                          = ReadExpGolomb(BitB, false);
            } else {
                Dec->SPS->DepthHorizontalDisparity                                  = 1;
                Dec->SPS->DepthVerticalDisparity                                    = 1;
                Dec->SPS->DepthHorizontalRSH                                        = 0;
                Dec->SPS->DepthVerticalRSH                                          = 0;
            }
            Dec->SPS->DepthFrameCroppingFlag                                        = ReadBits(BitB, 1, true);
            if (Dec->SPS->DepthFrameCroppingFlag == true) {
                Dec->SPS->DepthFrameLeftCropOffset                                  = ReadExpGolomb(BitB, false);
                Dec->SPS->DepthFrameRightCropOffset                                 = ReadExpGolomb(BitB, false);
                Dec->SPS->DepthFrameTopCropOffset                                   = ReadExpGolomb(BitB, false);
                Dec->SPS->DepthFrameBottomCropOffset                                = ReadExpGolomb(BitB, false);
            }
            Dec->SPS->GridPosViewCount                                              = ReadExpGolomb(BitB, false);
            for (uint8_t TextureView = 0; TextureView < Dec->SPS->GridPosViewCount; TextureView++) {
                Dec->SPS->GridPosViewID[TextureView]                                = ReadExpGolomb(BitB, false);
                Dec->SPS->GridPosX[TextureView]                                     = ReadExpGolomb(BitB, true);
                Dec->SPS->GridPosY[TextureView]                                     = ReadExpGolomb(BitB, true);
            }
            Dec->SPS->SlicePrediction                                               = ReadBits(BitB, 1, true);
            Dec->Slice->SeqViewSynthesisFlag                                        = ReadBits(BitB, 1, true);
        }
        Dec->SPS->ALCSpsEnableFlag                                                  = ReadBits(BitB, 1, true);
        Dec->SPS->EnableRLESkipFlag                                                 = ReadBits(BitB, 1, true);
        bool AllViewsPairedFlag                                                     = AreAllViewsPaired(Dec);
        if (AllViewsPairedFlag == false) {
            for (uint8_t View = 0; View < Dec->SPS->ViewCount; View++) {
                if (Dec->SPS->TextureViewPresent[View] == true) {
                    Dec->SPS->AnchorRefsCount[0][View]                              = ReadExpGolomb(BitB, false);
                    for (uint16_t AnchorViewL0 = 0; AnchorViewL0 < Dec->SPS->AnchorRefsCount[0][View]; AnchorViewL0++) {
                        Dec->SPS->AnchorRef[0][View][AnchorViewL0]                  = ReadExpGolomb(BitB, false);
                    }
                    Dec->SPS->AnchorRefsCount[1][View]                              = ReadExpGolomb(BitB, false);
                    for (uint16_t AnchorViewL1 = 0; AnchorViewL1 < Dec->SPS->AnchorRefsCount[1][View]; AnchorViewL1++) {
                        Dec->SPS->AnchorRef[1][View][AnchorViewL1]                  = ReadExpGolomb(BitB, false);
                    }
                }
            }
            for (uint16_t View = 0; View <= Dec->SPS->ViewCount; View++) {
                if (Dec->SPS->TextureViewPresent[View] == true) {
                    Dec->SPS->NonAnchorRefCount[0][View]                            = ReadExpGolomb(BitB, false);
                    for (uint16_t NonAnchorRefL0 = 0; NonAnchorRefL0 < Dec->SPS->NonAnchorRefCount[0][View]; NonAnchorRefL0++) {
                        Dec->SPS->NonAnchorRef[0][View][NonAnchorRefL0]             = ReadExpGolomb(BitB, false);
                    }
                    Dec->SPS->NonAnchorRefCount[1][View]                            = ReadExpGolomb(BitB, false);
                    for (uint16_t NonAnchorRefL1 = 0; NonAnchorRefL1 < Dec->SPS->NonAnchorRefCount[1][View]; NonAnchorRefL1++) {
                        Dec->SPS->NonAnchorRef[1][View][NonAnchorRefL1]             = ReadExpGolomb(BitB, false);
                    }
                }
            }
        }
    }
    
    void ParseNAL3DAVCExtension(DecodeAVC *Dec, BitBuffer *BitB) { // nal_unit_header_3davc_extension
        Dec->NAL->ViewIndex                                                         = ReadBits(BitB, 8, true);
        Dec->NAL->DepthFlag                                                         = ReadBits(BitB, 1, true);
        Dec->NAL->NonIDRFlag                                                        = ReadBits(BitB, 1, true);
        Dec->NAL->TemporalID[0]                                                     = ReadBits(BitB, 3, true);
        Dec->NAL->IsAnchorPicture                                                   = ReadBits(BitB, 1, true);
        Dec->NAL->InterViewFlag                                                     = ReadBits(BitB, 1, true);
    }
    
    void ParseNALMVCDepthView(DecodeAVC *Dec, BitBuffer *BitB) {
        
    }
    
    /* Hypothetical Reference Decoder */
    void ParseHypotheticalReferenceDecoder(DecodeAVC *Dec, BitBuffer *BitB) { // hrd_parameters( )
        Dec->HRD->NumCodedPictureBuffers                                            = ReadExpGolomb(BitB, false) + 1;
        Dec->HRD->BitRateScale                                                      = ReadBits(BitB, 4, true);
        Dec->HRD->CodedPictureBufferScale                                           = ReadBits(BitB, 4, true);
        for (uint8_t SchedSelIdx = 0; SchedSelIdx < Dec->HRD->NumCodedPictureBuffers; SchedSelIdx++) {
            Dec->HRD->BitRate[SchedSelIdx]                                          = ReadExpGolomb(BitB, false) + 1;
            Dec->HRD->CodedPictureSize[SchedSelIdx]                                 = ReadExpGolomb(BitB, false) + 1;
            Dec->HRD->IsConstantBitRate[SchedSelIdx]                                = ReadBits(BitB, 1, true) + 1; // FIXME: is +1 correct
        }
        Dec->HRD->InitialCPBDelayLength                                             = ReadBits(BitB, 5, true) + 1;
        Dec->HRD->CBPDelay                                                          = ReadBits(BitB, 5, true) + 1;
        Dec->HRD->DBPDelay                                                          = ReadBits(BitB, 5, true) + 1;
        Dec->HRD->TimeOffsetSize                                                    = ReadBits(BitB, 5, true);
    }
    
    /* Generic */
    void ParseNALSliceHeader(DecodeAVC *Dec, BitBuffer *BitB) { // slice_header
        Dec->Slice->FirstMacroBlockInSlice                                          = ReadExpGolomb(BitB, false); // 0
        Dec->Slice->Type                                                            = ReadExpGolomb(BitB, false); // 0, 34 0s remaining
        Dec->PPS->PicParamSetID                                                     = ReadExpGolomb(BitB, false); // 0, 26 0s remaining
        
        if (Dec->SPS->SeperateColorPlane == true) {
            Dec->Slice->ColorPlaneID                                                = ReadBits(BitB, 2, true);
        }
        
        Dec->Slice->FrameNumber                                                     = ReadExpGolomb(BitB, false); // FIXME: Should I use ReadBits?
        if (Dec->SPS->OnlyMacroBlocksInFrame == false) {
            Dec->Slice->SliceIsInterlaced                                           = ReadBits(BitB, 1, true);
            if (Dec->Slice->SliceIsInterlaced == true) {
                Dec->Slice->SliceIsBottomField                                      = ReadBits(BitB, 1, true);
            }
        }
        Dec->Slice->SliceIsIDR                                                      = ((Dec->NAL->NALUnitType == NAL_IDRSliceNonPartitioned) ? true : false);
        if (Dec->Slice->SliceIsIDR == true) {
            Dec->Slice->IDRPicID                                                    = ReadExpGolomb(BitB, false);
        }
        if (Dec->SPS->PicOrderCount == 0) {
            Dec->Slice->PictureOrderCountLSB                                        = ReadExpGolomb(BitB, false);
            if ((Dec->PPS->BottomPicFieldOrderInSliceFlag == true) && (Dec->Slice->SliceIsInterlaced == false)) {
                Dec->Slice->DeltaPicOrderCount[0]                                   = ReadExpGolomb(BitB, true);
            }
        }
        if (Dec->SPS->PicOrderCount == true && Dec->SPS->DeltaPicOrder == false) {
            Dec->Slice->DeltaPicOrderCount[0]                                       = ReadExpGolomb(BitB, true);
            if ((Dec->PPS->BottomPicFieldOrderInSliceFlag == true) && (Dec->Slice->SliceIsInterlaced == false)) {
                Dec->Slice->DeltaPicOrderCount[1]                                   = ReadExpGolomb(BitB, true);
            }
        }
        if (Dec->PPS->RedundantPictureFlag == true) {
            Dec->PPS->RedundantPictureCount                                         = ReadExpGolomb(BitB, false);
        }
        if ((Dec->Slice->Type == SliceB1) || (Dec->Slice->Type == SliceB2)) {
            Dec->Slice->DirectSpatialMVPredictionFlag                               = ReadBits(BitB, 1, true);
            
        }
        if ((Dec->Slice->Type == SliceP1)  || (Dec->Slice->Type == SliceP2)  ||
            (Dec->Slice->Type == SliceSP1) || (Dec->Slice->Type == SliceSP2) ||
            (Dec->Slice->Type == SliceB1)  || (Dec->Slice->Type == SliceB2)) {
            Dec->Slice->NumRefIDXActiveOverrideFlag                                 = ReadBits(BitB, 1, true);
            if (Dec->Slice->NumRefIDXActiveOverrideFlag == true) {
                Dec->MacroBlock->NumRefIndexActiveLevel0                            = ReadExpGolomb(BitB, false) + 1; // num_ref_idx_l0_active_minus1
                if ((Dec->Slice->Type == SliceB1) || (Dec->Slice->Type == SliceB2)) {
                    Dec->MacroBlock->NumRefIndexActiveLevel1                        = ReadExpGolomb(BitB, false) + 1; // num_ref_idx_l1_active_minus1
                }
            }
        }
        if ((Dec->NAL->NALUnitType == NAL_AuxiliarySliceExtension) || (Dec->NAL->NALUnitType == NAL_MVCDepthView)) {
            RefPicListMVCMod(Dec, BitB);
        } else {
            RefPicListMod(Dec, BitB);
        }
        if ((Dec->PPS->WeightedPrediction == true)
            && (
                (Dec->Slice->Type == SliceP1)  ||
                (Dec->Slice->Type == SliceP2)  ||
                (Dec->Slice->Type == SliceSP1) ||
                (Dec->Slice->Type == SliceSP2) ||
                (Dec->Slice->Type == SliceB1)  ||
                (Dec->Slice->Type == SliceB2)
                ) && (Dec->PPS->WeightedBiPrediction == true)) {
                pred_weight_table(Dec, BitB);
            }
        if (Dec->NAL->NALRefIDC != 0) {
            DecodeRefPicMarking(Dec, BitB);
        }
        if ((Dec->PPS->EntropyCodingMode  == true) && (((Dec->Slice->Type != SliceI1) || (Dec->Slice->Type != SliceI2) || (Dec->Slice->Type != SliceSI1) || (Dec->Slice->Type != SliceSI2)))) {
            Dec->Slice->CabacInitIDC                                                = ReadExpGolomb(BitB, true);
        }
        Dec->Slice->SliceQPDelta                                                    = ReadExpGolomb(BitB, true);
        if (
            (Dec->Slice->Type == SliceSP1) || (Dec->Slice->Type == SliceSP2) ||
            (Dec->Slice->Type == SliceSI1) || (Dec->Slice->Type == SliceSI2)) {
            
            if ((Dec->Slice->Type == SliceSP1) || (Dec->Slice->Type == SliceSP2)) {
                Dec->Slice->DecodePMBAsSPSlice                                      = ReadBits(BitB, 1, true);
            }
            Dec->Slice->SliceQSDelta                                                = ReadExpGolomb(BitB, true);
        }
        if (Dec->PPS->DeblockingFilterFlag == true) {
            Dec->Slice->DisableDeblockingFilterIDC                                  = ReadExpGolomb(BitB, false);
            if (Dec->Slice->DisableDeblockingFilterIDC  != true) {
                Dec->Slice->SliceAlphaOffsetC0                                      = ReadExpGolomb(BitB, true);
                Dec->Slice->SliceBetaOffset                                         = ReadExpGolomb(BitB, true);
            }
        }
        if (Dec->PPS->SliceGroups > 0 && (Dec->PPS->SliceGroupMapType >= 3 && Dec->PPS->SliceGroupMapType <= 5)) {
            uint64_t Bits                                                           = Ceili(log2(Dec->PPS->PicSizeInMapUnits / Dec->PPS->SliceGroupChangeRate));
            Dec->Slice->SliceGroupChangeCycle                                       = ReadBits(BitB, Ceili(log2(Dec->PPS->PicSizeInMapUnits /  Dec->Slice->SliceGroupChangeRate)), true);
        }
    }
    
    void ParseNALSliceData(DecodeAVC *Dec, BitBuffer *BitB, uint8_t Category) { // slice_data
        if (Dec->PPS->EntropyCodingMode == Arithmetic) {
            while (IsBitBufferAligned(BitB, 1) == false) {
                SkipBits(BitB, 1); // cabac_alignment_one_bit
            }
            uint64_t CurrentMacroBlockAddress                                       = Dec->Slice->FirstMacroBlockInSlice * (Dec->Slice->MbaffFrameFlag + 1);
            bool     MoreDataFlag                                                   = true;
            Dec->Slice->PreviousMacroBlockSkipped                                   = false;
            if ((Dec->Slice->Type != SliceI1) || (Dec->Slice->Type != SliceI2) || (Dec->Slice->Type != SliceSI1) || (Dec->Slice->Type != SliceSI2)) {
                if (Dec->PPS->EntropyCodingMode == ExpGolomb) {
                    Dec->Slice->MacroBlockSkipRun                                   = ReadExpGolomb(BitB, false);
                    Dec->Slice->PreviousMacroBlockSkipped                           = (Dec->Slice->MacroBlockSkipRun > 0);
                    for (uint8_t SkippedMacroBlock = 0; SkippedMacroBlock < Dec->Slice->MacroBlockSkipRun; SkippedMacroBlock++) {
                        CurrentMacroBlockAddress                                    = NextMacroBlockAddress(Dec, CurrentMacroBlockAddress);
                    }
                }
            }
        }
    }
    
    void ParseNALSlicePartitionA(DecodeAVC *Dec, BitBuffer *BitB) { // slice_data_partition_a_layer_rbsp
        ParseSliceHeader(Dec, BitB);
        uint64_t SliceID                                                            = ReadExpGolomb(BitB, false);
        ParseSliceData(Dec, BitB, 2); /* only category 2 parts of slice_data() syntax */
        rbsp_slice_trailing_bits(Dec, BitB); // AlignBitBuffer(BitB, 1);
    }
    
    void ParseNALSlicePartitionB(DecodeAVC *Dec, BitBuffer *BitB) { // slice_data_partition_b_layer_rbsp
        uint8_t SliceID                                                             = ReadExpGolomb(BitB, false);
        if (Dec->SPS->SeperateColorPlane == true) {
            uint8_t ColorPlaneID                                                    = ReadExpGolomb(BitB, false);
        }
        if (Dec->PPS->RedundantPictureFlag == true) {
            Dec->PPS->RedundantPictureCount                                         = ReadExpGolomb(BitB, false);
        }
        ParseSliceData(Dec, BitB, 3);
        rbsp_slice_trailing_bits(Dec, BitB); // AlignBitBuffer(BitB, 1);
    }
    
    void ParseNALSlicePartitionC(DecodeAVC *Dec, BitBuffer *BitB) { // slice_data_partition_c_layer_rbsp
        uint8_t SliceID                                                             = ReadExpGolomb(BitB, false);
        if (Dec->SPS->SeperateColorPlane == true) {
            uint8_t ColorPlaneID                                                    = ReadExpGolomb(BitB, false);
        }
        if (Dec->PPS->RedundantPictureFlag == true) {
            Dec->PPS->RedundantPictureCount                                         = ReadExpGolomb(BitB, false);
        }
        ParseSliceData(Dec, BitB, 4);
        rbsp_slice_trailing_bits(Dec, BitB); // AlignBitBuffer(BitB, 1);
    }
    
    void ParseNALSliceNonPartitioned(DecodeAVC *Dec, BitBuffer *BitB) { // slice_layer_without_partitioning_rbsp
        ParseNALSliceHeader(Dec, BitB);
        ParseNALSliceData(Dec, BitB, 0); // TODO: Fix category
        AlignBitBuffer(BitB, 1); // rbsp_slice_trailing_bits();
    }
    
    void ParseNALFillerData(DecodeAVC *Dec, BitBuffer *BitB) { // filler_data_rbsp
        while (PeekBits(BitB, 8, true) == 0xFF) {
            SkipBits(BitB, 8);
        }
    }
    
    void ParseNALPrefixUnit(DecodeAVC *Dec, BitBuffer *BitB) { // prefix_nal_unit_rbsp
        if (Dec->NAL->SVCExtensionFlag == true) {
            ParseNALPrefixUnitSVC(Dec, BitB);
        }
    }
    
    void ParseNALAccessUnitDelimiter(DecodeAVC *Dec, BitBuffer *BitB) { // access_unit_delimiter_rbsp
        Dec->Slice->PictureType                                                     = ReadBits(BitB, 3, true);
        AlignBitBuffer(BitB, 1);
    }
    
    /* Supplemental Enhancement Information */
    void ParseSEIBufferingPeriod(DecodeAVC *Dec, BitBuffer *BitB) { // buffering_period
        Dec->SPS->SeqParamSetID                                                     = ReadExpGolomb(BitB, false);
        if (Dec->SEI->NalHrdBpPresentFlag == true) {
            for (uint8_t SchedSelIdx = 0; SchedSelIdx <= Dec->HRD->NumCodedPictureBuffers; SchedSelIdx++) {
                Dec->SEI->InitialCPBRemovalDelay[SchedSelIdx]                       = ReadBits(BitB, Dec->HRD->InitialCPBDelayLength, true);
                Dec->SEI->InitialCPBRemovalDelayOffset[SchedSelIdx]                 = ReadBits(BitB, Dec->HRD->InitialCPBDelayLength, true);
            }
        }
        Dec->HRD->VclHrdBpPresentFlag                                               = Dec->VUI->VCLHrdParamsPresent > 0 ? 1 : 0;
        if (Dec->HRD->VclHrdBpPresentFlag == true) {
            for (uint8_t SchedSelIdx = 0; SchedSelIdx <= Dec->HRD->NumCodedPictureBuffers; SchedSelIdx++) {
                Dec->SEI->InitialCPBRemovalDelay[SchedSelIdx]                       = ReadBits(BitB, Dec->HRD->InitialCPBDelayLength, true);
                Dec->SEI->InitialCPBRemovalDelayOffset[SchedSelIdx]                 = ReadBits(BitB, Dec->HRD->InitialCPBDelayLength, true);
            }
        }
    }
    
    uint8_t GetClockTS(uint8_t PicStruct) {
        uint8_t ClockTS                                                             = 0;
        if ((PicStruct == 0) || (PicStruct == 1) || (PicStruct == 2)) {
            ClockTS                                                                 = 1;
        } else if ((PicStruct == 3) || (PicStruct == 4) || (PicStruct == 7)) {
            ClockTS                                                                 = 2;
        } else {
            ClockTS                                                                 = 3;
        }
        return ClockTS;
    }
    
    void ParseSEIPictureTiming(DecodeAVC *Dec, BitBuffer *BitB) { // pic_timing
        if (Dec->SEI->CpbDpbDelaysPresentFlag == true) {
            Dec->SEI->CPBRemovalDelay                                               = ReadBits(BitB, Dec->HRD->CBPDelay, true);
            Dec->SEI->DPBOutputDelay                                                = ReadBits(BitB, Dec->HRD->DBPDelay, true);
        }
        if (Dec->VUI->PicStructPresent[0] == true) {
            Dec->SEI->PicStruct                                                     = ReadBits(BitB, 4, true);
            uint8_t NumClockTS = GetClockTS(Dec->SEI->PicStruct);
            for (uint8_t Time = 0; Time < NumClockTS; Time++) {
                Dec->SEI->ClockTimestampFlag[Time]                                  = ReadBits(BitB, 1, true);
                if (Dec->SEI->ClockTimestampFlag[Time] == true) {
                    Dec->SEI->CTType                                                = ReadBits(BitB, 2, true);
                    Dec->SEI->NuitFieldBasedFlag                                    = ReadBits(BitB, 1, true);
                    Dec->SEI->CountingType                                          = ReadBits(BitB, 5, true);
                    Dec->SEI->FullTimestampFlag                                     = ReadBits(BitB, 1, true);
                    Dec->SEI->CountDroppedFlag                                      = ReadBits(BitB, 1, true);
                    Dec->SEI->NFrames                                               = ReadBits(BitB, 8, true);
                    if (Dec->SEI->FullTimestampFlag == true) {
                        Dec->SEI->Seconds                                           = ReadBits(BitB, 6, true);
                        Dec->SEI->Minutes                                           = ReadBits(BitB, 6, true);
                        Dec->SEI->Hours                                             = ReadBits(BitB, 5, true);
                    } else {
                        Dec->SEI->SecondsFlag                                       = ReadBits(BitB, 1, true);
                        if (Dec->SEI->SecondsFlag == true) {
                            Dec->SEI->Seconds                                       = ReadBits(BitB, 6, true);
                            Dec->SEI->MinutesFlag                                   = ReadBits(BitB, 1, true);
                            if (Dec->SEI->MinutesFlag == true) {
                                Dec->SEI->Minutes                                   = ReadBits(BitB, 6, true);
                                Dec->SEI->HoursFlag                                 = ReadBits(BitB, 1, true);
                                if (Dec->SEI->HoursFlag == true) {
                                    Dec->SEI->Hours                                 = ReadBits(BitB, 5, true);
                                }
                            }
                        }
                        if (Dec->HRD->TimeOffsetSize > 0) {
                            Dec->SEI->TimeOffset                                    = ReadBits(BitB, Dec->HRD->TimeOffsetSize, true);
                        }
                    }
                }
            }
        }
    }
    
    void ParseSEIPanScan(DecodeAVC *Dec, BitBuffer *BitB) { // pan_scan_rect
        Dec->SEI->PanScanID                                                         = ReadExpGolomb(BitB, false);
        Dec->SEI->DisablePanScanFlag                                                = ReadBits(BitB, 1, true);
        if (Dec->SEI->DisablePanScanFlag == false) {
            Dec->SEI->PanScanCount                                                  = ReadExpGolomb(BitB, false) + 1;
            for (uint8_t PanScan = 0; PanScan < Dec->SEI->PanScanCount; PanScan++) {
                Dec->SEI->PanScanOffsetLeft[PanScan]                                = ReadExpGolomb(BitB, true);
                Dec->SEI->PanScanOffsetRight[PanScan]                               = ReadExpGolomb(BitB, true);
                Dec->SEI->PanScanOffsetTop[PanScan]                                 = ReadExpGolomb(BitB, true);
                Dec->SEI->PanScanOffsetBottom[PanScan]                              = ReadExpGolomb(BitB, true);
            }
            Dec->SEI->PanScanRepitionPeriod                                         = ReadExpGolomb(BitB, false);
        }
    }
    
    void ParseSEIFiller(DecodeAVC *Dec, BitBuffer *BitB) { // filler_payload
        SkipBits(BitB, Bytes2Bits(Dec->SEI->SEISize));
    }
    
    void ParseSEIRegisteredUserData(DecodeAVC *Dec, BitBuffer *BitB) { // user_data_registered_itu_t_t35
        uint8_t CountryCodeSize = 0;
        Dec->SEI->CountryCode                                                       = ReadBits(BitB, 8, true);
        if (Dec->SEI->CountryCode != 0xFF) {
            CountryCodeSize                                                        += 1;
        } else {
            CountryCodeSize                                                        += 2;
            Dec->SEI->CountryCode                                                 <<= 8;
            Dec->SEI->CountryCode                                                  += ReadBits(BitB, 8, true);
        }
        
    }
    
    void ParseSEIUnregisteredUserData(DecodeAVC *Dec, BitBuffer *BitB) { // user_data_unregistered
        Dec->SEI->UnregisteredUserDataUUID = ReadUUID(BitB); // DC45E9BD-E6D9-48B7-962C-D820D923EEEF, x264 UserID.
        SkipBits(BitB, Bytes2Bits(Dec->SEI->SEISize - BitIOBinaryUUIDSize));
    }
    
    void ParseSEIRecoveryPoint(DecodeAVC *Dec, BitBuffer *BitB) { // recovery_point
        Dec->SEI->RecoveryFrameCount                                                = ReadExpGolomb(BitB, false);
        Dec->SEI->ExactMatchFlag                                                    = ReadBits(BitB, 1, true);
        Dec->SEI->BrokenLinkFlag                                                    = ReadBits(BitB, 1, true);
        Dec->SEI->ChangingSliceGroupIDC                                             = ReadBits(BitB, 2, true);
    }
    
    void ParseSEIRepetitiveReferencePicture(DecodeAVC *Dec, BitBuffer *BitB) { // dec_ref_pic_marking_repetition
        Dec->SEI->OriginalIDRFlag                                                   = ReadBits(BitB, 1, true);
        Dec->SEI->OriginalFrameNum                                                  = ReadExpGolomb(BitB, false);
        if (Dec->SPS->OnlyMacroBlocksInFrame == false) {
            Dec->SEI->OriginalFieldPicFlag                                          = ReadBits(BitB, 1, true);
            if (Dec->SEI->OriginalFieldPicFlag == true) {
                Dec->SEI->OriginalBottomFieldFlag                                   = ReadBits(BitB, 1, true);
            }
        }
        DecodeRefPicMarking(Dec, BitB); // dec_ref_pic_marking();
    }
    
    void ParseSEISparePicture(DecodeAVC *Dec, BitBuffer *BitB) { // spare_pic
        uint8_t MapUnitCount = 0;
        
        Dec->SEI->TargetFrameNum                                                    = ReadExpGolomb(BitB, false);
        Dec->SEI->SpareFieldFlag                                                    = ReadBits(BitB, 1, true);
        if (Dec->SEI->SpareFieldFlag == true) {
            Dec->SEI->TargetBottomFieldFlag                                         = ReadBits(BitB, 1, true);
        }
        Dec->SEI->NumSparePics                                                      = ReadExpGolomb(BitB, false) + 1;
        for (uint8_t SparePic = 0; SparePic < Dec->SEI->NumSparePics; SparePic++) {
            Dec->SEI->DeltaSpareFrameNum[SparePic]                                  = ReadExpGolomb(BitB, false);
            if (Dec->SEI->SpareFieldFlag == true) {
                Dec->SEI->SpareBottomFieldFlag[SparePic]                            = ReadExpGolomb(BitB, false);
            }
            Dec->SEI->SpareAreaIDC[SparePic]                                        = ReadExpGolomb(BitB, false);
            if (Dec->SEI->SpareAreaIDC[SparePic] == 1) {
                for (uint8_t MapUnit = 0; MapUnit < Dec->PPS->PicSizeInMapUnits; MapUnit++) {
                    Dec->SEI->SpareUnitFlag[SparePic][MapUnit]                      = ReadBits(BitB, 1, true);
                }
            } else if (Dec->SEI->SpareAreaIDC[SparePic] == 2) {
                while (MapUnitCount < Dec->PPS->PicSizeInMapUnits) {
                    Dec->SEI->ZeroRunLength[SparePic][MapUnitCount]                 = ReadExpGolomb(BitB, false);
                    MapUnitCount += Dec->SEI->ZeroRunLength[SparePic][MapUnitCount] + 1;
                }
            }
        }
    }
    
    void ParseSEISceneInfo(DecodeAVC *Dec, BitBuffer *BitB) { // scene_info
        Dec->SEI->SceneInfoPresentFlag                                              = ReadBits(BitB, 1, true);
        if (Dec->SEI->SceneInfoPresentFlag == true) {
            Dec->SEI->SceneID                                                       = ReadExpGolomb(BitB, false);
            Dec->SEI->SceneTransitionType                                           = ReadExpGolomb(BitB, false);
            if (Dec->SEI->SceneTransitionType > 3) {
                Dec->SEI->SecondSceneID                                             = ReadExpGolomb(BitB, false);
            }
        }
    }
    
    void ParseSEISubSequenceInfo(DecodeAVC *Dec, BitBuffer *BitB) { // sub_seq_info
        Dec->SEI->SubSequenceLayerNum                                                = ReadExpGolomb(BitB, false);
        Dec->SEI->SubSequenceID                                                      = ReadExpGolomb(BitB, false);
        Dec->SEI->FirstRefPicFlag                                                    = ReadBits(BitB, 1, true);
        Dec->SEI->LeadingNonRefPicFlag                                               = ReadBits(BitB, 1, true);
        Dec->SEI->LastPicFlag                                                        = ReadBits(BitB, 1, true);
        Dec->SEI->SubSeqFrameNumFlag                                                 = ReadBits(BitB, 1, true);
        if (Dec->SEI->SubSeqFrameNumFlag  == true) {
            Dec->SEI->SubSeqFrameNum                                                 = ReadExpGolomb(BitB, false);
        }
    }
    
    void ParseSEISubSequenceLayerProperties(DecodeAVC *Dec, BitBuffer *BitB) { // sub_seq_layer_characteristics
        Dec->SEI->NumSubSeqLayers                                                    = ReadExpGolomb(BitB, false) + 1;
        for (uint8_t Layer = 0; Layer < Dec->SEI->NumSubSeqLayers; Layer++) {
            Dec->SEI->AccurateStatisticsFlag                                         = ReadBits(BitB, 1, true);
            Dec->SEI->AverageBitRate                                                 = ReadBits(BitB, 16, true);
            Dec->SEI->AverageFrameRate                                               = ReadBits(BitB, 16, true);
        }
    }
    
    void ParseSEISubSequenceProperties(DecodeAVC *Dec, BitBuffer *BitB) { // sub_seq_characteristics
        Dec->SEI->SubSequenceLayerNum                                                = ReadExpGolomb(BitB, false);
        Dec->SEI->SubSequenceID                                                      = ReadExpGolomb(BitB, false);
        Dec->SEI->DurationFlag                                                       = ReadBits(BitB, 1, true);
        if (Dec->SEI->DurationFlag == true) {
            Dec->SEI->SubSeqDuration                                                 = ReadBits(BitB, 32, true);
        }
        Dec->SEI->AverageRateFlag                                                    = ReadBits(BitB, 1, true);
        if (Dec->SEI->AverageRateFlag == true) {
            Dec->SEI->AccurateStatisticsFlag                                         = ReadBits(BitB, 1, true);
            Dec->SEI->AverageBitRate                                                 = ReadBits(BitB, 16, true);
            Dec->SEI->AverageFrameRate                                               = ReadBits(BitB, 16, true);
        }
        Dec->SEI->NumReferencedSubSeqs                                               = ReadExpGolomb(BitB, false);
        for (uint16_t SubSequence = 0; SubSequence < Dec->SEI->NumReferencedSubSeqs; SubSequence++) {
            Dec->SEI->RefSubSeqLayerNum                                              = ReadExpGolomb(BitB, false);
            Dec->SEI->RefSubSeqID                                                    = ReadExpGolomb(BitB, false);
            Dec->SEI->RefSubSeqDirection                                             = ReadBits(BitB, 1, true);
        }
    }
    
    void ParseSEIFullFrameFreeze(DecodeAVC *Dec, BitBuffer *BitB) { // full_frame_freeze
        Dec->SEI->FullFrameFreezeRepitionPeriod                                      = ReadExpGolomb(BitB, false);
    }
    
    void ParseSEIFullFrameFreezeRelease(DecodeAVC *Dec, BitBuffer *BitB) { // full_frame_freeze_release
        SkipBits(BitB, Bytes2Bits(Dec->SEI->SEISize));
    }
    
    void ParseSEIFullFrameSnapshot(DecodeAVC *Dec, BitBuffer *BitB) { // full_frame_snapshot
        Dec->SEI->SnapshotID                                                         = ReadExpGolomb(BitB, false);
    }
    
    void ParseSEIProgressiveRefinementSegmentStart(DecodeAVC *Dec, BitBuffer *BitB) { // progressive_refinement_segment_start
        Dec->SEI->ProgressiveRefinementID                                            = ReadExpGolomb(BitB, false);
        Dec->SEI->NumRefinementSteps                                                 = ReadExpGolomb(BitB, false) + 1;
    }
    
    void ParseSEIProgressiveRefinementSegmentEnd(DecodeAVC *Dec, BitBuffer *BitB) { // progressive_refinement_segment_end
        Dec->SEI->ProgressiveRefinementID                                            = ReadExpGolomb(BitB, false);
    }
    
    void ParseSEIMotionConstrainedSliceGroupSet(DecodeAVC *Dec, BitBuffer *BitB) { // motion_constrained_slice_group_set
        Dec->SEI->NumSliceGroupsInSet                                                = ReadExpGolomb(BitB, false) + 1;
        if (Dec->SEI->NumSliceGroupsInSet > 1) {
            for (uint16_t SliceGroup = 0; SliceGroup < Dec->SEI->NumSliceGroupsInSet; SliceGroup++) {
                Dec->PPS->SliceGroupID[SliceGroup]                                   = ReadBits(BitB, Ceili(log2(Dec->PPS->SliceGroups)), true);
            }
        }
        Dec->SEI->ExactSampleValueMatchFlag[0]                                       = ReadBits(BitB, 1, true);
        Dec->SEI->PanScanRectFlag                                                    = ReadBits(BitB, 1, true);
        if (Dec->SEI->PanScanRectFlag == true) {
            Dec->SEI->PanScanID                                                      = ReadExpGolomb(BitB, false);
        }
    }
    
    void ParseSEIFilmGrainCharacteristics(DecodeAVC *Dec, BitBuffer *BitB) { // film_grain_characteristics
        Dec->SEI->FilmGrainCharactisticsCancelFlag                                   = ReadBits(BitB, 1, true);
        if (Dec->SEI->FilmGrainCharactisticsCancelFlag == false) {
            Dec->SEI->FilmGrainModelID                                               = ReadBits(BitB, 2, true);
            Dec->SEI->SeperateColorDescriptionFlag                                   = ReadBits(BitB, 1, true);
            if (Dec->SEI->SeperateColorDescriptionFlag == true) {
                Dec->SEI->FilmGrainBitDepthLuma                                      = ReadBits(BitB, 3, true) + 8;
                Dec->SEI->FilmGrainBitDepthChroma                                    = ReadBits(BitB, 3, true) + 8;
                Dec->SEI->FilmGrainFullRangeFlag                                     = ReadBits(BitB, 1, true);
                Dec->SEI->FilmGrainColorPrimaries                                    = ReadBits(BitB, 8, true);
                Dec->SEI->FilmGrainTransferCharacteristics                           = ReadBits(BitB, 8, true);
                Dec->SEI->FilmGrainMatrixCoefficents                                 = ReadBits(BitB, 8, true);
            }
            Dec->SEI->BlendingModeID                                                 = ReadBits(BitB, 2, true);
            Dec->SEI->Scalefactor                                                    = ReadBits(BitB, 4, true);
            for (uint8_t Channel = 0; Channel < 3; Channel++) {
                Dec->SEI->CompModelPresent[Channel]                                  = ReadBits(BitB, 1, true);
            }
            for (uint8_t Channel = 0; Channel < 3; Channel++) {
                if (Dec->SEI->CompModelPresent[Channel] == true) {
                    Dec->SEI->NumIntensityIntervals[Channel]                         = ReadBits(BitB, 8, true) + 1;
                    Dec->SEI->NumModelValues[Channel]                                = ReadBits(BitB, 3, true) + 1;
                    for (uint16_t Intensity = 0; Intensity < Dec->SEI->NumIntensityIntervals[Channel]; Intensity++) {
                        Dec->SEI->IntensityIntervalLowerBound[Channel][Intensity]    = ReadBits(BitB, 8, true);
                        Dec->SEI->IntensityIntervalUpperBound[Channel][Intensity]    = ReadBits(BitB, 8, true);
                        for (uint8_t ModelValue = 0; ModelValue < Dec->SEI->NumModelValues[Channel]; ModelValue++) {
                            Dec->SEI->CompModelValue[Channel][Intensity][ModelValue] = ReadExpGolomb(BitB, true);
                        }
                    }
                }
            }
            Dec->SEI->FilmGrainCharacteristicsRepetitionPeriod                       = ReadExpGolomb(BitB, false);
        }
    }
    
    void ParseSEIDeblockingFilterDisplayPreference(DecodeAVC *Dec, BitBuffer *BitB) { // deblocking_filter_display_preference
        Dec->SEI->DeblockingDisplayPreferenceCancelFlag           = ReadBits(BitB, 1, true);
        if (Dec->SEI->DeblockingDisplayPreferenceCancelFlag == false) {
            Dec->SEI->DisplayBeforeDeblockingFlag                 = ReadBits(BitB, 1, true);
            Dec->SEI->DecodeFrameBufferingConstraintFlag          = ReadBits(BitB, 1, true);
            Dec->SEI->DeblockingDisplayPreferenceRepetitionPeriod = ReadExpGolomb(BitB, false);
        }
    }
    
    void ParseSEIStereoVideoInfo(DecodeAVC *Dec, BitBuffer *BitB) { // stereo_video_info
        Dec->SEI->FieldViewsFlag               = ReadBits(BitB, 1, true);
        if (Dec->SEI->FieldViewsFlag == true) {
            Dec->SEI->TopFieldIsLeftViewFlag       = ReadBits(BitB, 1, true);
        } else {
            Dec->SEI->CurrentFrameIsLeftViewFlag   = ReadBits(BitB, 1, true);
            Dec->SEI->NextFrameIsDependentViewFlag = ReadBits(BitB, 1, true);
        }
        Dec->SEI->LeftViewSelfContainedFlag    = ReadBits(BitB, 1, true);
        Dec->SEI->RightViewSelfContainedFlag   = ReadBits(BitB, 1, true);
    }
    
    void ParseSEIPostFilterHint(DecodeAVC *Dec, BitBuffer *BitB) { // post_filter_hint
        Dec->SEI->FilterHintSizeY     = ReadExpGolomb(BitB, false);
        Dec->SEI->FilterHintSizeX     = ReadExpGolomb(BitB, false);
        Dec->SEI->FilterHintType      = ReadBits(BitB, 2, true);
        for (uint8_t Color = 0; Color < 3; Color++) {
            for (uint8_t CY = 0; CY < Dec->SEI->FilterHintSizeY; CY++) {
                for (uint8_t CX = 0; CX < Dec->SEI->FilterHintSizeX; CX++) {
                    Dec->SEI->FilterHint[Color][CY][CX] = ReadExpGolomb(BitB, true);
                }
            }
        }
        Dec->SPS->AdditionalExtensionFlag = ReadBits(BitB, 1, true);
    }
    
    void ParseSEIToneMappingInfo(DecodeAVC *Dec, BitBuffer *BitB) { // tone_mapping_info
        Dec->SEI->ToneMapID  = ReadExpGolomb(BitB, false);
        Dec->SEI->ToneMapCancelFlag = ReadBits(BitB, 1, true);
        if (Dec->SEI->ToneMapCancelFlag == false) {
            Dec->SEI->ToneMapRepetitionPeriod = ReadExpGolomb(BitB, false);
            Dec->SEI->CodedDataBitDepth       = ReadBits(BitB, 8, true);
            Dec->SEI->TargetBitDepth   = ReadBits(BitB, 8, true);
            Dec->SEI->ToneMapModelID = ReadExpGolomb(BitB, false);
            if (Dec->SEI->ToneMapModelID == 0) {
                Dec->SEI->ToneMinValue = ReadBits(BitB, 32, true);
                Dec->SEI->ToneMaxValue = ReadBits(BitB, 32, true);
            } else if (Dec->SEI->ToneMapModelID == 1) {
                Dec->SEI->SigmoidMidpoint = ReadBits(BitB, 32, true);
                Dec->SEI->SigmoidWidth    = ReadBits(BitB, 32, true);
            } else if (Dec->SEI->ToneMapModelID == 2) {
                for (uint16_t I = 0; I < (1 << Dec->SEI->TargetBitDepth); I++) {
                    Dec->SEI->StartOfCodedInterval[I] = ReadBits(BitB, ((Dec->SEI->CodedDataBitDepth + 7) >> 3) << 3, true);
                }
            } else if (Dec->SEI->ToneMapModelID == 3) {
                Dec->SEI->NumPivots = ReadBits(BitB, 16, true);
                for (uint16_t I = 0; I < Dec->SEI->NumPivots; I++) {
                    Dec->SEI->CodedPivotValue[I] = ReadBits(BitB, ((Dec->SEI->CodedDataBitDepth + 7) >> 3) << 3, true);
                    Dec->SEI->TargetPivotValue[I] = ReadBits(BitB, ((Dec->SEI->TargetBitDepth + 7) >> 3) << 3, true);
                }
            } else if (Dec->SEI->ToneMapModelID == 4) {
                Dec->SEI->CameraISOSpeedIDC = ReadBits(BitB, 8, true);
                if (Dec->SEI->CameraISOSpeedIDC == ExtendedISO) {
                    Dec->SEI->CameraISOSpeed = ReadBits(BitB, 32, true);
                }
                Dec->SEI->ExposureIndexIDC = ReadBits(BitB, 8, true);
                if (Dec->SEI->ExposureIndexIDC == ExtendedISO) {
                    Dec->SEI->ExposureIndexValue = ReadBits(BitB, 32, true);
                }
                Dec->SEI->ExposureCompensationSignFlag = ReadBits(BitB, 1, true);
                Dec->SEI->ExposureCompensationNumerator = ReadBits(BitB, 16, true);
                Dec->SEI->ExposureCompensationDenominatorIDC = ReadBits(BitB, 16, true);
                Dec->SEI->RefScreenLuminanceWhite = ReadBits(BitB, 32, true);
                Dec->SEI->ExtendedRangeWhiteLevel = ReadBits(BitB, 32, true);
                Dec->SEI->NominalBlackLumaCode    = ReadBits(BitB, 16, true);
                Dec->SEI->NominalWhiteLumaCode    = ReadBits(BitB, 16, true);
                Dec->SEI->ExtendedWhiteLumaCode   = ReadBits(BitB, 16, true);
            }
        }
    }
    
    void ParseSEIScalabilityInfo(DecodeAVC *Dec, BitBuffer *BitB) { // scalability_info
        Dec->SEI->TemporalIDNestingFlag                      = ReadBits(BitB, 1, true);
        Dec->SEI->PriorityLayerInfoPresent                   = ReadBits(BitB, 1, true);
        Dec->SEI->PriorityIDSettingFlag                      = ReadBits(BitB, 1, true);
        Dec->SEI->NumLayers                                  = ReadExpGolomb(BitB, false) + 1;
        for (uint8_t Layer = 0; Layer < Dec->SEI->NumLayers; Layer++) {
            Dec->SEI->LayerID[Layer]                         = ReadExpGolomb(BitB, false);
            Dec->NAL->PriorityID[Layer]                      = ReadBits(BitB, 6, true);
            Dec->NAL->IsDisposable[Layer]                    = ReadBits(BitB, 1, true);
            Dec->NAL->DependencyID[Layer]                    = ReadBits(BitB, 3, true);
            Dec->NAL->QualityID[0][Layer]                    = ReadBits(BitB, 4, true); // FIXME; is this correct?
            Dec->NAL->TemporalID[Layer]                      = ReadBits(BitB, 3, true);
            Dec->SEI->SubPicLayerFlag[Layer]                 = ReadBits(BitB, 1, true);
            Dec->SEI->SubRegionLayerFlag[Layer]              = ReadBits(BitB, 1, true);
            Dec->SEI->IROIDivisionFlag[Layer]                = ReadBits(BitB, 1, true);
            Dec->SEI->ProfileLevelInfoPresentFlag[Layer]     = ReadBits(BitB, 1, true);
            Dec->SEI->BitRateInfoPresent[Layer]              = ReadBits(BitB, 1, true);
            Dec->SEI->FrameRateInfoPresent[Layer]            = ReadBits(BitB, 1, true);
            Dec->SEI->FrameSizeInfoPresentFlag[Layer]        = ReadBits(BitB, 1, true);
            Dec->SEI->LayerDependencyInfoPresent[Layer]      = ReadBits(BitB, 1, true);
            Dec->SEI->ParameterSetsInfoPresent[Layer]        = ReadBits(BitB, 1, true);
            Dec->SEI->BitstreamRestrictionInfoPresent[Layer] = ReadBits(BitB, 1, true);
            Dec->SEI->ExactInterLayerPredFlag[Layer]         = ReadBits(BitB, 1, true);
            if ((Dec->SEI->SubPicLayerFlag[Layer] == true) || (Dec->SEI->IROIDivisionFlag[Layer] == true)) {
                Dec->SEI->ExactSampleValueMatchFlag[Layer]   = ReadBits(BitB, 1, true);
            }
            Dec->SEI->LayerConversionFlag[Layer]             = ReadBits(BitB, 1, true);
            Dec->SEI->LayerOutputFlag[Layer]                 = ReadBits(BitB, 1, true);
            if (Dec->SEI->ProfileLevelInfoPresentFlag[Layer] == true) {
                Dec->SEI->LayerProfileLevelID[Layer]         = ReadBits(BitB, 24, true);
            }
            if (Dec->SEI->BitRateInfoPresent[Layer] == true) {
                Dec->SEI->AvgBitRate[Layer]                  = ReadBits(BitB, 16, true);
                Dec->SEI->MaxBitRate[Layer]                  = ReadBits(BitB, 16, true);
                Dec->SEI->MaxBitRateRepresentation[Layer]    = ReadBits(BitB, 16, true);
                Dec->SEI->MaxBitRateWindow[Layer]            = ReadBits(BitB, 16, true);
            }
            if (Dec->SEI->FrameRateInfoPresent[Layer] == true) {
                Dec->SEI->ConstantFrameRateIDC[Layer]        = ReadBits(BitB, 2, true);
                Dec->SEI->AvgFrameRate[Layer]                = ReadBits(BitB, 16, true);
            }
            if ((Dec->SEI->FrameSizeInfoPresentFlag[Layer] == true) || (Dec->SEI->IROIDivisionFlag[Layer] == true)) {
                Dec->SEI->FrameWidthInMacroBlocks[Layer]     = ReadExpGolomb(BitB, false) + 1;
                Dec->SEI->FrameHeightInMacroBlocks[Layer]    = ReadExpGolomb(BitB, false) + 1;
            }
            if (Dec->SEI->SubRegionLayerFlag[Layer] == true) {
                Dec->SEI->BaseRegionLayerID[Layer]           = ReadExpGolomb(BitB, false);
                Dec->SEI->DynamicRectFlag[Layer]             = ReadBits(BitB, 1, true);
                if (Dec->SEI->DynamicRectFlag[Layer] == false) {
                    Dec->SEI->HorizontalOffset[Layer]        = ReadBits(BitB, 16, true);
                    Dec->SEI->VerticalOffset[Layer]          = ReadBits(BitB, 16, true);
                    Dec->SEI->RegionWidth[Layer]             = ReadBits(BitB, 16, true);
                    Dec->SEI->RegionHeight[Layer]            = ReadBits(BitB, 16, true);
                }
            }
            if (Dec->SEI->SubPicLayerFlag[Layer] == true) {
                Dec->SEI->ROIID[Layer]                       = ReadExpGolomb(BitB, false);
            }
            if (Dec->SEI->IROIDivisionFlag[Layer] == true) {
                Dec->SEI->IROIGridFlag[Layer]                = ReadBits(BitB, 1, true);
                if (Dec->SEI->IROIGridFlag[Layer] == true) {
                    Dec->SEI->GridWidthInMacroBlocks[Layer]  = ReadExpGolomb(BitB, false) + 1;
                    Dec->SEI->GridHeightInMacroBlocks[Layer] = ReadExpGolomb(BitB, false) + 1;
                } else {
                    Dec->SEI->NumROIs[Layer]                       = ReadExpGolomb(BitB, false) + 1;
                    for (uint8_t ROI = 0; ROI < Dec->SEI->NumROIs[Layer]; ROI++) {
                        Dec->SEI->FirstMacroBlockInROI[Layer][ROI] = ReadExpGolomb(BitB, false);
                        Dec->SEI->ROIWidthInMacroBlock[Layer][ROI] = ReadExpGolomb(BitB, false) + 1;
                        Dec->SEI->ROIHeightInMacroBlock[Layer][ROI] = ReadExpGolomb(BitB, false) + 1;
                        
                    }
                }
            }
            if (Dec->SEI->LayerDependencyInfoPresent[Layer] == true) {
                Dec->SEI->NumDependentLayers[Layer]          = ReadExpGolomb(BitB, false);
                for (uint8_t DependentLayer = 0; DependentLayer < Dec->SEI->NumDependentLayers[Layer]; DependentLayer++) {
                    Dec->SEI->DirectlyDependentLayerIDDelta[Layer][DependentLayer] = ReadExpGolomb(BitB, false) + 1;
                }
            } else {
                Dec->SEI->LayerDepInfoSourceLayerIDDelta[Layer] = ReadExpGolomb(BitB, false) + 1;
                if (Dec->SEI->ParameterSetsInfoPresent[Layer] == true) {
                    Dec->SEI->NumSequenceParameterSets[Layer] = ReadExpGolomb(BitB, false);
                    for (uint8_t SPS = 0; SPS < Dec->SEI->NumSequenceParameterSets[Layer]; SPS++) {
                        Dec->SEI->SPSIDDelta[Layer][SPS] = ReadExpGolomb(BitB, false);
                    }
                    Dec->SEI->NumSubsetSPS[Layer]    = ReadExpGolomb(BitB, false);
                    for (uint8_t SubsetSPS = 0; SubsetSPS < Dec->SEI->NumSubsetSPS[Layer]; SubsetSPS++) {
                        Dec->SEI->SubsetSPSIDDelta[Layer][SubsetSPS] = ReadExpGolomb(BitB, false);
                    }
                    Dec->SEI->NumPicParameterSets[Layer] = ReadExpGolomb(BitB, false) + 1;
                    for (uint8_t PicParameterSet = 0; PicParameterSet < Dec->SEI->NumPicParameterSets[Layer]; PicParameterSet++) {
                        Dec->SEI->PicParameterSetIDDelta[Layer][PicParameterSet] = ReadExpGolomb(BitB, false);
                    }
                } else {
                    Dec->SEI->PPSInfoSrcLayerIDDelta[Layer] = ReadExpGolomb(BitB, false);
                }
                if (Dec->SEI->BitstreamRestrictionInfoPresent[Layer] == true) {
                    Dec->VUI->MotionVectorsOverPicBoundaries[Layer] = ReadExpGolomb(BitB, false);
                    Dec->VUI->MaxBytesPerPicDenom[Layer]            = ReadExpGolomb(BitB, false);
                    Dec->VUI->MaxBitsPerMarcoBlockDenom[Layer]      = ReadExpGolomb(BitB, false);
                    Dec->VUI->MaxMotionVectorLength[Layer]          = ReadExpGolomb(BitB, false);
                    Dec->VUI->MaxMotionVectorHeight[Layer]          = ReadExpGolomb(BitB, false);
                    Dec->VUI->MaxReorderFrames[Layer]               = ReadExpGolomb(BitB, false);
                    Dec->VUI->MaxFrameBuffer[Layer]                 = ReadExpGolomb(BitB, false);
                }
                if (Dec->SEI->LayerConversionFlag[Layer] == true) {
                    Dec->SEI->ConversionTypeIDC[Layer]              = ReadExpGolomb(BitB, false);
                    for (uint8_t J = 0; J < 2; J++) {
                        Dec->SEI->RewritingInfoFlag[Layer][J]       = ReadBits(BitB, 1, true);
                        if (Dec->SEI->RewritingInfoFlag[Layer][J] == true) {
                            Dec->SEI->RewritingProfileLevelIDC[Layer][J] = ReadBits(BitB, 24, true);
                            Dec->SEI->RewritingAverageBitrate[Layer][J]  = ReadBits(BitB, 16, true);
                            Dec->SEI->RewritingMaxBitrate[Layer][J]      = ReadBits(BitB, 16, true);
                        }
                    }
                }
            }
        }
        if (Dec->SEI->PriorityLayerInfoPresent == true) {
            Dec->SEI->NumDependencyLayersForPriorityLayer = ReadBits(BitB, 1, true) + 1;
            for (uint8_t DependentLayer = 0; DependentLayer < Dec->SEI->NumDependencyLayersForPriorityLayer; DependentLayer++) {
                Dec->SEI->PriorityDependencyID[DependentLayer] = ReadBits(BitB, 3, true);
                Dec->SEI->NumPriorityLayers[DependentLayer]    = ReadExpGolomb(BitB, false) + 1;
                for (uint8_t PriorityLayer = 0; PriorityLayer < Dec->SEI->NumPriorityLayers[DependentLayer]; PriorityLayer++) {
                    Dec->SEI->PriorityLayerID[DependentLayer][PriorityLayer]   = ReadExpGolomb(BitB, false);
                    Dec->SEI->PriorityLevelIDC[DependentLayer][PriorityLayer]  = ReadBits(BitB, 24, true);
                    Dec->SEI->PriorityLevelAvgBitrate[DependentLayer][PriorityLayer]  = ReadBits(BitB, 16, true);
                    Dec->SEI->PriorityLevelMaxBitrate[DependentLayer][PriorityLayer]  = ReadBits(BitB, 16, true);
                }
            }
        }
        if (Dec->SEI->PriorityIDSettingFlag == true) {
            Dec->SEI->PriorityIDURIIndex = 0;
            Dec->SEI->PriorityIDSettingURI = ReadBits(BitB, 8, true);
            for (uint8_t StringByte = Dec->SEI->PriorityIDSettingURI; StringByte > 0; StringByte--) {
                Dec->SEI->PriorityIDSettingURIString[StringByte] = ReadBits(BitB, 8, true);
            }
        }
    }
    
    void ParseSEISubPictureScalableLayer(DecodeAVC *Dec, BitBuffer *BitB) { // sub_pic_scalable_layer
        Dec->SEI->LayerID[0] = ReadExpGolomb(BitB, false);
    }
    
    void ParseSEINonRequiredLayerRep(DecodeAVC *Dec, BitBuffer *BitB) { // non_required_layer_rep
        Dec->SEI->NumInfoEntries = ReadExpGolomb(BitB, false) + 1;
        for (uint32_t InfoEntry = 0; InfoEntry < Dec->SEI->NumInfoEntries; InfoEntry++) {
            Dec->SEI->EntryDependencyID[InfoEntry] = ReadBits(BitB, 3, true);
            Dec->SEI->NumNonRequiredLayerReps[InfoEntry] = ReadExpGolomb(BitB, false) + 1;
            for (uint8_t LayerRep = 0; LayerRep < Dec->SEI->NumNonRequiredLayerReps[InfoEntry]; LayerRep++) {
                Dec->SEI->NonRequiredLayerRepDependencyID[InfoEntry][LayerRep] = ReadBits(BitB, 3, true);
                Dec->SEI->NonRequiredLayerRepQualityID[InfoEntry][LayerRep] = ReadBits(BitB, 4, true);
            }
        }
    }
    
    void ParseSEIPriorityLayerInfo(DecodeAVC *Dec, BitBuffer *BitB) { // priority_layer_info
        Dec->SEI->PriorityDependencyID[0] = ReadBits(BitB, 3, true);
        Dec->SEI->NumPriorityIDs          = ReadBits(BitB, 4, true);
        for (uint8_t PriorityID = 0; PriorityID < Dec->SEI->NumPriorityIDs; PriorityID++) {
            Dec->SEI->AltPriorityIDs[PriorityID] = ReadBits(BitB, 6, true);
        }
    }
    
    void ParseSEILayersNotPresent(DecodeAVC *Dec, BitBuffer *BitB) { // layers_not_present
        Dec->SEI->NumLayersNotPresent = ReadExpGolomb(BitB, false);
        for (uint8_t Layer = 0; Layer < Dec->SEI->NumLayersNotPresent; Layer++) {
            Dec->SEI->LayerID[Layer] = ReadExpGolomb(BitB, false);
        }
    }
    
    void ParseSEILayerDependencyChange(DecodeAVC *Dec, BitBuffer *BitB) { // layer_dependency_change
        Dec->SEI->NumLayers = ReadExpGolomb(BitB, false) + 1;
        for (uint8_t Layer = 0; Layer < Dec->SEI->NumLayers; Layer++) {
            Dec->SEI->LayerID[Layer] = ReadExpGolomb(BitB, false);
            Dec->SEI->LayerDependencyInfoPresent[Layer] = ReadBits(BitB, 1, true);
            if (Dec->SEI->LayerDependencyInfoPresent[Layer] == true) {
                Dec->SEI->NumDependentLayers[Layer] = ReadExpGolomb(BitB, false);
                for (uint8_t DependentLayer = 0; DependentLayer < Dec->SEI->NumDependentLayers[Layer]; DependentLayer++) {
                    Dec->SEI->DirectlyDependentLayerIDDelta[Layer][DependentLayer] = ReadExpGolomb(BitB, false) + 1;
                }
            } else {
                Dec->SEI->LayerDepInfoSourceLayerIDDelta[Layer] = ReadExpGolomb(BitB, false) + 1;
            }
        }
    }
    
    void ParseSEIScalableNesting(DecodeAVC *Dec, BitBuffer *BitB) { // scalable_nesting
        Dec->SEI->AllLayerRepresentationsInAUFlag = ReadBits(BitB, 1, true);
        if (Dec->SEI->AllLayerRepresentationsInAUFlag == false) {
            Dec->SEI->NumLayerRepresentations = ReadExpGolomb(BitB, false) + 1;
            for (uint8_t LayerRepresentation = 0; LayerRepresentation < Dec->SEI->NumLayerRepresentations; LayerRepresentation++) {
                Dec->SEI->SEIDependencyID[LayerRepresentation] = ReadBits(BitB, 3, true);
                Dec->SEI->SEIQualityID[LayerRepresentation]    = ReadBits(BitB, 4, true);
            }
            Dec->SEI->SEITemporalID[0] = ReadBits(BitB, 3, true);
        }
        AlignBitBuffer(BitB, 1);
        ParseSEIMessage(Dec, BitB); // sei_message();
    }
    
    void ParseSEIBaseLayerTemporalHRD(DecodeAVC *Dec, BitBuffer *BitB) { // base_layer_temporal_hrd
        Dec->SEI->NumTemporalLayersInBaseLayer                 = ReadExpGolomb(BitB, false) + 1;
        for (uint8_t TemporalLayer = 0; TemporalLayer < Dec->SEI->NumTemporalLayersInBaseLayer; TemporalLayer++) {
            Dec->SEI->SEITemporalID[TemporalLayer]             = ReadBits(BitB, 3, true);
            Dec->SEI->SEITimingInfoPresent[TemporalLayer]      = ReadBits(BitB, 1, true);
            if (Dec->SEI->SEITimingInfoPresent[TemporalLayer] == true) {
                Dec->SEI->SEIUnitsInTick[TemporalLayer]        = ReadBits(BitB, 32, true);
                Dec->SEI->SEITimeScale[TemporalLayer]          = ReadBits(BitB, 32, true);
                Dec->SEI->SEIFixedFrameRateFlag[TemporalLayer] = ReadBits(BitB, 1, true);
            }
            Dec->SEI->SEINALHRDParamsPresentFlag[TemporalLayer] = ReadBits(BitB, 1, true);
            if (Dec->SEI->SEINALHRDParamsPresentFlag[TemporalLayer] == true) {
                ParseHypotheticalReferenceDecoder(Dec, BitB);
            }
            Dec->SEI->SEIVCLHRDParamsPresentFlag[TemporalLayer] = ReadBits(BitB, 1, true);
            if (Dec->SEI->SEIVCLHRDParamsPresentFlag[TemporalLayer] == true) {
                ParseHypotheticalReferenceDecoder(Dec, BitB);
            }
            if ((Dec->SEI->SEINALHRDParamsPresentFlag[TemporalLayer] == true) || (Dec->SEI->SEIVCLHRDParamsPresentFlag[TemporalLayer] == true)) {
                Dec->SEI->SEILowDelayHRDFlag[TemporalLayer] = ReadBits(BitB, 1, true);
            }
            Dec->SEI->SEIPicStructPresentFlag[TemporalLayer] = ReadBits(BitB, 1, true);
        }
    }
    
    void ParseSEIQualityLayerIntegrityCheck(DecodeAVC *Dec, BitBuffer *BitB) { // quality_layer_integrity_check
        Dec->SEI->NumInfoEntries = ReadExpGolomb(BitB, false) + 1;
        for (uint8_t IntegrityCheck = 0; IntegrityCheck < Dec->SEI->NumInfoEntries; IntegrityCheck++) {
            Dec->SEI->EntryDependencyID[IntegrityCheck] = ReadBits(BitB, 3, true);
            Dec->SEI->SEIQualityLayerCRC[IntegrityCheck] = ReadBits(BitB, 16, true);
        }
    }
    
    void ParseSEIRedundantPicProperty(DecodeAVC *Dec, BitBuffer *BitB) { // redundant_pic_property
        Dec->SEI->NumDependencyIDs = ReadExpGolomb(BitB, false) + 1;
        for (uint8_t DependencyID = 0; DependencyID < Dec->SEI->NumDependencyIDs; DependencyID++) {
            Dec->NAL->DependencyID[DependencyID]  = ReadBits(BitB, 3, true);
            Dec->SEI->NumQualityIDs[DependencyID] = ReadExpGolomb(BitB, false) + 1;
            for (uint8_t QualityID = 0; QualityID < Dec->SEI->NumQualityIDs[DependencyID]; QualityID++) {
                Dec->NAL->QualityID[DependencyID][QualityID] = ReadBits(BitB, 4, true);
                Dec->SEI->NumRedundantPics[DependencyID][QualityID] = ReadExpGolomb(BitB, false) + 1;
                for (uint8_t RedundantPic = 0; RedundantPic < Dec->SEI->NumRedundantPics[DependencyID][QualityID]; RedundantPic++) {
                    Dec->SEI->RedundantPicCount[DependencyID][QualityID][RedundantPic] = ReadExpGolomb(BitB, false) + 1;
                    Dec->SEI->RedundantPicsMatch[DependencyID][QualityID][RedundantPic] = ReadBits(BitB, 1, true);
                    if (Dec->SEI->RedundantPicsMatch[DependencyID][QualityID][RedundantPic] == false) {
                        Dec->SEI->MBTypeMatchFlag[DependencyID][QualityID][RedundantPic] = ReadBits(BitB, 1, true);
                        Dec->SEI->MotionMatchFlag[DependencyID][QualityID][RedundantPic] = ReadBits(BitB, 1, true);
                        Dec->SEI->ResidualMatchFlag[DependencyID][QualityID][RedundantPic] = ReadBits(BitB, 1, true);
                        Dec->SEI->IntraSamplesMatchFlag[DependencyID][QualityID][RedundantPic] = ReadBits(BitB, 1, true);
                    }
                }
            }
        }
    }
    
    void ParseSEITemporalDependencyRepresentationIndex(DecodeAVC *Dec, BitBuffer *BitB) { // tl0_dep_rep_index
        Dec->SEI->TemporalDependencyRepresentationIndexLevel0 = ReadBits(BitB, 8, true);
        Dec->SEI->EffectiveIDRPicID                           = ReadBits(BitB, 16, true);
    }
    
    void ParseSEITemporalLevelSwitchingPoint(DecodeAVC *Dec, BitBuffer *BitB) { // tl_switching_point
        Dec->SEI->DeltaFrameNum = ReadExpGolomb(BitB, true);
    }
    
    void ParseSEIParallelDecodingInfo(DecodeAVC *Dec, BitBuffer *BitB) { // parallel_decoding_info
        Dec->SPS->SeqParamSetID = ReadExpGolomb(BitB, false);
        for (uint8_t View = 0; View <= Dec->SPS->ViewCount; View++) {
            if (Dec->NAL->IsAnchorPicture == true) {
                for (uint8_t AnchorRef = 0; AnchorRef < Dec->SPS->AnchorRefsCount[0][View]; AnchorRef++) {
                    Dec->SEI->PDIInitDelayAnchor[0][View][AnchorRef] = ReadExpGolomb(BitB, false) + 2;
                }
                for (uint8_t AnchorRef = 0; AnchorRef < Dec->SPS->AnchorRefsCount[1][View]; AnchorRef++) {
                    Dec->SEI->PDIInitDelayAnchor[1][View][AnchorRef] = ReadExpGolomb(BitB, false) + 2;
                }
            } else {
                for (uint8_t NonAnchorRef = 0; NonAnchorRef < Dec->SPS->NonAnchorRefCount[0][View]; NonAnchorRef++) {
                    Dec->SEI->PDIInitDelayNonAnchor[0][View][NonAnchorRef] = ReadExpGolomb(BitB, false) + 2;
                }
                for (uint8_t NonAnchorRef = 0; NonAnchorRef < Dec->SPS->NonAnchorRefCount[1][View]; NonAnchorRef++) {
                    Dec->SEI->PDIInitDelayNonAnchor[1][View][NonAnchorRef] = ReadExpGolomb(BitB, false) + 2;
                }
            }
        }
    }
    
    void ParseSEIMVCScalableNesting(DecodeAVC *Dec, BitBuffer *BitB) { // mvc_scalable_nesting
        Dec->SEI->OperationPointFlag = ReadBits(BitB, 1, true);
        if (Dec->SEI->OperationPointFlag == false) {
            Dec->SEI->AllViewComponentsInAUFlag = ReadBits(BitB, 1, true);
            if (Dec->SEI->AllViewComponentsInAUFlag == false) {
                Dec->SEI->NumViewComponents = ReadExpGolomb(BitB, false) + 1;
                for (uint8_t ViewComponent = 0; ViewComponent < Dec->SEI->NumViewComponents; ViewComponent++) {
                    Dec->SEI->SEIViewID[ViewComponent] = ReadBits(BitB, 10, true);
                }
            }
        } else {
            Dec->SEI->NumViewComponentsOp = ReadExpGolomb(BitB, false) + 1;
            for (uint8_t ViewComponent = 0; ViewComponent < Dec->SEI->NumViewComponentsOp; ViewComponent++) {
                Dec->SEI->SEIOpViewID[ViewComponent] = ReadBits(BitB, 10, true);
            }
            Dec->SEI->SEIOpTemporalID = ReadBits(BitB, 3, true);
        }
        AlignBitBuffer(BitB, 1);
        ParseSEIMessage(Dec, BitB); // sei_message();
    }
    
    void ParseSEIViewScalabilityInfo(DecodeAVC *Dec, BitBuffer *BitB) { // view_scalability_info FIXME: FINISH THIS FUNCTION!!!
        Dec->SEI->NumOperationPoints                         = ReadExpGolomb(BitB, false) + 1;
        for (uint8_t OperationPoint = 0; OperationPoint < Dec->SEI->NumOperationPoints; OperationPoint++) {
            Dec->SEI->OperationPointID[OperationPoint]       = ReadExpGolomb(BitB, false);
            Dec->NAL->PriorityID[OperationPoint]             = ReadBits(BitB, 5, true);
            Dec->NAL->TemporalID[OperationPoint]             = ReadBits(BitB, 3, true);
            Dec->SEI->NumTargetOutputViews[OperationPoint]   = ReadExpGolomb(BitB, false) + 1;
            for (uint8_t OutputView = 0; OutputView < Dec->SEI->NumTargetOutputViews[OperationPoint]; OutputView++) {
                Dec->SPS->ViewID[OperationPoint][OutputView] = ReadExpGolomb(BitB, false);
            }
            Dec->SEI->ProfileLevelInfoPresentFlag[0]         = ReadBits(BitB, 1, true);
            Dec->SEI->BitRateInfoPresent[0]                  = ReadBits(BitB, 1, true);
            Dec->SEI->FrameRateInfoPresent[0]                = ReadBits(BitB, 1, true);
            if (Dec->SEI->NumTargetOutputViews[OperationPoint] == false) {
                Dec->SEI->ViewDependencyInfoPresent[OperationPoint] = ReadBits(BitB, 1, true);
            }
            Dec->SEI->ParameterSetsInfoPresent[OperationPoint]        = ReadBits(BitB, 1, true);
            Dec->SEI->BitstreamRestrictionInfoPresent[OperationPoint] = ReadBits(BitB, 1, true);
            if (Dec->SEI->ProfileLevelInfoPresentFlag[0] == true) {
                Dec->SEI->OpProfileLevelIDC[OperationPoint]           = ReadBits(BitB, 24, true);
            }
            if (Dec->SEI->BitstreamRestrictionInfoPresent[OperationPoint] == true) {
                Dec->SEI->AvgBitRate[OperationPoint]                  = ReadBits(BitB, 16, true);
                Dec->SEI->MaxBitRate[OperationPoint]                  = ReadBits(BitB, 16, true);
                Dec->SEI->MaxBitRateWindow[OperationPoint]            = ReadBits(BitB, 16, true);
            }
            if (Dec->SEI->FrameRateInfoPresent[0] == true) {
                Dec->SEI->ConstantFrameRateIDC[OperationPoint]        = ReadBits(BitB, 2, true);
                Dec->SEI->AvgFrameRate[OperationPoint]                = ReadBits(BitB, 16, true);
            }
            if (Dec->SEI->ViewDependencyInfoPresent[OperationPoint] == true) {
                Dec->SEI->NumDirectlyDependentViews[OperationPoint]   = ReadExpGolomb(BitB, false);
                for (uint8_t DirectDependentView = 0; DirectDependentView < Dec->SEI->NumDirectlyDependentViews[OperationPoint]; DirectDependentView++) {
                    Dec->SEI->DirectlyDependentViewID[OperationPoint][DirectDependentView] = ReadExpGolomb(BitB, false);
                }
            } else {
                Dec->SEI->ViewDependencyInfoSrcOpID[OperationPoint]   = ReadExpGolomb(BitB, false);
            }
            if (Dec->SEI->ParameterSetsInfoPresent[OperationPoint] == true) {
                Dec->SEI->NumSequenceParameterSets[OperationPoint] = ReadExpGolomb(BitB, false);
                for (uint8_t SPS = 0; SPS < Dec->SEI->NumSequenceParameterSets[OperationPoint]; SPS++) {
                    Dec->SEI->SPSIDDelta[OperationPoint][SPS] = ReadExpGolomb(BitB, false);
                }
                Dec->SEI->NumSubsetSPS[OperationPoint] = ReadExpGolomb(BitB, false);
                for (uint8_t SubsetSPS = 0; SubsetSPS < Dec->SEI->NumSubsetSPS[OperationPoint]; SubsetSPS++) {
                    Dec->SEI->SubsetSPSIDDelta[OperationPoint][SubsetSPS] = ReadExpGolomb(BitB, false);
                }
                Dec->SEI->NumPicParameterSets[OperationPoint] = ReadExpGolomb(BitB, false) + 1;
                for (uint8_t PPS = 0; PPS < Dec->SEI->NumPicParameterSets[OperationPoint]; PPS++) {
                    Dec->SEI->PicParameterSetIDDelta[OperationPoint][PPS] = ReadExpGolomb(BitB, false);
                }
            } else {
                Dec->SEI->PPSInfoSrcLayerIDDelta[OperationPoint] = ReadExpGolomb(BitB, false);
            }
            if (Dec->SEI->BitstreamRestrictionInfoPresent[OperationPoint] == true) {
                Dec->VUI->MotionVectorsOverPicBoundaries[OperationPoint]  = ReadBits(BitB, 1, true);
                Dec->VUI->MaxBytesPerPicDenom[OperationPoint]             = ReadExpGolomb(BitB, false);
                Dec->VUI->MaxBitsPerMarcoBlockDenom[OperationPoint]       = ReadExpGolomb(BitB, false);
                Dec->VUI->MaxMotionVectorLength[OperationPoint]           = ReadExpGolomb(BitB, false);
                Dec->VUI->MaxMotionVectorHeight[OperationPoint]           = ReadExpGolomb(BitB, false);
                Dec->VUI->MaxReorderFrames[OperationPoint]                = ReadExpGolomb(BitB, false);
                Dec->VUI->MaxFrameBuffer[OperationPoint]                  = ReadExpGolomb(BitB, false);
            }
            if (Dec->SEI->LayerConversionFlag[OperationPoint] == true) {
                Dec->SEI->ConversionTypeIDC[OperationPoint] = ReadExpGolomb(BitB, false);
                for (uint8_t J = 0; J < 2; J++) {
                    Dec->SEI->RewritingInfoFlag[OperationPoint][J] = ReadBits(BitB, 1, true);
                    for (uint8_t RewriteInfo = 0; RewriteInfo < Dec->SEI->RewritingInfoFlag[OperationPoint][J]; RewriteInfo++) {
                        Dec->SEI->RewritingProfileLevelIDC[OperationPoint][J] = ReadBits(BitB, 24, true);
                        Dec->SEI->RewritingAverageBitrate[OperationPoint][J]  = ReadBits(BitB, 16, true);
                        Dec->SEI->RewritingMaxBitrate[OperationPoint][J]      = ReadBits(BitB, 16, true);
                    }
                }
            }
        }
        if (Dec->SEI->PriorityLayerInfoPresent == true) {
            Dec->SEI->NumDependencyLayersForPriorityLayer                             = ReadExpGolomb(BitB, false) + 1;
            for (uint8_t DependencyLayer = 0; DependencyLayer < Dec->SEI->NumDependencyLayersForPriorityLayer; DependencyLayer++) {
                Dec->SEI->PriorityDependencyID[DependencyLayer]                       = ReadBits(BitB, 3, true);
                Dec->SEI->NumPriorityLayers[DependencyLayer]                          = ReadExpGolomb(BitB, false) + 1;
                for (uint8_t PriorityLayer = 0; PriorityLayer < Dec->SEI->NumPriorityLayers[DependencyLayer]; PriorityLayer++) {
                    Dec->SEI->PriorityLayerID[DependencyLayer][PriorityLayer]         = ReadExpGolomb(BitB, false);
                    Dec->SEI->PriorityLevelIDC[DependencyLayer][PriorityLayer]        = ReadBits(BitB, 24, true);
                    Dec->SEI->PriorityLevelAvgBitrate[DependencyLayer][PriorityLayer] = ReadBits(BitB, 16, true);
                    Dec->SEI->PriorityLevelMaxBitrate[DependencyLayer][PriorityLayer] = ReadBits(BitB, 16, true);
                }
            }
        }
        if (Dec->SEI->PriorityIDSettingFlag == true) {
            Dec->SEI->PriorityIDURIIndex = 0;
            Dec->SEI->PriorityIDSettingURI = ReadBits(BitB, 8, true);
            for (uint8_t StringByte = Dec->SEI->PriorityIDSettingURI; StringByte > 0; StringByte--) {
                Dec->SEI->PriorityIDSettingURIString[StringByte] = ReadBits(BitB, 8, true);
            }
        }
    }
    
    void ParseSEIMVCSceneInfo(DecodeAVC *Dec, BitBuffer *BitB) { // multiview_scene_info
        Dec->SEI->MaxDisparity = ReadExpGolomb(BitB, false);
    }
    
    void ParseSEIMVCAcquisitionInfo(DecodeAVC *Dec, BitBuffer *BitB) { // multiview_acquisition_info
        Dec->SPS->ViewCount            = ReadExpGolomb(BitB, false) + 1;
        Dec->SEI->IntrinsicParamFlag   = ReadBits(BitB, 1, true);
        Dec->SEI->ExtrinsicParamFlag   = ReadBits(BitB, 1, true);
        if (Dec->SEI->IntrinsicParamFlag == true) {
            Dec->SEI->IntrinsicParamsEqual = ReadBits(BitB, 1, true);
            Dec->SEI->PrecFocalLength      = ReadExpGolomb(BitB, false);
            Dec->SEI->PrecPrincipalPoint   = ReadExpGolomb(BitB, false);
            Dec->SEI->PrecSkewFactor       = ReadExpGolomb(BitB, false);
            if (Dec->SEI->IntrinsicParamsEqual == true) {
                Dec->SEI->NumParamSets     = 1;
            } else {
                Dec->SEI->NumParamSets     = Dec->SPS->ViewCount;
            }
            for (uint8_t ParamSet = 0; ParamSet < Dec->SEI->NumParamSets; ParamSet++) { // FIXME: ReadBits(BitB, 0, true)
                Dec->SEI->SignFocalLength[0][ParamSet]        = ReadBits(BitB, 1, true);
                Dec->SEI->ExponentFocalLength[0][ParamSet]    = ReadBits(BitB, 6, true);
                if (Dec->SEI->ExponentFocalLength[0][ParamSet] == 0) {
                    Dec->SEI->MantissaFocalLength[0][ParamSet] = ReadBits(BitB, Max(0, Dec->SEI->PrecFocalLength - 30), true);
                } else {
                    Dec->SEI->MantissaFocalLength[0][ParamSet] = ReadBits(BitB, Max(0, Dec->SEI->ExponentFocalLength[0][ParamSet] + Dec->SEI->PrecFocalLength - 31), true);
                }
                Dec->SEI->SignFocalLength[1][ParamSet]        = ReadBits(BitB, 1, true);
                Dec->SEI->ExponentFocalLength[1][ParamSet]    = ReadBits(BitB, 6, true);
                if (Dec->SEI->ExponentFocalLength[1][ParamSet] == 0) {
                    Dec->SEI->MantissaFocalLength[1][ParamSet] = ReadBits(BitB, Max(0, Dec->SEI->PrecFocalLength - 30), true);
                } else {
                    Dec->SEI->MantissaFocalLength[1][ParamSet] = ReadBits(BitB, Max(0, Dec->SEI->ExponentFocalLength[1][ParamSet] + Dec->SEI->PrecFocalLength - 31), true);
                }
                Dec->SEI->SignPrincipalPoint[0][ParamSet]     = ReadBits(BitB, 1, true);
                Dec->SEI->ExponentPrincipalPoint[0][ParamSet] = ReadBits(BitB, 6, true);
                if (Dec->SEI->ExponentFocalLength[1][ParamSet] == 0) {
                    Dec->SEI->MantissaPrincipalPoint[0][ParamSet] = ReadBits(BitB, Max(0, Dec->SEI->PrecPrincipalPoint - 30), true);
                } else {
                    Dec->SEI->MantissaPrincipalPoint[0][ParamSet] = ReadBits(BitB, Max(0, Dec->SEI->ExponentPrincipalPoint[0][ParamSet] + Dec->SEI->PrecPrincipalPoint - 31), true);
                }
                Dec->SEI->SignPrincipalPoint[1][ParamSet]     = ReadBits(BitB, 1, true);
                Dec->SEI->ExponentPrincipalPoint[1][ParamSet] = ReadBits(BitB, 6, true);
                if (Dec->SEI->ExponentFocalLength[1][ParamSet] == 0) {
                    Dec->SEI->MantissaPrincipalPoint[1][ParamSet] = ReadBits(BitB, Max(0, Dec->SEI->PrecPrincipalPoint - 30), true);
                } else {
                    Dec->SEI->MantissaPrincipalPoint[1][ParamSet] = ReadBits(BitB, Max(0, Dec->SEI->ExponentPrincipalPoint[1][ParamSet] + Dec->SEI->PrecPrincipalPoint - 31), true);
                }
                Dec->SEI->SignSkewFactor[ParamSet]            = ReadBits(BitB, 1, true);
                Dec->SEI->ExponentSkewFactor[ParamSet]        = ReadBits(BitB, 6, true);
                if (Dec->SEI->ExponentSkewFactor[ParamSet] == 0) {
                    Dec->SEI->MantissaSkewFactor[ParamSet] = ReadBits(BitB, Max(0, Dec->SEI->PrecSkewFactor - 30), true);
                } else {
                    Dec->SEI->MantissaSkewFactor[ParamSet] = ReadBits(BitB, Max(0, Dec->SEI->ExponentSkewFactor[ParamSet] + Dec->SEI->PrecSkewFactor - 31), true);
                }
            }
        }
        if (Dec->SEI->ExtrinsicParamFlag == true) {
            for (uint8_t View = 0; View < Dec->SPS->ViewCount; View++) {
                for (uint8_t Row = 1; Row <= 3; Row++) {
                    for (uint8_t Column = 1; Column <= 3; Column++) {
                        Dec->SEI->SignR[View][Row][Column]     = ReadBits(BitB, 1, true);
                        Dec->SEI->ExponentR[View][Row][Column] = ReadBits(BitB, 6, true);
                        Dec->SEI->MantissaR[View][Row][Column] = ReadBits(BitB, 0, true);
                    }
                    Dec->SEI->SignT[View][Row] = ReadBits(BitB, 1, true);
                    Dec->SEI->ExponentT[View][Row] = ReadBits(BitB, 6, true);
                    Dec->SEI->MantissaT[View][Row] = ReadBits(BitB, 0, true);
                }
            }
        }
    }
    
    void ParseSEINonRequiredViewComponent(DecodeAVC *Dec, BitBuffer *BitB) { // non_required_view_component
        Dec->SEI->NumInfoEntries                               = ReadExpGolomb(BitB, false) + 1;
        for (uint8_t InfoEntry = 0; InfoEntry < Dec->SEI->NumInfoEntries; InfoEntry++) {
            Dec->SEI->ViewOrderIndex[InfoEntry]                = ReadExpGolomb(BitB, false);
            Dec->SEI->NumNonRequiredViewComponents[InfoEntry]  = ReadExpGolomb(BitB, false) + 1;
            for (uint8_t ViewComponent = 0; ViewComponent < Dec->SEI->NumNonRequiredViewComponents[InfoEntry]; ViewComponent++) {
                Dec->SEI->IndexDelta[InfoEntry][ViewComponent] = ReadExpGolomb(BitB, false) + 1;
            }
        }
    }
    
    void ParseSEIViewDependencyChange(DecodeAVC *Dec, BitBuffer *BitB) { // view_dependency_change
        Dec->SPS->SeqParamSetID               = ReadExpGolomb(BitB, false);
        Dec->SEI->AnchorUpdateFlag    = ReadBits(BitB, 1, true);
        Dec->SEI->NonAnchorUpdateFlag = ReadBits(BitB, 1, true);
        if (Dec->SEI->AnchorUpdateFlag == true) {
            for (uint8_t View = 0; View < Dec->SPS->ViewCount; View++) {
                for (uint8_t AnchorRef = 0; AnchorRef < Dec->SPS->AnchorRefsCount[0][View]; AnchorRef++) {
                    Dec->SEI->AnchorRefFlag[0][View][AnchorRef] = ReadBits(BitB, 1, true);
                }
                for (uint8_t AnchorRef = 0; AnchorRef < Dec->SPS->AnchorRefsCount[1][View]; AnchorRef++) {
                    Dec->SEI->AnchorRefFlag[1][View][AnchorRef] = ReadBits(BitB, 1, true);
                }
            }
        }
        if (Dec->SEI->NonAnchorUpdateFlag == true) {
            for (uint8_t View = 0; View < Dec->SPS->ViewCount; View++) {
                for (uint8_t NonAnchorRef = 0; NonAnchorRef < Dec->SPS->NonAnchorRefCount[0][View]; NonAnchorRef++) {
                    Dec->SEI->NonAnchorRefFlag[0][View][NonAnchorRef] = ReadBits(BitB, 1, true);
                }
                for (uint8_t NonAnchorRef = 0; NonAnchorRef < Dec->SPS->NonAnchorRefCount[1][View]; NonAnchorRef++) {
                    Dec->SEI->NonAnchorRefFlag[1][View][NonAnchorRef] = ReadBits(BitB, 1, true);
                }
            }
        }
    }
    
    void ParseSEIOperationPointNotPresent(DecodeAVC *Dec, BitBuffer *BitB) { // operation_point(s)_not_present
        Dec->SEI->NumOperationPoints = ReadExpGolomb(BitB, false) + 1;
        for (uint8_t OperationPoint = 0; OperationPoint < Dec->SEI->NumOperationPoints; OperationPoint++) {
            Dec->SEI->OperationPointNotPresentID[OperationPoint] = ReadExpGolomb(BitB, false);
        }
    }
    
    void ParseSEIBaseViewTemporalHRD(DecodeAVC *Dec, BitBuffer *BitB) { // base_view_temporal_hrd
        Dec->SEI->NumTemporalLayersInBaseView = ReadExpGolomb(BitB, false) + 1;
        for (uint8_t TemporalLayer = 0; TemporalLayer < Dec->SEI->NumTemporalLayersInBaseView; TemporalLayer++) {
            Dec->SEI->SEIMVCTemporalID[TemporalLayer] = ReadBits(BitB, 3, true);
        }
    }
    
    void ParseSEIFramePackingArrangement(DecodeAVC *Dec, BitBuffer *BitB) { // frame_packing_arrangement
        Dec->SEI->FramePackingArrangementID = ReadExpGolomb(BitB, false);
        Dec->SEI->FramePackingArrangementCancelFlag = ReadBits(BitB, 1, true);
        if (Dec->SEI->FramePackingArrangementCancelFlag == false) {
            Dec->SEI->FramePackingArrangementType = ReadBits(BitB, 7, true);
            Dec->SEI->QuincunxSamplingFlag = ReadBits(BitB, 1, true);
            Dec->SEI->ContentIntrepretationType = ReadBits(BitB, 6, true);
            Dec->SEI->SpatialFlippingFlag = ReadBits(BitB, 1, true);
            Dec->SEI->FlippedFlagFrame0 = ReadBits(BitB, 1, true);
            Dec->SEI->FieldViewsFlag = ReadBits(BitB, 1, true);
            Dec->SEI->TheCurrentFrameIsFrame0 = ReadBits(BitB, 1, true);
            Dec->SEI->Frame0IsSelfContainedFlag = ReadBits(BitB, 1, true);
            Dec->SEI->Frame1IsSelfContainedFlag = ReadBits(BitB, 1, true);
            if ((Dec->SEI->QuincunxSamplingFlag == false) && (Dec->SEI->FramePackingArrangementType != 5)) {
                Dec->SEI->Frame0GridPositionX = ReadBits(BitB, 4, true);
                Dec->SEI->Frame0GridPositionY = ReadBits(BitB, 4, true);
                Dec->SEI->Frame1GridPositionX = ReadBits(BitB, 4, true);
                Dec->SEI->Frame1GridPositionY = ReadBits(BitB, 4, true);
            }
            Dec->SEI->FramePackingArrangementReservedByte = ReadBits(BitB, 8, true);
            Dec->SEI->FramePackingArrangementRepetitionPeriod = ReadExpGolomb(BitB, false);
        }
        Dec->SEI->FramePackingArrangementExtensionFlag = ReadBits(BitB, 1, true);
    }
    
    void ParseSEIMVCViewPosition(DecodeAVC *Dec, BitBuffer *BitB) { // multiview_view_position
        Dec->SPS->ViewCount = ReadExpGolomb(BitB, false) + 1;
        for (uint8_t View = 0; View < Dec->SPS->ViewCount; View++) {
            Dec->SEI->ViewPosition[View] = ReadExpGolomb(BitB, false);
        }
        Dec->SEI->MVCViewPositionExtensionFlag = ReadBits(BitB, 1, true);
    }
    
    void ParseSEIDisplayOrientation(DecodeAVC *Dec, BitBuffer *BitB) { // display_orientation
        Dec->SEI->DisplayOrientationCancelFlag = ReadBits(BitB, 1, true);
        if (Dec->SEI->DisplayOrientationCancelFlag == false) {
            Dec->SEI->HorizontalFlip = ReadBits(BitB, 1, true);
            Dec->SEI->VerticalFlip   = ReadBits(BitB, 1, true);
            Dec->SEI->CounterClockwiseRotation = ReadBits(BitB, 16, true);
            Dec->SEI->DisplayOrientationRepetitionPeriod = ReadExpGolomb(BitB, false);
            Dec->SEI->DisplayOrientationExtensionFlag = ReadBits(BitB, 1, true);
        }
    }
    
    void ParseSEIDepthRepresentationInformation(DecodeAVC *Dec, BitBuffer *BitB) { // depth_representation_info
        Dec->SEI->AllViewsEqual                                     = ReadBits(BitB, 1, true);
        if (Dec->SEI->AllViewsEqual == true) {
            Dec->SPS->ViewCount                                     = ReadExpGolomb(BitB, false) + 1;
        } else {
            Dec->SPS->ViewCount                                     = 1;
        }
        Dec->SEI->ZNearFlag                                         = ReadBits(BitB, 1, true);
        Dec->SEI->ZFarFlag                                          = ReadBits(BitB, 1, true);
        if ((Dec->SEI->ZNearFlag == true) || (Dec->SEI->ZFarFlag == true)) {
            Dec->SEI->ZAxisEqualFlag                                = ReadBits(BitB, 1, true);
            if (Dec->SEI->ZAxisEqualFlag == true) {
                Dec->SEI->CommonZAxisReferenceView                  = ReadExpGolomb(BitB, false);
            }
        }
        Dec->SEI->DMinFlag                                          = ReadBits(BitB, 1, true);
        Dec->SEI->DMaxFlag                                          = ReadBits(BitB, 1, true);
        Dec->SEI->DepthRepresentationType                           = ReadExpGolomb(BitB, false);
        for (uint8_t View = 0; View < Dec->SPS->ViewCount; View++) {
            Dec->SEI->DepthInfoViewID[View]                         = ReadExpGolomb(BitB, false);
            if ((Dec->SEI->ZNearFlag == true || Dec->SEI->ZFarFlag == true) && (Dec->SEI->ZAxisEqualFlag == false)) {
                Dec->SEI->ZAxisReferenceView[View]                  = ReadExpGolomb(BitB, false);
            }
            if (Dec->SEI->DMinFlag == true || Dec->SEI->DMaxFlag == true) {
                Dec->SEI->DisparityReferenceView[View]              = ReadExpGolomb(BitB, false);
            }
            if (Dec->SEI->ZNearFlag == true) {
                //depth_representation_sei_element(ZNearSign, ZNearExp, ZNearMantissa, ZNearManLen);
                //ParseSEIDepthRepresentationElement(BitB, Dec->SEI->ZNearSign, Dec->SEI->ZNearExp, Dec->SEI->ZNearMantissa, Dec->SEI->ZNear)
            }
            if (Dec->SEI->ZFarFlag == true) {
                //depth_representation_sei_element(ZFarSign, ZFarExp, ZFarMantissa, ZFarManLen);
            }
            if (Dec->SEI->DMinFlag == true) {
                //depth_representation_sei_element(DMinSign, DMinExp, DMinMantissa, DMinManLen);
            }
            if (Dec->SEI->DMaxFlag == true) {
                //depth_representation_sei_element(DMaxSign, DMaxExp, DMaxMantissa, DMaxManLen);
            }
        }
        if (Dec->SEI->DepthRepresentationType == 3) {
            Dec->SEI->DepthNonlinearRepresentation                  = ReadExpGolomb(BitB, false) + 1;
            for (uint8_t Index= 0; Index < Dec->SEI->DepthNonlinearRepresentation; Index++) {
                Dec->SEI->DepthNonlinearRepresentationModel[Index]  = ReadExpGolomb(BitB, false);
            }
        }
    }
    
    void ParseSEI3DReferenceDisplayInfo(DecodeAVC *Dec, BitBuffer *BitB) { // three_dimensional_reference_displays_info
        Dec->SEI->TruncationErrorExponent                           = ReadExpGolomb(BitB, false);
        Dec->SEI->TruncatedWidthExponent                            = ReadExpGolomb(BitB, false);
        Dec->SEI->ReferenceViewingDistanceFlag                      = ReadBits(BitB, 1, true);
        
        if (Dec->SEI->ReferenceViewingDistanceFlag == true) {
            Dec->SEI->TruncatedReferenveViewingDistanceExponent     = ReadExpGolomb(BitB, false);
        }
        Dec->SEI->NumReferenceDisplays                              = ReadExpGolomb(BitB, false) + 1;
        for (uint8_t Display= 0; Display < Dec->SEI->NumReferenceDisplays; Display++) {
            Dec->SEI->ReferenceBaselineExponent[Display]            = ReadExpGolomb(BitB, false);
            Dec->SEI->ReferenceBaselineMantissa[Display]            = ReadExpGolomb(BitB, false);
            Dec->SEI->ReferenceDisplayWidthExponent[Display]        = ReadExpGolomb(BitB, false);
            Dec->SEI->ReferenceDisplayWidthMantissa[Display]        = ReadExpGolomb(BitB, false);
            if (Dec->SEI->ReferenceViewingDistanceFlag == true) {
                Dec->SEI->ReferenceViewingDistanceExponent[Display] = ReadExpGolomb(BitB, false);
                Dec->SEI->ReferenceViewingDistanceMantissa[Display] = ReadExpGolomb(BitB, false);
            }
            Dec->SEI->ShiftPresentFlag[Display]                     = ReadBits(BitB, 1, true);
            if (Dec->SEI->ShiftPresentFlag[Display] == true) {
                Dec->SEI->SampleShift[Display]                      = ReadExpGolomb(BitB, false) - 512;
            }
        }
        Dec->SEI->ReferenceDisplays3DFlag                           = ReadBits(BitB, 1, true);
    }
    
    void ParseSEIDepthTiming(DecodeAVC *Dec, BitBuffer *BitB) { // depth_timing
        Dec->SEI->PerViewDepthTimingFlag                              = ReadBits(BitB, 1, true);
        
        if (Dec->SEI->PerViewDepthTimingFlag == true) {
            for (uint8_t View = 0; View < Dec->DPS->NumDepthViews; View++) {
                Dec->SEI->OffsetLength[View]                          = ReadBits(BitB, 5, true) + 1;
                Dec->SEI->DepthDisplayDelayOffsetFP[View]             = ReadBits(BitB, Dec->SEI->OffsetLength[View], true);
                Dec->SEI->DepthDisplayDelayOffsetDP[View]             = ReadBits(BitB, Dec->SEI->OffsetLength[View], true);
            }
        }
    }
    
    void ParseSEIDepthGridPosition(DecodeAVC *Dec, BitBuffer *BitB) { // depth_grid_position()
        Dec->SEI->DepthGridPositionXFP                                = ReadBits(BitB, 20, true);
        Dec->SEI->DepthGridPositionXDP                                = ReadBits(BitB, 4, true);
        Dec->SEI->DepthGridPositionXSignFlag                          = ReadBits(BitB, 1, true);
        Dec->SEI->DepthGridPositionYFP                                = ReadBits(BitB, 20, true);
        Dec->SEI->DepthGridPositionYDP                                = ReadBits(BitB, 4, true);
        Dec->SEI->DepthGridPositionYSignFlag                          = ReadBits(BitB, 1, true);
    }
    
    void ParseSEIDepthSamplingInfo(DecodeAVC *Dec, BitBuffer *BitB) { // depth_sampling_info
        Dec->SEI->DepthSampleWidthMul                                 = ReadBits(BitB, 16, true);
        Dec->SEI->DepthSampleWidthDP                                  = ReadBits(BitB, 4, true);
        Dec->SEI->DepthSampleHeightMul                                = ReadBits(BitB, 16, true);
        Dec->SEI->DepthSampleHeightDP                                 = ReadBits(BitB, 4, true);
        Dec->SEI->PerViewDepthTimingFlag                              = ReadBits(BitB, 1, true);
        if (Dec->SEI->PerViewDepthTimingFlag == true) {
            Dec->SEI->NumDepthGridViews                               = ReadExpGolomb(BitB, false) + 1;
            for (uint8_t DepthGrid = 0; DepthGrid < Dec->SEI->NumDepthGridViews; DepthGrid++) {
                Dec->SEI->DepthInfoViewID[DepthGrid]                  = ReadExpGolomb(BitB, false);
                ParseSEIDepthGridPosition(Dec, BitB);
            }
        } else {
            ParseSEIDepthGridPosition(Dec, BitB);
        }
    }
    
    void ParseSEIConstrainedDepthParameterSetID(DecodeAVC *Dec, BitBuffer *BitB) { // constrained_depth_parameter_set_identifier
        Dec->SEI->MaxDPSID                                            = ReadExpGolomb(BitB, false);
        Dec->SEI->MaxDPSIDDiff                                        = ReadExpGolomb(BitB, false);
    }
    
    void ParseSEIMeteringDisplayColorVolume(DecodeAVC *Dec, BitBuffer *BitB) { // mastering_display_color_volume
        for (uint8_t Color = 0; Color < 3; Color++) {
            Dec->SEI->DisplayPrimariesX[Color]                        = ReadBits(BitB, 16, true);
            Dec->SEI->DisplayPrimariesY[Color]                        = ReadBits(BitB, 16, true);
        }
        Dec->SEI->WhitePointX                                         = ReadBits(BitB, 16, true);
        Dec->SEI->WhitePointY                                         = ReadBits(BitB, 16, true);
        Dec->SEI->MaxDisplayMasteringLuminance                        = ReadBits(BitB, 32, true);
        Dec->SEI->MinDisplayMasteringLuminance                        = ReadBits(BitB, 32, true);
    }
    
    void ParseSEIMVCDScalableNesting(DecodeAVC *Dec, BitBuffer *BitB) { // mvcd_scalable_nesting
        Dec->SEI->OperationPointFlag                                  = ReadBits(BitB, 1, true);
        if (Dec->SEI->OperationPointFlag == false) {
            Dec->SEI->AllViewComponentsInAUFlag                       = ReadBits(BitB, 1, true);
            if (Dec->SEI->AllViewComponentsInAUFlag == false) {
                Dec->SEI->NumViewComponents                           = ReadExpGolomb(BitB, false) + 1;
                for (uint8_t ViewComponent = 0; ViewComponent < Dec->SEI->NumViewComponents; ViewComponent++) {
                    Dec->SEI->SEIViewID[ViewComponent]                = ReadBits(BitB, 10, true);
                    Dec->SEI->SEIViewApplicabilityFlag[ViewComponent] = ReadBits(BitB, 1, true);
                }
            }
        } else {
            Dec->SEI->SEIOpTextureOnlyFlag                            = ReadBits(BitB, 1, true);
            Dec->SEI->NumViewComponents                               = ReadExpGolomb(BitB, false) + 1;
            for (uint8_t ViewComponent = 0; ViewComponent < Dec->SEI->NumViewComponents; ViewComponent++) {
                Dec->SEI->SEIOpViewID[ViewComponent]                  = ReadBits(BitB, 10, true);
                if (Dec->SEI->SEIOpTextureOnlyFlag == false) {
                    Dec->SEI->SEIOpDepthFlag[ViewComponent]           = ReadBits(BitB, 1, true);
                    Dec->SEI->SEIOpTextureFlag[ViewComponent]         = ReadBits(BitB, 1, true);
                }
            }
            Dec->SEI->SEIOpTemporalID                                 = ReadBits(BitB, 1, true);
        }
        AlignBitBuffer(BitB, 1);
        ParseSEIMessage(Dec, BitB); // sei_message();
    }
    
    void ParseSEIDepthRepresentationElement(BitBuffer *BitB, uint8_t OutSign, uint8_t OutExp, uint8_t OutMantissa, uint8_t OutManLen) { // depth_representation_sei_element
        bool     DASignFlag                                           = ReadBits(BitB, 1, true); // da_sign_flag
        uint8_t  DAExponent                                           = ReadBits(BitB, 7, true); // da_exponent
        uint8_t  DAMatissaSize                                        = ReadBits(BitB, 5, true) + 1;
        uint64_t DAMatissa                                            = ReadBits(BitB, DAMatissaSize, true);
    }
    
    void SkipSEIReservedMessage(BitBuffer *BitB, size_t SEISize) { // reserved_sei_message
        SkipBits(BitB, Bytes2Bits(SEISize));
    }
    
    void ParseSEIMessage(DecodeAVC *Dec, BitBuffer *BitB) { // sei_message
        while (PeekBits(BitB, 8, true) == 0xFF) {
            SkipBits(BitB, 8);
            Dec->SEI->SEIType += 255;
        }
        Dec->SEI->SEIType += ReadBits(BitB, 8, true); // last_payload_type_byte, 5
        
        while (PeekBits(BitB, 8, true) == 0xFF) {
            SkipBits(BitB, 8);
            Dec->SEI->SEISize    += 255;
        }
        Dec->SEI->SEISize += ReadBits(BitB, 8, true); // last_payload_size_byte, 692, emulation prevention bytes not included, but these fields are.
        
        switch (Dec->SEI->SEIType) { // sei_payload
            case SEI_BufferingPeriod:                         // 0
                ParseSEIBufferingPeriod(Dec, BitB);
                break;
            case SEI_PictureTiming:                           // 1
                ParseSEIPictureTiming(Dec, BitB);
                break;
            case SEI_PanScan:                                 // 2
                ParseSEIPanScan(Dec, BitB);
                break;
            case SEI_Filler:                                  // 3
                ParseSEIFiller(Dec, BitB);
                break;
            case SEI_RegisteredUserData:                      // 4
                ParseSEIRegisteredUserData(Dec, BitB);
                break;
            case SEI_UnregisteredUserData:                    // 5
                ParseSEIUnregisteredUserData(Dec, BitB);
                break;
            case SEI_RecoveryPoint:                           // 6
                ParseSEIRecoveryPoint(Dec, BitB);
                break;
            case SEI_RepetitiveReferencePicture:              // 7
                ParseSEIRepetitiveReferencePicture(Dec, BitB);
                break;
            case SEI_SparePicture:                            // 8
                ParseSEISparePicture(Dec, BitB);
                break;
            case SEI_SceneInformation:                        // 9
                ParseSEISceneInfo(Dec, BitB);
                break;
            case SEI_SubSequenceInformation:                  // 10
                ParseSEISubSequenceInfo(Dec, BitB);
                break;
            case SEI_SubSequenceLayerProperties:              // 11
                ParseSEISubSequenceLayerProperties(Dec, BitB);
                break;
            case SEI_SubSequenceProperties:                   // 12
                ParseSEISubSequenceProperties(Dec, BitB);
                break;
            case SEI_FullFrameFreeze:                         // 13
                ParseSEIFullFrameFreeze(Dec, BitB);
                break;
            case SEI_FullFrameFreezeRelease:                  // 14
                ParseSEIFullFrameFreezeRelease(Dec, BitB);
                break;
            case SEI_FullFrameSnapshot:                       // 15
                ParseSEIFullFrameSnapshot(Dec, BitB);
                break;
            case SEI_ProgressiveRefinementSegmentStart:       // 16
                ParseSEIProgressiveRefinementSegmentStart(Dec, BitB);
                break;
            case SEI_ProgressiveRefinementSegmentEnd:         // 17
                ParseSEIProgressiveRefinementSegmentEnd(Dec, BitB);
                break;
            case SEI_MotionConstrainedSliceGroup:             // 18
                ParseSEIMotionConstrainedSliceGroupSet(Dec, BitB);
                break;
            case SEI_FilmGrainCharacteristics:                // 19
                ParseSEIFilmGrainCharacteristics(Dec, BitB);
                break;
            case SEI_DeblockingFilterDisplayPreferences:      // 20
                ParseSEIDeblockingFilterDisplayPreference(Dec, BitB);
                break;
            case SEI_StereoVideoInformation:                  // 21
                ParseSEIStereoVideoInfo(Dec, BitB);
                break;
            case SEI_PostFilterHint:                          // 22
                ParseSEIPostFilterHint(Dec, BitB);
                break;
            case SEI_ToneMappingInformation:                  // 23
                ParseSEIToneMappingInfo(Dec, BitB);
                break;
            case SEI_ScalabilityInformation:                  // 24
                ParseSEIScalabilityInfo(Dec, BitB);
                break;
            case SEI_SubPictureScalableLayer:                 // 25
                ParseSEISubPictureScalableLayer(Dec, BitB);
                break;
            case SEI_NonRequiredLayerRep:                     // 26
                ParseSEINonRequiredLayerRep(Dec, BitB);
                break;
            case SEI_PriorityLayerInformation:                // 27
                ParseSEIPriorityLayerInfo(Dec, BitB);
                break;
            case SEI_LayersNotPresent:                        // 28
                ParseSEILayersNotPresent(Dec, BitB);
                break;
            case SEI_LayerDependencyChange:                   // 29
                ParseSEILayerDependencyChange(Dec, BitB);
                break;
            case SEI_ScalableNesting:                         // 30
                ParseSEIScalableNesting(Dec, BitB);
                break;
            case SEI_BaseLayerTemporalHRD:                    // 31
                ParseSEIBaseLayerTemporalHRD(Dec, BitB);
                break;
            case SEI_QualityLayerIntegrityCheck:              // 32
                ParseSEIQualityLayerIntegrityCheck(Dec, BitB);
                break;
            case SEI_RedundantPictureProperty:                // 33
                ParseSEIRedundantPicProperty(Dec, BitB);
                break;
            case SEI_TemporalL0DependencyRepresentationIndex: // 34
                ParseSEITemporalDependencyRepresentationIndex(Dec, BitB);
                break;
            case SEI_TemporalLevelSwitchingPoint:             // 35
                ParseSEITemporalLevelSwitchingPoint(Dec, BitB);
                break;
            case SEI_ParallelDecodingInformation:             // 36
                ParseSEIParallelDecodingInfo(Dec, BitB);
                break;
            case SEI_MVCScalableNesting:                      // 37
                ParseSEIMVCScalableNesting(Dec, BitB);
                break;
            case SEI_ViewScalabilityInformation:              // 38
                ParseSEIViewScalabilityInfo(Dec, BitB);
                break;
            case SEI_MVCSceneInformation:                     // 39
                ParseSEIMVCSceneInfo(Dec, BitB);
                break;
            case SEI_MVCAquisitionInformation:                // 40
                ParseSEIMVCAcquisitionInfo(Dec, BitB);
                break;
            case SEI_NonRequiredViewComponent:                // 41
                ParseSEINonRequiredViewComponent(Dec, BitB);
                break;
            case SEI_ViewDependencyChange:                    // 42
                ParseSEIViewDependencyChange(Dec, BitB);
                break;
            case SEI_OperationPointsNotPresent:               // 43
                ParseSEIOperationPointNotPresent(Dec, BitB);
                break;
            case SEI_BaseViewTemporalHRD:                     // 44
                ParseSEIBaseViewTemporalHRD(Dec, BitB);
                break;
            case SEI_FramePackingArrangement:                 // 45
                ParseSEIFramePackingArrangement(Dec, BitB);
                break;
            case SEI_MVCViewPosition:                         // 46
                ParseSEIMVCViewPosition(Dec, BitB);
                break;
            case SEI_DisplayOrientation:                      // 47
                ParseSEIDisplayOrientation(Dec, BitB);
                break;
            case SEI_MVCDScalableNesting:                     // 48
                ParseSEIMVCDScalableNesting(Dec, BitB);
                break;
            case SEI_MVCDViewScalabilityInformation:          // 49
                ParseSEIViewScalabilityInfo(Dec, BitB);
                break;
            case SEI_DepthRepresentationInformation:          // 50
                ParseSEIDepthRepresentationInformation(Dec, BitB);
                break;
            case SEI_3DReferenceDisplaysInformation:          // 51
                ParseSEI3DReferenceDisplayInfo(Dec, BitB);
                break;
            case SEI_DepthTiming:                             // 52
                ParseSEIDepthTiming(Dec, BitB);
                break;
            case SEI_DepthSamplingInformation:                // 53
                ParseSEIDepthSamplingInfo(Dec, BitB);
                break;
            case SEI_MVCConstrainedDPSIdentifier:             // 54
                ParseSEIConstrainedDepthParameterSetID(Dec, BitB);
                break;
            case SEI_MasteringDisplayColorVolume:             // 137
                ParseSEIMeteringDisplayColorVolume(Dec, BitB);
                break;
            default:
                Log(LOG_ERR, "libModernAVC", "ParseSEIMessage", "Unrecognized SEIType: %d", Dec->SEI->SEIType);
                SkipBits(BitB, Dec->SEI->SEISize);
                break;
        }
    }
    
#ifdef __cplusplus
}
#endif
