#include "../../Dependencies/BitIO/libBitIO/include/BitIO.h"

#include "../include/libPCM.h"
#include "../include/WAVCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    static const uint8_t WAVNULLBinaryGUID[BitIOBinaryGUUIDSize] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    
    enum WAVFormatCode {
        PCM = 1,
        
    };
    
    void WriteWAVFMTChunk(PCMFile *PCM, BitBuffer *BitB) {
        uint8_t CBSize = 46;
        
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, 40); // ChunkSize
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 16, 0xFFFE); // WaveFormatExtensible
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 16, PCM->NumChannels);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, PCM->SampleRate);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, (PCM->SampleRate * PCM->NumChannels * PCM->BitDepth) / 8);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, PCM->BlockAlignment);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 16, PCM->BitDepth);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 16, CBSize);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 16, PCM->BitDepth); // ValidBitsPerSample
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, PCM->ChannelMask);
        WriteGUUID(BitIOBinaryGUID, BitB, WAVNULLBinaryGUID);
    }
    
    void WriteWAVLISTChunk(PCMFile *PCM, BitBuffer *BitB) {
        if (PCM->Meta->NumTags > 0) {
            // Start checking for tags to write
        }
    }
    
#ifdef __cplusplus
}
#endif
