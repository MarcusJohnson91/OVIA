#include "../../include/libPCM.h"
#include "../../include/Private/libPCMTypes.h"
#include "../../include/Private/Audio/AIFCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void ParseAIFMetadata(PCMFile *PCM, BitBuffer *BitB) {
        bool SSNDFound = false;
        
        PCM->FileSize                  = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
        SkipBits(BitB, 32);
        while (SSNDFound == false) {
            for (uint64_t FileByte = 0; FileByte < PCM->FileSize; FileByte++) {
                // Loop over all chunks until the SSND one is found
                uint32_t AIFFChunkID           = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32, true);
                if (AIFFChunkID == AIF_NAME) {
                    ParseAIFNameChunk(PCM, BitB);
                } else if (AIFFChunkID == AIF_COMM) {
                    uint32_t AIFFCommSize      = Bytes2Bits(ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32));
                    PCM->NumChannels           = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
                    PCM->NumSamples            = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                    PCM->BitDepth              = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
                    PCM->SampleRate            = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
                } else if (AIFFChunkID == AIF_SSND) { // samples
                    SSNDFound                  = true;
                    uint32_t AIFFSSNDSize      = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                    uint32_t AIFFSSNDOffset    = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                    uint32_t AIFFSSNDBlockSize = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                    SkipBits(BitB, Bytes2Bits(AIFFSSNDOffset));
                }
            }
        }
    }
    
    void ParseAIFNameChunk(PCMFile *PCM, BitBuffer *BitB) {
        uint32_t AIFFNameSize      = Bytes2Bits(ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32));
        PCM->Meta->SongTitleTag    = ReadBits(BitIOLSByte, BitIOLSBit, BitB, AIFFNameSize);
    }
    
    void ExtractAIFSamples(PCMFile *PCM, BitBuffer *BitB, uint64_t Samples2Extract) {
        PCM->Samples = calloc(1, Samples2Extract * PCM->NumChannels * PCM->BitDepth);
        // Now skip padding when you extract samples, and block alignment too.
    }
    
#ifdef __cplusplus
}
#endif
