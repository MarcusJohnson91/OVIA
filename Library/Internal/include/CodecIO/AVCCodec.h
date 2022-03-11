/*!
 @header              AVCCodec.h
 @author              Marcus Johnson
 @copyright           2016+
 @version             1.1.0
 @brief               This header contains code for AVC (encoding and decoding).
 @remak               ONLY LOSSLESS versions of AVC will ever be supported.
 */

#pragma once

#ifndef OVIA_CodecIO_AVCCodec_h
#define OVIA_CodecIO_AVCCodec_h

#include "../../../OVIA/include/CodecIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    typedef enum AVCConstants {
        AVCMagic                                    = 0x00000001,
        MVCMaxViews                                 = 2,
        MaxChromaChannels                           = 2,
        AVCMaxChannels                              = 3,
        MaxSeqParamSets                             = 4,
        MaxPicParamSets                             = 4,
        MaxSliceGroups                              = 8,
        MaxLevels                                   = 64,
        ExtendedISO                                 = 255,
        MacroBlockMaxPixels                         = 256,
        AVCStopBit                                  =   1,
    } AVCConstants;
    
    typedef enum AVCCompressionType {
        ExpGolomb                                   =  0,
        Arithmetic                                  =  1,
    } AVCCompressionType;
    
    typedef enum AVCChromaFormats {
        ChromaBW                                    =  0, // Black and white, When Profile 183, Chroma is assumed to be this, default is Chroma420
        Chroma420                                   =  1,
        Chroma422                                   =  2,
        Chroma444                                   =  3,
    } AVCChromaFormats;
    
    typedef enum AVCSampleAspectRatios {
        SAR_Unspecified                             =  0,
        SAR_Square                                  =  1,
        SAR_Twelve_Eleven                           =  2,
        SAR_Ten_Eleven                              =  3,
        SAR_Sixteen_Elevem                          =  4,
        SAR_Fourty_ThirtyThree                      =  5,
        SAR_TwentyFour_Eleven                       =  6,
        SAR_Twenty_Eleven                           =  7,
        SAR_ThirtyTwo_Eleven                        =  8,
        SAR_Eighty_ThirtyThree                      =  9,
        SAR_Eighteen_Eleven                         = 10,
        SAR_Fifteen_Eleven                          = 11,
        SAR_SixtyFour_ThirtyThree                   = 12,
        SAR_OneSixty_NinetyNine                     = 13,
        SAR_Four_Three                              = 14,
        SAR_Three_Two                               = 15,
        SAR_Two_One                                 = 16,
    } AVCSampleAspectRatios;
    
    typedef enum RefinedSliceGroupMapType {
        BoxOutClockwise                             = 0, // Box-out clockwise
        BoxOutCounterClockwise                      = 1, // Box-out counter-clockwise
        RasterScan                                  = 2, // Raster scan
        ReverseRasterScan                           = 4, // Reverse raster scan
        WipeRight                                   = 5, // Wipe right
        WipeLeft                                    = 6, // Wipe left
    } RefinedSliceGroupMapType;
    
    typedef enum AVCSliceType { // A group of macroblocks, a slice group contains all the macroblocks in a frame
        SliceP1                                     = 0, // Intra or Inter prediction, + 1 motion vector
        SliceP2                                     = 5, // Intra or Inter prediction, + 1 motion vector
        SliceB1                                     = 1, // Intra or Inter prediction, + 2 motion vectors
        SliceB2                                     = 6, // Intra or Inter prediction, + 2 motion vectors
        SliceI1                                     = 2, // Intra only
        SliceI2                                     = 7, // Intra only
        SliceSP1                                    = 3, // Intra or Inter prediction
        SliceSP2                                    = 8, // Intra or Inter prediction
        SliceSI1                                    = 4, // Intra Prediction only
        SliceSI2                                    = 9, // Intra Prediction only
        
        /* SVC Slices */
        SliceEP1                                    = 0,
        SliceEP2                                    = 5,
        SliceEB1                                    = 1,
        SliceEB2                                    = 6,
        SliceEI1                                    = 2,
        SliceEI2                                    = 7,
    } AVCSliceType;
    
    typedef enum FilmGrainType { // blending_mode_id
        AdditiveGrain                               =  0,
        MultiplicativeGrain                         =  1,
    } FilmGrainType;
    
    typedef enum VideoType {
        Component                                   =  0,
        PAL                                         =  1,
        NTSC                                        =  2,
        SECAM                                       =  3,
        MAC                                         =  4,
        Unspecified                                 =  5,
        Reserved1                                   =  6,
        Reserved2                                   =  7,
    } VideoType;
    
    typedef enum ChromaIntraPredictionMode { // intra_chroma_pred_mode
        ChromaIntra_DC                              =  0,
        ChromaIntra_Horizontal                      =  1,
        ChromaIntra_Vertical                        =  2,
        ChromaIntra_Plane                           =  3,
    } ChromaIntraPredictionMode;
    
    typedef enum IntraPrediction4x4Mode { // Intra4x4PredMode[luma4x4BlkIdx]
        Intra4x4Vertical                            =  0, // Intra_4x4_Vertical
        Intra4x4Horizontal                          =  1, // Intra_4x4_Horizontal
        Intra4x4DC                                  =  2, // Intra_4x4_DC
        Intra4x4DiagonalDownLeft                    =  3, // Intra_4x4_Diagonal_Down_Left
        Intra4x4DiagonalDownRight                   =  4, // Intra_4x4_Diagonal_Down_Right
        Intra4x4VerticalRight                       =  5, // Intra_4x4_Vertical_Right
        Intra4x4HorizontalDown                      =  6, // Intra_4x4_Horizontal_Down
        Intra4x4VerticalLeft                        =  7, // Intra_4x4_Vertical_Left
        Intra4x4HorizontalUp                        =  8, // Intra_4x4_Horizontal_Up
    } IntraPrediction4x4Mode;
    
    typedef enum PMacroBlockSubTypes { // sub_mb_type[mbPartIdx]
        P_L0_8x8                                    =  0, // P_L0_8x8
        P_L0_8x4                                    =  1, // P_L0_8x4
        P_L0_4x8                                    =  2, // P_L0_4x8
        P_L0_4x4                                    =  3, // P_L0_4x4
    } PMacroBlockSubTypes;
    
    typedef enum BMacroBlockSubTypes {   // sub_mb_type
        B_Direct_8x8                                =  0,
        B_L0_8x8                                    =  1,
        B_L1_8x8                                    =  2,
        B_Bi_8x8                                    =  3,
        B_L0_8x4                                    =  4,
        B_L0_4x8                                    =  5,
        B_L1_8x4                                    =  6,
        B_L1_4x8                                    =  7,
        B_Bi_8x4                                    =  8,
        B_Bi_4x8                                    =  9,
        B_L0_4x4                                    = 10,
        B_L1_4x4                                    = 11,
        B_Bi_4x4                                    = 12,
    } BMacroBlockSubTypes;
    
    typedef enum BSliceMacroBlockTypes {
        B_Direct_16x16                              =  0,
        B_L0_16x16                                  =  1,
        B_L1_16x16                                  =  2,
        B_Bi_16x16                                  =  3,
        B_L0_L0_16x8                                =  4,
        B_L0_L0_8x16                                =  5,
        B_L1_L1_16x8                                =  6,
        B_L1_L1_8x16                                =  7,
        B_L0_L1_16x8                                =  8,
        B_L0_L1_8x16                                =  9,
        B_L1_L0_16x8                                = 10,
        B_L1_L0_8x16                                = 11,
        B_L0_Bi_16x8                                = 12,
        B_L0_Bi_8x16                                = 13,
        B_L1_Bi_16x8                                = 14,
        B_L1_Bi_8x16                                = 15,
        B_Bi_L0_16x8                                = 16,
        B_Bi_L0_8x16                                = 17,
        B_Bi_L1_16x8                                = 18,
        B_Bi_L1_8x16                                = 19,
        B_Bi_Bi_16x8                                = 20,
        B_Bi_Bi_8x16                                = 21,
        B_8x8                                       = 22,
        // 23 - 255 = inferred to be B_Skip
        B_Skip                                      = 23,
    } BSliceMacroBlockTypes;
    
    typedef enum PSliceMacroBlockTypes { // Covers bot P and SP types. Table 7-13
        P_L0_16x16                                  =  0,
        P_L0_L0_16x8                                =  1,
        P_L0_L0_8x16                                =  2,
        P_8x8                                       =  3,
        P_8x8ref0                                   =  4,
        P_Skip                                      =  5,
    } PSliceMacroBlockTypes;
    
    typedef enum ISliceMacroBlockTypes { // Table 7-11; everything elses is I_PCM
        I_NxN                                       =  0,
        I_16x16_0_0_0                               =  1,
        I_16x16_1_0_0                               =  2,
        I_16x16_2_0_0                               =  3,
        I_16x16_3_0_0                               =  4,
        I_16x16_0_1_0                               =  5,
        I_16x16_1_1_0                               =  6,
        I_16x16_2_1_0                               =  7,
        I_16x16_3_1_0                               =  8,
        I_16x16_0_2_0                               =  9,
        I_16x16_1_2_0                               = 10,
        I_16x16_2_2_0                               = 11,
        I_16x16_3_2_0                               = 12,
        I_16x16_0_0_1                               = 13,
        I_16x16_1_0_1                               = 14,
        I_16x16_2_0_1                               = 15,
        I_16x16_3_0_1                               = 16,
        I_16x16_0_1_1                               = 17,
        I_16x16_1_1_1                               = 18,
        I_16x16_2_1_1                               = 19,
        I_16x16_3_1_1                               = 20,
        I_16x16_0_2_1                               = 21,
        I_16x16_1_2_1                               = 22,
        I_16x16_2_2_1                               = 23,
        I_16x16_3_2_1                               = 24,
        I_PCM                                       = 25,
    } ISliceMacroBlockTypes;
    
    typedef enum MacroBlockPredictionMode { // TODO: Fix me
        Intra_16x16                                 = 0,
        Intra_8x8                                   = 0,
        Intra_4x4                                   = 0,
        Inter_16x16                                 = 0,
        Inter_8x8                                   = 0,
        Inter_4x4                                   = 0,
    } MacroBlockPredictionMode;
    
    typedef enum PrimaryPictureType {
        wat                                         = 0,
    } PrimaryPictureType;
    
    typedef enum NonPartitionedMacroBlockPredictionMode {
        Direct                                      = 0,
        Pred_L0                                     = 1,
        Pred_L1                                     = 2,
        BiPred                                      = 3,
    } NonPartitionedMacroBlockPredictionMode;
    
    typedef enum SubMacroBlockTypes {
        B_SubMacroBlock                             = 0,
        P_SubMacroBlock                             = 1,
    } SubMacroBlockTypes;
    
    typedef enum NALTypes {
        NAL_Unspecified0                            =  0,
        NAL_NonIDRSlice                             =  1,
        NAL_SlicePartitionA                         =  2,
        NAL_SlicePartitionB                         =  3,
        NAL_SlicePartitionC                         =  4,
        NAL_IDRSliceNonPartitioned                  =  5,
        NAL_SupplementalEnhancementInfo             =  6,
        NAL_SequenceParameterSet                    =  7,
        NAL_PictureParameterSet                     =  8,
        NAL_AccessUnitDelimiter                     =  9,
        NAL_EndOfSequence                           = 10,
        NAL_EndOfStream                             = 11,
        NAL_FillerData                              = 12,
        NAL_SequenceParameterSetExtended            = 13,
        NAL_PrefixUnit                              = 14,
        NAL_SubsetSequenceParameterSet              = 15,
        NAL_DepthParameterSet                       = 16,
        NAL_Reserved1                               = 17,
        NAL_Reserved2                               = 18,
        NAL_AuxiliarySliceNonPartitioned            = 19,
        NAL_AuxiliarySliceExtension                 = 20,
        NAL_MVCDepthView                            = 21,
        NAL_Reserved3                               = 22,
        NAL_Reserved4                               = 23,
        NAL_Unspecified1                            = 24,
        NAL_Unspecified2                            = 25,
        NAL_Unspecified3                            = 26,
        NAL_Unspecified4                            = 27,
        NAL_Unspecified5                            = 28,
        NAL_Unspecified6                            = 29,
        NAL_Unspecified7                            = 30,
        NAL_Unspecified8                            = 31,
    } NALTypes;
    
    typedef enum SEITypes {
        SEI_BufferingPeriod                         =   0, // buffering_period
        SEI_PictureTiming                           =   1, // pic_timing
        SEI_PanScan                                 =   2, // pan_scan_rect
        SEI_Filler                                  =   3, // filler_payload
        SEI_RegisteredUserData                      =   4, // ITU-T-T35
        SEI_UnregisteredUserData                    =   5, // user_data_unregistered
        SEI_RecoveryPoint                           =   6, // recovery_point
        SEI_RepetitiveReferencePicture              =   7, // dec_ref_pic_marking_repetition
        SEI_SparePicture                            =   8, // spare_pic
        SEI_SceneInformation                        =   9, // scene_info
        SEI_SubSequenceInformation                  =  10, // sub_seq_info
        SEI_SubSequenceLayerProperties              =  11, // sub_seq_layer_characteristics
        SEI_SubSequenceProperties                   =  12, // sub_seq_characteristics
        SEI_FullFrameFreeze                         =  13, // full_frame_freeze
        SEI_FullFrameFreezeRelease                  =  14, // full_frame_freeze_release
        SEI_FullFrameSnapshot                       =  15, // full_frame_snapshot
        SEI_ProgressiveRefinementSegmentStart       =  16, // progressive_refinement_segment_start
        SEI_ProgressiveRefinementSegmentEnd         =  17, // progressive_refinement_segment_end
        SEI_MotionConstrainedSliceGroup             =  18, // motion_constrained_slice_group_set
        SEI_FilmGrainCharacteristics                =  19, // film_grain_characteristics
        SEI_DeblockingFilterDisplayPreferences      =  20, // deblocking_filter_display_preference
        SEI_StereoVideoInformation                  =  21, // stereo_video_info
        SEI_PostFilterHint                          =  22, // post_filter_hint
        SEI_ToneMappingInformation                  =  23, // tone_mapping_info
        SEI_ScalabilityInformation                  =  24, // scalability_info /* specified in Annex G */
        SEI_SubPictureScalableLayer                 =  25, // sub_pic_scalable_layer
        SEI_NonRequiredLayerRep                     =  26, // non_required_layer_rep
        SEI_PriorityLayerInformation                =  27, // priority_layer_info
        SEI_LayersNotPresent                        =  28, // layers_not_present
        SEI_LayerDependencyChange                   =  29, // layer_dependency_change
        SEI_ScalableNesting                         =  30, // scalable_nesting
        SEI_BaseLayerTemporalHRD                    =  31, // base_layer_temporal_hrd
        SEI_QualityLayerIntegrityCheck              =  32, // quality_layer_integrity_check
        SEI_RedundantPictureProperty                =  33, // redundant_pic_property
        SEI_TemporalL0DependencyRepresentationIndex =  34, // tl0_dep_rep_index
        SEI_TemporalLevelSwitchingPoint             =  35, // tl_switching_point
        /* MVC SEI Messages */
        SEI_ParallelDecodingInformation             =  36, // parallel_decoding_info
        SEI_MVCScalableNesting                      =  37, // mvc_scalable_nesting
        SEI_ViewScalabilityInformation              =  38, // view_scalability_info
        SEI_MVCSceneInformation                     =  39, // multiview_scene_info
        SEI_MVCAquisitionInformation                =  40, // multiview_acquisition_info
        SEI_NonRequiredViewComponent                =  41, // non_required_view_component
        SEI_ViewDependencyChange                    =  42, // view_dependency_change
        SEI_OperationPointsNotPresent               =  43, // operation_points_not_present
        SEI_BaseViewTemporalHRD                     =  44, // base_view_temporal_hrd
        SEI_FramePackingArrangement                 =  45, // frame_packing_arrangement
        SEI_MVCViewPosition                         =  46, // multiview_view_position
        SEI_DisplayOrientation                      =  47, // display_orientation
        SEI_MVCDScalableNesting                     =  48, // mvcd_scalable_nesting
        SEI_MVCDViewScalabilityInformation          =  49, // mvcd_view_scalability_info
        SEI_DepthRepresentationInformation          =  50, // depth_representation_info
        SEI_3DReferenceDisplaysInformation          =  51, // three_dimensional_reference_displays_info
        SEI_DepthTiming                             =  52, // depth_timing
        SEI_DepthSamplingInformation                =  53, // depth_sampling_info
        SEI_MVCConstrainedDPSIdentifier             =  54, // constrained_depth_parameter_set_identifier
        SEI_MasteringDisplayColorVolume             = 137, // mastering_display_color_volume
    } SEITypes;
    
    typedef enum ProfileTypes {
        Constrained_Baseline_Profile                =  66, // + constrainedflag1 = 1
        Baseline_Profile                            =  66,
        Main_Profile                                =  77,
        Scalable_Baseline_Profile                   =  83,
        Scalable_Constrained_Baseline_Profile       =  83,
        Scalable_High_Profile                       =  86,
        Constrained_Scalable_High_Profile           =  86, // + constraintflag5 = 1
        Scalable_High_Intra_Profile                 =  86, // + constraintflag3 = 1
        Extended_Profile                            =  88,
        High_Profile                                = 100,
        Constrained_High_Profile                    = 100, // + Flag 4 & 5 = 1
        Progressive_High_Profile                    = 100, // + ConstraintFlag4 = 1
        High10_Profile                              = 110,
        High10_Intra_Profile                        = 110, // Contraint3 = 1
        MultiView_High_Profile                      = 118,
        High422_Profile                             = 122, // 10 bits at 4:2:2
        High422_Intra_Profile                       = 122, // Constraint3 = 1
        Stereo_High_Profile                         = 128,
        MultiView_Depth_High_Profile                = 138,
        High444_Predictive_Profile                  = 244, // 8 10 14 bits
        CAVLC444_Intra_Profile                      = 244, //
    } ProfileTypes;
    
    typedef struct SequenceParameterSet { // Whole video variables
        bool         ConstraintFlag0:1;                            // constraint_set0_flag
        bool         ConstraintFlag1:1;                            // constraint_set1_flag
        bool         ConstraintFlag2:1;                            // constraint_set2_flag
        bool         ConstraintFlag3:1;                            // constraint_set3_flag
        bool         ConstraintFlag4:1;                            // constraint_set4_flag
        bool         ConstraintFlag5:1;                            // constraint_set5_flag
        bool         SeperateColorPlane:1;                         // seperate_color_plane_flag
        bool         QPPrimeBypassFlag:1;                          // qpprime_y_zero_transform_bypass_flag
        bool         ScalingMatrixFlag:1;                          // seq_scaling_matrix_present_flag
        bool         DeltaPicOrder:1;                              // delta_pic_order_always_zero_flag
        bool         GapsInFrameNumAllowed:1;                      // gaps_in_frame_num_value_allowed_flag
        bool         OnlyMacroBlocksInFrame:1;                     // frame_mbs_only_flag
        bool         AdaptiveFrameFlag:1;                          // mb_adaptive_frame_field_flag
        bool         Inference8x8:1;                               // direct_8x8_inference_flag
        bool         FrameCroppingFlag:1;                          // frame_cropping_flag
        bool         VUIPresent:1;                                 // vui_parameters_present_flag
        bool         AlphaIncrFlag:1;                              // alpha_incr_flag
        bool         AdditionalExtensionFlag:1;                    // additional_extension_flag
        bool         SVCVUIParamsPresent:1;                        // svc_vui_parameters_present_flag
        bool         DefaultGridPositionFlag:1;                    // default_grid_position_flag
        bool         RPUFilterFlag:1;                              // rpu_filter_enabled_flag
        bool         RPUFieldFlag:1;                               // rpu_field_processing_flag
        bool         MVCDVUIParametersPresent:1;                   // mvcd_vui_parameters_present_flag
        bool         MVCDTextureVUIParametersPresent:1;            // texture_vui_parameters_present_flag
        bool         SlicePrediction:1;                            // slice_header_prediction_flag
        bool         ReducedResolutionFlag:1;                      // reduced_resolution_flag
        bool         DepthFrameCroppingFlag:1;                     // depth_frame_cropping_flag
        bool         ALCSpsEnableFlag:1;                           // alc_sps_enable_flag
        bool         EnableRLESkipFlag:1;                          // enable_rle_skip_flag
        bool         MVCVUIParamsPresent:1;                        // mvc_vui_parameters_present_flag
        bool         AdditionalExtension2:1;                       // additional_extension2_flag
        bool         AdditionalExtension2DataFlag:1;               // additional_extension2_data_flag
        uint8_t      ChromaFormatIDC:2;                            // chroma_format_idc
        uint8_t      ChromaArrayType:2;                            // ChromaArrayType
        uint8_t      PicOrderCount:2;                              // pic_order_cnt_type
        uint8_t      AuxiliaryFormatID:2;                          // aux_format_idc
        uint8_t      AVC3DAcquisitionIDC:2;                        // 3dv_acquisition_idc
        uint8_t      LumaBitDepthMinus8:4;                         // bit_depth_luma_minus8
        uint8_t      ChromaBitDepthMinus8:4;                       // bit_depth_chroma_minus8
        uint8_t      BitDepthLuma:4;                               // BitDepthL
        uint8_t      BitDepthChroma:4;                             // BitDepthC
        uint8_t      MaxFrameNumMinus4:4;                          // log2_max_frame_num_minus4
        uint8_t      MaxPicOrder:4;                                // log2_max_pic_order_cnt_lsb_minus4
        uint8_t      AuxiliaryBitDepth:4;                          // bit_depth_aux_minus8
        uint32_t     AlphaOpaqueValue:5;                           // alpha_opaque_value
        uint8_t      AlphaTransparentValue:5;                      // alpha_transparent_value
        uint8_t      SeqParamSetID:5;                              // seq_parameter_set_id
        uint8_t      MacroBlockWidthChroma:5;                      // MbWidthC
        uint8_t      MacroBlockHeightChroma:5;                     // MbHeightC
        uint8_t      MFCFormatIDC:6;                               // mfc_format_idc
        uint8_t      DepthHorizontalRSH:6;                         // depth_hor_rsh
        uint8_t      DepthVerticalRSH:6;                           // depth_ver_rsh
        uint8_t      NumLevelValues:7;                             // num_level_values_signalled_minus1
        uint16_t     ViewCount:11;                                 // num_views_minus1
        uint16_t     DepthHorizontalDisparity:11;                  // depth_hor_mult_minus1
        uint16_t     DepthVerticalDisparity:11;                    // depth_ver_mult_minus1
        uint16_t     GridPosViewCount:11;                          // grid_pos_num_views
        uint8_t      ProfileIDC;                                   // profile_idc
        uint8_t      QPBDOffsetChroma;                             // QpBdOffsetC
        int32_t      OffsetNonRefPic;                              // offset_for_non_ref_pic
        int32_t      OffsetTop2Bottom;                             // offset_for_top_to_bottom_field
        uint8_t      RefFramesInPicOrder;                          // num_ref_frames_in_pic_order_cnt_cycle
        int32_t      MaxRefFrames;                                 // max_num_ref_frames
        uint16_t     PicWidthInMacroBlocksMinus1;                  // pic_width_in_mbs_minus1
        uint32_t     PicHeightInMapUnitsMinus1;                    // pic_height_in_map_units_minus1
        uint64_t     FrameCropLeft;                                // frame_crop_left_offset
        uint64_t     MaxFrameNum;                                  // MaxFrameNum
        uint64_t     FrameCropRight;                               // frame_crop_right_offset
        uint64_t     FrameCropTop;                                 // frame_crop_top_offset
        uint64_t     FrameCropBottom;                              // frame_crop_bottom_offset
        uint8_t      DepthPicWidthInMacroBlocks;                   // depth_pic_width_in_mbs_minus1
        uint8_t      DepthPicHeightInMapUnits;                     // depth_pic_height_in_map_units_minus1
        uint32_t     DepthFrameLeftCropOffset;                     // depth_frame_crop_left_offset
        uint32_t     DepthFrameRightCropOffset;                    // depth_frame_crop_right_offset
        uint32_t     DepthFrameTopCropOffset;                      // depth_frame_crop_top_offset
        uint32_t     DepthFrameBottomCropOffset;                   // depth_frame_crop_bottom_offset
        uint8_t     *AVC3DViewID;                                  // view_id_3dv
        uint8_t     *GridPosViewID;                                // grid_pos_view_id
        uint8_t     *GridPosX;                                     // grid_pos_x
        uint8_t     *GridPosY;                                     // grid_pos_y
        uint16_t    *NumApplicableOps;                             // num_applicable_ops_minus1
        uint8_t     *MVCViewX;                                     // view0_grid_position_x view1_grid_position_x
        uint8_t     *MVCViewY;                                     // view0_grid_position_y view1_grid_position_y
        bool        *DepthViewPresent;                             // depth_view_present_flag
        uint8_t     *DepthViewID;                                  // DepthViewId
        bool        *TextureViewPresent;                           // texture_view_present_flag
        uint8_t    **LevelIDC;                                     // level_idc
        unsigned   **ScalingListFlag;                              // seq_scaling_list_present_flag
        int32_t    **RefFrameOffset;                               // offset_for_ref_frame
        uint16_t   **ViewID;                                       // view_id
        uint8_t    **AnchorRefsCount;                              // num_anchor_refs_l0 num_anchor_refs_l1
        uint8_t    **AppOpTemporalID;                              // applicable_op_temporal_id
        uint16_t   **AppOpNumTargetViews;                          // applicable_op_num_target_views_minus1
        uint8_t    **AppOpNumViews;                                // applicable_op_num_views_minus1
        uint8_t    **AppOpTextureViews;                            // applicable_op_num_texture_views_minus1
        uint8_t    **AppOpNumDepthViews;                           // applicable_op_num_depth_views
        uint16_t   **MVCDAppOpDepthViews;                          // applicable_op_num_depth_views_minus1
        uint32_t  ***AnchorRef;                                    // anchor_ref_l0 anchor_ref_l1
        uint8_t   ***NonAnchorRefCount;                            // num_non_anchor_refs_l0 num_non_anchor_refs_l1
        uint16_t  ***NonAnchorRef;                                 // non_anchor_ref_l0 non_anchor_ref_l1
        uint8_t   ***AppOpTargetViewID;                            // applicable_op_target_view_id
        bool      ***AppOpDepthFlag;                               // applicable_op_depth_flag
        bool      ***AppOpTextureFlag;                             // applicable_op_texture_flag
    } SequenceParameterSet;

    typedef struct PictureParameterSet {
        bool         EntropyCodingMode:1;                          // entropy_coding_mode_flag
        bool         BottomPicFieldOrderInSliceFlag:1;             // bottom_field_pic_order_in_frame_present_flag
        bool         SliceGroupChangeDirection:1;                  // slice_group_change_direction_flag
        bool         WeightedPrediction:1;                         // weighted_pred_flag
        bool         DeblockingFilterFlag:1;                       // deblocking_filter_control_present_flag
        bool         ConstrainedIntraFlag:1;                       // constrained_intra_pred_flag
        bool         RedundantPictureFlag:1;                       // redundant_pic_cnt_present_flag
        bool         TransformIs8x8:1;                             // transform_8x8_mode_flag
        bool         SeperateScalingMatrix:1;                      // pic_scaling_matrix_present_flag
        uint8_t      WeightedBiPrediction:2;                       // weighted_bipred_idc
        uint8_t      SliceGroups:3;                                // num_slice_groups_minus1
        uint8_t      SliceGroupMapType:3;                          // slice_group_map_type
        uint8_t      ChromaQPOffset:5;                             // chroma_qp_index_offset
        uint8_t      ChromaQPOffset2:5;                            // second_chroma_qp_index_offset
        uint8_t      RedundantPictureCount:7;                      // redundant_pic_cnt
        uint8_t      PicParamSetID;                                // pic_parameter_set_id
        uint8_t      SliceGroupChangeRate;                         // slice_group_change_rate_minus1
                                                                   // SliceChangeRate = 1024 * PicHeightInMapUnits
        uint8_t      InitialSliceQP;                               // pic_init_qp_minus26
        uint8_t      InitialSliceQS;                               // pic_init_qs_minus26
        uint16_t     PicSizeInMapUnits;                            // pic_size_in_map_units_minus1
        uint64_t    *RunLength;                                    // run_length_minus1
        uint8_t     *TopLeft;                                      // top_left
        uint8_t     *BottomRight;                                  // bottom_right
        uint8_t     *SliceGroupID;                                 // slice_group_id
        uint8_t     *RefIndex;                                     // num_ref_idx_l0_default_active_minus1 num_ref_idx_l1_default_active_minus1
        bool        *PicScalingList;                               // pic_scaling_list_present_flag
        bool        *DefaultScalingMatrix4x4;                      // UseDefaultScalingMatrix4x4Flag
        bool        *DefaultScalingMatrix8x8;                      // UseDefaultScalingMatrix8x8Flag
    } PictureParameterSet;

    typedef struct Slice {
        bool         SliceIsInterlaced:1;                          // field_pic_flag
        bool         SliceIsBottomField:1;                         // bottom_field_flag
        bool         SliceIsIDR:1;                                 // IdrPicFlag
        bool         DecodePMBAsSPSlice:1;                         // sp_for_switch_flag
        bool         NumRefIDXActiveOverrideFlag:1;                // num_ref_idx_active_override_flag
        bool         DirectSpatialMVPredictionFlag:1;              // direct_spatial_mv_pred_flag
        bool         DepthWeightedPredictionFlag:1;                // depth_weighted_pred_flag
        bool         DMVPFlag:1;                                   // dmvp_flag
        bool         SeqViewSynthesisFlag:1;                       // seq_view_synthesis_flag
        bool         SliceVSPFlag:1;                               // slice_vsp_flag
        bool         SkipTypeMacroBlockFlag:1;                     // mb_skip_type_flag
        bool         SkipMotionVectors:1;                          // mb_vsskip_flag
        bool         SkipMacroBlock:1;                             // mb_skip_flag
        bool         MacroBlockSkipRunType:1;                      // mb_skip_run_type
        bool         SkipMacroBlockALC:1;                          // mb_alc_skip_flag
        bool         MacroBlockFieldDecodingFlag:1;                // mb_field_decoding_flag
        bool         IsEndOfSlice:1;                               // end_of_slice_flag
        bool         SeperateColorPlaneFlag:1;                     // separate_color_plane_flag
        bool         BasePredictionWeightTable:1;                  // base_pred_weight_table_flag
        bool         StoreRefBasePicFlag:1;                        // store_ref_base_pic_flag
        bool         ConstrainedIntraResamplingFlag:1;             // constrained_intra_resampling_flag
        bool         RefLayerChromaPhaseXFlag:1;                   // ref_layer_chroma_phase_x_plus1_flag
        bool         SkipSliceFlag:1;                              // slice_skip_flag
        bool         AdaptiveBaseModeFlag:1;                       // adaptive_base_mode_flag
        bool         DefaultBaseModeFlag:1;                        // default_base_mode_flag
        bool         AdaptiveMotionPredictionFlag:1;               // adaptive_motion_prediction_flag
        bool         DefaultMotionPredictionFlag:1;                // default_motion_prediction_flag
        bool         AdaptiveResidualPredictionFlag:1;             // adaptive_residual_prediction_flag
        bool         DefaultResidualPredictionFlag:1;              // default_residual_prediction_flag
        bool         ResidualPredictionFlag:1;                     // residual_prediction_flag
        bool         TCoefficentPredictionFlag:1;                  // tcoeff_level_prediction_flag
        bool         MbaffFrameFlag:1;                             // MbaffFrameFlag
        uint8_t      DisableDeblockingFilterIDC:2;                 // disable_deblocking_filter_idc
        uint8_t      CabacInitIDC:2;                               // cabac_init_idc
        uint8_t      PreSliceHeaderSrc:2;                          // pre_slice_header_src
        uint8_t      PreRefListSrc:2;                              // pre_ref_lists_src
        uint8_t      PrePredictionWeightTableSrc:2;                // pre_pred_weight_table_src
        uint8_t      PreDecReferencePictureMarkingSrc:2;           // pre_dec_ref_pic_marking_src
        uint8_t      ModPicNumsIDC:2;                              // modification_of_pic_nums_idc
        uint8_t      RefLayerChromaPhaseY:2;                       // ref_layer_chroma_phase_y_plus1
        uint8_t      PictureType:3;                                // primary_pic_type
        uint8_t      ColorPlaneID:3;                               // color_plane_id
        uint8_t      DisableInterLayerDeblocking:3;                // disable_inter_layer_deblocking_filter_idc
        uint8_t      Type:4;                                       // slice_type
        uint8_t      SliceAlphaOffsetC0:4;                         // slice_alpha_c0_offset_div2
        uint8_t      SliceBetaOffset:4;                            // slice_beta_offset_div2
        uint8_t      InterLayerSliceAplhaC0Offset:4;               // inter_layer_slice_alpha_c0_offset_div2
        uint8_t      InterLayerSliceBetaOffset:4;                  // inter_layer_slice_beta_offset_div2
        uint8_t      ScanIndexStart:4;                             // scan_idx_start
        uint8_t      ScanIndexEnd:4;                               // scan_idx_end
        uint8_t      AbsDiffPicNum:4;                              // abs_diff_pic_num_minus1
        uint8_t      SliceQSDelta:6;                               // slice_qs_delta
        uint8_t      RefLayerDQID:6;                               // ref_layer_dq_id
        uint8_t      LumaWeightDenom:7;                            // luma_log2_weight_denom
        uint8_t      ChromaWeightDenom:7;                          // chroma_log2_weight_denom
        uint32_t     PictureOrderCountLSB:17;                      // pic_order_cnt_lsb
        int8_t       SliceQPDelta;                                 // slice_qp_delta
        uint8_t      TopFieldOrderCount;                           // TopFieldOrderCnt
        uint8_t      BottomFieldOrderCount;                        // BottomFieldOrderCnt
        uint8_t      MacroBlockSkipRun;                            // mb_skip_run
        uint8_t      PreviousMacroBlockSkipped;                    // prevMbSkipped
        uint8_t      DepthParamSetID;                              // dps_id
        uint8_t      AbsDiffViewIdx;                               // abs_diff_view_idx_minus1
        int16_t      ScaledRefLayerLeftOffset;                     // scaled_ref_layer_left_offset
        int16_t      ScaledRefLayerTopOffset;                      // scaled_ref_layer_top_offset
        int16_t      ScaledRefLayerRightOffset;                    // scaled_ref_layer_right_offset
        int16_t      ScaledRefLayerBottomOffset;                   // scaled_ref_layer_bottom_offset
        uint16_t     IDRPicID;                                     // idr_pic_id
        uint32_t     FirstMacroBlockInSlice;                       // first_mb_in_slice
        uint32_t     MacroBlocksInSlice;                           // num_mbs_in_slice_minus1
        uint64_t     ScaledRefLayerPicHeightInSamplesL;            // ScaledRefLayerPicHeightInSamplesL
        uint64_t     LongTermPicNum;                               // long_term_pic_num, Minimum MaxDpbMbs / ( PicWidthInMbs * FrameHeightInMbs ), 16 ) + 1
        uint64_t     CurrentMacroBlockAddress;                     // CurrMbAddr
        uint64_t     SliceGroupChangeCycle;                        // slice_group_change_cycle
        uint64_t     SliceGroupChangeRate;                         // slice_group_change_rate
        uint64_t     FrameNumber;                                  // frame_num
        uint64_t     PicWidthInSamplesChroma;                      // PicWidthInSamplesC
        uint64_t     PicWidthInSamplesLuma;                        // PicWidthInSamplesL
        uint64_t     FrameHeightInMacroBlocks;                     // FrameHeightInMbs
        uint64_t     PicHeightInMapUnits;                          // PicHeightInMapUnits
        uint64_t     PicWidthInMacroBlocks;                        // PicWidthInMbs
        uint64_t     PicHeightInMacroBlocks;                       // PicHeightInMbs
        int64_t      PicSizeInMacroBlocks;                         // PicSizeInMbs
        uint64_t     RawMacroBlockSizeInBits;                      // RawMbBits
        int32_t     *DeltaPicOrderCount;                           // delta_pic_order_cnt
        uint8_t     *DeltaPictureOrderCountBottom;                 // delta_pic_order_cnt_bottom
        bool        *RefPicListModFlag;                            // ref_pic_list_modification_flag_l0, ref_pic_list_modification_flag_l1
        bool        *LumaWeightFlag;                               // luma_weight_l0_flag luma_weight_l1_flag
        bool        *ChromaWeightFlag;                             // chroma_weight_l0_flag chroma_weight_l1_flag
        int8_t     **LumaWeight;                                   // luma_weight_l0 luma_weight_l1
        int8_t     **LumaOffset;                                   // luma_offset_l0 luma_offset_l1
        int8_t    ***ChromaWeight;                                 // chroma_weight_l0 chroma_weight_l1
        int8_t    ***ChromaOffset;                                 // chroma_offset_l0 chroma_offset_l1
    } Slice;

    typedef struct NALHeader {
        bool         SVCExtensionFlag:1;                           // svc_extension_flag
        bool         AVC3DExtensionFlag:1;                         // avc_3d_extension_flag
        bool         EmptyPictureBufferBeforeDisplay:1;            // no_output_of_prior_pics_flag
        bool         FrameIsLongTermReference:1;                   // long_term_reference_flag
        bool         AdaptiveRefPicMarkingModeFlag:1;              // adaptive_ref_pic_marking_mode_flag
        bool         IDRFlag:1;                                    // idr_flag
        bool         AdditionalPrefixNALExtensionFlag:1;           // additional_prefix_nal_unit_extension_flag
        bool         AdditionalPrefixNALExtensionDataFlag:1;       // additional_prefix_nal_unit_extension_data_flag
        bool         NoInterLayerPredictionFlag:1;                 // no_inter_layer_pred_flag
        bool         UseReferenceBasePictureFlag:1;                // use_ref_base_pic_flag
        bool         OutputFlag:1;                                 // output_flag
        bool         DepthFlag:1;                                  // depth_flag
        bool         NonIDRFlag:1;                                 // non_idr_flag
        bool         IsAnchorPicture:1;                            // anchor_pic_flag
        bool         InterViewFlag:1;                              // inter_view_flag
        uint8_t      NALRefIDC:2;                                  // nal_ref_idc
        uint8_t      NALUnitType:5;                                // nal_unit_type
        uint8_t      MemManControlOp;                              // memory_management_control_operation
        uint8_t      PicNumDiff;                                   // difference_of_pic_nums_minus1
        uint8_t      LongTermFrameIndex;                           // long_term_frame_idx
        uint8_t      MaxLongTermFrameIndex;                        // max_long_term_frame_idx_plus1
        uint8_t      ViewIndex;                                    // view_idx; what the hell is a view index?
        uint64_t     NALUnitSize;                                  // NumBytesInNALunit
        uint8_t     *PriorityID;                                   // priority_id
        uint8_t     *DependencyID;                                 // dependency_id
        uint8_t     *TemporalID;                                   // temporal_id
        bool        *IsDisposable;                                 // discardable_flag
        uint8_t    **QualityID;                                    // quality_id
    } NALHeader;

    typedef struct MacroBlock {
        bool         DirectType:1;                                 // mb_direct_type_flag
        bool         ALCFlag:1;                                    // mb_alc_flag
        bool         TransfomSizeIs16x16:1;                        // transform_size_16x16_flag
        bool         TransformSizeIs8x8:1;                         // transform_size_8x8_flag
        bool         NoMBSmallerThan8x8Flag:1;                     // noSubMbPartSizeLessThan8x8Flag
        bool         BaseModeFlag:1;                               // base_mode_flag
        uint8_t      Type;                                         // mb_type
        uint8_t      BlockPattern;                                 // coded_block_pattern
        uint8_t      BlockPatternChroma;                           // CodedBlockPatternChroma
        uint8_t      BlockPatternLuma;                             // CodedBlockPatternLuma
        uint8_t      QPDelta;                                      // mb_qp_delta
                                                                   // 256 is a guess
        uint8_t      IntraChromaPredictionMode;                    // intra_chroma_pred_mode
        uint8_t      NumRefIndexActiveLevel0;                      // num_ref_idx_l0_active_minus1
        uint8_t      NumRefIndexActiveLevel1;                      // num_ref_idx_l1_active_minus1
        uint8_t     *RemIntra4x4PredictionMode;                    // rem_intra4x4_pred_mode
        uint8_t     *RemIntra8x8PredictionMode;                    // rem_intra8x8_pred_mode
        uint8_t     *RefIndexLevel0;                               // ref_idx_l0
        uint8_t     *RefIndexLevel1;                               // ref_idx_l1
        uint8_t     *SubMacroBlockType;                            // sub_mb_type
        bool        *PreviousIntra4x4PredictionFlag;               // prev_intra4x4_pred_mode_flag
        bool        *PreviousIntra8x8PredictionFlag;               // prev_intra8x8_pred_mode_flag
        bool        *MotionPredictionFlagLevel0;                   // motion_prediction_flag_l0
        bool        *MotionPredictionFlagLevel1;                   // motion_prediction_flag_l1
        uint16_t    *PCMLumaSample;                                // pcm_sample_luma
        uint16_t   **PCMChromaSample;                              // pcm_sample_chroma
        bool       **BVSPFlag;                                     // bvsp_flag_l0 bvsp_flag_l1
        uint8_t   ***MVDLevel0;                                    // mvd_l0
        uint8_t   ***MVDLevel1;                                    // mvd_l1
    } MacroBlock;

    typedef struct HypotheticalReferenceDecoder { // hrd_parameters
        bool         VclHrdBpPresentFlag:1;                        // VclHrdBpPresentFlag
        uint8_t      NumCodedPictureBuffers;                       // cpb_cnt_minus1
        uint8_t      BitRateScale;                                 // bit_rate_scale
        uint8_t      CodedPictureBufferScale;                      // cpb_size_scale
        uint8_t      CBPDelay;                                     // cpb_removal_delay_length_minus1
        uint8_t      DBPDelay;                                     // dpb_output_delay_length_minus1
        uint8_t      TimeOffsetSize;                               // time_offset_length
        uint32_t     InitialCPBDelayLength;                        // initial_cpb_removal_delay_length_minus1
        uint32_t    *BitRate;                                      // bit_rate_value_minus1
        uint32_t    *CodedPictureSize;                             // cpb_size_value_minus1
        bool        *IsConstantBitRate;                            // cbr_flag
    } HypotheticalReferenceDecoder;

    typedef struct ScalableVideoCoding {
        bool         InterLayerDeblockingFilterPresent:1;          // inter_layer_deblocking_filter_control_present_flag
        bool         ChromaPhaseXFlag:1;                           // chroma_phase_x_plus1_flag
        bool         SeqRefLayerChromaPhaseX:1;                    // seq_ref_layer_chroma_phase_x_plus1_flag
        bool         SequenceCoeffLevelPresent:1;                  // seq_tcoeff_level_prediction_flag
        bool         AdaptiveCoeffsPresent:1;                      // adaptive_tcoeff_level_prediction_flag
        bool         SliceHeaderRestricted:1;                      // slice_header_restriction_flag
        bool         AdaptiveMarkingModeFlag:1;                    // adaptive_ref_base_pic_marking_mode_flag
        uint8_t      ExtendedSpatialScalabilityIDC;                // extended_spatial_scalability_idc
        uint8_t      ChromaPhaseY;                                 // chroma_phase_y_plus1
        uint8_t      SeqRefLayerChromaPhaseY;                      // seq_ref_layer_chroma_phase_y_plus1
        uint8_t      RefLayerLeftOffset;                           // seq_scaled_ref_layer_left_offset
        uint8_t      RefLayerTopOffset;                            // seq_scaled_ref_layer_top_offset
        uint8_t      RefLayerRightOffset;                          // seq_scaled_ref_layer_right_offset
        uint8_t      RefLayerBottomOffset;                         // seq_scaled_ref_layer_bottom_offset
        uint32_t     BaseControlOperation;                         // memory_management_base_control_operation
        uint32_t     NumBasePicDiff;                               // difference_of_base_pic_nums_minus1
        uint32_t     LongTermBasePicNum;                           // long_term_base_pic_num
    } ScalableVideoCoding;

    typedef struct VideoUsabilityInformation {
        bool         AspectRatioInfoPresent:1;                     // aspect_ratio_info_present_flag
        bool         OverscanInfoPresent:1;                        // overscan_info_present_flag
        bool         DisplayInOverscan:1;                          // overscan_appropriate_flag
        bool         VideoSignalTypePresent:1;                     // video_signal_type_present_flag
        bool         FullRange:1;                                  // video_full_range_flag
        bool         ColorDescriptionPresent:1;                    // color_description_present_flag
        bool         ChromaLocationPresent:1;                      // chroma_loc_info_present_flag
        bool         LowDelayHRD:1;                                // low_delay_hrd_flag
        bool         BitStreamRestricted:1;                        // bitstream_restriction_flag
        bool         MVCTimingInfoPresent:1;                       // vui_mvc_timing_info_present_flag
        uint8_t      VideoType;                                    // video_format
        uint8_t      ColorPrimaries;                               // color_primaries
        uint8_t      TransferProperties;                           // transfer_characteristics
        uint8_t      MatrixCoefficients;                           // matrix_coefficients
        uint8_t      ChromaSampleLocationTop;                      // chroma_sample_loc_type_top_field
        uint8_t      ChromaSampleLocationBottom;                   // chroma_sample_loc_type_bottom_field
        uint8_t      AspectRatioIDC;                               // aspect_ratio_idc
        uint8_t      VUIMVCDNumOpPoints;                           // vui_mvcd_num_ops_minus1
        uint8_t      VUIExtNumEntries;                             // vui_ext_num_entries_minus1
        uint16_t     MVCNumOpertionPoints;                         // vui_mvc_num_ops_minus1
        uint16_t     SARWidth;                                     // sar_width
        uint16_t     SAWHeight;                                    // sar_height
        uint32_t    *MaxBytesPerPicDenom;                          // max_bytes_per_pic_denom
        uint32_t    *MaxBitsPerMarcoBlockDenom;                    // max_bits_per_mb_denom
        uint32_t    *MaxMotionVectorLength;                        // log2_max_mv_length_horizontal
        uint32_t    *MaxMotionVectorHeight;                        // log2_max_mv_length_vertical
        uint32_t    *MaxReorderFrames;                             // max_num_reorder_frames
        uint32_t    *MaxFrameBuffer;                               // max_dec_frame_buffering
        uint32_t    *MVCUnitsInTick;                               // vui_mvc_num_units_in_tick
        uint32_t    *MVCTimeScale;                                 // vui_mvc_time_scale
        uint32_t    *VUIMVCDNumUnitsInTick;                        // vui_mvcd_num_units_in_tick
        uint32_t    *VUIMVCDTimeScale;                             // vui_mvcd_time_scale
        uint32_t    *VUIExtNumUnitsInTick;                         // vui_ext_num_units_in_tick
        uint32_t    *VUIExtTimeScale;                              // vui_ext_time_scale
        bool        *TimingInfoPresent;                            // timing_info_present_flag
        bool        *FixedFrameRateFlag;                           // fixed_frame_rate_flag
        bool        *NALHrdParamsPresent;                          // nal_hrd_parameters_present_flag
        bool        *VCLHrdParamsPresent;                          // vcl_hrd_parameters_present_flag
        bool        *PicStructPresent;                             // pic_struct_present_flag
        bool        *MotionVectorsOverPicBoundaries;               // motion_vectors_over_pic_boundaries_flag
        bool        *MVCFixedFrameRate;                            // vui_mvc_fixed_frame_rate_flag
        bool        *MVCNALHRDParamsPresent;                       // vui_mvc_nal_hrd_parameters_present_flag
        bool        *MVCVCLHRDParamsPresent;                       // vui_mvc_vcl_hrd_parameters_present_flag
        bool        *MVCLowDelayFlag;                              // vui_mvc_low_delay_hrd_flag
        bool        *VUIMVCDTimingInfoPresent;                     // vui_mvcd_timing_info_present_flag
        bool        *VUIMVCDFixedFrameRateFlag;                    // vui_mvcd_fixed_frame_rate_flag
        bool        *VUIMVCDNalHRDParametersPresent;               // vui_mvcd_nal_hrd_parameters_present_flag
        bool        *VUIMVCDVclHRDParametersPresent;               // vui_mvcd_vcl_hrd_parameters_present_flag
        bool        *VUIMVCDHRDLowDelayPresent;                    // vui_mvcd_low_delay_hrd_flag
        bool        *VUIMVCDPicStructPresent;                      // vui_mvcd_pic_struct_present_flag
        bool        *VUIExtTimingInfoPresentFlag;                  // vui_ext_timing_info_present_flag
        bool        *VUIExtFixedFrameRateFlag;                     // vui_ext_fixed_frame_rate_flag
        bool        *VUIExtNALHRDPresentFlag;                      // vui_ext_nal_hrd_parameters_present_flag
        bool        *VUIExtVCLHRDPresentFlag;                      // vui_ext_vcl_hrd_parameters_present_flag
        bool        *VUIExtLowDelayHRDFlag;                        // vui_ext_low_delay_hrd_flag
        bool        *VUIExtPicStructPresentFlag;                   // vui_ext_pic_struct_present_flag
        bool       **VUIMVCDDepthFlag;                             // vui_mvcd_depth_flag
        bool       **VUIMVCDTextureFlag;                           // vui_mvcd_texture_flag
        uint8_t     *MVCNumTargetViews;                            // vui_mvc_num_target_output_views_minus1
        uint8_t     *VUIMVCDTemporalID;                            // vui_mvcd_temporal_id
        uint8_t     *VUIExtDependencyID;                           // vui_ext_dependency_id
        uint8_t     *VUIExtQualityID;                              // vui_ext_quality_id
        uint8_t     *VUIExtTemporalID;                             // vui_ext_temporal_id
        uint16_t    *VUIMVCDNumTargetOutputViews;                  // vui_mvcd_num_target_output_views_minus1
        uint32_t    *UnitsPerTick;                                 // num_units_in_tick
        uint32_t    *TimeScale;                                    // time_scale
        uint8_t    **VUIMVCDViewID;                                // vui_mvcd_view_id
        uint8_t    **MVCTemporalID;                                // vui_mvc_temporal_id
        uint8_t    **MVCViewID;                                    // vui_mvc_view_id
    } VideoUsabilityInformation;

    typedef struct DepthParameterSet {
        bool         VSPParamFlag:1;                               // vsp_param_flag
        bool         AdditionalExtensionFlag:1;                    // depth_param_additional_extension_data_flag
        uint8_t      DepthParameterSetID;                          // depth_parameter_set_id
        uint8_t      PredictionDirection;                          // pred_direction
        uint8_t      PredictedWeight0;                             // predWeight0
        uint8_t      NumDepthViews;                                // num_depth_views_minus1
        uint8_t      DepthMappingValues;                           // nonlinear_depth_representation_num
        uint8_t     *ReferenceDPSID;                               // ref_dps_id0, ref_dps_id1
        uint8_t     *DepthRepresentationModel;                     // nonlinear_depth_representation_model
    } DepthParameterSet;

    typedef struct SupplementalEnchancementInfo {
        bool         NalHrdBpPresentFlag:1;                        // NalHrdBpPresentFlag
        bool         FullTimestampFlag:1;                          // full_timestamp_flag
        bool         CountDroppedFlag:1;                           // cnt_dropped_flag
        bool         HoursFlag:1;                                  // hours_flag
        bool         MinutesFlag:1;                                // minutes_flag
        bool         SecondsFlag:1;                                // seconds_flag
        bool         DisablePanScanFlag:1;                         // pan_scan_rect_cancel_flag
        bool         ExactMatchFlag:1;                             // exact_match_flag
        bool         BrokenLinkFlag:1;                             // broken_link_flag
        bool         OriginalIDRFlag:1;                            // original_idr_flag
        bool         OriginalFieldPicFlag:1;                       // original_field_pic_flag
        bool         OriginalBottomFieldFlag:1;                    // original_bottom_field_flag
        bool         SpareFieldFlag:1;                             // spare_field_flag
        bool         TargetBottomFieldFlag:1;                      // target_bottom_field_flag
        bool         SceneInfoPresentFlag:1;                       // scene_info_present_flag
        bool         NuitFieldBasedFlag:1;                         // nuit_field_based_flag
        bool         FirstRefPicFlag:1;                            // first_ref_pic_flag
        bool         LeadingNonRefPicFlag:1;                       // leading_non_ref_pic_flag
        bool         LastPicFlag:1;                                // last_pic_flag
        bool         SubSeqFrameNumFlag:1;                         // sub_seq_frame_num_flag
        bool         AccurateStatisticsFlag:1;                     // accurate_statistics_flag
        bool         DurationFlag:1;                               // duration_flag
        bool         AverageRateFlag:1;                            // average_rate_flag
        bool         PanScanRectFlag:1;                            // pan_scan_rect_flag
        bool         FilmGrainCharactisticsCancelFlag:1;           // film_grain_characteristics_cancel_flag
        bool         SeperateColorDescriptionFlag:1;               // separate_color_description_present_flag
        bool         FilmGrainFullRangeFlag:1;                     // film_grain_full_range_flag
        bool         DeblockingDisplayPreferenceCancelFlag:1;      // deblocking_display_preference_cancel_flag
        bool         DisplayBeforeDeblockingFlag:1;                // display_prior_to_deblocking_preferred_flag
        bool         DecodeFrameBufferingConstraintFlag:1;         // dec_frame_buffering_constraint_flag
        bool         FieldViewsFlag:1;                             // field_views_flag
        bool         TopFieldIsLeftViewFlag:1;                     // top_field_is_left_view_flag
        bool         CurrentFrameIsLeftViewFlag:1;                 // current_frame_is_left_view_flag
        bool         NextFrameIsDependentViewFlag:1;               // next_frame_is_second_view_flag
        bool         LeftViewSelfContainedFlag:1;                  // left_view_self_contained_flag
        bool         RightViewSelfContainedFlag:1;                 // right_view_self_contained_flag
        bool         ToneMapCancelFlag:1;                          // tone_map_cancel_flag
        bool         ExposureCompensationSignFlag:1;               // exposure_compensation_value_sign_flag
        bool         AllLayerRepresentationsInAUFlag:1;            // all_layer_representations_in_au_flag
        bool         OperationPointFlag:1;                         // operation_point_flag
        bool         AllViewComponentsInAUFlag:1;                  // all_view_components_in_au_flag
        bool         FramePackingArrangementCancelFlag:1;          // frame_packing_arrangement_cancel_flag
        bool         SpatialFlippingFlag:1;                        // spatial_flipping_flag
        bool         FlippedFlagFrame0:1;                          // frame0_flipped_flag
        bool         TheCurrentFrameIsFrame0:1;                    // current_frame_is_frame0_flag
        bool         Frame0IsSelfContainedFlag:1;                  // frame0_self_contained_flag
        bool         Frame1IsSelfContainedFlag:1;                  // frame1_self_contained_flag
        bool         FramePackingArrangementExtensionFlag:1;       // frame_packing_arrangement_extension_flag
        bool         DisplayOrientationCancelFlag:1;               // display_orientation_cancel_flag
        bool         HorizontalFlip:1;                             // hor_flip
        bool         VerticalFlip:1;                               // ver_flip
        bool         CounterClockwiseRotation:1;                   // anticlockwise_rotation
        bool         DisplayOrientationRepetitionPeriod:1;         // display_orientation_repetition_period
        bool         DisplayOrientationExtensionFlag:1;            // display_orientation_extension_flag
        bool         AllViewsEqual:1;                              // all_views_equal_flag
        bool         ZNearFlag:1;                                  // z_near_flag
        bool         ZFarFlag:1;                                   // z_far_flag
        bool         ZAxisEqualFlag:1;                             // z_axis_equal_flag
        bool         DMinFlag:1;                                   // d_min_flag
        bool         DMaxFlag:1;                                   // d_max_flag
        bool         ReferenceViewingDistanceFlag:1;               // ref_viewing_distance_flag
        bool         ReferenceDisplays3DFlag:1;                    // three_dimensional_reference_displays_extension_flag
        bool         PerViewDepthTimingFlag:1;                     // per_view_depth_timing_flag
        bool         PerViewDepthSamplingGridFlag:1;               // per_view_depth_grid_pos_flag
        bool         DepthGridPositionXSignFlag:1;                 // depth_grid_pos_x_sign_flag
        bool         DepthGridPositionYSignFlag:1;                 // depth_grid_pos_y_sign_flag
        bool         MVCViewPositionExtensionFlag:1;               // multiview_view_position_extension_flag
        bool         IntrinsicParamFlag:1;                         // intrinsic_param_flag
        bool         ExtrinsicParamFlag:1;                         // extrinsic_param_flag
        bool         AnchorUpdateFlag:1;                           // anchor_update_flag
        bool         NonAnchorUpdateFlag:1;                        // non_anchor_update_flag
        bool         SEIOpTextureOnlyFlag:1;                       // sei_op_texture_only_flag
        bool         TemporalIDNestingFlag:1;                      // temporal_id_nesting_flag
        bool         PriorityLayerInfoPresent:1;                   // priority_layer_info_present_flag
        bool         PriorityIDSettingFlag:1;                      // priority_id_setting_flag
        bool         CpbDpbDelaysPresentFlag:1;                    // CpbDpbDelaysPresentFlag
        uint8_t      ChangingSliceGroupIDC:2;                      // changing_slice_group_idc
        uint8_t      SEIOpTemporalID:3;                            // sei_op_temporal_id
        uint8_t      FilmGrainBitDepthLuma:4;                      // film_grain_bit_depth_luma_minus8
        uint8_t      FilmGrainBitDepthChroma:4;                    // film_grain_bit_depth_chroma_minus8
        uint8_t      NumPriorityIDs:4;                             // num_priority_ids
        uint8_t      DepthGridPositionXDP:4;                       // depth_grid_pos_x_dp
        uint8_t      DepthGridPositionYDP:4;                       // depth_grid_pos_y_dp
        uint32_t     DepthGridPositionXFP:20;                      // depth_grid_pos_x_fp
        uint32_t     DepthGridPositionYFP:20;                      // depth_grid_pos_y_fp
        uint32_t     SEISize;                                       // sei_size
        uint32_t     SEIType;                                      // sei_type
        uint8_t      CPBRemovalDelay;                              // cpb_removal_delay
        uint8_t      DPBOutputDelay;                               // dpb_output_delay
        uint8_t      PicStruct;                                    // pic_struct
        uint8_t      CTType;                                       // ct_type
        uint8_t      CountingType;                                 // counting_type
        uint8_t      NFrames;                                      // n_frames
        uint8_t      Hours;                                        // hours_value
        uint8_t      Minutes;                                      // minutes_value
        uint8_t      Seconds;                                      // seconds_value
        uint8_t      TimeOffset;                                   // time_offset
        uint8_t      PanScanID;                                    // pan_scan_rect_id
        uint8_t      PanScanCount;                                 // pan_scan_cnt_minus1
        uint16_t     PanScanRepitionPeriod;                        // pan_scan_rect_repetition_period
        uint16_t     CountryCode;                                  // itu_t_t35_country_code
        uint8_t      RecoveryFrameCount;                           // recovery_frame_cnt
        uint32_t     OriginalFrameNum;                             // original_frame_num
        uint32_t     TargetFrameNum;                               // target_frame_num
        uint32_t     NumSparePics;                                 // num_spare_pics_minus1
        uint8_t      SceneID;                                      // scene_id
        uint8_t      SceneTransitionType;                          // scene_transition_type
        uint32_t     SecondSceneID;                                // second_scene_id
        uint8_t      SubSequenceLayerNum;                          // sub_seq_layer_num
        uint8_t      SubSequenceID;                                // sub_seq_id
        uint8_t      SubSeqFrameNum;                               // sub_seq_frame_num
        uint8_t      NumSubSeqLayers;                              // num_sub_seq_layers_minus1
        uint16_t     AverageBitRate;                               // average_bit_rate
        uint16_t     AverageFrameRate;                             // average_frame_rate
        uint32_t     SubSeqDuration;                               // sub_seq_duration
        uint8_t      NumReferencedSubSeqs;                         // num_referenced_subseqs
        uint8_t      RefSubSeqLayerNum;                            // ref_sub_seq_layer_num
        uint8_t      RefSubSeqID;                                  // ref_sub_seq_id
        uint8_t      RefSubSeqDirection;                           // ref_sub_seq_direction
        uint64_t     FullFrameFreezeRepitionPeriod;                // full_frame_freeze_repetition_period
        uint8_t      SnapshotID;                                   // snapshot_id
        uint8_t      ProgressiveRefinementID;                      // progressive_refinement_id
        uint8_t      NumRefinementSteps;                           // num_refinement_steps_minus1
        uint8_t      NumSliceGroupsInSet;                          // num_slice_groups_in_set_minus1
        uint8_t      FilmGrainModelID;                             // film_grain_model_id
        uint8_t      FilmGrainColorPrimaries;                      // film_grain_color_primaries
        uint8_t      FilmGrainTransferCharacteristics;             // film_grain_transfer_characteristics
        uint8_t      FilmGrainMatrixCoefficents;                   // film_grain_matrix_coefficients
        uint8_t      BlendingModeID;                               // blending_mode_id
        uint8_t      Scalefactor;                                  // log2_scale_factor
        uint8_t      FilmGrainCharacteristicsRepetitionPeriod;     // film_grain_characteristics_repetition_period
        uint8_t      DeblockingDisplayPreferenceRepetitionPeriod;  // deblocking_display_preference_repetition_period
        uint8_t      FilterHintSizeY;                              // filter_hint_size_y
        uint8_t      FilterHintSizeX;                              // filter_hint_size_x
        uint8_t      FilterHintType;                               // filter_hint_type
        uint8_t      ToneMapID;                                    // tone_map_id
        uint8_t      ToneMapRepetitionPeriod;                      // tone_map_repetition_period
        uint8_t      CodedDataBitDepth;                            // coded_data_bit_depth
        uint8_t      TargetBitDepth;                               // target_bit_depth
        uint8_t      ToneMapModelID;                               // tone_map_model_id
        uint32_t     ToneMinValue;                                 // min_value
        uint32_t     ToneMaxValue;                                 // max_value
        uint32_t     SigmoidMidpoint;                              // sigmoid_midpoint
        uint32_t     SigmoidWidth;                                 // sigmoid_width
        uint8_t      NumPivots;                                    // num_pivots
        uint8_t      CameraISOSpeedIDC;                            // camera_iso_speed_idc
        uint8_t      CameraISOSpeed;                               // camera_iso_speed_value
        uint8_t      ExposureIndexIDC;                             // exposure_index_idc
        uint8_t      ExposureIndexValue;                           // exposure_index_value
        uint16_t     ExposureCompensationNumerator;                // exposure_compensation_value_numerator
        uint16_t     ExposureCompensationDenominatorIDC;           // exposure_compensation_value_denom_idc
        uint32_t     RefScreenLuminanceWhite;                      // ref_screen_luminance_white
        uint32_t     ExtendedRangeWhiteLevel;                      // extended_range_white_level
        uint16_t     NominalBlackLumaCode;                         // nominal_black_level_luma_code_value
        uint16_t     NominalWhiteLumaCode;                         // nominal_white_level_luma_code_value
        uint16_t     ExtendedWhiteLumaCode;                        // extended_white_level_luma_code_value
        uint32_t     NumInfoEntries;                               // num_info_entries_minus1
        uint8_t      NumLayersNotPresent;                          // num_layers
        uint8_t      NumLayers;                                    // num_layers_minus1
        uint8_t      NumLayerRepresentations;                      // num_layer_representations_minus1
        uint8_t      NumTemporalLayersInBaseLayer;                 // num_of_temporal_layers_in_base_layer_minus1
        uint8_t      NumDependencyIDs;                             // num_dIds_minus1
        uint8_t      TemporalDependencyRepresentationIndexLevel0;  // tl0_dep_rep_idx
        uint16_t     EffectiveIDRPicID;                            // effective_idr_pic_id
        uint8_t      DeltaFrameNum;                                // delta_frame_num
        uint8_t      NumViewComponents;                            // num_view_components_minus1
        uint8_t      NumViewComponentsOp;                          // num_view_components_op_minus1
        uint8_t      NumOperationPoints;                           // num_operation_points_minus1
        uint8_t      FramePackingArrangementID;                    // frame_packing_arrangement_id
        uint8_t      FramePackingArrangementType;                  // frame_packing_arrangement_type
        uint8_t      QuincunxSamplingFlag;                         // quincunx_sampling_flag
        uint8_t      ContentIntrepretationType;                    // content_interpretation_type
        uint8_t      Frame0GridPositionX;                          // frame0_grid_position_x // FIXME: Combine these
        uint8_t      Frame0GridPositionY;                          // frame0_grid_position_y
        uint8_t      Frame1GridPositionX;                          // frame1_grid_position_x
        uint8_t      Frame1GridPositionY;                          // frame1_grid_position_y
        uint8_t      FramePackingArrangementReservedByte;          // frame_packing_arrangement_reserved_byte
        uint8_t      FramePackingArrangementRepetitionPeriod;      // frame_packing_arrangement_repetition_period
        uint16_t     WhitePointX;                                  // white_point_x
        uint16_t     WhitePointY;                                  // white_point_y
        uint32_t     MaxDisplayMasteringLuminance;                 // max_display_mastering_luminance
        uint32_t     MinDisplayMasteringLuminance;                 // min_display_mastering_luminance
        uint8_t      CommonZAxisReferenceView;                     // common_z_axis_reference_view
        uint8_t      DepthRepresentationType;                      // depth_representation_type
        uint8_t      DepthNonlinearRepresentation;                 // depth_nonlinear_representation_num_minus1
        uint8_t      TruncationErrorExponent;                      // prec_ref_baseline
        uint8_t      TruncatedWidthExponent;                       // prec_ref_display_width
        uint8_t      TruncatedReferenveViewingDistanceExponent;    // prec_ref_viewing_dist
        uint8_t      NumReferenceDisplays;                         // num_ref_displays_minus1
        uint16_t     DepthSampleWidthMul;                          // dttsr_x_mul
        uint8_t      DepthSampleWidthDP;                           // dttsr_x_dp
        uint16_t     DepthSampleHeightMul;                         // dttsr_y_mul
        uint8_t      DepthSampleHeightDP;                          // dttsr_y_dp
        uint8_t      NumDepthGridViews;                            // num_video_plus_depth_views_minus1
        uint8_t      MaxDPSID;                                     // max_dps_id
        uint8_t      MaxDPSIDDiff;                                 // max_dps_id_diff
        uint8_t      NumTemporalLayersInBaseView;                  // num_of_temporal_layers_in_base_view_minus1
        uint8_t      MaxDisparity;                                 // max_disparity
        uint8_t      IntrinsicParamsEqual;                         // intrinsic_params_equal
        uint8_t      PrecFocalLength;                              // prec_focal_length
        uint8_t      PrecPrincipalPoint;                           // prec_principal_point
        uint8_t      PrecSkewFactor;                               // prec_skew_factor
        uint8_t      NumParamSets;                                 // num_of_param_sets
        uint8_t      PrecRotationParam;                            // prec_rotation_param
        uint8_t      PrecTranslationParam;                         // prec_translation_param
        uint8_t      NumDependencyLayersForPriorityLayer;          // pr_num_dIds_minus1
        uint8_t      PriorityIDURIIndex;                           // PriorityIdSettingUriIdx
        uint8_t      PriorityIDSettingURI;                         // priority_id_setting_uri aka UTF8 url
        uint16_t    *SEIViewID;                                    // sei_view_id
        bool        *SEIViewApplicabilityFlag;                     // sei_view_applicability_flag
        uint16_t    *SEIOpViewID;                                  // sei_op_view_id
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
        uint8_t     *ViewDependencyInfoSrcOpID;                    // view_dependency_info_src_op_id
        uint16_t    *DisplayPrimariesX;                            // display_primaries_x
        uint16_t    *DisplayPrimariesY;                            // display_primaries_y
        uint8_t     *DepthInfoViewID;                              // depth_info_view_id
        uint8_t     *ZAxisReferenceView;                           // z_axis_reference_view
        uint8_t     *DisparityReferenceView;                       // disparity_reference_view
        uint8_t     *DepthNonlinearRepresentationModel;            // depth_nonlinear_representation_model
        uint8_t     *ReferenceBaselineExponent;                    // exponent_ref_baseline
        uint8_t     *ReferenceBaselineMantissa;                    // mantissa_ref_baseline
        uint8_t     *ReferenceDisplayWidthExponent;                // exponent_ref_display_width
        uint8_t     *ReferenceDisplayWidthMantissa;                // mantissa_ref_display_width
        uint8_t     *ReferenceViewingDistanceExponent;             // exponent_ref_viewing_distance
        uint8_t     *ReferenceViewingDistanceMantissa;             // mantissa_ref_viewing_distance
        bool        *ShiftPresentFlag;                             // additional_shift_present_flag
        int16_t     *SampleShift;                                  // num_sample_shift_plus512
        uint8_t     *OffsetLength;                                 // offset_len_minus1
        uint8_t     *DepthDisplayDelayOffsetFP;                    // depth_disp_delay_offset_fp
        uint8_t     *DepthDisplayDelayOffsetDP;                    // depth_disp_delay_offset_dp
        uint8_t     *DepthGridViewID;                              // depth_grid_view_id
        uint8_t     *ViewPosition;                                 // view_position
        uint8_t     *SEIMVCTemporalID;                             // sei_mvc_temporal_id
        uint8_t     *SignSkewFactor;                               // sign_skew_factor
        uint8_t     *ExponentSkewFactor;                           // exponent_skew_factor
        uint8_t     *MantissaSkewFactor;                           // mantissa_skew_factor
        uint8_t     *ViewOrderIndex;                               // view_order_index
        uint8_t     *NumNonRequiredViewComponents;                 // num_non_required_view_components_minus1
        uint8_t     *OperationPointNotPresentID;                   // operation_point_not_present_id
        bool        *SEIOpDepthFlag;                               // sei_op_depth_flag
        bool        *SEIOpTextureFlag;                             // sei_op_texture_flag
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
        uint8_t     *NumSequenceParameterSets;                     // num_seq_parameter_sets
        uint8_t     *NumSubsetSPS;                                 // num_subset_seq_parameter_sets
        uint8_t     *NumPicParameterSets;                          // num_pic_parameter_sets_minus1
        uint8_t     *PPSInfoSrcLayerIDDelta;                       // parameter_sets_info_src_layer_id_delta
        uint8_t     *ConversionTypeIDC;                            // conversion_type_idc
        uint8_t     *NumPriorityLayers;                            // pr_num_minus1
        uint8_t     *PriorityIDSettingURIString;                   // string to contain that
        uint8_t     *LayerID;                                      // layer_id
        uint8_t     *InitialCPBRemovalDelay;                       // initial_cpb_removal_delay
        uint8_t     *InitialCPBRemovalDelayOffset;                 // initial_cpb_removal_delay_offset
        bool        *ClockTimestampFlag;                           // clock_timestamp_flag
        int8_t      *PanScanOffsetLeft;                            // pan_scan_rect_left_offset
        int8_t      *PanScanOffsetRight;                           // pan_scan_rect_right_offset
        int8_t      *PanScanOffsetTop;                             // pan_scan_rect_top_offset
        int8_t      *PanScanOffsetBottom;                          // pan_scan_rect_bottom_offset
        uint32_t    *DeltaSpareFrameNum;                           // delta_spare_frame_num
        bool        *SpareBottomFieldFlag;                         // spare_bottom_field_flag
        uint8_t     *SpareAreaIDC;                                 // spare_area_idc
        bool        *ExactSampleValueMatchFlag;                    // exact_sample_value_match_flag
        bool        *CompModelPresent;                             // comp_model_present_flag
        uint16_t    *NumIntensityIntervals;                        // num_intensity_intervals_minus1
        uint8_t     *NumModelValues;                               // num_model_values_minus1
        uint8_t     *StartOfCodedInterval;                         // start_of_coded_interval
        uint8_t     *CodedPivotValue;                              // coded_pivot_value
        uint16_t    *TargetPivotValue;                             // target_pivot_value
        uint8_t     *EntryDependencyID;                            // entry_dependency_id
        uint8_t     *NumNonRequiredLayerReps;                      // num_non_required_layer_reps_minus1
        uint8_t     *PriorityDependencyID;                         // pr_dependency_id
        uint8_t     *AltPriorityIDs;                               // alt_priority_id
        bool        *LayerDependencyInfoPresent;                   // layer_dependency_info_present_flag
        uint8_t     *NumDependentLayers;                           // num_directly_dependent_layers
        uint8_t     *LayerDepInfoSourceLayerIDDelta;               // layer_dependency_info_src_layer_id_delta_minus1
        uint8_t     *SEIDependencyID;                              // sei_dependency_id
        uint8_t     *SEIQualityID;                                 // sei_quality_id
        uint8_t     *SEITemporalID;                                // sei_temporal_id
        bool        *SEITimingInfoPresent;                         // sei_timing_info_present_flag
        uint32_t    *SEIUnitsInTick;                               // sei_num_units_in_tick
        uint32_t    *SEITimeScale;                                 // sei_time_scale
        bool        *SEIFixedFrameRateFlag;                        // sei_fixed_frame_rate_flag
        bool        *SEINALHRDParamsPresentFlag;                   // sei_nal_hrd_parameters_present_flag
        bool        *SEIVCLHRDParamsPresentFlag;                   // sei_vcl_hrd_parameters_present_flag
        bool        *SEILowDelayHRDFlag;                           // sei_low_delay_hrd_flag
        bool        *SEIPicStructPresentFlag;                      // sei_pic_struct_present_flag
        uint16_t    *SEIQualityLayerCRC;                           // quality_layer_crc
        uint8_t     *NumQualityIDs;                                // num_qIds_minus1
        char        *UnregisteredUserDataUUID;
        uint8_t    **DirectlyDependentViewID;                      // directly_dependent_view_id
        uint8_t    **SignFocalLength;                              // sign_focal_length_x, sign_focal_length_y (x=0,y=1)
        uint16_t   **ExponentFocalLength;                          // exponent_focal_length_x, exponent_focal_length_y
        uint16_t   **MantissaFocalLength;                          // mantissa_focal_length_x, mantissa_focal_length_y
        uint8_t    **SignPrincipalPoint;                           // sign_principal_point_x, sign_principal_point_y
        uint8_t    **ExponentPrincipalPoint;                       // exponent_principal_point_x, exponent_principal_point_y
        uint8_t    **MantissaPrincipalPoint;                       // mantissa_principal_point_x, mantissa_principal_point_y
        uint8_t    **SignT;                                        // sign_t
        uint8_t    **ExponentT;                                    // exponent_t
        uint8_t    **MantissaT;                                    // mantissa_t
        uint8_t    **IndexDelta;                                   // index_delta_minus1
        uint8_t    **FirstMacroBlockInROI;                         // first_mb_in_roi
        uint8_t    **ROIWidthInMacroBlock;                         // roi_width_in_mbs_minus1
        uint8_t    **ROIHeightInMacroBlock;                        // roi_height_in_mbs_minus1
        uint8_t    **SPSIDDelta;                                   // seq_parameter_set_id_delta
        uint8_t    **SubsetSPSIDDelta;                             // subset_seq_parameter_set_id_delta
        uint8_t    **PicParameterSetIDDelta;                       // pic_parameter_set_id_delta
        bool       **RewritingInfoFlag;                            // rewriting_info_flag
        uint32_t   **RewritingProfileLevelIDC;                     // rewriting_profile_level_idc
        uint16_t   **RewritingAverageBitrate;                      // rewriting_avg_bitrate
        uint16_t   **RewritingMaxBitrate;                          // rewriting_max_bitrate
        uint8_t    **PriorityLayerID;                              // pr_id
        uint32_t   **PriorityLevelIDC;                             // pr_profile_level_idc
        uint8_t    **PriorityLevelAvgBitrate;                      // pr_avg_bitrate
        uint8_t    **PriorityLevelMaxBitrate;                      // pr_max_bitrate
        bool       **SpareUnitFlag;                                // spare_unit_flag
        uint8_t    **ZeroRunLength;                                // zero_run_length
        uint8_t    **IntensityIntervalLowerBound;                  // intensity_interval_lower_bound
        uint8_t    **IntensityIntervalUpperBound;                  // intensity_interval_upper_bound
        uint8_t    **NonRequiredLayerRepDependencyID;              // non_required_layer_rep_dependency_id
        uint8_t    **NonRequiredLayerRepQualityID;                 // non_required_layer_rep_quality_id
        uint8_t    **DirectlyDependentLayerIDDelta;                // directly_dependent_layer_id_delta_minus1
        uint8_t    **NumRedundantPics;                             // num_redundant_pics_minus1
        uint8_t   ***SignR;                                        // sign_r
        uint8_t   ***ExponentR;                                    // exponent_r
        uint8_t   ***MantissaR;                                    // mantissa_r
        bool      ***AnchorRefFlag;                                // anchor_ref_l0_flag
        bool      ***NonAnchorRefFlag;                             // non_anchor_ref_l0_flag
        uint8_t   ***CompModelValue;                               // comp_model_value
        uint8_t   ***FilterHint;                                   // filter_hint
        uint8_t   ***RedundantPicCount;                            // redundant_pic_cnt_minus1
        bool      ***RedundantPicsMatch;                           // pic_match_flag
        bool      ***MBTypeMatchFlag;                              // mb_type_match_flag
        bool      ***MotionMatchFlag;                              // motion_match_flag
        bool      ***ResidualMatchFlag;                            // residual_match_flag
        bool      ***IntraSamplesMatchFlag;                        // intra_samples_match_flag
        uint8_t   ***PDIInitDelayAnchor;                           // pdi_init_delay_anchor_minus2_l0, pdi_init_delay_anchor_minus2_l1
        uint8_t   ***PDIInitDelayNonAnchor;                        // pdi_init_delay_non_anchor_minus2_l0, pdi_init_delay_non_anchor_minus2_l1
    } SupplementalEnhancementInfo;

    typedef struct AVCOptions {
        bool                          EncodedInterlaced;
        bool                          EncodedLosslessly;
        DepthParameterSet            *DPS;
        HypotheticalReferenceDecoder *HRD;
        MacroBlock                   *MacroBlock;
        NALHeader                    *NAL;
        PictureParameterSet          *PPS;
        ScalableVideoCoding          *SVC;
        SequenceParameterSet         *SPS;
        Slice                        *Slice;
        SupplementalEnhancementInfo  *SEI;
        uint8_t                       ProfileMajor;
        uint8_t                       ProfileMinor;
        uint8_t                       SubsampleType;
        uint64_t                   ***DisparityOffset;
        uint64_t                   ***DisparityScale;
        uint64_t                      Bitrate;
        uint64_t                      FrameRateDenominator;
        uint64_t                      FrameRateNumerator;
        VideoUsabilityInformation    *VUI;
    } AVCOptions;

    static const int8_t ScalingList4x4[16] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
    };

    static const int8_t ScalingList8x8[64] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
    };

    static const int8_t xDZL[20] = { // Table 8-11
        0, 1, 0, 1, -2, -1, 0, 1, 2, 3, -2, -1, 0, 1, 2, 3, 0, 1, 0, 1
    };

    static const int8_t yDZL[20] = { // Table 8-11
        -2, -2, -1, -1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 2, 3, 3
    };

    static const int8_t xFracL[16] = { // Table 8-12
        0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3
    };

    static const int8_t yFracL[16] = { // Table 8-12; there's a 3rd table but it's confusing named predPartLXL
        0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3
    };

    static const uint8_t ZigZagScan4x4[16] = { // Table 8-13, zig-zag
        0, 1, 4, 8, 5, 2, 3, 6, 9, 12, 13, 10, 7, 11, 14, 15
    };

    static const uint8_t FieldScan4x4[16] = { // Table 8-13, field
        0, 4, 1, 8, 12, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15
    };

    static const uint8_t ZigZagScan8x8[64] = { // Table 8-14, zig-zag
        0,  1,  8, 16,  9,  2,  3, 10, 17, 24, 32, 25, 18, 11,  4,  5,
        12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13,  6,  7, 14, 21, 28,
        35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
        58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63
    };

    static const uint8_t FieldScan8x8[64] = { // Table 8-14, field
        0,  8, 16,  1,  9, 24, 32, 17,  2, 25, 40, 48, 56, 33, 10,  3,
        18, 41, 49, 57, 26, 11,  4, 19, 34, 42, 50, 58, 27, 12,  5, 20,
        35, 43, 51, 59, 28, 21,  6, 21, 36, 44, 52, 60, 29, 14, 22, 37,
        45, 53, 61, 30,  7, 15, 38, 46, 54, 62, 23, 31, 39, 47, 55, 63
    };

    static const uint16_t ISOTable[31] = {
        0,   10,   12,   16,   20,   25,   32,   40,
        50,   64,   80,  100,  125,  160,  200,  250,
        320,  400,  500,  640,  800, 1000, 1250, 1600,
        2000, 2500, 3200, 4000, 5000, 6400, 8000
    };

    static const uint8_t qCodIRangeIdx[4][64] = { // Table 9-44, qCodIRangeIdx
        {
        128, 128, 128, 123, 116, 111, 105, 100,
        95,  90,  85,  81,  77,  73,  69,  66,
        62,  59,  56,  53,  51,  48,  46,  43,
        41,  39,  37,  35,  33,  32,  30,  29,
        27,  26,  24,  23,  22,  21,  20,  19,
        18,  17,  16,  15,  14,  14,  13,  12,
        12,  11,  11,  10,  10,   9,   9,   8,
        8,   7,   7,   7,   6,   6,   6,   2
        }, // 0
        {
        176, 167, 158, 150, 142, 135, 128, 122,
        116, 110, 104,  99,  94,  89,  85,  80,
        76,  72,  69,  65,  62,  59,  56,  53,
        50,  48,  45,  43,  41,  39,  37,  35,
        33,  33,  31,  30,  28,  27,  26,  24,
        23,  22,  20,  19,  18,  17,  16,  15,
        14,  14,  13,  12,  12,  11,  11,  10,
        9,   9,   9,   8,   8,   7,   7,   2,
        }, // 1
        {
        208, 197, 187, 178, 169, 160, 152, 144,
        137, 130, 123, 117, 111, 105, 100,  95,
        90,  86,  81,  77,  73,  69,  66,  63,
        59,  56,  54,  51,  48,  46,  43,  41,
        39,  37,  35,  33,  32,  30,  29,  27,
        26,  25,  23,  22,  21,  20,  19,  18,
        17,  16,  15,  15,  14,  13,  12,  12,
        11,  11,  10,  10,   9,   9,   8,   2
        }, // 2
        {
        240, 227, 216, 205, 195, 185, 175, 166,
        158, 150, 142, 135, 128, 122, 116, 110,
        104,  99,  94,  89,  85,  80,  76,  72,
        69,  65,  62,  59,  56,  53,  50,  48,
        45,  43,  41,  39,  37,  35,  33,  31,
        30,  28,  27,  25,  24,  23,  22,  21,
        20,  19,  18,  17,  16,  15,  14,  14,
        13,  12,  12,  11,  11,  10,   9,   2,
        }, // 3
    };

    static const uint8_t transIdxLPS[64] = { // Table 9-45, transIdxLPS
        0,  0,  1,  2,  2,  4,  4,  5,  6,  7,  8,  9,  9, 11, 11, 12,
        13, 13, 15, 15, 16, 16, 18, 18, 19, 19, 21, 21, 22, 22, 23, 24,
        24, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 30, 30, 31, 32, 32,
        33, 33, 33, 34, 34, 35, 35, 35, 36, 36, 36, 37, 37, 38, 38, 63
    };

    static const uint8_t transIdxMPS[64] = { // Table 9-45, transIdxMPS
        1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16,
        17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
        33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
        49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 62, 63
    };

    static const uint8_t V4x4[6][3] = { // 8-315
        {10, 16, 13},
        {11, 18, 14},
        {13, 20, 16},
        {14, 23, 18},
        {16, 25, 20},
        {18, 29, 23}
    };

    static const uint8_t V8x8[6][6] = {
        {20, 18, 32, 19, 25, 24},
        {22, 19, 35, 21, 28, 26},
        {26, 23, 42, 24, 33, 31},
        {28, 25, 45, 26, 35, 33},
        {32, 28, 51, 30, 40, 38},
        {36, 32, 58, 34, 46, 43}
    };

    static const uint8_t Luma4x4ZigZagScan[16] = { // Table 6-10, TODO: this needs to be redone
        0, 1, 4, 8, 5, 2, 3, 6, 9, 12, 13, 10, 7, 11, 14, 15
    };

    static const uint8_t CBPCAT12Intra[48] = { // Table 9-4, coded_block_pattern, ChromaArrayType = 1 or 2
        47, 31, 15,  0, 23, 27, 29, 30,
        7, 11, 13, 14, 39, 43, 45, 46,
        16,  3,  5, 10, 12, 19, 21, 26,
        28, 35, 37, 42, 44,  1,  2,  4,
        8, 17, 18, 20, 24,  6,  9, 22,
        25, 32, 33, 34, 36, 40, 38, 41,
    };

    static const uint8_t CBPCAT12Inter[48] = { // Table 9-4, coded_block_pattern, ChromaArrayType = 1 or 2
        0, 16,  1,  2,  4,  8, 32,  3,
        5, 10, 12, 15, 47,  7, 11, 13,
        14,  6,  9, 31, 35, 37, 42, 44,
        33, 34, 36, 40, 39, 43, 45, 46,
        17, 18, 20, 24, 19, 21, 26, 28,
        23, 27, 29, 30, 22, 25, 38, 41
    };

    static const uint8_t CBPCAT03Intra[16] = { // Table 9-4, coded_block_pattern, ChromaArrayType = 0 or 3
        15,  0, 7, 11, 13, 14, 3, 5,
        10, 12, 1,  2,  4,  8, 6, 9
    };

    static const uint8_t CBPCAT03Inter[16] = { // Table 9-4, coded_block_pattern, ChromaArrayType = 0 or 3
        0,  1,  2,  4,  8,  3, 5, 10,
        12, 15, 7, 11, 13, 14, 6,  9
    };

    AVCOptions *AVCOptions_Init(void);

    bool AreAllViewsPaired(AVCOptions *Options);

    void AVCOptions_Deinit(AVCOptions *Options);

