/*!
 @header              FLACNativeStream.h
 @author              Marcus Johnson
 @copyright           2020+
 @version             1.0.0
 @brief               This header contains code for FLACNative streams.
 */

#pragma once

#ifndef OVIA_StreamIO_FLACNativeStream_H
#define OVIA_StreamIO_FLACNativeStream_H

#include "StreamIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
#ifdef OVIA_StreamIO_FLAC
    extern const OVIA_MagicIDs FLACSignature;
    
    const OVIA_MagicIDs FLACSignature = {
        .NumMagicIDs          = 1,
        .MagicIDs             = {
            [0]               = {
                .OffsetInBits = 0,
                .SizeInBits   = 32,
                .Signature    = (uint8_t[4]){0x66, 0x4C, 0x61, 0x43},
            },
        },
    };
    
    extern const OVIA_Extensions FLACExtensions;

    const OVIA_Extensions FLACExtensions = {
        .NumExtensions     = 1,
        .Extensions        = {
            [0]            = {
                .Size      = 4,
                .Extension = UTF32String("flac"),
            },
        },
    };

    extern const OVIA_MIMETypes FLACMIMETypes;
    
    const OVIA_MIMETypes FLACMIMETypes = {
        .NumMIMETypes     = 1,
        .MIMETypes        = {
            [0]           = {
                .Size     = 10,
                .MIMEType = UTF32String("audio/flac"),
            },
        },
    };
    
#if defined(OVIA_StreamIO_Encode)
    extern const OVIA_Stream FLACEncoder;
    
    const OVIA_Stream FLACEncoder = {
        .MagicID = &FLACSignature,
    };
#endif /* OVIA_StreamIO_Encode */
    
#if defined(OVIA_StreamIO_Decode)
    extern const OVIA_Stream FLACDecoder;
    
    const OVIA_Stream FLACDecoder = {
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
        .ByteOrder         = ByteOrder_LSByteIsNearest,
        .BitOrder          = BitOrder_LSBitIsFarthest,
    };

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_StreamIO_FLACNativeStream_H */
