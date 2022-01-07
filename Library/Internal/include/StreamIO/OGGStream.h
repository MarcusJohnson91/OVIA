/*!
 @header              OGGStream.h
 @author              Marcus Johnson
 @copyright           2020+
 @version             1.0.0
 @brief               This header contains code for OGG streams.
 */

#pragma once

#ifndef OVIA_StreamIO_OGGStream_H
#define OVIA_StreamIO_OGGStream_H

#include "StreamIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
#ifdef OVIA_StreamIO_OGG
    extern const OVIA_MagicIDs OGGSignature;
    
    const OVIA_MagicIDs OGGSignature = {
        .NumMagicIDs          = 1,
        .MagicIDs             = {
            [0]               = {
                .OffsetInBits = 0,
                .SizeInBits   = 32,
                .Signature    = (uint8_t[4]){0x4F, 0x67, 0x67, 0x53},
            },
        },
    };
    
    extern const OVIA_Extensions OGGExtensions;
    
    const OVIA_Extensions OGGExtensions = {
        .NumExtensions     = 4,
        .Extensions        = {
            [0]            = {
                .Size      = 4,
                .Extension = UTF32String("opus"),
            },
            [1]            = {
                .Size      = 3,
                .Extension = UTF32String("ogg"),
            },
            [2]            = {
                .Size      = 3,
                .Extension = UTF32String("ogv"),
            },
            [3]            = {
                .Size      = 3,
                .Extension = UTF32String("oga"),
            },
            [4]            = {
                .Size      = 3,
                .Extension = UTF32String("ogx"),
            },
            [5]            = {
                .Size      = 3,
                .Extension = UTF32String("ogm"),
            },
            [6]            = {
                .Size      = 3,
                .Extension = UTF32String("spx"),
            },
        },
    };
    
    extern const OVIA_MIMETypes OGGMIMETypes;
    
    const OVIA_MIMETypes OGGMIMETypes = {
        .NumMIMETypes     = 3,
        .MIMETypes        = {
            [0]           = {
                .Size     = 9,
                .MIMEType = UTF32String("audio/ogg"),
            },
            [1]           = {
                .Size     = 9,
                .MIMEType = UTF32String("video/ogg"),
            },
            [2]           = {
                .Size     = 15,
                .MIMEType = UTF32String("application/ogg"),
            },
        },
    };
    
#if defined(OVIA_StreamIO_Encode)
    extern const OVIA_Stream OGGMuxer;
    
    const OVIA_Stream OGGMuxer = {
        .MagicID = &OGGSignature,
    };
#endif /* OVIA_StreamIO_Encode */
    
#if defined(OVIA_StreamIO_Decode)
    extern const OVIA_Stream OGGDemuxer;
    
    const OVIA_Stream OGGDemuxer = {
        .MagicID = &OGGSignature,
    };
#endif /* OVIA_StreamIO_Decode */
    
#endif /* OGG Literals */

    /*
     Ok, so an OGG stream can contain various formats, like FLAC
     */

    /*
     Actual Format:

     Packet 0: First Packet in the First Page, size = 79 bytes; Marked as Start of Stream

     PacketType     = 0x7F
     FLACString     = 0x46, 0x4C, 0x41, 0x43
     MappingMajorV  = 0x1
     MappingMinorV  = 0x0
     HeaderPackets  = Num packets in header, 16 bit big endian, 0x0000 = Unknown
     FLACSignature  = 0x664C6143
     FLACStreamInfo = The actual StreamInfo block
     */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_StreamIO_OGGStream_H */
