#include "../include/AIFCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void ParseAIFNAME(BitInput *BitI, AIFHeader *AIF, uint32_t ChunkSize) {
        char *Name = calloc(ChunkSize, 1);
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Name[Byte] = ReadBits(BitI, 8);
        }
        AIF->Meta->SongTitleTag = Name;
        AIF->Meta->NumTags += 1;
    }
    
    void ParseAIFCOMM(BitInput *BitI, AIFHeader *AIF, uint32_t ChunkSize) {
        AIF->Channels  = ReadBits(BitI, 16);
        AIF->NumFrames = ReadBits(BitI, 32);
        AIF->BitDepth  = ReadBits(BitI, 16);
    }
    
    void ParseAIFSNSD(BitInput *BitI, AIFHeader *AIF, uint32_t ChunkSize) {
        AIF->Offset    = ReadBits(BitI, 32);
        AIF->BlockSize = ReadBits(BitI, 32);
        SkipBits(BitI, Bytes2Bits(AIF->BlockSize)); // Skip Offset bytes for alignment
    }
    
#ifdef __cplusplus
}
#endif
