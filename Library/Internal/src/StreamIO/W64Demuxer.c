#include "../../../Dependencies/FoundationIO/libFoundationIO/include/BitIO.h"
#include "../../../Dependencies/FoundationIO/libFoundationIO/include/Math.h"
#include "../../../Dependencies/FoundationIO/libFoundationIO/include/Log.h"

#include "../../include/libPCM.h"
#include "../../include/Private/libPCMTypes.h"
#include "../../include/Private/Audio/W64Common.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    /* Format decoding */
    static void W64ParseFMTChunk(PCMFile *PCM, BitBuffer *BitB) {
        PCM->Aud->FormatType       = ReadBits(LSByteFirst, LSBitFirst, BitB, 16);
        PCM->NumChannels           = ReadBits(LSByteFirst, LSBitFirst, BitB, 16);
        PCM->Aud->SampleRate       = ReadBits(LSByteFirst, LSBitFirst, BitB, 32);
        BitBuffer_Skip(BitB, 32); // ByteRate
        PCM->Aud->BlockAlignment   = ReadBits(LSByteFirst, LSBitFirst, BitB, 16);
        PCM->BitDepth              = ReadBits(LSByteFirst, LSBitFirst, BitB, 16);
    }
    
    static void W64ParseBEXTChunk(PCMFile *PCM, BitBuffer *BitB) {
        
    }
    
    static void W64ParseDATAChunk(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) { // return the number of samples read
        PCM->NumChannelAgnosticSamples = (((ChunkSize - 24 / PCM->Aud->BlockAlignment) / PCM->NumChannels) / PCM->BitDepth);
    }
    
    static void W64ParseLEVLChunk(PCMFile *PCM, BitBuffer *BitB) { // aka Peak Envelope Chunk
        
    }
    
    void W64ParseMetadata(PCMFile *PCM, BitBuffer *BitB) {
        uint8_t *ChunkUUIDString = ReadGUUID(GUIDString, BitB);
        uint64_t W64Size         = ReadBits(LSByteFirst, LSBitFirst, BitB, 64);
        if (CompareGUUIDs(GUIDString, ChunkUUIDString, W64_RIFF_GUIDString) == Yes) {
            
        } else if (CompareGUUIDs(GUIDString, ChunkUUIDString, W64_WAVE_GUIDString) == Yes) {
            
        } else if (CompareGUUIDs(GUIDString, ChunkUUIDString, W64_FMT_GUIDString) == Yes) {
            W64ParseFMTChunk(PCM, BitB);
        } else if (CompareGUUIDs(GUIDString, ChunkUUIDString, W64_DATA_GUIDString) == Yes) {
            
        } else if (CompareGUUIDs(GUIDString, ChunkUUIDString, W64_LEVL_GUIDString) == Yes) {
            
        } else if (CompareGUUIDs(GUIDString, ChunkUUIDString, W64_BEXT_GUIDString) == Yes) {
            W64ParseBEXTChunk(PCM, BitB);
        } else if (CompareGUUIDs(GUIDString, ChunkUUIDString, W64_FACT_GUIDString) == Yes) {
            
        } else if (CompareGUUIDs(GUIDString, ChunkUUIDString, W64_JUNK_GUIDString) == Yes) {
            
        } else if (CompareGUUIDs(GUIDString, ChunkUUIDString, W64_MRKR_GUIDString) == Yes) {
            
        } else if (CompareGUUIDs(GUIDString, ChunkUUIDString, W64_SUMM_GUIDString) == Yes) {
            
        } else if (CompareGUUIDs(GUIDString, ChunkUUIDString, W64_LIST_GUIDString) == Yes) {
            
        }
    }
    
    void W64ExtractSamples(PCMFile *PCM, BitBuffer *BitB, uint64_t NumSamples2Extract, uint32_t **ExtractedSamples) {
        if (PCM != NULL && BitB != NULL && ExtractedSamples != NULL) {
            for (uint64_t Sample = 0; Sample < NumSamples2Extract; Sample++) {
                for (uint64_t Channel = 0; Channel < PCM->NumChannels; Channel++) {
                    ExtractedSamples[Channel][Sample] = ReadBits(LSByteFirst, LSBitFirst, BitB, (uint64_t) Bits2Bytes(PCM->BitDepth, Yes));
                }
            }
        } else if (PCM == NULL) {
            Log(Log_ERROR, __func__, U8("PCM Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (ExtractedSamples == NULL) {
            Log(Log_ERROR, __func__, U8("ExtractedSamples Pointer is NULL"));
        }
    }
    
#ifdef __cplusplus
}
#endif

