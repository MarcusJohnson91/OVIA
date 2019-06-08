#include "../../OVIA.h"
#include "../InternalOVIA.h"

#pragma once

#ifndef OVIA_FLACCommon_H
#define OVIA_FLACCommon_H

#ifdef __cplusplus
extern "C" {
#endif

    enum FLACConstants {
        FLACMagic                                                   = 0x664c6143, // fLaC
        FLACFrameMagic                                              =     0x3FFE,
        FLACMaxChannels                                             =          8,
        FLACMaxSamplesInBlock                                       =      65535,
        FLACMaxChannelsInBlock                                      =          1,
        FLACMaxBitDepth                                             =         32,
        FLACMaxSampleRate                                           =     655350,
        FLACMaxMIMEString                                           =         32,
        FLACMaxPicDescriptionString                                 =        128,
        FLACMaxPicBuffer                                            =    6750000, // 1500x1500 max
        FLACMD5Size                                                 =         16,
        FLACMaxCueTracks                                            =        100,
        FLACMaxVorbisComments                                       =       1024,
        FLACMaxVorbisCommentSize                                    =       1024,
        FLACMaxSeekPoints                                           =        255, // 100 points in a 5 hour movie at 5 minutes.
        FLACVorbisMaxComments                                       =        100,
        FLACISRCSize                                                =         12,
        FLACMaxLPCCoefficents                                       =         32,
        FLACMediaCatalogNumberSize                                  =        128,
        FLACMaxCoeffs                                               =        882,
        FLACMaxRicePartitions                                       =         16,
    };

    typedef enum FLACPicTypes {
        Other                                                       =          0,
        FileIcon                                                    =          1, // 32x32, PNG only
        OtherFileIcon                                               =          2,
        FrontCover                                                  =          3, // main album art
        BackCover                                                   =          4,
        LeafletPage                                                 =          5,
        LabelSide                                                   =          6, // no clue
        LeadPerformer                                               =          7,
        Performer                                                   =          8,
        Conducter                                                   =          9,
        BandOrOrchestra                                             =         10,
        Composer                                                    =         11,
        Writer                                                      =         12,
        RecordingLocation                                           =         13,
        DuringRecording                                             =         14,
        DuringPerformance                                           =         15,
        Screenshot                                                  =         16,
        XiphLogo                                                    =         17,
        Illustration                                                =         18,
        BandLogo                                                    =         19,
        PublisherLogo                                               =         20,
    } FLACPicTypes;

    enum FLACSpeakerLayout {
        Mono                                                        =          0,
        Left_Right                                                  =          1,
        Left_Right_Center                                           =          2,
        Left_Right_BackLeft_BackRight                               =          3,
        Left_Right_Center_BackLeft_BackRight                        =          4,
        Left_Right_Center_LFE_BackLeft_BackRight                    =          5,
        Left_Right_Center_LFE_BackCenter_SideLeft_SideRight         =          6,
        Left_Right_Center_LFE_BackLeft_BackRight_SideLeft_SideRight =          7,
        Left_Diff                                                   =          8,
        Right_Diff                                                  =          9,
        Average_Diff                                                =         10,
    };

    enum FLACPredictionTypes {
        Verbatim                                                    =          0,
        Constant                                                    =          1,
        FixedLinear                                                 =          2,
        FIRLinear                                                   =          3,
    };

    enum FLACResidualTypes {
        Simple                                                      =          0,
        Complex                                                     =          1,
    };

    enum FLACMetadataTypes {
        StreamInfo                                                  =          0, // 0x30 = 0 in ascii, 0x31 = 1, etc.
        Padding                                                     =          1,
        Custom                                                      =          2, // Application aka App specific
        SeekTable                                                   =          3,
        VorbisComment                                               =          4,
        Cuesheet                                                    =          5,
        Picture                                                     =          6,
    };

    enum FLACSubFrameTypes {
        Subframe_Constant                                           =          0,
        Subframe_Verbatim                                           =          1,
        Subframe_Fixed                                              =          8,
        Subframe_LPC                                                =         63,
    };

    enum FLACBlockType {
        FixedBlockSize                                              =          0,
        VariableBlockSize                                           =          1,
    };

    enum FLACRicePartitionType {
        RICE1                                                       =          0,
        RICE2                                                       =          1,
    };
    
    /* OVIA specific functions */
    
    void        OVIA_FLAC_Frame_Read(OVIA *Ovia, BitBuffer *BitB);
    
    void        OVIA_FLAC_SubFrame_Read(OVIA *Ovia, BitBuffer *BitB, uint8_t Channel);
    
    void        OVIA_FLAC_SubFrame_Verbatim(OVIA *Ovia, BitBuffer *BitB);
    
    void        OVIA_FLAC_SubFrame_Constant(OVIA *Ovia, BitBuffer *BitB);
    
    void        OVIA_FLAC_SubFrame_Fixed(OVIA *Ovia, BitBuffer *BitB);
    
    void        OVIA_FLAC_SubFrame_LPC(OVIA *Ovia, BitBuffer *BitB, uint8_t Channel);
    
    void        OVIA_FLAC_Decode_Residual(OVIA *Ovia, BitBuffer *BitB);
    
    void        OVIA_FLAC_Decode_RICE1(OVIA *Ovia, BitBuffer *BitB);
    
    void        OVIA_FLAC_Decode_RICE2(OVIA *Ovia, BitBuffer *BitB);
    
    uint8_t     GetBlockSizeInSamples(uint8_t BlockSize);
    
    void        OVIA_FLAC_Stream_Read(OVIA *Ovia, BitBuffer *BitB);
    
    bool        OVIA_FLAC_Parse_Blocks(OVIA *Ovia, BitBuffer *BitB);
    
    void        OVIA_FLAC_StreamInfo_Parse(OVIA *Ovia, BitBuffer *BitB);
    
    void        OVIA_FLAC_SkipPadding(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize);
    
    void        OVIA_FLAC_SkipCustom(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize);
    
    void        OVIA_FLAC_SeekTable_Read(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize);
    
    void        OVIA_FLAC_Vorbis_Parse(OVIA *Ovia, BitBuffer *BitB);
    
    void        OVIA_FLAC_CUE_Parse(OVIA *Ovia, BitBuffer *BitB);
    
    uint8_t    *OVIA_FLAC_Pic_Read(OVIA *Ovia, BitBuffer *BitB);
    
#ifdef __cplusplus
}
#endif

#endif /* OVIA_FLACCommon_H */
