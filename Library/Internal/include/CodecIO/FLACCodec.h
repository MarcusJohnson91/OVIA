/*!
 @header              FLACCodec.h
 @author              Marcus Johnson
 @copyright           2017+
 @SPDX-License-Identifier: Apache-2.0
 @version             1.0.0
 @brief               This header contains code for FLAC (encoding and decoding).
 */

#pragma once

#ifndef OVIA_CodecIO_FLACCodec_H
#define OVIA_CodecIO_FLACCodec_H

#include "../../../OVIA/include/CodecIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
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

    typedef enum FLAC_BlockTypes {
        BlockType_StreamInfo                                            =          0, // 0x30 = 0 in ascii, 0x31 = 1, etc.
        BlockType_Padding                                               =          1,
        BlockType_Custom                                                =          2, // Application aka App specific
        BlockType_SeekTable                                             =          3,
        BlockType_Vorbis                                                =          4,
        BlockType_Cuesheet                                              =          5,
        BlockType_Picture                                               =          6,
    } FLAC_BlockTypes;

    typedef enum SubFrameTypes {
        Subframe_Constant                                           =          0,
        Subframe_Verbatim                                           =          2,
        Subframe_Fixed                                              =         24,
        Subframe_LPC                                                =         63,
    } SubFrameTypes;

    typedef enum FLACBlockType {
        BlockType_Fixed                                              =          0,
        BlockType_Variable                                           =          1,
    } FLACBlockType;

    typedef enum FLACRicePartitionType { // FLACResidualCodingType
        CodingType_4Bit      = 0,
        CodingType_5Bit      = 1,
        CodingType_Reserved1 = 2,
        CodingType_Reserved2 = 3,
        /*
         Residual coding method:
         00 : partitioned Rice coding with 4-bit Rice parameter; RESIDUAL_CODING_METHOD_PARTITIONED_RICE follows
         01 : partitioned Rice coding with 5-bit Rice parameter; RESIDUAL_CODING_METHOD_PARTITIONED_RICE2 follows
         10-11 : reserved
         */
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
        UTF8         **ISRC;
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
        uint8_t      *PicData;
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
        int8_t        LPCCoeff[32]; // Max 32 Coeffs
        uint8_t       NumLPCCoeffs;
        uint8_t       QLevel; // Quantization Level aka Shift
        uint8_t       LPCOrder;
        uint8_t       LPCShift:5;
        uint8_t       LPCPrecision:4;
        uint8_t       PartitionOrder:4;
        uint8_t       RicePartitionType:2;
    } FLACLPC;

    typedef struct FLACOptions {
        Picture         *Pictures;
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

    FLACOptions *FLACOptions_Init(void);

    void        FLAC_Compose(FLACOptions *Options, BitBuffer *BitB);

    void        FLAC_Write_Audio(FLACOptions *Options, BitBuffer *BitB, AudioScape2D *Audio);

    void        FLAC_Write_StreamInfo(FLACOptions *Options, BitBuffer *BitB);

    void        FLAC_Frame_Read(FLACOptions *Options, BitBuffer *BitB, AudioScape2D *Audio);

    uint8_t     FLAC_GetNumChannels(FLACOptions *Options);

    void        FLAC_SubFrame_Read(FLACOptions *Options, BitBuffer *BitB, AudioScape2D *Audio, uint8_t Channel);

    void        FLAC_SubFrame_Verbatim(FLACOptions *Options, BitBuffer *BitB, AudioScape2D *Audio);

    void        FLAC_SubFrame_Constant(FLACOptions *Options, BitBuffer *BitB, AudioScape2D *Audio);

    void        FLAC_SubFrame_Fixed(FLACOptions *Options, BitBuffer *BitB, AudioScape2D *Audio);

    void        FLAC_SubFrame_LPC(FLACOptions *Options, BitBuffer *BitB, AudioScape2D *Audio, uint8_t Channel);

    void        FLAC_Decode_RICE(FLACOptions *Options, BitBuffer *BitB, uint8_t RICEPartitionType);

    uint16_t    FLAC_GetBlockSizeInSamples(FLACOptions *Options);

    bool        FLAC_Parse_Blocks(FLACOptions *Options, BitBuffer *BitB);

    void        FLAC_Parse_StreamInfo(FLACOptions *Options, BitBuffer *BitB);

    void        FLAC_Parse_SeekTable(FLACOptions *Options, BitBuffer *BitB, uint32_t ChunkSize);

    void        FLAC_Parse_Vorbis(FLACOptions *Options, BitBuffer *BitB);

    void        FLAC_CUE_Parse(FLACOptions *Options, BitBuffer *BitB);

    void        FLAC_Pic_Read(FLACOptions *Options, BitBuffer *BitB);

    void        FLACOptions_Deinit(FLACOptions *Options);

#ifdef OVIA_CodecIO_FLAC
    extern const CodecIO_AudioLimitations FLACLimits;
    
    const CodecIO_AudioLimitations FLACLimits = {
        .MaxSampleRate     = 655350,
        .MaxBitDepth       = 32,
        .MaxNumChannels    = 8,
        .SupportedChannels = AudioMask_FrontLeft | AudioMask_FrontRight | AudioMask_FrontCenter | AudioMask_LFE | AudioMask_SurroundLeft | AudioMask_SurroundRight | AudioMask_SurroundCenter | AudioMask_RearLeft | AudioMask_RearRight,
    };

    extern const OVIA_MIMETypes FLACMIMETypes;
    
    const OVIA_MIMETypes FLACMIMETypes = {
        .NumMIMETypes     = 1,
        .MIMETypes        = {
            OVIA_RegisterMIMEType("audio/flac")
        }
    };
    
    extern const OVIA_Extensions FLACExtensions;

    const OVIA_Extensions FLACExtensions = {
        .NumExtensions     = 1,
        .Extensions        = {
            OVIA_RegisterExtension("flac")
        }
    };

    extern const OVIA_MagicIDs FLACMagicIDs;
    
    const OVIA_MagicIDs FLACMagicIDs = {
        .NumMagicIDs   = 1,
        .MagicIDs      = {
            OVIA_RegisterSignature(0, 32, (uint8_t[4]) {0x66, 0x4C, 0x61, 0x43})
        },
    };
    
#if defined(OVIA_CodecIO_Encode)
    extern const CodecIO_Encoder FLACEncoder;
    
    const CodecIO_Encoder FLACEncoder = {
        .Function_Initalize   = FLACOptions_Init,
        .Function_Parse       = FLAC_Compose,
        .Function_Media       = FLAC_Extract,
        .Function_Deinitalize = FLACOptions_Deinit,
    };
#endif /* OVIA_CodecIO_Encode */
    
#if defined(OVIA_CodecIO_Decode)
    extern const CodecIO_Decoder FLACDecoder;
    
    const CodecIO_Decoder FLACDecoder = {
        .Function_Initalize   = FLACOptions_Init,
        .Function_Parse       = FLAC_Parse,
        .Function_Media       = FLAC_Extract,
        .Function_Deinitalize = FLACOptions_Deinit,
    };
#endif /* OVIA_CodecIO_Decode */
    
#endif /* FLAC Literals */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_CodecIO_FLACCodec_H */
