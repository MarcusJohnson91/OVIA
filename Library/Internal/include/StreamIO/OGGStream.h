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
