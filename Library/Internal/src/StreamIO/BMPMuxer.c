#include "../../include/libPCM.h"
#include "../../include/Private/libPCMTypes.h"
#include "../../include/Private/Image/BMPCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void SetBMPParameters(PCMFile *PCM, uint32_t Height, uint32_t Width, uint8_t NumChannels, uint16_t BitDepth) {
        PCM->BMP->Height      = Height;
        PCM->BMP->Width       = Width;
        PCM->BMP->BitDepth    = BitDepth;
        PCM->BMP->NumChannels = NumChannels;
        PCM->BMP->Planes      = 1;
    }
    
    void BMPWriteHeader(PCMFile *PCM, BitBuffer *BitB, uint32_t NumPixels) {
        WriteBits(BitIOMSByte, BitIOLSBit, BitB, 16, BMP_BM);
        uint32_t FileSize = 2 + 40 + Bits2Bytes(PCM->BMP->NumChannels * NumPixels, true); // Plus the various headers and shit.
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, FileSize);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 16, 0); // Reserved1
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 16, 0); // Reserved2
        uint32_t PixelOffset = 0;
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, PixelOffset);
        uint32_t DIBHeaderSize = 40;
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, DIBHeaderSize);
        /* Write DIB Header */
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, PCM->BMP->Width);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, PCM->BMP->Height);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 16, PCM->BMP->Planes);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 16, PCM->BMP->BitDepth);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, PCM->BMP->CompressionType);
        PCM->BMP->ImageSize = Bits2Bytes(NumPixels * PCM->BMP->BitDepth, No);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, PCM->BMP->ImageSize);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, PCM->BMP->WidthPixelsPerMeter);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, PCM->BMP->HeightPixelsPerMeter);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, PCM->BMP->NumColorsInIndexUsed);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, PCM->BMP->NumImportantColorsInIndex);
    }
    
    void BMPInsertPixels(PCMFile *PCM, BitBuffer *OutputPixels, uint32_t NumPixels2Write, uint16_t **Pixels2Write) {
        if (PCM == NULL) {
            BitIOLog(LOG_ERROR, libPCMLibraryName, __func__, "PCM Pointer is NULL");
        } else if (OutputPixels == NULL) {
            BitIOLog(LOG_ERROR, libPCMLibraryName, __func__, "BitBuffer Pointer is NULL");
        } else {
            uint64_t ChannelCount = PCM->BMP->NumChannels;
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
