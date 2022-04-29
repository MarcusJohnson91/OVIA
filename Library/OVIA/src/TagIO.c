#include "../include/TagIO.h"                                            /* Included for our declarations */

#include "../../../Dependencies/FoundationIO/Library/include/AssertIO.h" /* Included for Assertions */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

#ifdef    OVIA_TagIO_AIF
#include "../../Internal/include/TagIO/AIFTags.h"
#endif /* OVIA_TagIO_AIF */

#ifdef OVIA_TagIO_APE
#include "../../Internal/include/TagIO/APETags.h"
#endif /* OVIA_TagIO_APE */

#ifdef OVIA_TagIO_FLAC
#include "../../Internal/include/TagIO/FLACTags.h"
#endif /* OVIA_TagIO_FLAC */

#ifdef OVIA_TagIO_ID3
#include "../../Internal/include/TagIO/ID3Tags.h"
#endif /* OVIA_TagIO_ID3 */

#ifdef OVIA_TagIO_WAV
#include "../../Internal/include/TagIO/RIFFTags.h"
#endif /* OVIA_TagIO_WAV */

    TagIO_Tags *TagIO_Init(uint64_t NumTags) {
        AssertIO(NumTags > 0);
        TagIO_Tags *Tags    = calloc(1, sizeof(TagIO_Tags));
        AssertIO(Tags != NULL);
        Tags->Tags          = calloc(NumTags, sizeof(TagIO_Tag));
        AssertIO(Tags->Tags != NULL);
        Tags->TotalTags = NumTags;
        return Tags;
    }

    void TagIO_Expand(TagIO_Tags *Tags, uint64_t NumTags2Add) {
        AssertIO(Tags != NULL);
        AssertIO(NumTags2Add > 0);
        TagIO_Tag **OriginalTags = Tags->Tags;
        TagIO_Tag **NewTags      = realloc(Tags->Tags, (Tags->NumOccupiedTags + NumTags2Add) * sizeof(TagIO_Tag*));
        AssertIO(NewTags != NULL);
        Tags->Tags           = NewTags;
        Tags->TotalTags     += NumTags2Add;
        free(OriginalTags);
    }

    uint64_t TagIO_GetNextUnusedIndex(TagIO_Tags *Tags) {
        AssertIO(Tags != NULL);
        uint64_t Index = 0ULL;
        if (Tags->NumOccupiedTags < Tags->TotalTags) {
            Index = Tags->NumOccupiedTags + 1;
        } else {
            TagIO_Expand(Tags, 4); // 4 seems like a good choice
            Index = Tags->NumOccupiedTags + 1;
        }
        return Index;
    }

    void TagIO_Shrink(TagIO_Tags *Tags) {
        AssertIO(Tags != NULL);
        AssertIO(Tags->NumOccupiedTags < Tags->TotalTags);
        TagIO_Tag **OriginalTags = Tags->Tags;
        TagIO_Tag **NewTags      = realloc(Tags->Tags, Tags->NumOccupiedTags * sizeof(TagIO_Tag*));
        AssertIO(NewTags != NULL);
        if (NewTags != NULL) {
            Tags->Tags           = NewTags;
            Tags->TotalTags     += Tags->NumOccupiedTags;
            free(OriginalTags);
        }
    }

    void TagIO_Deinit(TagIO_Tags *Tags) {
        AssertIO(Tags != NULL);
        for (uint64_t Tag = 0; Tag < Tags->NumOccupiedTags; Tag++) {
            free(Tags->Tags[Tag]->TagValue);
        }
        free(Tags->Tags);
        free(Tags);
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
