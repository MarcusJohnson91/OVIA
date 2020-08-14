/*!
 @header              AIFTags.h
 @author              Marcus Johnson
 @copyright           2020+
 @version             1.0.0
 @brief               This header contains code for AIF tags.
 */

#include "TagIO.h"

#pragma  once

#ifndef OVIA_TagIO_AIFTags_H
#define OVIA_TagIO_AIFTags_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef struct BitBuffer BitBuffer;

    void AIFReadNameChunk(TagIO_Tags *Tags, BitBuffer *BitB);

    void AIFReadAuthorChunk(TagIO_Tags *Tags, BitBuffer *BitB);

    void AIFReadAnnotationChunk(TagIO_Tags *Tags, BitBuffer *BitB);

    void AIFReadCommentChunk(TagIO_Tags *Tags, BitBuffer *BitB);

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_TagIO_AIFTags_H */
