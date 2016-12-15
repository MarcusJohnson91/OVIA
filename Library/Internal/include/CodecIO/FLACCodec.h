#include </usr/local/Packages/BitIO/include/BitIO.h>

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

    enum FLACConstants {
        FLACMagic                                                   = 0x664c6143, // fLaC
        FLACFrameMagic                                              =     0x3FFE,
        FLACMaxChannels                                             =          8,
        FLACMaxSamplesInBlock                                       =      65535,
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
    } FLACConstants;

    enum FLACPictureTypes {
        Other                                                       = 0,
        FileIcon                                                    = 1, // 32x32, PNG only
        OtherFileIcon                                               = 2,
        FrontCover                                                  = 3, // main album art
        BackCover                                                   = 4,
        LeafletPage                                                 = 5,
        LabelSide                                                   = 6, // no clue
        LeadPerformer                                               = 7,
        Performer                                                   = 8,
        Conducter                                                   = 9,
        BandOrOrchestra                                             = 10,
        Composer                                                    = 11,
        Writer                                                      = 12,
        RecordingLocation                                           = 13,
        DuringRecording                                             = 14,
        DuringPerformance                                           = 15,
        Screenshot                                                  = 16,
        XiphLogo                                                    = 17,
        Illustration                                                = 18,
        BandLogo                                                    = 19,
        PublisherLogo                                               = 20,
    } FLACPictureTypes;

    enum SpeakerLayout {
        Mono                                                        = 0,
        Left_Right                                                  = 1,
        Left_Right_Center                                           = 2,
        Left_Right_BackLeft_BackRight                               = 3,
        Left_Right_Center_BackLeft_BackRight                        = 4,
        Left_Right_Center_LFE_BackLeft_BackRight                    = 5,
        Left_Right_Center_LFE_BackCenter_SideLeft_SideRight         = 6,
        Left_Right_Center_LFE_BackLeft_BackRight_SideLeft_SideRight = 7,
        Left_Diff                                                   = 8,
        Right_Diff                                                  = 9,
        Average_Diff                                                = 10,
    };

    enum WAVEConstants {
        RIFFMagic                                                   = 0x52494646,
        WAVEMagic                                                   = 0x57415645,
    } WAVEConstants;

    enum FLACPredictionTypes {
        Verbatim                                                    = 0,
        Constant                                                    = 1,
        FixedLinear                                                 = 2,
        FIRLinear                                                   = 3,
    } FLACPredictionTypes;

    enum FLACResidualTypes {
        Simple                                                      = 0,
        Complex                                                     = 1,
    } FLACResidualTypes;

    enum FLACMetadataTypes {
        StreamInfo                                                  = 0, // 0x30 = 0 in ascii, 0x31 = 1, etc.
        Padding                                                     = 1,
        Custom                                                      = 2, // Application aka App specific
        SeekTable                                                   = 3,
        VorbisComment                                               = 4,
        Cuesheet                                                    = 5,
        Picture                                                     = 6,
    } FLACMetadataTypes;

    enum FLACSubFrameTypes {
        Subframe_Constant                                           =  0,
        Subframe_Verbatim                                           =  1,
        Subframe_Fixed                                              =  8,
        Subframe_LPC                                                = 63,
    } FLACSubFrameTypes;

    enum FLACBlockType {
        FixedBlockSize    = 0,
        VariableBlockSize = 1,
    } FLACBlockType;

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
        uint8_t     MD5[FLACMD5Size]; // MD5
    } FLACStreamInfo;

    typedef struct CueSheetTrack {
        uint64_t TrackOffset[FLACMaxCueTracks];         // samples from the beginning of the FLAC file to the first sample of the track
        uint8_t  TrackNum[FLACMaxCueTracks];            // which track is this again?
        uint8_t  ISRC[FLACMaxCueTracks][FLACISRCSize];            // the tracks's ISRC number.
        bool     IsAudio[FLACMaxCueTracks];             // 1 for audio, 0 for data
        bool     PreEmphasis[FLACMaxCueTracks];         // 1 for yes 0 for no.
                                                        // 19 bytes of 0s
        uint8_t  NumTrackIndexPoints;
    } CueSheetTrack;

    typedef struct FLACCueSheet {
        char     CatalogID[128];
        uint64_t LeadIn;              // in samples
        bool     IsCD;                // 1 if it came from a CD; 0 otherwise
                                      // 265 bytes of 0s to be skipped
        uint8_t  NumTracks;           // the number of tracks, CD has a max of 100 (including lead out)

        // Cuesheet_Track
        //CueSheetTrack Track[FLACMaxCueTracks];  // Pointer to array of CueSheetTrack structs.

        // How about we just do it normally?
        // CUESHEET_TRACK
        uint64_t TrackOffset[FLACMaxCueTracks];         // samples from the beginning of the FLAC file to the first sample of the track
        uint8_t  TrackNum[FLACMaxCueTracks];            // which track is this again?
        uint8_t  ISRC[FLACMaxCueTracks][12];            // the tracks's ISRC number.
        bool     IsAudio[FLACMaxCueTracks];             // 1 for audio, 0 for data
        bool     PreEmphasis[FLACMaxCueTracks];         // 1 for yes 0 for no.
                                                        // 19 bytes of 0s
        uint8_t  NumTrackIndexPoints[FLACMaxCueTracks];
        // CUESHEET_TRACK


        // Cuesheet_Track_Index
        uint64_t       IndexOffset; // no idea. must be multiple of CD sector for cd sources
        uint8_t      IndexPointNum;
    } FLACCueSheet;

    typedef struct FLACVorbisComment {
        char     VendorString[FLACMaxVorbisCommentSize];
        char     UserCommentString[FLACVorbisMaxComments][FLACMaxVorbisCommentSize];
        bool     VorbisFramingBit;
    } FLACVorbisComment;

    typedef struct FLACSeekTable {
        uint32_t NumSeekPoints;
        uint64_t SampleInTargetFrame[FLACMaxSeekPoints]; // FIXME: Sample in the whole file, or sample in a specific frame?
        uint64_t OffsetFrom1stSample[FLACMaxSeekPoints]; // in bytes
        uint16_t     TargetFrameSize[FLACMaxSeekPoints];
    } FLACSeekTable;

    typedef struct FLACStream {
        uint32_t BlockSize;
        uint32_t SampleRate;
        uint8_t  BitDepth;
    } FLACStream;

    typedef struct FLACPicture {
        uint32_t PicType;
        uint32_t MIMESize; // size of the MIME string in bytes
        uint8_t  MIMEString[FLACMaxMIMEString];
        uint32_t PicDescriptionSize;
        uint8_t  PicDescriptionString[FLACMaxPicDescriptionString];
        uint32_t Width;
        uint32_t Height;
        uint32_t BitDepth;
        uint32_t ColorsUsed; // 0 for not paletted
        uint32_t PictureSize;
        uint8_t  PictureBuffer[FLACMaxPicBuffer];
    } FLACPicture;

    typedef struct FLACSubFrame {
        bool    ZeroBit;
        uint8_t SubFrameType:6;
        uint8_t WastedBits:6; // Uses unary coding
    } FLACSubFrame;

    typedef struct FLACFrame {
        bool          BlockType:1;
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
    
#ifdef __cplusplus
}
#endif
