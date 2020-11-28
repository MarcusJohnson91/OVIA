#include "../../include/TagIO.h"
#include "../../include/Private/StreamIO/AIFStream.h"
#include "../../../Dependencies/FoundationIO/Library/include/MathIO.h"
#include "../../../Dependencies/FoundationIO/Library/include/TextIO/StringIO.h"
#include "../../../Dependencies/FoundationIO/Library/include/TextIO/LogIO.h"


#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    void AIFReadNameChunk(TagIO_Tags *Tags, BitBuffer *BitB) {
        if (Tags != NULL && BitB != NULL) {
            uint64_t StringSize            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
            uint64_t Index                 = TagIO_GetNextUnusedIndex(Tags);
            UTF8    *String                = BitBuffer_ReadUTF8(BitB, StringSize);
            Tags->Tags[Index]->TagValue    = UTF8_Decode(String);
            Tags->Tags[Index]->TagType     = TagType_Title;
            UTF8_Deinit(String);
        } else if (Tags == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Tags Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void AIFReadAuthorChunk(TagIO_Tags *Tags, BitBuffer *BitB) {
        if (Tags != NULL && BitB != NULL) {
            uint64_t StringSize            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
            uint64_t Index                 = TagIO_GetNextUnusedIndex(Tags);
            UTF8    *String                = BitBuffer_ReadUTF8(BitB, StringSize);
            Tags->Tags[Index]->TagValue    = UTF8_Decode(String);
            Tags->Tags[Index]->TagType     = TagType_Artist;
            UTF8_Deinit(String);
        } else if (Tags == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Tags Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void AIFReadAnnotationChunk(TagIO_Tags *Tags, BitBuffer *BitB) {
        if (Tags != NULL && BitB != NULL) {
            uint64_t StringSize            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
            uint64_t Index                 = TagIO_GetNextUnusedIndex(Tags);
            UTF8    *String                = BitBuffer_ReadUTF8(BitB, StringSize);
            Tags->Tags[Index]->TagValue    = UTF8_Decode(String);
            Tags->Tags[Index]->TagType     = TagType_Comment;
            UTF8_Deinit(String);
        } else if (Tags == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Tags Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void AIFReadCommentChunk(TagIO_Tags *Tags, BitBuffer *BitB) {
        if (Tags != NULL && BitB != NULL) {
            uint64_t StringSize            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
            uint64_t Index                 = TagIO_GetNextUnusedIndex(Tags);
            UTF8    *String                = BitBuffer_ReadUTF8(BitB, StringSize);
            Tags->Tags[Index]->TagValue    = UTF8_Decode(String);
            Tags->Tags[Index]->TagType     = TagType_Comment;
            UTF8_Deinit(String);
        } else if (Tags == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Tags Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void AIFReadID3Chunk(TagIO_Tags *Tags, BitBuffer *BitB) {
        if (Tags != NULL && BitB != NULL) {
            // Basically just wrap the ID3 reader
        } else if (Tags == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Tags Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
