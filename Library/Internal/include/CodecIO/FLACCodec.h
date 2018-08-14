#include "../../libOVIA.h"

#include "../../../../Dependencies/FoundationIO/libFoundationIO/include/BitIO.h"
#include "../../../../Dependencies/FoundationIO/libFoundationIO/include/Log.h"
#include "../../../../Dependencies/FoundationIO/libFoundationIO/include/Math.h"
#include "../../../../Dependencies/FoundationIO/libFoundationIO/include/ContainerIO.h"

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
        FLACMedizCatalogNumberSize                                  =        128,
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
    
    typedef struct FLACEncoder EncodeFLAC;
    
    typedef struct FLACDecoder DecodeFLAC;
    
    EncodeFLAC *InitFLACEncoder(void);
    
    DecodeFLAC *InitFLACDecoder(void);
    
    void        DeinitFLACEncoder(EncodeFLAC *Enc);
    
    void        DeinitFLACDecoder(DecodeFLAC *Dec);
    
    /* Decode specific functions */
    
    void        FLACReadFrame(BitBuffer *InputFLAC, DecodeFLAC *Dec);
    
    void        FLACReadSubFrame(BitBuffer *InputFLAC, DecodeFLAC *Dec, uint8_t Channel);
    
    void        FLACDecodeSubFrameVerbatim(BitBuffer *InputFLAC, DecodeFLAC *Dec);
    
    void        FLACDecodeSubFrameConstant(BitBuffer *InputFLAC, DecodeFLAC *Dec);
    
    void        FLACDecodeSubFrameFixed(BitBuffer *InputFLAC, DecodeFLAC *Dec);
    
    void        FLACDecodeSubFrameLPC(BitBuffer *InputFLAC, DecodeFLAC *Dec, uint8_t Channel);
    
    void        DecodeFLACesidual(BitBuffer *InputFLAC, DecodeFLAC *Dec);
    
    void        DecodeFLACice1Partition(BitBuffer *InputFLAC, DecodeFLAC *Dec);
    
    void        DecodeFLACice2Partition(BitBuffer *InputFLAC, DecodeFLAC *Dec);
    
    void        FLACBitDepth(DecodeFLAC *Dec);
    
    void        FLACSampleRate(BitBuffer *InputFLAC, DecodeFLAC *Dec);
    
    uint8_t     GetBlockSizeInSamples(uint8_t BlockSize);
    
    void        FLACReadStream(BitBuffer *InputFLAC, DecodeFLAC *Dec);
    
    void        FLACParseMetadata(BitBuffer *InputFLAC, DecodeFLAC *Dec);
    
    void        FLACParseStreamInfo(BitBuffer *InputFLAC, DecodeFLAC *Dec);
    
    void        FLACSkipPadding(BitBuffer *InputFLAC, DecodeFLAC *Dec);
    
    void        FLACSkipCustom(BitBuffer *InputFLAC, DecodeFLAC *Dec);
    
    void        FLACParseSeekTable(BitBuffer *InputFLAC, DecodeFLAC *Dec);
    
    void        FLACParseVorbisComment(BitBuffer *InputFLAC, DecodeFLAC *Dec);
    
    void        FLACParseCuesheet(BitBuffer *InputFLAC, DecodeFLAC *Dec);
    
    void        FLACParsePicture(BitBuffer *InputFLAC, DecodeFLAC *Dec);
    
    /* Encode specific functions */
    
    int8_t      EncodeFLACFile(OVIA *Ovia, BitBuffer *OutputFLAC, EncodeFLAC *Enc);
    
    typedef struct FLACStreamInfo {
        uint64_t  SamplesInStream:36; // 36 bits, all the samples in the stream; channel agnostic
        uint32_t            BitDepth;
        uint32_t MinimumFrameSize:24;
        uint32_t MaximumFrameSize:24;
        uint32_t       SampleRate:20; // 20 bits used
        uint16_t MinimumBlockSize:16; // in samples; must be at least 16
        uint16_t MaximumBlockSize:16; // If these match than it's fixed blocksize format
        uint8_t      CodedSampleRate; // 4 bits
        uint8_t             Channels;
        uint8_t      CodedBitDepth:6; // 5 bits, 4-32 bits per sample
        uint8_t      CodedChannels:4; // 3 bits, add 1 to get the real value
        uint8_t              MD5[16]; // MD5
    } FLACStreamInfo;
    
    typedef struct CueSheetTrack {
        uint8_t  **ISRC;            // the tracks's ISRC number.
        uint64_t *TrackOffset;         // samples from the beginning of the FLAC file to the first sample of the track
        uint8_t  *TrackNum;            // which track is this again?
        bool     *IsAudio;             // 1 for audio, 0 for data
        bool     *PreEmphasis;         // 1 for yes 0 for no.
        uint8_t  NumTrackIndexPoints;
    } CueSheetTrack;
    
    typedef struct FLACCueSheet {
        CueSheetTrack *Tracks;
        uint8_t       *NumTrackIndexPoints;
        char          *CatalogID;
        uint64_t      LeadIn;              // in samples
        uint64_t      IndexOffset; // no idea. must be multiple of CD sector for cd sources
        size_t        CatalogIDSize;
        uint8_t       NumTracks;           // the number of tracks, CD has a max of 100 (including lead out)
        bool          IsCD;                // 1 if it came from a CD; 0 otherwise
        uint8_t       IndexPointNum;
    } FLACCueSheet;
    
    typedef struct FLACVorbisComment {
        char     *VendorTag;
        uint8_t  *UserTagSize; // array for each tag, that says the size of the tag
        char     *UserTagString; // Array of tags.
        uint8_t   VendorTagSize;
        uint8_t   NumUserTags; // Number of tags.
        bool      VorbisFramingBit;
    } FLACVorbisComment;
    
    typedef struct FLACSeekTable {
        uint64_t     *SampleInTargetFrame; // FIXME: Sample in the whole file, or sample in a specific frame?
        uint64_t     *OffsetFrom1stSample; // in bytes
        uint16_t     *TargetFrameSize;
        uint32_t     NumSeekPoints;
    } FLACSeekTable;
    
    typedef struct FLACStream {
        uint32_t BlockSize;
        uint32_t SampleRate;
        uint8_t  BitDepth;
    } FLACStream;
    
    typedef struct FLACPicture {
        uint32_t *PictureStart; // Pointer to the start of the picture
        uint8_t  *MIMEString;
        uint8_t      *PicDescriptionString;
        uint32_t      PicDescriptionSize;
        FLACPicTypes  PicType;
        uint32_t      Width;
        uint32_t      Height;
        
        uint32_t      MIMESize; // size of the MIME string in bytes
        
        
        uint32_t  BitDepth;
        uint32_t  ColorsUsed; // 0 for not paletted
        uint32_t  PictureSize;
    } FLACPicture;
    
    typedef struct FLACSubFrame {
        uint8_t SubFrameType:6;
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
        uint8_t       FLACFrameCRC;
        uint8_t       PartitionOrder;
        uint8_t       CurrentPartition;
        uint8_t       SamplesInPartition;
        uint8_t       CodedSamplesInBlock;
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
    
    typedef struct FLACMeta {
        FLACPicture       *Pic;
        FLACCueSheet      *Cue;
        FLACStreamInfo    *StreamInfo;
        FLACSeekTable     *Seek; // Because the user may want to skip around, but this has nothing to do with decoding.
        FLACVorbisComment *Vorbis;
        uint32_t           MetadataSize;
    } FLACMeta;
    
    typedef struct FLACData {
        uint32_t         **RAWAudio;
        FLACFrame         *Frame;
        FLACLPC           *LPC;
        RICEPartition     *Rice;
        FLACSubFrame      *SubFrame;
        bool               GetSampleRateFromStreamInfo;
    } FLACData;
    
    struct FLACEncoder {
        int64_t   RawSamples[FLACMaxSamplesInBlock];
        FLACMeta *Meta;
        FLACData *Data;
        uint16_t  MaxBlockSize;
        uint8_t   MaxFilterOrder;
        uint8_t   MaxRICEPartitionOrder;
        bool      EncodeSubset;
        bool      OptimizeFile;
    };
    
    struct FLACDecoder {
        int64_t   DecodedSamples[FLACMaxSamplesInBlock];
        FLACMeta *Meta;
        FLACData *Data;
        bool      SeekTableIsPresent;
        bool      CuesheetIsPresent;
        bool      VorbisCommentIsPresent;
        bool      PictureIsPresent;
        bool      LastMetadataBlock;
    };
    
    void ModernFLACSetEncodeOptions(EncodeFLAC *Enc, const bool EncodeAsSubset, const bool Optimize) {
        if (Enc != NULL) {
            Enc->EncodeSubset = EncodeAsSubset;
            Enc->OptimizeFile = Optimize;
        }
    }
    
    void ModernFLACSetPicture(EncodeFLAC *Enc, const char *MIMEString, const FLACPicTypes PictureType, const UTF8 *PicDescription, const uint8_t *PictureBuffer) {
    }
    
    uint8_t *ModernFLACExtractPicture(DecodeFLAC *Dec, const FLACPictureTypes PictureType) {
        
        return NULL;
    }
    
    DecodeFLAC *InitDecodeFLAC(void) {
        DecodeFLAC *Dec           = calloc(1, sizeof(DecodeFLAC));
        if (Dec != NULL) {
            Dec->Meta             = calloc(1, sizeof(FLACMeta));
            Dec->Meta->StreamInfo = calloc(1, sizeof(FLACStreamInfo));
            Dec->Meta->Seek       = calloc(1, sizeof(FLACSeekTable));
            Dec->Meta->Vorbis     = calloc(1, sizeof(FLACVorbisComment));
            Dec->Meta->Cue        = calloc(1, sizeof(FLACCueSheet));
            Dec->Meta->Pic        = calloc(1, sizeof(FLACPicture));
            
            Dec->Data             = calloc(1, sizeof(FLACData));
            Dec->Data->Frame      = calloc(1, sizeof(FLACFrame));
            Dec->Data->SubFrame   = calloc(1, sizeof(FLACSubFrame));
            Dec->Data->LPC        = calloc(1, sizeof(FLACLPC));
            Dec->Data->Rice       = calloc(1, sizeof(RICEPartition));
        } else {
            BitIOLog(BitIOLog_ERROR, __func__, u8"EncodeFLAC Pointer is NULL");
        }
        return Dec;
    }
    
    EncodeFLAC *InitEncodeFLAC(void) {
        EncodeFLAC *Enc            = calloc(1, sizeof(EncodeFLAC));
        if (Enc != NULL) {
            Enc->Meta              = calloc(1, sizeof(FLACMeta));
            Enc->Meta->StreamInfo  = calloc(1, sizeof(FLACStreamInfo));
            Enc->Meta->Seek        = calloc(1, sizeof(FLACSeekTable));
            Enc->Meta->Vorbis      = calloc(1, sizeof(FLACVorbisComment));
            Enc->Meta->Cue         = calloc(1, sizeof(FLACCueSheet));
            Enc->Meta->Pic         = calloc(1, sizeof(FLACPicture));
            
            Enc->Data              = calloc(1, sizeof(FLACData));
            Enc->Data->Frame       = calloc(1, sizeof(FLACFrame));
            Enc->Data->SubFrame    = calloc(1, sizeof(FLACSubFrame));
            Enc->Data->LPC         = calloc(1, sizeof(FLACLPC));
            Enc->Data->Rice        = calloc(1, sizeof(RICEPartition));
        } else {
            BitIOLog(BitIOLog_ERROR, __func__, u8"EncodeFLAC Pointer is NULL");
        }
        return Enc;
    }
    
    
#ifdef __cplusplus
}
#endif

#endif /* OVIA_FLACCommon_H */
