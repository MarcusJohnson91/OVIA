/*!
 @header              StreamIO.h
 @author              Marcus Johnson
 @copyright           2020+
 @version             1.0.0
 @brief               This header contains code for streams (muxing and demuxing).
 */

#include "../../../Dependencies/FoundationIO/Library/include/PlatformIO.h" /* Included for Platform Independence macros */
#include "../../../Dependencies/FoundationIO/Library/include/BitIO.h"      /* Included for BitIO_ByteOrders and BitIO_BitOrders enums */
#include "OVIAInternal.h"

#pragma  once

#ifndef OVIA_StreamIO_H
#define OVIA_StreamIO_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

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
        OVIA_MagicIDs        *MagicID;           // uint8_t              *SyncCode;          // If SyncType is Marker
        uint64_t              PacketSizeInBytes; // 0 = Unknown
        uint64_t              OffsetInBits;      //
        uint64_t              FieldSizeInBits;   // the number of bits to extract in order to mask out whatever informat is necessary.
        StreamIO_PacketTypes  PacketType;        // If SyncType is Packet
        StreamIO_SyncTypes    SyncType;
        BitIO_ByteOrders      ByteOrder;
        BitIO_BitOrders       BitOrder;
    } OVIA_Stream;

    static const OVIA_MagicIDs NativeFLACMagicID = {
        .NumMagicIDs         = 1,
        .MagicIDOffsetInBits = 0,
        .MagicIDSizeInBits   = 32,
        .MagicIDNumber       = {
            [0]              = (uint8_t[4]){0x66, 0x4C, 0x61, 0x43},
        },
    };

    static const OVIA_Stream NativeFLACInfo = {
        .SyncType          = SyncType_Packet | SyncType_Marker, // NativeFLAC uses both kinds, the metablocks are Packet based, then the sync codes are for audio frames; the switch happens once the IsLastMetaBlock bit is set, which is the first bit.
        // How do we handle formats that need to extract multiple pieces of information?
        // Packet formats tend to use flag bits.
        /*
         Logic: If SyncType is both Packet and Marker, we need to know how to detect the switch over
         */
        .PacketType        = PacketType_Variable,
        .PacketSizeInBytes = 0,
        .MagicID           = &NativeFLACMagicID,
        .OffsetInBits      = 1,
        .FieldSizeInBits   = 7,
        .ByteOrder         = ByteOrder_LSByteIsFarthest,
        .BitOrder          = BitIO_BitOrder_MSBit,
    };

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
        uint8_t          Alignment;   // number of bits needed for each field
        BitIO_ByteOrders ByteOrder;   // What byte order does the Stream use?
        BitIO_BitOrders  BitOrder;    // What bit order does the stream use?
        uint8_t          ChunkIDSize; // How many bits should be read for each chunk id
        uint32_t         PayloadSize; // How many bits can be stored in the payload, 0 = variable sized.
        
        /*
         MagicID for the Stream format
         ChunkIDs
         Payload size like in MPEG Transport Stream being 188 bytes per payload
         a PayloadSize of 0 means variable sized payloads

         What do we do with basic formats like BMP/PNM/WAV/AIF?

         and what about formats that use their own 1 off stream format like FLAC?
         */
    } OVIAStream;

    typedef struct OVIADemuxer {
        const OVIA_MagicIDs    *MagicID;
        const OVIA_MediaTypes   MediaType;
    } OVIADemuxer;

    typedef struct OVIAMuxer {
        const OVIA_MediaTypes   MediaType;
        const OVIA_ContainerIDs ContainerID;
    } OVIAMuxer;

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_StreamIO_H */

