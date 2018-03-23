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
        PCM->Pic->Height = Height;
        PCM->Pic->Width  = Width;
        PCM->BitDepth    = BitDepth;
        PCM->NumChannels = NumChannels;
    }
    
    void BMPWriteHeader(PCMFile *PCM, BitBuffer *BitB, uint32_t NumPixels) {
        WriteBits(MSByteFirst, LSBitFirst, BitB, 16, BMP_BM);
        PCM->FileSize = 2 + 40 + Bits2Bytes(PCM->NumChannels * NumPixels, true); // Plus the various headers and shit.
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, PCM->FileSize);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 16, 0); // Reserved1
        WriteBits(LSByteFirst, LSBitFirst, BitB, 16, 0); // Reserved2
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, PCM->Pic->BMPPixelOffset);
        uint32_t DIBHeaderSize = 40;
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, DIBHeaderSize);
        /* Write DIB Header */
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, PCM->Pic->Width);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, PCM->Pic->Height);
        uint16_t NumPlanes = 1; // Constant
        WriteBits(LSByteFirst, LSBitFirst, BitB, 16, NumPlanes);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 16, PCM->BitDepth);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, PCM->Pic->BMPCompressionType);
        uint32_t ImageSize = Bits2Bytes(NumPixels * PCM->BitDepth, No);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, ImageSize);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, PCM->Pic->BMPWidthPixelsPerMeter);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, PCM->Pic->BMPHeightPixelsPerMeter);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, PCM->Pic->BMPColorsIndexed);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, PCM->Pic->BMPIndexColorsUsed);
    }
    
    void BMPInsertImage(PCMFile *PCM, BitBuffer *CreatedImage, uint16_t ***Image2Insert) {
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
    
#ifdef __cplusplus
}
#endif
