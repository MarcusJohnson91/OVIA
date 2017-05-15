/*!
 @header    libModernAVC.h
 @author    Marcus Johnson aka BumbleBritches57
 @copyright 2016, Marcus Johnson
 @version   0.2.3
 */

#pragma once

#ifndef LIBMODERNAVC_LIBMODERNAVC_H
#define LIBMODERNAVC_LIBMODERNAVC_H

#ifdef __cplusplus
extern "C" {
#endif
    
    enum AVCConstants {
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
    };
    
    enum EntropyCoder {
        ExpGolomb                                   =  0,
        Arithmetic                                  =  1,
    };
    
    enum ChromaFormats {
        ChromaBW                                    =  0, // Black and white, When Profile 183, Chroma is assumed to be this, default is Chroma420
        Chroma420                                   =  1,
        Chroma422                                   =  2,
        Chroma444                                   =  3,
    };
    
    enum SampleAspectRatios {
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
    };
    
    enum RefinedSliceGroupMapType {
        BoxOutClockwise                             = 0, // Box-out clockwise
        BoxOutCounterClockwise                      = 1, // Box-out counter-clockwise
        RasterScan                                  = 2, // Raster scan
        ReverseRasterScan                           = 4, // Reverse raster scan
        WipeRight                                   = 5, // Wipe right
        WipeLeft                                    = 6, // Wipe left
    };
    
    enum SliceType { // A group of macroblocks, a slice group contains all the macroblocks in a frame
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
    };
    
    enum FilmGrainType { // blending_mode_id
        AdditiveGrain                               =  0,
        MultiplicativeGrain                         =  1,
    };
    
    enum VideoType {
        Component                                   =  0,
        PAL                                         =  1,
        NTSC                                        =  2,
        SECAM                                       =  3,
        MAC                                         =  4,
        Unspecified                                 =  5,
        Reserved1                                   =  6,
        Reserved2                                   =  7,
    };
    
    enum ChromaIntraPredictionMode { // intra_chroma_pred_mode
        ChromaIntra_DC                              =  0,
        ChromaIntra_Horizontal                      =  1,
        ChromaIntra_Vertical                        =  2,
        ChromaIntra_Plane                           =  3,
    };
    
    enum IntraPrediction4x4Mode { // Intra4x4PredMode[luma4x4BlkIdx]
        Intra4x4Vertical                            =  0, // Intra_4x4_Vertical
        Intra4x4Horizontal                          =  1, // Intra_4x4_Horizontal
        Intra4x4DC                                  =  2, // Intra_4x4_DC
        Intra4x4DiagonalDownLeft                    =  3, // Intra_4x4_Diagonal_Down_Left
        Intra4x4DiagonalDownRight                   =  4, // Intra_4x4_Diagonal_Down_Right
        Intra4x4VerticalRight                       =  5, // Intra_4x4_Vertical_Right
        Intra4x4HorizontalDown                      =  6, // Intra_4x4_Horizontal_Down
        Intra4x4VerticalLeft                        =  7, // Intra_4x4_Vertical_Left
        Intra4x4HorizontalUp                        =  8, // Intra_4x4_Horizontal_Up
    };
    
    enum PMacroBlockSubTypes { // sub_mb_type[mbPartIdx]
        P_L0_8x8                                    =  0, // P_L0_8x8
        P_L0_8x4                                    =  1, // P_L0_8x4
        P_L0_4x8                                    =  2, // P_L0_4x8
        P_L0_4x4                                    =  3, // P_L0_4x4
    };
    
    enum BMacroBlockSubTypes {   // sub_mb_type
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
    };
    
    enum BSliceMacroBlockTypes {
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
    };
    
    enum PSliceMacroBlockTypes { // Covers bot P and SP types. Table 7-13
        P_L0_16x16                                  =  0,
        P_L0_L0_16x8                                =  1,
        P_L0_L0_8x16                                =  2,
        P_8x8                                       =  3,
        P_8x8ref0                                   =  4,
        P_Skip                                      =  5,
    };
    
    enum ISliceMacroBlockTypes { // Table 7-11; everything elses is I_PCM
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
    };
    
    enum MacroBlockPredictionMode { // TODO: Fix me
        Intra_16x16                                 = 0,
        Intra_8x8                                   = 0,
        Intra_4x4                                   = 0,
        Inter_16x16                                 = 0,
        Inter_8x8                                   = 0,
        Inter_4x4                                   = 0,
    };
    
    enum PrimaryPictureType {
        wat                                         = 0,
    };
    
    enum NonPartitionedMacroBlockPredictionMode {
        Direct                                      = 0,
        Pred_L0                                     = 1,
        Pred_L1                                     = 2,
        BiPred                                      = 3,
    };
    
    enum SubMacroBlockTypes {
        B_SubMacroBlock                             = 0,
        P_SubMacroBlock                             = 1,
    };
    
    enum NALTypes {
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
    };
    
    enum SEITypes {
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
    };
    
    enum ProfileTypes {
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
    };
    
    typedef struct AVCFile AVCFile;
    
#ifdef __cplusplus
}
#endif

#endif /* LIBMODERNAVC_LIBMODERNAVC_H */
