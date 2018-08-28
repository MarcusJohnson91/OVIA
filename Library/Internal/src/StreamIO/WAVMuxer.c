#include "../../../include/Private/Audio/WAVCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    static const uint8_t WAVNULLBinaryGUID[BinaryGUUIDSize] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    
    void WAVWriteHeader(OVIA *Ovia, BitBuffer *BitB) {
        
    }
    
    static void WAVWriteFMTChunk(OVIA *Ovia, BitBuffer *BitB) {
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, 40); // ChunkSize
        WriteBits(LSByteFirst, LSBitFirst, BitB, 16, 0xFFFE); // WaveFormatExtensible
        WriteBits(LSByteFirst, LSBitFirst, BitB, 16, OVIA_GetNumChannels(Ovia));
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, Ovia->Aud->SampleRate);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, (Ovia->Aud->SampleRate * OVIA_GetNumChannels(Ovia) * Ovia->BitDepth) / 8);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, Ovia->Aud->BlockAlignment);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 16, Ovia->BitDepth);
        uint8_t CBSize = 46;
        WriteBits(LSByteFirst, LSBitFirst, BitB, 16, CBSize);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 16, Ovia->BitDepth); // ValidBitsPerSample
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, Ovia->Aud->ChannelMask);
        WriteGUUID(GUIDString, BitB, WAVNULLBinaryGUID);
    }
    
    static void WAVWriteLISTChunk(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia->Aud->Meta->NumTags > 0) {
            // Start checking for tags to write
        }
    }
    
    void WAVAppendSamples(OVIA *Ovia, BitBuffer *BitB, uint32_t NumSamples2Write, uint32_t **Samples2Write) {
        if (PCM != NULL && BitB != NULL && Samples2Write != NULL) {
            uint64_t ChannelCount = OVIA_GetNumChannels(Ovia);
            uint64_t BitDepth     = Ovia->BitDepth;
            for (uint32_t Sample = 0; Sample < NumSamples2Write; Sample++) {
                for (uint16_t Channel = 0; Channel < ChannelCount; Channel++) {
                    WriteBits(LSByteFirst, LSBitFirst, BitB, BitDepth, Samples2Write[Channel][Sample]);
                }
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (OutputSamples == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (Samples2Write == NULL) {
            Log(Log_ERROR, __func__, U8("Samples2Write Pointer is NULL"));
        }
    }
    
#ifdef __cplusplus
}
#endif
