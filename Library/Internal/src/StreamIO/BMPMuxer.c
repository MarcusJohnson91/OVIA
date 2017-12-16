#include "../../../Dependencies/BitIO/libBitIO/include/BitIO.h"
#include "../../../Dependencies/BitIO/libBitIO/include/BitIOLog.h"
#include "../../../Dependencies/BitIO/libBitIO/include/BitIOMath.h"

#include "../../include/libPCM.h"
#include "../../include/Private/libPCMTypes.h"
#include "../../include/Private/Image/BMPCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void SetBMPParameters(PCMFile *PCM, uint32_t Height, uint32_t Width, uint8_t NumChannels, uint16_t BitDepth) {
        PCM->PIC->Height = Height;
        PCM->PIC->Width  = Width;
        PCM->BitDepth    = BitDepth;
        PCM->NumChannels = NumChannels;
    }
    
    void BMPWriteHeader(PCMFile *PCM, BitBuffer *BitB, uint32_t NumPixels) {
        WriteBits(BitIOMSByte, BitIOLSBit, BitB, 16, BMP_BM);
        PCM->FileSize = 2 + 40 + Bits2Bytes(PCM->NumChannels * NumPixels, true); // Plus the various headers and shit.
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, PCM->FileSize);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 16, 0); // Reserved1
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 16, 0); // Reserved2
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, PCM->PIC->BMPPixelOffset);
        uint32_t DIBHeaderSize = 40;
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, DIBHeaderSize);
        /* Write DIB Header */
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, PCM->PIC->Width);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, PCM->PIC->Height);
        uint16_t NumPlanes = 1; // Constant
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 16, NumPlanes);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 16, PCM->BitDepth);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, PCM->PIC->BMPCompressionType);
        uint32_t ImageSize = Bits2Bytes(NumPixels * PCM->BitDepth, No);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, ImageSize);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, PCM->PIC->BMPWidthPixelsPerMeter);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, PCM->PIC->BMPHeightPixelsPerMeter);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, PCM->PIC->BMPColorsIndexed);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, PCM->PIC->BMPIndexColorsUsed);
    }
    
    void BMPInsertPixels(PCMFile *PCM, BitBuffer *OutputPixels, uint32_t NumPixels2Write, uint16_t **Pixels2Write) {
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
