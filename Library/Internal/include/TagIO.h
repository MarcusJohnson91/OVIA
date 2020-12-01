/*!
 @header              TagIO.h
 @author              Marcus Johnson
 @copyright           2020+
 @version             1.0.0
 @brief               This header contains code for Tag conversion (e.g. ID3 to Vorbis and vice versa).
 */

#include "Private/MediaIO.h"

#pragma once

#ifndef OVIA_TagIO_H
#define OVIA_TagIO_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef enum TagIO_TagTypes {
        TagType_Unspecified      = 0,
        TagType_SpokenLanguage   = 1,
        TagType_WrittenLanguage  = 2,
        TagType_Lyrics           = 3,
        TagType_Custom           = 4,
        TagType_Title            = 5,
        TagType_Artist           = 6,
        TagType_Author           = 7,
        TagType_Composer         = 8,
        TagType_Album            = 9,
        TagType_Year             = 10,
        TagType_Date             = 11,
        TagType_Genre            = 12,
        TagType_BeatsPM          = 13, // BPM beats per minute
        TagType_Track            = 14,
        TagType_NumTracks        = 15,
        TagType_Disc             = 16,
        TagType_NumDiscs         = 17,
        TagType_Copyright        = 18,
        TagType_Publisher        = 19,
        TagType_EncodingSoftware = 20,
        TagType_Comment          = 21,
    } TagIO_TagTypes;

    typedef struct TagIO_Tag {
        UTF32          *TagValue;
        TagIO_TagTypes  TagType;
        OVIA_MediaTypes MediaType;
    } TagIO_Tag;

    typedef struct TagIO_Tags {
        TagIO_Tag **Tags;
        uint64_t    NumOccupiedTags;
        uint64_t    TotalTags;
    } TagIO_Tags;

    TagIO_Tags *TagIO_Init(uint64_t NumTags);

    void        TagIO_Expand(TagIO_Tags *Tags, uint64_t NumTags2Add);

    uint64_t    TagIO_GetNextUnusedIndex(TagIO_Tags *Tags);

    void        TagIO_Shrink(TagIO_Tags *Tags);

    void        TagIO_Deinit(TagIO_Tags *Tags);

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_TagIO_H */
