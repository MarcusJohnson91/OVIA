/*!
 @header              FLACNativeStream.h
 @author              Marcus Johnson
 @copyright           2020+
 @version             1.0.0
 @brief               This header contains code for FLACNative streams.
 */

#include "StreamIO.h"

#pragma once

#ifndef OVIA_StreamIO_FLACNativeStream_H
#define OVIA_StreamIO_FLACNativeStream_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    /*
     Ok, so an FLACNative stream can contain FLAC.

     It starts with the FLACMagic.

     it contains various Blocks.

     then the actual audio data.
     */

    /*
     NativeFLAC Extension: .flac, .fla, 
     */

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
        .ByteOrder         = ByteOrder_LSByteIsNearest,
        .BitOrder          = BitOrder_LSBitIsFarthest,
    };

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_StreamIO_FLACNativeStream_H */
