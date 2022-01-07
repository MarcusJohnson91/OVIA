/*!
 @header              StreamIO.h
 @author              Marcus Johnson
 @copyright           2020+
 @version             1.0.0
 @brief               This header contains code for streams (muxing and demuxing).
 */

#pragma once

#ifndef OVIA_StreamIO_H
#define OVIA_StreamIO_H

#include "../MediaIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h" /* Included for Byte and Bit Order enums */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    /*
     The biggest problem with StreamIO, is we need to provide a interface for both files and packets.

     I mean, isn't that the entire god damn point of BufferIO? that it can be used to piece files into chunks, and load multiple packets into chunks as well?

     it's the entire god damn point...

     So we're ok there...

     All we really need is the infrastructure to say where a file or network input or output is in relation to BufferIO, and have this interface be maintained by the buffer it's self.

     So, modify FileIO and NetworkIO to have BufferIO control them; they're downstream from BufferIO; BufferIO is in charge.

     ----

     Is there a lower level interface for dealing with files on POSIX or in C?

     Fuck a big ass FILE pointer with it's associated buffers and all kinds of stupid shit.

     Ok, yes you can use the `open()` function, it's a part of POSIX, and is available on windows; returns a file descriptor aka an integer.
     */

    typedef enum StreamIO_SyncTypes {
        SyncType_Unknown = 0,
        SyncType_Marker  = 1,
        SyncType_Packet  = 2,
    } StreamIO_SyncTypes;

    typedef enum StreamIO_PacketTypes {
        PacketType_Unknown  = 0,
        PacketType_Constant = 1,
        PacketType_Variable = 2,
    } StreamIO_PacketTypes;

    typedef struct OVIA_Stream {
        // Once we have the kind of marker to look for, we need to know the offset from the sync code as well as the actual sync code
        const OVIA_MagicIDs        *MagicID;           // uint8_t              *SyncCode;          // If SyncType is Marker
        const uint64_t              PacketSizeInBytes; // 0 = Unknown
        const uint64_t              OffsetInBits;      //
        const uint64_t              FieldSizeInBits;   // the number of bits to extract in order to mask out whatever informat is necessary.
        const StreamIO_PacketTypes  PacketType;        // If SyncType is Packet
        const StreamIO_SyncTypes    SyncType;
        const BufferIO_ByteOrders   ByteOrder;
        const BufferIO_BitOrders    BitOrder;
    } OVIA_Stream;

    static const OVIA_Stream M2TSStreamInfo = {
        .SyncType          = SyncType_Packet,
        .PacketType        = PacketType_Constant,
        .PacketSizeInBytes = 192,
    };

    // if PackSize is Variable, we need to know how to get the size of each packet.

    /*
     What is our goal? Extract substreams and data from container formats, interleaving/deinterleaving streams.

     identifying contained substreams.

     extract audio/video frames intact so things can be remuxed or whatever.
     */

    /*
     So, theres Streams, and Substreams (with potential for sub-substreams etc.)
     So, What does a Stream need?
     */

    typedef struct OVIAStream {
        uint32_t            PayloadSize; // How many bits can be stored in the payload, 0 = variable sized.
        uint8_t             Alignment;   // number of bits needed for each field
        uint8_t             ChunkIDSize; // How many bits should be read for each chunk id
        BufferIO_ByteOrders ByteOrder;   // What byte order does the Stream use?
        BufferIO_BitOrders  BitOrder;    // What bit order does the stream use?
        
        /*
         MagicID for the Stream format
         ChunkIDs
         Payload size like in MPEG Transport Stream being 188 bytes per payload
         a PayloadSize of 0 means variable sized payloads

         What do we do with basic formats like BMP/PNM/WAV/AIF?

         and what about formats that use their own 1 off stream format like FLAC?
         */
    } OVIAStream;

    typedef enum StreamIO_StreamTypes {
        StreamType_Unknown = 0,
        StreamType_Audio2D = 1,
        StreamType_Audio3D = 2,
        StreamType_Image   = 3,
        StreamType_Video   = 4,
    } StreamIO_StreamTypes;

    typedef struct StreamIO_Limitations {
        uint64_t             MaxStreamSize;       // after accounting for all the overhead.
        StreamIO_StreamTypes SupportedStreamTypes;
    } StreamIO_Limitations;

    typedef struct OVIADemuxer {
        const OVIA_MagicIDs    *MagicID;
        const OVIA_MediaTypes   MediaType;
    } OVIADemuxer;

    typedef struct OVIAMuxer {
        const OVIA_MediaTypes   MediaType;
        const OVIA_ContainerIDs ContainerID;
    } OVIAMuxer;

    /*
     RIFF/RF64 as well as "AVI ", "ANI ", "WebP".

     Structure of RIFF: RIFFXXXXYYYY where XXXX is the size and YYYY is the substream.
     */

    /*
     We also need to work on limiting which codecs can be used with which Stream

     So, WAV streams primarily support PCM streams

     MKV streams can support most codecs.

     OGG can support a handful of audio and video codecs.

     FLACNative streams can only support FLAC encoded audio.

     So, Stream registry needs to know the kind of codecs supported; Image, Video, 2DAudio, 3DAudio, Subtitles, Tags

     and which codecs are actually supported
     */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_StreamIO_H */

