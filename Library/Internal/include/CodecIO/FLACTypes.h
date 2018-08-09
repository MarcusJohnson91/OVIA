#include "../../libModernFLAC.h"
#include "../../../../Dependencies/libPCM/Dependencies/BitIO/libBitIO/include/BitIOLog.h"

#pragma once

#ifndef libModernFLAC_Types_H
#define libModernFLAC_Types_H

#ifdef __cplusplus
extern "C" {
#endif
    
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

#endif /* libModernFLAC_Types_H */
