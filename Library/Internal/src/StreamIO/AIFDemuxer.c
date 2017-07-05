#include "../../Dependencies/BitIO/libBitIO/include/BitIO.h"

#include "../include/libPCM.h"
#include "../include/PCMTypes.h"
#include "../include/AIFCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void ParseAIFNAME(AIFHeader *AIF, BitBuffer *BitB, uint32_t ChunkSize) {
        char *Name = calloc(1, ChunkSize);
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Name[Byte] = ReadBits(BitB, 8, true);
        }
        AIF->Meta->SongTitleTag = Name;
        AIF->Meta->NumTags += 1;
    }
    
    void ParseAIFCOMM(AIFHeader *AIF, BitBuffer *BitB, uint32_t ChunkSize) {
        AIF->Channels  = ReadBits(BitB, 16, true);
        AIF->NumFrames = ReadBits(BitB, 32, true);
        AIF->BitDepth  = ReadBits(BitB, 16, true);
    }
    
    void ParseAIFSNSD(AIFHeader *AIF, BitBuffer *BitB, uint32_t ChunkSize) {
        AIF->Offset    = ReadBits(BitB, 32, true);
        AIF->BlockSize = ReadBits(BitB, 32, true);
        SkipBits(BitB, Bytes2Bits(AIF->BlockSize)); // Skip Offset bytes for alignment
    }
    
#ifdef __cplusplus
}
#endif
