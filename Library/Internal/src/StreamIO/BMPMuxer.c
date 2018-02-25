#include "../../../Dependencies/FoundationIO/libFoundationIO/include/BitIO.h"
#include "../../../Dependencies/FoundationIO/libFoundationIO/include/Log.h"
#include "../../../Dependencies/FoundationIO/libFoundationIO/include/Math.h"

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
        WriteBits(MSByteFirst, LSBitFirst, BitB, 16, BMP_BM);
        PCM->FileSize = 2 + 40 + Bits2Bytes(PCM->NumChannels * NumPixels, true); // Plus the various headers and shit.
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, PCM->FileSize);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 16, 0); // Reserved1
        WriteBits(LSByteFirst, LSBitFirst, BitB, 16, 0); // Reserved2
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, PCM->PIC->BMPPixelOffset);
        uint32_t DIBHeaderSize = 40;
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, DIBHeaderSize);
        /* Write DIB Header */
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, PCM->PIC->Width);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, PCM->PIC->Height);
        uint16_t NumPlanes = 1; // Constant
        WriteBits(LSByteFirst, LSBitFirst, BitB, 16, NumPlanes);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 16, PCM->BitDepth);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, PCM->PIC->BMPCompressionType);
        uint32_t ImageSize = Bits2Bytes(NumPixels * PCM->BitDepth, No);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, ImageSize);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, PCM->PIC->BMPWidthPixelsPerMeter);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, PCM->PIC->BMPHeightPixelsPerMeter);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, PCM->PIC->BMPColorsIndexed);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, PCM->PIC->BMPIndexColorsUsed);
    }
    
    void BMPInsertPixels(PCMFile *PCM, BitBuffer *OutputPixels, uint32_t NumPixels2Write, uint16_t **Pixels2Write) {
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
