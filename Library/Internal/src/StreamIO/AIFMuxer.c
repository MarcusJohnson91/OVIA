#include "../../include/libPCM.h"
#include "../../include/Private/libPCMTypes.h"
#include "../../include/Private/Audio/AIFCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void WriteAIFHeader(PCMFile *PCM, BitBuffer *BitB) {
        
    }
    
    void AIFInsertSamples(PCMFile *PCM, BitBuffer *OutputSamples, uint32_t NumSamples2Write, uint32_t **Samples2Write) {
        if (PCM == NULL) {
            BitIOLog(LOG_ERROR, "libPCM", "AIFInsertSamples", "PCM Pointer is NULL");
        } else if (OutputSamples == NULL) {
            BitIOLog(LOG_ERROR, "libPCM", "AIFInsertSamples", "BitBuffer Pointer is NULL");
        } else {
            uint64_t ChannelCount = PCM->AUD->NumChannels;
            uint64_t BitDepth     = PCM->AUD->BitDepth;
            for (uint32_t Sample = 0; Sample < NumSamples2Write; Sample++) {
                for (uint16_t Channel = 0; Channel < ChannelCount; Channel++) {
                    WriteBits(BitIOMSByte, BitIOMSBit, OutputSamples, BitDepth, Samples2Write[Channel][Sample]);
                    BitBufferSkip(OutputSamples, 8 - (BitDepth % 8));
                }
            }
        }
    }
    
#ifdef __cplusplus
}
#endif
