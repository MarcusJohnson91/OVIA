#include "OVIACommon.h"

#pragma once

#ifndef OVIA_FLACCommon_H
#define OVIA_FLACCommon_H

#ifdef __cplusplus
extern "C" {
#endif

    typedef enum FLACConstants {
        FLACMagic                                                   = 0x664c6143, // fLaC
        FrameMagic                                                  =     0x3FFE,
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
    } FLACConstants;

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

    typedef enum FLACSpeakerLayout {
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
    } FLACSpeakerLayout;

    typedef enum FLACPredictionTypes {
        Verbatim                                                    =          0,
        Constant                                                    =          1,
        FixedLinear                                                 =          2,
        FIRLinear                                                   =          3,
    } FLACPredictionTypes;

    typedef enum FLACResidualTypes {
        Simple                                                      =          0,
        Complex                                                     =          1,
    } FLACResidualTypes;

    typedef enum BlockTypes {
        Block_StreamInfo                                            =          0, // 0x30 = 0 in ascii, 0x31 = 1, etc.
        Block_Padding                                                     =          1,
        Block_Custom                                                      =          2, // Application aka App specific
        Block_SeekTable                                                   =          3,
        Block_Vorbis                                               =          4,
        Block_Cuesheet                                                    =          5,
        Block_Picture                                                     =          6,
    } BlockTypes;

    typedef enum SubFrameTypes {
        Subframe_Constant                                           =          0,
        Subframe_Verbatim                                           =          1,
        Subframe_Fixed                                              =          8,
        Subframe_LPC                                                =         63,
    } SubFrameTypes;

    typedef enum FLACBlockType {
        BlockType_Fixed                                              =          0,
        BlockType_Variable                                           =          1,
    } FLACBlockType;

    typedef enum FLACRicePartitionType {
        RICE1                                                       =          0,
        RICE2                                                       =          1,
    } FLACRicePartitionType;
    
    typedef struct StreamInfo {
        uint64_t  SamplesInStream:36; // 36 bits, all the samples in the stream; channel agnostic
        uint32_t MinimumFrameSize:24;
        uint32_t MaximumFrameSize:24;
        uint16_t MinimumBlockSize:16; // in samples; must be at least 16
        uint16_t MaximumBlockSize:16; // If these match than it's fixed blocksize format
        uint8_t      CodedSampleRate; // 4 bits
        uint8_t      CodedBitDepth:6; // 5 bits, 4-32 bits per sample
        uint8_t      CodedChannels:4; // 3 bits, add 1 to get the real value
        uint8_t                 *MD5; // MD5
    } StreamInfo;
    
    typedef struct CueSheetTrack {
        UTF8      *ISRC;                // the tracks's ISRC number as a string.
        uint64_t  *TrackOffset;         // samples from the beginning of the FLAC file to the first sample of the track
        uint8_t   *TrackNum;            // which track is this again?
        bool      *IsAudio;             // 1 for audio, 0 for data
        bool      *PreEmphasis;         // 1 for true 0 for false.
    } CueSheetTrack;
    
    typedef struct FLACCueSheet {
        UTF8          *ISRC;
        uint64_t      *Offset;
        uint8_t       *Num;
        bool          *IsAudio;
        bool          *PreEmphasis;
        uint8_t        NumTrackIndexPoints;
        UTF8          *CatalogID;
        uint64_t       LeadIn;              // in samples
        uint64_t       IndexOffset;         // false idea. must be multiple of CD sector for cd sources
        uint64_t       CatalogIDSize;
        uint8_t        NumTracks;           // the number of tracks, CD has a max of 100 (including lead out)
        bool           IsCD;                // 1 if it came from a CD; 0 otherwise
        uint8_t        IndexPointNum;
    } FLACCueSheet;
    
    typedef struct SeekTable {
        uint32_t      NumSeekPoints;
        uint64_t     *SampleInTargetFrame; // FIXME: Sample in the whole file, or sample in a specific frame?
        uint64_t     *OffsetFrom1stSample; // in bytes
        uint16_t     *TargetFrameSize;
    } SeekTable;
    
    typedef struct Picture {
        uint32_t     *PictureStart; // Pointer to the start of the picture
        uint8_t      *MIMEString;
        uint8_t      *PicDescriptionString;
        uint32_t      PicDescriptionSize;
        uint32_t      Width;
        uint32_t      Height;
        uint32_t      MIMESize; // size of the MIME string in bytes
        uint32_t      BitDepth;
        uint32_t      ColorsUsed; // 0 for not paletted
        uint32_t      PictureSize;
        FLACPicTypes  PicType;
    } Picture;
    
    typedef struct SubFrame {
        uint8_t  SubFrameType;
        uint8_t  LPCFilterOrder;
        uint8_t  LPCPrecision;
        uint8_t  LPCShift;
        uint64_t Coeff;
        uint8_t  WastedBits:6; // Uses unary coding
        bool     WastedBitsFlag:1;
    } SubFrame;
    
    typedef struct Frame {
        SubFrame     *Sub;
        uint64_t      SampleNumber:36;
        uint32_t      SampleRate;
        uint32_t      FrameNumber:31;
        uint16_t      BlockSize; // SamplesInBlock
        uint8_t       BitDepth;
        uint8_t       PartitionOrder;
        uint8_t       CurrentPartition;
        uint8_t       SamplesInPartition;
        uint8_t       CodedBlockSize;
        uint8_t       CodedSampleRate:5;
        uint8_t       ChannelLayout:4;
        uint8_t       CodedBitDepth:4;
        bool          BlockType:1;
    } Frame;
    
    typedef struct RICEPartition {
        uint8_t      *RICEParameter;
        uint8_t       EscapeBitDepth:5;
        uint8_t       NumRICEPartitions:4;
    } RICEPartition;
    
    typedef struct FLACLPC {
        int8_t       *LPCCoeff;
        uint8_t       LPCOrder;
        uint8_t       NumLPCCoeffs;
        uint8_t       LPCShift:5;
        uint8_t       LPCPrecision:4;
        uint8_t       PartitionOrder:4;
        uint8_t       RicePartitionType:2;
    } FLACLPC;
    
    typedef struct FLACOptions {
        Picture        **Pictures;
        StreamInfo      *StreamInfo;
        FLACCueSheet    *CueSheet;
        SeekTable       *SeekPoints;
        Frame           *Frame;
        FLACLPC         *LPC;
        RICEPartition   *Rice;
        uint32_t         NumPictures;
        bool             EncodeSubset;
    } FLACOptions;
    
    /* OVIA specific functions */
    
    void *FLACOptions_Init(void);
    
    void        FLAC_Frame_Read(FLACOptions *FLAC, BitBuffer *BitB, Audio2DContainer *Audio);
    
    uint8_t FLAC_GetNumChannels(FLACOptions *FLAC);
    
    void        FLAC_SubFrame_Read(FLACOptions *FLAC, BitBuffer *BitB, Audio2DContainer *Audio, uint8_t Channel);
    
    void        FLAC_SubFrame_Verbatim(FLACOptions *FLAC, BitBuffer *BitB, Audio2DContainer *Audio);
    
    void        FLAC_SubFrame_Constant(FLACOptions *FLAC, BitBuffer *BitB, Audio2DContainer *Audio);
    
    void        FLAC_SubFrame_Fixed(FLACOptions *FLAC, BitBuffer *BitB, Audio2DContainer *Audio);
    
    void        FLAC_SubFrame_LPC(FLACOptions *FLAC, BitBuffer *BitB, Audio2DContainer *Audio, uint8_t Channel);
    
    void FLAC_Decode_RICE(FLACOptions *FLAC, BitBuffer *BitB, uint8_t RICEPartitionType);
    
    uint16_t    FLAC_GetBlockSizeInSamples(FLACOptions *FLAC);
    
    bool        FLAC_Parse_Blocks(FLACOptions *FLAC, BitBuffer *BitB);
    
    void        FLAC_Parse_StreamInfo(FLACOptions *FLAC, BitBuffer *BitB);
    
    void        FLAC_Parse_SeekTable(FLACOptions *FLAC, BitBuffer *BitB, uint32_t ChunkSize);
    
    void        FLAC_Parse_Vorbis(FLACOptions *FLAC, BitBuffer *BitB);
    
    void        FLAC_CUE_Parse(FLACOptions *FLAC, BitBuffer *BitB);
    
    uint8_t    *FLAC_Pic_Read(FLACOptions *FLAC, BitBuffer *BitB);
    
    void        FLACOptions_Deinit(FLACOptions *FLAC);
    
#ifdef __cplusplus
}
#endif

#endif /* OVIA_FLACCommon_H */
