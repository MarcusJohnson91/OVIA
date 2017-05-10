#include "../../Dependencies/BitIO/libBitIO/include/BitIO.h"

#include "../include/libPCM.h"
#include "../include/PCMTypes.h"
#include "../include/AIFCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void ParseAIFNAME(BitInput *BitI, AIFHeader *AIF, uint32_t ChunkSize) {
        char *Name = calloc(1, ChunkSize);
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Name[Byte] = ReadBits(BitI, 8, true);
        }
        AIF->Meta->SongTitleTag = Name;
        AIF->Meta->NumTags += 1;
    }
    
    void ParseAIFCOMM(BitInput *BitI, AIFHeader *AIF, uint32_t ChunkSize) {
        AIF->Channels  = ReadBits(BitI, 16, true);
        AIF->NumFrames = ReadBits(BitI, 32, true);
        AIF->BitDepth  = ReadBits(BitI, 16, true);
    }
    
    void ParseAIFSNSD(BitInput *BitI, AIFHeader *AIF, uint32_t ChunkSize) {
        AIF->Offset    = ReadBits(BitI, 32, true);
        AIF->BlockSize = ReadBits(BitI, 32, true);
        SkipBits(BitI, Bytes2Bits(AIF->BlockSize)); // Skip Offset bytes for alignment
    }
    
#ifdef __cplusplus
}
#endif
