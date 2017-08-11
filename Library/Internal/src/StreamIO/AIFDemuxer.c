#include "../../include/libPCM.h"
#include "../../include/Private/libPCMTypes.h"
#include "../../include/Private/Audio/AIFCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void ParseAIFMetadata(PCMFile *PCM, BitBuffer *BitB) {
        bool SSNDFound = false;
        
        PCM->FileSize                  = ReadBits(BitB, 32, true);
        SkipBits(BitB, 32);
        while (SSNDFound == false) {
            for (uint64_t FileByte = 0; FileByte < PCM->FileSize; FileByte++) {
                // Loop over all chunks until the SSND one is found
                uint32_t AIFFChunkID           = ReadBits(BitB, 32, true);
                if (AIFFChunkID == AIF_NAME) {
                    ParseAIFNameChunk(PCM, BitB);
                } else if (AIFFChunkID == AIF_COMM) {
                    uint32_t AIFFCommSize      = Bytes2Bits(ReadBits(BitB, 32, true));
                    PCM->NumChannels           = ReadBits(BitB, 16, true);
                    PCM->NumSamples            = ReadBits(BitB, 32, true);
                    PCM->BitDepth              = ReadBits(BitB, 16, true);
                    PCM->SampleRate            = ReadBits(BitB, 16, true);
                } else if (AIFFChunkID == AIF_SSND) { // samples
                    SSNDFound                  = true;
                    uint32_t AIFFSSNDSize      = ReadBits(BitB, 32, true);
                    uint32_t AIFFSSNDOffset    = ReadBits(BitB, 32, true);
                    uint32_t AIFFSSNDBlockSize = ReadBits(BitB, 32, true);
                    SkipBits(BitB, Bytes2Bits(AIFFSSNDOffset));
                }
            }
        }
    }
    
    void ParseAIFNameChunk(PCMFile *PCM, BitBuffer *BitB) {
        uint32_t AIFFNameSize      = Bytes2Bits(ReadBits(BitB, 32, true));
        PCM->Meta->SongTitleTag    = ReadBits(BitB, AIFFNameSize, true);
    }
    
    void ExtractAIFSamples(PCMFile *PCM, BitBuffer *BitB, uint64_t Samples2Extract) {
        PCM->Samples = calloc(1, Samples2Extract * PCM->NumChannels * PCM->BitDepth);
        // Now skip padding when you extract samples, and block alignment too.
    }
    
#ifdef __cplusplus
}
#endif
