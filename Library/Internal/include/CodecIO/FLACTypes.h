#include "../../../Dependencies/libPCM/Dependencies/BitIO/libBitIO/include/BitIO.h"

#pragma once

#ifndef LIBMODERNFLAC_FLACTYPES_H
#define LIBMODERNFLAC_FLACTYPES_H

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef struct FLACStreamInfo {
        uint16_t MinimumBlockSize:16; // in samples; must be at least 16
        uint16_t MaximumBlockSize:16; // If these match than it's fixed blocksize format
        uint32_t MinimumFrameSize:24;
        uint32_t MaximumFrameSize:24;
        uint8_t      CodedSampleRate; // 4 bits
        uint32_t       SampleRate:20; // 20 bits used
        uint8_t      CodedChannels:4; // 3 bits, add 1 to get the real value
        uint8_t             Channels;
        uint8_t      CodedBitDepth:6; // 5 bits, 4-32 bits per sample
        uint32_t            BitDepth;
        uint64_t  SamplesInStream:36; // 36 bits, all the samples in the stream; channel agnostic
        uint8_t     MD5[BitIOMD5Size]; // MD5
    } FLACStreamInfo;
    
    typedef struct CueSheetTrack {
        uint64_t *TrackOffset;         // samples from the beginning of the FLAC file to the first sample of the track
        uint8_t  *TrackNum;            // which track is this again?
        uint8_t  **ISRC;            // the tracks's ISRC number.
        bool     *IsAudio;             // 1 for audio, 0 for data
        bool     *PreEmphasis;         // 1 for yes 0 for no.
                                                        // 19 bytes of 0s
        uint8_t  NumTrackIndexPoints;
    } CueSheetTrack;
    
    typedef struct FLACCueSheet {
        size_t    CatalogIDSize;
        char     *CatalogID;
        uint64_t  LeadIn;              // in samples
        bool      IsCD;                // 1 if it came from a CD; 0 otherwise
                                      // 265 bytes of 0s to be skipped
        uint8_t  NumTracks;           // the number of tracks, CD has a max of 100 (including lead out)
        
        // Cuesheet_Track
        //CueSheetTrack Track[FLACMaxCueTracks];  // Pointer to array of CueSheetTrack structs.
        
        // How about we just do it normally?
        // CUESHEET_TRACK
        uint64_t *TrackOffset;         // samples from the beginning of the FLAC file to the first sample of the track
        uint8_t  *TrackNum;            // which track is this again?
        uint8_t  **ISRC;            // the tracks's ISRC number.
        bool     *IsAudio;             // 1 for audio, 0 for data
        bool     *PreEmphasis;         // 1 for yes 0 for no.
                                                        // 19 bytes of 0s
        uint8_t  *NumTrackIndexPoints;
        // CUESHEET_TRACK
        
        
        // Cuesheet_Track_Index
        uint64_t       IndexOffset; // no idea. must be multiple of CD sector for cd sources
        uint8_t      IndexPointNum;
    } FLACCueSheet;
    
    typedef struct FLACVorbisComment {
        bool      VorbisFramingBit;
        uint8_t   VendorTagSize;
        char     *VendorTag;
        uint8_t   NumUserTags; // Number of tags.
        uint8_t  *UserTagSize; // array for each tag, that says the size of the tag
        char     *UserTagString; // Array of tags.
    } FLACVorbisComment;
    
    typedef struct FLACSeekTable {
        uint32_t NumSeekPoints;
        uint64_t *SampleInTargetFrame; // FIXME: Sample in the whole file, or sample in a specific frame?
        uint64_t *OffsetFrom1stSample; // in bytes
        uint16_t     *TargetFrameSize;
    } FLACSeekTable;
    
    typedef struct FLACStream {
        uint32_t BlockSize;
        uint32_t SampleRate;
        uint8_t  BitDepth;
    } FLACStream;
    
    typedef struct FLACPicture {
        uint32_t PicType;
        uint32_t MIMESize; // size of the MIME string in bytes
        uint8_t  *MIMEString;
        uint32_t PicDescriptionSize;
        uint8_t  *PicDescriptionString;
        uint32_t Width;
        uint32_t Height;
        uint32_t BitDepth;
        uint32_t ColorsUsed; // 0 for not paletted
        uint32_t PictureSize;
        uint32_t *PictureStart; // Pointer to the start of the picture
    } FLACPicture;
    
    typedef struct FLACSubFrame {
        uint8_t SubFrameType:6;
        bool    WastedBitsFlag:1;
        uint8_t WastedBits:6; // Uses unary coding
    } FLACSubFrame;
    
    typedef struct FLACFrame {
        bool          BlockType:1;
        uint8_t       CodedSamplesInBlock;
        uint16_t      BlockSize; // SamplesInBlock
        uint8_t       CodedSampleRate:5;
        uint32_t      SampleRate;
        uint8_t       ChannelLayout:4;
        uint8_t       CodedBitDepth:4;
        uint8_t       BitDepth;
        // If variable block size
        uint64_t      SampleNumber:36;
        // Otherwise
        uint32_t      FrameNumber:31;
        // End
        FLACSubFrame *Sub;
        uint8_t       FLACFrameCRC;
        uint8_t       PartitionOrder;
        uint8_t       CurrentPartition;
        uint8_t       SamplesInPartition;
    } FLACFrame;
    
    typedef struct RICEPartition {
        uint8_t       NumRICEPartitions:4;
        uint8_t       EscapeBitDepth:5;
        uint8_t      *RICEParameter;
    } RICEPartition;
    
    typedef struct FLACLPC {
        uint8_t       LPCOrder;
        uint8_t       LPCPrecision:4;
        uint8_t       LPCShift:5;
        uint8_t       NumLPCCoeffs;
        int8_t       *LPCCoeff;
        uint8_t       RicePartitionType:2;
        uint8_t       PartitionOrder:4;
    } FLACLPC;
    
    typedef struct FLACMeta {
        uint32_t           MetadataSize;
        FLACStreamInfo    *StreamInfo;
        FLACSeekTable     *Seek; // Because the user may want to skip around, but this has nothing to do with decoding.
        FLACVorbisComment *Vorbis;
        FLACCueSheet      *Cue;
        FLACPicture       *Pic;
    } FLACMeta;
    
    typedef struct FLACData {
        bool               GetSampleRateFromStreamInfo;
        FLACFrame         *Frame;
        FLACSubFrame      *SubFrame;
        FLACLPC           *LPC;
        RICEPartition     *Rice;
        uint32_t         **RAWAudio;
    } FLACData;
    
    struct FLACEncoder {
        bool      EncodeSubset;
        bool      OptimizeFile;
        uint16_t  MaxBlockSize;
        uint8_t   MaxFilterOrder;
        uint8_t   MaxRICEPartitionOrder;
        FLACMeta *Meta;
        FLACData *Data;
        int64_t   RawSamples[FLACMaxSamplesInBlock];
    };
    
    struct FLACDecoder {
        bool      SeekTableIsPresent;
        bool      CuesheetIsPresent;
        bool      VorbisCommentIsPresent;
        bool      PictureIsPresent;
        bool      LastMetadataBlock;
        FLACMeta *Meta;
        FLACData *Data;
        int64_t   DecodedSamples[FLACMaxSamplesInBlock];
    };
    
    DecodeFLAC *InitDecodeFLAC(void) {
        DecodeFLAC *Dec       = calloc(1, sizeof(DecodeFLAC));
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
        return Dec;
    }
    
    EncodeFLAC *InitEncodeFLAC(void) {
        EncodeFLAC *Enc        = calloc(1, sizeof(EncodeFLAC));
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
        return Enc;
    }
    
#ifdef __cplusplus
}
#endif

#endif /* LIBMODERNFLAC_FLACTYPES_H */