#ifdef OVIA_CodecIO_AVC
    extern const CodecIO_ImageChannelConfig AVCChannelConfig;

    const CodecIO_ImageChannelConfig AVCChannelConfig = {
        .NumChannels = 5,
        .Channels    = {
            [0]      = ImageMask_2D | ImageMask_Luma,
            [1]      = ImageMask_2D | ImageMask_Luma | ImageMask_Chroma1 | ImageMask_Chroma2,
            [2]      = ImageMask_2D | ImageMask_Luma | ImageMask_Chroma1 | ImageMask_Chroma2 | ImageMask_Alpha,
            [3]      = ImageMask_3D | ImageMask_Luma | ImageMask_Chroma1 | ImageMask_Chroma2,
            [4]      = ImageMask_3D | ImageMask_Luma | ImageMask_Chroma1 | ImageMask_Chroma2 | ImageMask_Alpha,
        },
    };

    extern const CodecIO_ImageLimitations AVCLimits;

    const CodecIO_ImageLimitations AVCLimits = {
        .MaxHeight      = 4320,
        .MaxWidth       = 7680,
        .MaxBitDepth    = 16,
        .ChannelConfigs = &AVCChannelConfig,
    };

    extern const OVIA_MIMETypes AVCMIMETypes;

    const OVIA_MIMETypes AVCMIMETypes = {
        .NumMIMETypes     = 4,
        .MIMETypes        = {
            [0]           = {
                .Size     = 9,
                .MIMEType = UTF32String("video/avc"),
            },
            [1]           = {
                .Size     = 9,
                .MIMEType = UTF32String("video/mvc"),
            },
            [2]           = {
                .Size     = 9,
                .MIMEType = UTF32String("video/svc"),
            },
            [3]           = {
                .Size     = 10,
                .MIMEType = UTF32String("video/h264"),
            },
        },
    };

    extern const OVIA_Extensions AVCExtensions;

    const OVIA_Extensions AVCExtensions = {
        .NumExtensions     = 4,
        .Extensions        = {
            [0]            = {
                .Size      = 4,
                .Extension = UTF32String("h264"),
            },
            [1]            = {
                .Size      = 4,
                .Extension = UTF32String("avc"),
            },
            [2]            = {
                .Size      = 3,
                .Extension = UTF32String("mvc"),
            },
            [3]            = {
                .Size      = 3,
                .Extension = UTF32String("svc"),
            },
        },
    };

    extern const OVIA_MagicIDs AVCMagicIDs;

    const OVIA_MagicIDs AVCMagicIDs = {
        .NumMagicIDs          = 2,
        .MagicIDs             = {
            [0]               = {
                .OffsetInBits = 0,
                .SizeInBits   = 24,
                .Signature    = (uint8_t[3]) {0x00, 0x00, 0x01},
            },
            [1]               = {
                .OffsetInBits = 0,
                .SizeInBits   = 32,
                .Signature    = (uint8_t[4]) {0x00, 0x00, 0x00, 0x01},
            },
        },
    };

#if defined(OVIA_CodecIO_Encode)
    extern const CodecIO_Encoder AVCOptions;

    const CodecIO_Encoder AVCOptions = {
        .Function_Initalize   = AVCOptions_Init,
        .Function_Parse       = AVC_Parse,
        .Function_Media       = AVC_Extract,
        .Function_Deinitalize = AVCOptions_Deinit,
    };
#endif /* OVIA_CodecIO_Encode */

#if defined(OVIA_CodecIO_Decode)
    extern const CodecIO_Decoder AVCOptions;

    const CodecIO_Decoder AVCOptions = {
        .Function_Initalize   = AVCOptions_Init,
        .Function_Parse       = AVC_Parse,
        .Function_Media       = AVC_Compose,
        .Function_Deinitalize = AVCOptions_Deinit,
    };
#endif /* OVIA_CodecIO_Decode */

#endif /* AVC Literals */
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_CodecIO_AVCCodec_h */
