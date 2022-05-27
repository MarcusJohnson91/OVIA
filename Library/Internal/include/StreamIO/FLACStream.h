/*!
 @header              FLACStream.h
 @author              Marcus Johnson
 @copyright           2020+
 @version             1.0.0
 @brief               This header contains code for FLACNative streams.
 */

#pragma once

#ifndef OVIA_StreamIO_FLACStream_H
#define OVIA_StreamIO_FLACStream_H

#include "../../../OVIA/include/StreamIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
#ifdef OVIA_StreamIO_FLAC
    extern const OVIA_MagicIDs FLACSignature;
    
    const OVIA_MagicIDs FLACSignature = {
        .NumMagicIDs          = 1,
        .MagicIDs             = {
            OVIA_RegisterSignature(0, 32, (uint8_t[4]){0x66, 0x4C, 0x61, 0x43})
        },
    };
    
    extern const OVIA_Extensions FLACExtensions;

    const OVIA_Extensions FLACExtensions = {
        .NumExtensions     = 1,
        .Extensions        = {
            OVIA_RegisterExtension("flac")
        }
    };

    extern const OVIA_MIMETypes FLACMIMETypes;
    
    const OVIA_MIMETypes FLACMIMETypes = {
        .NumMIMETypes     = 1,
        .MIMETypes        = {
            OVIA_RegisterMIMEType("audio/flac")
        }
    };
    
#if defined(OVIA_StreamIO_Encode)
    extern const OVIA_Stream FLACMuxer;
    
    const OVIA_Stream FLACMuxer = {
        .MagicID = &FLACSignature,
    };
#endif /* OVIA_StreamIO_Encode */
    
#if defined(OVIA_StreamIO_Decode)
    extern const OVIA_Stream FLACDemuxer;
    
    const OVIA_Stream FLACDemuxer = {
        .MagicID = &FLACSignature,
    };
#endif /* OVIA_StreamIO_Decode */
    
#endif /* FLAC Literals */

    /*
     Ok, so an FLACNative stream can contain FLAC.

     It starts with the FLACMagic.

     it contains various Blocks.

     then the actual audio data.
     */

    /*
     NativeFLAC Extension: .flac, .fla, 
     */

    static const OVIA_Stream NativeFLACInfo = {
        .SyncType          = SyncType_Packet | SyncType_Marker, // NativeFLAC uses both kinds, the metablocks are Packet based, then the sync codes are for audio frames; the switch happens once the IsLastMetaBlock bit is set, which is the first bit.
                                                                // How do we handle formats that need to extract multiple pieces of information?
                                                                // Packet formats tend to use flag bits.
        /*
         Logic: If SyncType is both Packet and Marker, we need to know how to detect the switch over
         */
        .PacketType        = PacketType_Variable,
        .PacketSizeInBytes = 0,
        .MagicID           = &FLACSignature,
        .OffsetInBits      = 1,
        .FieldSizeInBits   = 7,
        .ByteOrder         = ByteOrder_MSByteIsRight,
        .BitOrder          = BitOrder_MSBitIsLeft,
    };

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_StreamIO_FLACStream_H */
