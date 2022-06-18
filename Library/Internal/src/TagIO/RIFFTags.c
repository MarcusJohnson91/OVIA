#include "../../include/TagIO/RIFFTags.h"
#include "../../include/StreamIO/WAVStream.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/TextIO/StringIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/TextIO/LogIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    /*
     This all needs to be Rewritten to use the TagIO API.
     */
    static void WAV_Read_INFO_TRCK(TagIO_Tags *Tags, BitBuffer *BitB) {
        AssertIO(Tags != NULL);
        AssertIO(BitB != NULL);
        uint64_t StringSize            = BitBuffer_GetUTF8StringSize(BitB);
        uint64_t Index                 = TagIO_GetNextUnusedIndex(Tags);
        UTF8    *String                = BitBuffer_ReadUTF8(BitB, StringSize);
        Tags->Tags[Index]->TagValue    = UTF8_Decode(String);
        Tags->Tags[Index]->TagType     = TagType_NumTracks;
        UTF8_Deinit(String);
    }
    
    static void WAV_Read_INFO_IPRT(TagIO_Tags *Tags, BitBuffer *BitB) {
        AssertIO(Tags != NULL);
        AssertIO(BitB != NULL);
        uint64_t StringSize            = BitBuffer_GetUTF8StringSize(BitB);
        uint64_t Index                 = TagIO_GetNextUnusedIndex(Tags);
        UTF8    *String                = BitBuffer_ReadUTF8(BitB, StringSize);
        Tags->Tags[Index]->TagValue    = UTF8_Decode(String);
        Tags->Tags[Index]->TagType     = TagType_Track;
        UTF8_Deinit(String);
    }
    
    static void WAV_Read_INFO_IART(TagIO_Tags *Tags, BitBuffer *BitB) {
        AssertIO(Tags != NULL);
        AssertIO(BitB != NULL);
        uint64_t StringSize            = BitBuffer_GetUTF8StringSize(BitB);
        uint64_t Index                 = TagIO_GetNextUnusedIndex(Tags);
        UTF8    *String                = BitBuffer_ReadUTF8(BitB, StringSize);
        Tags->Tags[Index]->TagValue    = UTF8_Decode(String);
        Tags->Tags[Index]->TagType     = TagType_Artist;
        UTF8_Deinit(String);
    }
    
    static void WAV_Read_INFO_ICRD(TagIO_Tags *Tags, BitBuffer *BitB) {
        AssertIO(Tags != NULL);
        AssertIO(BitB != NULL);
        uint64_t StringSize            = BitBuffer_GetUTF8StringSize(BitB);
        uint64_t Index                 = TagIO_GetNextUnusedIndex(Tags);
        UTF8    *String                = BitBuffer_ReadUTF8(BitB, StringSize);
        Tags->Tags[Index]->TagValue    = UTF8_Decode(String);
        Tags->Tags[Index]->TagType     = TagType_Date;
        UTF8_Deinit(String);
    }
    
    static void WAV_Read_INFO_IGNR(TagIO_Tags *Tags, BitBuffer *BitB) {
        AssertIO(Tags != NULL);
        AssertIO(BitB != NULL);
        uint64_t StringSize            = BitBuffer_GetUTF8StringSize(BitB);
        uint64_t Index                 = TagIO_GetNextUnusedIndex(Tags);
        UTF8    *String                = BitBuffer_ReadUTF8(BitB, StringSize);
        Tags->Tags[Index]->TagValue    = UTF8_Decode(String);
        Tags->Tags[Index]->TagType     = TagType_Genre;
        UTF8_Deinit(String);
    }
    
    static void WAV_Read_INFO_INAM(TagIO_Tags *Tags, BitBuffer *BitB) {
        AssertIO(Tags != NULL);
        AssertIO(BitB != NULL);
        uint64_t StringSize            = BitBuffer_GetUTF8StringSize(BitB);
        uint64_t Index                 = TagIO_GetNextUnusedIndex(Tags);
        UTF8    *String                = BitBuffer_ReadUTF8(BitB, StringSize);
        Tags->Tags[Index]->TagValue    = UTF8_Decode(String);
        Tags->Tags[Index]->TagType     = TagType_Title;
        UTF8_Deinit(String);
    }
    
    static void WAV_Read_INFO_IPRD(TagIO_Tags *Tags, BitBuffer *BitB) {
        AssertIO(Tags != NULL);
        AssertIO(BitB != NULL);
        uint64_t StringSize            = BitBuffer_GetUTF8StringSize(BitB);
        uint64_t Index                 = TagIO_GetNextUnusedIndex(Tags);
        UTF8    *String                = BitBuffer_ReadUTF8(BitB, StringSize);
        Tags->Tags[Index]->TagValue    = UTF8_Decode(String);
        Tags->Tags[Index]->TagType     = TagType_Album;
        UTF8_Deinit(String);
    }
    
    static void WAV_Read_INFO_ISFT(TagIO_Tags *Tags, BitBuffer *BitB) { // Encoder
        AssertIO(Tags != NULL);
        AssertIO(BitB != NULL);
        uint64_t StringSize            = BitBuffer_GetUTF8StringSize(BitB);
        uint64_t Index                 = TagIO_GetNextUnusedIndex(Tags);
        UTF8    *String                = BitBuffer_ReadUTF8(BitB, StringSize);
        Tags->Tags[Index]->TagValue    = UTF8_Decode(String);
        Tags->Tags[Index]->TagType     = TagType_EncodingSoftware;
        UTF8_Deinit(String);
    }
    
    static void ReadINFO_ICMT(TagIO_Tags *Tags, BitBuffer *BitB) {
        AssertIO(Tags != NULL);
        AssertIO(BitB != NULL);
        uint64_t StringSize            = BitBuffer_GetUTF8StringSize(BitB);
        uint64_t Index                 = TagIO_GetNextUnusedIndex(Tags);
        UTF8    *String                = BitBuffer_ReadUTF8(BitB, StringSize);
        Tags->Tags[Index]->TagValue    = UTF8_Decode(String);
        Tags->Tags[Index]->TagType     = TagType_Comment;
        UTF8_Deinit(String);
    }
    
    static void WAVReadLISTChunk(TagIO_Tags *Tags, BitBuffer *BitB) {
        AssertIO(Tags != NULL);
        AssertIO(BitB != NULL);
        uint32_t SubChunkID     = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32);
        uint32_t SubChunkSize   = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32);
        uint64_t OriginalOffset = BitBuffer_GetPosition(BitB); // After reading each Chunk, make sure the offset is corrected
        uint64_t TargetOffset   = OriginalOffset + Bytes2Bits(SubChunkSize);
        
        switch (SubChunkID) {
            case WAV_IART: // Artist
                WAV_Read_INFO_IART(Tags, BitB);
                break;
            case WAV_ICRD: // Release date
                WAV_Read_INFO_ICRD(Tags, BitB);
                break;
            case WAV_IGNR: // Genre
                WAV_Read_INFO_IGNR(Tags, BitB);
                break;
            case WAV_INAM: // Title
                WAV_Read_INFO_INAM(Tags, BitB);
                break;
            case WAV_IPRD: // Album
                WAV_Read_INFO_IPRD(Tags, BitB);
                break;
            case WAV_ISFT: // Encoder
                WAV_Read_INFO_ISFT(Tags, BitB);
                break;
            case WAV_ICMT:
                ReadINFO_ICMT(Tags, BitB);
                break;
            default:
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Unknown LIST Chunk: 0x%X"), SubChunkID);
                break;
        }
        uint64_t NewOffset = BitBuffer_GetPosition(BitB);
        if (NewOffset < TargetOffset) {
            BitBuffer_Seek(BitB, TargetOffset - NewOffset);
        }
    }
    
    static void WAV_Write_LIST(TagIO_Tags *Tags, BitBuffer *BitB) {
        AssertIO(Tags != NULL);
        AssertIO(BitB != NULL);
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
