#include "../../../Dependencies/FoundationIO/libFoundationIO/include/BitIO.h"
#include "../../../Dependencies/FoundationIO/libFoundationIO/include/Log.h"

#include "../../include/libPCM.h"
#include "../../include/Private/libPCMTypes.h"
#include "../../include/Private/Audio/W64Common.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    // To encode W64 i'll need to read the values from the struct and write it to the file
    
    static uint64_t CalculateW64ByteRate(const uint64_t NumChannels, const uint64_t SampleRate, const uint8_t BitDepth) {
        return NumChannels * SampleRate * (BitDepth / 8);
    }
    
    static uint64_t CalculateW64BlockAlign(const uint64_t NumChannels, const uint8_t BitDepth) {
        return NumChannels * (BitDepth / 8);
    }
    
    static void W64WriteFMTChunk(PCMFile *PCM, BitBuffer *BitB) {
        uint64_t ByteRate   = CalculateW64ByteRate(PCM->NumChannels, PCM->Aud->SampleRate, PCM->BitDepth);
        uint64_t BlockAlign = CalculateW64BlockAlign(PCM->NumChannels, PCM->BitDepth);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 16, 0);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 16, PCM->NumChannels);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, PCM->Aud->SampleRate);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, ByteRate);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, BlockAlign);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 16, PCM->BitDepth);
    }
    
    void W64WriteHeader(PCMFile *PCM, BitBuffer *BitB) {
        WriteGUUID(GUIDString, BitB, W64_RIFF_GUIDString);
        // Write the size of the file including all header fields
        uint64_t W64Size = (PCM->NumChannelAgnosticSamples * PCM->NumChannels * PCM->BitDepth);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 64, W64Size);
        WriteGUUID(GUIDString, BitB, W64_WAVE_GUIDString);
        WriteGUUID(GUIDString, BitB, W64_FMT_GUIDString);
        uint64_t FMTSize = 40;
        WriteBits(LSByteFirst, LSBitFirst, BitB, 64, FMTSize);
        W64WriteFMTChunk(PCM, BitB);
        WriteGUUID(GUIDString, BitB, W64_DATA_GUIDString);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 64, PCM->NumChannelAgnosticSamples);
    }
    
    void W64InsertSamples(PCMFile *PCM, BitBuffer *OutputSamples, uint32_t NumSamples2Write, uint32_t **Samples2Write) {
        if (PCM != NULL && OutputSamples != NULL && Samples2Write != NULL) {
            uint64_t ChannelCount = PCM->NumChannels;
            uint64_t BitDepth     = PCM->BitDepth;
            for (uint32_t Sample = 0; Sample < NumSamples2Write; Sample++) {
                for (uint16_t Channel = 0; Channel < ChannelCount; Channel++) {
                    WriteBits(LSByteFirst, LSBitFirst, OutputSamples, BitDepth, Samples2Write[Channel][Sample]);
                }
            }
        } else if (PCM == NULL) {
            Log(Log_ERROR, __func__, U8("PCM Pointer is NULL"));
        } else if (OutputSamples == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (Samples2Write == NULL) {
            Log(Log_ERROR, __func__, U8("Samples2Write Pointer is NULL"));
        }
    }
    
#ifdef __cplusplus
}
#endif
