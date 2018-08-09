#include "../../../include/Private/Audio/WAVCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    static const uint8_t WAVNULLBinaryGUID[BinaryGUUIDSize] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    
    void WAVWriteHeader(PCMFile *PCM, BitBuffer *BitB) {
        
    }
    
    static void WAVWriteFMTChunk(PCMFile *PCM, BitBuffer *BitB) {
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, 40); // ChunkSize
        WriteBits(LSByteFirst, LSBitFirst, BitB, 16, 0xFFFE); // WaveFormatExtensible
        WriteBits(LSByteFirst, LSBitFirst, BitB, 16, PCM->NumChannels);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, PCM->Aud->SampleRate);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, (PCM->Aud->SampleRate * PCM->NumChannels * PCM->BitDepth) / 8);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, PCM->Aud->BlockAlignment);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 16, PCM->BitDepth);
        uint8_t CBSize = 46;
        WriteBits(LSByteFirst, LSBitFirst, BitB, 16, CBSize);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 16, PCM->BitDepth); // ValidBitsPerSample
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, PCM->Aud->ChannelMask);
        WriteGUUID(GUIDString, BitB, WAVNULLBinaryGUID);
    }
    
    static void WAVWriteLISTChunk(PCMFile *PCM, BitBuffer *BitB) {
        if (PCM->Aud->Meta->NumTags > 0) {
            // Start checking for tags to write
        }
    }
    
    void WAVInsertSamples(PCMFile *PCM, BitBuffer *OutputSamples, uint32_t NumSamples2Write, uint32_t **Samples2Write) {
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
