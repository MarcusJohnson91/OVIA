/*!
 @header              AIFStream.h
 @author              Marcus Johnson
 @copyright           2020+
 @version             1.0.0
 @brief               This header contains code for AIF streams.
 */

#include "StreamIO.h"

#pragma  once

#ifndef OVIA_StreamIO_AIFStream_H
#define OVIA_StreamIO_AIFStream_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef struct TagIO_Tags TagIO_Tags;

    void *AIFOptions_Init(void);

    void AIFSkipPadding(BitBuffer *BitB, uint32_t SubChunkSize);

    void AIFReadMetadata(TagIO_Tags *Tags, BitBuffer *BitB);

    void AIFOptions_Deinit(void *Options);

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_StreamIO_AIFStream_H */

