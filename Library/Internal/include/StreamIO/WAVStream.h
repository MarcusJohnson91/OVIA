/*!
 @header              WAVStream.h
 @author              Marcus Johnson
 @copyright           2017+
 @version             1.0.0
 @brief               This header contains code for reading and writing lossless WAV audio files
 */

#pragma once

#ifndef OVIA_StreamIO_WAVStream_h
#define OVIA_StreamIO_WAVStream_h

#include "../StreamIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef struct WAVInfo {
        UTF8 *Title;
        UTF8 *Artist;
        UTF8 *Album;
        UTF8 *ReleaseDate;
        UTF8 *Genre;
        UTF8 *CreationSoftware;
    } WAVInfo;

    typedef struct BEXTChunk {
        UTF8 *Description;
        UTF8 *Originator;
        UTF8 *OriginatorRef;
        UTF8 *OriginatorDate;
    } BEXTChunk;
    
    typedef struct WAVOptions {
        WAVInfo   *Info;
        BEXTChunk *BEXT;
        uint64_t   NumSamplesWritten;
        uint32_t   SampleRate;
        uint32_t   ByteRate;
        uint32_t   BlockAlign;
        uint32_t   SpeakerMask;
        uint32_t   ChannelMask;
        uint16_t   CompressionFormat;
        uint16_t   BlockAlignment;
        uint16_t   NumChannels;
        uint16_t   BitDepth;
        uint16_t   ValidBitsPerSample;
    } WAVOptions;

    void *WAVOptions_Init(void);

    void WAVSkipPadding(BitBuffer *BitB, uint32_t SubChunkSize);

    void WAVOptions_Deinit(void *Options);
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_StreamIO_WAVStream_h */
