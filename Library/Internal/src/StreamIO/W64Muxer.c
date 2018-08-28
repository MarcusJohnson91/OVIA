#include "../../../include/Private/Audio/W64Common.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    // To encode W64 i'll need to read the values from the struct and write it to the file
    
    static uint64_t CalculateW64ByteRate(uint64_t NumChannels, uint8_t BitDepth, uint64_t SampleRate) {
        return NumChannels * SampleRate * (BitDepth / 8);
    }
    
    static uint64_t CalculateW64BlockAlign(uint64_t NumChannels, uint8_t BitDepth) {
        return NumChannels * (BitDepth / 8);
    }
    
    static void W64WriteFMTChunk(AudioContainer *Audio, BitBuffer *BitB) {
        uint64_t NumChannels = AudioContainer_GetNumSamples(Audio);
        uint64_t SampleRate  = AudioContainer_GetSampleRate(Audio);
        uint8_t  BitDepth    = Bits2Bytes(AudioContainer_GetBitDepth(Audio), Yes);
        uint64_t ByteRate    = CalculateW64ByteRate(NumChannels, BitDepth, SampleRate);
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
    
    void W64AppendSamples(AudioContainer *Audio, BitBuffer *BitB) {
        if (Audio != NULL && BitB != NULL) {
            uint64_t NumChannels  = AudioContainer_GetNumSamples(Audio);
            uint64_t BitDepth     = Bits2Bytes(AudioContainer_GetBitDepth(Audio), Yes);
            uint64_t NumSamples   = AudioContainer_GetNumSamples(Audio);
            Audio_Types Type      = AudioContainer_GetType(Audio);
            if (Type == AudioContainer_SInteger8) {
                int8_t **Samples  = (int8_t**)    AudioContainer_GetArray(Audio);
                for (uint32_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint16_t Channel = 0; Channel < NumChannels; Channel++) {
                        WriteBits(LSByteFirst, LSBitFirst, BitB, BitDepth, Samples[Channel][Sample]);
                    }
                }
            } else if (AudioContainer_UInteger8) {
                uint8_t **Samples = (uint8_t**)   AudioContainer_GetArray(Audio);
                for (uint32_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint16_t Channel = 0; Channel < NumChannels; Channel++) {
                        WriteBits(LSByteFirst, LSBitFirst, BitB, BitDepth, Samples[Channel][Sample]);
                    }
                }
            } else if (AudioContainer_SInteger16) {
                int16_t **Samples = (int16_t**)   AudioContainer_GetArray(Audio);
                for (uint32_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint16_t Channel = 0; Channel < NumChannels; Channel++) {
                        WriteBits(LSByteFirst, LSBitFirst, BitB, BitDepth, Samples[Channel][Sample]);
                    }
                }
            } else if (AudioContainer_UInteger16) {
                uint16_t **Samples = (uint16_t**) AudioContainer_GetArray(Audio);
                for (uint32_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint16_t Channel = 0; Channel < NumChannels; Channel++) {
                        WriteBits(LSByteFirst, LSBitFirst, BitB, BitDepth, Samples[Channel][Sample]);
                    }
                }
            } else if (AudioContainer_SInteger32) {
                int32_t  **Samples = (int32_t**)  AudioContainer_GetArray(Audio);
                for (uint32_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint16_t Channel = 0; Channel < NumChannels; Channel++) {
                        WriteBits(LSByteFirst, LSBitFirst, BitB, BitDepth, Samples[Channel][Sample]);
                    }
                }
            } else if (AudioContainer_UInteger32) {
                uint32_t **Samples = (uint32_t**) AudioContainer_GetArray(Audio);
                for (uint32_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint16_t Channel = 0; Channel < NumChannels; Channel++) {
                        WriteBits(LSByteFirst, LSBitFirst, BitB, BitDepth, Samples[Channel][Sample]);
                    }
                }
            }
        } else if (Audio == NULL) {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
#ifdef __cplusplus
}
#endif
