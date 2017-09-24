#include "../../include/Private/Common/libModernAVC_Tables.h"
#include "../../include/Private/Common/libModernAVC_Types.h"
#include "../../include/Private/Common/libModernAVC_Common.h"
#include "../../include/Private/Decode/libModernAVC_Decode.h"
#include "../../include/Private/Decode/libModernAVC_DecodeMacroBlock.h"
#include "../../include/Private/Decode/libModernAVC_DecodeSlice.h"
#include "../../include/Private/Decode/libModernAVC_ParseNAL.h"

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
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSequenceParameterSetData", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSequenceParameterSetData", "Pointer to BitBuffer is NULL");
        } else {
            ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
            Dec->SPS->ProfileIDC                                                        = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8); // 100
            Dec->SPS->ConstraintFlag0                                                   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1); // 0
            Dec->SPS->ConstraintFlag1                                                   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1); // 0
            Dec->SPS->ConstraintFlag2                                                   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1); // 0
            Dec->SPS->ConstraintFlag3                                                   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1); // 0
            Dec->SPS->ConstraintFlag4                                                   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1); // 0
            Dec->SPS->ConstraintFlag5                                                   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1); // 0
            SkipBits(BitB, 2); // Zero bits.
            Dec->SPS->LevelIDC[0]                                                       = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8); // 51
            Dec->SPS->SeqParamSetID                                                     = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false); // 1
            
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
                ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                Dec->SPS->ChromaFormatIDC                                               = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false); // 1 aka 420
                if (Dec->SPS->ChromaFormatIDC == Chroma444) {
                    Dec->SPS->SeperateColorPlane                                        = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                }
                Dec->SPS->LumaBitDepthMinus8                                            = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false); // 8
                Dec->SPS->ChromaBitDepthMinus8                                          = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                Dec->SPS->QPPrimeBypassFlag                                             = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1); // qpprime_y_zero_transform_bypass_flag
                Dec->SPS->ScalingMatrixFlag                                             = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                
                if (Dec->SPS->ScalingMatrixFlag == true) {
                    for (uint8_t i = 0; i < ((Dec->SPS->ChromaFormatIDC != Chroma444) ? 8 : 12); i++) {
                        Dec->SPS->ScalingListFlag[i]                                    = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
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
            Dec->SPS->PicOrderCount                                                     = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            if (Dec->SPS->PicOrderCount == 0) {
                Dec->SPS->MaxPicOrder                                                   = ReadExpGolomb(BitB, false) + 4;
            } else {
                Dec->SPS->DeltaPicOrder                                                 = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                Dec->SPS->OffsetNonRefPic                                               = ReadExpGolomb(BitB);
                Dec->SPS->OffsetTop2Bottom                                              = ReadExpGolomb(BitB);
                Dec->SPS->RefFramesInPicOrder                                           = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                for (uint8_t i = 0; i < Dec->SPS->RefFramesInPicOrder; i++) {
                    Dec->SPS->RefFrameOffset[i]                                         = ReadExpGolomb(BitB);
                }
                
            }
            Dec->SPS->MaxRefFrames                                                      = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            Dec->SPS->GapsInFrameNumAllowed                                             = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            Dec->SPS->PicWidthInMacroBlocksMinus1                                       = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            Dec->SPS->PicHeightInMapUnitsMinus1                                         = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            Dec->SPS->OnlyMacroBlocksInFrame                                            = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->SPS->OnlyMacroBlocksInFrame == false) {
                Dec->SPS->AdaptiveFrameFlag                                             = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            }
            Dec->SPS->Inference8x8                                                      = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            Dec->SPS->FrameCroppingFlag                                                 = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->SPS->FrameCroppingFlag == true) {
                Dec->SPS->FrameCropLeft                                                 = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                Dec->SPS->FrameCropRight                                                = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                Dec->SPS->FrameCropTop                                                  = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                Dec->SPS->FrameCropBottom                                               = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            }
            Dec->SPS->VUIPresent                                                        = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->SPS->VUIPresent == true) {
                ParseVideoUsabilityInformation(Dec, BitB);
            }
        }
    }
    
    void ParseNALSequenceParameterSetExtended(DecodeAVC *Dec, BitBuffer *BitB) { // seq_parameter_set_extension_rbsp?
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNALSequenceParameterSetExtended", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNALSequenceParameterSetExtended", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SPS->SeqParamSetID                                                     = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            Dec->SPS->AuxiliaryFormatID                                                 = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            if (Dec->SPS->AuxiliaryFormatID != 0) {
                Dec->SPS->AuxiliaryBitDepth                                             = ReadExpGolomb(BitB, false) + 8;
                Dec->SPS->AlphaIncrFlag                                                 = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                Dec->SPS->AlphaOpaqueValue                                              = ReadBits(BitIOMSByte, BitIOLSBit, BitB, Dec->SPS->AuxiliaryBitDepth + 9);
                Dec->SPS->AlphaTransparentValue                                         = ReadBits(BitIOMSByte, BitIOLSBit, BitB, Dec->SPS->AuxiliaryBitDepth + 9);
            }
            Dec->SPS->AdditionalExtensionFlag                                           = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            AlignBitBuffer(BitB, 1); // rbsp_trailing_bits
        }
    }
    
    void ParseNALSubsetSPS(DecodeAVC *Dec, BitBuffer *BitB) { // subset_seq_parameter_set_rbsp
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNALSubsetSPS", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNALSubsetSPS", "Pointer to BitBuffer is NULL");
        } else {
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
                Dec->SPS->MVCVUIParamsPresent                                           = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
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
            Dec->SPS->AdditionalExtension2                                              = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->SPS->AdditionalExtension2 == true) {
                while (more_rbsp_data()) {
                    Dec->SPS->AdditionalExtension2DataFlag                              = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                }
            }
            AlignBitBuffer(BitB, 1); // rbsp_trailing_bits
        }
    }
    
    void ParseNALSequenceParameterSet(DecodeAVC *Dec, BitBuffer *BitB) { // seq_parameter_set_rbsp
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNALSequenceParameterSet", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNALSequenceParameterSet", "Pointer to BitBuffer is NULL");
        } else {
            ParseSequenceParameterSetData(Dec, BitB);                        // seq_parameter_set_data
            AlignBitBuffer(BitB, 1);                                         // rbsp_trailing_bits();
        }
    }
    
    /* Video Usability Information */
    void ParseVideoUsabilityInformation(DecodeAVC *Dec, BitBuffer *BitB) { // Video Usability Information; ParseVUIParameters
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseVideoUsabilityInformation", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseVideoUsabilityInformation", "Pointer to BitBuffer is NULL");
        } else {
            Dec->VUI->AspectRatioInfoPresent                                            = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->VUI->AspectRatioInfoPresent == true) {
                Dec->VUI->AspectRatioIDC                                                = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
                if (Dec->VUI->AspectRatioIDC == 255) { // Extended_SAR = 255
                    Dec->VUI->SARWidth                                                  = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
                    Dec->VUI->SAWHeight                                                 = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
                }
            }
            Dec->VUI->OverscanInfoPresent                                               = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->VUI->OverscanInfoPresent == true) {
                Dec->VUI->DisplayInOverscan                                             = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            }
            Dec->VUI->VideoSignalTypePresent                                            = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->VUI->VideoSignalTypePresent == true) {
                Dec->VUI->VideoType                                                     = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 3);
                Dec->VUI->FullRange                                                     = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                Dec->VUI->ColorDescriptionPresent                                       = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                if (Dec->VUI->ColorDescriptionPresent == true) {
                    Dec->VUI->ColorPrimaries                                            = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
                    Dec->VUI->TransferProperties                                        = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
                    Dec->VUI->MatrixCoefficients                                        = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
                }
            }
            Dec->VUI->ChromaLocationPresent                                             = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->VUI->ChromaLocationPresent == true) {
                Dec->VUI->ChromaSampleLocationTop                                       = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                Dec->VUI->ChromaSampleLocationBottom                                    = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            }
            Dec->VUI->TimingInfoPresent[0]                                              = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->VUI->TimingInfoPresent[0] == true) {
                Dec->VUI->UnitsPerTick[0]                                               = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32);
                Dec->VUI->TimeScale[0]                                                  = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32);
                Dec->VUI->FixedFrameRateFlag[0]                                         = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            }
            Dec->VUI->NALHrdParamsPresent[0]                                            = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->VUI->NALHrdParamsPresent[0] == true) {
                ParseHypotheticalReferenceDecoder(Dec, BitB);
            }
            Dec->VUI->VCLHrdParamsPresent[0]                                            = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->VUI->VCLHrdParamsPresent[0] == true) {
                ParseHypotheticalReferenceDecoder(Dec, BitB); // wat
            }
            if ((Dec->VUI->NALHrdParamsPresent[0] || Dec->VUI->VCLHrdParamsPresent[0]) == true) {
                //Dec->StreamIsLowDelay                                                   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            }
            Dec->VUI->PicStructPresent[0]                                               = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            Dec->VUI->BitStreamRestricted                                               = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->VUI->BitStreamRestricted == true) {
                Dec->VUI->MotionVectorsOverPicBoundaries[0]                             = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                Dec->VUI->MaxBytesPerPicDenom[0]                                        = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                Dec->VUI->MaxBitsPerMarcoBlockDenom[0]                                  = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                Dec->VUI->MaxMotionVectorLength[0]                                      = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                Dec->VUI->MaxMotionVectorHeight[0]                                      = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                Dec->VUI->MaxReorderFrames[0]                                           = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                Dec->VUI->MaxFrameBuffer[0]                                             = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            }
        }
    }
    
    void ParseMVCDVUIParametersExtension(DecodeAVC *Dec, BitBuffer *BitB) { // mvcd_vui_parameters_extension
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseMVCDVUIParametersExtension", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseMVCDVUIParametersExtension", "Pointer to BitBuffer is NULL");
        } else {
            Dec->VUI->VUIMVCDNumOpPoints                                                = ReadExpGolomb(BitB, false) + 1;
            for (uint8_t MVCDOpPoint = 0; MVCDOpPoint < Dec->VUI->VUIMVCDNumOpPoints; MVCDOpPoint++) {
                Dec->VUI->VUIMVCDTemporalID[MVCDOpPoint]                                = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 3);
                Dec->VUI->VUIMVCDNumTargetOutputViews[MVCDOpPoint]                      = ReadExpGolomb(BitB, false) + 1;
                for (uint16_t VUIMVCDView = 0; VUIMVCDView < Dec->VUI->VUIMVCDNumTargetOutputViews[MVCDOpPoint]; VUIMVCDView++) {
                    Dec->VUI->VUIMVCDViewID[MVCDOpPoint][VUIMVCDView]                   = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    Dec->VUI->VUIMVCDDepthFlag[MVCDOpPoint][VUIMVCDView]                = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    Dec->VUI->VUIMVCDTextureFlag[MVCDOpPoint][VUIMVCDView]              = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                }
                Dec->VUI->VUIMVCDTimingInfoPresent[MVCDOpPoint]                         = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                if (Dec->VUI->VUIMVCDTimingInfoPresent[MVCDOpPoint] == true) {
                    Dec->VUI->VUIMVCDNumUnitsInTick[MVCDOpPoint]                        = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32);
                    Dec->VUI->VUIMVCDTimeScale[MVCDOpPoint]                             = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32);
                    Dec->VUI->VUIMVCDFixedFrameRateFlag[MVCDOpPoint]                    = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                }
                Dec->VUI->VUIMVCDNalHRDParametersPresent[MVCDOpPoint]                   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                if (Dec->VUI->VUIMVCDNalHRDParametersPresent[MVCDOpPoint] == true) {
                    hrd_parameters();
                }
                Dec->VUI->VUIMVCDVclHRDParametersPresent[MVCDOpPoint]                   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                if (Dec->VUI->VUIMVCDVclHRDParametersPresent[MVCDOpPoint] == true) {
                    hrd_parameters();
                }
                if ((Dec->VUI->VUIMVCDNalHRDParametersPresent[MVCDOpPoint] || Dec->VUI->VUIMVCDVclHRDParametersPresent[MVCDOpPoint]) == true) {
                    Dec->VUI->VUIMVCDHRDLowDelayPresent[MVCDOpPoint]                    = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                }
                Dec->VUI->VUIMVCDPicStructPresent[MVCDOpPoint]                          = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            }
        }
    }
    
    void ParseMVCVUIParametersExtension(DecodeAVC *Dec, BitBuffer *BitB) { // mvc_vui_parameters_extension
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseMVCVUIParametersExtension", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseMVCVUIParametersExtension", "Pointer to BitBuffer is NULL");
        } else {
            Dec->VUI->MVCNumOpertionPoints                                              = ReadExpGolomb(BitB, false) + 1;
            for (uint16_t Operation = 0; Operation < Dec->VUI->MVCNumOpertionPoints; Operation++) {
                Dec->VUI->MVCTemporalID[0][Operation]                                   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 3);
                Dec->VUI->MVCNumTargetViews[Operation]                                  = ReadExpGolomb(BitB, false) + 1;
                for (uint16_t OutputView = 0; OutputView < Dec->VUI->MVCNumTargetViews[Operation]; OutputView++) {
                    Dec->VUI->MVCViewID[Operation][OutputView]                          = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                }
                Dec->VUI->TimingInfoPresent[Operation]                                  = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                if (Dec->VUI->TimingInfoPresent[Operation] == true) {
                    Dec->VUI->MVCUnitsInTick[Operation]                                 = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32);
                    Dec->VUI->MVCTimeScale[Operation]                                   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32);
                    Dec->VUI->MVCFixedFrameRate[Operation]                              = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                }
                Dec->VUI->MVCNALHRDParamsPresent[Operation]                             = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                if (Dec->VUI->MVCNALHRDParamsPresent[Operation] == true) {
                    ParseHypotheticalReferenceDecoder(Dec, BitB);
                }
                Dec->VUI->MVCVCLHRDParamsPresent[Operation]                             = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                if (Dec->VUI->MVCVCLHRDParamsPresent[Operation] == true) {
                    ParseHypotheticalReferenceDecoder(Dec, BitB);
                }
                if ((Dec->VUI->MVCNALHRDParamsPresent[Operation] || Dec->VUI->MVCVCLHRDParamsPresent[Operation]) == true) {
                    Dec->VUI->MVCLowDelayFlag[Operation]                                = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                }
                Dec->VUI->PicStructPresent[Operation]                                   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            }
        }
    }
    
    void ParseSVCVUIExtension(DecodeAVC *Dec, BitBuffer *BitB) { // svc_vui_parameters_extension
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSVCVUIExtension", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSVCVUIExtension", "Pointer to BitBuffer is NULL");
        } else {
            Dec->VUI->VUIExtNumEntries                                                  = ReadExpGolomb(BitB, false) + 1;
            for (uint8_t VUIExtEntry = 0; VUIExtEntry < Dec->VUI->VUIExtNumEntries; VUIExtEntry++) {
                Dec->VUI->VUIExtDependencyID[VUIExtEntry]                               = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 3);
                Dec->VUI->VUIExtQualityID[VUIExtEntry]                                  = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 4);
                Dec->VUI->VUIExtTemporalID[VUIExtEntry]                                 = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 3);
                Dec->VUI->VUIExtTimingInfoPresentFlag[VUIExtEntry]                      = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                if (Dec->VUI->VUIExtTimingInfoPresentFlag[VUIExtEntry] == true) {
                    Dec->VUI->VUIExtNumUnitsInTick[VUIExtEntry]                         = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32);
                    Dec->VUI->VUIExtTimeScale[VUIExtEntry]                              = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32);
                    Dec->VUI->VUIExtFixedFrameRateFlag[VUIExtEntry]                     = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                }
                Dec->VUI->VUIExtNALHRDPresentFlag[VUIExtEntry]                          = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                if (Dec->VUI->VUIExtNALHRDPresentFlag[VUIExtEntry] == true) {
                    ParseHypotheticalReferenceDecoder(Dec, BitB);
                }
                Dec->VUI->VUIExtVCLHRDPresentFlag[VUIExtEntry]                          = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                if (Dec->VUI->VUIExtVCLHRDPresentFlag[VUIExtEntry] == true) {
                    ParseHypotheticalReferenceDecoder(Dec, BitB);
                }
                if ((Dec->VUI->VUIExtNALHRDPresentFlag[VUIExtEntry] == true) || (Dec->VUI->VUIExtVCLHRDPresentFlag[VUIExtEntry] == true)) {
                    Dec->VUI->VUIExtLowDelayHRDFlag[VUIExtEntry]                        = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                }
                Dec->VUI->VUIExtPicStructPresentFlag[VUIExtEntry]                       = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            }
        }
    }
    
    /* Picture Parameter Set */
    void ParseNALPictureParameterSet(DecodeAVC *Dec, BitBuffer *BitB) { // pic_parameter_set_rbsp
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNALPictureParameterSet", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNALPictureParameterSet", "Pointer to BitBuffer is NULL");
        } else {
            Dec->PPS->PicParamSetID                                                     = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false); // 3?
            Dec->SPS->SeqParamSetID                                                     = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false); // 2
            Dec->PPS->EntropyCodingMode                                                 = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1); // bit 9, 0
            Dec->PPS->BottomPicFieldOrderInSliceFlag                                    = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1); // bit 8, 0
            Dec->PPS->SliceGroups                                                       = ReadExpGolomb(BitB, false) + 1; // 2
            if (Dec->PPS->SliceGroups > 0) {
                Dec->PPS->SliceGroupMapType                                             = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false); // 48?
                if (Dec->PPS->SliceGroupMapType == 0) {
                    for (uint8_t SliceGroup = 0; SliceGroup <= Dec->PPS->SliceGroups; SliceGroup++) {
                        Dec->PPS->RunLength[SliceGroup]                                 = ReadExpGolomb(BitB, false) + 1;
                    }
                } else if (Dec->PPS->SliceGroupMapType == 2) {
                    for (uint8_t SliceGroup = 0; SliceGroup <= Dec->PPS->SliceGroups; SliceGroup++) {
                        Dec->PPS->TopLeft[SliceGroup]                                   = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                        Dec->PPS->BottomRight[SliceGroup]                               = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    }
                } else if ((Dec->PPS->SliceGroupMapType == 3) || (Dec->PPS->SliceGroupMapType == 4) || (Dec->PPS->SliceGroupMapType == 5)) {
                    for (uint8_t SliceGroup = 0; SliceGroup <= Dec->PPS->SliceGroups; SliceGroup++) {
                        Dec->PPS->SliceGroupChangeDirection                             = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                        Dec->PPS->SliceGroupChangeRate                                  = ReadExpGolomb(BitB, false) + 1;
                    }
                } else if (Dec->PPS->SliceGroupMapType == 6) {
                    Dec->PPS->PicSizeInMapUnits                                         = ReadExpGolomb(BitB, false) + 1;
                    for (uint64_t MapUnit = 0; MapUnit <= Dec->PPS->PicSizeInMapUnits; MapUnit++) {
                        Dec->PPS->SliceGroupID[MapUnit]                                 = ReadBits(BitIOMSByte, BitIOLSBit, BitB, Ceili(log2(Dec->PPS->SliceGroups)));
                    }
                }
            }
            Dec->PPS->RefIndex[0]                                                       = ReadExpGolomb(BitB, false) + 1;
            Dec->PPS->RefIndex[1]                                                       = ReadExpGolomb(BitB, false) + 1;
            Dec->PPS->WeightedPrediction                                                = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            Dec->PPS->WeightedBiPrediction                                              = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            Dec->PPS->InitialSliceQP                                                    = ReadExpGolomb(BitB, true) + 26;
            Dec->PPS->InitialSliceQS                                                    = ReadExpGolomb(BitB, true) + 26;
            Dec->PPS->ChromaQPOffset                                                    = ReadExpGolomb(BitB);
            Dec->PPS->DeblockingFilterFlag                                              = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            Dec->PPS->ConstrainedIntraFlag                                              = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            Dec->PPS->RedundantPictureFlag                                              = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (more_rbsp_data() == true) {
                Dec->PPS->TransformIs8x8                                                = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                Dec->PPS->SeperateScalingMatrix                                         = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                if (Dec->PPS->SeperateScalingMatrix == true) {
                    for (uint8_t i = 0; i < 6 + ((Dec->SPS->ChromaFormatIDC != Chroma444) ? 2 : 6) * Dec->PPS->TransformIs8x8; i++) {
                        Dec->PPS->PicScalingList[i]                                     = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                        if (Dec->PPS->PicScalingList[i] == true) {
                            if (i < 6) {
                                ScalingList(Dec, BitB, ScalingList4x4[i], 16, Dec->PPS->DefaultScalingMatrix4x4[i]);
                            } else {
                                ScalingList(Dec, BitB, ScalingList8x8[i - 6], 64, Dec->PPS->DefaultScalingMatrix8x8[i - 6]);
                            }
                        }
                    }
                    Dec->PPS->ChromaQPOffset                                            = ReadExpGolomb(BitB);
                }
                AlignBitBuffer(BitB, 1);
            }
        }
    }
    
    /* Scalable Video Coding */
    void ParseNALSVCExtension(DecodeAVC *Dec, BitBuffer *BitB) { // nal_unit_header_svc_extension
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNALSVCExtension", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNALSVCExtension", "Pointer to BitBuffer is NULL");
        } else {
            Dec->NAL->IDRFlag                                                           = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            Dec->NAL->PriorityID[0]                                                     = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 6);
            Dec->NAL->NoInterLayerPredictionFlag                                        = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            Dec->NAL->DependencyID[0]                                                   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 3);
            Dec->NAL->QualityID[0][0]                                                   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 4);
            Dec->NAL->TemporalID[0]                                                     = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 3);
            Dec->NAL->UseReferenceBasePictureFlag                                       = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            Dec->NAL->IsDisposable[0]                                                   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            Dec->NAL->OutputFlag                                                        = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            SkipBits(BitB, 2); // reserved_three_2bits
        }
    }
    
    void ParseNALSequenceParameterSetSVC(DecodeAVC *Dec, BitBuffer *BitB) { // seq_parameter_set_svc_extension
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNALSequenceParameterSetSVC", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNALSequenceParameterSetSVC", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SVC->InterLayerDeblockingFilterPresent                                 = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            Dec->SVC->ExtendedSpatialScalabilityIDC                                     = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 2);
            if ((Dec->SPS->ChromaFormatIDC == Chroma420) || (Dec->SPS->ChromaFormatIDC == Chroma422)) {
                Dec->SVC->ChromaPhaseXFlag                                              = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            }
            if (Dec->SPS->ChromaFormatIDC == Chroma420) {
                Dec->SVC->ChromaPhaseY                                                  = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 2);
            }
            if (Dec->SVC->ExtendedSpatialScalabilityIDC == 1) {
                if (Dec->SPS->ChromaFormatIDC != ChromaBW) {
                    Dec->SVC->SeqRefLayerChromaPhaseX                                   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                    Dec->SVC->SeqRefLayerChromaPhaseY                                   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 2);
                }
                Dec->SVC->RefLayerLeftOffset                                            = ReadExpGolomb(BitB);
                Dec->SVC->RefLayerTopOffset                                             = ReadExpGolomb(BitB);
                Dec->SVC->RefLayerRightOffset                                           = ReadExpGolomb(BitB);
                Dec->SVC->RefLayerBottomOffset                                          = ReadExpGolomb(BitB);
            }
            Dec->SVC->SequenceCoeffLevelPresent                                         = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->SVC->SequenceCoeffLevelPresent == true) {
                Dec->SVC->AdaptiveCoeffsPresent                                         = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            }
            Dec->SVC->SliceHeaderRestricted                                             = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
        }
    }
    
    void ParseNALPrefixUnitSVC(DecodeAVC *Dec, BitBuffer *BitB) { // prefix_nal_unit_svc
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNALPrefixUnitSVC", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNALPrefixUnitSVC", "Pointer to BitBuffer is NULL");
        } else {
            if (Dec->NAL->NALRefIDC != 0) {
                Dec->Slice->StoreRefBasePicFlag                                         = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                if (((Dec->NAL->UseReferenceBasePictureFlag) || (Dec->Slice->StoreRefBasePicFlag)) && (!Dec->NAL->IDRFlag)) {
                    ParseReferenceBasePictureSyntax(Dec, BitB); // dec_ref_base_pic_marking();
                }
                Dec->NAL->AdditionalPrefixNALExtensionFlag                              = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                if (Dec->NAL->AdditionalPrefixNALExtensionFlag == true) {
                    while (more_rbsp_data()) {
                        Dec->NAL->AdditionalPrefixNALExtensionDataFlag                  = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                    }
                }
                AlignBitBuffer(BitB, 1); // rbsp_trailing_bits()
            } else if (more_rbsp_data()) {
                while (more_rbsp_data()) {
                    Dec->NAL->AdditionalPrefixNALExtensionDataFlag                      = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                }
            }
            AlignBitBuffer(BitB, 1); // rbsp_trailing_bits()
        }
    }
    
    /* Multi-View Coding */
    void ParseNALMVCExtension(DecodeAVC *Dec, BitBuffer *BitB) { // nal_unit_header_mvc_extension
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNALMVCExtension", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNALMVCExtension", "Pointer to BitBuffer is NULL");
        } else {
            Dec->NAL->NonIDRFlag                                                        = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            Dec->NAL->PriorityID[0]                                                     = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 6);
            Dec->SPS->ViewID[0][0]                                                      = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 10);
            Dec->NAL->TemporalID[0]                                                     = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 3);
            Dec->NAL->IsAnchorPicture                                                   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            Dec->NAL->InterViewFlag                                                     = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            SkipBits(BitB, 1);
        }
    }
    
    void ParseSPSMVCDExtension(DecodeAVC *Dec, BitBuffer *BitB) { // seq_parameter_set_mvcd_extension
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSPSMVCDExtension", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSPSMVCDExtension", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SPS->ViewCount                                                         = ReadExpGolomb(BitB, false) + 1;
            for (uint16_t View = 0; View < Dec->SPS->ViewCount; View++) {
                Dec->SPS->ViewID[View][0]                                               = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                Dec->SPS->DepthViewPresent[View]                                        = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                Dec->SPS->DepthViewID[View]                                             = Dec->SPS->ViewID[View][0];
                Dec->DPS->NumDepthViews                                                += Dec->SPS->DepthViewPresent[View];
                Dec->SPS->TextureViewPresent[View]                                      = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            }
            for (uint16_t View = 1; View < Dec->SPS->ViewCount; View++) {
                if (Dec->SPS->DepthViewPresent[View] == true) {
                    Dec->SPS->AnchorRefsCount[0][View]                                  = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    for (uint8_t AnchorRef = 0; AnchorRef < Dec->SPS->AnchorRefsCount[0][View]; AnchorRef++) {
                        Dec->SPS->AnchorRef[0][View][AnchorRef]                         = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    }
                    Dec->SPS->AnchorRefsCount[1][View]                                  = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    for (uint8_t AnchorRef = 0; AnchorRef < Dec->SPS->AnchorRefsCount[1][View]; AnchorRef++) {
                        Dec->SPS->AnchorRef[1][View][AnchorRef]                         = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    }
                }
            }
            for (uint16_t View = 1; View < Dec->SPS->ViewCount; View++) {
                if (Dec->SPS->DepthViewPresent[View] == true) {
                    Dec->SPS->NonAnchorRefCount[0][View]                                = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    for (uint16_t NonAnchorRef = 0; NonAnchorRef < Dec->SPS->NonAnchorRefCount[0][View]; NonAnchorRef++) {
                        Dec->SPS->NonAnchorRef[0][View][NonAnchorRef]                   = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    }
                    Dec->SPS->NonAnchorRefCount[1][View]                                = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    for (uint16_t NonAnchorRef = 0; NonAnchorRef < Dec->SPS->NonAnchorRefCount[0][View]; NonAnchorRef++) {
                        Dec->SPS->NonAnchorRef[1][View][NonAnchorRef]                   = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    }
                }
            }
            Dec->SPS->NumLevelValues                                                    = ReadExpGolomb(BitB, false) + 1;
            for (uint8_t Level = 0; Level < Dec->SPS->NumLevelValues; Level++) {
                Dec->SPS->LevelIDC[Level]                                               = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                Dec->SPS->NumApplicableOps[Level]                                       = ReadExpGolomb(BitB, false) + 1;
                for (uint16_t AppOp = 0; AppOp < Dec->SPS->NumApplicableOps[Level]; AppOp++) {
                    Dec->SPS->AppOpTemporalID[Level][AppOp]                             = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    Dec->SPS->AppOpNumTargetViews[Level][AppOp]                         = ReadExpGolomb(BitB, false) + 1;
                    for (uint16_t AppOpTargetView = 0; AppOpTargetView < Dec->SPS->AppOpNumTargetViews[Level][AppOp]; AppOpTargetView++) {
                        Dec->SPS->AppOpTargetViewID[Level][AppOp][AppOpTargetView]      = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                        Dec->SPS->AppOpDepthFlag[Level][AppOp][AppOpTargetView]         = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                        Dec->SPS->AppOpTextureFlag[Level][AppOp][AppOpTargetView]       = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    }
                    Dec->SPS->AppOpTextureViews[Level][AppOp]                           = ReadExpGolomb(BitB, false) + 1;
                    Dec->SPS->AppOpNumDepthViews[Level][AppOp]                          = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                }
            }
            Dec->SPS->MVCDVUIParametersPresent                                          = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->SPS->MVCDVUIParametersPresent == true) {
                ParseMVCDVUIParametersExtension(Dec, BitB); // mvcd_vui_parameters_extension();
            }
            Dec->SPS->MVCDTextureVUIParametersPresent                                   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->SPS->MVCDTextureVUIParametersPresent == true) {
                ParseMVCVUIParametersExtension(Dec, BitB); //mvc_vui_parameters_extension();
            }
        }
    }
    
    void ParseNALDepthParameterSet(DecodeAVC *Dec, BitBuffer *BitB) { // depth_parameter_set_rbsp
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNALDepthParameterSet", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNALDepthParameterSet", "Pointer to BitBuffer is NULL");
        } else {
            Dec->DPS->DepthParameterSetID                                               = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            Dec->DPS->PredictionDirection                                               = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            if ((Dec->DPS->PredictionDirection == 0) || (Dec->DPS->PredictionDirection == 1)) {
                Dec->DPS->ReferenceDPSID[0]                                             = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                Dec->DPS->PredictedWeight0                                              = 64;
            }
            if (Dec->DPS->PredictionDirection == 0) {
                Dec->DPS->ReferenceDPSID[1]                                             = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                Dec->DPS->PredictedWeight0                                              = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 6);
            }
            Dec->DPS->NumDepthViews                                                     = ReadExpGolomb(BitB, false) + 1;
            depth_ranges(BitB, Dec->DPS->NumDepthViews, Dec->DPS->PredictionDirection, Dec->DPS->DepthParameterSetID);
            Dec->DPS->VSPParamFlag                                                      = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->DPS->VSPParamFlag == true) {
                vsp_param(Dec, BitB, Dec->DPS->NumDepthViews, Dec->DPS->PredictionDirection, Dec->DPS->DepthParameterSetID);
            }
            Dec->DPS->AdditionalExtensionFlag                                           = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            Dec->DPS->DepthMappingValues                                                = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            for (uint8_t i = 1; i <= Dec->DPS->DepthMappingValues; i++) {
                Dec->DPS->DepthRepresentationModel[i]                                   = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            }
            if (Dec->DPS->AdditionalExtensionFlag == true) {
                while (more_rbsp_data() == true) {
                    Dec->DPS->AdditionalExtensionFlag                                   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                }
            }
            AlignBitBuffer(BitB, 1); // rbsp_trailing_bits
        }
    }
    
    void ParseSPS3DAVCExtension(DecodeAVC *Dec, BitBuffer *BitB) { // seq_parameter_set_3davc_extension
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSPS3DAVCExtension", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSPS3DAVCExtension", "Pointer to BitBuffer is NULL");
        } else {
            if (Dec->DPS->NumDepthViews > 0) {
                Dec->SPS->AVC3DAcquisitionIDC                                           = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                for (uint8_t View = 0; View < Dec->DPS->NumDepthViews; View++) {
                    Dec->SPS->AVC3DViewID[View]                                         = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                }
                if (Dec->SPS->AVC3DAcquisitionIDC > 0) {
                    DepthRanges(BitB, Dec->DPS->NumDepthViews, 2, 0);
                    vsp_param(Dec, BitB, Dec->DPS->NumDepthViews, 2, 0);
                }
                Dec->SPS->ReducedResolutionFlag                                         = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                if (Dec->SPS->ReducedResolutionFlag == true) {
                    Dec->SPS->DepthPicWidthInMacroBlocks                                = ReadExpGolomb(BitB, false) + 1;
                    Dec->SPS->DepthPicHeightInMapUnits                                  = ReadExpGolomb(BitB, false) + 1;
                    Dec->SPS->DepthHorizontalDisparity                                  = ReadExpGolomb(BitB, false) + 1;
                    Dec->SPS->DepthVerticalDisparity                                    = ReadExpGolomb(BitB, false) + 1;
                    Dec->SPS->DepthHorizontalRSH                                        = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    Dec->SPS->DepthVerticalRSH                                          = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                } else {
                    Dec->SPS->DepthHorizontalDisparity                                  = 1;
                    Dec->SPS->DepthVerticalDisparity                                    = 1;
                    Dec->SPS->DepthHorizontalRSH                                        = 0;
                    Dec->SPS->DepthVerticalRSH                                          = 0;
                }
                Dec->SPS->DepthFrameCroppingFlag                                        = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                if (Dec->SPS->DepthFrameCroppingFlag == true) {
                    Dec->SPS->DepthFrameLeftCropOffset                                  = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    Dec->SPS->DepthFrameRightCropOffset                                 = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    Dec->SPS->DepthFrameTopCropOffset                                   = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    Dec->SPS->DepthFrameBottomCropOffset                                = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                }
                Dec->SPS->GridPosViewCount                                              = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                for (uint8_t TextureView = 0; TextureView < Dec->SPS->GridPosViewCount; TextureView++) {
                    Dec->SPS->GridPosViewID[TextureView]                                = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    Dec->SPS->GridPosX[TextureView]                                     = ReadExpGolomb(BitB);
                    Dec->SPS->GridPosY[TextureView]                                     = ReadExpGolomb(BitB);
                }
                Dec->SPS->SlicePrediction                                               = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                Dec->Slice->SeqViewSynthesisFlag                                        = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            }
            Dec->SPS->ALCSpsEnableFlag                                                  = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            Dec->SPS->EnableRLESkipFlag                                                 = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            bool AllViewsPairedFlag                                                     = AreAllViewsPaired(Dec);
            if (AllViewsPairedFlag == false) {
                for (uint8_t View = 0; View < Dec->SPS->ViewCount; View++) {
                    if (Dec->SPS->TextureViewPresent[View] == true) {
                        Dec->SPS->AnchorRefsCount[0][View]                              = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                        for (uint16_t AnchorViewL0 = 0; AnchorViewL0 < Dec->SPS->AnchorRefsCount[0][View]; AnchorViewL0++) {
                            Dec->SPS->AnchorRef[0][View][AnchorViewL0]                  = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                        }
                        Dec->SPS->AnchorRefsCount[1][View]                              = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                        for (uint16_t AnchorViewL1 = 0; AnchorViewL1 < Dec->SPS->AnchorRefsCount[1][View]; AnchorViewL1++) {
                            Dec->SPS->AnchorRef[1][View][AnchorViewL1]                  = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                        }
                    }
                }
                for (uint16_t View = 0; View <= Dec->SPS->ViewCount; View++) {
                    if (Dec->SPS->TextureViewPresent[View] == true) {
                        Dec->SPS->NonAnchorRefCount[0][View]                            = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                        for (uint16_t NonAnchorRefL0 = 0; NonAnchorRefL0 < Dec->SPS->NonAnchorRefCount[0][View]; NonAnchorRefL0++) {
                            Dec->SPS->NonAnchorRef[0][View][NonAnchorRefL0]             = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                        }
                        Dec->SPS->NonAnchorRefCount[1][View]                            = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                        for (uint16_t NonAnchorRefL1 = 0; NonAnchorRefL1 < Dec->SPS->NonAnchorRefCount[1][View]; NonAnchorRefL1++) {
                            Dec->SPS->NonAnchorRef[1][View][NonAnchorRefL1]             = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                        }
                    }
                }
            }
        }
    }
    
    void ParseNAL3DAVCExtension(DecodeAVC *Dec, BitBuffer *BitB) { // nal_unit_header_3davc_extension
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNAL3DAVCExtension", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNAL3DAVCExtension", "Pointer to BitBuffer is NULL");
        } else {
            Dec->NAL->ViewIndex                                                         = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
            Dec->NAL->DepthFlag                                                         = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            Dec->NAL->NonIDRFlag                                                        = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            Dec->NAL->TemporalID[0]                                                     = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 3);
            Dec->NAL->IsAnchorPicture                                                   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            Dec->NAL->InterViewFlag                                                     = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
        }
    }
    
    /* Hypothetical Reference Decoder */
    void ParseHypotheticalReferenceDecoder(DecodeAVC *Dec, BitBuffer *BitB) { // hrd_parameters
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseHypotheticalReferenceDecoder", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseHypotheticalReferenceDecoder", "Pointer to BitBuffer is NULL");
        } else {
            Dec->HRD->NumCodedPictureBuffers                                            = ReadExpGolomb(BitB, false) + 1;
            Dec->HRD->BitRateScale                                                      = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 4);
            Dec->HRD->CodedPictureBufferScale                                           = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 4);
            for (uint8_t SchedSelIdx = 0; SchedSelIdx < Dec->HRD->NumCodedPictureBuffers; SchedSelIdx++) {
                Dec->HRD->BitRate[SchedSelIdx]                                          = ReadExpGolomb(BitB, false) + 1;
                Dec->HRD->CodedPictureSize[SchedSelIdx]                                 = ReadExpGolomb(BitB, false) + 1;
                Dec->HRD->IsConstantBitRate[SchedSelIdx]                                = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1, true) + 1; // FIXME: is +1 correct
            }
            Dec->HRD->InitialCPBDelayLength                                             = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 5, true) + 1;
            Dec->HRD->CBPDelay                                                          = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 5, true) + 1;
            Dec->HRD->DBPDelay                                                          = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 5, true) + 1;
            Dec->HRD->TimeOffsetSize                                                    = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 5);
        }
    }
    
    /* Generic */
    void ParseNALSliceHeader(DecodeAVC *Dec, BitBuffer *BitB) { // slice_header
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNALSliceHeader", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNALSliceHeader", "Pointer to BitBuffer is NULL");
        } else {
            Dec->Slice->FirstMacroBlockInSlice                                          = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false); // 0
            Dec->Slice->Type                                                            = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false); // 0, 34 0s remaining
            Dec->PPS->PicParamSetID                                                     = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false); // 0, 26 0s remaining
            
            if (Dec->SPS->SeperateColorPlane == true) {
                Dec->Slice->ColorPlaneID                                                = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 2);
            }
            
            Dec->Slice->FrameNumber                                                     = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false); // FIXME: Should I use ReadBits?
            if (Dec->SPS->OnlyMacroBlocksInFrame == false) {
                Dec->Slice->SliceIsInterlaced                                           = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                if (Dec->Slice->SliceIsInterlaced == true) {
                    Dec->Slice->SliceIsBottomField                                      = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                }
            }
            Dec->Slice->SliceIsIDR                                                      = ((Dec->NAL->NALUnitType == NAL_IDRSliceNonPartitioned) ? true : false);
            if (Dec->Slice->SliceIsIDR == true) {
                Dec->Slice->IDRPicID                                                    = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            }
            if (Dec->SPS->PicOrderCount == 0) {
                Dec->Slice->PictureOrderCountLSB                                        = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                if ((Dec->PPS->BottomPicFieldOrderInSliceFlag == true) && (Dec->Slice->SliceIsInterlaced == false)) {
                    Dec->Slice->DeltaPicOrderCount[0]                                   = ReadExpGolomb(BitB);
                }
            }
            if (Dec->SPS->PicOrderCount == true && Dec->SPS->DeltaPicOrder == false) {
                Dec->Slice->DeltaPicOrderCount[0]                                       = ReadExpGolomb(BitB);
                if ((Dec->PPS->BottomPicFieldOrderInSliceFlag == true) && (Dec->Slice->SliceIsInterlaced == false)) {
                    Dec->Slice->DeltaPicOrderCount[1]                                   = ReadExpGolomb(BitB);
                }
            }
            if (Dec->PPS->RedundantPictureFlag == true) {
                Dec->PPS->RedundantPictureCount                                         = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            }
            if ((Dec->Slice->Type == SliceB1) || (Dec->Slice->Type == SliceB2)) {
                Dec->Slice->DirectSpatialMVPredictionFlag                               = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                
            }
            if ((Dec->Slice->Type == SliceP1)  || (Dec->Slice->Type == SliceP2)  ||
                (Dec->Slice->Type == SliceSP1) || (Dec->Slice->Type == SliceSP2) ||
                (Dec->Slice->Type == SliceB1)  || (Dec->Slice->Type == SliceB2)) {
                Dec->Slice->NumRefIDXActiveOverrideFlag                                 = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
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
                Dec->Slice->CabacInitIDC                                                = ReadExpGolomb(BitB);
            }
            Dec->Slice->SliceQPDelta                                                    = ReadExpGolomb(BitB);
            if (
                (Dec->Slice->Type == SliceSP1) || (Dec->Slice->Type == SliceSP2) ||
                (Dec->Slice->Type == SliceSI1) || (Dec->Slice->Type == SliceSI2)) {
                
                if ((Dec->Slice->Type == SliceSP1) || (Dec->Slice->Type == SliceSP2)) {
                    Dec->Slice->DecodePMBAsSPSlice                                      = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                }
                Dec->Slice->SliceQSDelta                                                = ReadExpGolomb(BitB);
            }
            if (Dec->PPS->DeblockingFilterFlag == true) {
                Dec->Slice->DisableDeblockingFilterIDC                                  = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                if (Dec->Slice->DisableDeblockingFilterIDC  != true) {
                    Dec->Slice->SliceAlphaOffsetC0                                      = ReadExpGolomb(BitB);
                    Dec->Slice->SliceBetaOffset                                         = ReadExpGolomb(BitB);
                }
            }
            if (Dec->PPS->SliceGroups > 0 && (Dec->PPS->SliceGroupMapType >= 3 && Dec->PPS->SliceGroupMapType <= 5)) {
                uint64_t Bits                                                           = Ceili(log2(Dec->PPS->PicSizeInMapUnits / Dec->PPS->SliceGroupChangeRate));
                Dec->Slice->SliceGroupChangeCycle                                       = ReadBits(BitIOMSByte, BitIOLSBit, BitB, Ceili(log2(Dec->PPS->PicSizeInMapUnits /  Dec->Slice->SliceGroupChangeRate)));
            }
        }
    }
    
    void ParseNALSliceData(DecodeAVC *Dec, BitBuffer *BitB, uint8_t Category) { // slice_data
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNALSliceData", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNALSliceData", "Pointer to BitBuffer is NULL");
        } else {
            if (Dec->PPS->EntropyCodingMode == Arithmetic) {
                while (IsBitBufferAligned(BitB, 1) == false) {
                    SkipBits(BitB, 1); // cabac_alignment_one_bit
                }
                uint64_t CurrentMacroBlockAddress                                       = Dec->Slice->FirstMacroBlockInSlice * (Dec->Slice->MbaffFrameFlag + 1);
                bool     MoreDataFlag                                                   = true;
                Dec->Slice->PreviousMacroBlockSkipped                                   = false;
                if ((Dec->Slice->Type != SliceI1) || (Dec->Slice->Type != SliceI2) || (Dec->Slice->Type != SliceSI1) || (Dec->Slice->Type != SliceSI2)) {
                    if (Dec->PPS->EntropyCodingMode == ExpGolomb) {
                        Dec->Slice->MacroBlockSkipRun                                   = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                        Dec->Slice->PreviousMacroBlockSkipped                           = (Dec->Slice->MacroBlockSkipRun > 0);
                        for (uint8_t SkippedMacroBlock = 0; SkippedMacroBlock < Dec->Slice->MacroBlockSkipRun; SkippedMacroBlock++) {
                            CurrentMacroBlockAddress                                    = NextMacroBlockAddress(Dec, CurrentMacroBlockAddress);
                        }
                    }
                }
            }
        }
    }
    
    void ParseNALSlicePartitionA(DecodeAVC *Dec, BitBuffer *BitB) { // slice_data_partition_a_layer_rbsp
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNALSlicePartitionA", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNALSlicePartitionA", "Pointer to BitBuffer is NULL");
        } else {
            ParseSliceHeader(Dec, BitB);
            uint64_t SliceID                                                            = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            ParseSliceData(Dec, BitB, 2); /* only category 2 parts of slice_data() syntax */
            rbsp_slice_trailing_bits(Dec, BitB); // AlignBitBuffer(BitB, 1);
        }
    }
    
    void ParseNALSlicePartitionB(DecodeAVC *Dec, BitBuffer *BitB) { // slice_data_partition_b_layer_rbsp
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNALSlicePartitionB", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNALSlicePartitionB", "Pointer to BitBuffer is NULL");
        } else {
            uint8_t SliceID                                                             = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            if (Dec->SPS->SeperateColorPlane == true) {
                uint8_t ColorPlaneID                                                    = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            }
            if (Dec->PPS->RedundantPictureFlag == true) {
                Dec->PPS->RedundantPictureCount                                         = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            }
            ParseSliceData(Dec, BitB, 3);
            rbsp_slice_trailing_bits(Dec, BitB); // AlignBitBuffer(BitB, 1);
        }
    }
    
    void ParseNALSlicePartitionC(DecodeAVC *Dec, BitBuffer *BitB) { // slice_data_partition_c_layer_rbsp
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNALSlicePartitionC", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNALSlicePartitionC", "Pointer to BitBuffer is NULL");
        } else {
            uint8_t SliceID                                                             = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            if (Dec->SPS->SeperateColorPlane == true) {
                uint8_t ColorPlaneID                                                    = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            }
            if (Dec->PPS->RedundantPictureFlag == true) {
                Dec->PPS->RedundantPictureCount                                         = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            }
            ParseSliceData(Dec, BitB, 4);
            rbsp_slice_trailing_bits(Dec, BitB); // AlignBitBuffer(BitB, 1);
        }
    }
    
    void ParseNALSliceNonPartitioned(DecodeAVC *Dec, BitBuffer *BitB) { // slice_layer_without_partitioning_rbsp
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNALSliceNonPartitioned", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNALSliceNonPartitioned", "Pointer to BitBuffer is NULL");
        } else {
            ParseNALSliceHeader(Dec, BitB);
            ParseNALSliceData(Dec, BitB, 0); // TODO: Fix category
            AlignBitBuffer(BitB, 1); // rbsp_slice_trailing_bits();
        }
    }
    
    void ParseNALFillerData(DecodeAVC *Dec, BitBuffer *BitB) { // filler_data_rbsp
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNALFillerData", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNALFillerData", "Pointer to BitBuffer is NULL");
        } else {
            while (PeekBits(BitB, 8, true) == 0xFF) {
                SkipBits(BitB, 8);
            }
        }
    }
    
    void ParseNALPrefixUnit(DecodeAVC *Dec, BitBuffer *BitB) { // prefix_nal_unit_rbsp
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNALPrefixUnit", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNALPrefixUnit", "Pointer to BitBuffer is NULL");
        } else {
            if (Dec->NAL->SVCExtensionFlag == true) {
                ParseNALPrefixUnitSVC(Dec, BitB);
            }
        }
    }
    
    void ParseNALAccessUnitDelimiter(DecodeAVC *Dec, BitBuffer *BitB) { // access_unit_delimiter_rbsp
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNALAccessUnitDelimiter", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseNALAccessUnitDelimiter", "Pointer to BitBuffer is NULL");
        } else {
            Dec->Slice->PictureType                                                     = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 3);
            AlignBitBuffer(BitB, 1);
        }
    }
    
    /* Supplemental Enhancement Information */
    void ParseSEIBufferingPeriod(DecodeAVC *Dec, BitBuffer *BitB) { // buffering_period
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIBufferingPeriod", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIBufferingPeriod", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SPS->SeqParamSetID                                                     = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            if (Dec->SEI->NalHrdBpPresentFlag == true) {
                for (uint8_t SchedSelIdx = 0; SchedSelIdx <= Dec->HRD->NumCodedPictureBuffers; SchedSelIdx++) {
                    Dec->SEI->InitialCPBRemovalDelay[SchedSelIdx]                       = ReadBits(BitIOMSByte, BitIOLSBit, BitB, Dec->HRD->InitialCPBDelayLength);
                    Dec->SEI->InitialCPBRemovalDelayOffset[SchedSelIdx]                 = ReadBits(BitIOMSByte, BitIOLSBit, BitB, Dec->HRD->InitialCPBDelayLength);
                }
            }
            Dec->HRD->VclHrdBpPresentFlag                                               = Dec->VUI->VCLHrdParamsPresent > 0 ? 1 : 0;
            if (Dec->HRD->VclHrdBpPresentFlag == true) {
                for (uint8_t SchedSelIdx = 0; SchedSelIdx <= Dec->HRD->NumCodedPictureBuffers; SchedSelIdx++) {
                    Dec->SEI->InitialCPBRemovalDelay[SchedSelIdx]                       = ReadBits(BitIOMSByte, BitIOLSBit, BitB, Dec->HRD->InitialCPBDelayLength);
                    Dec->SEI->InitialCPBRemovalDelayOffset[SchedSelIdx]                 = ReadBits(BitIOMSByte, BitIOLSBit, BitB, Dec->HRD->InitialCPBDelayLength);
                }
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
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIPictureTiming", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIPictureTiming", "Pointer to BitBuffer is NULL");
        } else {
            if (Dec->SEI->CpbDpbDelaysPresentFlag == true) {
                Dec->SEI->CPBRemovalDelay                                               = ReadBits(BitIOMSByte, BitIOLSBit, BitB, Dec->HRD->CBPDelay);
                Dec->SEI->DPBOutputDelay                                                = ReadBits(BitIOMSByte, BitIOLSBit, BitB, Dec->HRD->DBPDelay);
            }
            if (Dec->VUI->PicStructPresent[0] == true) {
                Dec->SEI->PicStruct                                                     = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 4);
                uint8_t NumClockTS = GetClockTS(Dec->SEI->PicStruct);
                for (uint8_t Time = 0; Time < NumClockTS; Time++) {
                    Dec->SEI->ClockTimestampFlag[Time]                                  = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                    if (Dec->SEI->ClockTimestampFlag[Time] == true) {
                        Dec->SEI->CTType                                                = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 2);
                        Dec->SEI->NuitFieldBasedFlag                                    = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                        Dec->SEI->CountingType                                          = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 5);
                        Dec->SEI->FullTimestampFlag                                     = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                        Dec->SEI->CountDroppedFlag                                      = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                        Dec->SEI->NFrames                                               = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
                        if (Dec->SEI->FullTimestampFlag == true) {
                            Dec->SEI->Seconds                                           = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 6);
                            Dec->SEI->Minutes                                           = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 6);
                            Dec->SEI->Hours                                             = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 5);
                        } else {
                            Dec->SEI->SecondsFlag                                       = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                            if (Dec->SEI->SecondsFlag == true) {
                                Dec->SEI->Seconds                                       = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 6);
                                Dec->SEI->MinutesFlag                                   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                                if (Dec->SEI->MinutesFlag == true) {
                                    Dec->SEI->Minutes                                   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 6);
                                    Dec->SEI->HoursFlag                                 = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                                    if (Dec->SEI->HoursFlag == true) {
                                        Dec->SEI->Hours                                 = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 5);
                                    }
                                }
                            }
                            if (Dec->HRD->TimeOffsetSize > 0) {
                                Dec->SEI->TimeOffset                                    = ReadBits(BitIOMSByte, BitIOLSBit, BitB, Dec->HRD->TimeOffsetSize);
                            }
                        }
                    }
                }
            }
        }
    }
    
    void ParseSEIPanScan(DecodeAVC *Dec, BitBuffer *BitB) { // pan_scan_rect
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIPanScan", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIPanScan", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->PanScanID                                                         = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            Dec->SEI->DisablePanScanFlag                                                = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->SEI->DisablePanScanFlag == false) {
                Dec->SEI->PanScanCount                                                  = ReadExpGolomb(BitB, false) + 1;
                for (uint8_t PanScan = 0; PanScan < Dec->SEI->PanScanCount; PanScan++) {
                    Dec->SEI->PanScanOffsetLeft[PanScan]                                = ReadExpGolomb(BitB);
                    Dec->SEI->PanScanOffsetRight[PanScan]                               = ReadExpGolomb(BitB);
                    Dec->SEI->PanScanOffsetTop[PanScan]                                 = ReadExpGolomb(BitB);
                    Dec->SEI->PanScanOffsetBottom[PanScan]                              = ReadExpGolomb(BitB);
                }
                Dec->SEI->PanScanRepitionPeriod                                         = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            }
        }
    }
    
    void ParseSEIFiller(DecodeAVC *Dec, BitBuffer *BitB) { // filler_payload
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIFiller", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIFiller", "Pointer to BitBuffer is NULL");
        } else {
            SkipBits(BitB, Bytes2Bits(Dec->SEI->SEISize));
        }
    }
    
    void ParseSEIRegisteredUserData(DecodeAVC *Dec, BitBuffer *BitB) { // user_data_registered_itu_t_t35
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIRegisteredUserData", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIRegisteredUserData", "Pointer to BitBuffer is NULL");
        } else {
            uint8_t CountryCodeSize = 0;
            Dec->SEI->CountryCode                                                       = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
            if (Dec->SEI->CountryCode != 0xFF) {
                CountryCodeSize                                                        += 1;
            } else {
                CountryCodeSize                                                        += 2;
                Dec->SEI->CountryCode                                                 <<= 8;
                Dec->SEI->CountryCode                                                  += ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
            }
        }
    }
    
    void ParseSEIUnregisteredUserData(DecodeAVC *Dec, BitBuffer *BitB) { // user_data_unregistered
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIUnregisteredUserData", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIUnregisteredUserData", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->UnregisteredUserDataUUID = ReadUUID(BitB); // DC45E9BD-E6D9-48B7-962C-D820D923EEEF, x264 UserID.
            SkipBits(BitB, Bytes2Bits(Dec->SEI->SEISize - BitIOBinaryUUIDSize));
        }
    }
    
    void ParseSEIRecoveryPoint(DecodeAVC *Dec, BitBuffer *BitB) { // recovery_point
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIRecoveryPoint", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIRecoveryPoint", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->RecoveryFrameCount                                                = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            Dec->SEI->ExactMatchFlag                                                    = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            Dec->SEI->BrokenLinkFlag                                                    = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            Dec->SEI->ChangingSliceGroupIDC                                             = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 2);
        }
    }
    
    void ParseSEIRepetitiveReferencePicture(DecodeAVC *Dec, BitBuffer *BitB) { // dec_ref_pic_marking_repetition
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIRepetitiveReferencePicture", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIRepetitiveReferencePicture", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->OriginalIDRFlag                                                   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            Dec->SEI->OriginalFrameNum                                                  = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            if (Dec->SPS->OnlyMacroBlocksInFrame == false) {
                Dec->SEI->OriginalFieldPicFlag                                          = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                if (Dec->SEI->OriginalFieldPicFlag == true) {
                    Dec->SEI->OriginalBottomFieldFlag                                   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                }
            }
            DecodeRefPicMarking(Dec, BitB); // dec_ref_pic_marking();
        }
    }
    
    void ParseSEISparePicture(DecodeAVC *Dec, BitBuffer *BitB) { // spare_pic
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEISparePicture", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEISparePicture", "Pointer to BitBuffer is NULL");
        } else {
            uint8_t MapUnitCount = 0;
            
            Dec->SEI->TargetFrameNum                                                    = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            Dec->SEI->SpareFieldFlag                                                    = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->SEI->SpareFieldFlag == true) {
                Dec->SEI->TargetBottomFieldFlag                                         = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            }
            Dec->SEI->NumSparePics                                                      = ReadExpGolomb(BitB, false) + 1;
            for (uint8_t SparePic = 0; SparePic < Dec->SEI->NumSparePics; SparePic++) {
                Dec->SEI->DeltaSpareFrameNum[SparePic]                                  = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                if (Dec->SEI->SpareFieldFlag == true) {
                    Dec->SEI->SpareBottomFieldFlag[SparePic]                            = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                }
                Dec->SEI->SpareAreaIDC[SparePic]                                        = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                if (Dec->SEI->SpareAreaIDC[SparePic] == 1) {
                    for (uint8_t MapUnit = 0; MapUnit < Dec->PPS->PicSizeInMapUnits; MapUnit++) {
                        Dec->SEI->SpareUnitFlag[SparePic][MapUnit]                      = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                    }
                } else if (Dec->SEI->SpareAreaIDC[SparePic] == 2) {
                    while (MapUnitCount < Dec->PPS->PicSizeInMapUnits) {
                        Dec->SEI->ZeroRunLength[SparePic][MapUnitCount]                 = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                        MapUnitCount += Dec->SEI->ZeroRunLength[SparePic][MapUnitCount] + 1;
                    }
                }
            }
        }
    }
    
    void ParseSEISceneInfo(DecodeAVC *Dec, BitBuffer *BitB) { // scene_info
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEISceneInfo", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEISceneInfo", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->SceneInfoPresentFlag                                              = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->SEI->SceneInfoPresentFlag == true) {
                Dec->SEI->SceneID                                                       = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                Dec->SEI->SceneTransitionType                                           = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                if (Dec->SEI->SceneTransitionType > 3) {
                    Dec->SEI->SecondSceneID                                             = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                }
            }
        }
    }
    
    void ParseSEISubSequenceInfo(DecodeAVC *Dec, BitBuffer *BitB) { // sub_seq_info
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEISubSequenceInfo", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEISubSequenceInfo", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->SubSequenceLayerNum                                                = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            Dec->SEI->SubSequenceID                                                      = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            Dec->SEI->FirstRefPicFlag                                                    = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            Dec->SEI->LeadingNonRefPicFlag                                               = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            Dec->SEI->LastPicFlag                                                        = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            Dec->SEI->SubSeqFrameNumFlag                                                 = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->SEI->SubSeqFrameNumFlag  == true) {
                Dec->SEI->SubSeqFrameNum                                                 = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            }
        }
    }
    
    void ParseSEISubSequenceLayerProperties(DecodeAVC *Dec, BitBuffer *BitB) { // sub_seq_layer_characteristics
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEISubSequenceLayerProperties", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEISubSequenceLayerProperties", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->NumSubSeqLayers                                                    = ReadExpGolomb(BitB, false) + 1;
            for (uint8_t Layer = 0; Layer < Dec->SEI->NumSubSeqLayers; Layer++) {
                Dec->SEI->AccurateStatisticsFlag                                         = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                Dec->SEI->AverageBitRate                                                 = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
                Dec->SEI->AverageFrameRate                                               = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
            }
        }
    }
    
    void ParseSEISubSequenceProperties(DecodeAVC *Dec, BitBuffer *BitB) { // sub_seq_characteristics
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEISubSequenceProperties", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEISubSequenceProperties", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->SubSequenceLayerNum                                                = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            Dec->SEI->SubSequenceID                                                      = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            Dec->SEI->DurationFlag                                                       = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->SEI->DurationFlag == true) {
                Dec->SEI->SubSeqDuration                                                 = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32);
            }
            Dec->SEI->AverageRateFlag                                                    = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->SEI->AverageRateFlag == true) {
                Dec->SEI->AccurateStatisticsFlag                                         = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                Dec->SEI->AverageBitRate                                                 = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
                Dec->SEI->AverageFrameRate                                               = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
            }
            Dec->SEI->NumReferencedSubSeqs                                               = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            for (uint16_t SubSequence = 0; SubSequence < Dec->SEI->NumReferencedSubSeqs; SubSequence++) {
                Dec->SEI->RefSubSeqLayerNum                                              = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                Dec->SEI->RefSubSeqID                                                    = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                Dec->SEI->RefSubSeqDirection                                             = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            }
        }
    }
    
    void ParseSEIFullFrameFreeze(DecodeAVC *Dec, BitBuffer *BitB) { // full_frame_freeze
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIFullFrameFreeze", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIFullFrameFreeze", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->FullFrameFreezeRepitionPeriod                                      = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
        }
    }
    
    void ParseSEIFullFrameFreezeRelease(DecodeAVC *Dec, BitBuffer *BitB) { // full_frame_freeze_release
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIFullFrameFreezeRelease", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIFullFrameFreezeRelease", "Pointer to BitBuffer is NULL");
        } else {
            SkipBits(BitB, Bytes2Bits(Dec->SEI->SEISize));
        }
    }
    
    void ParseSEIFullFrameSnapshot(DecodeAVC *Dec, BitBuffer *BitB) { // full_frame_snapshot
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIFullFrameSnapshot", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIFullFrameSnapshot", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->SnapshotID                                                         = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
        }
    }
    
    void ParseSEIProgressiveRefinementSegmentStart(DecodeAVC *Dec, BitBuffer *BitB) { // progressive_refinement_segment_start
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIProgressiveRefinementSegmentStart", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIProgressiveRefinementSegmentStart", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->ProgressiveRefinementID                                            = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            Dec->SEI->NumRefinementSteps                                                 = ReadExpGolomb(BitB, false) + 1;
        }
    }
    
    void ParseSEIProgressiveRefinementSegmentEnd(DecodeAVC *Dec, BitBuffer *BitB) { // progressive_refinement_segment_end
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIProgressiveRefinementSegmentEnd", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIProgressiveRefinementSegmentEnd", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->ProgressiveRefinementID                                            = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
        }
    }
    
    void ParseSEIMotionConstrainedSliceGroupSet(DecodeAVC *Dec, BitBuffer *BitB) { // motion_constrained_slice_group_set
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIMotionConstrainedSliceGroupSet", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIMotionConstrainedSliceGroupSet", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->NumSliceGroupsInSet                                                = ReadExpGolomb(BitB, false) + 1;
            if (Dec->SEI->NumSliceGroupsInSet > 1) {
                for (uint16_t SliceGroup = 0; SliceGroup < Dec->SEI->NumSliceGroupsInSet; SliceGroup++) {
                    Dec->PPS->SliceGroupID[SliceGroup]                                   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, Ceili(log2(Dec->PPS->SliceGroups)));
                }
            }
            Dec->SEI->ExactSampleValueMatchFlag[0]                                       = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            Dec->SEI->PanScanRectFlag                                                    = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->SEI->PanScanRectFlag == true) {
                Dec->SEI->PanScanID                                                      = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            }
        }
    }
    
    void ParseSEIFilmGrainCharacteristics(DecodeAVC *Dec, BitBuffer *BitB) { // film_grain_characteristics
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIFilmGrainCharacteristics", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIFilmGrainCharacteristics", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->FilmGrainCharactisticsCancelFlag                                   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->SEI->FilmGrainCharactisticsCancelFlag == false) {
                Dec->SEI->FilmGrainModelID                                               = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 2);
                Dec->SEI->SeperateColorDescriptionFlag                                   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                if (Dec->SEI->SeperateColorDescriptionFlag == true) {
                    Dec->SEI->FilmGrainBitDepthLuma                                      = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 3, true) + 8;
                    Dec->SEI->FilmGrainBitDepthChroma                                    = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 3, true) + 8;
                    Dec->SEI->FilmGrainFullRangeFlag                                     = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                    Dec->SEI->FilmGrainColorPrimaries                                    = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
                    Dec->SEI->FilmGrainTransferCharacteristics                           = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
                    Dec->SEI->FilmGrainMatrixCoefficents                                 = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
                }
                Dec->SEI->BlendingModeID                                                 = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 2);
                Dec->SEI->Scalefactor                                                    = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 4);
                for (uint8_t Channel = 0; Channel < 3; Channel++) {
                    Dec->SEI->CompModelPresent[Channel]                                  = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                }
                for (uint8_t Channel = 0; Channel < 3; Channel++) {
                    if (Dec->SEI->CompModelPresent[Channel] == true) {
                        Dec->SEI->NumIntensityIntervals[Channel]                         = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8, true) + 1;
                        Dec->SEI->NumModelValues[Channel]                                = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 3, true) + 1;
                        for (uint16_t Intensity = 0; Intensity < Dec->SEI->NumIntensityIntervals[Channel]; Intensity++) {
                            Dec->SEI->IntensityIntervalLowerBound[Channel][Intensity]    = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
                            Dec->SEI->IntensityIntervalUpperBound[Channel][Intensity]    = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
                            for (uint8_t ModelValue = 0; ModelValue < Dec->SEI->NumModelValues[Channel]; ModelValue++) {
                                Dec->SEI->CompModelValue[Channel][Intensity][ModelValue] = ReadExpGolomb(BitB);
                            }
                        }
                    }
                }
                Dec->SEI->FilmGrainCharacteristicsRepetitionPeriod                       = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            }
        }
    }
    
    void ParseSEIDeblockingFilterDisplayPreference(DecodeAVC *Dec, BitBuffer *BitB) { // deblocking_filter_display_preference
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIDeblockingFilterDisplayPreference", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIDeblockingFilterDisplayPreference", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->DeblockingDisplayPreferenceCancelFlag           = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->SEI->DeblockingDisplayPreferenceCancelFlag == false) {
                Dec->SEI->DisplayBeforeDeblockingFlag                 = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                Dec->SEI->DecodeFrameBufferingConstraintFlag          = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                Dec->SEI->DeblockingDisplayPreferenceRepetitionPeriod = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            }
        }
    }
    
    void ParseSEIStereoVideoInfo(DecodeAVC *Dec, BitBuffer *BitB) { // stereo_video_info
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIStereoVideoInfo", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIStereoVideoInfo", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->FieldViewsFlag               = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->SEI->FieldViewsFlag == true) {
                Dec->SEI->TopFieldIsLeftViewFlag       = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            } else {
                Dec->SEI->CurrentFrameIsLeftViewFlag   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                Dec->SEI->NextFrameIsDependentViewFlag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            }
            Dec->SEI->LeftViewSelfContainedFlag    = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            Dec->SEI->RightViewSelfContainedFlag   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
        }
    }
    
    void ParseSEIPostFilterHint(DecodeAVC *Dec, BitBuffer *BitB) { // post_filter_hint
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIPostFilterHint", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIPostFilterHint", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->FilterHintSizeY     = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            Dec->SEI->FilterHintSizeX     = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            Dec->SEI->FilterHintType      = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 2);
            for (uint8_t Color = 0; Color < 3; Color++) {
                for (uint8_t CY = 0; CY < Dec->SEI->FilterHintSizeY; CY++) {
                    for (uint8_t CX = 0; CX < Dec->SEI->FilterHintSizeX; CX++) {
                        Dec->SEI->FilterHint[Color][CY][CX] = ReadExpGolomb(BitB);
                    }
                }
            }
            Dec->SPS->AdditionalExtensionFlag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
        }
    }
    
    void ParseSEIToneMappingInfo(DecodeAVC *Dec, BitBuffer *BitB) { // tone_mapping_info
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIToneMappingInfo", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIToneMappingInfo", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->ToneMapID  = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            Dec->SEI->ToneMapCancelFlag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->SEI->ToneMapCancelFlag == false) {
                Dec->SEI->ToneMapRepetitionPeriod = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                Dec->SEI->CodedDataBitDepth       = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
                Dec->SEI->TargetBitDepth   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
                Dec->SEI->ToneMapModelID = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                if (Dec->SEI->ToneMapModelID == 0) {
                    Dec->SEI->ToneMinValue = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32);
                    Dec->SEI->ToneMaxValue = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32);
                } else if (Dec->SEI->ToneMapModelID == 1) {
                    Dec->SEI->SigmoidMidpoint = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32);
                    Dec->SEI->SigmoidWidth    = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32);
                } else if (Dec->SEI->ToneMapModelID == 2) {
                    for (uint16_t I = 0; I < (1 << Dec->SEI->TargetBitDepth); I++) {
                        Dec->SEI->StartOfCodedInterval[I] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, ((Dec->SEI->CodedDataBitDepth + 7) >> 3) << 3);
                    }
                } else if (Dec->SEI->ToneMapModelID == 3) {
                    Dec->SEI->NumPivots = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
                    for (uint16_t I = 0; I < Dec->SEI->NumPivots; I++) {
                        Dec->SEI->CodedPivotValue[I] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, ((Dec->SEI->CodedDataBitDepth + 7) >> 3) << 3);
                        Dec->SEI->TargetPivotValue[I] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, ((Dec->SEI->TargetBitDepth + 7) >> 3) << 3);
                    }
                } else if (Dec->SEI->ToneMapModelID == 4) {
                    Dec->SEI->CameraISOSpeedIDC = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
                    if (Dec->SEI->CameraISOSpeedIDC == ExtendedISO) {
                        Dec->SEI->CameraISOSpeed = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32);
                    }
                    Dec->SEI->ExposureIndexIDC = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
                    if (Dec->SEI->ExposureIndexIDC == ExtendedISO) {
                        Dec->SEI->ExposureIndexValue = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32);
                    }
                    Dec->SEI->ExposureCompensationSignFlag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                    Dec->SEI->ExposureCompensationNumerator = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
                    Dec->SEI->ExposureCompensationDenominatorIDC = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
                    Dec->SEI->RefScreenLuminanceWhite = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32);
                    Dec->SEI->ExtendedRangeWhiteLevel = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32);
                    Dec->SEI->NominalBlackLumaCode    = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
                    Dec->SEI->NominalWhiteLumaCode    = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
                    Dec->SEI->ExtendedWhiteLumaCode   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
                }
            }
        }
    }
    
    void ParseSEIScalabilityInfo(DecodeAVC *Dec, BitBuffer *BitB) { // scalability_info
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIScalabilityInfo", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIScalabilityInfo", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->TemporalIDNestingFlag                      = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            Dec->SEI->PriorityLayerInfoPresent                   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            Dec->SEI->PriorityIDSettingFlag                      = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            Dec->SEI->NumLayers                                  = ReadExpGolomb(BitB, false) + 1;
            for (uint8_t Layer = 0; Layer < Dec->SEI->NumLayers; Layer++) {
                Dec->SEI->LayerID[Layer]                         = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                Dec->NAL->PriorityID[Layer]                      = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 6);
                Dec->NAL->IsDisposable[Layer]                    = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                Dec->NAL->DependencyID[Layer]                    = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 3);
                Dec->NAL->QualityID[0][Layer]                    = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 4); // FIXME; is this correct?
                Dec->NAL->TemporalID[Layer]                      = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 3);
                Dec->SEI->SubPicLayerFlag[Layer]                 = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                Dec->SEI->SubRegionLayerFlag[Layer]              = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                Dec->SEI->IROIDivisionFlag[Layer]                = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                Dec->SEI->ProfileLevelInfoPresentFlag[Layer]     = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                Dec->SEI->BitRateInfoPresent[Layer]              = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                Dec->SEI->FrameRateInfoPresent[Layer]            = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                Dec->SEI->FrameSizeInfoPresentFlag[Layer]        = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                Dec->SEI->LayerDependencyInfoPresent[Layer]      = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                Dec->SEI->ParameterSetsInfoPresent[Layer]        = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                Dec->SEI->BitstreamRestrictionInfoPresent[Layer] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                Dec->SEI->ExactInterLayerPredFlag[Layer]         = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                if ((Dec->SEI->SubPicLayerFlag[Layer] == true) || (Dec->SEI->IROIDivisionFlag[Layer] == true)) {
                    Dec->SEI->ExactSampleValueMatchFlag[Layer]   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                }
                Dec->SEI->LayerConversionFlag[Layer]             = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                Dec->SEI->LayerOutputFlag[Layer]                 = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                if (Dec->SEI->ProfileLevelInfoPresentFlag[Layer] == true) {
                    Dec->SEI->LayerProfileLevelID[Layer]         = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 24);
                }
                if (Dec->SEI->BitRateInfoPresent[Layer] == true) {
                    Dec->SEI->AvgBitRate[Layer]                  = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
                    Dec->SEI->MaxBitRate[Layer]                  = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
                    Dec->SEI->MaxBitRateRepresentation[Layer]    = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
                    Dec->SEI->MaxBitRateWindow[Layer]            = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
                }
                if (Dec->SEI->FrameRateInfoPresent[Layer] == true) {
                    Dec->SEI->ConstantFrameRateIDC[Layer]        = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 2);
                    Dec->SEI->AvgFrameRate[Layer]                = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
                }
                if ((Dec->SEI->FrameSizeInfoPresentFlag[Layer] == true) || (Dec->SEI->IROIDivisionFlag[Layer] == true)) {
                    Dec->SEI->FrameWidthInMacroBlocks[Layer]     = ReadExpGolomb(BitB, false) + 1;
                    Dec->SEI->FrameHeightInMacroBlocks[Layer]    = ReadExpGolomb(BitB, false) + 1;
                }
                if (Dec->SEI->SubRegionLayerFlag[Layer] == true) {
                    Dec->SEI->BaseRegionLayerID[Layer]           = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    Dec->SEI->DynamicRectFlag[Layer]             = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                    if (Dec->SEI->DynamicRectFlag[Layer] == false) {
                        Dec->SEI->HorizontalOffset[Layer]        = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
                        Dec->SEI->VerticalOffset[Layer]          = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
                        Dec->SEI->RegionWidth[Layer]             = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
                        Dec->SEI->RegionHeight[Layer]            = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
                    }
                }
                if (Dec->SEI->SubPicLayerFlag[Layer] == true) {
                    Dec->SEI->ROIID[Layer]                       = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                }
                if (Dec->SEI->IROIDivisionFlag[Layer] == true) {
                    Dec->SEI->IROIGridFlag[Layer]                = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                    if (Dec->SEI->IROIGridFlag[Layer] == true) {
                        Dec->SEI->GridWidthInMacroBlocks[Layer]  = ReadExpGolomb(BitB, false) + 1;
                        Dec->SEI->GridHeightInMacroBlocks[Layer] = ReadExpGolomb(BitB, false) + 1;
                    } else {
                        Dec->SEI->NumROIs[Layer]                       = ReadExpGolomb(BitB, false) + 1;
                        for (uint8_t ROI = 0; ROI < Dec->SEI->NumROIs[Layer]; ROI++) {
                            Dec->SEI->FirstMacroBlockInROI[Layer][ROI] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                            Dec->SEI->ROIWidthInMacroBlock[Layer][ROI] = ReadExpGolomb(BitB, false) + 1;
                            Dec->SEI->ROIHeightInMacroBlock[Layer][ROI] = ReadExpGolomb(BitB, false) + 1;
                            
                        }
                    }
                }
                if (Dec->SEI->LayerDependencyInfoPresent[Layer] == true) {
                    Dec->SEI->NumDependentLayers[Layer]          = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    for (uint8_t DependentLayer = 0; DependentLayer < Dec->SEI->NumDependentLayers[Layer]; DependentLayer++) {
                        Dec->SEI->DirectlyDependentLayerIDDelta[Layer][DependentLayer] = ReadExpGolomb(BitB, false) + 1;
                    }
                } else {
                    Dec->SEI->LayerDepInfoSourceLayerIDDelta[Layer] = ReadExpGolomb(BitB, false) + 1;
                    if (Dec->SEI->ParameterSetsInfoPresent[Layer] == true) {
                        Dec->SEI->NumSequenceParameterSets[Layer] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                        for (uint8_t SPS = 0; SPS < Dec->SEI->NumSequenceParameterSets[Layer]; SPS++) {
                            Dec->SEI->SPSIDDelta[Layer][SPS] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                        }
                        Dec->SEI->NumSubsetSPS[Layer]    = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                        for (uint8_t SubsetSPS = 0; SubsetSPS < Dec->SEI->NumSubsetSPS[Layer]; SubsetSPS++) {
                            Dec->SEI->SubsetSPSIDDelta[Layer][SubsetSPS] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                        }
                        Dec->SEI->NumPicParameterSets[Layer] = ReadExpGolomb(BitB, false) + 1;
                        for (uint8_t PicParameterSet = 0; PicParameterSet < Dec->SEI->NumPicParameterSets[Layer]; PicParameterSet++) {
                            Dec->SEI->PicParameterSetIDDelta[Layer][PicParameterSet] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                        }
                    } else {
                        Dec->SEI->PPSInfoSrcLayerIDDelta[Layer] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    }
                    if (Dec->SEI->BitstreamRestrictionInfoPresent[Layer] == true) {
                        Dec->VUI->MotionVectorsOverPicBoundaries[Layer] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                        Dec->VUI->MaxBytesPerPicDenom[Layer]            = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                        Dec->VUI->MaxBitsPerMarcoBlockDenom[Layer]      = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                        Dec->VUI->MaxMotionVectorLength[Layer]          = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                        Dec->VUI->MaxMotionVectorHeight[Layer]          = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                        Dec->VUI->MaxReorderFrames[Layer]               = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                        Dec->VUI->MaxFrameBuffer[Layer]                 = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    }
                    if (Dec->SEI->LayerConversionFlag[Layer] == true) {
                        Dec->SEI->ConversionTypeIDC[Layer]              = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                        for (uint8_t J = 0; J < 2; J++) {
                            Dec->SEI->RewritingInfoFlag[Layer][J]       = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                            if (Dec->SEI->RewritingInfoFlag[Layer][J] == true) {
                                Dec->SEI->RewritingProfileLevelIDC[Layer][J] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 24);
                                Dec->SEI->RewritingAverageBitrate[Layer][J]  = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
                                Dec->SEI->RewritingMaxBitrate[Layer][J]      = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
                            }
                        }
                    }
                }
            }
            if (Dec->SEI->PriorityLayerInfoPresent == true) {
                Dec->SEI->NumDependencyLayersForPriorityLayer = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1, true) + 1;
                for (uint8_t DependentLayer = 0; DependentLayer < Dec->SEI->NumDependencyLayersForPriorityLayer; DependentLayer++) {
                    Dec->SEI->PriorityDependencyID[DependentLayer] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 3);
                    Dec->SEI->NumPriorityLayers[DependentLayer]    = ReadExpGolomb(BitB, false) + 1;
                    for (uint8_t PriorityLayer = 0; PriorityLayer < Dec->SEI->NumPriorityLayers[DependentLayer]; PriorityLayer++) {
                        Dec->SEI->PriorityLayerID[DependentLayer][PriorityLayer]   = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                        Dec->SEI->PriorityLevelIDC[DependentLayer][PriorityLayer]  = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 24);
                        Dec->SEI->PriorityLevelAvgBitrate[DependentLayer][PriorityLayer]  = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
                        Dec->SEI->PriorityLevelMaxBitrate[DependentLayer][PriorityLayer]  = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
                    }
                }
            }
            if (Dec->SEI->PriorityIDSettingFlag == true) {
                Dec->SEI->PriorityIDURIIndex = 0;
                Dec->SEI->PriorityIDSettingURI = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
                for (uint8_t StringByte = Dec->SEI->PriorityIDSettingURI; StringByte > 0; StringByte--) {
                    Dec->SEI->PriorityIDSettingURIString[StringByte] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
                }
            }
        }
    }
    
    void ParseSEISubPictureScalableLayer(DecodeAVC *Dec, BitBuffer *BitB) { // sub_pic_scalable_layer
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEISubPictureScalableLayer", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEISubPictureScalableLayer", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->LayerID[0] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
        }
    }
    
    void ParseSEINonRequiredLayerRep(DecodeAVC *Dec, BitBuffer *BitB) { // non_required_layer_rep
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEINonRequiredLayerRep", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEINonRequiredLayerRep", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->NumInfoEntries = ReadExpGolomb(BitB, false) + 1;
            for (uint32_t InfoEntry = 0; InfoEntry < Dec->SEI->NumInfoEntries; InfoEntry++) {
                Dec->SEI->EntryDependencyID[InfoEntry] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 3);
                Dec->SEI->NumNonRequiredLayerReps[InfoEntry] = ReadExpGolomb(BitB, false) + 1;
                for (uint8_t LayerRep = 0; LayerRep < Dec->SEI->NumNonRequiredLayerReps[InfoEntry]; LayerRep++) {
                    Dec->SEI->NonRequiredLayerRepDependencyID[InfoEntry][LayerRep] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 3);
                    Dec->SEI->NonRequiredLayerRepQualityID[InfoEntry][LayerRep] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 4);
                }
            }
        }
    }
    
    void ParseSEIPriorityLayerInfo(DecodeAVC *Dec, BitBuffer *BitB) { // priority_layer_info
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIPriorityLayerInfo", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIPriorityLayerInfo", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->PriorityDependencyID[0] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 3);
            Dec->SEI->NumPriorityIDs          = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 4);
            for (uint8_t PriorityID = 0; PriorityID < Dec->SEI->NumPriorityIDs; PriorityID++) {
                Dec->SEI->AltPriorityIDs[PriorityID] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 6);
            }
        }
    }
    
    void ParseSEILayersNotPresent(DecodeAVC *Dec, BitBuffer *BitB) { // layers_not_present
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEILayersNotPresent", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEILayersNotPresent", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->NumLayersNotPresent = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            for (uint8_t Layer = 0; Layer < Dec->SEI->NumLayersNotPresent; Layer++) {
                Dec->SEI->LayerID[Layer] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            }
        }
    }
    
    void ParseSEILayerDependencyChange(DecodeAVC *Dec, BitBuffer *BitB) { // layer_dependency_change
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEILayerDependencyChange", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEILayerDependencyChange", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->NumLayers = ReadExpGolomb(BitB, false) + 1;
            for (uint8_t Layer = 0; Layer < Dec->SEI->NumLayers; Layer++) {
                Dec->SEI->LayerID[Layer] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                Dec->SEI->LayerDependencyInfoPresent[Layer] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                if (Dec->SEI->LayerDependencyInfoPresent[Layer] == true) {
                    Dec->SEI->NumDependentLayers[Layer] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    for (uint8_t DependentLayer = 0; DependentLayer < Dec->SEI->NumDependentLayers[Layer]; DependentLayer++) {
                        Dec->SEI->DirectlyDependentLayerIDDelta[Layer][DependentLayer] = ReadExpGolomb(BitB, false) + 1;
                    }
                } else {
                    Dec->SEI->LayerDepInfoSourceLayerIDDelta[Layer] = ReadExpGolomb(BitB, false) + 1;
                }
            }
        }
    }
    
    void ParseSEIScalableNesting(DecodeAVC *Dec, BitBuffer *BitB) { // scalable_nesting
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIScalableNesting", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIScalableNesting", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->AllLayerRepresentationsInAUFlag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->SEI->AllLayerRepresentationsInAUFlag == false) {
                Dec->SEI->NumLayerRepresentations = ReadExpGolomb(BitB, false) + 1;
                for (uint8_t LayerRepresentation = 0; LayerRepresentation < Dec->SEI->NumLayerRepresentations; LayerRepresentation++) {
                    Dec->SEI->SEIDependencyID[LayerRepresentation] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 3);
                    Dec->SEI->SEIQualityID[LayerRepresentation]    = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 4);
                }
                Dec->SEI->SEITemporalID[0] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 3);
            }
            AlignBitBuffer(BitB, 1);
            ParseSEIMessage(Dec, BitB); // sei_message();
        }
    }
    
    void ParseSEIBaseLayerTemporalHRD(DecodeAVC *Dec, BitBuffer *BitB) { // base_layer_temporal_hrd
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIBaseLayerTemporalHRD", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIBaseLayerTemporalHRD", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->NumTemporalLayersInBaseLayer                 = ReadExpGolomb(BitB, false) + 1;
            for (uint8_t TemporalLayer = 0; TemporalLayer < Dec->SEI->NumTemporalLayersInBaseLayer; TemporalLayer++) {
                Dec->SEI->SEITemporalID[TemporalLayer]             = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 3);
                Dec->SEI->SEITimingInfoPresent[TemporalLayer]      = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                if (Dec->SEI->SEITimingInfoPresent[TemporalLayer] == true) {
                    Dec->SEI->SEIUnitsInTick[TemporalLayer]        = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32);
                    Dec->SEI->SEITimeScale[TemporalLayer]          = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32);
                    Dec->SEI->SEIFixedFrameRateFlag[TemporalLayer] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                }
                Dec->SEI->SEINALHRDParamsPresentFlag[TemporalLayer] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                if (Dec->SEI->SEINALHRDParamsPresentFlag[TemporalLayer] == true) {
                    ParseHypotheticalReferenceDecoder(Dec, BitB);
                }
                Dec->SEI->SEIVCLHRDParamsPresentFlag[TemporalLayer] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                if (Dec->SEI->SEIVCLHRDParamsPresentFlag[TemporalLayer] == true) {
                    ParseHypotheticalReferenceDecoder(Dec, BitB);
                }
                if ((Dec->SEI->SEINALHRDParamsPresentFlag[TemporalLayer] == true) || (Dec->SEI->SEIVCLHRDParamsPresentFlag[TemporalLayer] == true)) {
                    Dec->SEI->SEILowDelayHRDFlag[TemporalLayer] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                }
                Dec->SEI->SEIPicStructPresentFlag[TemporalLayer] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            }
        }
    }
    
    void ParseSEIQualityLayerIntegrityCheck(DecodeAVC *Dec, BitBuffer *BitB) { // quality_layer_integrity_check
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIQualityLayerIntegrityCheck", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIQualityLayerIntegrityCheck", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->NumInfoEntries = ReadExpGolomb(BitB, false) + 1;
            for (uint8_t IntegrityCheck = 0; IntegrityCheck < Dec->SEI->NumInfoEntries; IntegrityCheck++) {
                Dec->SEI->EntryDependencyID[IntegrityCheck] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 3);
                Dec->SEI->SEIQualityLayerCRC[IntegrityCheck] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
            }
        }
    }
    
    void ParseSEIRedundantPicProperty(DecodeAVC *Dec, BitBuffer *BitB) { // redundant_pic_property
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIRedundantPicProperty", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIRedundantPicProperty", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->NumDependencyIDs = ReadExpGolomb(BitB, false) + 1;
            for (uint8_t DependencyID = 0; DependencyID < Dec->SEI->NumDependencyIDs; DependencyID++) {
                Dec->NAL->DependencyID[DependencyID]  = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 3);
                Dec->SEI->NumQualityIDs[DependencyID] = ReadExpGolomb(BitB, false) + 1;
                for (uint8_t QualityID = 0; QualityID < Dec->SEI->NumQualityIDs[DependencyID]; QualityID++) {
                    Dec->NAL->QualityID[DependencyID][QualityID] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 4);
                    Dec->SEI->NumRedundantPics[DependencyID][QualityID] = ReadExpGolomb(BitB, false) + 1;
                    for (uint8_t RedundantPic = 0; RedundantPic < Dec->SEI->NumRedundantPics[DependencyID][QualityID]; RedundantPic++) {
                        Dec->SEI->RedundantPicCount[DependencyID][QualityID][RedundantPic] = ReadExpGolomb(BitB, false) + 1;
                        Dec->SEI->RedundantPicsMatch[DependencyID][QualityID][RedundantPic] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                        if (Dec->SEI->RedundantPicsMatch[DependencyID][QualityID][RedundantPic] == false) {
                            Dec->SEI->MBTypeMatchFlag[DependencyID][QualityID][RedundantPic] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                            Dec->SEI->MotionMatchFlag[DependencyID][QualityID][RedundantPic] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                            Dec->SEI->ResidualMatchFlag[DependencyID][QualityID][RedundantPic] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                            Dec->SEI->IntraSamplesMatchFlag[DependencyID][QualityID][RedundantPic] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                        }
                    }
                }
            }
        }
    }
    
    void ParseSEITemporalDependencyRepresentationIndex(DecodeAVC *Dec, BitBuffer *BitB) { // tl0_dep_rep_index
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEITemporalDependencyRepresentationIndex", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEITemporalDependencyRepresentationIndex", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->TemporalDependencyRepresentationIndexLevel0 = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
            Dec->SEI->EffectiveIDRPicID                           = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
        }
    }
    
    void ParseSEITemporalLevelSwitchingPoint(DecodeAVC *Dec, BitBuffer *BitB) { // tl_switching_point
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEITemporalLevelSwitchingPoint", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEITemporalLevelSwitchingPoint", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->DeltaFrameNum = ReadExpGolomb(BitB);
        }
    }
    
    void ParseSEIParallelDecodingInfo(DecodeAVC *Dec, BitBuffer *BitB) { // parallel_decoding_info
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIParallelDecodingInfo", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIParallelDecodingInfo", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SPS->SeqParamSetID = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
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
    }
    
    void ParseSEIMVCScalableNesting(DecodeAVC *Dec, BitBuffer *BitB) { // mvc_scalable_nesting
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIMVCScalableNesting", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIMVCScalableNesting", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->OperationPointFlag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->SEI->OperationPointFlag == false) {
                Dec->SEI->AllViewComponentsInAUFlag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                if (Dec->SEI->AllViewComponentsInAUFlag == false) {
                    Dec->SEI->NumViewComponents = ReadExpGolomb(BitB, false) + 1;
                    for (uint8_t ViewComponent = 0; ViewComponent < Dec->SEI->NumViewComponents; ViewComponent++) {
                        Dec->SEI->SEIViewID[ViewComponent] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 10);
                    }
                }
            } else {
                Dec->SEI->NumViewComponentsOp = ReadExpGolomb(BitB, false) + 1;
                for (uint8_t ViewComponent = 0; ViewComponent < Dec->SEI->NumViewComponentsOp; ViewComponent++) {
                    Dec->SEI->SEIOpViewID[ViewComponent] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 10);
                }
                Dec->SEI->SEIOpTemporalID = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 3);
            }
            AlignBitBuffer(BitB, 1);
            ParseSEIMessage(Dec, BitB); // sei_message();
        }
    }
    
    void ParseSEIViewScalabilityInfo(DecodeAVC *Dec, BitBuffer *BitB) { // view_scalability_info FIXME: FINISH THIS FUNCTION!!!
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIViewScalabilityInfo", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIViewScalabilityInfo", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->NumOperationPoints                         = ReadExpGolomb(BitB, false) + 1;
            for (uint8_t OperationPoint = 0; OperationPoint < Dec->SEI->NumOperationPoints; OperationPoint++) {
                Dec->SEI->OperationPointID[OperationPoint]       = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                Dec->NAL->PriorityID[OperationPoint]             = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 5);
                Dec->NAL->TemporalID[OperationPoint]             = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 3);
                Dec->SEI->NumTargetOutputViews[OperationPoint]   = ReadExpGolomb(BitB, false) + 1;
                for (uint8_t OutputView = 0; OutputView < Dec->SEI->NumTargetOutputViews[OperationPoint]; OutputView++) {
                    Dec->SPS->ViewID[OperationPoint][OutputView] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                }
                Dec->SEI->ProfileLevelInfoPresentFlag[0]         = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                Dec->SEI->BitRateInfoPresent[0]                  = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                Dec->SEI->FrameRateInfoPresent[0]                = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                if (Dec->SEI->NumTargetOutputViews[OperationPoint] == false) {
                    Dec->SEI->ViewDependencyInfoPresent[OperationPoint] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                }
                Dec->SEI->ParameterSetsInfoPresent[OperationPoint]        = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                Dec->SEI->BitstreamRestrictionInfoPresent[OperationPoint] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                if (Dec->SEI->ProfileLevelInfoPresentFlag[0] == true) {
                    Dec->SEI->OpProfileLevelIDC[OperationPoint]           = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 24);
                }
                if (Dec->SEI->BitstreamRestrictionInfoPresent[OperationPoint] == true) {
                    Dec->SEI->AvgBitRate[OperationPoint]                  = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
                    Dec->SEI->MaxBitRate[OperationPoint]                  = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
                    Dec->SEI->MaxBitRateWindow[OperationPoint]            = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
                }
                if (Dec->SEI->FrameRateInfoPresent[0] == true) {
                    Dec->SEI->ConstantFrameRateIDC[OperationPoint]        = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 2);
                    Dec->SEI->AvgFrameRate[OperationPoint]                = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
                }
                if (Dec->SEI->ViewDependencyInfoPresent[OperationPoint] == true) {
                    Dec->SEI->NumDirectlyDependentViews[OperationPoint]   = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    for (uint8_t DirectDependentView = 0; DirectDependentView < Dec->SEI->NumDirectlyDependentViews[OperationPoint]; DirectDependentView++) {
                        Dec->SEI->DirectlyDependentViewID[OperationPoint][DirectDependentView] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    }
                } else {
                    Dec->SEI->ViewDependencyInfoSrcOpID[OperationPoint]   = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                }
                if (Dec->SEI->ParameterSetsInfoPresent[OperationPoint] == true) {
                    Dec->SEI->NumSequenceParameterSets[OperationPoint] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    for (uint8_t SPS = 0; SPS < Dec->SEI->NumSequenceParameterSets[OperationPoint]; SPS++) {
                        Dec->SEI->SPSIDDelta[OperationPoint][SPS] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    }
                    Dec->SEI->NumSubsetSPS[OperationPoint] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    for (uint8_t SubsetSPS = 0; SubsetSPS < Dec->SEI->NumSubsetSPS[OperationPoint]; SubsetSPS++) {
                        Dec->SEI->SubsetSPSIDDelta[OperationPoint][SubsetSPS] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    }
                    Dec->SEI->NumPicParameterSets[OperationPoint] = ReadExpGolomb(BitB, false) + 1;
                    for (uint8_t PPS = 0; PPS < Dec->SEI->NumPicParameterSets[OperationPoint]; PPS++) {
                        Dec->SEI->PicParameterSetIDDelta[OperationPoint][PPS] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    }
                } else {
                    Dec->SEI->PPSInfoSrcLayerIDDelta[OperationPoint] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                }
                if (Dec->SEI->BitstreamRestrictionInfoPresent[OperationPoint] == true) {
                    Dec->VUI->MotionVectorsOverPicBoundaries[OperationPoint]  = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                    Dec->VUI->MaxBytesPerPicDenom[OperationPoint]             = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    Dec->VUI->MaxBitsPerMarcoBlockDenom[OperationPoint]       = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    Dec->VUI->MaxMotionVectorLength[OperationPoint]           = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    Dec->VUI->MaxMotionVectorHeight[OperationPoint]           = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    Dec->VUI->MaxReorderFrames[OperationPoint]                = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    Dec->VUI->MaxFrameBuffer[OperationPoint]                  = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                }
                if (Dec->SEI->LayerConversionFlag[OperationPoint] == true) {
                    Dec->SEI->ConversionTypeIDC[OperationPoint] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    for (uint8_t J = 0; J < 2; J++) {
                        Dec->SEI->RewritingInfoFlag[OperationPoint][J] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                        for (uint8_t RewriteInfo = 0; RewriteInfo < Dec->SEI->RewritingInfoFlag[OperationPoint][J]; RewriteInfo++) {
                            Dec->SEI->RewritingProfileLevelIDC[OperationPoint][J] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 24);
                            Dec->SEI->RewritingAverageBitrate[OperationPoint][J]  = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
                            Dec->SEI->RewritingMaxBitrate[OperationPoint][J]      = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
                        }
                    }
                }
            }
            if (Dec->SEI->PriorityLayerInfoPresent == true) {
                Dec->SEI->NumDependencyLayersForPriorityLayer                             = ReadExpGolomb(BitB, false) + 1;
                for (uint8_t DependencyLayer = 0; DependencyLayer < Dec->SEI->NumDependencyLayersForPriorityLayer; DependencyLayer++) {
                    Dec->SEI->PriorityDependencyID[DependencyLayer]                       = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 3);
                    Dec->SEI->NumPriorityLayers[DependencyLayer]                          = ReadExpGolomb(BitB, false) + 1;
                    for (uint8_t PriorityLayer = 0; PriorityLayer < Dec->SEI->NumPriorityLayers[DependencyLayer]; PriorityLayer++) {
                        Dec->SEI->PriorityLayerID[DependencyLayer][PriorityLayer]         = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                        Dec->SEI->PriorityLevelIDC[DependencyLayer][PriorityLayer]        = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 24);
                        Dec->SEI->PriorityLevelAvgBitrate[DependencyLayer][PriorityLayer] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
                        Dec->SEI->PriorityLevelMaxBitrate[DependencyLayer][PriorityLayer] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
                    }
                }
            }
            if (Dec->SEI->PriorityIDSettingFlag == true) {
                Dec->SEI->PriorityIDURIIndex = 0;
                Dec->SEI->PriorityIDSettingURI = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
                for (uint8_t StringByte = Dec->SEI->PriorityIDSettingURI; StringByte > 0; StringByte--) {
                    Dec->SEI->PriorityIDSettingURIString[StringByte] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
                }
            }
        }
    }
    
    void ParseSEIMVCSceneInfo(DecodeAVC *Dec, BitBuffer *BitB) { // multiview_scene_info
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIMVCSceneInfo", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIMVCSceneInfo", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->MaxDisparity = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
        }
    }
    
    void ParseSEIMVCAcquisitionInfo(DecodeAVC *Dec, BitBuffer *BitB) { // multiview_acquisition_info
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIMVCAcquisitionInfo", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIMVCAcquisitionInfo", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SPS->ViewCount            = ReadExpGolomb(BitB, false) + 1;
            Dec->SEI->IntrinsicParamFlag   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            Dec->SEI->ExtrinsicParamFlag   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->SEI->IntrinsicParamFlag == true) {
                Dec->SEI->IntrinsicParamsEqual = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                Dec->SEI->PrecFocalLength      = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                Dec->SEI->PrecPrincipalPoint   = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                Dec->SEI->PrecSkewFactor       = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                if (Dec->SEI->IntrinsicParamsEqual == true) {
                    Dec->SEI->NumParamSets     = 1;
                } else {
                    Dec->SEI->NumParamSets     = Dec->SPS->ViewCount;
                }
                for (uint8_t ParamSet = 0; ParamSet < Dec->SEI->NumParamSets; ParamSet++) { // FIXME: ReadBits(BitIOMSByte, BitIOLSBit, BitB, 0, true)
                    Dec->SEI->SignFocalLength[0][ParamSet]        = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                    Dec->SEI->ExponentFocalLength[0][ParamSet]    = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 6);
                    if (Dec->SEI->ExponentFocalLength[0][ParamSet] == 0) {
                        Dec->SEI->MantissaFocalLength[0][ParamSet] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, Max(0, Dec->SEI->PrecFocalLength - 30));
                    } else {
                        Dec->SEI->MantissaFocalLength[0][ParamSet] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, Max(0, Dec->SEI->ExponentFocalLength[0][ParamSet] + Dec->SEI->PrecFocalLength - 31));
                    }
                    Dec->SEI->SignFocalLength[1][ParamSet]        = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                    Dec->SEI->ExponentFocalLength[1][ParamSet]    = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 6);
                    if (Dec->SEI->ExponentFocalLength[1][ParamSet] == 0) {
                        Dec->SEI->MantissaFocalLength[1][ParamSet] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, Max(0, Dec->SEI->PrecFocalLength - 30));
                    } else {
                        Dec->SEI->MantissaFocalLength[1][ParamSet] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, Max(0, Dec->SEI->ExponentFocalLength[1][ParamSet] + Dec->SEI->PrecFocalLength - 31));
                    }
                    Dec->SEI->SignPrincipalPoint[0][ParamSet]     = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                    Dec->SEI->ExponentPrincipalPoint[0][ParamSet] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 6);
                    if (Dec->SEI->ExponentFocalLength[1][ParamSet] == 0) {
                        Dec->SEI->MantissaPrincipalPoint[0][ParamSet] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, Max(0, Dec->SEI->PrecPrincipalPoint - 30));
                    } else {
                        Dec->SEI->MantissaPrincipalPoint[0][ParamSet] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, Max(0, Dec->SEI->ExponentPrincipalPoint[0][ParamSet] + Dec->SEI->PrecPrincipalPoint - 31));
                    }
                    Dec->SEI->SignPrincipalPoint[1][ParamSet]     = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                    Dec->SEI->ExponentPrincipalPoint[1][ParamSet] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 6);
                    if (Dec->SEI->ExponentFocalLength[1][ParamSet] == 0) {
                        Dec->SEI->MantissaPrincipalPoint[1][ParamSet] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, Max(0, Dec->SEI->PrecPrincipalPoint - 30));
                    } else {
                        Dec->SEI->MantissaPrincipalPoint[1][ParamSet] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, Max(0, Dec->SEI->ExponentPrincipalPoint[1][ParamSet] + Dec->SEI->PrecPrincipalPoint - 31));
                    }
                    Dec->SEI->SignSkewFactor[ParamSet]            = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                    Dec->SEI->ExponentSkewFactor[ParamSet]        = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 6);
                    if (Dec->SEI->ExponentSkewFactor[ParamSet] == 0) {
                        Dec->SEI->MantissaSkewFactor[ParamSet] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, Max(0, Dec->SEI->PrecSkewFactor - 30));
                    } else {
                        Dec->SEI->MantissaSkewFactor[ParamSet] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, Max(0, Dec->SEI->ExponentSkewFactor[ParamSet] + Dec->SEI->PrecSkewFactor - 31));
                    }
                }
            }
            if (Dec->SEI->ExtrinsicParamFlag == true) {
                for (uint8_t View = 0; View < Dec->SPS->ViewCount; View++) {
                    for (uint8_t Row = 1; Row <= 3; Row++) {
                        for (uint8_t Column = 1; Column <= 3; Column++) {
                            Dec->SEI->SignR[View][Row][Column]     = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                            Dec->SEI->ExponentR[View][Row][Column] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 6);
                            Dec->SEI->MantissaR[View][Row][Column] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 0);
                        }
                        Dec->SEI->SignT[View][Row] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                        Dec->SEI->ExponentT[View][Row] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 6);
                        Dec->SEI->MantissaT[View][Row] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 0);
                    }
                }
            }
        }
    }
    
    void ParseSEINonRequiredViewComponent(DecodeAVC *Dec, BitBuffer *BitB) { // non_required_view_component
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEINonRequiredViewComponent", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEINonRequiredViewComponent", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->NumInfoEntries                               = ReadExpGolomb(BitB, false) + 1;
            for (uint8_t InfoEntry = 0; InfoEntry < Dec->SEI->NumInfoEntries; InfoEntry++) {
                Dec->SEI->ViewOrderIndex[InfoEntry]                = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                Dec->SEI->NumNonRequiredViewComponents[InfoEntry]  = ReadExpGolomb(BitB, false) + 1;
                for (uint8_t ViewComponent = 0; ViewComponent < Dec->SEI->NumNonRequiredViewComponents[InfoEntry]; ViewComponent++) {
                    Dec->SEI->IndexDelta[InfoEntry][ViewComponent] = ReadExpGolomb(BitB, false) + 1;
                }
            }
        }
    }
    
    void ParseSEIViewDependencyChange(DecodeAVC *Dec, BitBuffer *BitB) { // view_dependency_change
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIViewDependencyChange", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIViewDependencyChange", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SPS->SeqParamSetID       = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            Dec->SEI->AnchorUpdateFlag    = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            Dec->SEI->NonAnchorUpdateFlag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->SEI->AnchorUpdateFlag == true) {
                for (uint8_t View = 0; View < Dec->SPS->ViewCount; View++) {
                    for (uint8_t AnchorRef = 0; AnchorRef < Dec->SPS->AnchorRefsCount[0][View]; AnchorRef++) {
                        Dec->SEI->AnchorRefFlag[0][View][AnchorRef] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                    }
                    for (uint8_t AnchorRef = 0; AnchorRef < Dec->SPS->AnchorRefsCount[1][View]; AnchorRef++) {
                        Dec->SEI->AnchorRefFlag[1][View][AnchorRef] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                    }
                }
            }
            if (Dec->SEI->NonAnchorUpdateFlag == true) {
                for (uint8_t View = 0; View < Dec->SPS->ViewCount; View++) {
                    for (uint8_t NonAnchorRef = 0; NonAnchorRef < Dec->SPS->NonAnchorRefCount[0][View]; NonAnchorRef++) {
                        Dec->SEI->NonAnchorRefFlag[0][View][NonAnchorRef] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                    }
                    for (uint8_t NonAnchorRef = 0; NonAnchorRef < Dec->SPS->NonAnchorRefCount[1][View]; NonAnchorRef++) {
                        Dec->SEI->NonAnchorRefFlag[1][View][NonAnchorRef] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                    }
                }
            }
        }
    }
    
    void ParseSEIOperationPointNotPresent(DecodeAVC *Dec, BitBuffer *BitB) { // operation_point(s)_not_present
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIOperationPointNotPresent", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIOperationPointNotPresent", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->NumOperationPoints = ReadExpGolomb(BitB, false) + 1;
            for (uint8_t OperationPoint = 0; OperationPoint < Dec->SEI->NumOperationPoints; OperationPoint++) {
                Dec->SEI->OperationPointNotPresentID[OperationPoint] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            }
        }
    }
    
    void ParseSEIBaseViewTemporalHRD(DecodeAVC *Dec, BitBuffer *BitB) { // base_view_temporal_hrd
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIBaseViewTemporalHRD", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIBaseViewTemporalHRD", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->NumTemporalLayersInBaseView = ReadExpGolomb(BitB, false) + 1;
            for (uint8_t TemporalLayer = 0; TemporalLayer < Dec->SEI->NumTemporalLayersInBaseView; TemporalLayer++) {
                Dec->SEI->SEIMVCTemporalID[TemporalLayer] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 3);
            }
        }
    }
    
    void ParseSEIFramePackingArrangement(DecodeAVC *Dec, BitBuffer *BitB) { // frame_packing_arrangement
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIFramePackingArrangement", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIFramePackingArrangement", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->FramePackingArrangementID = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            Dec->SEI->FramePackingArrangementCancelFlag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->SEI->FramePackingArrangementCancelFlag == false) {
                Dec->SEI->FramePackingArrangementType = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 7);
                Dec->SEI->QuincunxSamplingFlag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                Dec->SEI->ContentIntrepretationType = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 6);
                Dec->SEI->SpatialFlippingFlag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                Dec->SEI->FlippedFlagFrame0 = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                Dec->SEI->FieldViewsFlag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                Dec->SEI->TheCurrentFrameIsFrame0 = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                Dec->SEI->Frame0IsSelfContainedFlag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                Dec->SEI->Frame1IsSelfContainedFlag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                if ((Dec->SEI->QuincunxSamplingFlag == false) && (Dec->SEI->FramePackingArrangementType != 5)) {
                    Dec->SEI->Frame0GridPositionX = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 4);
                    Dec->SEI->Frame0GridPositionY = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 4);
                    Dec->SEI->Frame1GridPositionX = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 4);
                    Dec->SEI->Frame1GridPositionY = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 4);
                }
                Dec->SEI->FramePackingArrangementReservedByte = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
                Dec->SEI->FramePackingArrangementRepetitionPeriod = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            }
            Dec->SEI->FramePackingArrangementExtensionFlag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
        }
    }
    
    void ParseSEIMVCViewPosition(DecodeAVC *Dec, BitBuffer *BitB) { // multiview_view_position
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIMVCViewPosition", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIMVCViewPosition", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SPS->ViewCount = ReadExpGolomb(BitB, false) + 1;
            for (uint8_t View = 0; View < Dec->SPS->ViewCount; View++) {
                Dec->SEI->ViewPosition[View] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            }
            Dec->SEI->MVCViewPositionExtensionFlag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
        }
    }
    
    void ParseSEIDisplayOrientation(DecodeAVC *Dec, BitBuffer *BitB) { // display_orientation
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIDisplayOrientation", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIDisplayOrientation", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->DisplayOrientationCancelFlag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->SEI->DisplayOrientationCancelFlag == false) {
                Dec->SEI->HorizontalFlip = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                Dec->SEI->VerticalFlip   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                Dec->SEI->CounterClockwiseRotation = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
                Dec->SEI->DisplayOrientationRepetitionPeriod = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                Dec->SEI->DisplayOrientationExtensionFlag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            }
        }
    }
    
    void ParseSEIDepthRepresentationInformation(DecodeAVC *Dec, BitBuffer *BitB) { // depth_representation_info
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIDepthRepresentationInformation", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIDepthRepresentationInformation", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->AllViewsEqual                                     = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->SEI->AllViewsEqual == true) {
                Dec->SPS->ViewCount                                     = ReadExpGolomb(BitB, false) + 1;
            } else {
                Dec->SPS->ViewCount                                     = 1;
            }
            Dec->SEI->ZNearFlag                                         = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            Dec->SEI->ZFarFlag                                          = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if ((Dec->SEI->ZNearFlag == true) || (Dec->SEI->ZFarFlag == true)) {
                Dec->SEI->ZAxisEqualFlag                                = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                if (Dec->SEI->ZAxisEqualFlag == true) {
                    Dec->SEI->CommonZAxisReferenceView                  = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                }
            }
            Dec->SEI->DMinFlag                                          = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            Dec->SEI->DMaxFlag                                          = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            Dec->SEI->DepthRepresentationType                           = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            for (uint8_t View = 0; View < Dec->SPS->ViewCount; View++) {
                Dec->SEI->DepthInfoViewID[View]                         = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                if ((Dec->SEI->ZNearFlag == true || Dec->SEI->ZFarFlag == true) && (Dec->SEI->ZAxisEqualFlag == false)) {
                    Dec->SEI->ZAxisReferenceView[View]                  = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                }
                if (Dec->SEI->DMinFlag == true || Dec->SEI->DMaxFlag == true) {
                    Dec->SEI->DisparityReferenceView[View]              = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
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
                    Dec->SEI->DepthNonlinearRepresentationModel[Index]  = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                }
            }
        }
    }
    
    void ParseSEI3DReferenceDisplayInfo(DecodeAVC *Dec, BitBuffer *BitB) { // three_dimensional_reference_displays_info
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEI3DReferenceDisplayInfo", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEI3DReferenceDisplayInfo", "Pointer to BitBuffer is NULL");
                } else {
                    Dec->SEI->TruncationErrorExponent                           = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    Dec->SEI->TruncatedWidthExponent                            = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    Dec->SEI->ReferenceViewingDistanceFlag                      = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                    
                    if (Dec->SEI->ReferenceViewingDistanceFlag == true) {
                        Dec->SEI->TruncatedReferenveViewingDistanceExponent     = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    }
                    Dec->SEI->NumReferenceDisplays                              = ReadExpGolomb(BitB, false) + 1;
                    for (uint8_t Display= 0; Display < Dec->SEI->NumReferenceDisplays; Display++) {
                        Dec->SEI->ReferenceBaselineExponent[Display]            = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                        Dec->SEI->ReferenceBaselineMantissa[Display]            = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                        Dec->SEI->ReferenceDisplayWidthExponent[Display]        = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                        Dec->SEI->ReferenceDisplayWidthMantissa[Display]        = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                        if (Dec->SEI->ReferenceViewingDistanceFlag == true) {
                            Dec->SEI->ReferenceViewingDistanceExponent[Display] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                            Dec->SEI->ReferenceViewingDistanceMantissa[Display] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                        }
                        Dec->SEI->ShiftPresentFlag[Display]                     = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                        if (Dec->SEI->ShiftPresentFlag[Display] == true) {
                            Dec->SEI->SampleShift[Display]                      = ReadExpGolomb(BitB, false) - 512;
                        }
                    }
                    Dec->SEI->ReferenceDisplays3DFlag                           = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                }
    }
    
    void ParseSEIDepthTiming(DecodeAVC *Dec, BitBuffer *BitB) { // depth_timing
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIDepthTiming", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIDepthTiming", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->PerViewDepthTimingFlag                              = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            
            if (Dec->SEI->PerViewDepthTimingFlag == true) {
                for (uint8_t View = 0; View < Dec->DPS->NumDepthViews; View++) {
                    Dec->SEI->OffsetLength[View]                          = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 5, true) + 1;
                    Dec->SEI->DepthDisplayDelayOffsetFP[View]             = ReadBits(BitIOMSByte, BitIOLSBit, BitB, Dec->SEI->OffsetLength[View]);
                    Dec->SEI->DepthDisplayDelayOffsetDP[View]             = ReadBits(BitIOMSByte, BitIOLSBit, BitB, Dec->SEI->OffsetLength[View]);
                }
            }
        }
    }
    
    void ParseSEIDepthGridPosition(DecodeAVC *Dec, BitBuffer *BitB) { // depth_grid_position()
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIDepthGridPosition", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIDepthGridPosition", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->DepthGridPositionXFP                                = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 20);
            Dec->SEI->DepthGridPositionXDP                                = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 4);
            Dec->SEI->DepthGridPositionXSignFlag                          = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            Dec->SEI->DepthGridPositionYFP                                = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 20);
            Dec->SEI->DepthGridPositionYDP                                = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 4);
            Dec->SEI->DepthGridPositionYSignFlag                          = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
        }
    }
    
    void ParseSEIDepthSamplingInfo(DecodeAVC *Dec, BitBuffer *BitB) { // depth_sampling_info
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIDepthSamplingInfo", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIDepthSamplingInfo", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->DepthSampleWidthMul                                 = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
            Dec->SEI->DepthSampleWidthDP                                  = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 4);
            Dec->SEI->DepthSampleHeightMul                                = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
            Dec->SEI->DepthSampleHeightDP                                 = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 4);
            Dec->SEI->PerViewDepthTimingFlag                              = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->SEI->PerViewDepthTimingFlag == true) {
                Dec->SEI->NumDepthGridViews                               = ReadExpGolomb(BitB, false) + 1;
                for (uint8_t DepthGrid = 0; DepthGrid < Dec->SEI->NumDepthGridViews; DepthGrid++) {
                    Dec->SEI->DepthInfoViewID[DepthGrid]                  = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    ParseSEIDepthGridPosition(Dec, BitB);
                }
            } else {
                ParseSEIDepthGridPosition(Dec, BitB);
            }
        }
    }
    
    void ParseSEIConstrainedDepthParameterSetID(DecodeAVC *Dec, BitBuffer *BitB) { // constrained_depth_parameter_set_identifier
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIConstrainedDepthParameterSetID", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIConstrainedDepthParameterSetID", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->MaxDPSID                                            = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
            Dec->SEI->MaxDPSIDDiff                                        = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
        }
    }
    
    void ParseSEIMeteringDisplayColorVolume(DecodeAVC *Dec, BitBuffer *BitB) { // mastering_display_color_volume
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIMeteringDisplayColorVolume", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIMeteringDisplayColorVolume", "Pointer to BitBuffer is NULL");
        } else {
            for (uint8_t Color = 0; Color < 3; Color++) {
                Dec->SEI->DisplayPrimariesX[Color]                        = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
                Dec->SEI->DisplayPrimariesY[Color]                        = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
            }
            Dec->SEI->WhitePointX                                         = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
            Dec->SEI->WhitePointY                                         = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
            Dec->SEI->MaxDisplayMasteringLuminance                        = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32);
            Dec->SEI->MinDisplayMasteringLuminance                        = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32);
        }
    }
    
    void ParseSEIMVCDScalableNesting(DecodeAVC *Dec, BitBuffer *BitB) { // mvcd_scalable_nesting
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIMVCDScalableNesting", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIMVCDScalableNesting", "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->OperationPointFlag                                  = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->SEI->OperationPointFlag == false) {
                Dec->SEI->AllViewComponentsInAUFlag                       = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                if (Dec->SEI->AllViewComponentsInAUFlag == false) {
                    Dec->SEI->NumViewComponents                           = ReadExpGolomb(BitB, false) + 1;
                    for (uint8_t ViewComponent = 0; ViewComponent < Dec->SEI->NumViewComponents; ViewComponent++) {
                        Dec->SEI->SEIViewID[ViewComponent]                = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 10);
                        Dec->SEI->SEIViewApplicabilityFlag[ViewComponent] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                    }
                }
            } else {
                Dec->SEI->SEIOpTextureOnlyFlag                            = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                Dec->SEI->NumViewComponents                               = ReadExpGolomb(BitB, false) + 1;
                for (uint8_t ViewComponent = 0; ViewComponent < Dec->SEI->NumViewComponents; ViewComponent++) {
                    Dec->SEI->SEIOpViewID[ViewComponent]                  = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 10);
                    if (Dec->SEI->SEIOpTextureOnlyFlag == false) {
                        Dec->SEI->SEIOpDepthFlag[ViewComponent]           = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                        Dec->SEI->SEIOpTextureFlag[ViewComponent]         = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                    }
                }
                Dec->SEI->SEIOpTemporalID                                 = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            }
            AlignBitBuffer(BitB, 1);
            ParseSEIMessage(Dec, BitB); // sei_message();
        }
    }
    
    void ParseSEIDepthRepresentationElement(BitBuffer *BitB, uint8_t OutSign, uint8_t OutExp, uint8_t OutMantissa, uint8_t OutManLen) { // depth_representation_sei_element
        if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIDepthRepresentationElement", "Pointer to BitBuffer is NULL");
        } else {
            bool     DASignFlag                                           = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1); // da_sign_flag
            uint8_t  DAExponent                                           = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 7); // da_exponent
            uint8_t  DAMatissaSize                                        = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 5, true) + 1;
            uint64_t DAMatissa                                            = ReadBits(BitIOMSByte, BitIOLSBit, BitB, DAMatissaSize);
        }
    }
    
    void SkipSEIReservedMessage(BitBuffer *BitB, size_t SEISize) { // reserved_sei_message
        if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "SkipSEIReservedMessage", "Pointer to BitBuffer is NULL");
        } else {
            SkipBits(BitB, Bytes2Bits(SEISize));
        }
    }
    
    void ParseSEIMessage(DecodeAVC *Dec, BitBuffer *BitB) { // sei_message
        if (Dec == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIMessage", "Pointer to DecodeAVC is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ParseSEIMessage", "Pointer to BitBuffer is NULL");
        } else {
            while (PeekBits(BitB, 8, true) == 0xFF) {
                SkipBits(BitB, 8);
                Dec->SEI->SEIType += 255;
            }
            Dec->SEI->SEIType += ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8); // last_payload_type_byte, 5
            
            while (PeekBits(BitB, 8, true) == 0xFF) {
                SkipBits(BitB, 8);
                Dec->SEI->SEISize    += 255;
            }
            Dec->SEI->SEISize += ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8); // last_payload_size_byte, 692, emulation prevention bytes not included, but these fields are.
            
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
    }
    
#ifdef __cplusplus
}
#endif
