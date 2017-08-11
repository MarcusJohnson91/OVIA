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
        WriteBits(BitB, 0,                16, false); // PCM
        WriteBits(BitB, PCM->NumChannels, 16, false);
        WriteBits(BitB, PCM->SampleRate,  32, false);
        WriteBits(BitB, ByteRate,         32, false);
        WriteBits(BitB, BlockAlign,       16, false);
        WriteBits(BitB, PCM->BitDepth,    16, false);
    }
    
    void WriteW64Header(PCMFile *PCM, BitBuffer *BitB) {
        WriteUUID(BitB, RIFF_UUID);
        // Write the size of the file including all header fields
        uint64_t W64Size = (W64->NumSamples * W64->Channels * W64->BitDepth) + W64HeaderSize;
        WriteBits(BitB, SwapEndian64(W64Size), 64, false); // little endian field
        WriteUUID(BitB, WAVE_UUID);
        WriteUUID(BitB, FMT_UUID);
        uint64_t FMTSize = 40;
        WriteBits(BitB, SwapEndian64(FMTSize), 64, false); // little endian
        WriteW64FMT(W64, BitB);
        WriteUUID(BitB, DATA_UUID);
        WriteBits(BitB, SwapEndian64(W64->NumSamples), 64, false); // little endian
    }
    
#ifdef __cplusplus
}
#endif
