#include "../../../include/Private/Audio/W64Common.h"

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
    
    static void W64WriteFMTChunk(AudioContainer *Audio, BitBuffer *BitB) {
        uint64_t NumChannels = AudioContainer_GetNumSamples(Audio);
        uint64_t SampleRate  = AudioContainer_GetSampleRate(Audio);
        uint8_t  BitDepth    = Bits2Bytes(AudioContainer_GetBitDepth(Audio), Yes);
        uint64_t ByteRate    = CalculateW64ByteRate(NumChannels, SampleRate, BitDepth);
        uint64_t BlockAlign  = CalculateW64BlockAlign(NumChannels, BitDepth);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 16, 0);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 16, NumChannels);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, SampleRate);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, ByteRate);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 32, BlockAlign);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 16, BitDepth);
    }
    
    void W64WriteHeader(AudioContainer *Audio, BitBuffer *BitB) {
        uint64_t NumChannels = AudioContainer_GetNumSamples(Audio);
        uint64_t NumSamples  = AudioContainer_GetNumSamples(Audio);
        uint8_t  BitDepth    = Bits2Bytes(AudioContainer_GetBitDepth(Audio), Yes);
        WriteGUUID(GUIDString, BitB, W64_RIFF_GUIDString);
        uint64_t W64Size     = (NumSamples * NumChannels * BitDepth);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 64, W64Size);
        WriteGUUID(GUIDString, BitB, W64_WAVE_GUIDString);
        WriteGUUID(GUIDString, BitB, W64_FMT_GUIDString);
        uint64_t FMTSize     = 40;
        WriteBits(LSByteFirst, LSBitFirst, BitB, 64, FMTSize);
        W64WriteFMTChunk(Audio, BitB);
        WriteGUUID(GUIDString, BitB, W64_DATA_GUIDString);
        WriteBits(LSByteFirst, LSBitFirst, BitB, 64, NumSamples);
    }
    
    void W64AppendSamples(AudioContainer *Audio, BitBuffer *OutputSamples) {
        if (Audio != NULL && OutputSamples != NULL) {
            uint64_t NumChannels  = AudioContainer_GetNumSamples(Audio);
            uint64_t BitDepth     = Bits2Bytes(AudioContainer_GetBitDepth(Audio), Yes);
            uint64_t NumSamples   = AudioContainer_GetNumSamples(Audio);
            for (uint32_t Sample = 0; Sample < NumSamples; Sample++) {
                for (uint16_t Channel = 0; Channel < NumChannels; Channel++) {
                    WriteBits(LSByteFirst, LSBitFirst, OutputSamples, BitDepth, ); // The last parameter needs to be a pointer to the actual array.
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
