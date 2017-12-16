#include "../../../Dependencies/BitIO/libBitIO/include/BitIO.h"
#include "../../../Dependencies/BitIO/libBitIO/include/BitIOLog.h"

#include "../../include/libPCM.h"
#include "../../include/Private/libPCMTypes.h"
#include "../../include/Private/Image/PXMCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    static void PXMWritePAMHeader(PCMFile *PCM, BitBuffer *BitB) {
        
    }
    
    static void PXMWriteBinaryPNMHeader(PCMFile *PCM, BitBuffer *BitB) {
        
    }
    
    static void PXMWriteASCIIPNMHeader(PCMFile *PCM, BitBuffer *BitB) {
        
    }
    
    void PXMWriteHeader(PCMFile *PCM, BitBuffer *BitB) {
        if (PCM->PIC->PXMType == PAMPXM) {
            PXMWritePAMHeader(PCM, BitB);
        } else if (PCM->PIC->PXMType == BinaryPXM) {
            PXMWriteBinaryPNMHeader(PCM, BitB);
        } else if (PCM->PIC->PXMType == ASCIIPXM) {
            PXMWriteASCIIPNMHeader(PCM, BitB);
        }
    }
    
    void PXMInsertPixels(PCMFile *PCM, BitBuffer *OutputPixels, uint32_t NumPixels2Write, uint16_t **Pixels2Write) {
        if (PCM == NULL) {
            BitIOLog(BitIOLog_ERROR, libPCMLibraryName, __func__, "PCM Pointer is NULL");
        } else if (OutputPixels == NULL) {
            BitIOLog(BitIOLog_ERROR, libPCMLibraryName, __func__, "BitBuffer Pointer is NULL");
        } else {
            uint64_t ChannelCount = PCM->NumChannels;
            for (uint32_t Pixel = 0; Pixel < NumPixels2Write; Pixel++) {
                for (uint16_t Channel = 0; Channel < ChannelCount; Channel++) {
                    WriteBits(BitIOMSByte, BitIOMSBit, OutputPixels, ChannelCount, Pixels2Write[Channel][Pixel]);
                }
            }
        }
    }
    
#ifdef __cplusplus
}
#endif
