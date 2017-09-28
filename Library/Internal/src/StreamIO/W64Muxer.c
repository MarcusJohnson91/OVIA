#include "../../include/libPCM.h"
#include "../../include/Private/libPCMTypes.h"
#include "../../include/Private/Audio/W64Common.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    // To encode W64 i'll need to read the values from the struct and write it to the file
    
    uint64_t CalculateW64ByteRate(const uint64_t NumChannels, const uint64_t SampleRate, const uint8_t BitDepth) {
        return NumChannels * SampleRate * (BitDepth / 8);
    }
    
    uint64_t CalculateW64BlockAlign(const uint64_t NumChannels, const uint8_t BitDepth) {
        return NumChannels * (BitDepth / 8);
    }
    
    void WriteW64FMT(PCMFile *PCM, BitBuffer *BitB) {
        uint64_t ByteRate   = CalculateW64ByteRate(PCM->NumChannels, PCM->SampleRate, PCM->BitDepth);
        uint64_t BlockAlign = CalculateW64BlockAlign(PCM->NumChannels, PCM->BitDepth);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 16, 0);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 16, PCM->NumChannels);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, PCM->SampleRate);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, ByteRate);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, BlockAlign);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 16, PCM->BitDepth);
    }
    
    void WriteW64Header(PCMFile *PCM, BitBuffer *BitB) {
        WriteGUUID(BitIOUUIDString, BitB, W64_RIFF_GUIDString);
        // Write the size of the file including all header fields
        uint64_t W64Size = (PCM->NumSamples * PCM->NumChannels * PCM->BitDepth) + W64HeaderSize;
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 64, W64Size);
        WriteGUUID(BitIOUUIDString, BitB, W64_WAVE_GUIDString);
        WriteGUUID(BitIOUUIDString, BitB, W64_FMT_GUIDString);
        uint64_t FMTSize = 40;
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 64, FMTSize);
        WriteW64FMT(W64, BitB);
        WriteGUUID(BitIOUUIDString, BitB, W64_DATA_GUIDString);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 64, PCM->NumSamples);
    }
    
#ifdef __cplusplus
}
#endif
