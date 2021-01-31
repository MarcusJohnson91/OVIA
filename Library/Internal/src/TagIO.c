#include "../../../Dependencies/FoundationIO/Library/include/PlatformIO.h"

#ifdef OVIA_TagIO_AIF
#endif /* OVIA_TagIO_AIF */

#ifdef OVIA_EnableAIF
#include "../../include/Private/StreamIO/AIFStream.h"
#endif /* OVIA_EnableAIF */

#ifdef OVIA_StreamIO_BMP
#include "../../include/Private/StreamIO/BMPStream.h"
#endif /* OVIA_StreamIO_BMP */

#ifdef OVIA_StreamIO_PNM
#include "../../include/Private/StreamIO/PNMStream.h"
#endif /* OVIA_StreamIO_PNM */

#ifdef OVIA_EnableRIFF
#include "../../include/Private/StreamIO/RIFFStream.h"
#endif /* OVIA_EnableRIFF */

#ifdef OVIA_StreamIO_OGG
#include "../../include/Private/StreamIO/OGGStream.h"
#endif /* OVIA_StreamIO_OGG */

#ifdef OVIA_StreamIO_FLAC
#include "../../include/Private/StreamIO/FLACNativeStream.h"
#endif /* OVIA_StreamIO_FLAC */

#include "../../include/TagIO.h"
#include "../../../Dependencies/FoundationIO/Library/include/TextIO/LogIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    TagIO_Tags *TagIO_Init(uint64_t NumTags) {
        TagIO_Tags *Tags    = calloc(1, sizeof(TagIO_Tags));
        Tags->Tags          = calloc(NumTags, sizeof(TagIO_Tag));
        if (Tags->Tags != NULL) {
            Tags->TotalTags = NumTags;
        } else {
            TagIO_Deinit(Tags);
        }
        return Tags;
    }

    void TagIO_Expand(TagIO_Tags *Tags, uint64_t NumTags2Add) {
        if (Tags != NULL && NumTags2Add != 0) {
            TagIO_Tag **OriginalTags = Tags->Tags;
            TagIO_Tag **NewTags      = realloc(Tags->Tags, (Tags->NumOccupiedTags + NumTags2Add) * sizeof(TagIO_Tag*));
            if (NewTags != NULL) {
                Tags->Tags           = NewTags;
                Tags->TotalTags     += NumTags2Add;
                free(OriginalTags);
            }
        } else if (Tags == NULL) {
            Log(Severity_USER, PlatformIO_FunctionName, UTF8String("Tags Pointer is NULL"));
        } else if (NumTags2Add == 0) {
            Log(Severity_USER, PlatformIO_FunctionName, UTF8String("NumTags2Add is zero, which doesn't make sense"));
        }
    }

    uint64_t TagIO_GetNextUnusedIndex(TagIO_Tags *Tags) {
        uint64_t Index = 0ULL;
        if (Tags != NULL) {
            if (Tags->NumOccupiedTags < Tags->TotalTags) {
                Index = Tags->NumOccupiedTags + 1;
            } else {
                TagIO_Expand(Tags, 4); // 4 seems like a good choice
                Index = Tags->NumOccupiedTags + 1;
            }
        } else if (Tags == NULL) {
            Log(Severity_USER, PlatformIO_FunctionName, UTF8String("Tags Pointer is NULL"));
        }
        return Index;
    }

    void TagIO_Shrink(TagIO_Tags *Tags) {
        if (Tags != NULL && Tags->NumOccupiedTags < Tags->TotalTags) {
            TagIO_Tag **OriginalTags = Tags->Tags;
            TagIO_Tag **NewTags      = realloc(Tags->Tags, Tags->NumOccupiedTags * sizeof(TagIO_Tag*));
            if (NewTags != NULL) {
                Tags->Tags           = NewTags;
                Tags->TotalTags     += Tags->NumOccupiedTags;
                free(OriginalTags);
            }
        } else if (Tags == NULL) {
            Log(Severity_USER, PlatformIO_FunctionName, UTF8String("Tags Pointer is NULL"));
        }
    }

    void TagIO_Deinit(TagIO_Tags *Tags) {
        if (Tags != NULL) {
            for (uint64_t Tag = 0; Tag < Tags->NumOccupiedTags; Tag++) {
                free(Tags->Tags[Tag]->TagValue);
            }
            free(Tags->Tags);
            free(Tags);
        }
    }


#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
