#include "../../../Dependencies/BitIO/libBitIO/include/BitIO.h"

#include "../../include/ModernAVCTypes.h"
#include "../../include/Common.h"
#include "../../include/Decode/DecodeAVC.h"
#include "../../include/Decode/DecodeMacroBlock.h"
#include "../../include/Decode/DecodeSlice.h"
#include "../../include/Decode/ParseNAL.h"
#include "../../include/ModernAVCTables.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    /* Sequence Parameter Set */
    void ParseSequenceParameterSetData(AVCFile *AVC, BitBuffer *BitB) { // seq_parameter_set_data
        AVC->SPS->ProfileIDC                           = ReadBits(BitB, 8, true); // 100
        AVC->SPS->ConstraintFlag0                      = ReadBits(BitB, 1, true); // 0
        AVC->SPS->ConstraintFlag1                      = ReadBits(BitB, 1, true); // 0
        AVC->SPS->ConstraintFlag2                      = ReadBits(BitB, 1, true); // 0
        AVC->SPS->ConstraintFlag3                      = ReadBits(BitB, 1, true); // 0
        AVC->SPS->ConstraintFlag4                      = ReadBits(BitB, 1, true); // 0
        AVC->SPS->ConstraintFlag5                      = ReadBits(BitB, 1, true); // 0
        SkipBits(BitB, 2); // Zero bits.
        AVC->SPS->LevelIDC[0]                          = ReadBits(BitB, 8, true); // 51
        AVC->SPS->SeqParamSetID                        = ReadExpGolomb(BitB, false); // 1
        
        if ((AVC->SPS->ProfileIDC == 44)  ||
            (AVC->SPS->ProfileIDC == 83)  ||
            (AVC->SPS->ProfileIDC == 86)  ||
            (AVC->SPS->ProfileIDC == 100) ||
            (AVC->SPS->ProfileIDC == 110) ||
            (AVC->SPS->ProfileIDC == 118) ||
            (AVC->SPS->ProfileIDC == 122) ||
            (AVC->SPS->ProfileIDC == 128) ||
            (AVC->SPS->ProfileIDC == 134) ||
            (AVC->SPS->ProfileIDC == 138) ||
            (AVC->SPS->ProfileIDC == 139) ||
            (AVC->SPS->ProfileIDC == 244)) {
            AVC->SPS->ChromaFormatIDC                  = ReadExpGolomb(BitB, false); // 1 aka 420
            if (AVC->SPS->ChromaFormatIDC == Chroma444) {
                AVC->SPS->SeperateColorPlane           = ReadBits(BitB, 1, true);
            }
            AVC->SPS->LumaBitDepthMinus8               = ReadExpGolomb(BitB, false); // 8
            AVC->SPS->ChromaBitDepthMinus8             = ReadExpGolomb(BitB, false);
            AVC->SPS->QPPrimeBypassFlag                = ReadBits(BitB, 1, true); // qpprime_y_zero_transform_bypass_flag
            AVC->SPS->ScalingMatrixFlag                = ReadBits(BitB, 1, true);
            
            if (AVC->SPS->ScalingMatrixFlag == true) {
                for (uint8_t i = 0; i < ((AVC->SPS->ChromaFormatIDC != Chroma444) ? 8 : 12); i++) {
                    AVC->SPS->ScalingListFlag[i]       = ReadBits(BitB, 1, true);
                    if (AVC->SPS->ScalingListFlag[i] == true) {
                        if (i < 6) {
                            ScalingList(AVC, BitB, ScalingList4x4[i], 16, UseDefaultScalingMatrix4x4Flag[i]);
                        } else {
                            ScalingList(BitB, ScalingList8x8[i - 6], 64, UseDefaultScalingMatrix8x8Flag[i - 6]);
                        }
                    }
                }
            }
        }
        AVC->SPS->MaxFrameNumMinus4                    = ReadExpGolomb(BitB, false) + 4; // 3
        AVC->SPS->PicOrderCount                        = ReadExpGolomb(BitB, false);
        if (AVC->SPS->PicOrderCount == 0) {
            AVC->SPS->MaxPicOrder                      = ReadExpGolomb(BitB, false) + 4;
        } else {
            AVC->SPS->DeltaPicOrder                    = ReadBits(BitB, 1, true);
            AVC->SPS->OffsetNonRefPic                  = ReadExpGolomb(BitB, true);
            AVC->SPS->OffsetTop2Bottom                 = ReadExpGolomb(BitB, true);
            AVC->SPS->RefFramesInPicOrder              = ReadExpGolomb(BitB, false);
            for (uint8_t i                             = 0; i < AVC->SPS->RefFramesInPicOrder; i++) {
                AVC->SPS->RefFrameOffset[i]            = ReadExpGolomb(BitB, true);
            }
            
        }
        AVC->SPS->MaxRefFrames                         = ReadExpGolomb(BitB, false);
        AVC->SPS->GapsInFrameNumAllowed                = ReadBits(BitB, 1, true);
        AVC->SPS->PicWidthInMacroBlocksMinus1          = ReadExpGolomb(BitB, false);
        AVC->SPS->PicHeightInMapUnitsMinus1            = ReadExpGolomb(BitB, false);
        AVC->SPS->OnlyMacroBlocksInFrame               = ReadBits(BitB, 1, true);
        if (AVC->SPS->OnlyMacroBlocksInFrame == false) {
            AVC->SPS->AdaptiveFrameFlag                = ReadBits(BitB, 1, true);
        }
        AVC->SPS->Inference8x8                         = ReadBits(BitB, 1, true);
        AVC->SPS->FrameCroppingFlag                    = ReadBits(BitB, 1, true);
        if (AVC->SPS->FrameCroppingFlag == true) {
            AVC->SPS->FrameCropLeft                    = ReadExpGolomb(BitB, false);
            AVC->SPS->FrameCropRight                   = ReadExpGolomb(BitB, false);
            AVC->SPS->FrameCropTop                     = ReadExpGolomb(BitB, false);
            AVC->SPS->FrameCropBottom                  = ReadExpGolomb(BitB, false);
        }
        AVC->SPS->VUIPresent                           = ReadBits(BitB, 1, true);
        if (AVC->SPS->VUIPresent == true) {
            ParseVideoUsabilityInformation(AVC, BitB);
        }
    }
    
    void ParseNALSequenceParameterSetExtended(AVCFile *AVC, BitBuffer *BitB) { // seq_parameter_set_extension_rbsp?
        AVC->SPS->SeqParamSetID                        = ReadExpGolomb(BitB, false);
        AVC->SPS->AuxiliaryFormatID                    = ReadExpGolomb(BitB, false);
        if (AVC->SPS->AuxiliaryFormatID != 0) {
            AVC->SPS->AuxiliaryBitDepth                = ReadExpGolomb(BitB, false) + 8;
            AVC->SPS->AlphaIncrFlag                    = ReadBits(BitB, 1, true);
            AVC->SPS->AlphaOpaqueValue                 = ReadBits(BitB, AVC->SPS->AuxiliaryBitDepth + 9, true);
            AVC->SPS->AlphaTransparentValue            = ReadBits(BitB, AVC->SPS->AuxiliaryBitDepth + 9, true);
        }
        AVC->SPS->AdditionalExtensionFlag              = ReadBits(BitB, 1, true);
        AlignInput(BitB, 1); // rbsp_trailing_bits
    }
    
    void ParseNALSubsetSPS(AVCFile *AVC, BitBuffer *BitB) { // subset_seq_parameter_set_rbsp
        ReadSequenceParameterSetData(AVC, BitB);
        if ((AVC->SPS->ProfileIDC == Scalable_Constrained_Baseline_Profile) || (AVC->SPS->ProfileIDC == Scalable_High_Intra_Profile)) { // Scalable Video mode
            ParseNALSequenceParameterSetSVC(AVC, BitB);
            if (AVC->SPS->VUIPresent == true) {
                ParseSVCVUIExtension(AVC, BitB);
            }
        } else if ((AVC->SPS->ProfileIDC == MultiView_High_Profile) || (AVC->SPS->ProfileIDC == Stereo_High_Profile) || (AVC->SPS->ProfileIDC == 134)) {
            // Multi View Coding
            SkipBits(BitB, 1);
            ParseSPSMultiViewCodingExtension(AVC, BitB); // seq_parameter_set_mvc_extension
            AVC->SPS->MVCVUIParamsPresent              = ReadBits(BitB, 1, true);
            if (AVC->SPS->MVCVUIParamsPresent == true) {
                ParseMVCVUI(AVC, BitB);
            }
        } else if (AVC->SPS->ProfileIDC == MultiView_Depth_High_Profile) {
            // MVCD
            SkipBits(BitB, 1);
            ParseSPSMVCDExtension(AVC, BitB);
        } else if (AVC->SPS->ProfileIDC == 139) {
            // MVCD && 3DAVC
            SkipBits(BitB, 1);
            ParseSPSMVCDExtension(AVC, BitB);
            ParseSPS3DAVCExtension(AVC, BitB);
        }
        AVC->SPS->AdditionalExtension2                 = ReadBits(BitB, 1, true);
        if (AVC->SPS->AdditionalExtension2 == true) {
            while (more_rbsp_data()) {
                AVC->SPS->AdditionalExtension2DataFlag = ReadBits(BitB, 1, true);
            }
        }
        AlignInput(BitB, 1); // rbsp_trailing_bits
    }
    
    void ParseNALSequenceParameterSet(AVCFile *AVC, BitBuffer *BitB) { // seq_parameter_set_rbsp
        ParseSequenceParameterSetData(AVC, BitB); // seq_parameter_set_data
        AlignInput(BitB, 1);            // rbsp_trailing_bits();
    }
    
    /* Video Usability Information */
    void ParseVideoUsabilityInformation(AVCFile *AVC, BitBuffer *BitB) { // Video Usability Information; ParseVUIParameters
        AVC->VUI->AspectRatioInfoPresent                           = ReadBits(BitB, 1, true);
        if (AVC->VUI->AspectRatioInfoPresent == true) {
            AVC->VUI->AspectRatioIDC                               = ReadBits(BitB, 8, true);
            if (AVC->VUI->AspectRatioIDC == 255) { // Extended_SAR = 255
                AVC->VUI->SARWidth                                 = ReadBits(BitB, 16, true);
                AVC->VUI->SAWHeight                                = ReadBits(BitB, 16, true);
            }
        }
        AVC->VUI->OverscanInfoPresent                              = ReadBits(BitB, 1, true);
        if (AVC->VUI->OverscanInfoPresent == true) {
            AVC->VUI->DisplayInOverscan                            = ReadBits(BitB, 1, true);
        }
        AVC->VUI->VideoSignalTypePresent                           = ReadBits(BitB, 1, true);
        if (AVC->VUI->VideoSignalTypePresent == true) {
            AVC->VUI->VideoType                                    = ReadBits(BitB, 3, true);
            AVC->VUI->FullRange                                    = ReadBits(BitB, 1, true);
            AVC->VUI->ColorDescriptionPresent                      = ReadBits(BitB, 1, true);
            if (AVC->VUI->ColorDescriptionPresent == true) {
                AVC->VUI->ColorPrimaries                           = ReadBits(BitB, 8, true);
                AVC->VUI->TransferProperties                       = ReadBits(BitB, 8, true);
                AVC->VUI->MatrixCoefficients                       = ReadBits(BitB, 8, true);
            }
        }
        AVC->VUI->ChromaLocationPresent                            = ReadBits(BitB, 1, true);
        if (AVC->VUI->ChromaLocationPresent == true) {
            AVC->VUI->ChromaSampleLocationTop                      = ReadExpGolomb(BitB, false);
            AVC->VUI->ChromaSampleLocationBottom                   = ReadExpGolomb(BitB, false);
        }
        AVC->VUI->TimingInfoPresent[0]                             = ReadBits(BitB, 1, true);
        if (AVC->VUI->TimingInfoPresent[0] == true) {
            AVC->VUI->UnitsPerTick[0]                              = ReadBits(BitB, 32, true);
            AVC->VUI->TimeScale[0]                                 = ReadBits(BitB, 32, true);
            AVC->VUI->FixedFrameRateFlag[0]                        = ReadBits(BitB, 1, true);
        }
        AVC->VUI->NALHrdParamsPresent[0]                           = ReadBits(BitB, 1, true);
        if (AVC->VUI->NALHrdParamsPresent[0] == true) {
            ParseHypotheticalReferenceDecoder(AVC, BitB);
        }
        AVC->VUI->VCLHrdParamsPresent[0]                           = ReadBits(BitB, 1, true);
        if (AVC->VUI->VCLHrdParamsPresent[0] == true) {
            ParseHypotheticalReferenceDecoder(AVC, BitB); // wat
        }
        if ((AVC->VUI->NALHrdParamsPresent[0] || AVC->VUI->VCLHrdParamsPresent[0]) == true) {
            AVC->StreamIsLowDelay                                  = ReadBits(BitB, 1, true);
        }
        AVC->VUI->PicStructPresent[0]                              = ReadBits(BitB, 1, true);
        AVC->VUI->BitStreamRestricted                              = ReadBits(BitB, 1, true);
        if (AVC->VUI->BitStreamRestricted == true) {
            AVC->VUI->MotionVectorsOverPicBoundaries[0]            = ReadBits(BitB, 1, true);
            AVC->VUI->MaxBytesPerPicDenom[0]                       = ReadExpGolomb(BitB, false);
            AVC->VUI->MaxBitsPerMarcoBlockDenom[0]                 = ReadExpGolomb(BitB, false);
            AVC->VUI->MaxMotionVectorLength[0]                     = ReadExpGolomb(BitB, false);
            AVC->VUI->MaxMotionVectorHeight[0]                     = ReadExpGolomb(BitB, false);
            AVC->VUI->MaxReorderFrames[0]                          = ReadExpGolomb(BitB, false);
            AVC->VUI->MaxFrameBuffer[0]                            = ReadExpGolomb(BitB, false);
        }
    }
    
    void ParseMVCDVUIParametersExtension(AVCFile *AVC, BitBuffer *BitB) { // mvcd_vui_parameters_extension
        AVC->VUI->VUIMVCDNumOpPoints                                   = ReadExpGolomb(BitB, false) + 1;
        for (uint8_t MVCDOpPoint = 0; MVCDOpPoint < AVC->VUI->VUIMVCDNumOpPoints; MVCDOpPoint++) {
            AVC->VUI->VUIMVCDTemporalID[MVCDOpPoint]                   = ReadBits(BitB, 3, true);
            AVC->VUI->VUIMVCDNumTargetOutputViews[MVCDOpPoint]         = ReadExpGolomb(BitB, false) + 1;
            for (uint16_t VUIMVCDView = 0; VUIMVCDView < AVC->VUI->VUIMVCDNumTargetOutputViews[MVCDOpPoint]; VUIMVCDView++) {
                AVC->VUI->VUIMVCDViewID[MVCDOpPoint][VUIMVCDView]      = ReadExpGolomb(BitB, false);
                AVC->VUI->VUIMVCDDepthFlag[MVCDOpPoint][VUIMVCDView]   = ReadExpGolomb(BitB, false);
                AVC->VUI->VUIMVCDTextureFlag[MVCDOpPoint][VUIMVCDView] = ReadExpGolomb(BitB, false);
            }
            AVC->VUI->VUIMVCDTimingInfoPresent[MVCDOpPoint]            = ReadBits(BitB, 1, true);
            if (AVC->VUI->VUIMVCDTimingInfoPresent[MVCDOpPoint] == true) {
                AVC->VUI->VUIMVCDNumUnitsInTick[MVCDOpPoint]           = ReadBits(BitB, 32, true);
                AVC->VUI->VUIMVCDTimeScale[MVCDOpPoint]                = ReadBits(BitB, 32, true);
                AVC->VUI->VUIMVCDFixedFrameRateFlag[MVCDOpPoint]       = ReadBits(BitB, 1, true);
            }
            AVC->VUI->VUIMVCDNalHRDParametersPresent[MVCDOpPoint]      = ReadBits(BitB, 1, true);
            if (AVC->VUI->VUIMVCDNalHRDParametersPresent[MVCDOpPoint] == true) {
                hrd_parameters();
            }
            AVC->VUI->VUIMVCDVclHRDParametersPresent[MVCDOpPoint]      = ReadBits(BitB, 1, true);
            if (AVC->VUI->VUIMVCDVclHRDParametersPresent[MVCDOpPoint] == true) {
                hrd_parameters();
            }
            if ((AVC->VUI->VUIMVCDNalHRDParametersPresent[MVCDOpPoint] || AVC->VUI->VUIMVCDVclHRDParametersPresent[MVCDOpPoint]) == true) {
                AVC->VUI->VUIMVCDHRDLowDelayPresent[MVCDOpPoint]       = ReadBits(BitB, 1, true);
            }
            AVC->VUI->VUIMVCDPicStructPresent[MVCDOpPoint]             = ReadBits(BitB, 1, true);
        }
    }
    
    void ParseMVCVUIParametersExtension(AVCFile *AVC, BitBuffer *BitB) { // mvc_vui_parameters_extension
        AVC->VUI->MVCNumOpertionPoints                     = ReadExpGolomb(BitB, false) + 1;
        for (uint16_t Operation = 0; Operation < AVC->VUI->MVCNumOpertionPoints; Operation++) {
            AVC->VUI->MVCTemporalID[0][Operation]          = ReadBits(BitB, 3, true);
            AVC->VUI->MVCNumTargetViews[Operation]         = ReadExpGolomb(BitB, false) + 1;
            for (uint16_t OutputView = 0; OutputView < AVC->VUI->MVCNumTargetViews[Operation]; OutputView++) {
                AVC->VUI->MVCViewID[Operation][OutputView] = ReadExpGolomb(BitB, false);
            }
            AVC->VUI->TimingInfoPresent[Operation]         = ReadBits(BitB, 1, true);
            if (AVC->VUI->TimingInfoPresent[Operation] == true) {
                AVC->VUI->MVCUnitsInTick[Operation]        = ReadBits(BitB, 32, true);
                AVC->VUI->MVCTimeScale[Operation]          = ReadBits(BitB, 32, true);
                AVC->VUI->MVCFixedFrameRate[Operation]     = ReadBits(BitB, 1, true);
            }
            AVC->VUI->MVCNALHRDParamsPresent[Operation]    = ReadBits(BitB, 1, true);
            if (AVC->VUI->MVCNALHRDParamsPresent[Operation] == true) {
                ParseHypotheticalReferenceDecoder(AVC, BitB);
            }
            AVC->VUI->MVCVCLHRDParamsPresent[Operation]    = ReadBits(BitB, 1, true);
            if (AVC->VUI->MVCVCLHRDParamsPresent[Operation] == true) {
                ParseHypotheticalReferenceDecoder(AVC, BitB);
            }
            if ((AVC->VUI->MVCNALHRDParamsPresent[Operation] || AVC->VUI->MVCVCLHRDParamsPresent[Operation]) == true) {
                AVC->VUI->MVCLowDelayFlag[Operation]       = ReadBits(BitB, 1, true);
            }
            AVC->VUI->PicStructPresent[Operation]          = ReadBits(BitB, 1, true);
        }
    }
    
    void ParseSVCVUIExtension(AVCFile *AVC, BitBuffer *BitB) { // svc_vui_parameters_extension
        AVC->VUI->VUIExtNumEntries                              = ReadExpGolomb(BitB, false) + 1;
        for (uint8_t VUIExtEntry = 0; VUIExtEntry < AVC->VUI->VUIExtNumEntries; VUIExtEntry++) {
            AVC->VUI->VUIExtDependencyID[VUIExtEntry]           = ReadBits(BitB, 3, true);
            AVC->VUI->VUIExtQualityID[VUIExtEntry]              = ReadBits(BitB, 4, true);
            AVC->VUI->VUIExtTemporalID[VUIExtEntry]             = ReadBits(BitB, 3, true);
            AVC->VUI->VUIExtTimingInfoPresentFlag[VUIExtEntry]  = ReadBits(BitB, 1, true);
            if (AVC->VUI->VUIExtTimingInfoPresentFlag[VUIExtEntry] == true) {
                AVC->VUI->VUIExtNumUnitsInTick[VUIExtEntry]     = ReadBits(BitB, 32, true);
                AVC->VUI->VUIExtTimeScale[VUIExtEntry]          = ReadBits(BitB, 32, true);
                AVC->VUI->VUIExtFixedFrameRateFlag[VUIExtEntry] = ReadBits(BitB, 1, true);
            }
            AVC->VUI->VUIExtNALHRDPresentFlag[VUIExtEntry]      = ReadBits(BitB, 1, true);
            if (AVC->VUI->VUIExtNALHRDPresentFlag[VUIExtEntry] == true) {
                ParseHypotheticalReferenceDecoder(AVC, BitB);
            }
            AVC->VUI->VUIExtVCLHRDPresentFlag[VUIExtEntry]      = ReadBits(BitB, 1, true);
            if (AVC->VUI->VUIExtVCLHRDPresentFlag[VUIExtEntry] == true) {
                ParseHypotheticalReferenceDecoder(AVC, BitB);
            }
            if ((AVC->VUI->VUIExtNALHRDPresentFlag[VUIExtEntry] == true) || (AVC->VUI->VUIExtVCLHRDPresentFlag[VUIExtEntry] == true)) {
                AVC->VUI->VUIExtLowDelayHRDFlag[VUIExtEntry]    = ReadBits(BitB, 1, true);
            }
            AVC->VUI->VUIExtPicStructPresentFlag[VUIExtEntry]   = ReadBits(BitB, 1, true);
        }
    }
    
    /* Picture Parameter Set */
    void ParseNALPictureParameterSet(AVCFile *AVC, BitBuffer *BitB) { // pic_parameter_set_rbsp
        AVC->PPS->PicParamSetID                         = ReadExpGolomb(BitB, false); // 3?
        AVC->SPS->SeqParamSetID                         = ReadExpGolomb(BitB, false); // 2
        AVC->PPS->EntropyCodingMode                     = ReadBits(BitB, 1, true); // bit 9, 0
        AVC->PPS->BottomPicFieldOrderInSliceFlag        = ReadBits(BitB, 1, true); // bit 8, 0
        AVC->PPS->SliceGroups                           = ReadExpGolomb(BitB, false) + 1; // 2
        if (AVC->PPS->SliceGroups > 0) {
            AVC->PPS->SliceGroupMapType                 = ReadExpGolomb(BitB, false); // 48?
            if (AVC->PPS->SliceGroupMapType == 0) {
                for (uint8_t SliceGroup = 0; SliceGroup <= AVC->PPS->SliceGroups; SliceGroup++) {
                    AVC->PPS->RunLength[SliceGroup]     = ReadExpGolomb(BitB, false) + 1;
                }
            } else if (AVC->PPS->SliceGroupMapType == 2) {
                for (uint8_t SliceGroup = 0; SliceGroup <= AVC->PPS->SliceGroups; SliceGroup++) {
                    AVC->PPS->TopLeft[SliceGroup]       = ReadExpGolomb(BitB, false);
                    AVC->PPS->BottomRight[SliceGroup]   = ReadExpGolomb(BitB, false);
                }
            } else if ((AVC->PPS->SliceGroupMapType == 3) || (AVC->PPS->SliceGroupMapType == 4) || (AVC->PPS->SliceGroupMapType == 5)) {
                for (uint8_t SliceGroup = 0; SliceGroup <= AVC->PPS->SliceGroups; SliceGroup++) {
                    AVC->PPS->SliceGroupChangeDirection = ReadBits(BitB, 1, true);
                    AVC->PPS->SliceGroupChangeRate      = ReadExpGolomb(BitB, false) + 1;
                }
            } else if (AVC->PPS->SliceGroupMapType == 6) {
                AVC->PPS->PicSizeInMapUnits             = ReadExpGolomb(BitB, false) + 1;
                for (uint64_t MapUnit = 0; MapUnit <= AVC->PPS->PicSizeInMapUnits; MapUnit++) {
                    AVC->PPS->SliceGroupID[MapUnit]     = ReadBits(BitB, Ceili(log2(AVC->PPS->SliceGroups)));
                }
            }
        }
        AVC->PPS->RefIndex[0]                           = ReadExpGolomb(BitB, false) + 1;
        AVC->PPS->RefIndex[1]                           = ReadExpGolomb(BitB, false) + 1;
        AVC->PPS->WeightedPrediction                    = ReadBits(BitB, 1, true);
        AVC->PPS->WeightedBiPrediction                  = ReadBits(BitB, 1, true);
        AVC->PPS->InitialSliceQP                        = ReadExpGolomb(BitB, true) + 26;
        AVC->PPS->InitialSliceQS                        = ReadExpGolomb(BitB, true) + 26;
        AVC->PPS->ChromaQPOffset                        = ReadExpGolomb(BitB, true);
        AVC->PPS->DeblockingFilterFlag                  = ReadBits(BitB, 1, true);
        AVC->PPS->ConstrainedIntraFlag                  = ReadBits(BitB, 1, true);
        AVC->PPS->RedundantPictureFlag                  = ReadBits(BitB, 1, true);
        if (more_rbsp_data() == true) {
            AVC->PPS->TransformIs8x8                    = ReadBits(BitB, 1, true);
            AVC->PPS->SeperateScalingMatrix             = ReadBits(BitB, 1, true);
            if (AVC->PPS->SeperateScalingMatrix == true) {
                for (uint8_t i = 0; i < 6 + ((AVC->SPS->ChromaFormatIDC != Chroma444) ? 2 : 6) * AVC->PPS->TransformIs8x8; i++) {
                    AVC->PPS->PicScalingList[i]         = ReadBits(BitB, 1, true);
                    if (AVC->PPS->PicScalingList[i] == true) {
                        if (i < 6) {
                            ScalingList(AVC, BitB, ScalingList4x4[i], 16, UseDefaultScalingMatrix4x4Flag[i]);
                        } else {
                            ScalingList(AVC, BitB, ScalingList8x8[i - 6], 64, UseDefaultScalingMatrix8x8Flag[i - 6]);
                        }
                    }
                }
                AVC->PPS->ChromaQPOffset                = ReadExpGolomb(BitB, true);
            }
            AlignInput(BitB, 1);
        }
    }
    
    /* Scalable Video Coding */
    void ParseNALSVCExtension(AVCFile *AVC, BitBuffer *BitB) { // nal_unit_header_svc_extension
        AVC->NAL->IDRFlag                     = ReadBits(BitB, 1, true);
        AVC->NAL->PriorityID[0]               = ReadBits(BitB, 6, true);
        AVC->NAL->NoInterLayerPredictionFlag  = ReadBits(BitB, 1, true);
        AVC->NAL->DependencyID[0]             = ReadBits(BitB, 3, true);
        AVC->NAL->QualityID[0][0]             = ReadBits(BitB, 4, true);
        AVC->NAL->TemporalID[0]               = ReadBits(BitB, 3, true);
        AVC->NAL->UseReferenceBasePictureFlag = ReadBits(BitB, 1, true);
        AVC->NAL->IsDisposable[0]             = ReadBits(BitB, 1, true);
        AVC->NAL->OutputFlag                  = ReadBits(BitB, 1, true);
        SkipBits(BitB, 2); // reserved_three_2bits
    }
    
    void ParseNALSequenceParameterSetSVC(AVCFile *AVC, BitBuffer *BitB) { // seq_parameter_set_svc_extension
        AVC->SVC->InterLayerDeblockingFilterPresent = ReadBits(BitB, 1, true);
        AVC->SVC->ExtendedSpatialScalabilityIDC     = ReadBits(BitB, 2, true);
        if ((AVC->SPS->ChromaFormatIDC == Chroma420) || (AVC->SPS->ChromaFormatIDC == Chroma422)) {
            AVC->SVC->ChromaPhaseXFlag              = ReadBits(BitB, 1, true);
        }
        if (AVC->SPS->ChromaFormatIDC == Chroma420) {
            AVC->SVC->ChromaPhaseY                  = ReadBits(BitB, 2, true);
        }
        if (AVC->SVC->ExtendedSpatialScalabilityIDC == 1) {
            if (AVC->SPS->ChromaFormatIDC != ChromaBW) {
                AVC->SVC->SeqRefLayerChromaPhaseX   = ReadBits(BitB, 1, true);
                AVC->SVC->SeqRefLayerChromaPhaseY   = ReadBits(BitB, 2, true);
            }
            AVC->SVC->RefLayerLeftOffset            = ReadExpGolomb(BitB, true);
            AVC->SVC->RefLayerTopOffset             = ReadExpGolomb(BitB, true);
            AVC->SVC->RefLayerRightOffset           = ReadExpGolomb(BitB, true);
            AVC->SVC->RefLayerBottomOffset          = ReadExpGolomb(BitB, true);
        }
        AVC->SVC->SequenceCoeffLevelPresent         = ReadBits(BitB, 1, true);
        if (AVC->SVC->SequenceCoeffLevelPresent == true) {
            AVC->SVC->AdaptiveCoeffsPresent         = ReadBits(BitB, 1, true);
        }
        AVC->SVC->SliceHeaderRestricted             = ReadBits(BitB, 1, true);
    }
    
    void ParseNALPrefixUnitSVC(AVCFile *AVC, BitBuffer *BitB) { // prefix_nal_unit_svc
        if (AVC->NAL->NALRefIDC != 0) {
            AVC->Slice->StoreRefBasePicFlag                        = ReadBits(BitB, 1, true);
            if (((AVC->NAL->UseReferenceBasePictureFlag) || (AVC->Slice->StoreRefBasePicFlag)) && (!AVC->NAL->IDRFlag)) {
                ParseReferenceBasePictureSyntax(AVC, BitB); // dec_ref_base_pic_marking();
            }
            AVC->NAL->AdditionalPrefixNALExtensionFlag             = ReadBits(BitB, 1, true);
            if (AVC->NAL->AdditionalPrefixNALExtensionFlag == true) {
                while (more_rbsp_data()) {
                    AVC->NAL->AdditionalPrefixNALExtensionDataFlag = ReadBits(BitB, 1, true);
                }
            }
            AlignInput(BitB, 1); // rbsp_trailing_bits()
        } else if (more_rbsp_data()) {
            while (more_rbsp_data()) {
                AVC->NAL->AdditionalPrefixNALExtensionDataFlag     = ReadBits(BitB, 1, true);
            }
        }
        AlignInput(BitB, 1); // rbsp_trailing_bits()
    }
    
    /* Multi-View Coding */
    void ParseNALMVCExtension(AVCFile *AVC, BitBuffer *BitB) { // nal_unit_header_mvc_extension
        AVC->NAL->NonIDRFlag                  = ReadBits(BitB, 1, true);
        AVC->NAL->PriorityID[0]               = ReadBits(BitB, 6, true);
        AVC->SPS->ViewID[0][0]                = ReadBits(BitB, 10, true);
        AVC->NAL->TemporalID[0]               = ReadBits(BitB, 3, true);
        AVC->NAL->IsAnchorPicture             = ReadBits(BitB, 1, true);
        AVC->NAL->InterViewFlag               = ReadBits(BitB, 1, true);
        SkipBits(BitB, 1);
    }
    
    void ParseSPSMVCDExtension(AVCFile *AVC, BitBuffer *BitB) { // seq_parameter_set_mvcd_extension
        AVC->SPS->ViewCount                                                    = ReadExpGolomb(BitB, false) + 1;
        for (uint16_t View = 0; View < AVC->SPS->ViewCount; View++) {
            AVC->SPS->ViewID[View][0]                                          = ReadExpGolomb(BitB, false);
            AVC->SPS->DepthViewPresent[View]                                   = ReadBits(BitB, 1, true);
            AVC->SPS->DepthViewID[View]                                        = AVC->SPS->ViewID[View][0];
            AVC->DPS->NumDepthViews                                           += AVC->SPS->DepthViewPresent[View];
            AVC->SPS->TextureViewPresent[View]                                 = ReadBits(BitB, 1, true);
        }
        for (uint16_t View = 1; View < AVC->SPS->ViewCount; View++) {
            if (AVC->SPS->DepthViewPresent[View] == true) {
                AVC->SPS->AnchorRefsCount[0][View]                             = ReadExpGolomb(BitB, false);
                for (uint8_t AnchorRef                                         = 0; AnchorRef < AVC->SPS->AnchorRefsCount[0][View]; AnchorRef++) {
                    AVC->SPS->AnchorRef[0][View][AnchorRef]                    = ReadExpGolomb(BitB, false);
                }
                AVC->SPS->AnchorRefsCount[1][View]                             = ReadExpGolomb(BitB, false);
                for (uint8_t AnchorRef                                         = 0; AnchorRef < AVC->SPS->AnchorRefsCount[1][View]; AnchorRef++) {
                    AVC->SPS->AnchorRef[1][View][AnchorRef]                    = ReadExpGolomb(BitB, false);
                }
            }}
        for (uint16_t View = 1; View < AVC->SPS->ViewCount; View++) {
            if (AVC->SPS->DepthViewPresent[View] == true) {
                AVC->SPS->NonAnchorRefCount[0][View]                           = ReadExpGolomb(BitB, false);
                for (uint16_t NonAnchorRef                                     = 0; NonAnchorRef < AVC->SPS->NonAnchorRefCount[0][View]; NonAnchorRef++) {
                    AVC->SPS->NonAnchorRef[0][View][NonAnchorRef]              = ReadExpGolomb(BitB, false);
                }
                AVC->SPS->NonAnchorRefCount[1][View]                           = ReadExpGolomb(BitB, false);
                for (uint16_t NonAnchorRef                                     = 0; NonAnchorRef < AVC->SPS->NonAnchorRefCount[0][View]; NonAnchorRef++) {
                    AVC->SPS->NonAnchorRef[1][View][NonAnchorRef]              = ReadExpGolomb(BitB, false);
                }
            }
        }
        AVC->SPS->NumLevelValues                                               = ReadExpGolomb(BitB, false) + 1;
        for (uint8_t Level = 0; Level < AVC->SPS->NumLevelValues; Level++) {
            AVC->SPS->LevelIDC[Level]                                          = ReadExpGolomb(BitB, false);
            AVC->SPS->NumApplicableOps[Level]                                  = ReadExpGolomb(BitB, false) + 1;
            for (uint16_t AppOp = 0; AppOp < AVC->SPS->NumApplicableOps[Level]; AppOp++) {
                AVC->SPS->AppOpTemporalID[Level][AppOp]                        = ReadExpGolomb(BitB, false);
                AVC->SPS->AppOpNumTargetViews[Level][AppOp]                    = ReadExpGolomb(BitB, false) + 1;
                for (uint16_t AppOpTargetView = 0; AppOpTargetView < AVC->SPS->AppOpNumTargetViews[Level][AppOp]; AppOpTargetView++) {
                    AVC->SPS->AppOpTargetViewID[Level][AppOp][AppOpTargetView] = ReadExpGolomb(BitB, false);
                    AVC->SPS->AppOpDepthFlag[Level][AppOp][AppOpTargetView]    = ReadExpGolomb(BitB, false);
                    AVC->SPS->AppOpTextureFlag[Level][AppOp][AppOpTargetView]  = ReadExpGolomb(BitB, false);
                }
                AVC->SPS->AppOpTextureViews[Level][AppOp]                      = ReadExpGolomb(BitB, false) + 1;
                AVC->SPS->AppOpNumDepthViews[Level][AppOp]                     = ReadExpGolomb(BitB, false);
            }
        }
        AVC->SPS->MVCDVUIParametersPresent                                     = ReadBits(BitB, 1, true);
        if (AVC->SPS->MVCDVUIParametersPresent == true) {
            ParseMVCDVUIParametersExtension(AVC, BitB); // mvcd_vui_parameters_extension();
        }
        AVC->SPS->MVCDTextureVUIParametersPresent                              = ReadBits(BitB, 1, true);
        if (AVC->SPS->MVCDTextureVUIParametersPresent == true) {
            ParseMVCVUIParametersExtension(AVC, BitB); //mvc_vui_parameters_extension();
        }
    }
    
    void ParseNALDepthParameterSet(AVCFile *AVC, BitBuffer *BitB) { // depth_parameter_set_rbsp
        AVC->DPS->DepthParameterSetID             = ReadExpGolomb(BitB, false);
        AVC->DPS->PredictionDirection             = ReadExpGolomb(BitB, false);
        if ((AVC->DPS->PredictionDirection == 0) || (AVC->DPS->PredictionDirection == 1)) {
            AVC->DPS->ReferenceDPSID[0]           = ReadExpGolomb(BitB, false);
            AVC->DPS->PredictedWeight0            = 64;
        }
        if (AVC->DPS->PredictionDirection == 0) {
            AVC->DPS->ReferenceDPSID[1]           = ReadExpGolomb(BitB, false);
            AVC->DPS->PredictedWeight0            = ReadBits(BitB, 6, true);
        }
        AVC->DPS->NumDepthViews                   = ReadExpGolomb(BitB, false) + 1;
        depth_ranges(BitB, AVC->DPS->NumDepthViews, AVC->DPS->PredictionDirection, AVC->DPS->DepthParameterSetID);
        AVC->DPS->VSPParamFlag                    = ReadBits(BitB, 1, true);
        if (AVC->DPS->VSPParamFlag == true) {
            vsp_param(AVC, BitB, AVC->DPS->NumDepthViews, AVC->DPS->PredictionDirection, AVC->DPS->DepthParameterSetID);
        }
        AVC->DPS->AdditionalExtensionFlag         = ReadBits(BitB, 1, true);
        AVC->DPS->DepthMappingValues              = ReadExpGolomb(BitB, false);
        for (uint8_t i = 1; i <= AVC->DPS->DepthMappingValues; i++) {
            AVC->DPS->DepthRepresentationModel[i] = ReadExpGolomb(BitB, false);
        }
        if (AVC->DPS->AdditionalExtensionFlag == true) {
            while (more_rbsp_data() == true) {
                AVC->DPS->AdditionalExtensionFlag = ReadBits(BitB, 1, true);
            }
        }
        AlignInput(BitB, 1); // rbsp_trailing_bits
    }
    
    void ParseSPS3DAVCExtension(AVCFile *AVC, BitBuffer *BitB) { // seq_parameter_set_3davc_extension
        if (AVC->DPS->NumDepthViews > 0) {
            AVC->SPS->AVC3DAcquisitionIDC                               = ReadExpGolomb(BitB, false);
            for (uint8_t View = 0; View < AVC->DPS->NumDepthViews; View++) {
                AVC->SPS->AVC3DViewID[View]                             = ReadExpGolomb(BitB, false);
            }
            if (AVC->SPS->AVC3DAcquisitionIDC > 0) {
                DepthRanges(BitB, AVC->DPS->NumDepthViews, 2, 0);
                vsp_param(AVC, BitB, AVC->DPS->NumDepthViews, 2, 0);
            }
            AVC->SPS->ReducedResolutionFlag                             = ReadBits(BitB, 1, true);
            if (AVC->SPS->ReducedResolutionFlag == true) {
                AVC->SPS->DepthPicWidthInMacroBlocks                    = ReadExpGolomb(BitB, false) + 1;
                AVC->SPS->DepthPicHeightInMapUnits                      = ReadExpGolomb(BitB, false) + 1;
                AVC->SPS->DepthHorizontalDisparity                      = ReadExpGolomb(BitB, false) + 1;
                AVC->SPS->DepthVerticalDisparity                        = ReadExpGolomb(BitB, false) + 1;
                AVC->SPS->DepthHorizontalRSH                            = ReadExpGolomb(BitB, false);
                AVC->SPS->DepthVerticalRSH                              = ReadExpGolomb(BitB, false);
            } else {
                AVC->SPS->DepthHorizontalDisparity                      = 1;
                AVC->SPS->DepthVerticalDisparity                        = 1;
                AVC->SPS->DepthHorizontalRSH                            = 0;
                AVC->SPS->DepthVerticalRSH                              = 0;
            }
            AVC->SPS->DepthFrameCroppingFlag                            = ReadBits(BitB, 1, true);
            if (AVC->SPS->DepthFrameCroppingFlag == true) {
                AVC->SPS->DepthFrameLeftCropOffset                      = ReadExpGolomb(BitB, false);
                AVC->SPS->DepthFrameRightCropOffset                     = ReadExpGolomb(BitB, false);
                AVC->SPS->DepthFrameTopCropOffset                       = ReadExpGolomb(BitB, false);
                AVC->SPS->DepthFrameBottomCropOffset                    = ReadExpGolomb(BitB, false);
            }
            AVC->SPS->GridPosViewCount                                  = ReadExpGolomb(BitB, false);
            for (uint8_t TextureView = 0; TextureView < AVC->SPS->GridPosViewCount; TextureView++) {
                AVC->SPS->GridPosViewID[TextureView]                    = ReadExpGolomb(BitB, false);
                AVC->SPS->GridPosX[TextureView]                         = ReadExpGolomb(BitB, true);
                AVC->SPS->GridPosY[TextureView]                         = ReadExpGolomb(BitB, true);
            }
            AVC->SPS->SlicePrediction                                   = ReadBits(BitB, 1, true);
            AVC->Slice->SeqViewSynthesisFlag                            = ReadBits(BitB, 1, true);
        }
        AVC->SPS->ALCSpsEnableFlag                                      = ReadBits(BitB, 1, true);
        AVC->SPS->EnableRLESkipFlag                                     = ReadBits(BitB, 1, true);
        bool AllViewsPairedFlag                                         = AreAllViewsPaired(AVC);
        if (AllViewsPairedFlag == false) {
            for (uint8_t View = 0; View < AVC->SPS->ViewCount; View++) {
                if (AVC->SPS->TextureViewPresent[View] == true) {
                    AVC->SPS->AnchorRefsCount[0][View]                  = ReadExpGolomb(BitB, false);
                    for (uint16_t AnchorViewL0                          = 0; AnchorViewL0 < AVC->SPS->AnchorRefsCount[0][View]; AnchorViewL0++) {
                        AVC->SPS->AnchorRef[0][View][AnchorViewL0]      = ReadExpGolomb(BitB, false);
                    }
                    AVC->SPS->AnchorRefsCount[1][View]                  = ReadExpGolomb(BitB, false);
                    for (uint16_t AnchorViewL1                          = 0; AnchorViewL1 < AVC->SPS->AnchorRefsCount[1][View]; AnchorViewL1++) {
                        AVC->SPS->AnchorRef[1][View][AnchorViewL1]      = ReadExpGolomb(BitB, false);
                    }
                }
            }
            for (uint16_t View = 0; View <= AVC->SPS->ViewCount; View++) {
                if (AVC->SPS->TextureViewPresent[View] == true) {
                    AVC->SPS->NonAnchorRefCount[0][View]                = ReadExpGolomb(BitB, false);
                    for (uint16_t NonAnchorRefL0 = 0; NonAnchorRefL0 < AVC->SPS->NonAnchorRefCount[0][View]; NonAnchorRefL0++) {
                        AVC->SPS->NonAnchorRef[0][View][NonAnchorRefL0] = ReadExpGolomb(BitB, false);
                    }
                    AVC->SPS->NonAnchorRefCount[1][View]                = ReadExpGolomb(BitB, false);
                    for (uint16_t NonAnchorRefL1 = 0; NonAnchorRefL1 < AVC->SPS->NonAnchorRefCount[1][View]; NonAnchorRefL1++) {
                        AVC->SPS->NonAnchorRef[1][View][NonAnchorRefL1] = ReadExpGolomb(BitB, false);
                    }
                }
            }
        }
    }
    
    void ParseNAL3DAVCExtension(AVCFile *AVC, BitBuffer *BitB) { // nal_unit_header_3davc_extension
        AVC->NAL->ViewIndex                   = ReadBits(BitB, 8, true);
        AVC->NAL->DepthFlag                   = ReadBits(BitB, 1, true);
        AVC->NAL->NonIDRFlag                  = ReadBits(BitB, 1, true);
        AVC->NAL->TemporalID[0]               = ReadBits(BitB, 3, true);
        AVC->NAL->IsAnchorPicture             = ReadBits(BitB, 1, true);
        AVC->NAL->InterViewFlag               = ReadBits(BitB, 1, true);
    }
    
    void ParseNALMVCDepthView(AVCFile *AVC, BitBuffer *BitB) {
        
    }
    
    /* Hypothetical Reference Decoder */
    void ParseHypotheticalReferenceDecoder(AVCFile *AVC, BitBuffer *BitB) { // hrd_parameters( )
        AVC->HRD->NumCodedPictureBuffers             = ReadExpGolomb(BitB, false) + 1;
        AVC->HRD->BitRateScale                       = ReadBits(BitB, 4, true);
        AVC->HRD->CodedPictureBufferScale            = ReadBits(BitB, 4, true);
        for (uint8_t SchedSelIdx = 0; SchedSelIdx < AVC->HRD->NumCodedPictureBuffers; SchedSelIdx++) {
            AVC->HRD->BitRate[SchedSelIdx]           = ReadExpGolomb(BitB, false) + 1;
            AVC->HRD->CodedPictureSize[SchedSelIdx]  = ReadExpGolomb(BitB, false) + 1;
            AVC->HRD->IsConstantBitRate[SchedSelIdx] = ReadBits(BitB, 1, true) + 1; // FIXME: is +1 correct
        }
        AVC->HRD->InitialCPBDelayLength              = ReadBits(BitB, 5, true) + 1;
        AVC->HRD->CBPDelay                           = ReadBits(BitB, 5, true) + 1;
        AVC->HRD->DBPDelay                           = ReadBits(BitB, 5, true) + 1;
        AVC->HRD->TimeOffsetSize                     = ReadBits(BitB, 5, true);
    }
    
    /* Generic */
    void ParseNALSliceHeader(AVCFile *AVC, BitBuffer *BitB) { // slice_header
        AVC->Slice->FirstMacroBlockInSlice                                 = ReadExpGolomb(BitB, false); // 0
        AVC->Slice->Type                                                   = ReadExpGolomb(BitB, false); // 0, 34 0s remaining
        AVC->PPS->PicParamSetID                                            = ReadExpGolomb(BitB, false); // 0, 26 0s remaining
        
        if (AVC->SPS->SeperateColorPlane == true) {
            AVC->Slice->ColorPlaneID                                       = ReadBits(BitB, 2, true);
        }
        
        AVC->Slice->FrameNumber                                            = ReadExpGolomb(BitB, false); // FIXME: Should I use ReadBits?
        if (AVC->SPS->OnlyMacroBlocksInFrame == false) { //
            AVC->Slice->SliceIsInterlaced                                  = ReadBits(BitB, 1, true);
            if (AVC->Slice->SliceIsInterlaced == true) {
                AVC->Slice->SliceIsBottomField                             = ReadBits(BitB, 1, true);
            }
        }
        AVC->Slice->SliceIsIDR                                             = ((AVC->NAL->NALUnitType == NAL_IDRSliceNonPartitioned) ? true : false);
        if (AVC->Slice->SliceIsIDR == true) {
            AVC->Slice->IDRPicID                                           = ReadExpGolomb(BitB, false);
        }
        if (AVC->SPS->PicOrderCount == 0) {
            AVC->Slice->PictureOrderCountLSB                               = ReadExpGolomb(BitB, false);
            if ((AVC->PPS->BottomPicFieldOrderInSliceFlag == true) && (AVC->Slice->SliceIsInterlaced == false)) {
                AVC->Slice->DeltaPicOrderCount[0]                          = ReadExpGolomb(BitB, true);
            }
        }
        if (AVC->SPS->PicOrderCount == true && AVC->SPS->DeltaPicOrder == false) {
            AVC->Slice->DeltaPicOrderCount[0]                              = ReadExpGolomb(BitB, true);
            if ((AVC->PPS->BottomPicFieldOrderInSliceFlag == true) && (AVC->Slice->SliceIsInterlaced == false)) {
                AVC->Slice->DeltaPicOrderCount[1]                          = ReadExpGolomb(BitB, true);
            }
        }
        if (AVC->PPS->RedundantPictureFlag == true) {
            AVC->PPS->RedundantPictureCount                                = ReadExpGolomb(BitB, false);
        }
        if ((AVC->Slice->Type == SliceB1) || (AVC->Slice->Type == SliceB2)) {
            AVC->Slice->DirectSpatialMVPredictionFlag                      = ReadBits(BitB, 1, true);
            
        }
        if ((AVC->Slice->Type == SliceP1)  || (AVC->Slice->Type == SliceP2)  ||
            (AVC->Slice->Type == SliceSP1) || (AVC->Slice->Type == SliceSP2) ||
            (AVC->Slice->Type == SliceB1)  || (AVC->Slice->Type == SliceB2)) {
            AVC->Slice->NumRefIDXActiveOverrideFlag                        = ReadBits(BitB, 1, true);
            if (AVC->Slice->NumRefIDXActiveOverrideFlag == true) {
                AVC->MacroBlock->NumRefIndexActive[0]                      = ReadExpGolomb(BitB, false) + 1; // num_ref_idx_l0_active_minus1
                if ((AVC->Slice->Type == SliceB1) || (AVC->Slice->Type == SliceB2)) {
                    AVC->MacroBlock->NumRefIndexActive[1]                  = ReadExpGolomb(BitB, false) + 1; // num_ref_idx_l1_active_minus1
                }
            }
        }
        if ((AVC->NAL->NALUnitType == NAL_AuxiliarySliceExtension) || (AVC->NAL->NALUnitType == NAL_MVCDepthView)) {
            RefPicListMVCMod(AVC, BitB);
        } else {
            RefPicListMod(AVC, BitB);
        }
        if ((AVC->PPS->WeightedPrediction == true)
            && (
             (AVC->Slice->Type == SliceP1)  ||
             (AVC->Slice->Type == SliceP2)  ||
             (AVC->Slice->Type == SliceSP1) ||
             (AVC->Slice->Type == SliceSP2) ||
             (AVC->Slice->Type == SliceB1)  ||
             (AVC->Slice->Type == SliceB2)
             ) && (AVC->PPS->WeightedBiPrediction == true)) {
            pred_weight_table(AVC, BitB);
        }
        if (AVC->NAL->NALRefIDC != 0) {
            DecodeRefPicMarking(AVC, BitB);
        }
        if ((AVC->PPS->EntropyCodingMode  == true) && (((AVC->Slice->Type != SliceI1) || (AVC->Slice->Type != SliceI2) || (AVC->Slice->Type != SliceSI1) || (AVC->Slice->Type != SliceSI2)))) {
            AVC->Slice->CabacInitIDC                                       = ReadExpGolomb(BitB, true);
        }
        AVC->Slice->SliceQPDelta                                           = ReadExpGolomb(BitB, true);
        if (
            (AVC->Slice->Type == SliceSP1) || (AVC->Slice->Type == SliceSP2) ||
            (AVC->Slice->Type == SliceSI1) || (AVC->Slice->Type == SliceSI2)) {
            
            if ((AVC->Slice->Type == SliceSP1) || (AVC->Slice->Type == SliceSP2)) {
                AVC->Slice->DecodePMBAsSPSlice                             = ReadBits(BitB, 1, true);
            }
            AVC->Slice->SliceQSDelta                                       = ReadExpGolomb(BitB, true);
        }
        if (AVC->PPS->DeblockingFilterFlag == true) {
            AVC->Slice->DisableDeblockingFilterIDC                         = ReadExpGolomb(BitB, false);
            if (AVC->Slice->DisableDeblockingFilterIDC  != true) {
                AVC->Slice->SliceAlphaOffsetC0                             = ReadExpGolomb(BitB, true);
                AVC->Slice->SliceBetaOffset                                = ReadExpGolomb(BitB, true);
            }
        }
        if (AVC->PPS->SliceGroups > 0 && (AVC->PPS->SliceGroupMapType >= 3 && AVC->PPS->SliceGroupMapType <= 5)) {
            uint64_t Bits                                                  = Ceili(log2(AVC->PPS->PicSizeInMapUnits / AVC->PPS->SliceGroupChangeRate));
            AVC->Slice->SliceGroupChangeCycle                              = ReadBits(BitB, Ceili(log2(AVC->PPS->PicSizeInMapUnits /  AVC->Slice->SliceGroupChangeRate)), true);
        }
    }
    
    void ParseNALSliceData(AVCFile *AVC, BitBuffer *BitB, uint8_t Category) { // slice_data
        if (AVC->PPS->EntropyCodingMode == Arithmetic) {
            while (IsStreamByteAligned(BitB->BitsUnavailable, 1) == false) {
                SkipBits(BitB, 1); // cabac_alignment_one_bit
            }
            uint64_t CurrentMacroBlockAddress             = AVC->Slice->FirstMacroBlockInSlice * (AVC->Slice->MbaffFrameFlag + 1);
            bool     MoreDataFlag                         = true;
            AVC->Slice->PreviousMacroBlockSkipped         = false;
            if ((AVC->Slice->Type != SliceI1) || (AVC->Slice->Type != SliceI2) || (AVC->Slice->Type != SliceSI1) || (AVC->Slice->Type != SliceSI2)) {
                if (AVC->PPS->EntropyCodingMode == ExpGolomb) {
                    AVC->Slice->MacroBlockSkipRun         = ReadExpGolomb(BitB, false);
                    AVC->Slice->PreviousMacroBlockSkipped = (AVC->Slice->MacroBlockSkipRun > 0);
                    for (uint8_t SkippedMacroBlock        = 0; SkippedMacroBlock < AVC->Slice->MacroBlockSkipRun; SkippedMacroBlock++) {
                        CurrentMacroBlockAddress          = NextMacroBlockAddress(AVC, CurrentMacroBlockAddress);
                    }
                }
            }
        }
    }
    
    void ParseNALSlicePartitionA(AVCFile *AVC, BitBuffer *BitB) { // slice_data_partition_a_layer_rbsp
        ParseSliceHeader(AVC, BitB);
        uint64_t SliceID = ReadExpGolomb(BitB, false);
        ParseSliceData(AVC, BitB, 2); /* only category 2 parts of slice_data() syntax */
        rbsp_slice_trailing_bits(AVC, BitB); // AlignInput(BitB, 1);
    }
    
    void ParseNALSlicePartitionB(AVCFile *AVC, BitBuffer *BitB) { // slice_data_partition_b_layer_rbsp
        uint8_t SliceID = ReadExpGolomb(BitB, false);
        if (AVC->SPS->SeperateColorPlane == true) {
            uint8_t ColorPlaneID = ReadExpGolomb(BitB, false);
        }
        if (AVC->PPS->RedundantPictureFlag == true) {
            AVC->PPS->RedundantPictureCount = ReadExpGolomb(BitB, false);
        }
        ParseSliceData(AVC, BitB, 3);
        rbsp_slice_trailing_bits(AVC, BitB); // AlignInput(BitB, 1);
    }
    
    void ParseNALSlicePartitionC(AVCFile *AVC, BitBuffer *BitB) { // slice_data_partition_c_layer_rbsp
        uint8_t SliceID = ReadExpGolomb(BitB, false);
        if (AVC->SPS->SeperateColorPlane == true) {
            uint8_t ColorPlaneID = ReadExpGolomb(BitB, false);
        }
        if (AVC->PPS->RedundantPictureFlag == true) {
            AVC->PPS->RedundantPictureCount = ReadExpGolomb(BitB, false);
        }
        ParseSliceData(AVC, BitB, 4);
        rbsp_slice_trailing_bits(AVC, BitB); // AlignInput(BitB, 1);
    }
    
    void ParseNALSliceNonPartitioned(AVCFile *AVC, BitBuffer *BitB) { // slice_layer_without_partitioning_rbsp
        ParseNALSliceHeader(AVC, BitB);
        ParseNALSliceData(AVC, BitB, 0); // TODO: Fix category
        AlignInput(BitB, 1); // rbsp_slice_trailing_bits();
    }
    
    void ParseNALFillerData(AVCFile *AVC, BitBuffer *BitB) { // filler_data_rbsp
        while (PeekBits(BitB, 8, true) == 0xFF) {
            SkipBits(BitB, 8);
        }
    }
    
    void ParseNALPrefixUnit(AVCFile *AVC, BitBuffer *BitB) { // prefix_nal_unit_rbsp
        if (AVC->NAL->SVCExtensionFlag == true) {
            ParseNALPrefixUnitSVC(AVC, BitB);
        }
    }
    
    void ParseNALAccessUnitDelimiter(AVCFile *AVC, BitBuffer *BitB) { // access_unit_delimiter_rbsp
        AVC->Slice->PictureType = ReadBits(BitB, 3, true);
        AlignInput(BitB, 1);
    }
    
    /* Supplemental Enhancement Information */
    void ParseSEIBufferingPeriod(AVCFile *AVC, BitBuffer *BitB) { // buffering_period
        AVC->SPS->SeqParamSetID = ReadExpGolomb(BitB, false);
        if (AVC->SEI->NalHrdBpPresentFlag == true) {
            for (uint8_t SchedSelIdx = 0; SchedSelIdx <= AVC->HRD->NumCodedPictureBuffers; SchedSelIdx++) {
                AVC->SEI->InitialCPBRemovalDelay[SchedSelIdx] = ReadBits(BitB, AVC->HRD->InitialCPBDelayLength, true);
                AVC->SEI->InitialCPBRemovalDelayOffset[SchedSelIdx] = ReadBits(BitB, AVC->HRD->InitialCPBDelayLength, true);
            }
        }
        AVC->HRD->VclHrdBpPresentFlag = AVC->VUI->VCLHrdParamsPresent > 0 ? 1 : 0;
        if (AVC->HRD->VclHrdBpPresentFlag == true) {
            for (uint8_t SchedSelIdx = 0; SchedSelIdx <= AVC->HRD->NumCodedPictureBuffers; SchedSelIdx++) {
                AVC->SEI->InitialCPBRemovalDelay[SchedSelIdx] = ReadBits(BitB, AVC->HRD->InitialCPBDelayLength, true);
                AVC->SEI->InitialCPBRemovalDelayOffset[SchedSelIdx] = ReadBits(BitB, AVC->HRD->InitialCPBDelayLength, true);
            }
        }
    }
    
    uint8_t GetClockTS(uint8_t PicStruct) {
        uint8_t ClockTS = 0;
        if ((PicStruct == 0) || (PicStruct == 1) || (PicStruct == 2)) {
            ClockTS     = 1;
        } else if ((PicStruct == 3) || (PicStruct == 4) || (PicStruct == 7)) {
            ClockTS     = 2;
        } else {
            ClockTS     = 3;
        }
        return ClockTS;
    }
    
    void ParseSEIPictureTiming(AVCFile *AVC, BitBuffer *BitB) { // pic_timing
        if (AVC->SEI->CpbDpbDelaysPresentFlag == true) {
            AVC->SEI->CPBRemovalDelay                   = ReadBits(BitB, AVC->HRD->CBPDelay, true);
            AVC->SEI->DPBOutputDelay                    = ReadBits(BitB, AVC->HRD->DBPDelay, true);
        }
        if (AVC->VUI->PicStructPresent[0] == true) {
            AVC->SEI->PicStruct                         = ReadBits(BitB, 4, true);
            uint8_t NumClockTS = GetClockTS(AVC->SEI->PicStruct);
            for (uint8_t Time = 0; Time < NumClockTS; Time++) {
                AVC->SEI->ClockTimestampFlag[Time]  = ReadBits(BitB, 1, true);
                if (AVC->SEI->ClockTimestampFlag[Time] == true) {
                    AVC->SEI->CTType                = ReadBits(BitB, 2, true);
                    AVC->SEI->NuitFieldBasedFlag    = ReadBits(BitB, 1, true);
                    AVC->SEI->CountingType          = ReadBits(BitB, 5, true);
                    AVC->SEI->FullTimestampFlag     = ReadBits(BitB, 1, true);
                    AVC->SEI->CountDroppedFlag      = ReadBits(BitB, 1, true);
                    AVC->SEI->NFrames               = ReadBits(BitB, 8, true);
                    if (AVC->SEI->FullTimestampFlag == true) {
                        AVC->SEI->Seconds           = ReadBits(BitB, 6, true);
                        AVC->SEI->Minutes           = ReadBits(BitB, 6, true);
                        AVC->SEI->Hours             = ReadBits(BitB, 5, true);
                    } else {
                        AVC->SEI->SecondsFlag       = ReadBits(BitB, 1, true);
                        if (AVC->SEI->SecondsFlag == true) {
                            AVC->SEI->Seconds       = ReadBits(BitB, 6, true);
                            AVC->SEI->MinutesFlag   = ReadBits(BitB, 1, true);
                            if (AVC->SEI->MinutesFlag == true) {
                                AVC->SEI->Minutes   = ReadBits(BitB, 6, true);
                                AVC->SEI->HoursFlag = ReadBits(BitB, 1, true);
                                if (AVC->SEI->HoursFlag == true) {
                                    AVC->SEI->Hours = ReadBits(BitB, 5, true);
                                }
                            }
                        }
                        if (AVC->HRD->TimeOffsetSize > 0) {
                            AVC->SEI->TimeOffset = ReadBits(BitB, AVC->HRD->TimeOffsetSize, true);
                        }
                    }
                }
            }
        }
    }
    
    void ParseSEIPanScan(AVCFile *AVC, BitBuffer *BitB) { // pan_scan_rect
        AVC->SEI->PanScanID = ReadExpGolomb(BitB, false);
        AVC->SEI->DisablePanScanFlag = ReadBits(BitB, 1, true);
        if (AVC->SEI->DisablePanScanFlag == false) {
            AVC->SEI->PanScanCount   = ReadExpGolomb(BitB, false) + 1;
            for (uint8_t PanScan = 0; PanScan < AVC->SEI->PanScanCount; PanScan++) {
                AVC->SEI->PanScanOffsetLeft[PanScan]   = ReadExpGolomb(BitB, true);
                AVC->SEI->PanScanOffsetRight[PanScan]  = ReadExpGolomb(BitB, true);
                AVC->SEI->PanScanOffsetTop[PanScan]    = ReadExpGolomb(BitB, true);
                AVC->SEI->PanScanOffsetBottom[PanScan] = ReadExpGolomb(BitB, true);
            }
            AVC->SEI->PanScanRepitionPeriod            = ReadExpGolomb(BitB, false);
        }
    }
    
    void ParseSEIFiller(AVCFile *AVC, BitBuffer *BitB) { // filler_payload
        SkipBits(BitB, Bytes2Bits(AVC->SEI->SEISize));
    }
    
    void ParseSEIRegisteredUserData(AVCFile *AVC, BitBuffer *BitB) { // user_data_registered_itu_t_t35
        uint8_t CountryCodeSize = 0;
        AVC->SEI->CountryCode = ReadBits(BitB, 8, true);
        if (AVC->SEI->CountryCode != 0xFF) {
            CountryCodeSize += 1;
        } else {
            CountryCodeSize += 2;
            AVC->SEI->CountryCode <<= 8;
            AVC->SEI->CountryCode += ReadBits(BitB, 8, true);
        }
        
    }
    
    void ParseSEIUnregisteredUserData(AVCFile *AVC, BitBuffer *BitB) { // user_data_unregistered
        ReadUUID(BitB, AVC->SEI->UnregisteredUserDataUUID); // DC45E9BD-E6D9-48B7-962C-D820D923EEEF, x264 UserID.
        SkipBits(BitB, Bytes2Bits(AVC->SEI->SEISize - BitIOBinaryUUIDSize));
    }
    
    void ParseSEIRecoveryPoint(AVCFile *AVC, BitBuffer *BitB) { // recovery_point
        AVC->SEI->RecoveryFrameCount    = ReadExpGolomb(BitB, false);
        AVC->SEI->ExactMatchFlag        = ReadBits(BitB, 1, true);
        AVC->SEI->BrokenLinkFlag        = ReadBits(BitB, 1, true);
        AVC->SEI->ChangingSliceGroupIDC = ReadBits(BitB, 2, true);
    }
    
    void ParseSEIRepetitiveReferencePicture(AVCFile *AVC, BitBuffer *BitB) { // dec_ref_pic_marking_repetition
        AVC->SEI->OriginalIDRFlag                 = ReadBits(BitB, 1, true);
        AVC->SEI->OriginalFrameNum                = ReadExpGolomb(BitB, false);
        if (AVC->SPS->OnlyMacroBlocksInFrame == false) {
            AVC->SEI->OriginalFieldPicFlag        = ReadBits(BitB, 1, true);
            if (AVC->SEI->OriginalFieldPicFlag == true) {
                AVC->SEI->OriginalBottomFieldFlag = ReadBits(BitB, 1, true);
            }
        }
        DecodeRefPicMarking(AVC, BitB); // dec_ref_pic_marking();
    }
    
    void ParseSEISparePicture(AVCFile *AVC, BitBuffer *BitB) { // spare_pic
        uint8_t MapUnitCount = 0;
        
        AVC->SEI->TargetFrameNum = ReadExpGolomb(BitB, false);
        AVC->SEI->SpareFieldFlag = ReadBits(BitB, 1, true);
        if (AVC->SEI->SpareFieldFlag == true) {
            AVC->SEI->TargetBottomFieldFlag = ReadBits(BitB, 1, true);
        }
        AVC->SEI->NumSparePics                           = ReadExpGolomb(BitB, false) + 1;
        for (uint8_t SparePic = 0; SparePic < AVC->SEI->NumSparePics; SparePic++) {
            AVC->SEI->DeltaSpareFrameNum[SparePic]       = ReadExpGolomb(BitB, false);
            if (AVC->SEI->SpareFieldFlag == true) {
                AVC->SEI->SpareBottomFieldFlag[SparePic] = ReadExpGolomb(BitB, false);
            }
            AVC->SEI->SpareAreaIDC[SparePic]             = ReadExpGolomb(BitB, false);
            if (AVC->SEI->SpareAreaIDC[SparePic] == 1) {
                for (uint8_t MapUnit = 0; MapUnit < AVC->PPS->PicSizeInMapUnits; MapUnit++) {
                    AVC->SEI->SpareUnitFlag[SparePic][MapUnit] = ReadBits(BitB, 1, true);
                }
            } else if (AVC->SEI->SpareAreaIDC[SparePic] == 2) {
                while (MapUnitCount < AVC->PPS->PicSizeInMapUnits) {
                    AVC->SEI->ZeroRunLength[SparePic][MapUnitCount] = ReadExpGolomb(BitB, false);
                    MapUnitCount += AVC->SEI->ZeroRunLength[SparePic][MapUnitCount] + 1;
                }
            }
        }
    }
    
    void ParseSEISceneInfo(AVCFile *AVC, BitBuffer *BitB) { // scene_info
        AVC->SEI->SceneInfoPresentFlag    = ReadBits(BitB, 1, true);
        if (AVC->SEI->SceneInfoPresentFlag == true) {
            AVC->SEI->SceneID             = ReadExpGolomb(BitB, false);
            AVC->SEI->SceneTransitionType = ReadExpGolomb(BitB, false);
            if (AVC->SEI->SceneTransitionType > 3) {
                AVC->SEI->SecondSceneID   = ReadExpGolomb(BitB, false);
            }
        }
    }
    
    void ParseSEISubSequenceInfo(AVCFile *AVC, BitBuffer *BitB) { // sub_seq_info
        AVC->SEI->SubSequenceLayerNum      = ReadExpGolomb(BitB, false);
        AVC->SEI->SubSequenceID            = ReadExpGolomb(BitB, false);
        AVC->SEI->FirstRefPicFlag          = ReadBits(BitB, 1, true);
        AVC->SEI->LeadingNonRefPicFlag     = ReadBits(BitB, 1, true);
        AVC->SEI->LastPicFlag              = ReadBits(BitB, 1, true);
        AVC->SEI->SubSeqFrameNumFlag       = ReadBits(BitB, 1, true);
        if (AVC->SEI->SubSeqFrameNumFlag  == true) {
            AVC->SEI->SubSeqFrameNum       = ReadExpGolomb(BitB, false);
        }
    }
    
    void ParseSEISubSequenceLayerProperties(AVCFile *AVC, BitBuffer *BitB) { // sub_seq_layer_characteristics
        AVC->SEI->NumSubSeqLayers            = ReadExpGolomb(BitB, false) + 1;
        for (uint8_t Layer = 0; Layer < AVC->SEI->NumSubSeqLayers; Layer++) {
            AVC->SEI->AccurateStatisticsFlag = ReadBits(BitB, 1, true);
            AVC->SEI->AverageBitRate         = ReadBits(BitB, 16, true);
            AVC->SEI->AverageFrameRate       = ReadBits(BitB, 16, true);
        }
    }
    
    void ParseSEISubSequenceProperties(AVCFile *AVC, BitBuffer *BitB) { // sub_seq_characteristics
        AVC->SEI->SubSequenceLayerNum        = ReadExpGolomb(BitB, false);
        AVC->SEI->SubSequenceID              = ReadExpGolomb(BitB, false);
        AVC->SEI->DurationFlag               = ReadBits(BitB, 1, true);
        if (AVC->SEI->DurationFlag == true) {
            AVC->SEI->SubSeqDuration         = ReadBits(BitB, 32, true);
        }
        AVC->SEI->AverageRateFlag            = ReadBits(BitB, 1, true);
        if (AVC->SEI->AverageRateFlag == true) {
            AVC->SEI->AccurateStatisticsFlag = ReadBits(BitB, 1, true);
            AVC->SEI->AverageBitRate         = ReadBits(BitB, 16, true);
            AVC->SEI->AverageFrameRate       = ReadBits(BitB, 16, true);
        }
        AVC->SEI->NumReferencedSubSeqs       = ReadExpGolomb(BitB, false);
        for (uint16_t SubSequence = 0; SubSequence < AVC->SEI->NumReferencedSubSeqs; SubSequence++) {
            AVC->SEI->RefSubSeqLayerNum      = ReadExpGolomb(BitB, false);
            AVC->SEI->RefSubSeqID            = ReadExpGolomb(BitB, false);
            AVC->SEI->RefSubSeqDirection     = ReadBits(BitB, 1, true);
        }
    }
    
    void ParseSEIFullFrameFreeze(AVCFile *AVC, BitBuffer *BitB) { // full_frame_freeze
        AVC->SEI->FullFrameFreezeRepitionPeriod = ReadExpGolomb(BitB, false);
    }
    
    void ParseSEIFullFrameFreezeRelease(AVCFile *AVC, BitBuffer *BitB) { // full_frame_freeze_release
        SkipBits(BitB, Bytes2Bits(AVC->SEI->SEISize));
    }
    
    void ParseSEIFullFrameSnapshot(AVCFile *AVC, BitBuffer *BitB) { // full_frame_snapshot
        AVC->SEI->SnapshotID                    = ReadExpGolomb(BitB, false);
    }
    
    void ParseSEIProgressiveRefinementSegmentStart(AVCFile *AVC, BitBuffer *BitB) { // progressive_refinement_segment_start
        AVC->SEI->ProgressiveRefinementID       = ReadExpGolomb(BitB, false);
        AVC->SEI->NumRefinementSteps            = ReadExpGolomb(BitB, false) + 1;
    }
    
    void ParseSEIProgressiveRefinementSegmentEnd(AVCFile *AVC, BitBuffer *BitB) { // progressive_refinement_segment_end
        AVC->SEI->ProgressiveRefinementID       = ReadExpGolomb(BitB, false);
    }
    
    void ParseSEIMotionConstrainedSliceGroupSet(AVCFile *AVC, BitBuffer *BitB) { // motion_constrained_slice_group_set
        AVC->SEI->NumSliceGroupsInSet              = ReadExpGolomb(BitB, false) + 1;
        if (AVC->SEI->NumSliceGroupsInSet > 1) {
            for (uint16_t SliceGroup = 0; SliceGroup < AVC->SEI->NumSliceGroupsInSet; SliceGroup++) {
                AVC->PPS->SliceGroupID[SliceGroup] = ReadBits(BitB, Ceili(log2(AVC->PPS->SliceGroups)), true);
            }
        }
        AVC->SEI->ExactSampleValueMatchFlag[0]     = ReadBits(BitB, 1, true);
        AVC->SEI->PanScanRectFlag                  = ReadBits(BitB, 1, true);
        if (AVC->SEI->PanScanRectFlag == true) {
            AVC->SEI->PanScanID                    = ReadExpGolomb(BitB, false);
        }
    }
    
    void ParseSEIFilmGrainCharacteristics(AVCFile *AVC, BitBuffer *BitB) { // film_grain_characteristics
        AVC->SEI->FilmGrainCharactisticsCancelFlag = ReadBits(BitB, 1, true);
        if (AVC->SEI->FilmGrainCharactisticsCancelFlag == false) {
            AVC->SEI->FilmGrainModelID                     = ReadBits(BitB, 2, true);
            AVC->SEI->SeperateColorDescriptionFlag         = ReadBits(BitB, 1, true);
            if (AVC->SEI->SeperateColorDescriptionFlag == true) {
                AVC->SEI->FilmGrainBitDepthLuma            = ReadBits(BitB, 3, true) + 8;
                AVC->SEI->FilmGrainBitDepthChroma          = ReadBits(BitB, 3, true) + 8;
                AVC->SEI->FilmGrainFullRangeFlag           = ReadBits(BitB, 1, true);
                AVC->SEI->FilmGrainColorPrimaries          = ReadBits(BitB, 8, true);
                AVC->SEI->FilmGrainTransferCharacteristics = ReadBits(BitB, 8, true);
                AVC->SEI->FilmGrainMatrixCoefficents       = ReadBits(BitB, 8, true);
            }
            AVC->SEI->BlendingModeID                       = ReadBits(BitB, 2, true);
            AVC->SEI->Scalefactor                          = ReadBits(BitB, 4, true);
            for (uint8_t Channel = 0; Channel < 3; Channel++) {
                AVC->SEI->CompModelPresent[Channel]   = ReadBits(BitB, 1, true);
            }
            for (uint8_t Channel = 0; Channel < 3; Channel++) {
                if (AVC->SEI->CompModelPresent[Channel] == true) {
                    AVC->SEI->NumIntensityIntervals[Channel]       = ReadBits(BitB, 8, true) + 1;
                    AVC->SEI->NumModelValues[Channel]              = ReadBits(BitB, 3, true) + 1;
                    for (uint16_t Intensity = 0; Intensity < AVC->SEI->NumIntensityIntervals[Channel]; Intensity++) {
                        AVC->SEI->IntensityIntervalLowerBound[Channel][Intensity] = ReadBits(BitB, 8, true);
                        AVC->SEI->IntensityIntervalUpperBound[Channel][Intensity] = ReadBits(BitB, 8, true);
                        for (uint8_t ModelValue = 0; ModelValue < AVC->SEI->NumModelValues[Channel]; ModelValue++) {
                            AVC->SEI->CompModelValue[Channel][Intensity][ModelValue] = ReadExpGolomb(BitB, true);
                        }
                    }
                }
            }
            AVC->SEI->FilmGrainCharacteristicsRepetitionPeriod = ReadExpGolomb(BitB, false);
        }
    }
    
    void ParseSEIDeblockingFilterDisplayPreference(AVCFile *AVC, BitBuffer *BitB) { // deblocking_filter_display_preference
        AVC->SEI->DeblockingDisplayPreferenceCancelFlag           = ReadBits(BitB, 1, true);
        if (AVC->SEI->DeblockingDisplayPreferenceCancelFlag == false) {
            AVC->SEI->DisplayBeforeDeblockingFlag                 = ReadBits(BitB, 1, true);
            AVC->SEI->DecodeFrameBufferingConstraintFlag          = ReadBits(BitB, 1, true);
            AVC->SEI->DeblockingDisplayPreferenceRepetitionPeriod = ReadExpGolomb(BitB, false);
        }
    }
    
    void ParseSEIStereoVideoInfo(AVCFile *AVC, BitBuffer *BitB) { // stereo_video_info
        AVC->SEI->FieldViewsFlag               = ReadBits(BitB, 1, true);
        if (AVC->SEI->FieldViewsFlag == true) {
            AVC->SEI->TopFieldIsLeftViewFlag       = ReadBits(BitB, 1, true);
        } else {
            AVC->SEI->CurrentFrameIsLeftViewFlag   = ReadBits(BitB, 1, true);
            AVC->SEI->NextFrameIsDependentViewFlag = ReadBits(BitB, 1, true);
        }
        AVC->SEI->LeftViewSelfContainedFlag    = ReadBits(BitB, 1, true);
        AVC->SEI->RightViewSelfContainedFlag   = ReadBits(BitB, 1, true);
    }
    
    void ParseSEIPostFilterHint(AVCFile *AVC, BitBuffer *BitB) { // post_filter_hint
        AVC->SEI->FilterHintSizeY     = ReadExpGolomb(BitB, false);
        AVC->SEI->FilterHintSizeX     = ReadExpGolomb(BitB, false);
        AVC->SEI->FilterHintType      = ReadBits(BitB, 2, true);
        for (uint8_t Color = 0; Color < 3; Color++) {
            for (uint8_t CY = 0; CY < AVC->SEI->FilterHintSizeY; CY++) {
                for (uint8_t CX = 0; CX < AVC->SEI->FilterHintSizeX; CX++) {
                    AVC->SEI->FilterHint[Color][CY][CX] = ReadExpGolomb(BitB, true);
                }
            }
        }
        AVC->SPS->AdditionalExtensionFlag = ReadBits(BitB, 1, true);
    }
    
    void ParseSEIToneMappingInfo(AVCFile *AVC, BitBuffer *BitB) { // tone_mapping_info
        AVC->SEI->ToneMapID  = ReadExpGolomb(BitB, false);
        AVC->SEI->ToneMapCancelFlag = ReadBits(BitB, 1, true);
        if (AVC->SEI->ToneMapCancelFlag == false) {
            AVC->SEI->ToneMapRepetitionPeriod = ReadExpGolomb(BitB, false);
            AVC->SEI->CodedDataBitDepth       = ReadBits(BitB, 8, true);
            AVC->SEI->TargetBitDepth   = ReadBits(BitB, 8, true);
            AVC->SEI->ToneMapModelID = ReadExpGolomb(BitB, false);
            if (AVC->SEI->ToneMapModelID == 0) {
                AVC->SEI->ToneMinValue = ReadBits(BitB, 32, true);
                AVC->SEI->ToneMaxValue = ReadBits(BitB, 32, true);
            } else if (AVC->SEI->ToneMapModelID == 1) {
                AVC->SEI->SigmoidMidpoint = ReadBits(BitB, 32, true);
                AVC->SEI->SigmoidWidth    = ReadBits(BitB, 32, true);
            } else if (AVC->SEI->ToneMapModelID == 2) {
                for (uint16_t I = 0; I < (1 << AVC->SEI->TargetBitDepth); I++) {
                    AVC->SEI->StartOfCodedInterval[I] = ReadBits(BitB, ((AVC->SEI->CodedDataBitDepth + 7) >> 3) << 3, true);
                }
            } else if (AVC->SEI->ToneMapModelID == 3) {
                AVC->SEI->NumPivots = ReadBits(BitB, 16, true);
                for (uint16_t I = 0; I < AVC->SEI->NumPivots; I++) {
                    AVC->SEI->CodedPivotValue[I] = ReadBits(BitB, ((AVC->SEI->CodedDataBitDepth + 7) >> 3) << 3, true);
                    AVC->SEI->TargetPivotValue[I] = ReadBits(BitB, ((AVC->SEI->TargetBitDepth + 7) >> 3) << 3, true);
                }
            } else if (AVC->SEI->ToneMapModelID == 4) {
                AVC->SEI->CameraISOSpeedIDC = ReadBits(BitB, 8, true);
                if (AVC->SEI->CameraISOSpeedIDC == ExtendedISO) {
                    AVC->SEI->CameraISOSpeed = ReadBits(BitB, 32, true);
                }
                AVC->SEI->ExposureIndexIDC = ReadBits(BitB, 8, true);
                if (AVC->SEI->ExposureIndexIDC == ExtendedISO) {
                    AVC->SEI->ExposureIndexValue = ReadBits(BitB, 32, true);
                }
                AVC->SEI->ExposureCompensationSignFlag = ReadBits(BitB, 1, true);
                AVC->SEI->ExposureCompensationNumerator = ReadBits(BitB, 16, true);
                AVC->SEI->ExposureCompensationDenominatorIDC = ReadBits(BitB, 16, true);
                AVC->SEI->RefScreenLuminanceWhite = ReadBits(BitB, 32, true);
                AVC->SEI->ExtendedRangeWhiteLevel = ReadBits(BitB, 32, true);
                AVC->SEI->NominalBlackLumaCode    = ReadBits(BitB, 16, true);
                AVC->SEI->NominalWhiteLumaCode    = ReadBits(BitB, 16, true);
                AVC->SEI->ExtendedWhiteLumaCode   = ReadBits(BitB, 16, true);
            }
        }
    }
    
    void ParseSEIScalabilityInfo(AVCFile *AVC, BitBuffer *BitB) { // scalability_info
        AVC->SEI->TemporalIDNestingFlag                      = ReadBits(BitB, 1, true);
        AVC->SEI->PriorityLayerInfoPresent                   = ReadBits(BitB, 1, true);
        AVC->SEI->PriorityIDSettingFlag                      = ReadBits(BitB, 1, true);
        AVC->SEI->NumLayers                                  = ReadExpGolomb(BitB, false) + 1;
        for (uint8_t Layer = 0; Layer < AVC->SEI->NumLayers; Layer++) {
            AVC->SEI->LayerID[Layer]                         = ReadExpGolomb(BitB, false);
            AVC->NAL->PriorityID[Layer]                      = ReadBits(BitB, 6, true);
            AVC->NAL->IsDisposable[Layer]                    = ReadBits(BitB, 1, true);
            AVC->NAL->DependencyID[Layer]                    = ReadBits(BitB, 3, true);
            AVC->NAL->QualityID[0][Layer]                    = ReadBits(BitB, 4, true); // FIXME; is this correct?
            AVC->NAL->TemporalID[Layer]                      = ReadBits(BitB, 3, true);
            AVC->SEI->SubPicLayerFlag[Layer]                 = ReadBits(BitB, 1, true);
            AVC->SEI->SubRegionLayerFlag[Layer]              = ReadBits(BitB, 1, true);
            AVC->SEI->IROIDivisionFlag[Layer]                = ReadBits(BitB, 1, true);
            AVC->SEI->ProfileLevelInfoPresentFlag[Layer]     = ReadBits(BitB, 1, true);
            AVC->SEI->BitRateInfoPresent[Layer]              = ReadBits(BitB, 1, true);
            AVC->SEI->FrameRateInfoPresent[Layer]            = ReadBits(BitB, 1, true);
            AVC->SEI->FrameSizeInfoPresentFlag[Layer]        = ReadBits(BitB, 1, true);
            AVC->SEI->LayerDependencyInfoPresent[Layer]      = ReadBits(BitB, 1, true);
            AVC->SEI->ParameterSetsInfoPresent[Layer]        = ReadBits(BitB, 1, true);
            AVC->SEI->BitstreamRestrictionInfoPresent[Layer] = ReadBits(BitB, 1, true);
            AVC->SEI->ExactInterLayerPredFlag[Layer]         = ReadBits(BitB, 1, true);
            if ((AVC->SEI->SubPicLayerFlag[Layer] == true) || (AVC->SEI->IROIDivisionFlag[Layer] == true)) {
                AVC->SEI->ExactSampleValueMatchFlag[Layer]   = ReadBits(BitB, 1, true);
            }
            AVC->SEI->LayerConversionFlag[Layer]             = ReadBits(BitB, 1, true);
            AVC->SEI->LayerOutputFlag[Layer]                 = ReadBits(BitB, 1, true);
            if (AVC->SEI->ProfileLevelInfoPresentFlag[Layer] == true) {
                AVC->SEI->LayerProfileLevelID[Layer]         = ReadBits(BitB, 24, true);
            }
            if (AVC->SEI->BitRateInfoPresent[Layer] == true) {
                AVC->SEI->AvgBitRate[Layer]                  = ReadBits(BitB, 16, true);
                AVC->SEI->MaxBitRate[Layer]                  = ReadBits(BitB, 16, true);
                AVC->SEI->MaxBitRateRepresentation[Layer]    = ReadBits(BitB, 16, true);
                AVC->SEI->MaxBitRateWindow[Layer]            = ReadBits(BitB, 16, true);
            }
            if (AVC->SEI->FrameRateInfoPresent[Layer] == true) {
                AVC->SEI->ConstantFrameRateIDC[Layer]        = ReadBits(BitB, 2, true);
                AVC->SEI->AvgFrameRate[Layer]                = ReadBits(BitB, 16, true);
            }
            if ((AVC->SEI->FrameSizeInfoPresentFlag[Layer] == true) || (AVC->SEI->IROIDivisionFlag[Layer] == true)) {
                AVC->SEI->FrameWidthInMacroBlocks[Layer]     = ReadExpGolomb(BitB, false) + 1;
                AVC->SEI->FrameHeightInMacroBlocks[Layer]    = ReadExpGolomb(BitB, false) + 1;
            }
            if (AVC->SEI->SubRegionLayerFlag[Layer] == true) {
                AVC->SEI->BaseRegionLayerID[Layer]           = ReadExpGolomb(BitB, false);
                AVC->SEI->DynamicRectFlag[Layer]             = ReadBits(BitB, 1, true);
                if (AVC->SEI->DynamicRectFlag[Layer] == false) {
                    AVC->SEI->HorizontalOffset[Layer]        = ReadBits(BitB, 16, true);
                    AVC->SEI->VerticalOffset[Layer]          = ReadBits(BitB, 16, true);
                    AVC->SEI->RegionWidth[Layer]             = ReadBits(BitB, 16, true);
                    AVC->SEI->RegionHeight[Layer]            = ReadBits(BitB, 16, true);
                }
            }
            if (AVC->SEI->SubPicLayerFlag[Layer] == true) {
                AVC->SEI->ROIID[Layer]                       = ReadExpGolomb(BitB, false);
            }
            if (AVC->SEI->IROIDivisionFlag[Layer] == true) {
                AVC->SEI->IROIGridFlag[Layer]                = ReadBits(BitB, 1, true);
                if (AVC->SEI->IROIGridFlag[Layer] == true) {
                    AVC->SEI->GridWidthInMacroBlocks[Layer]  = ReadExpGolomb(BitB, false) + 1;
                    AVC->SEI->GridHeightInMacroBlocks[Layer] = ReadExpGolomb(BitB, false) + 1;
                } else {
                    AVC->SEI->NumROIs[Layer]                       = ReadExpGolomb(BitB, false) + 1;
                    for (uint8_t ROI = 0; ROI < AVC->SEI->NumROIs[Layer]; ROI++) {
                        AVC->SEI->FirstMacroBlockInROI[Layer][ROI] = ReadExpGolomb(BitB, false);
                        AVC->SEI->ROIWidthInMacroBlock[Layer][ROI] = ReadExpGolomb(BitB, false) + 1;
                        AVC->SEI->ROIHeightInMacroBlock[Layer][ROI] = ReadExpGolomb(BitB, false) + 1;
                        
                    }
                }
            }
            if (AVC->SEI->LayerDependencyInfoPresent[Layer] == true) {
                AVC->SEI->NumDependentLayers[Layer]          = ReadExpGolomb(BitB, false);
                for (uint8_t DependentLayer = 0; DependentLayer < AVC->SEI->NumDependentLayers[Layer]; DependentLayer++) {
                    AVC->SEI->DirectlyDependentLayerIDDelta[Layer][DependentLayer] = ReadExpGolomb(BitB, false) + 1;
                }
            } else {
                AVC->SEI->LayerDepInfoSourceLayerIDDelta[Layer] = ReadExpGolomb(BitB, false) + 1;
                if (AVC->SEI->ParameterSetsInfoPresent[Layer] == true) {
                    AVC->SEI->NumSequenceParameterSets[Layer] = ReadExpGolomb(BitB, false);
                    for (uint8_t SPS = 0; SPS < AVC->SEI->NumSequenceParameterSets[Layer]; SPS++) {
                        AVC->SEI->SPSIDDelta[Layer][SPS] = ReadExpGolomb(BitB, false);
                    }
                    AVC->SEI->NumSubsetSPS[Layer]    = ReadExpGolomb(BitB, false);
                    for (uint8_t SubsetSPS = 0; SubsetSPS < AVC->SEI->NumSubsetSPS[Layer]; SubsetSPS++) {
                        AVC->SEI->SubsetSPSIDDelta[Layer][SubsetSPS] = ReadExpGolomb(BitB, false);
                    }
                    AVC->SEI->NumPicParameterSets[Layer] = ReadExpGolomb(BitB, false) + 1;
                    for (uint8_t PicParameterSet = 0; PicParameterSet < AVC->SEI->NumPicParameterSets[Layer]; PicParameterSet++) {
                        AVC->SEI->PicParameterSetIDDelta[Layer][PicParameterSet] = ReadExpGolomb(BitB, false);
                    }
                } else {
                    AVC->SEI->PPSInfoSrcLayerIDDelta[Layer] = ReadExpGolomb(BitB, false);
                }
                if (AVC->SEI->BitstreamRestrictionInfoPresent[Layer] == true) {
                    AVC->VUI->MotionVectorsOverPicBoundaries[Layer] = ReadExpGolomb(BitB, false);
                    AVC->VUI->MaxBytesPerPicDenom[Layer]            = ReadExpGolomb(BitB, false);
                    AVC->VUI->MaxBitsPerMarcoBlockDenom[Layer]      = ReadExpGolomb(BitB, false);
                    AVC->VUI->MaxMotionVectorLength[Layer]          = ReadExpGolomb(BitB, false);
                    AVC->VUI->MaxMotionVectorHeight[Layer]          = ReadExpGolomb(BitB, false);
                    AVC->VUI->MaxReorderFrames[Layer]               = ReadExpGolomb(BitB, false);
                    AVC->VUI->MaxFrameBuffer[Layer]                 = ReadExpGolomb(BitB, false);
                }
                if (AVC->SEI->LayerConversionFlag[Layer] == true) {
                    AVC->SEI->ConversionTypeIDC[Layer]              = ReadExpGolomb(BitB, false);
                    for (uint8_t J = 0; J < 2; J++) {
                        AVC->SEI->RewritingInfoFlag[Layer][J]       = ReadBits(BitB, 1, true);
                        if (AVC->SEI->RewritingInfoFlag[Layer][J] == true) {
                            AVC->SEI->RewritingProfileLevelIDC[Layer][J] = ReadBits(BitB, 24, true);
                            AVC->SEI->RewritingAverageBitrate[Layer][J]  = ReadBits(BitB, 16, true);
                            AVC->SEI->RewritingMaxBitrate[Layer][J]      = ReadBits(BitB, 16, true);
                        }
                    }
                }
            }
        }
        if (AVC->SEI->PriorityLayerInfoPresent == true) {
            AVC->SEI->NumDependencyLayersForPriorityLayer = ReadBits(BitB, 1, true) + 1;
            for (uint8_t DependentLayer = 0; DependentLayer < AVC->SEI->NumDependencyLayersForPriorityLayer; DependentLayer++) {
                AVC->SEI->PriorityDependencyID[DependentLayer] = ReadBits(BitB, 3, true);
                AVC->SEI->NumPriorityLayers[DependentLayer]    = ReadExpGolomb(BitB, false) + 1;
                for (uint8_t PriorityLayer = 0; PriorityLayer < AVC->SEI->NumPriorityLayers[DependentLayer]; PriorityLayer++) {
                    AVC->SEI->PriorityLayerID[DependentLayer][PriorityLayer]   = ReadExpGolomb(BitB, false);
                    AVC->SEI->PriorityLevelIDC[DependentLayer][PriorityLayer]  = ReadBits(BitB, 24, true);
                    AVC->SEI->PriorityLevelAvgBitrate[DependentLayer][PriorityLayer]  = ReadBits(BitB, 16, true);
                    AVC->SEI->PriorityLevelMaxBitrate[DependentLayer][PriorityLayer]  = ReadBits(BitB, 16, true);
                }
            }
        }
        if (AVC->SEI->PriorityIDSettingFlag == true) {
            AVC->SEI->PriorityIDURIIndex = 0;
            AVC->SEI->PriorityIDSettingURI = ReadBits(BitB, 8, true);
            for (uint8_t StringByte = AVC->SEI->PriorityIDSettingURI; StringByte > 0; StringByte--) {
                AVC->SEI->PriorityIDSettingURIString[StringByte] = ReadBits(BitB, 8, true);
            }
        }
    }
    
    void ParseSEISubPictureScalableLayer(AVCFile *AVC, BitBuffer *BitB) { // sub_pic_scalable_layer
        AVC->SEI->LayerID[0] = ReadExpGolomb(BitB, false);
    }
    
    void ParseSEINonRequiredLayerRep(AVCFile *AVC, BitBuffer *BitB) { // non_required_layer_rep
        AVC->SEI->NumInfoEntries = ReadExpGolomb(BitB, false) + 1;
        for (uint32_t InfoEntry = 0; InfoEntry < AVC->SEI->NumInfoEntries; InfoEntry++) {
            AVC->SEI->EntryDependencyID[InfoEntry] = ReadBits(BitB, 3, true);
            AVC->SEI->NumNonRequiredLayerReps[InfoEntry] = ReadExpGolomb(BitB, false) + 1;
            for (uint8_t LayerRep = 0; LayerRep < AVC->SEI->NumNonRequiredLayerReps[InfoEntry]; LayerRep++) {
                AVC->SEI->NonRequiredLayerRepDependencyID[InfoEntry][LayerRep] = ReadBits(BitB, 3, true);
                AVC->SEI->NonRequiredLayerRepQualityID[InfoEntry][LayerRep] = ReadBits(BitB, 4, true);
            }
        }
    }
    
    void ParseSEIPriorityLayerInfo(AVCFile *AVC, BitBuffer *BitB) { // priority_layer_info
        AVC->SEI->PriorityDependencyID[0] = ReadBits(BitB, 3, true);
        AVC->SEI->NumPriorityIDs          = ReadBits(BitB, 4, true);
        for (uint8_t PriorityID = 0; PriorityID < AVC->SEI->NumPriorityIDs; PriorityID++) {
            AVC->SEI->AltPriorityIDs[PriorityID] = ReadBits(BitB, 6, true);
        }
    }
    
    void ParseSEILayersNotPresent(AVCFile *AVC, BitBuffer *BitB) { // layers_not_present
        AVC->SEI->NumLayersNotPresent = ReadExpGolomb(BitB, false);
        for (uint8_t Layer = 0; Layer < AVC->SEI->NumLayersNotPresent; Layer++) {
            AVC->SEI->LayerID[Layer] = ReadExpGolomb(BitB, false);
        }
    }
    
    void ParseSEILayerDependencyChange(AVCFile *AVC, BitBuffer *BitB) { // layer_dependency_change
        AVC->SEI->NumLayers = ReadExpGolomb(BitB, false) + 1;
        for (uint8_t Layer = 0; Layer < AVC->SEI->NumLayers; Layer++) {
            AVC->SEI->LayerID[Layer] = ReadExpGolomb(BitB, false);
            AVC->SEI->LayerDependencyInfoPresent[Layer] = ReadBits(BitB, 1, true);
            if (AVC->SEI->LayerDependencyInfoPresent[Layer] == true) {
                AVC->SEI->NumDependentLayers[Layer] = ReadExpGolomb(BitB, false);
                for (uint8_t DependentLayer = 0; DependentLayer < AVC->SEI->NumDependentLayers[Layer]; DependentLayer++) {
                    AVC->SEI->DirectlyDependentLayerIDDelta[Layer][DependentLayer] = ReadExpGolomb(BitB, false) + 1;
                }
            } else {
                AVC->SEI->LayerDepInfoSourceLayerIDDelta[Layer] = ReadExpGolomb(BitB, false) + 1;
            }
        }
    }
    
    void ParseSEIScalableNesting(AVCFile *AVC, BitBuffer *BitB) { // scalable_nesting
        AVC->SEI->AllLayerRepresentationsInAUFlag = ReadBits(BitB, 1, true);
        if (AVC->SEI->AllLayerRepresentationsInAUFlag == false) {
            AVC->SEI->NumLayerRepresentations = ReadExpGolomb(BitB, false) + 1;
            for (uint8_t LayerRepresentation = 0; LayerRepresentation < AVC->SEI->NumLayerRepresentations; LayerRepresentation++) {
                AVC->SEI->SEIDependencyID[LayerRepresentation] = ReadBits(BitB, 3, true);
                AVC->SEI->SEIQualityID[LayerRepresentation]    = ReadBits(BitB, 4, true);
            }
            AVC->SEI->SEITemporalID[0] = ReadBits(BitB, 3, true);
        }
        AlignInput(BitB, 1);
        ParseSEIMessage(AVC, BitB); // sei_message();
    }
    
    void ParseSEIBaseLayerTemporalHRD(AVCFile *AVC, BitBuffer *BitB) { // base_layer_temporal_hrd
        AVC->SEI->NumTemporalLayersInBaseLayer                 = ReadExpGolomb(BitB, false) + 1;
        for (uint8_t TemporalLayer = 0; TemporalLayer < AVC->SEI->NumTemporalLayersInBaseLayer; TemporalLayer++) {
            AVC->SEI->SEITemporalID[TemporalLayer]             = ReadBits(BitB, 3, true);
            AVC->SEI->SEITimingInfoPresent[TemporalLayer]      = ReadBits(BitB, 1, true);
            if (AVC->SEI->SEITimingInfoPresent[TemporalLayer] == true) {
                AVC->SEI->SEIUnitsInTick[TemporalLayer]        = ReadBits(BitB, 32, true);
                AVC->SEI->SEITimeScale[TemporalLayer]          = ReadBits(BitB, 32, true);
                AVC->SEI->SEIFixedFrameRateFlag[TemporalLayer] = ReadBits(BitB, 1, true);
            }
            AVC->SEI->SEINALHRDParamsPresentFlag[TemporalLayer] = ReadBits(BitB, 1, true);
            if (AVC->SEI->SEINALHRDParamsPresentFlag[TemporalLayer] == true) {
                ParseHypotheticalReferenceDecoder(AVC, BitB);
            }
            AVC->SEI->SEIVCLHRDParamsPresentFlag[TemporalLayer] = ReadBits(BitB, 1, true);
            if (AVC->SEI->SEIVCLHRDParamsPresentFlag[TemporalLayer] == true) {
                ParseHypotheticalReferenceDecoder(AVC, BitB);
            }
            if ((AVC->SEI->SEINALHRDParamsPresentFlag[TemporalLayer] == true) || (AVC->SEI->SEIVCLHRDParamsPresentFlag[TemporalLayer] == true)) {
                AVC->SEI->SEILowDelayHRDFlag[TemporalLayer] = ReadBits(BitB, 1, true);
            }
            AVC->SEI->SEIPicStructPresentFlag[TemporalLayer] = ReadBits(BitB, 1, true);
        }
    }
    
    void ParseSEIQualityLayerIntegrityCheck(AVCFile *AVC, BitBuffer *BitB) { // quality_layer_integrity_check
        AVC->SEI->NumInfoEntries = ReadExpGolomb(BitB, false) + 1;
        for (uint8_t IntegrityCheck = 0; IntegrityCheck < AVC->SEI->NumInfoEntries; IntegrityCheck++) {
            AVC->SEI->EntryDependencyID[IntegrityCheck] = ReadBits(BitB, 3, true);
            AVC->SEI->SEIQualityLayerCRC[IntegrityCheck] = ReadBits(BitB, 16, true);
        }
    }
    
    void ParseSEIRedundantPicProperty(AVCFile *AVC, BitBuffer *BitB) { // redundant_pic_property
        AVC->SEI->NumDependencyIDs = ReadExpGolomb(BitB, false) + 1;
        for (uint8_t DependencyID = 0; DependencyID < AVC->SEI->NumDependencyIDs; DependencyID++) {
            AVC->NAL->DependencyID[DependencyID]  = ReadBits(BitB, 3, true);
            AVC->SEI->NumQualityIDs[DependencyID] = ReadExpGolomb(BitB, false) + 1;
            for (uint8_t QualityID = 0; QualityID < AVC->SEI->NumQualityIDs[DependencyID]; QualityID++) {
                AVC->NAL->QualityID[DependencyID][QualityID] = ReadBits(BitB, 4, true);
                AVC->SEI->NumRedundantPics[DependencyID][QualityID] = ReadExpGolomb(BitB, false) + 1;
                for (uint8_t RedundantPic = 0; RedundantPic < AVC->SEI->NumRedundantPics[DependencyID][QualityID]; RedundantPic++) {
                    AVC->SEI->RedundantPicCount[DependencyID][QualityID][RedundantPic] = ReadExpGolomb(BitB, false) + 1;
                    AVC->SEI->RedundantPicsMatch[DependencyID][QualityID][RedundantPic] = ReadBits(BitB, 1, true);
                    if (AVC->SEI->RedundantPicsMatch[DependencyID][QualityID][RedundantPic] == false) {
                        AVC->SEI->MBTypeMatchFlag[DependencyID][QualityID][RedundantPic] = ReadBits(BitB, 1, true);
                        AVC->SEI->MotionMatchFlag[DependencyID][QualityID][RedundantPic] = ReadBits(BitB, 1, true);
                        AVC->SEI->ResidualMatchFlag[DependencyID][QualityID][RedundantPic] = ReadBits(BitB, 1, true);
                        AVC->SEI->IntraSamplesMatchFlag[DependencyID][QualityID][RedundantPic] = ReadBits(BitB, 1, true);
                    }
                }
            }
        }
    }
    
    void ParseSEITemporalDependencyRepresentationIndex(AVCFile *AVC, BitBuffer *BitB) { // tl0_dep_rep_index
        AVC->SEI->TemporalDependencyRepresentationIndexLevel0 = ReadBits(BitB, 8, true);
        AVC->SEI->EffectiveIDRPicID                           = ReadBits(BitB, 16, true);
    }
    
    void ParseSEITemporalLevelSwitchingPoint(AVCFile *AVC, BitBuffer *BitB) { // tl_switching_point
        AVC->SEI->DeltaFrameNum = ReadExpGolomb(BitB, true);
    }
    
    void ParseSEIParallelDecodingInfo(AVCFile *AVC, BitBuffer *BitB) { // parallel_decoding_info
        AVC->SPS->SeqParamSetID = ReadExpGolomb(BitB, false);
        for (uint8_t View = 0; View <= AVC->SPS->ViewCount; View++) {
            if (AVC->NAL->IsAnchorPicture == true) {
                for (uint8_t AnchorRef = 0; AnchorRef < AVC->SPS->AnchorRefsCount[0][View]; AnchorRef++) {
                    AVC->SEI->PDIInitDelayAnchor[0][View][AnchorRef] = ReadExpGolomb(BitB, false) + 2;
                }
                for (uint8_t AnchorRef = 0; AnchorRef < AVC->SPS->AnchorRefsCount[1][View]; AnchorRef++) {
                    AVC->SEI->PDIInitDelayAnchor[1][View][AnchorRef] = ReadExpGolomb(BitB, false) + 2;
                }
            } else {
                for (uint8_t NonAnchorRef = 0; NonAnchorRef < AVC->SPS->NonAnchorRefCount[0][View]; NonAnchorRef++) {
                    AVC->SEI->PDIInitDelayNonAnchor[0][View][NonAnchorRef] = ReadExpGolomb(BitB, false) + 2;
                }
                for (uint8_t NonAnchorRef = 0; NonAnchorRef < AVC->SPS->NonAnchorRefCount[1][View]; NonAnchorRef++) {
                    AVC->SEI->PDIInitDelayNonAnchor[1][View][NonAnchorRef] = ReadExpGolomb(BitB, false) + 2;
                }
            }
        }
    }
    
    void ParseSEIMVCScalableNesting(AVCFile *AVC, BitBuffer *BitB) { // mvc_scalable_nesting
        AVC->SEI->OperationPointFlag = ReadBits(BitB, 1, true);
        if (AVC->SEI->OperationPointFlag == false) {
            AVC->SEI->AllViewComponentsInAUFlag = ReadBits(BitB, 1, true);
            if (AVC->SEI->AllViewComponentsInAUFlag == false) {
                AVC->SEI->NumViewComponents = ReadExpGolomb(BitB, false) + 1;
                for (uint8_t ViewComponent = 0; ViewComponent < AVC->SEI->NumViewComponents; ViewComponent++) {
                    AVC->SEI->SEIViewID[ViewComponent] = ReadBits(BitB, 10, true);
                }
            }
        } else {
            AVC->SEI->NumViewComponentsOp = ReadExpGolomb(BitB, false) + 1;
            for (uint8_t ViewComponent = 0; ViewComponent < AVC->SEI->NumViewComponentsOp; ViewComponent++) {
                AVC->SEI->SEIOpViewID[ViewComponent] = ReadBits(BitB, 10, true);
            }
            AVC->SEI->SEIOpTemporalID = ReadBits(BitB, 3, true);
        }
        AlignInput(BitB, 1);
        ParseSEIMessage(AVC, BitB); // sei_message();
    }
    
    void ParseSEIViewScalabilityInfo(AVCFile *AVC, BitBuffer *BitB) { // view_scalability_info FIXME: FINISH THIS FUNCTION!!!
        AVC->SEI->NumOperationPoints                         = ReadExpGolomb(BitB, false) + 1;
        for (uint8_t OperationPoint = 0; OperationPoint < AVC->SEI->NumOperationPoints; OperationPoint++) {
            AVC->SEI->OperationPointID[OperationPoint]       = ReadExpGolomb(BitB, false);
            AVC->NAL->PriorityID[OperationPoint]             = ReadBits(BitB, 5, true);
            AVC->NAL->TemporalID[OperationPoint]             = ReadBits(BitB, 3, true);
            AVC->SEI->NumTargetOutputViews[OperationPoint]   = ReadExpGolomb(BitB, false) + 1;
            for (uint8_t OutputView = 0; OutputView < AVC->SEI->NumTargetOutputViews[OperationPoint]; OutputView++) {
                AVC->SPS->ViewID[OperationPoint][OutputView] = ReadExpGolomb(BitB, false);
            }
            AVC->SEI->ProfileLevelInfoPresentFlag[0]         = ReadBits(BitB, 1, true);
            AVC->SEI->BitRateInfoPresent[0]                  = ReadBits(BitB, 1, true);
            AVC->SEI->FrameRateInfoPresent[0]                = ReadBits(BitB, 1, true);
            if (AVC->SEI->NumTargetOutputViews[OperationPoint] == false) {
                AVC->SEI->ViewDependencyInfoPresent[OperationPoint] = ReadBits(BitB, 1, true);
            }
            AVC->SEI->ParameterSetsInfoPresent[OperationPoint]        = ReadBits(BitB, 1, true);
            AVC->SEI->BitstreamRestrictionInfoPresent[OperationPoint] = ReadBits(BitB, 1, true);
            if (AVC->SEI->ProfileLevelInfoPresentFlag[0] == true) {
                AVC->SEI->OpProfileLevelIDC[OperationPoint]           = ReadBits(BitB, 24, true);
            }
            if (AVC->SEI->BitstreamRestrictionInfoPresent[OperationPoint] == true) {
                AVC->SEI->AvgBitRate[OperationPoint]                  = ReadBits(BitB, 16, true);
                AVC->SEI->MaxBitRate[OperationPoint]                  = ReadBits(BitB, 16, true);
                AVC->SEI->MaxBitRateWindow[OperationPoint]            = ReadBits(BitB, 16, true);
            }
            if (AVC->SEI->FrameRateInfoPresent[0] == true) {
                AVC->SEI->ConstantFrameRateIDC[OperationPoint]        = ReadBits(BitB, 2, true);
                AVC->SEI->AvgFrameRate[OperationPoint]                = ReadBits(BitB, 16, true);
            }
            if (AVC->SEI->ViewDependencyInfoPresent[OperationPoint] == true) {
                AVC->SEI->NumDirectlyDependentViews[OperationPoint]   = ReadExpGolomb(BitB, false);
                for (uint8_t DirectDependentView = 0; DirectDependentView < AVC->SEI->NumDirectlyDependentViews[OperationPoint]; DirectDependentView++) {
                    AVC->SEI->DirectlyDependentViewID[OperationPoint][DirectDependentView] = ReadExpGolomb(BitB, false);
                }
            } else {
                AVC->SEI->ViewDependencyInfoSrcOpID[OperationPoint]   = ReadExpGolomb(BitB, false);
            }
            if (AVC->SEI->ParameterSetsInfoPresent[OperationPoint] == true) {
                AVC->SEI->NumSequenceParameterSets[OperationPoint] = ReadExpGolomb(BitB, false);
                for (uint8_t SPS = 0; SPS < AVC->SEI->NumSequenceParameterSets[OperationPoint]; SPS++) {
                    AVC->SEI->SPSIDDelta[OperationPoint][SPS] = ReadExpGolomb(BitB, false);
                }
                AVC->SEI->NumSubsetSPS[OperationPoint] = ReadExpGolomb(BitB, false);
                for (uint8_t SubsetSPS = 0; SubsetSPS < AVC->SEI->NumSubsetSPS[OperationPoint]; SubsetSPS++) {
                    AVC->SEI->SubsetSPSIDDelta[OperationPoint][SubsetSPS] = ReadExpGolomb(BitB, false);
                }
                AVC->SEI->NumPicParameterSets[OperationPoint] = ReadExpGolomb(BitB, false) + 1;
                for (uint8_t PPS = 0; PPS < AVC->SEI->NumPicParameterSets[OperationPoint]; PPS++) {
                    AVC->SEI->PicParameterSetIDDelta[OperationPoint][PPS] = ReadExpGolomb(BitB, false);
                }
            } else {
                AVC->SEI->PPSInfoSrcLayerIDDelta[OperationPoint] = ReadExpGolomb(BitB, false);
            }
            if (AVC->SEI->BitstreamRestrictionInfoPresent[OperationPoint] == true) {
                AVC->VUI->MotionVectorsOverPicBoundaries[OperationPoint]  = ReadBits(BitB, 1, true);
                AVC->VUI->MaxBytesPerPicDenom[OperationPoint]             = ReadExpGolomb(BitB, false);
                AVC->VUI->MaxBitsPerMarcoBlockDenom[OperationPoint]       = ReadExpGolomb(BitB, false);
                AVC->VUI->MaxMotionVectorLength[OperationPoint]           = ReadExpGolomb(BitB, false);
                AVC->VUI->MaxMotionVectorHeight[OperationPoint]           = ReadExpGolomb(BitB, false);
                AVC->VUI->MaxReorderFrames[OperationPoint]                = ReadExpGolomb(BitB, false);
                AVC->VUI->MaxFrameBuffer[OperationPoint]                  = ReadExpGolomb(BitB, false);
            }
            if (AVC->SEI->LayerConversionFlag[OperationPoint] == true) {
                AVC->SEI->ConversionTypeIDC[OperationPoint] = ReadExpGolomb(BitB, false);
                for (uint8_t J = 0; J < 2; J++) {
                    AVC->SEI->RewritingInfoFlag[OperationPoint][J] = ReadBits(BitB, 1, true);
                    for (uint8_t RewriteInfo = 0; RewriteInfo < AVC->SEI->RewritingInfoFlag[OperationPoint][J]; RewriteInfo++) {
                        AVC->SEI->RewritingProfileLevelIDC[OperationPoint][J] = ReadBits(BitB, 24, true);
                        AVC->SEI->RewritingAverageBitrate[OperationPoint][J]  = ReadBits(BitB, 16, true);
                        AVC->SEI->RewritingMaxBitrate[OperationPoint][J]      = ReadBits(BitB, 16, true);
                    }
                }
            }
        }
        if (AVC->SEI->PriorityLayerInfoPresent == true) {
            AVC->SEI->NumDependencyLayersForPriorityLayer                             = ReadExpGolomb(BitB, false) + 1;
            for (uint8_t DependencyLayer = 0; DependencyLayer < AVC->SEI->NumDependencyLayersForPriorityLayer; DependencyLayer++) {
                AVC->SEI->PriorityDependencyID[DependencyLayer]                       = ReadBits(BitB, 3, true);
                AVC->SEI->NumPriorityLayers[DependencyLayer]                          = ReadExpGolomb(BitB, false) + 1;
                for (uint8_t PriorityLayer = 0; PriorityLayer < AVC->SEI->NumPriorityLayers[DependencyLayer]; PriorityLayer++) {
                    AVC->SEI->PriorityLayerID[DependencyLayer][PriorityLayer]         = ReadExpGolomb(BitB, false);
                    AVC->SEI->PriorityLevelIDC[DependencyLayer][PriorityLayer]        = ReadBits(BitB, 24, true);
                    AVC->SEI->PriorityLevelAvgBitrate[DependencyLayer][PriorityLayer] = ReadBits(BitB, 16, true);
                    AVC->SEI->PriorityLevelMaxBitrate[DependencyLayer][PriorityLayer] = ReadBits(BitB, 16, true);
                }
            }
        }
        if (AVC->SEI->PriorityIDSettingFlag == true) {
            AVC->SEI->PriorityIDURIIndex = 0;
            AVC->SEI->PriorityIDSettingURI = ReadBits(BitB, 8, true);
            for (uint8_t StringByte = AVC->SEI->PriorityIDSettingURI; StringByte > 0; StringByte--) {
                AVC->SEI->PriorityIDSettingURIString[StringByte] = ReadBits(BitB, 8, true);
            }
        }
    }
    
    void ParseSEIMVCSceneInfo(AVCFile *AVC, BitBuffer *BitB) { // multiview_scene_info
        AVC->SEI->MaxDisparity = ReadExpGolomb(BitB, false);
    }
    
    void ParseSEIMVCAcquisitionInfo(AVCFile *AVC, BitBuffer *BitB) { // multiview_acquisition_info
        AVC->SPS->ViewCount            = ReadExpGolomb(BitB, false) + 1;
        AVC->SEI->IntrinsicParamFlag   = ReadBits(BitB, 1, true);
        AVC->SEI->ExtrinsicParamFlag   = ReadBits(BitB, 1, true);
        if (AVC->SEI->IntrinsicParamFlag == true) {
            AVC->SEI->IntrinsicParamsEqual = ReadBits(BitB, 1, true);
            AVC->SEI->PrecFocalLength      = ReadExpGolomb(BitB, false);
            AVC->SEI->PrecPrincipalPoint   = ReadExpGolomb(BitB, false);
            AVC->SEI->PrecSkewFactor       = ReadExpGolomb(BitB, false);
            if (AVC->SEI->IntrinsicParamsEqual == true) {
                AVC->SEI->NumParamSets     = 1;
            } else {
                AVC->SEI->NumParamSets     = AVC->SPS->ViewCount;
            }
            for (uint8_t ParamSet = 0; ParamSet < AVC->SEI->NumParamSets; ParamSet++) { // FIXME: ReadBits(BitB, 0, true)
                AVC->SEI->SignFocalLength[0][ParamSet]        = ReadBits(BitB, 1, true);
                AVC->SEI->ExponentFocalLength[0][ParamSet]    = ReadBits(BitB, 6, true);
                if (AVC->SEI->ExponentFocalLength[0][ParamSet] == 0) {
                    AVC->SEI->MantissaFocalLength[0][ParamSet] = ReadBits(BitB, Max(0, AVC->SEI->PrecFocalLength - 30), true);
                } else {
                    AVC->SEI->MantissaFocalLength[0][ParamSet] = ReadBits(BitB, Max(0, AVC->SEI->ExponentFocalLength[0][ParamSet] + AVC->SEI->PrecFocalLength - 31), true);
                }
                AVC->SEI->SignFocalLength[1][ParamSet]        = ReadBits(BitB, 1, true);
                AVC->SEI->ExponentFocalLength[1][ParamSet]    = ReadBits(BitB, 6, true);
                if (AVC->SEI->ExponentFocalLength[1][ParamSet] == 0) {
                    AVC->SEI->MantissaFocalLength[1][ParamSet] = ReadBits(BitB, Max(0, AVC->SEI->PrecFocalLength - 30), true);
                } else {
                    AVC->SEI->MantissaFocalLength[1][ParamSet] = ReadBits(BitB, Max(0, AVC->SEI->ExponentFocalLength[1][ParamSet] + AVC->SEI->PrecFocalLength - 31), true);
                }
                AVC->SEI->SignPrincipalPoint[0][ParamSet]     = ReadBits(BitB, 1, true);
                AVC->SEI->ExponentPrincipalPoint[0][ParamSet] = ReadBits(BitB, 6, true);
                if (AVC->SEI->ExponentFocalLength[1][ParamSet] == 0) {
                    AVC->SEI->MantissaPrincipalPoint[0][ParamSet] = ReadBits(BitB, Max(0, AVC->SEI->PrecPrincipalPoint - 30), true);
                } else {
                    AVC->SEI->MantissaPrincipalPoint[0][ParamSet] = ReadBits(BitB, Max(0, AVC->SEI->ExponentPrincipalPoint[0][ParamSet] + AVC->SEI->PrecPrincipalPoint - 31), true);
                }
                AVC->SEI->SignPrincipalPoint[1][ParamSet]     = ReadBits(BitB, 1, true);
                AVC->SEI->ExponentPrincipalPoint[1][ParamSet] = ReadBits(BitB, 6, true);
                if (AVC->SEI->ExponentFocalLength[1][ParamSet] == 0) {
                    AVC->SEI->MantissaPrincipalPoint[1][ParamSet] = ReadBits(BitB, Max(0, AVC->SEI->PrecPrincipalPoint - 30), true);
                } else {
                    AVC->SEI->MantissaPrincipalPoint[1][ParamSet] = ReadBits(BitB, Max(0, AVC->SEI->ExponentPrincipalPoint[1][ParamSet] + AVC->SEI->PrecPrincipalPoint - 31), true);
                }
                AVC->SEI->SignSkewFactor[ParamSet]            = ReadBits(BitB, 1, true);
                AVC->SEI->ExponentSkewFactor[ParamSet]        = ReadBits(BitB, 6, true);
                if (AVC->SEI->ExponentSkewFactor[ParamSet] == 0) {
                    AVC->SEI->MantissaSkewFactor[ParamSet] = ReadBits(BitB, Max(0, AVC->SEI->PrecSkewFactor - 30), true);
                } else {
                    AVC->SEI->MantissaSkewFactor[ParamSet] = ReadBits(BitB, Max(0, AVC->SEI->ExponentSkewFactor[ParamSet] + AVC->SEI->PrecSkewFactor - 31), true);
                }
            }
        }
        if (AVC->SEI->ExtrinsicParamFlag == true) {
            for (uint8_t View = 0; View < AVC->SPS->ViewCount; View++) {
                for (uint8_t Row = 1; Row <= 3; Row++) {
                    for (uint8_t Column = 1; Column <= 3; Column++) {
                        AVC->SEI->SignR[View][Row][Column]     = ReadBits(BitB, 1, true);
                        AVC->SEI->ExponentR[View][Row][Column] = ReadBits(BitB, 6, true);
                        AVC->SEI->MantissaR[View][Row][Column] = ReadBits(BitB, 0, true);
                    }
                    AVC->SEI->SignT[View][Row] = ReadBits(BitB, 1, true);
                    AVC->SEI->ExponentT[View][Row] = ReadBits(BitB, 6, true);
                    AVC->SEI->MantissaT[View][Row] = ReadBits(BitB, 0, true);
                }
            }
        }
    }
    
    void ParseSEINonRequiredViewComponent(AVCFile *AVC, BitBuffer *BitB) { // non_required_view_component
        AVC->SEI->NumInfoEntries                               = ReadExpGolomb(BitB, false) + 1;
        for (uint8_t InfoEntry = 0; InfoEntry < AVC->SEI->NumInfoEntries; InfoEntry++) {
            AVC->SEI->ViewOrderIndex[InfoEntry]                = ReadExpGolomb(BitB, false);
            AVC->SEI->NumNonRequiredViewComponents[InfoEntry]  = ReadExpGolomb(BitB, false) + 1;
            for (uint8_t ViewComponent = 0; ViewComponent < AVC->SEI->NumNonRequiredViewComponents[InfoEntry]; ViewComponent++) {
                AVC->SEI->IndexDelta[InfoEntry][ViewComponent] = ReadExpGolomb(BitB, false) + 1;
            }
        }
    }
    
    void ParseSEIViewDependencyChange(AVCFile *AVC, BitBuffer *BitB) { // view_dependency_change
        AVC->SPS->SeqParamSetID               = ReadExpGolomb(BitB, false);
        AVC->SEI->AnchorUpdateFlag    = ReadBits(BitB, 1, true);
        AVC->SEI->NonAnchorUpdateFlag = ReadBits(BitB, 1, true);
        if (AVC->SEI->AnchorUpdateFlag == true) {
            for (uint8_t View = 0; View < AVC->SPS->ViewCount; View++) {
                for (uint8_t AnchorRef = 0; AnchorRef < AVC->SPS->AnchorRefsCount[0][View]; AnchorRef++) {
                    AVC->SEI->AnchorRefFlag[0][View][AnchorRef] = ReadBits(BitB, 1, true);
                }
                for (uint8_t AnchorRef = 0; AnchorRef < AVC->SPS->AnchorRefsCount[1][View]; AnchorRef++) {
                    AVC->SEI->AnchorRefFlag[1][View][AnchorRef] = ReadBits(BitB, 1, true);
                }
            }
        }
        if (AVC->SEI->NonAnchorUpdateFlag == true) {
            for (uint8_t View = 0; View < AVC->SPS->ViewCount; View++) {
                for (uint8_t NonAnchorRef = 0; NonAnchorRef < AVC->SPS->NonAnchorRefCount[0][View]; NonAnchorRef++) {
                    AVC->SEI->NonAnchorRefFlag[0][View][NonAnchorRef] = ReadBits(BitB, 1, true);
                }
                for (uint8_t NonAnchorRef = 0; NonAnchorRef < AVC->SPS->NonAnchorRefCount[1][View]; NonAnchorRef++) {
                    AVC->SEI->NonAnchorRefFlag[1][View][NonAnchorRef] = ReadBits(BitB, 1, true);
                }
            }
        }
    }
    
    void ParseSEIOperationPointNotPresent(AVCFile *AVC, BitBuffer *BitB) { // operation_point(s)_not_present
        AVC->SEI->NumOperationPoints = ReadExpGolomb(BitB, false) + 1;
        for (uint8_t OperationPoint = 0; OperationPoint < AVC->SEI->NumOperationPoints; OperationPoint++) {
            AVC->SEI->OperationPointNotPresentID[OperationPoint] = ReadExpGolomb(BitB, false);
        }
    }
    
    void ParseSEIBaseViewTemporalHRD(AVCFile *AVC, BitBuffer *BitB) { // base_view_temporal_hrd
        AVC->SEI->NumTemporalLayersInBaseView = ReadExpGolomb(BitB, false) + 1;
        for (uint8_t TemporalLayer = 0; TemporalLayer < AVC->SEI->NumTemporalLayersInBaseView; TemporalLayer++) {
            AVC->SEI->SEIMVCTemporalID[TemporalLayer] = ReadBits(BitB, 3, true);
        }
    }
    
    void ParseSEIFramePackingArrangement(AVCFile *AVC, BitBuffer *BitB) { // frame_packing_arrangement
        AVC->SEI->FramePackingArrangementID = ReadExpGolomb(BitB, false);
        AVC->SEI->FramePackingArrangementCancelFlag = ReadBits(BitB, 1, true);
        if (AVC->SEI->FramePackingArrangementCancelFlag == false) {
            AVC->SEI->FramePackingArrangementType = ReadBits(BitB, 7, true);
            AVC->SEI->QuincunxSamplingFlag = ReadBits(BitB, 1, true);
            AVC->SEI->ContentIntrepretationType = ReadBits(BitB, 6, true);
            AVC->SEI->SpatialFlippingFlag = ReadBits(BitB, 1, true);
            AVC->SEI->FlippedFlagFrame0 = ReadBits(BitB, 1, true);
            AVC->SEI->FieldViewsFlag = ReadBits(BitB, 1, true);
            AVC->SEI->TheCurrentFrameIsFrame0 = ReadBits(BitB, 1, true);
            AVC->SEI->Frame0IsSelfContainedFlag = ReadBits(BitB, 1, true);
            AVC->SEI->Frame1IsSelfContainedFlag = ReadBits(BitB, 1, true);
            if ((AVC->SEI->QuincunxSamplingFlag == false) && (AVC->SEI->FramePackingArrangementType != 5)) {
                AVC->SEI->Frame0GridPositionX = ReadBits(BitB, 4, true);
                AVC->SEI->Frame0GridPositionY = ReadBits(BitB, 4, true);
                AVC->SEI->Frame1GridPositionX = ReadBits(BitB, 4, true);
                AVC->SEI->Frame1GridPositionY = ReadBits(BitB, 4, true);
            }
            AVC->SEI->FramePackingArrangementReservedByte = ReadBits(BitB, 8, true);
            AVC->SEI->FramePackingArrangementRepetitionPeriod = ReadExpGolomb(BitB, false);
        }
        AVC->SEI->FramePackingArrangementExtensionFlag = ReadBits(BitB, 1, true);
    }
    
    void ParseSEIMVCViewPosition(AVCFile *AVC, BitBuffer *BitB) { // multiview_view_position
        AVC->SPS->ViewCount = ReadExpGolomb(BitB, false) + 1;
        for (uint8_t View = 0; View < AVC->SPS->ViewCount; View++) {
            AVC->SEI->ViewPosition[View] = ReadExpGolomb(BitB, false);
        }
        AVC->SEI->MVCViewPositionExtensionFlag = ReadBits(BitB, 1, true);
    }
    
    void ParseSEIDisplayOrientation(AVCFile *AVC, BitBuffer *BitB) { // display_orientation
        AVC->SEI->DisplayOrientationCancelFlag = ReadBits(BitB, 1, true);
        if (AVC->SEI->DisplayOrientationCancelFlag == false) {
            AVC->SEI->HorizontalFlip = ReadBits(BitB, 1, true);
            AVC->SEI->VerticalFlip   = ReadBits(BitB, 1, true);
            AVC->SEI->CounterClockwiseRotation = ReadBits(BitB, 16, true);
            AVC->SEI->DisplayOrientationRepetitionPeriod = ReadExpGolomb(BitB, false);
            AVC->SEI->DisplayOrientationExtensionFlag = ReadBits(BitB, 1, true);
        }
    }
    
    void ParseSEIDepthRepresentationInformation(AVCFile *AVC, BitBuffer *BitB) { // depth_representation_info
        AVC->SEI->AllViewsEqual                                     = ReadBits(BitB, 1, true);
        if (AVC->SEI->AllViewsEqual == true) {
            AVC->SPS->ViewCount                                     = ReadExpGolomb(BitB, false) + 1;
        } else {
            AVC->SPS->ViewCount                                     = 1;
        }
        AVC->SEI->ZNearFlag                                         = ReadBits(BitB, 1, true);
        AVC->SEI->ZFarFlag                                          = ReadBits(BitB, 1, true);
        if ((AVC->SEI->ZNearFlag == true) || (AVC->SEI->ZFarFlag == true)) {
            AVC->SEI->ZAxisEqualFlag                                = ReadBits(BitB, 1, true);
            if (AVC->SEI->ZAxisEqualFlag == true) {
                AVC->SEI->CommonZAxisReferenceView                  = ReadExpGolomb(BitB, false);
            }
        }
        AVC->SEI->DMinFlag                                          = ReadBits(BitB, 1, true);
        AVC->SEI->DMaxFlag                                          = ReadBits(BitB, 1, true);
        AVC->SEI->DepthRepresentationType                           = ReadExpGolomb(BitB, false);
        for (uint8_t View                                           = 0; View < AVC->SPS->ViewCount; View++) {
            AVC->SEI->DepthInfoViewID[View]                         = ReadExpGolomb(BitB, false);
            if ((AVC->SEI->ZNearFlag == true || AVC->SEI->ZFarFlag == true) && (AVC->SEI->ZAxisEqualFlag == false)) {
                AVC->SEI->ZAxisReferenceView[View]                  = ReadExpGolomb(BitB, false);
            }
            if (AVC->SEI->DMinFlag == true || AVC->SEI->DMaxFlag == true) {
                AVC->SEI->DisparityReferenceView[View]              = ReadExpGolomb(BitB, false);
            }
            if (AVC->SEI->ZNearFlag == true) {
                //depth_representation_sei_element(ZNearSign, ZNearExp, ZNearMantissa, ZNearManLen);
                //ParseSEIDepthRepresentationElement(BitB, AVC->SEI->ZNearSign, AVC->SEI->ZNearExp, AVC->SEI->ZNearMantissa, AVC->SEI->ZNear)
            }
            if (AVC->SEI->ZFarFlag == true) {
                //depth_representation_sei_element(ZFarSign, ZFarExp, ZFarMantissa, ZFarManLen);
            }
            if (AVC->SEI->DMinFlag == true) {
                //depth_representation_sei_element(DMinSign, DMinExp, DMinMantissa, DMinManLen);
            }
            if (AVC->SEI->DMaxFlag == true) {
                //depth_representation_sei_element(DMaxSign, DMaxExp, DMaxMantissa, DMaxManLen);
            }
        }
        if (AVC->SEI->DepthRepresentationType == 3) {
            AVC->SEI->DepthNonlinearRepresentation                  = ReadExpGolomb(BitB, false) + 1;
            for (uint8_t Index                                      = 0; Index < AVC->SEI->DepthNonlinearRepresentation; Index++) {
                AVC->SEI->DepthNonlinearRepresentationModel[Index]  = ReadExpGolomb(BitB, false);
            }
        }
    }
    
    void ParseSEI3DReferenceDisplayInfo(AVCFile *AVC, BitBuffer *BitB) { // three_dimensional_reference_displays_info
        AVC->SEI->TruncationErrorExponent                           = ReadExpGolomb(BitB, false);
        AVC->SEI->TruncatedWidthExponent                            = ReadExpGolomb(BitB, false);
        AVC->SEI->ReferenceViewingDistanceFlag                      = ReadBits(BitB, 1, true);
        
        if (AVC->SEI->ReferenceViewingDistanceFlag == true) {
            AVC->SEI->TruncatedReferenveViewingDistanceExponent     = ReadExpGolomb(BitB, false);
        }
        AVC->SEI->NumReferenceDisplays                              = ReadExpGolomb(BitB, false) + 1;
        for (uint8_t Display                                        = 0; Display < AVC->SEI->NumReferenceDisplays; Display++) {
            AVC->SEI->ReferenceBaselineExponent[Display]            = ReadExpGolomb(BitB, false);
            AVC->SEI->ReferenceBaselineMantissa[Display]            = ReadExpGolomb(BitB, false);
            AVC->SEI->ReferenceDisplayWidthExponent[Display]        = ReadExpGolomb(BitB, false);
            AVC->SEI->ReferenceDisplayWidthMantissa[Display]        = ReadExpGolomb(BitB, false);
            if (AVC->SEI->ReferenceViewingDistanceFlag == true) {
                AVC->SEI->ReferenceViewingDistanceExponent[Display] = ReadExpGolomb(BitB, false);
                AVC->SEI->ReferenceViewingDistanceMantissa[Display] = ReadExpGolomb(BitB, false);
            }
            AVC->SEI->ShiftPresentFlag[Display]                     = ReadBits(BitB, 1, true);
            if (AVC->SEI->ShiftPresentFlag[Display] == true) {
                AVC->SEI->SampleShift[Display]                      = ReadExpGolomb(BitB, false) - 512;
            }
        }
        AVC->SEI->ReferenceDisplays3DFlag                           = ReadBits(BitB, 1, true);
    }
    
    void ParseSEIDepthTiming(AVCFile *AVC, BitBuffer *BitB) { // depth_timing
        AVC->SEI->PerViewDepthTimingFlag                              = ReadBits(BitB, 1, true);
        
        if (AVC->SEI->PerViewDepthTimingFlag == true) {
            for (uint8_t View = 0; View < AVC->DPS->NumDepthViews; View++) {
                AVC->SEI->OffsetLength[View]                          = ReadBits(BitB, 5, true) + 1;
                AVC->SEI->DepthDisplayDelayOffsetFP[View]             = ReadBits(BitB, AVC->SEI->OffsetLength[View], true);
                AVC->SEI->DepthDisplayDelayOffsetDP[View]             = ReadBits(BitB, AVC->SEI->OffsetLength[View], true);
            }
        }
    }
    
    void ParseSEIDepthGridPosition(AVCFile *AVC, BitBuffer *BitB) { // depth_grid_position()
        AVC->SEI->DepthGridPositionXFP                                = ReadBits(BitB, 20, true);
        AVC->SEI->DepthGridPositionXDP                                = ReadBits(BitB, 4, true);
        AVC->SEI->DepthGridPositionXSignFlag                          = ReadBits(BitB, 1, true);
        AVC->SEI->DepthGridPositionYFP                                = ReadBits(BitB, 20, true);
        AVC->SEI->DepthGridPositionYDP                                = ReadBits(BitB, 4, true);
        AVC->SEI->DepthGridPositionYSignFlag                          = ReadBits(BitB, 1, true);
    }
    
    void ParseSEIDepthSamplingInfo(AVCFile *AVC, BitBuffer *BitB) { // depth_sampling_info
        AVC->SEI->DepthSampleWidthMul                                 = ReadBits(BitB, 16, true);
        AVC->SEI->DepthSampleWidthDP                                  = ReadBits(BitB, 4, true);
        AVC->SEI->DepthSampleHeightMul                                = ReadBits(BitB, 16, true);
        AVC->SEI->DepthSampleHeightDP                                 = ReadBits(BitB, 4, true);
        AVC->SEI->PerViewDepthTimingFlag                              = ReadBits(BitB, 1, true);
        if (AVC->SEI->PerViewDepthTimingFlag == true) {
            AVC->SEI->NumDepthGridViews                               = ReadExpGolomb(BitB, false) + 1;
            for (uint8_t DepthGrid = 0; DepthGrid < AVC->SEI->NumDepthGridViews; DepthGrid++) {
                AVC->SEI->DepthInfoViewID[DepthGrid]                  = ReadExpGolomb(BitB, false);
                ParseSEIDepthGridPosition(AVC, BitB);
            }
        } else {
            ParseSEIDepthGridPosition(AVC, BitB);
        }
    }
    
    void ParseSEIConstrainedDepthParameterSetID(AVCFile *AVC, BitBuffer *BitB) { // constrained_depth_parameter_set_identifier
        AVC->SEI->MaxDPSID                                            = ReadExpGolomb(BitB, false);
        AVC->SEI->MaxDPSIDDiff                                        = ReadExpGolomb(BitB, false);
    }
    
    void ParseSEIMeteringDisplayColorVolume(AVCFile *AVC, BitBuffer *BitB) { // mastering_display_color_volume
        for (uint8_t Color                                            = 0; Color < 3; Color++) {
            AVC->SEI->DisplayPrimariesX[Color]                        = ReadBits(BitB, 16, true);
            AVC->SEI->DisplayPrimariesY[Color]                        = ReadBits(BitB, 16, true);
        }
        AVC->SEI->WhitePointX                                         = ReadBits(BitB, 16, true);
        AVC->SEI->WhitePointY                                         = ReadBits(BitB, 16, true);
        AVC->SEI->MaxDisplayMasteringLuminance                        = ReadBits(BitB, 32, true);
        AVC->SEI->MinDisplayMasteringLuminance                        = ReadBits(BitB, 32, true);
    }
    
    void ParseSEIMVCDScalableNesting(AVCFile *AVC, BitBuffer *BitB) { // mvcd_scalable_nesting
        AVC->SEI->OperationPointFlag                                  = ReadBits(BitB, 1, true);
        if (AVC->SEI->OperationPointFlag == false) {
            AVC->SEI->AllViewComponentsInAUFlag                       = ReadBits(BitB, 1, true);
            if (AVC->SEI->AllViewComponentsInAUFlag == false) {
                AVC->SEI->NumViewComponents                           = ReadExpGolomb(BitB, false) + 1;
                for (uint8_t ViewComponent                            = 0; ViewComponent < AVC->SEI->NumViewComponents; ViewComponent++) {
                    AVC->SEI->SEIViewID[ViewComponent]                = ReadBits(BitB, 10, true);
                    AVC->SEI->SEIViewApplicabilityFlag[ViewComponent] = ReadBits(BitB, 1, true);
                }
            }
        } else {
            AVC->SEI->SEIOpTextureOnlyFlag                            = ReadBits(BitB, 1, true);
            AVC->SEI->NumViewComponents                               = ReadExpGolomb(BitB, false) + 1;
            for (uint8_t ViewComponent                                = 0; ViewComponent < AVC->SEI->NumViewComponents; ViewComponent++) {
                AVC->SEI->SEIOpViewID[ViewComponent]                  = ReadBits(BitB, 10, true);
                if (AVC->SEI->SEIOpTextureOnlyFlag == false) {
                    AVC->SEI->SEIOpDepthFlag[ViewComponent]           = ReadBits(BitB, 1, true);
                    AVC->SEI->SEIOpTextureFlag[ViewComponent]         = ReadBits(BitB, 1, true);
                }
            }
            AVC->SEI->SEIOpTemporalID                                 = ReadBits(BitB, 1, true);
        }
        AlignInput(BitB, 1);
        ParseSEIMessage(AVC, BitB); // sei_message();
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
    
    void ParseSEIMessage(AVCFile *AVC, BitBuffer *BitB) { // sei_message
        char ErrorDescription[BitIOStringSize];
        
        while (PeekBits(BitB, 8, true) == 0xFF) {
            SkipBits(BitB, 8);
            AVC->SEI->SEIType += 255;
        }
        AVC->SEI->SEIType += ReadBits(BitB, 8, true); // last_payload_type_byte, 5
        
        while (PeekBits(BitB, 8, true) == 0xFF) {
            SkipBits(BitB, 8);
            AVC->SEI->SEISize    += 255;
        }
        AVC->SEI->SEISize += ReadBits(BitB, 8, true); // last_payload_size_byte, 692, emulation prevention bytes not included, but these fields are.
        
        switch (AVC->SEI->SEIType) { // sei_payload
            case SEI_BufferingPeriod:                         // 0
                ParseSEIBufferingPeriod(AVC, BitB);
                break;
            case SEI_PictureTiming:                           // 1
                ParseSEIPictureTiming(AVC, BitB);
                break;
            case SEI_PanScan:                                 // 2
                ParseSEIPanScan(AVC, BitB);
                break;
            case SEI_Filler:                                  // 3
                ParseSEIFiller(AVC, BitB);
                break;
            case SEI_RegisteredUserData:                      // 4
                ParseSEIRegisteredUserData(AVC, BitB);
                break;
            case SEI_UnregisteredUserData:                    // 5
                ParseSEIUnregisteredUserData(AVC, BitB);
                break;
            case SEI_RecoveryPoint:                           // 6
                ParseSEIRecoveryPoint(AVC, BitB);
                break;
            case SEI_RepetitiveReferencePicture:              // 7
                ParseSEIRepetitiveReferencePicture(AVC, BitB);
                break;
            case SEI_SparePicture:                            // 8
                ParseSEISparePicture(AVC, BitB);
                break;
            case SEI_SceneInformation:                        // 9
                ParseSEISceneInfo(AVC, BitB);
                break;
            case SEI_SubSequenceInformation:                  // 10
                ParseSEISubSequenceInfo(AVC, BitB);
                break;
            case SEI_SubSequenceLayerProperties:              // 11
                ParseSEISubSequenceLayerProperties(AVC, BitB);
                break;
            case SEI_SubSequenceProperties:                   // 12
                ParseSEISubSequenceProperties(AVC, BitB);
                break;
            case SEI_FullFrameFreeze:                         // 13
                ParseSEIFullFrameFreeze(AVC, BitB);
                break;
            case SEI_FullFrameFreezeRelease:                  // 14
                ParseSEIFullFrameFreezeRelease(AVC, BitB);
                break;
            case SEI_FullFrameSnapshot:                       // 15
                ParseSEIFullFrameSnapshot(AVC, BitB);
                break;
            case SEI_ProgressiveRefinementSegmentStart:       // 16
                ParseSEIProgressiveRefinementSegmentStart(AVC, BitB);
                break;
            case SEI_ProgressiveRefinementSegmentEnd:         // 17
                ParseSEIProgressiveRefinementSegmentEnd(AVC, BitB);
                break;
            case SEI_MotionConstrainedSliceGroup:             // 18
                ParseSEIMotionConstrainedSliceGroupSet(AVC, BitB);
                break;
            case SEI_FilmGrainCharacteristics:                // 19
                ParseSEIFilmGrainCharacteristics(AVC, BitB);
                break;
            case SEI_DeblockingFilterDisplayPreferences:      // 20
                ParseSEIDeblockingFilterDisplayPreference(AVC, BitB);
                break;
            case SEI_StereoVideoInformation:                  // 21
                ParseSEIStereoVideoInfo(AVC, BitB);
                break;
            case SEI_PostFilterHint:                          // 22
                ParseSEIPostFilterHint(AVC, BitB);
                break;
            case SEI_ToneMappingInformation:                  // 23
                ParseSEIToneMappingInfo(AVC, BitB);
                break;
            case SEI_ScalabilityInformation:                  // 24
                ParseSEIScalabilityInfo(AVC, BitB);
                break;
            case SEI_SubPictureScalableLayer:                 // 25
                ParseSEISubPictureScalableLayer(AVC, BitB);
                break;
            case SEI_NonRequiredLayerRep:                     // 26
                ParseSEINonRequiredLayerRep(AVC, BitB);
                break;
            case SEI_PriorityLayerInformation:                // 27
                ParseSEIPriorityLayerInfo(AVC, BitB);
                break;
            case SEI_LayersNotPresent:                        // 28
                ParseSEILayersNotPresent(AVC, BitB);
                break;
            case SEI_LayerDependencyChange:                   // 29
                ParseSEILayerDependencyChange(AVC, BitB);
                break;
            case SEI_ScalableNesting:                         // 30
                ParseSEIScalableNesting(AVC, BitB);
                break;
            case SEI_BaseLayerTemporalHRD:                    // 31
                ParseSEIBaseLayerTemporalHRD(AVC, BitB);
                break;
            case SEI_QualityLayerIntegrityCheck:              // 32
                ParseSEIQualityLayerIntegrityCheck(AVC, BitB);
                break;
            case SEI_RedundantPictureProperty:                // 33
                ParseSEIRedundantPicProperty(AVC, BitB);
                break;
            case SEI_TemporalL0DependencyRepresentationIndex: // 34
                ParseSEITemporalDependencyRepresentationIndex(AVC, BitB);
                break;
            case SEI_TemporalLevelSwitchingPoint:             // 35
                ParseSEITemporalLevelSwitchingPoint(AVC, BitB);
                break;
            case SEI_ParallelDecodingInformation:             // 36
                ParseSEIParallelDecodingInfo(AVC, BitB);
                break;
            case SEI_MVCScalableNesting:                      // 37
                ParseSEIMVCScalableNesting(AVC, BitB);
                break;
            case SEI_ViewScalabilityInformation:              // 38
                ParseSEIViewScalabilityInfo(AVC, BitB);
                break;
            case SEI_MVCSceneInformation:                     // 39
                ParseSEIMVCSceneInfo(AVC, BitB);
                break;
            case SEI_MVCAquisitionInformation:                // 40
                ParseSEIMVCAcquisitionInfo(AVC, BitB);
                break;
            case SEI_NonRequiredViewComponent:                // 41
                ParseSEINonRequiredViewComponent(AVC, BitB);
                break;
            case SEI_ViewDependencyChange:                    // 42
                ParseSEIViewDependencyChange(AVC, BitB);
                break;
            case SEI_OperationPointsNotPresent:               // 43
                ParseSEIOperationPointNotPresent(AVC, BitB);
                break;
            case SEI_BaseViewTemporalHRD:                     // 44
                ParseSEIBaseViewTemporalHRD(AVC, BitB);
                break;
            case SEI_FramePackingArrangement:                 // 45
                ParseSEIFramePackingArrangement(AVC, BitB);
                break;
            case SEI_MVCViewPosition:                         // 46
                ParseSEIMVCViewPosition(AVC, BitB);
                break;
            case SEI_DisplayOrientation:                      // 47
                ParseSEIDisplayOrientation(AVC, BitB);
                break;
            case SEI_MVCDScalableNesting:                     // 48
                ParseSEIMVCDScalableNesting(AVC, BitB);
                break;
            case SEI_MVCDViewScalabilityInformation:          // 49
                ParseSEIViewScalabilityInfo(AVC, BitB);
                break;
            case SEI_DepthRepresentationInformation:          // 50
                ParseSEIDepthRepresentationInformation(AVC, BitB);
                break;
            case SEI_3DReferenceDisplaysInformation:          // 51
                ParseSEI3DReferenceDisplayInfo(AVC, BitB);
                break;
            case SEI_DepthTiming:                             // 52
                ParseSEIDepthTiming(AVC, BitB);
                break;
            case SEI_DepthSamplingInformation:                // 53
                ParseSEIDepthSamplingInfo(AVC, BitB);
                break;
            case SEI_MVCConstrainedDPSIdentifier:             // 54
                ParseSEIConstrainedDepthParameterSetID(AVC, BitB);
                break;
            case SEI_MasteringDisplayColorVolume:             // 137
                ParseSEIMeteringDisplayColorVolume(AVC, BitB);
                break;
            default:
                snprintf(ErrorDescription, BitIOStringSize, "Unrecognized SEIType: %d", AVC->SEI->SEIType);
                Log(LOG_ERR, "libAVC", "ParseSEIMessage", ErrorDescription);
                SkipBits(BitB, AVC->SEI->SEISize);
                break;
        }
    }
    
#ifdef __cplusplus
}
#endif
