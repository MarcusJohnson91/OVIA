#include "../../include/libPCM.h"
#include "../../include/Private/libPCMTypes.h"
#include "../../include/Private/Audio/AIFCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void ParseAIFMetadata(PCMFile *PCM, BitBuffer *BitB) {
        bool SSNDFound = false;
        
        PCM->AUD->FileSize                  = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
        BitBufferSkip(BitB, 32);
        while (SSNDFound == false) {
            for (uint64_t FileByte = 0; FileByte < PCM->AUD->FileSize; FileByte++) {
                // Loop over all chunks until the SSND one is found
                uint32_t AIFFChunkID           = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                if (AIFFChunkID == AIF_NAME) {
                    ParseAIFNameChunk(PCM, BitB);
                } else if (AIFFChunkID == AIF_COMM) {
                    uint32_t AIFFCommSize      = Bytes2Bits(ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32));
                    PCM->AUD->NumChannels           = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
                    PCM->AUD->NumSamples            = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                    PCM->AUD->BitDepth              = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
                    PCM->AUD->SampleRate            = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
                } else if (AIFFChunkID == AIF_SSND) { // samples
                    SSNDFound                  = true;
                    uint32_t AIFFSSNDSize      = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                    uint32_t AIFFSSNDOffset    = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                    uint32_t AIFFSSNDBlockSize = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                    BitBufferSkip(BitB, Bytes2Bits(AIFFSSNDOffset));
                }
            }
        }
    }
    
    void ParseAIFNameChunk(PCMFile *PCM, BitBuffer *BitB) {
        uint32_t AIFFNameSize           = Bytes2Bits(ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32));
        PCM->AUD->Meta->SongTitleTag    = ReadBits(BitIOLSByte, BitIOLSBit, BitB, AIFFNameSize);
    }
    
    uint32_t **AIFExtractSamples(PCMFile *PCM, BitBuffer *BitB, uint64_t Samples2Extract) {
        uint64_t ExtractedSampleSize = Samples2Extract * PCM->AUD->NumChannels * PCM->AUD->BitDepth;
        uint32_t **ExtractedSamples = calloc(1, ExtractedSampleSize);
        if (ExtractedSamples == NULL) {
            Log(LOG_ERR, "libPCM", "AIFExtractSamples", "Not enough memory to allocate a buffer for the extracted samples, %d", ExtractedSampleSize);
        } else {
            
        }
        return ExtractedSamples;
        // Now skip padding when you extract samples, and block alignment too.
    }
    
#ifdef __cplusplus
}
#endif
