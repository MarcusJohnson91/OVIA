#include "Common.h"
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct FLACFile {
        uint32_t           MetadataSize;
        FLACStreamInfo    *StreamInfo;
        FLACCueSheet      *Cue;
        FLACFrame         *Frame;
        FLACVorbisComment *Vorbis;
        FLACSeekTable     *Seek;
        FLACPicture       *Pic;
        bool               GetSampleRateFromStreamInfo;
        uint32_t           RAWAudio[FLACMaxChannels][FLACMaxSamplesInBlock];

    } FLACFile;

    uint8_t   GetBlockSizeInSamples(uint8_t BlockSize);

    void      FLACSampleRate(BitInput *BitI, FLACFile *FLAC);

    uint32_t  FLACBitDepth(FLACFile *FLAC);

    void      SkipMetadataPadding(BitInput *BitI, FLACFile *FLAC);

    void      FLACReadFrame(BitInput *BitI, FLACFile *FLAC);

    void      FLACReadMetadata(BitInput *BitI, FLACFile *FLAC, int64_t MaxPicBuffer);

    void      FLACReadPicture(BitInput *BitI, FLACFile *FLAC, int64_t MaxPicBuffer);

    void      FLACReadCuesheet(BitInput *BitI, FLACFile *FLAC);

    void      FLACReadSeekTable(BitInput *BitI, FLACFile *FLAC);

    void      FLACReadStreamInfo(BitInput *BitI, FLACFile *FLAC);

    void      FLACReadVorbisComment(BitInput *BitI, FLACFile *FLAC);
    
    void      FLACDecodeFile(int argc, const char *argv[]);
    
    void      InitFLACFile(FLACFile *FLAC);

    void      FLACReadSubFrame(BitInput *BitI, FLACFile *FLAC, uint8_t Channel);

    void      FLACDecodeResidual(BitInput *BitI, FLACFile *FLAC, uint32_t BlockSize, uint8_t PredictorOrder);

    void      FLACDecodeRicePartition(BitInput *BitI, FLACFile *FLAC, uint8_t PartitionType);
    
#ifdef __cplusplus
}
#endif
