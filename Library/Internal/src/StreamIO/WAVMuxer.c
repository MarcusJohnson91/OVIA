#include "../../../Dependencies/BitIO/libBitIO/include/BitIO.h"
#include "../../../Dependencies/BitIO/libBitIO/include/BitIOLog.h"

#include "../../include/libPCM.h"
#include "../../include/Private/libPCMTypes.h"
#include "../../include/Private/Audio/WAVCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    static const uint8_t WAVNULLBinaryGUID[BitIOBinaryGUUIDSize] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    
    void WAVWriteHeader(PCMFile *PCM, BitBuffer *BitB) {
        
    }
    
    static void WAVWriteFMTChunk(PCMFile *PCM, BitBuffer *BitB) {
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, 40); // ChunkSize
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 16, 0xFFFE); // WaveFormatExtensible
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 16, PCM->NumChannels);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, PCM->AUD->SampleRate);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, (PCM->AUD->SampleRate * PCM->NumChannels * PCM->BitDepth) / 8);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, PCM->AUD->BlockAlignment);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 16, PCM->BitDepth);
        uint8_t CBSize = 46;
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 16, CBSize);
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 16, PCM->BitDepth); // ValidBitsPerSample
        WriteBits(BitIOLSByte, BitIOLSBit, BitB, 32, PCM->AUD->ChannelMask);
        WriteGUUID(BitIOBinaryGUUID, BitIOLSByte, BitB, WAVNULLBinaryGUID);
    }
    
    static void WAVWriteLISTChunk(PCMFile *PCM, BitBuffer *BitB) {
        if (PCM->AUD->Meta->NumTags > 0) {
            // Start checking for tags to write
        }
    }
    
    void WAVInsertSamples(PCMFile *PCM, BitBuffer *OutputSamples, uint32_t NumSamples2Write, uint32_t **Samples2Write) {
        if (PCM == NULL) {
            BitIOLog(BitIOLog_ERROR, libPCMLibraryName, __func__, "PCM Pointer is NULL");
        } else if (OutputSamples == NULL) {
            BitIOLog(BitIOLog_ERROR, libPCMLibraryName, __func__, "BitBuffer Pointer is NULL");
        } else {
            uint64_t ChannelCount = PCM->NumChannels;
            uint64_t BitDepth     = PCM->BitDepth;
            for (uint32_t Sample = 0; Sample < NumSamples2Write; Sample++) {
                for (uint16_t Channel = 0; Channel < ChannelCount; Channel++) {
                    WriteBits(BitIOLSByte, BitIOLSBit, OutputSamples, BitDepth, Samples2Write[Channel][Sample]);
                }
            }
        }
    }
    
#ifdef __cplusplus
}
#endif
