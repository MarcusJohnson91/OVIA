#include "../../include/TagIO/AIFTags.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/TextIO/StringIO.h"


#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    void AIF_Read_Name(TagIO_Tags *Tags, BitBuffer *BitB) {
        AssertIO(Tags != NULL);
        AssertIO(BitB != NULL);
        uint64_t StringSize            = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32);
        uint64_t Index                 = TagIO_GetNextUnusedIndex(Tags);
        UTF8    *String                = BitBuffer_ReadUTF8(BitB, StringSize);
        Tags->Tags[Index]->TagValue    = UTF8_Decode(String);
        Tags->Tags[Index]->TagType     = TagType_Title;
        UTF8_Deinit(String);
    }
    
    void AIF_Read_Author(TagIO_Tags *Tags, BitBuffer *BitB) {
        AssertIO(Tags != NULL);
        AssertIO(BitB != NULL);
        uint64_t StringSize            = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32);
        uint64_t Index                 = TagIO_GetNextUnusedIndex(Tags);
        UTF8    *String                = BitBuffer_ReadUTF8(BitB, StringSize);
        Tags->Tags[Index]->TagValue    = UTF8_Decode(String);
        Tags->Tags[Index]->TagType     = TagType_Artist;
        UTF8_Deinit(String);
    }
    
    void AIF_Read_Annotation(TagIO_Tags *Tags, BitBuffer *BitB) {
        AssertIO(Tags != NULL);
        AssertIO(BitB != NULL);
        uint64_t StringSize            = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32);
        uint64_t Index                 = TagIO_GetNextUnusedIndex(Tags);
        UTF8    *String                = BitBuffer_ReadUTF8(BitB, StringSize);
        Tags->Tags[Index]->TagValue    = UTF8_Decode(String);
        Tags->Tags[Index]->TagType     = TagType_Comment;
        UTF8_Deinit(String);
    }
    
    void AIF_Read_Comment(TagIO_Tags *Tags, BitBuffer *BitB) {
        AssertIO(Tags != NULL);
        AssertIO(BitB != NULL);
        uint64_t StringSize            = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32);
        uint64_t Index                 = TagIO_GetNextUnusedIndex(Tags);
        UTF8    *String                = BitBuffer_ReadUTF8(BitB, StringSize);
        Tags->Tags[Index]->TagValue    = UTF8_Decode(String);
        Tags->Tags[Index]->TagType     = TagType_Comment;
        UTF8_Deinit(String);
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
