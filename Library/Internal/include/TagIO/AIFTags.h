/*!
 @header              AIFTags.h
 @author              Marcus Johnson
 @copyright           2020+
 @version             1.0.0
 @brief               This header contains code for AIF tags.
 */

#pragma once

#ifndef OVIA_TagIO_AIFTags_H
#define OVIA_TagIO_AIFTags_H

#include "TagIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef struct BitBuffer BitBuffer;

    void AIF_Read_Name(TagIO_Tags *Tags, BitBuffer *BitB);

    void AIF_Read_Author(TagIO_Tags *Tags, BitBuffer *BitB);

    void AIF_Read_Annotation(TagIO_Tags *Tags, BitBuffer *BitB);

    void AIF_Read_Comment(TagIO_Tags *Tags, BitBuffer *BitB);

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_TagIO_AIFTags_H */
