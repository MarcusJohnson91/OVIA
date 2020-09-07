/*!
 @header              CodecIO.h
 @author              Marcus Johnson
 @copyright           2020+
 @version             1.0.0
 @brief               This header contains code for codecs (encoding and decoding).
 */

#include "../MediaIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/TextIO/LogIO.h"

#pragma once

#ifndef OVIA_CodecIO_H
#define OVIA_CodecIO_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef struct CodecIO_ImageLimitations {
        uint64_t                 MaxWidth;
        uint64_t                 MaxHeight;
        uint8_t                  MaxBitDepth;
        uint8_t                  MaxViews;
        MediaIO_ImageChannelMask SupportedChannels;
    } CodecIO_ImageLimitations;

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_CodecIO_H */
