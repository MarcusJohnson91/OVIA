#include "../../Dependencies/BitIO/libBitIO/include/BitIO.h"

#include "../include/libPCM.h"
#include "../include/WAVCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    static const uint8_t WAVNULLBinaryGUID[BitIOBinaryGUUIDSize] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    
    void WriteWAVFMTChunk(PCMFile *PCM, BitBuffer *BitB) {
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, 40); // ChunkSize
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 16, 0xFFFE); // WaveFormatExtensible
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 16, PCM->AUD->NumChannels);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, PCM->AUD->SampleRate);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, (PCM->AUD->SampleRate * PCM->AUD->NumChannels * PCM->AUD->BitDepth) / 8);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, PCM->AUD->BlockAlignment);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 16, PCM->AUD->BitDepth);
        uint8_t CBSize = 46;
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 16, CBSize);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 16, PCM->AUD->BitDepth); // ValidBitsPerSample
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, PCM->AUD->ChannelMask);
        WriteGUUID(BitIOBinaryGUID, BitB, WAVNULLBinaryGUID);
    }
    
    void WriteWAVLISTChunk(PCMFile *PCM, BitBuffer *BitB) {
        if (PCM->AUD->Meta->NumTags > 0) {
            // Start checking for tags to write
        }
    }
    
#ifdef __cplusplus
}
#endif
