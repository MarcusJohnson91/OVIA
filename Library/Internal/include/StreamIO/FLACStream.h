/*!
 @header              FLACNativeStream.h
 @author              Marcus Johnson
 @copyright           2020+
 @version             1.0.0
 @brief               This header contains code for FLACNative streams.
 Has Substreams.
 is multiplexible
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

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_StreamIO_FLACNativeStream_H */
