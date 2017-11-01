#include "../../include/libPCM.h"
#include "../../include/Private/libPCMTypes.h"
#include "../../include/Private/Image/PXMCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void PXMWriteHeader(PCMFile *PCM, BitBuffer *BitB) {
        if (PCM->PXM->PXMType == PAMPXM) {
            
        }
    }
    
    void PXMInsertPixels(PCMFile *PCM, BitBuffer *OutputPixels, uint32_t NumPixels2Write, uint16_t **Pixels2Write) {
        if (PCM == NULL) {
            BitIOLog(LOG_ERROR, "libPCM", "PXMInsertPixels", "PCM Pointer is NULL");
        } else if (OutputPixels == NULL) {
            BitIOLog(LOG_ERROR, "libPCM", "PXMInsertPixels", "BitBuffer Pointer is NULL");
        } else {
            uint64_t ChannelCount = PCM->PXM->NumChannels;
            for (uint16_t Channel = 0; Channel < ChannelCount; Channel++) {
                for (uint32_t Pixel = 0; Pixel < NumPixels2Write; Pixel++) {
                    WriteBits(BitIOMSByte, BitIOMSBit, OutputPixels, ChannelCount, Pixels2Write[Channel][Pixel]);
                }
            }
        }
    }
    
#ifdef __cplusplus
}
#endif
