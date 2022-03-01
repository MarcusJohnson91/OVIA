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

#include "../../../OVIA/include/StreamIO.h"

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

#ifdef OVIA_StreamIO_WAV
    extern const OVIA_MagicIDs WAVSignature;

    const OVIA_MagicIDs WAVSignature = {
        .NumMagicIDs          = 1,
        .MagicIDs             = {
            [0]               = {
                .OffsetInBits = 0,
                .SizeInBits   = 32,
                .Signature    = (uint8_t[4]) {0x57, 0x41, 0x56, 0x45},
            },
        },
    };

    extern const OVIA_Extensions WAVExtensions;

    const OVIA_Extensions WAVExtensions = {
        .NumExtensions     = 2,
        .Extensions        = {
            [1]            = {
                .Size      = 3,
                .Extension = UTF32String("wav"),
            },
            [2]            = {
                .Size      = 4,
                .Extension = UTF32String("wave"),
            },
        },
    };

    extern const OVIA_MIMETypes WAVMIMETypes;

    const OVIA_MIMETypes WAVMIMETypes = {
        .NumMIMETypes     = 2,
        .MIMETypes        = {
            [0]           = {
                .Size     = 10,
                .MIMEType = UTF32String("audio/wav"),
            },
            [1]           = {
                .Size     = 12,
                .MIMEType = UTF32String("audio/x-wav"),
            }
        },
    };

#if defined(OVIA_StreamIO_Encode)
    extern const OVIA_Stream WAVMuxer;

    const OVIA_Stream WAVMuxer = {
        .MediaType             = MediaType_Audio2D,
        .MagicID               = &WAVSignature,
        .Function_Initialize   = WAVOptions_Init,
        .Function_Parse        = WAVParseMetadata,
        .Function_Decode       = WAVExtractSamples,
        .Function_Deinitialize = WAVOptions_Deinit,
    };
#endif /* OVIA_StreamIO_Encode */

#if defined(OVIA_StreamIO_Decode)
    extern const StreamIO_Demuxer WAVDemuxer;

    const StreamIO_Demuxer WAVDemuxer = {
        .MediaType             = MediaType_Audio2D,
        .MagicID               = &WAVSignature,
        .Function_Initialize   = WAVOptions_Init,
        .Function_Parse        = WAVParseMetadata,
        .Function_Decode       = WAVExtractSamples,
        .Function_Deinitialize = WAVOptions_Deinit,
    };
#endif /* OVIA_StreamIO_Decode */

#endif /* OVIA_StreamIO_WAV */
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_StreamIO_WAVStream_h */
