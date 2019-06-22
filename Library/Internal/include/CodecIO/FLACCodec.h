#include "../../../Dependencies/FoundationIO/libFoundationIO/include/BitIO.h"
#include "../../../Dependencies/FoundationIO/libFoundationIO/include/ContainerIO.h"
#include "../../../Dependencies/FoundationIO/libFoundationIO/include/CryptographyIO.h"
#include "../../../Dependencies/FoundationIO/libFoundationIO/include/Log.h"
#include "../../../Dependencies/FoundationIO/libFoundationIO/include/Math.h"

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
    
    typedef struct FLACStreamInfo {
        uint64_t  SamplesInStream:36; // 36 bits, all the samples in the stream; channel agnostic
        uint32_t MinimumFrameSize:24;
        uint32_t MaximumFrameSize:24;
        uint16_t MinimumBlockSize:16; // in samples; must be at least 16
        uint16_t MaximumBlockSize:16; // If these match than it's fixed blocksize format
        uint8_t      CodedSampleRate; // 4 bits
        uint8_t      CodedBitDepth:6; // 5 bits, 4-32 bits per sample
        uint8_t      CodedChannels:4; // 3 bits, add 1 to get the real value
        uint8_t                 *MD5; // MD5
    } FLACStreamInfo;
    
    typedef struct CueSheetTrack {
        UTF8      *ISRC;                // the tracks's ISRC number as a string.
        uint64_t  *TrackOffset;         // samples from the beginning of the FLAC file to the first sample of the track
        uint8_t   *TrackNum;            // which track is this again?
        bool      *IsAudio;             // 1 for audio, 0 for data
        bool      *PreEmphasis;         // 1 for true 0 for false.
    } CueSheetTrack;
    
    typedef struct FLACCueSheet {
        CueSheetTrack *Tracks;
        uint8_t        NumTrackIndexPoints;
        char          *CatalogID;
        uint64_t       LeadIn;              // in samples
        uint64_t       IndexOffset;         // false idea. must be multiple of CD sector for cd sources
        size_t         CatalogIDSize;
        uint8_t        NumTracks;           // the number of tracks, CD has a max of 100 (including lead out)
        bool           IsCD;                // 1 if it came from a CD; 0 otherwise
        uint8_t        IndexPointNum;
    } FLACCueSheet;
    
    typedef struct FLACSeekTable {
        uint32_t      NumSeekPoints;
        uint64_t     *SampleInTargetFrame; // FIXME: Sample in the whole file, or sample in a specific frame?
        uint64_t     *OffsetFrom1stSample; // in bytes
        uint16_t     *TargetFrameSize;
    } FLACSeekTable;
    
    typedef struct FLACPicture {
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
    } FLACPicture;
    
    typedef struct FLACSubFrame {
        uint8_t SubFrameType;
        uint8_t LPCFilterOrder;
        uint8_t WastedBits:6; // Uses unary coding
        bool    WastedBitsFlag:1;
    } FLACSubFrame;
    
    typedef struct FLACFrame {
        FLACSubFrame *Sub;
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
    } FLACFrame;
    
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
        FLACPicture    **Pictures;
        FLACStreamInfo  *StreamInfo;
        FLACCueSheet    *CueSheet;
        FLACSeekTable   *SeekPoints;
        FLACFrame       *Frame;
        FLACLPC         *LPC;
        RICEPartition   *Rice;
        uint32_t         NumPictures;
        bool             EncodeSubset;
    } FLACOptions;
    
    /* OVIA specific functions */
    
    void        OVIA_FLAC_Frame_Read(BitBuffer *BitB);
    
    void        OVIA_FLAC_SubFrame_Read(BitBuffer *BitB, uint8_t Channel);
    
    void        OVIA_FLAC_SubFrame_Verbatim(BitBuffer *BitB);
    
    void        OVIA_FLAC_SubFrame_Constant(BitBuffer *BitB);
    
    void        OVIA_FLAC_SubFrame_Fixed(BitBuffer *BitB);
    
    void        OVIA_FLAC_SubFrame_LPC(BitBuffer *BitB, uint8_t Channel);
    
    void        OVIA_FLAC_Decode_Residual(BitBuffer *BitB);
    
    void        OVIA_FLAC_Decode_RICE1(BitBuffer *BitB);
    
    void        OVIA_FLAC_Decode_RICE2(BitBuffer *BitB);
    
    uint8_t     GetBlockSizeInSamples(uint8_t BlockSize);
    
    void        OVIA_FLAC_Stream_Read(BitBuffer *BitB);
    
    bool        OVIA_FLAC_Parse_Blocks(BitBuffer *BitB);
    
    void        OVIA_FLAC_StreamInfo_Parse(BitBuffer *BitB);
    
    void        OVIA_FLAC_SkipPadding(BitBuffer *BitB, uint32_t ChunkSize);
    
    void        OVIA_FLAC_SkipCustom(BitBuffer *BitB, uint32_t ChunkSize);
    
    void        OVIA_FLAC_SeekTable_Read(BitBuffer *BitB, uint32_t ChunkSize);
    
    void        OVIA_FLAC_Vorbis_Parse(BitBuffer *BitB);
    
    void        OVIA_FLAC_CUE_Parse(BitBuffer *BitB);
    
    uint8_t    *OVIA_FLAC_Pic_Read(BitBuffer *BitB);
    
    bool                 OVIA_FLAC_CUE_GetIsCD(OVIA *Ovia);
    bool                 OVIA_FLAC_Frame_GetBlockType(OVIA *Ovia);
    bool                 OVIA_FLAC_GetEncodeSubset(OVIA *Ovia);
    uint8_t             *OVIA_FLAC_GetMD5(OVIA *Ovia);
    uint8_t              OVIA_FLAC_Frame_GetChannelLayout(OVIA *Ovia);
    uint8_t              OVIA_FLAC_Frame_GetCodedBitDepth(OVIA *Ovia);
    uint8_t              OVIA_FLAC_Frame_GetCodedBlockSize(OVIA *Ovia);
    uint8_t              OVIA_FLAC_Frame_GetCodedSampleRate(OVIA *Ovia);
    uint8_t              OVIA_FLAC_LPC_GetLPCCoeff(uint8_t CoeffNum);
    uint8_t              OVIA_FLAC_LPC_GetLPCOrder(OVIA *Ovia);
    uint8_t              OVIA_FLAC_LPC_GetLPCPrecision(OVIA *Ovia);
    uint8_t              OVIA_FLAC_LPC_GetLPCShift(OVIA *Ovia);
    uint8_t              OVIA_FLAC_LPC_GetNumLPCCoeffs(OVIA *Ovia);
    uint8_t              OVIA_FLAC_LPC_GetRICEParameter(uint8_t Partition);
    uint8_t              OVIA_FLAC_LPC_GetRICEPartitionOrder(OVIA *Ovia);
    uint8_t              OVIA_FLAC_LPC_GetRICEPartitionType(OVIA *Ovia);
    uint64_t             OVIA_FLAC_Frame_GetFrameNumber(OVIA *Ovia);
    uint64_t             OVIA_FLAC_Frame_GetSampleNumber(OVIA *Ovia);
    void                 OVIA_FLAC_CUE_SetCatalogID(char *CatalogID);
    void                 OVIA_FLAC_CUE_SetIsCD(bool IsCD);
    void                 OVIA_FLAC_CUE_SetLeadIn(uint64_t LeadIn);
    void                 OVIA_FLAC_CUE_SetNumTracks(uint8_t NumTracks);
    void                 OVIA_FLAC_CUE_SetTrack(uint64_t Offset, bool IsAudio, bool PreEmphasis, UTF8 *ISRC);
    void                 OVIA_FLAC_CUE_Track_SetOffset(uint8_t Track, uint64_t TrackOffset);
    void                 OVIA_FLAC_Frame_SetBlockSize(uint16_t BlockSize);
    void                 OVIA_FLAC_Frame_SetBlockType(bool BlockTypeIsFixed);
    void                 OVIA_FLAC_Frame_SetChannelLayout(uint8_t ChannelLayout);
    void                 OVIA_FLAC_Frame_SetCodedBitDepth(uint8_t CodedBitDepth);
    void                 OVIA_FLAC_Frame_SetCodedBlockSize(uint8_t CodedBlockSize);
    void                 OVIA_FLAC_Frame_SetCodedSampleRate(uint8_t CodedSampleRate);
    void                 OVIA_FLAC_Frame_SetFrameNumber(uint64_t FrameNumber);
    void                 OVIA_FLAC_Frame_SetSampleNumber(uint64_t SampleNumber);
    void                 OVIA_FLAC_Frame_SetSampleRate(uint32_t SampleRate);
    void                 OVIA_FLAC_LPC_SetLPCCoeff(uint8_t CoeffNum, uint8_t Coeff);
    void                 OVIA_FLAC_LPC_SetLPCOrder(uint8_t LPCOrder);
    void                 OVIA_FLAC_LPC_SetLPCPrecision(uint8_t LPCPrecision);
    void                 OVIA_FLAC_LPC_SetLPCShift(uint8_t LPCShift);
    void                 OVIA_FLAC_LPC_SetNumLPCCoeffs(uint8_t NumLPCCoeffs);
    void                 OVIA_FLAC_LPC_SetRICEParameter(uint8_t Partition, uint8_t Parameter);
    void                 OVIA_FLAC_LPC_SetRICEPartitionOrder(uint8_t RICEPartitionOrder);
    void                 OVIA_FLAC_LPC_SetRICEPartitionType(uint8_t RICEPartitionType);
    void                 OVIA_FLAC_Seek_SetSeekPoint(uint32_t SeekPoint, uint64_t Sample, uint64_t Offset, uint16_t FrameSize);
    void                 OVIA_FLAC_SetEncodeSubset(bool EncodeSubset);
    void                 OVIA_FLAC_SetMaxBlockSize(uint16_t MaxBlockSize);
    void                 OVIA_FLAC_SetMaxFilterOrder(uint16_t MaxFilterOrder);
    void                 OVIA_FLAC_SetMaxFrameSize(uint16_t MaxFrameSize);
    void                 OVIA_FLAC_SetMaxRicePartitionOrder(uint8_t MaxRICEPartitionOrder);
    void                 OVIA_FLAC_SetMD5(uint8_t *MD5);
    void                 OVIA_FLAC_SetMinBlockSize(uint16_t MinBlockSize);
    void                 OVIA_FLAC_SetMinFrameSize(uint16_t MinFrameSize);
    void                 OVIA_FLAC_SubFrame_SetType(uint8_t SubframeType);
    void                 OVIA_FLAC_SubFrame_SetWastedBits(bool WastedBitsFlag, uint8_t NumWastedBits);
    
#ifdef __cplusplus
}
#endif

#endif /* OVIA_FLACCommon_H */
