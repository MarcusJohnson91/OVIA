#include "../../../Dependencies/FoundationIO/libFoundationIO/include/BitIO.h"
#include "../../../Dependencies/FoundationIO/libFoundationIO/include/Log.h"

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
        if (PCM->Pic->PXMType == PAMPXM) {
            PXMWritePAMHeader(PCM, BitB);
        } else if (PCM->Pic->PXMType == BinaryPXM) {
            PXMWriteBinaryPNMHeader(PCM, BitB);
        } else if (PCM->Pic->PXMType == ASCIIPXM) {
            PXMWriteASCIIPNMHeader(PCM, BitB);
        }
    }
    
    void PXMInsertImage(PCMFile *PCM, BitBuffer *CreatedImage, uint16_t ***Image2Insert) {
        if (PCM != NULL && CreatedImage != NULL && Image2Insert != NULL) {
            for (uint64_t Width = 0; Width < PCM->Pic->Width; Width++) {
                for (uint64_t Height = 0; Height < PCM->Pic->Height; Height++) {
                    for (uint16_t Channel = 0; Channel < PCM->NumChannels; Channel++) {
                        WriteBits(MSByteFirst, MSBitFirst, CreatedImage, PCM->BitDepth, Image2Insert[Width][Height][Channel]);
                    }
                }
            }
        } else if (PCM == NULL) {
            Log(Log_ERROR, __func__, U8("PCM Pointer is NULL"));
        } else if (CreatedImage == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (Image2Insert == NULL) {
            Log(Log_ERROR, __func__, U8("Pixels2Write Pointer is NULL"));
        }
    }
    
    void PXMInsertPixels(PCMFile *PCM, BitBuffer *OutputPixels, uint32_t NumPixels2Write, uint16_t **Pixels2Write) {
        if (PCM != NULL && OutputPixels != NULL && Pixels2Write != NULL) {
            uint64_t ChannelCount = PCM->NumChannels;
            for (uint32_t Pixel = 0; Pixel < NumPixels2Write; Pixel++) {
                for (uint16_t Channel = 0; Channel < ChannelCount; Channel++) {
                    WriteBits(MSByteFirst, MSBitFirst, OutputPixels, ChannelCount, Pixels2Write[Channel][Pixel]);
                }
            }
        } else if (PCM == NULL) {
            Log(Log_ERROR, __func__, U8("PCM Pointer is NULL"));
        } else if (OutputPixels == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (Pixels2Write == NULL) {
            Log(Log_ERROR, __func__, U8("Pixels2Write Pointer is NULL"));
        }
    }
    
#ifdef __cplusplus
}
#endif
