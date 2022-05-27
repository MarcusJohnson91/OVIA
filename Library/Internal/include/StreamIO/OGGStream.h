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

#include "../../../OVIA/include/StreamIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
#ifdef OVIA_StreamIO_OGG
    extern const OVIA_MagicIDs OGGSignature;
    
    const OVIA_MagicIDs OGGSignature = {
        .NumMagicIDs          = 1,
        .MagicIDs             = {
            OVIA_RegisterSignature(0, 32, (uint8_t[4]){0x4F, 0x67, 0x67, 0x53})
        },
    };
    
    extern const OVIA_Extensions OGGExtensions;
    
    const OVIA_Extensions OGGExtensions = {
        .NumExtensions     = 7,
        .Extensions        = {
            OVIA_RegisterExtension("opus")
            OVIA_RegisterExtension("ogg")
            OVIA_RegisterExtension("ogv")
            OVIA_RegisterExtension("oga")
            OVIA_RegisterExtension("ogx")
            OVIA_RegisterExtension("ogm")
            OVIA_RegisterExtension("spx")
        }
    };
    
    extern const OVIA_MIMETypes OGGMIMETypes;
    
    const OVIA_MIMETypes OGGMIMETypes = {
        .NumMIMETypes     = 3,
        .MIMETypes        = {
            OVIA_RegisterMIMEType("audio/ogg")
            OVIA_RegisterMIMEType("video/ogg")
            OVIA_RegisterMIMEType("application/ogg")
        }
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
