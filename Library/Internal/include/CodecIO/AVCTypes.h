#include <stdbool.h>
#include <stdint.h>

#pragma once

#ifndef LIBMODERNAVC_AVCTYPES_H
#define LIBMODERNAVC_AVCTYPES_H

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef struct SequenceParameterSet { // Whole video variables
        /* Standard SPS */
        uint8_t      ProfileIDC:8;                                 // profile_idc
        bool         ConstraintFlag0:1;                            // constraint_set0_flag
        bool         ConstraintFlag1:1;                            // constraint_set1_flag
        bool         ConstraintFlag2:1;                            // constraint_set2_flag
        bool         ConstraintFlag3:1;                            // constraint_set3_flag
        bool         ConstraintFlag4:1;                            // constraint_set4_flag
        bool         ConstraintFlag5:1;                            // constraint_set5_flag
        uint8_t    **LevelIDC;                                     // level_idc
        uint8_t      SeqParamSetID:5;                              // seq_parameter_set_id
        uint8_t      ChromaFormatIDC:2;                            // chroma_format_idc
        uint8_t      MacroBlockWidthChroma:5;                      // MbWidthC
        uint8_t      MacroBlockHeightChroma:5;                     // MbHeightC
        bool         SeperateColorPlane:1;                         // seperate_color_plane_flag
        uint8_t      ChromaArrayType:2;                            // ChromaArrayType
        uint8_t      LumaBitDepthMinus8:4;                         // bit_depth_luma_minus8
        uint8_t      ChromaBitDepthMinus8:4;                       // bit_depth_chroma_minus8
        uint8_t      BitDepthLuma:4;                               // BitDepthL
        uint8_t      BitDepthChroma:4;                             // BitDepthC
        uint8_t      QPBDOffsetChroma;                             // QpBdOffsetC
        unsigned     QPPrimeBypassFlag:1;                          // qpprime_y_zero_transform_bypass_flag
        unsigned     ScalingMatrixFlag:1;                          // seq_scaling_matrix_present_flag
        unsigned     **ScalingListFlag;                            // seq_scaling_list_present_flag
        unsigned     MaxFrameNumMinus4:4;                          // log2_max_frame_num_minus4
        uint8_t      PicOrderCount:2;                              // pic_order_cnt_type
        uint8_t      MaxPicOrder:4;                                // log2_max_pic_order_cnt_lsb_minus4
        bool         DeltaPicOrder:1;                              // delta_pic_order_always_zero_flag
        int32_t      OffsetNonRefPic;                              // offset_for_non_ref_pic
        int32_t      OffsetTop2Bottom;                             // offset_for_top_to_bottom_field
        uint8_t      RefFramesInPicOrder:8;                        // num_ref_frames_in_pic_order_cnt_cycle
        int32_t    **RefFrameOffset;                               // offset_for_ref_frame
        int32_t      MaxRefFrames;                                 // max_num_ref_frames
        bool         GapsInFrameNumAllowed:1;                      // gaps_in_frame_num_value_allowed_flag
        uint16_t     PicWidthInMacroBlocksMinus1;                  // pic_width_in_mbs_minus1
        uint32_t     PicHeightInMapUnitsMinus1;                    // pic_height_in_map_units_minus1
        bool         OnlyMacroBlocksInFrame:1;                     // frame_mbs_only_flag
        bool         AdaptiveFrameFlag:1;                          // mb_adaptive_frame_field_flag
        bool         Inference8x8:1;                               // direct_8x8_inference_flag
        bool         FrameCroppingFlag:1;                          // frame_cropping_flag
        uint64_t     FrameCropLeft;                                // frame_crop_left_offset
        uint64_t     MaxFrameNum;                                  // MaxFrameNum
        uint64_t     FrameCropRight;                               // frame_crop_right_offset
        uint64_t     FrameCropTop;                                 // frame_crop_top_offset
        uint64_t     FrameCropBottom;                              // frame_crop_bottom_offset
        bool         VUIPresent:1;                                 // vui_parameters_present_flag
        
        /* Standard SPS Extension */
        uint8_t      AuxiliaryFormatID:2;                          // aux_format_idc
        uint8_t      AuxiliaryBitDepth:4;                          // bit_depth_aux_minus8
        bool         AlphaIncrFlag:1;                              // alpha_incr_flag
        uint32_t     AlphaOpaqueValue:5;                           // alpha_opaque_value
        uint8_t      AlphaTransparentValue:5;                      // alpha_transparent_value
        bool         AdditionalExtensionFlag:1;                    // additional_extension_flag
        
        /* Standard SPS Subset */
        bool         SVCVUIParamsPresent:1;                        // svc_vui_parameters_present_flag
        
        /* MVC SPS */
        uint16_t     ViewCount:11;                                 // num_views_minus1
        uint16_t   **ViewID;                                       // view_id
        uint8_t    **AnchorRefsCount;                              // num_anchor_refs_l0 num_anchor_refs_l1
        uint32_t  ***AnchorRef;                                    // anchor_ref_l0 anchor_ref_l1
        uint8_t   ***NonAnchorRefCount;                            // num_non_anchor_refs_l0 num_non_anchor_refs_l1
        uint16_t  ***NonAnchorRef;                                 // non_anchor_ref_l0 non_anchor_ref_l1
        uint8_t      NumLevelValues:7;                             // num_level_values_signalled_minus1
        uint16_t    *NumApplicableOps;                             // num_applicable_ops_minus1
        uint8_t    **AppOpTemporalID;                              // applicable_op_temporal_id
        uint16_t   **AppOpNumTargetViews;                          // applicable_op_num_target_views_minus1
        uint8_t   ***AppOpTargetViewID;                            // applicable_op_target_view_id
        uint8_t    **AppOpNumViews;                                // applicable_op_num_views_minus1
        uint8_t      MFCFormatIDC:6;                               // mfc_format_idc
        bool         DefaultGridPositionFlag:1;                    // default_grid_position_flag
        uint8_t     *MVCViewX;                                     // view0_grid_position_x view1_grid_position_x
        uint8_t     *MVCViewY;                                     // view0_grid_position_y view1_grid_position_y
        bool         RPUFilterFlag:1;                              // rpu_filter_enabled_flag
        bool         RPUFieldFlag:1;                               // rpu_field_processing_flag
        
        /* MVCD SPS Extension */
        bool        *DepthViewPresent;                             // depth_view_present_flag
        uint8_t     *DepthViewID;                                  // DepthViewId
        bool        *TextureViewPresent;                           // texture_view_present_flag
        bool      ***AppOpDepthFlag;                               // applicable_op_depth_flag
        bool      ***AppOpTextureFlag;                             // applicable_op_texture_flag
        uint8_t    **AppOpTextureViews;                            // applicable_op_num_texture_views_minus1
        uint8_t    **AppOpNumDepthViews;                           // applicable_op_num_depth_views
        uint16_t   **MVCDAppOpDepthViews;                          // applicable_op_num_depth_views_minus1
        bool         MVCDVUIParametersPresent:1;                   // mvcd_vui_parameters_present_flag
        bool         MVCDTextureVUIParametersPresent:1;            // texture_vui_parameters_present_flag
        
        /* AVC3D SPS Extension */
        bool         SlicePrediction:1;                            // slice_header_prediction_flag
        uint8_t      AVC3DAcquisitionIDC:2;                        // 3dv_acquisition_idc
        uint8_t     *AVC3DViewID;                                  // view_id_3dv
        bool         ReducedResolutionFlag:1;                      // reduced_resolution_flag
        uint8_t      DepthPicWidthInMacroBlocks;                   // depth_pic_width_in_mbs_minus1
        uint8_t      DepthPicHeightInMapUnits;                     // depth_pic_height_in_map_units_minus1
        uint16_t     DepthHorizontalDisparity:11;                  // depth_hor_mult_minus1
        uint16_t     DepthVerticalDisparity:11;                    // depth_ver_mult_minus1
        uint8_t      DepthHorizontalRSH:6;                         // depth_hor_rsh
        uint8_t      DepthVerticalRSH:6;                           // depth_ver_rsh
        bool         DepthFrameCroppingFlag:1;                     // depth_frame_cropping_flag
        uint32_t     DepthFrameLeftCropOffset;                     // depth_frame_crop_left_offset
        uint32_t     DepthFrameRightCropOffset;                    // depth_frame_crop_right_offset
        uint32_t     DepthFrameTopCropOffset;                      // depth_frame_crop_top_offset
        uint32_t     DepthFrameBottomCropOffset;                   // depth_frame_crop_bottom_offset
        uint16_t     GridPosViewCount:11;                          // grid_pos_num_views
        uint8_t     *GridPosViewID;                                // grid_pos_view_id
        uint8_t     *GridPosX;                                     // grid_pos_x
        uint8_t     *GridPosY;                                     // grid_pos_y
        bool         ALCSpsEnableFlag:1;                           // alc_sps_enable_flag
        bool         EnableRLESkipFlag:1;                          // enable_rle_skip_flag
        bool         MVCVUIParamsPresent:1;                        // mvc_vui_parameters_present_flag
        bool         AdditionalExtension2:1;                       // additional_extension2_flag
        bool         AdditionalExtension2DataFlag:1;               // additional_extension2_data_flag
    } SequenceParameterSet;
    
    typedef struct PictureParameterSet {     // Per slice
        uint8_t      PicParamSetID:8;                              // pic_parameter_set_id
        bool         EntropyCodingMode:1;                          // entropy_coding_mode_flag
        bool         BottomPicFieldOrderInSliceFlag:1;             // bottom_field_pic_order_in_frame_present_flag
        uint8_t      SliceGroups:3;                                // num_slice_groups_minus1
        uint8_t      SliceGroupMapType:3;                          // slice_group_map_type
        uint64_t    *RunLength;                                    // run_length_minus1
        uint8_t     *TopLeft;                                      // top_left
        uint8_t     *BottomRight;                                  // bottom_right
        bool         SliceGroupChangeDirection:1;                  // slice_group_change_direction_flag
        uint8_t      SliceGroupChangeRate;                         // slice_group_change_rate_minus1
                                                                   // SliceChangeRate = 1024 * PicHeightInMapUnits
        uint16_t     PicSizeInMapUnits;                            // pic_size_in_map_units_minus1
        uint8_t     *SliceGroupID;                                 // slice_group_id
        uint8_t     *RefIndex;                                     // num_ref_idx_l0_default_active_minus1 num_ref_idx_l1_default_active_minus1
        bool         WeightedPrediction:1;                         // weighted_pred_flag
        uint8_t      WeightedBiPrediction:2;                       // weighted_bipred_idc
        uint8_t      InitialSliceQP:8;                             // pic_init_qp_minus26
        uint8_t      InitialSliceQS:8;                             // pic_init_qs_minus26
        int8_t       ChromaQPOffset:5;                             // chroma_qp_index_offset
        bool         DeblockingFilterFlag:1;                       // deblocking_filter_control_present_flag
        bool         ConstrainedIntraFlag:1;                       // constrained_intra_pred_flag
        bool         RedundantPictureFlag:1;                       // redundant_pic_cnt_present_flag
        uint8_t      RedundantPictureCount:7;                      // redundant_pic_cnt
        bool         TransformIs8x8:1;                             // transform_8x8_mode_flag
        bool         SeperateScalingMatrix:1;                      // pic_scaling_matrix_present_flag
        bool        *PicScalingList;                               // pic_scaling_list_present_flag
        uint8_t      ChromaQPOffset2:5;                            // second_chroma_qp_index_offset
    } PictureParameterSet;
    
    typedef struct Slice {
        uint8_t      PictureType:3;                                // primary_pic_type
        uint32_t     FirstMacroBlockInSlice;                       // first_mb_in_slice
        uint8_t      Type:4;                                       // slice_type
        uint8_t      ColorPlaneID:3;                               // color_plane_id
        uint64_t     FrameNumber;                                  // frame_num
        bool         SliceIsInterlaced:1;                          // field_pic_flag
        bool         SliceIsBottomField:1;                         // bottom_field_flag
        bool         SliceIsIDR:1;                                 // IdrPicFlag
        uint16_t     IDRPicID:16;                                  // idr_pic_id
        uint32_t     PictureOrderCountLSB:17;                      // pic_order_cnt_lsb
        int32_t     *DeltaPicOrderCount;                           // delta_pic_order_cnt
        uint8_t     *DeltaPictureOrderCountBottom;                 // delta_pic_order_cnt_bottom
        int8_t       SliceQPDelta;                                 // slice_qp_delta
        uint64_t     SliceGroupChangeCycle;                        // slice_group_change_cycle, 0 to Ceil( PicSizeInMapUnits SliceGroupChangeRate ), inclusive.
        uint64_t     SliceGroupChangeRate;                         // slice_group_change_rate
        uint8_t      DisableDeblockingFilterIDC:2;                 // disable_deblocking_filter_idc
        uint8_t      SliceAlphaOffsetC0:4;                         // slice_alpha_c0_offset_div2
        uint8_t      SliceBetaOffset:4;                            // slice_beta_offset_div2
        uint8_t      SliceQSDelta:6;                               // slice_qs_delta
        bool         DecodePMBAsSPSlice:1;                         // sp_for_switch_flag
        uint8_t      CabacInitIDC:2;                               // cabac_init_idc
        bool         NumRefIDXActiveOverrideFlag:1;                // num_ref_idx_active_override_flag
        bool         DirectSpatialMVPredictionFlag:1;              // direct_spatial_mv_pred_flag
        uint8_t      TopFieldOrderCount;                           // TopFieldOrderCnt, PicOrderCntMsb + pic_order_cnt_lsb
        uint8_t      BottomFieldOrderCount;                        // BottomFieldOrderCnt, TopFieldOrderCnt + delta_pic_order_cnt_bottom
        
        /* SliceData */
        uint64_t     PicWidthInSamplesChroma;                      // PicWidthInSamplesC
        uint64_t     PicWidthInSamplesLuma;                        // PicWidthInSamplesL
        uint64_t     FrameHeightInMacroBlocks;                     // FrameHeightInMbs
        uint64_t     PicHeightInMapUnits;                          // PicHeightInMapUnits
        uint64_t     PicWidthInMacroBlocks;                        // PicWidthInMbs
        uint64_t     PicHeightInMacroBlocks;                       // PicHeightInMbs FrameHeightInMbs / ( 1 + field_pic_flag )
        int64_t      PicSizeInMacroBlocks;                         // PicSizeInMbs = PicWidthInMbs * PicHeightInMbs
        uint64_t     RawMacroBlockSizeInBits;                      // RawMbBits
        uint8_t      MacroBlockSkipRun;                            // mb_skip_run, PicSizeInMbs − CurrMbAddr
        uint8_t      PreviousMacroBlockSkipped;                    // prevMbSkipped
        
        /* AVC3D CODING */
        uint8_t      PreSliceHeaderSrc:2;                          // pre_slice_header_src
        uint8_t      PreRefListSrc:2;                              // pre_ref_lists_src
        uint8_t      PrePredictionWeightTableSrc:2;                // pre_pred_weight_table_src
        uint8_t      PreDecReferencePictureMarkingSrc:2;           // pre_dec_ref_pic_marking_src
        bool         DepthWeightedPredictionFlag:1;                // depth_weighted_pred_flag
        bool         DMVPFlag:1;                                   // dmvp_flag
        bool         SeqViewSynthesisFlag:1;                       // seq_view_synthesis_flag
        bool         SliceVSPFlag:1;                               // slice_vsp_flag
        uint8_t      DepthParamSetID;                              // dps_id
        
        /* AVC3DSliceDataExtension */
        bool         SkipTypeMacroBlockFlag:1;                     // mb_skip_type_flag
        bool         SkipMotionVectors:1;                          // mb_vsskip_flag
        bool         SkipMacroBlock:1;                             // mb_skip_flag
        bool         MacroBlockSkipRunType:1;                      // mb_skip_run_type
        bool         SkipMacroBlockALC:1;                          // mb_alc_skip_flag
        bool         MacroBlockFieldDecodingFlag:1;                // mb_field_decoding_flag
        bool         IsEndOfSlice:1;                               // end_of_slice_flag
        
        /* SCALABLE VIDEO CODING */
        bool         SeperateColorPlaneFlag:1;                     // separate_color_plane_flag
        bool         BasePredictionWeightTable:1;                  // base_pred_weight_table_flag
        bool         StoreRefBasePicFlag:1;                        // store_ref_base_pic_flag
        uint8_t      RefLayerDQID:6;                               // ref_layer_dq_id
        uint8_t      DisableInterLayerDeblocking:3;                // disable_inter_layer_deblocking_filter_idc
        uint8_t      InterLayerSliceAplhaC0Offset:4;               // inter_layer_slice_alpha_c0_offset_div2
        uint8_t      InterLayerSliceBetaOffset:4;                  // inter_layer_slice_beta_offset_div2
        bool         ConstrainedIntraResamplingFlag:1;             // constrained_intra_resampling_flag
        bool         RefLayerChromaPhaseXFlag:1;                   // ref_layer_chroma_phase_x_plus1_flag
        uint8_t      RefLayerChromaPhaseY:2;                       // ref_layer_chroma_phase_y_plus1
        int16_t      ScaledRefLayerLeftOffset;                     // scaled_ref_layer_left_offset
        int16_t      ScaledRefLayerTopOffset;                      // scaled_ref_layer_top_offset
        int16_t      ScaledRefLayerRightOffset;                    // scaled_ref_layer_right_offset
        int16_t      ScaledRefLayerBottomOffset;                   // scaled_ref_layer_bottom_offset
        bool         SkipSliceFlag:1;                              // slice_skip_flag
        uint32_t     MacroBlocksInSlice;                           // num_mbs_in_slice_minus1
        bool         AdaptiveBaseModeFlag:1;                       // adaptive_base_mode_flag
        bool         DefaultBaseModeFlag:1;                        // default_base_mode_flag
        bool         AdaptiveMotionPredictionFlag:1;               // adaptive_motion_prediction_flag
        bool         DefaultMotionPredictionFlag:1;                // default_motion_prediction_flag
        bool         AdaptiveResidualPredictionFlag:1;             // adaptive_residual_prediction_flag
        bool         DefaultResidualPredictionFlag:1;              // default_residual_prediction_flag
        bool         ResidualPredictionFlag:1;                     // residual_prediction_flag
        bool         TCoefficentPredictionFlag:1;                  // tcoeff_level_prediction_flag
        uint8_t      ScanIndexStart:4;                             // scan_idx_start
        uint8_t      ScanIndexEnd:4;                               // scan_idx_end
        bool        *RefPicListModFlag;                            // ref_pic_list_modification_flag_l0, ref_pic_list_modification_flag_l1
        uint8_t      ModPicNumsIDC:2;                              // modification_of_pic_nums_idc
        uint8_t      AbsDiffPicNum:4;                              // abs_diff_pic_num_minus1
        uint64_t     LongTermPicNum;                               // long_term_pic_num, Min( MaxDpbMbs / ( PicWidthInMbs * FrameHeightInMbs ), 16 ) + 1
        uint8_t      AbsDiffViewIdx;                               // abs_diff_view_idx_minus1
        uint8_t      LumaWeightDenom:7;                            // luma_log2_weight_denom
        uint8_t      ChromaWeightDenom:7;                          // chroma_log2_weight_denom
        bool        *LumaWeightFlag;                               // luma_weight_l0_flag luma_weight_l1_flag
        int8_t     **LumaWeight;                                   // luma_weight_l0 luma_weight_l1
        int8_t     **LumaOffset;                                   // luma_offset_l0 luma_offset_l1
        bool        *ChromaWeightFlag;                             // chroma_weight_l0_flag chroma_weight_l1_flag
        int8_t    ***ChromaWeight;                                 // chroma_weight_l0 chroma_weight_l1
        int8_t    ***ChromaOffset;                                 // chroma_offset_l0 chroma_offset_l1
        uint64_t     CurrentMacroBlockAddress;                     // CurrMbAddr
        bool         MbaffFrameFlag:1;                             // MbaffFrameFlag
    } Slice;
    
    typedef struct NALHeader {
        uint8_t      NALRefIDC:2;                                  // nal_ref_idc
        uint8_t      NALUnitType:5;                                // nal_unit_type
        size_t       NALUnitSize;                                  // NumBytesInNALunit
        bool         SVCExtensionFlag:1;                           // svc_extension_flag
        bool         AVC3DExtensionFlag:1;                         // avc_3d_extension_flag
        bool         EmptyPictureBufferBeforeDisplay:1;            // no_output_of_prior_pics_flag
        bool         FrameIsLongTermReference:1;                   // long_term_reference_flag
        bool         AdaptiveRefPicMarkingModeFlag:1;              // adaptive_ref_pic_marking_mode_flag
        uint8_t      MemManControlOp;                              // memory_management_control_operation
        uint8_t      PicNumDiff;                                   // difference_of_pic_nums_minus1
        uint8_t      LongTermFrameIndex;                           // long_term_frame_idx
        uint8_t      MaxLongTermFrameIndex;                        // max_long_term_frame_idx_plus1
        
        /* SVC */
        bool         IDRFlag:1;                                    // idr_flag
        bool         AdditionalPrefixNALExtensionFlag:1;           // additional_prefix_nal_unit_extension_flag
        bool         AdditionalPrefixNALExtensionDataFlag:1;       // additional_prefix_nal_unit_extension_data_flag
        uint8_t     *PriorityID;                                   // priority_id
        bool         NoInterLayerPredictionFlag:1;                 // no_inter_layer_pred_flag
        uint8_t     *DependencyID;                                 // dependency_id
        uint8_t    **QualityID;                                    // quality_id
        uint8_t     *TemporalID;                                   // temporal_id
        bool         UseReferenceBasePictureFlag:1;                // use_ref_base_pic_flag
        bool         *IsDisposable;                                // discardable_flag
        bool         OutputFlag:1;                                 // output_flag
        /* 3DAVC */
        uint8_t      ViewIndex;                                    // view_idx; what the hell is a view index?
        bool         DepthFlag:1;                                  // depth_flag
        bool         NonIDRFlag:1;                                 // non_idr_flag
        bool         IsAnchorPicture:1;                            // anchor_pic_flag
        bool         InterViewFlag:1;                              // inter_view_flag
    } NALHeader;
    
    typedef struct MacroBlock {
        /* AVC3FMacroBloclLayer */
        uint8_t      Type;                                         // mb_type
        bool         DirectType:1;                                 // mb_direct_type_flag
        bool         ALCFlag:1;                                    // mb_alc_flag
        uint16_t    *PCMLumaSample;                                // pcm_sample_luma
        uint16_t   **PCMChromaSample;                              // pcm_sample_chroma
        bool         TransfomSizeIs16x16:1;                        // transform_size_16x16_flag
        uint8_t      BlockPattern;                                 // coded_block_pattern
        uint8_t      BlockPatternChroma;                           // CodedBlockPatternChroma
        uint8_t      BlockPatternLuma;                             // CodedBlockPatternLuma
        bool         TransformSizeIs8x8:1;                         // transform_size_8x8_flag
        bool         NoMBSmallerThan8x8Flag:1;                     // noSubMbPartSizeLessThan8x8Flag
        uint8_t      QPDelta;                                      // mb_qp_delta
        
        /* AVC3DMacroBlockPredictionExtension */ // 256 is a guess
        bool        *PreviousIntra4x4PredictionFlag;               // prev_intra4x4_pred_mode_flag
        uint8_t     *RemIntra4x4PredictionMode;                    // rem_intra4x4_pred_mode
        bool        *PreviousIntra8x8PredictionFlag;               // prev_intra8x8_pred_mode_flag
        uint8_t     *RemIntra8x8PredictionMode;                    // rem_intra8x8_pred_mode
        uint8_t      IntraChromaPredictionMode;                    // intra_chroma_pred_mode
        uint8_t     *RefIndexLevel0;                               // ref_idx_l0
        uint8_t     *RefIndexLevel1;                               // ref_idx_l1
        bool       **BVSPFlag;                                     // bvsp_flag_l0 bvsp_flag_l1
        uint8_t  ***MVDLevel0;                                     // mvd_l0
        uint8_t  ***MVDLevel1;                                     // mvd_l1
        uint8_t     *SubMacroBlockType;                            // sub_mb_type
        bool        *MotionPredictionFlagLevel0;                   // motion_prediction_flag_l0
        bool        *MotionPredictionFlagLevel1;                   // motion_prediction_flag_l1
        uint8_t      NumRefIndexActiveLevel0;                      // num_ref_idx_l0_active_minus1
        uint8_t      NumRefIndexActiveLevel1;                      // num_ref_idx_l1_active_minus1
        bool         BaseModeFlag:1;                               // base_mode_flag
    } MacroBlock;
    
    typedef struct HypotheticalReferenceDecoder { // hrd_parameters
        uint8_t      NumCodedPictureBuffers;                       // cpb_cnt_minus1
        uint8_t      BitRateScale;                                 // bit_rate_scale
        uint8_t      CodedPictureBufferScale;                      // cpb_size_scale
        uint32_t    *BitRate;                                      // bit_rate_value_minus1
        uint32_t    *CodedPictureSize;                             // cpb_size_value_minus1
        bool        *IsConstantBitRate;                            // cbr_flag
        uint32_t     InitialCPBDelayLength;                        // initial_cpb_removal_delay_length_minus1
        uint8_t      CBPDelay;                                     // cpb_removal_delay_length_minus1
        uint8_t      DBPDelay;                                     // dpb_output_delay_length_minus1
        uint8_t      TimeOffsetSize;                               // time_offset_length
        bool         VclHrdBpPresentFlag:1;                        // VclHrdBpPresentFlag
    } HypotheticalReferenceDecoder;
    
    typedef struct ScalableVideoCoding {
        bool         InterLayerDeblockingFilterPresent:1;          // inter_layer_deblocking_filter_control_present_flag
        uint8_t      ExtendedSpatialScalabilityIDC;                // extended_spatial_scalability_idc
        bool         ChromaPhaseXFlag:1;                           // chroma_phase_x_plus1_flag
        uint8_t      ChromaPhaseY;                                 // chroma_phase_y_plus1
        bool         SeqRefLayerChromaPhaseX:1;                    // seq_ref_layer_chroma_phase_x_plus1_flag
        uint8_t      SeqRefLayerChromaPhaseY;                      // seq_ref_layer_chroma_phase_y_plus1
        uint8_t      RefLayerLeftOffset;                           // seq_scaled_ref_layer_left_offset
        uint8_t      RefLayerTopOffset;                            // seq_scaled_ref_layer_top_offset
        uint8_t      RefLayerRightOffset;                          // seq_scaled_ref_layer_right_offset
        uint8_t      RefLayerBottomOffset;                         // seq_scaled_ref_layer_bottom_offset
        bool         SequenceCoeffLevelPresent:1;                  // seq_tcoeff_level_prediction_flag
        bool         AdaptiveCoeffsPresent:1;                      // adaptive_tcoeff_level_prediction_flag
        bool         SliceHeaderRestricted:1;                      // slice_header_restriction_flag
        
        /* dec_ref_base_pic_marking */
        bool         AdaptiveMarkingModeFlag:1;                    // adaptive_ref_base_pic_marking_mode_flag
        uint32_t     BaseControlOperation;                         // memory_management_base_control_operation
        uint32_t     NumBasePicDiff;                               // difference_of_base_pic_nums_minus1
        uint32_t     LongTermBasePicNum;                           // long_term_base_pic_num
    } ScalableVideoCoding;
    
    typedef struct VideoUsabilityInformation {
        /* Standard VUI */
        bool         AspectRatioInfoPresent:1;                     // aspect_ratio_info_present_flag
        uint8_t      AspectRatioIDC;                               // aspect_ratio_idc
        uint16_t     SARWidth;                                     // sar_width
        uint16_t     SAWHeight;                                    // sar_height
        bool         OverscanInfoPresent:1;                        // overscan_info_present_flag
        bool         DisplayInOverscan:1;                          // overscan_appropriate_flag
        bool         VideoSignalTypePresent:1;                     // video_signal_type_present_flag
        uint8_t      VideoType;                                    // video_format
        bool         FullRange:1;                                  // video_full_range_flag
        bool         ColorDescriptionPresent:1;                    // color_description_present_flag
        uint8_t      ColorPrimaries;                               // color_primaries
        uint8_t      TransferProperties;                           // transfer_characteristics
        uint8_t      MatrixCoefficients;                           // matrix_coefficients
        bool         ChromaLocationPresent:1;                      // chroma_loc_info_present_flag
        uint8_t      ChromaSampleLocationTop;                      // chroma_sample_loc_type_top_field
        uint8_t      ChromaSampleLocationBottom;                   // chroma_sample_loc_type_bottom_field
        bool        *TimingInfoPresent;                            // timing_info_present_flag
        uint32_t    *UnitsPerTick;                                 // num_units_in_tick
        uint32_t    *TimeScale;                                    // time_scale
        bool        *FixedFrameRateFlag;                           // fixed_frame_rate_flag
        bool        *NALHrdParamsPresent;                          // nal_hrd_parameters_present_flag
        bool        *VCLHrdParamsPresent;                          // vcl_hrd_parameters_present_flag
        bool         LowDelayHRD:1;                                // low_delay_hrd_flag
        bool        *PicStructPresent;                             // pic_struct_present_flag
        bool         BitStreamRestricted:1;                        // bitstream_restriction_flag
        bool        *MotionVectorsOverPicBoundaries;               // motion_vectors_over_pic_boundaries_flag
        uint32_t    *MaxBytesPerPicDenom;                          // max_bytes_per_pic_denom
        uint32_t    *MaxBitsPerMarcoBlockDenom;                    // max_bits_per_mb_denom
        uint32_t    *MaxMotionVectorLength;                        // log2_max_mv_length_horizontal
        uint32_t    *MaxMotionVectorHeight;                        // log2_max_mv_length_vertical
        uint32_t    *MaxReorderFrames;                             // max_num_reorder_frames
        uint32_t    *MaxFrameBuffer;                               // max_dec_frame_buffering
        
        /* MVC VUI */
        uint16_t     MVCNumOpertionPoints;                         // vui_mvc_num_ops_minus1
        uint8_t    **MVCTemporalID;                                // vui_mvc_temporal_id
        uint8_t     *MVCNumTargetViews;                            // vui_mvc_num_target_output_views_minus1
        uint8_t    **MVCViewID;                                    // vui_mvc_view_id
        bool         MVCTimingInfoPresent:1;                       // vui_mvc_timing_info_present_flag
        uint32_t    *MVCUnitsInTick;                               // vui_mvc_num_units_in_tick
        uint32_t    *MVCTimeScale;                                 // vui_mvc_time_scale
        bool        *MVCFixedFrameRate;                            // vui_mvc_fixed_frame_rate_flag
        bool        *MVCNALHRDParamsPresent;                       // vui_mvc_nal_hrd_parameters_present_flag
        bool        *MVCVCLHRDParamsPresent;                       // vui_mvc_vcl_hrd_parameters_present_flag
        bool        *MVCLowDelayFlag;                              // vui_mvc_low_delay_hrd_flag
                                                                   // vui_mvc_pic_struct_present_flag
        
        /* MVCD VUI */
        uint8_t      VUIMVCDNumOpPoints;                           // vui_mvcd_num_ops_minus1
        uint8_t     *VUIMVCDTemporalID;                            // vui_mvcd_temporal_id
        uint16_t    *VUIMVCDNumTargetOutputViews;                  // vui_mvcd_num_target_output_views_minus1
        uint8_t    **VUIMVCDViewID;                                // vui_mvcd_view_id
        bool       **VUIMVCDDepthFlag;                             // vui_mvcd_depth_flag
        bool       **VUIMVCDTextureFlag;                           // vui_mvcd_texture_flag
        bool        *VUIMVCDTimingInfoPresent;                     // vui_mvcd_timing_info_present_flag
        uint32_t    *VUIMVCDNumUnitsInTick;                        // vui_mvcd_num_units_in_tick
        uint32_t    *VUIMVCDTimeScale;                             // vui_mvcd_time_scale
        bool        *VUIMVCDFixedFrameRateFlag;                    // vui_mvcd_fixed_frame_rate_flag
        bool        *VUIMVCDNalHRDParametersPresent;               // vui_mvcd_nal_hrd_parameters_present_flag
        bool        *VUIMVCDVclHRDParametersPresent;               // vui_mvcd_vcl_hrd_parameters_present_flag
        bool        *VUIMVCDHRDLowDelayPresent;                    // vui_mvcd_low_delay_hrd_flag
        bool        *VUIMVCDPicStructPresent;                      // vui_mvcd_pic_struct_present_flag
        
        /* SVC VUI */
        uint8_t      VUIExtNumEntries;                             // vui_ext_num_entries_minus1
        uint8_t     *VUIExtDependencyID;                           // vui_ext_dependency_id
        uint8_t     *VUIExtQualityID;                              // vui_ext_quality_id
        uint8_t     *VUIExtTemporalID;                             // vui_ext_temporal_id
        bool        *VUIExtTimingInfoPresentFlag;                  // vui_ext_timing_info_present_flag
        uint32_t    *VUIExtNumUnitsInTick;                         // vui_ext_num_units_in_tick
        uint32_t    *VUIExtTimeScale;                              // vui_ext_time_scale
        bool        *VUIExtFixedFrameRateFlag;                     // vui_ext_fixed_frame_rate_flag
        bool        *VUIExtNALHRDPresentFlag;                      // vui_ext_nal_hrd_parameters_present_flag
        bool        *VUIExtVCLHRDPresentFlag;                      // vui_ext_vcl_hrd_parameters_present_flag
        bool        *VUIExtLowDelayHRDFlag;                        // vui_ext_low_delay_hrd_flag
        bool        *VUIExtPicStructPresentFlag;                   // vui_ext_pic_struct_present_flag
    } VideoUsabilityInformation;
    
    typedef struct DepthParameterSet {
        uint8_t      DepthParameterSetID;                          // depth_parameter_set_id
        uint8_t      PredictionDirection;                          // pred_direction
        uint8_t     *ReferenceDPSID;                               // ref_dps_id0, ref_dps_id1
        uint8_t      PredictedWeight0;                             // predWeight0
        uint8_t      NumDepthViews;                                // num_depth_views_minus1
        bool         VSPParamFlag:1;                               // vsp_param_flag
        bool         AdditionalExtensionFlag:1;                    // depth_param_additional_extension_data_flag
        uint8_t      DepthMappingValues;                           // nonlinear_depth_representation_num
        uint8_t     *DepthRepresentationModel;                     // nonlinear_depth_representation_model
    } DepthParameterSet;
    
    typedef struct SupplementalEnchancementInfo {
        uint32_t     SEISize;
        uint32_t     SEIType;                                      // sei_type
        /* Buffering Period */
        bool         NalHrdBpPresentFlag:1;                        // NalHrdBpPresentFlag
        uint8_t     *InitialCPBRemovalDelay;                       // initial_cpb_removal_delay
        uint8_t     *InitialCPBRemovalDelayOffset;                 // initial_cpb_removal_delay_offset
        
        /* pic_timing */
        uint8_t      CPBRemovalDelay;                              // cpb_removal_delay
        uint8_t      DPBOutputDelay;                               // dpb_output_delay
        uint8_t      PicStruct;                                    // pic_struct
        bool        *ClockTimestampFlag;                           // clock_timestamp_flag
        uint8_t      CTType;                                       // ct_type
        bool         NuitFieldBasedFlag;                           // nuit_field_based_flag
        uint8_t      CountingType;                                 // counting_type
        bool         FullTimestampFlag:1;                          // full_timestamp_flag
        bool         CountDroppedFlag:1;                           // cnt_dropped_flag
        uint8_t      NFrames;                                      // n_frames
        bool         HoursFlag:1;                                  // hours_flag
        uint8_t      Hours;                                        // hours_value
        bool         MinutesFlag:1;                                // minutes_flag
        uint8_t      Minutes;                                      // minutes_value
        bool         SecondsFlag:1;                                // seconds_flag
        uint8_t      Seconds;                                      // seconds_value
        uint8_t      TimeOffset;                                   // time_offset
        
        /* Pan Scan */
        uint8_t      PanScanID;                                    // pan_scan_rect_id
        bool         DisablePanScanFlag:1;                         // pan_scan_rect_cancel_flag
        uint8_t      PanScanCount;                                 // pan_scan_cnt_minus1
        int8_t      *PanScanOffsetLeft;                            // pan_scan_rect_left_offset
        int8_t      *PanScanOffsetRight;                           // pan_scan_rect_right_offset
        int8_t      *PanScanOffsetTop;                             // pan_scan_rect_top_offset
        int8_t      *PanScanOffsetBottom;                          // pan_scan_rect_bottom_offset
        uint16_t     PanScanRepitionPeriod;                        // pan_scan_rect_repetition_period
        
        /* ITU T-35 */
        uint16_t     CountryCode;                                  // itu_t_t35_country_code
        
        /* Recovery Point */
        uint8_t      RecoveryFrameCount;                           // recovery_frame_cnt
        bool         ExactMatchFlag:1;                             // exact_match_flag
        bool         BrokenLinkFlag:1;                             // broken_link_flag
        uint8_t      ChangingSliceGroupIDC:2;                      // changing_slice_group_idc
        
        /* Reference Picture Marking Repitition */
        bool         OriginalIDRFlag:1;                            // original_idr_flag
        uint32_t     OriginalFrameNum;                             // original_frame_num
        bool         OriginalFieldPicFlag:1;                       // original_field_pic_flag
        bool         OriginalBottomFieldFlag:1;                    // original_bottom_field_flag
        
        /* Spare Picture */
        uint32_t     TargetFrameNum;                               // target_frame_num
        bool         SpareFieldFlag:1;                             // spare_field_flag
        bool         TargetBottomFieldFlag:1;                      // target_bottom_field_flag
        uint32_t     NumSparePics;                                 // num_spare_pics_minus1
        uint32_t    *DeltaSpareFrameNum;                           // delta_spare_frame_num
        bool        *SpareBottomFieldFlag;                         // spare_bottom_field_flag
        uint8_t     *SpareAreaIDC;                                 // spare_area_idc
        bool       **SpareUnitFlag;                                // spare_unit_flag
        uint8_t    **ZeroRunLength;                                // zero_run_length
        
        /* Scene Info */
        uint32_t     SceneInfoPresentFlag;                         // scene_info_present_flag
        uint8_t      SceneID;                                      // scene_id
        uint8_t      SceneTransitionType;                          // scene_transition_type
        uint32_t     SecondSceneID;                                // second_scene_id
        
        /* Sub Sequence Info */     // sub_seq_info
        uint8_t      SubSequenceLayerNum;                          // sub_seq_layer_num
        uint8_t      SubSequenceID;                                // sub_seq_id
        bool         FirstRefPicFlag:1;                            // first_ref_pic_flag
        bool         LeadingNonRefPicFlag:1;                       // leading_non_ref_pic_flag
        bool         LastPicFlag:1;                                // last_pic_flag
        bool         SubSeqFrameNumFlag:1;                         // sub_seq_frame_num_flag
        uint8_t      SubSeqFrameNum;                               // sub_seq_frame_num
        
        /* Sub Sequence Layer Characteristics */
        uint8_t      NumSubSeqLayers;                              // num_sub_seq_layers_minus1
        bool         AccurateStatisticsFlag:1;                     // accurate_statistics_flag
        uint16_t     AverageBitRate;                               // average_bit_rate
        uint16_t     AverageFrameRate;                             // average_frame_rate
        
        /* Sub Sequence Characteristics */
        bool         DurationFlag:1;                               // duration_flag
        uint32_t     SubSeqDuration;                               // sub_seq_duration
        bool         AverageRateFlag:1;                            // average_rate_flag
        uint8_t      NumReferencedSubSeqs;                         // num_referenced_subseqs
        uint8_t      RefSubSeqLayerNum;                            // ref_sub_seq_layer_num
        uint8_t      RefSubSeqID;                                  // ref_sub_seq_id
        uint8_t      RefSubSeqDirection;                           // ref_sub_seq_direction
        
        /* Full Frame Freeze */
        uint64_t     FullFrameFreezeRepitionPeriod;                // full_frame_freeze_repetition_period
        
        /* Full Frame Snapshot */
        uint8_t      SnapshotID;                                   // snapshot_id
        
        /* Progressive Refinement Segment Start */
        uint8_t      ProgressiveRefinementID;                      // progressive_refinement_id
        uint8_t      NumRefinementSteps;                           // num_refinement_steps_minus1
        
        /* Motion Constrained Slice Group */
        uint8_t      NumSliceGroupsInSet;                          // num_slice_groups_in_set_minus1
        bool        *ExactSampleValueMatchFlag;                    // exact_sample_value_match_flag
        bool         PanScanRectFlag:1;                            // pan_scan_rect_flag
        
        /* Film Grain Characteristics */
        bool         FilmGrainCharactisticsCancelFlag:1;           // film_grain_characteristics_cancel_flag
        uint8_t      FilmGrainModelID;                             // film_grain_model_id
        bool         SeperateColorDescriptionFlag:1;               // separate_color_description_present_flag
        uint8_t      FilmGrainBitDepthLuma:4;                      // film_grain_bit_depth_luma_minus8
        uint8_t      FilmGrainBitDepthChroma:4;                    // film_grain_bit_depth_chroma_minus8
        bool         FilmGrainFullRangeFlag:1;                     // film_grain_full_range_flag
        uint8_t      FilmGrainColorPrimaries;                      // film_grain_color_primaries
        uint8_t      FilmGrainTransferCharacteristics;             // film_grain_transfer_characteristics
        uint8_t      FilmGrainMatrixCoefficents;                   // film_grain_matrix_coefficients
        uint8_t      BlendingModeID;                               // blending_mode_id
        uint8_t      Scalefactor;                                  // log2_scale_factor
        bool        *CompModelPresent;                             // comp_model_present_flag
        uint16_t    *NumIntensityIntervals;                        // num_intensity_intervals_minus1
        uint8_t     *NumModelValues;                               // num_model_values_minus1
        uint8_t    **IntensityIntervalLowerBound;                  // intensity_interval_lower_bound
        uint8_t    **IntensityIntervalUpperBound;                  // intensity_interval_upper_bound
        uint8_t   ***CompModelValue;                               // comp_model_value
        uint8_t      FilmGrainCharacteristicsRepetitionPeriod;     // film_grain_characteristics_repetition_period
        
        /* Deblocking Filter Display Preference */
        bool         DeblockingDisplayPreferenceCancelFlag:1;      // deblocking_display_preference_cancel_flag
        bool         DisplayBeforeDeblockingFlag:1;                // display_prior_to_deblocking_preferred_flag
        bool         DecodeFrameBufferingConstraintFlag:1;         // dec_frame_buffering_constraint_flag
        uint8_t      DeblockingDisplayPreferenceRepetitionPeriod;  // deblocking_display_preference_repetition_period
        
        /* Stereo Video Info */
        bool         FieldViewsFlag:1;                             // field_views_flag
        bool         TopFieldIsLeftViewFlag:1;                     // top_field_is_left_view_flag
        bool         CurrentFrameIsLeftViewFlag:1;                 // current_frame_is_left_view_flag
        bool         NextFrameIsDependentViewFlag:1;               // next_frame_is_second_view_flag
        bool         LeftViewSelfContainedFlag:1;                  // left_view_self_contained_flag
        bool         RightViewSelfContainedFlag:1;                 // right_view_self_contained_flag
        
        /* Post Filter Hint */
        uint8_t      FilterHintSizeY;                              // filter_hint_size_y
        uint8_t      FilterHintSizeX;                              // filter_hint_size_x
        uint8_t      FilterHintType;                               // filter_hint_type
        uint8_t   ***FilterHint;                                   // filter_hint
        
        /* Tone Mapping Info */
        uint8_t      ToneMapID;                                    // tone_map_id
        bool         ToneMapCancelFlag:1;                          // tone_map_cancel_flag
        uint8_t      ToneMapRepetitionPeriod;                      // tone_map_repetition_period
        uint8_t      CodedDataBitDepth;                            // coded_data_bit_depth
        uint8_t      TargetBitDepth;                               // target_bit_depth
        uint8_t      ToneMapModelID;                               // tone_map_model_id
        uint32_t     ToneMinValue;                                 // min_value
        uint32_t     ToneMaxValue;                                 // max_value
        uint32_t     SigmoidMidpoint;                              // sigmoid_midpoint
        uint32_t     SigmoidWidth;                                 // sigmoid_width
        uint8_t     *StartOfCodedInterval;                         // start_of_coded_interval
        uint8_t      NumPivots;                                    // num_pivots
        uint8_t     *CodedPivotValue;                              // coded_pivot_value
        uint16_t    *TargetPivotValue;                             // target_pivot_value
        uint8_t      CameraISOSpeedIDC;                            // camera_iso_speed_idc
        uint8_t      CameraISOSpeed;                               // camera_iso_speed_value
        uint8_t      ExposureIndexIDC;                             // exposure_index_idc
        uint8_t      ExposureIndexValue;                           // exposure_index_value
        bool         ExposureCompensationSignFlag:1;               // exposure_compensation_value_sign_flag
        uint16_t     ExposureCompensationNumerator;                // exposure_compensation_value_numerator
        uint16_t     ExposureCompensationDenominatorIDC;           // exposure_compensation_value_denom_idc
        uint32_t     RefScreenLuminanceWhite;                      // ref_screen_luminance_white
        uint32_t     ExtendedRangeWhiteLevel;                      // extended_range_white_level
        uint16_t     NominalBlackLumaCode;                         // nominal_black_level_luma_code_value
        uint16_t     NominalWhiteLumaCode;                         // nominal_white_level_luma_code_value
        uint16_t     ExtendedWhiteLumaCode;                        // extended_white_level_luma_code_value
        
        /* Sub Picture Scalable Layer */
        
        /* Non Required Layer Rep */ // 256 is a guess
        uint32_t     NumInfoEntries;                               // num_info_entries_minus1
        uint8_t     *EntryDependencyID;                            // entry_dependency_id
        uint8_t     *NumNonRequiredLayerReps;                      // num_non_required_layer_reps_minus1
        uint8_t    **NonRequiredLayerRepDependencyID;              // non_required_layer_rep_dependency_id
        uint8_t    **NonRequiredLayerRepQualityID;                 // non_required_layer_rep_quality_id
        
        /* Priority Layer Information */
        uint8_t     *PriorityDependencyID;                         // pr_dependency_id
        uint8_t      NumPriorityIDs:4;                             // num_priority_ids
        uint8_t     *AltPriorityIDs;                               // alt_priority_id
        
        /* Layers Not Present */
        uint8_t      NumLayersNotPresent;                          // num_layers
        uint8_t     *LayerID;                                      // layer_id
        
        /* Layer Dependency Change */
        uint8_t      NumLayers;                                    // num_layers_minus1
        bool        *LayerDependencyInfoPresent;                   // layer_dependency_info_present_flag
        uint8_t     *NumDependentLayers;                           // num_directly_dependent_layers
        uint8_t    **DirectlyDependentLayerIDDelta;                // directly_dependent_layer_id_delta_minus1
        uint8_t     *LayerDepInfoSourceLayerIDDelta;               // layer_dependency_info_src_layer_id_delta_minus1
        
        /* Scalable Nesting */
        bool         AllLayerRepresentationsInAUFlag:1;            // all_layer_representations_in_au_flag
        uint8_t      NumLayerRepresentations;                      // num_layer_representations_minus1
        uint8_t     *SEIDependencyID;                              // sei_dependency_id
        uint8_t     *SEIQualityID;                                 // sei_quality_id
        uint8_t     *SEITemporalID;                                // sei_temporal_id
        
        /* Base Layer Temporal HRD */
        uint8_t      NumTemporalLayersInBaseLayer;                 // num_of_temporal_layers_in_base_layer_minus1
        bool        *SEITimingInfoPresent;                         // sei_timing_info_present_flag
        uint32_t    *SEIUnitsInTick;                               // sei_num_units_in_tick
        uint32_t    *SEITimeScale;                                 // sei_time_scale
        bool        *SEIFixedFrameRateFlag;                        // sei_fixed_frame_rate_flag
        bool        *SEINALHRDParamsPresentFlag;                   // sei_nal_hrd_parameters_present_flag
        bool        *SEIVCLHRDParamsPresentFlag;                   // sei_vcl_hrd_parameters_present_flag
        bool        *SEILowDelayHRDFlag;                           // sei_low_delay_hrd_flag
        bool        *SEIPicStructPresentFlag;                      // sei_pic_struct_present_flag
        
        /* Quality Layer Integrity Check */
        uint16_t    *SEIQualityLayerCRC;                           // quality_layer_crc
        
        /* Redundant Pic Property */
        uint8_t      NumDependencyIDs;                             // num_dIds_minus1
        uint8_t     *NumQualityIDs;                                // num_qIds_minus1
        uint8_t    **NumRedundantPics;                             // num_redundant_pics_minus1
        uint8_t   ***RedundantPicCount;                            // redundant_pic_cnt_minus1
        bool      ***RedundantPicsMatch;                           // pic_match_flag
        bool      ***MBTypeMatchFlag;                              // mb_type_match_flag
        bool      ***MotionMatchFlag;                              // motion_match_flag
        bool      ***ResidualMatchFlag;                            // residual_match_flag
        bool      ***IntraSamplesMatchFlag;                        // intra_samples_match_flag
        
        /* Temporal Level 0 Dependency Representation Index */
        uint8_t      TemporalDependencyRepresentationIndexLevel0;  // tl0_dep_rep_idx
        uint16_t     EffectiveIDRPicID;                            // effective_idr_pic_id
        
        /* Temporal Level Switching Point */
        uint8_t      DeltaFrameNum;                                // delta_frame_num
        
        /* Parallel Decoding Info */
        uint8_t   ***PDIInitDelayAnchor;                           // pdi_init_delay_anchor_minus2_l0, pdi_init_delay_anchor_minus2_l1
        uint8_t   ***PDIInitDelayNonAnchor;                        // pdi_init_delay_non_anchor_minus2_l0, pdi_init_delay_non_anchor_minus2_l1
        
        /* MVC Scalable Nesting */
        bool         OperationPointFlag:1;                         // operation_point_flag
        bool         AllViewComponentsInAUFlag:1;                  // all_view_components_in_au_flag
        uint8_t      NumViewComponents;                            // num_view_components_minus1
        uint16_t    *SEIViewID;                                    // sei_view_id
        bool        *SEIViewApplicabilityFlag;                     // sei_view_applicability_flag
        uint8_t      NumViewComponentsOp;                          // num_view_components_op_minus1
        uint16_t    *SEIOpViewID;                                  // sei_op_view_id
        uint8_t      SEIOpTemporalID:3;                            // sei_op_temporal_id
        
        /* View Scalability Info */
        uint8_t      NumOperationPoints;                           // num_operation_points_minus1
        uint8_t     *OperationPointID;                             // operation_point_id
        uint8_t     *NumTargetOutputViews;                         // num_target_output_views_minus1
        bool        *BitRateInfoPresent;                           // bitrate_info_present_flag
        bool        *FrameRateInfoPresent;                         // frm_rate_info_present_flag
        bool        *ViewDependencyInfoPresent;                    // view_dependency_info_present_flag
        bool        *ParameterSetsInfoPresent;                     // parameter_sets_info_present_flag
        bool        *BitstreamRestrictionInfoPresent;              // bitstream_restriction_info_present_flag
        uint32_t    *OpProfileLevelIDC;                            // op_profile_level_idc
        uint16_t    *AvgBitRate;                                   // avg_bitrate
        uint16_t    *MaxBitRate;                                   // max_bitrate
        uint16_t    *MaxBitRateWindow;                             // max_bitrate_calc_window
        uint8_t     *ConstantFrameRateIDC;                         // constant_frm_rate_idc
        uint16_t    *AvgFrameRate;                                 // avg_frm_rate
        uint8_t     *NumDirectlyDependentViews;                    // num_directly_dependent_views
        uint8_t    **DirectlyDependentViewID;                      // directly_dependent_view_id
        uint8_t     *ViewDependencyInfoSrcOpID;                    // view_dependency_info_src_op_id
        
        /* Frame Packing Arrangement */
        uint8_t      FramePackingArrangementID;                    // frame_packing_arrangement_id
        bool         FramePackingArrangementCancelFlag:1;          // frame_packing_arrangement_cancel_flag
        uint8_t      FramePackingArrangementType;                  // frame_packing_arrangement_type
        uint8_t      QuincunxSamplingFlag;                         // quincunx_sampling_flag
        uint8_t      ContentIntrepretationType;                    // content_interpretation_type
        bool         SpatialFlippingFlag:1;                        // spatial_flipping_flag
        bool         FlippedFlagFrame0:1;                          // frame0_flipped_flag
        bool         TheCurrentFrameIsFrame0:1;                    // current_frame_is_frame0_flag
        bool         Frame0IsSelfContainedFlag:1;                  // frame0_self_contained_flag
        bool         Frame1IsSelfContainedFlag:1;                  // frame1_self_contained_flag
        uint8_t      Frame0GridPositionX;                          // frame0_grid_position_x // FIXME: Combine these
        uint8_t      Frame0GridPositionY;                          // frame0_grid_position_y
        uint8_t      Frame1GridPositionX;                          // frame1_grid_position_x
        uint8_t      Frame1GridPositionY;                          // frame1_grid_position_y
        uint8_t      FramePackingArrangementReservedByte;          // frame_packing_arrangement_reserved_byte
        uint8_t      FramePackingArrangementRepetitionPeriod;      // frame_packing_arrangement_repetition_period
        bool         FramePackingArrangementExtensionFlag:1;       // frame_packing_arrangement_extension_flag
        
        /* Display Orientation */
        bool         DisplayOrientationCancelFlag:1;               // display_orientation_cancel_flag
        bool         HorizontalFlip:1;                             // hor_flip
        bool         VerticalFlip:1;                               // ver_flip
        bool         CounterClockwiseRotation:1;                   // anticlockwise_rotation
        bool         DisplayOrientationRepetitionPeriod:1;         // display_orientation_repetition_period
        bool         DisplayOrientationExtensionFlag:1;            // display_orientation_extension_flag
        
        /* Metering Display Color Volume */
        uint16_t    *DisplayPrimariesX;                            // display_primaries_x
        uint16_t    *DisplayPrimariesY;                            // display_primaries_y
        uint16_t     WhitePointX;                                  // white_point_x
        uint16_t     WhitePointY;                                  // white_point_y
        uint32_t     MaxDisplayMasteringLuminance;                 // max_display_mastering_luminance
        uint32_t     MinDisplayMasteringLuminance;                 // min_display_mastering_luminance
        
        /* Depth Representation Information */
        bool         AllViewsEqual:1;                              // all_views_equal_flag
        bool         ZNearFlag:1;                                  // z_near_flag
        bool         ZFarFlag:1;                                   // z_far_flag
        bool         ZAxisEqualFlag:1;                             // z_axis_equal_flag
        uint8_t      CommonZAxisReferenceView;                     // common_z_axis_reference_view
        bool         DMinFlag:1;                                   // d_min_flag
        bool         DMaxFlag:1;                                   // d_max_flag
        uint8_t      DepthRepresentationType;                      // depth_representation_type
        uint8_t     *DepthInfoViewID;                              // depth_info_view_id
        uint8_t     *ZAxisReferenceView;                           // z_axis_reference_view
        uint8_t     *DisparityReferenceView;                       // disparity_reference_view
        uint8_t      DepthNonlinearRepresentation;                 // depth_nonlinear_representation_num_minus1
        uint8_t     *DepthNonlinearRepresentationModel;            // depth_nonlinear_representation_model
        
        /* 3D Reference Display Info */
        uint8_t      TruncationErrorExponent;                      // prec_ref_baseline
        uint8_t      TruncatedWidthExponent;                       // prec_ref_display_width
        bool         ReferenceViewingDistanceFlag:1;               // ref_viewing_distance_flag
        uint8_t      TruncatedReferenveViewingDistanceExponent;    // prec_ref_viewing_dist
        uint8_t      NumReferenceDisplays;                         // num_ref_displays_minus1
        uint8_t     *ReferenceBaselineExponent;                    // exponent_ref_baseline
        uint8_t     *ReferenceBaselineMantissa;                    // mantissa_ref_baseline
        uint8_t     *ReferenceDisplayWidthExponent;                // exponent_ref_display_width
        uint8_t     *ReferenceDisplayWidthMantissa;                // mantissa_ref_display_width
        uint8_t     *ReferenceViewingDistanceExponent;             // exponent_ref_viewing_distance
        uint8_t     *ReferenceViewingDistanceMantissa;             // mantissa_ref_viewing_distance
        bool        *ShiftPresentFlag;                             // additional_shift_present_flag
        int16_t     *SampleShift;                                  // num_sample_shift_plus512
        bool         ReferenceDisplays3DFlag:1;                    // three_dimensional_reference_displays_extension_flag
        
        /* Depth Timing Info */
        bool         PerViewDepthTimingFlag:1;                     // per_view_depth_timing_flag
        uint8_t     *OffsetLength;                                 // offset_len_minus1
        uint8_t     *DepthDisplayDelayOffsetFP;                    // depth_disp_delay_offset_fp
        uint8_t     *DepthDisplayDelayOffsetDP;                    // depth_disp_delay_offset_dp
        
        /* Depth Sampling Info */
        uint16_t     DepthSampleWidthMul;                          // dttsr_x_mul
        uint8_t      DepthSampleWidthDP;                           // dttsr_x_dp
        uint16_t     DepthSampleHeightMul;                         // dttsr_y_mul
        uint8_t      DepthSampleHeightDP;                          // dttsr_y_dp
        bool         PerViewDepthSamplingGridFlag:1;               // per_view_depth_grid_pos_flag
        uint8_t      NumDepthGridViews;                            // num_video_plus_depth_views_minus1
        uint8_t     *DepthGridViewID;                              // depth_grid_view_id
        
        /* Depth Grid Position */
        uint32_t     DepthGridPositionXFP:20;                      // depth_grid_pos_x_fp
        uint8_t      DepthGridPositionXDP:4;                       // depth_grid_pos_x_dp
        bool         DepthGridPositionXSignFlag:1;                 // depth_grid_pos_x_sign_flag
        uint32_t     DepthGridPositionYFP:20;                      // depth_grid_pos_y_fp
        uint8_t      DepthGridPositionYDP:4;                       // depth_grid_pos_y_dp
        bool         DepthGridPositionYSignFlag:1;                 // depth_grid_pos_y_sign_flag
        
        /* Unregistered User Data */
        char        *UnregisteredUserDataUUID;
        
        /* Constrained Depth Parameter Set Identifier */
        uint8_t      MaxDPSID;                                     // max_dps_id
        uint8_t      MaxDPSIDDiff;                                 // max_dps_id_diff
        
        /* MVC View Position */
        uint8_t     *ViewPosition;                                 // view_position
        bool         MVCViewPositionExtensionFlag:1;               // multiview_view_position_extension_flag
        
        /* Base View Temporal HRD */
        uint8_t      NumTemporalLayersInBaseView;                  // num_of_temporal_layers_in_base_view_minus1
        uint8_t     *SEIMVCTemporalID;                             // sei_mvc_temporal_id
        
        /* MVC Scene Info */
        uint8_t      MaxDisparity;                                 // max_disparity
        
        /* MVC Acquisition Info */
        bool         IntrinsicParamFlag:1;                         // intrinsic_param_flag
        bool         ExtrinsicParamFlag:1;                         // extrinsic_param_flag
        uint8_t      IntrinsicParamsEqual;                         // intrinsic_params_equal
        uint8_t      PrecFocalLength;                              // prec_focal_length
        uint8_t      PrecPrincipalPoint;                           // prec_principal_point
        uint8_t      PrecSkewFactor;                               // prec_skew_factor
        uint8_t      NumParamSets;                                 // num_of_param_sets
        uint8_t    **SignFocalLength;                              // sign_focal_length_x, sign_focal_length_y (x=0,y=1)
        uint16_t   **ExponentFocalLength;                          // exponent_focal_length_x, exponent_focal_length_y
        uint16_t   **MantissaFocalLength;                          // mantissa_focal_length_x, mantissa_focal_length_y
        uint8_t    **SignPrincipalPoint;                           // sign_principal_point_x, sign_principal_point_y
        uint8_t    **ExponentPrincipalPoint;                       // exponent_principal_point_x, exponent_principal_point_y
        uint8_t    **MantissaPrincipalPoint;                       // mantissa_principal_point_x, mantissa_principal_point_y
        uint8_t     *SignSkewFactor;                               // sign_skew_factor
        uint8_t     *ExponentSkewFactor;                           // exponent_skew_factor
        uint8_t     *MantissaSkewFactor;                           // mantissa_skew_factor
        uint8_t      PrecRotationParam;                            // prec_rotation_param
        uint8_t      PrecTranslationParam;                         // prec_translation_param
        uint8_t   ***SignR;                                        // sign_r
        uint8_t   ***ExponentR;                                    // exponent_r
        uint8_t   ***MantissaR;                                    // mantissa_r
        uint8_t    **SignT;                                        // sign_t
        uint8_t    **ExponentT;                                    // exponent_t
        uint8_t    **MantissaT;                                    // mantissa_t
        
        /* Unnessacary View Component */
        uint8_t     *ViewOrderIndex;                               // view_order_index
        uint8_t     *NumNonRequiredViewComponents;                 // num_non_required_view_components_minus1
        uint8_t    **IndexDelta;                                   // index_delta_minus1
        
        /* View Dependency Change */
        bool         AnchorUpdateFlag:1;                           // anchor_update_flag
        bool         NonAnchorUpdateFlag:1;                        // non_anchor_update_flag
        bool      ***AnchorRefFlag;                                // anchor_ref_l0_flag
        bool      ***NonAnchorRefFlag;                             // non_anchor_ref_l0_flag
        
        /* Operation Point(s) Not Present */
        uint8_t     *OperationPointNotPresentID;                   // operation_point_not_present_id
        
        /* MVCD Scalable Nesting */
        bool         SEIOpTextureOnlyFlag:1;                       // sei_op_texture_only_flag
        bool        *SEIOpDepthFlag;                               // sei_op_depth_flag
        bool        *SEIOpTextureFlag;                             // sei_op_texture_flag
        
        /* Scalability Info */
        bool         TemporalIDNestingFlag:1;                      // temporal_id_nesting_flag
        bool         PriorityLayerInfoPresent:1;                   // priority_layer_info_present_flag
        bool         PriorityIDSettingFlag:1;                      // priority_id_setting_flag
        bool        *SubPicLayerFlag;                              // sub_pic_layer_flag
        bool        *SubRegionLayerFlag;                           // sub_region_layer_flag
        bool        *IROIDivisionFlag;                             // iroi_division_info_present_flag
        bool        *FrameSizeInfoPresentFlag;                     // frm_size_info_present_flag
        bool        *ExactInterLayerPredFlag;                      // exact_inter_layer_pred_flag
        bool        *LayerConversionFlag;                          // layer_conversion_flag
        bool        *LayerOutputFlag;                              // layer_output_flag
        bool        *ProfileLevelInfoPresentFlag;                  // profile_level_info_present_flag
        uint32_t    *LayerProfileLevelID;                          // layer_profile_level_id
        uint16_t    *MaxBitRateRepresentation;                     // max_bitrate_layer_representation
        uint32_t    *FrameWidthInMacroBlocks;                      // frm_width_in_mbs_minus1
        uint32_t    *FrameHeightInMacroBlocks;                     // frm_height_in_mbs_minus1
        uint8_t     *BaseRegionLayerID;                            // base_region_layer_id
        bool        *DynamicRectFlag;                              // dynamic_rect_flag
        uint16_t    *HorizontalOffset;                             // horizontal_offset
        uint16_t    *VerticalOffset;                               // vertical_offset
        uint16_t    *RegionWidth;                                  // region_width
        uint16_t    *RegionHeight;                                 // region_height
        uint8_t     *ROIID;                                        // roi_id
        bool        *IROIGridFlag;                                 // iroi_grid_flag
        uint32_t    *GridWidthInMacroBlocks;                       // grid_width_in_mbs_minus1
        uint32_t    *GridHeightInMacroBlocks;                      // grid_height_in_mbs_minus1
        uint8_t     *NumROIs;                                      // num_rois_minus1
        uint8_t    **FirstMacroBlockInROI;                         // first_mb_in_roi
        uint8_t    **ROIWidthInMacroBlock;                         // roi_width_in_mbs_minus1
        uint8_t    **ROIHeightInMacroBlock;                        // roi_height_in_mbs_minus1
        uint8_t     *NumSequenceParameterSets;                     // num_seq_parameter_sets
        uint8_t    **SPSIDDelta;                                   // seq_parameter_set_id_delta
        uint8_t     *NumSubsetSPS;                                 // num_subset_seq_parameter_sets
        uint8_t    **SubsetSPSIDDelta;                             // subset_seq_parameter_set_id_delta
        uint8_t     *NumPicParameterSets;                          // num_pic_parameter_sets_minus1
        uint8_t    **PicParameterSetIDDelta;                       // pic_parameter_set_id_delta
        uint8_t     *PPSInfoSrcLayerIDDelta;                       // parameter_sets_info_src_layer_id_delta
        uint8_t     *ConversionTypeIDC;                            // conversion_type_idc
        bool       **RewritingInfoFlag;                            // rewriting_info_flag
        uint32_t   **RewritingProfileLevelIDC;                     // rewriting_profile_level_idc
        uint16_t   **RewritingAverageBitrate;                      // rewriting_avg_bitrate
        uint16_t   **RewritingMaxBitrate;                          // rewriting_max_bitrate
        uint8_t      NumDependencyLayersForPriorityLayer;          // pr_num_dIds_minus1
        uint8_t     *NumPriorityLayers;                            // pr_num_minus1
        uint8_t    **PriorityLayerID;                              // pr_id
        uint32_t   **PriorityLevelIDC;                             // pr_profile_level_idc
        uint8_t    **PriorityLevelAvgBitrate;                      // pr_avg_bitrate
        uint8_t    **PriorityLevelMaxBitrate;                      // pr_max_bitrate
        uint8_t      PriorityIDURIIndex;                           // PriorityIdSettingUriIdx
        uint8_t      PriorityIDSettingURI;                         // priority_id_setting_uri aka UTF8 url
        uint8_t     *PriorityIDSettingURIString;                   // string to contain that
        
        /* Pic Timing Info */
        bool         CpbDpbDelaysPresentFlag:1;                    // CpbDpbDelaysPresentFlag
    } SupplementalEnhancementInfo;
    
    struct AVCFile {
        /* Global Variables */
        size_t                           CurrentFilePosition;
        size_t                           NALSize;
        
        /* Global Flags */
        bool                             StreamIsLowDelay:1;
        
        /* Regular AVC */
        NALHeader                                *NAL;
        
        SequenceParameterSet                     *SPS; // [MaxSeqParamSets]
        PictureParameterSet                      *PPS; // [MaxSeqParamSets]
        VideoUsabilityInformation                *VUI;
        HypotheticalReferenceDecoder             *HRD;
        SupplementalEnhancementInfo              *SEI;
        
        /* SVC */
        ScalableVideoCoding                      *SVC;
        
        /* Slice */
        Slice                                  *Slice;
        
        /**            MVC Extension               **/
        
        /* MVC */
        DepthParameterSet                        *DPS;
        
        /* MacroBlock */
        MacroBlock                        *MacroBlock;
        
        /* Raw PCM */
        uint16_t      **RawMacroBlock;
        uint8_t     ***DisparityScale;
        uint8_t    ***DisparityOffset;
    };
    
#ifdef __cplusplus
}
#endif

#endif /* LIBMODERNAVC_AVCTYPES_H */
