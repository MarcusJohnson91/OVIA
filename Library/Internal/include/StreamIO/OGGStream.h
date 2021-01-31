/*!
 @header              OGGStream.h
 @author              Marcus Johnson
 @copyright           2020+
 @version             1.0.0
 @brief               This header contains code for OGG streams.
 Has Substreams.
 is multiplexible
 */

#include "StreamIO.h"

#pragma once

#ifndef OVIA_StreamIO_OGGStream_H
#define OVIA_StreamIO_OGGStream_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
#ifdef OVIA_StreamIO_OGG
    extern OVIA_MagicIDs OGGSignature = {
        .NumMagicIDs         = 3,
        .MagicIDOffsetInBits = 0,
        .MagicIDSizeInBits   = 32,
        .MagicIDNumber       = {
            [0]              = (uint8_t[2]){0x52, 0x49, 0x46, 0x46},
            [1]              = (uint8_t[2]){0x42, 0x57, 0x36, 0x34},
            [2]              = (uint8_t[4]){0x66, 0x66, 0x69, 0x72},
        }
    };

    extern const OVIA_Extensions OGGExtensions = {
        .NumExtensions = 4,
        .Extensions    = {
            [0]        = UTF32String("wav"),
            [1]        = UTF32String("wave"),
            [2]        = UTF32String("bwf"),
            [3]        = UTF32String("w64"),
        },
    };

    extern const OVIA_MIMETypes OGGMIMETypes = {
        .NumMIMETypes = 4,
        .MIMETypes    = {
            [0]       = UTF32String("audio/vnd.wave"),
            [1]       = UTF32String("audio/wav"),
            [2]       = UTF32String("audio/wave"),
            [3]       = UTF32String("audio/x-wav"),
        },
    };
#endif /* OVIA_StreamIO_OGG */
    
#if defined(OVIA_StreamIO_Encode) && defined(OVIA_StreamIO_OGG)
    extern OVIA_Stream OGGEncoder = {
        .MagicID = &OGGMagicIDs,
    };
#endif /* OVIA_StreamIO_Encode && OVIA_StreamIO_OGG */
    
#if defined(OVIA_StreamIO_Decode) && defined(OVIA_StreamIO_OGG)
    extern OVIA_Stream OGGDecoder = {
        .MagicID = &OGGMagicIDs,
    };
#endif /* OVIA_StreamIO_Decode && OVIA_StreamIO_OGG */

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
