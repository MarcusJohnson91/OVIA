#include "../../include/Private/W64Common.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    static Audio_ChannelMask W64ChannelMap2AudioMask(uint64_t ChannelMask) {
        Audio_ChannelMask AudioMask = 0;
        if (ChannelMask & 0x1) {
            AudioMask += AudioMask_FrontLeft;
        } else if (ChannelMask & 0x2) {
            AudioMask += AudioMask_FrontRight;
        } else if (ChannelMask & 0x4) {
            AudioMask += AudioMask_FrontCenter;
        } else if (ChannelMask & 0x8) {
            AudioMask += AudioMask_LFE;
        } else if (ChannelMask & 0x10) {
            AudioMask += AudioMask_RearLeft;
        } else if (ChannelMask & 0x20) {
            AudioMask += AudioMask_RearRight;
        } else if (ChannelMask & 0x40) {
            AudioMask += AudioMask_FrontCenterLeft;
        } else if (ChannelMask & 0x80) {
            AudioMask += AudioMask_FrontCenterRight;
        } else if (ChannelMask & 0x100) {
            AudioMask += AudioMask_RearCenter;
        } else if (ChannelMask & 0x200) {
            AudioMask += AudioMask_SurroundLeft;
        } else if (ChannelMask & 0x400) {
            AudioMask += AudioMask_SurroundRight;
        } else if (ChannelMask & 0x800) {
            AudioMask += AudioMask_TopCenter;
        } else if (ChannelMask & 0x1000) {
            AudioMask += AudioMask_TopFrontLeft;
        } else if (ChannelMask & 0x2000) {
            AudioMask += AudioMask_TopFrontCenter;
        } else if (ChannelMask & 0x4000) {
            AudioMask += AudioMask_TopFrontRight;
        } else if (ChannelMask & 0x8000) {
            AudioMask += AudioMask_TopRearLeft;
        } else if (ChannelMask & 0x10000) {
            AudioMask += AudioMask_TopRearCenter;
        } else if (ChannelMask & 0x20000) {
            AudioMask += AudioMask_TopRearRight;
        }
        return AudioMask;
    }
    
    /* Format Decoding */
    static void W64ParseFMTChunk(BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            OVIA_W64_SetCompressionType(Ovia, BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16));
            OVIA_SetNumChannels(Ovia,         BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16));
            OVIA_SetSampleRate(Ovia,          BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32));
            OVIA_SetBlockSize(Ovia,           BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32));
            OVIA_W64_SetBlockAlignment(Ovia,  BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16));
            OVIA_SetBitDepth(Ovia,            BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16));
            // Read the SpeakerMask
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void W64ParseBEXTChunk(BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void W64ParseDATAChunk(BitBuffer *BitB, uint32_t ChunkSize) { // return the number of samples read
        if (Ovia != NULL && BitB != NULL) {
            OVIA_SetNumSamples(Ovia, (((ChunkSize - 24 / OVIA_W64_GetBlockAlignment(Ovia)) / OVIA_GetNumChannels(Ovia)) / OVIA_GetBitDepth(Ovia)));
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void W64ParseLEVLChunk(BitBuffer *BitB) { // aka Peak Envelope Chunk
        if (Ovia != NULL && BitB != NULL) {
            
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void W64ParseMetadata(BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            uint8_t *ChunkUUIDString = BitBuffer_ReadGUUID(BitB, GUIDString);
            uint64_t W64Size         = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 64);
            if (CompareGUUIDs(GUIDString, ChunkUUIDString, W64_RIFF_GUIDString) == true) {
                
            } else if (CompareGUUIDs(GUIDString, ChunkUUIDString, W64_WAVE_GUIDString) == true) {
                
            } else if (CompareGUUIDs(GUIDString, ChunkUUIDString, W64_FMT_GUIDString) == true) {
                W64ParseFMTChunk(Ovia, BitB);
            } else if (CompareGUUIDs(GUIDString, ChunkUUIDString, W64_DATA_GUIDString) == true) {
                
            } else if (CompareGUUIDs(GUIDString, ChunkUUIDString, W64_LEVL_GUIDString) == true) {
                
            } else if (CompareGUUIDs(GUIDString, ChunkUUIDString, W64_BEXT_GUIDString) == true) {
                W64ParseBEXTChunk(Ovia, BitB);
            } else if (CompareGUUIDs(GUIDString, ChunkUUIDString, W64_FACT_GUIDString) == true) {
                
            } else if (CompareGUUIDs(GUIDString, ChunkUUIDString, W64_JUNK_GUIDString) == true) {
                
            } else if (CompareGUUIDs(GUIDString, ChunkUUIDString, W64_MRKR_GUIDString) == true) {
                
            } else if (CompareGUUIDs(GUIDString, ChunkUUIDString, W64_SUMM_GUIDString) == true) {
                
            } else if (CompareGUUIDs(GUIDString, ChunkUUIDString, W64_LIST_GUIDString) == true) {
                
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    Audio2DContainer *W64ExtractSamples(BitBuffer *BitB) {
        Audio2DContainer *Audio = NULL;
        if (Ovia != NULL && BitB != NULL) {
            uint64_t BitDepth     = OVIA_GetBitDepth(Ovia);
            uint64_t NumChannels  = OVIA_GetNumChannels(Ovia);
            uint64_t SampleRate   = OVIA_GetSampleRate(Ovia);
            uint64_t NumSamples   = OVIA_GetNumSamples(Ovia);
            if (BitDepth <= 8) {
                Audio             = AudioContainer_Init(AudioType_Integer8, AudioMask_FrontLeft | AudioMask_FrontRight, SampleRate, NumSamples);
                uint8_t **Samples = (uint8_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        Samples[Channel][Sample] = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, Bits2Bytes(BitDepth, RoundingType_Up));
                    }
                }
            } else if (BitDepth > 8 && BitDepth <= 16) {
                Audio             = AudioContainer_Init(AudioType_Integer16, AudioMask_FrontLeft | AudioMask_FrontRight, SampleRate, NumSamples);
                uint16_t **Samples = (uint16_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        Samples[Channel][Sample] = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, Bits2Bytes(BitDepth, RoundingType_Up));
                    }
                }
            } else if (BitDepth > 16 && BitDepth <= 32) {
                Audio             = AudioContainer_Init(AudioType_Integer32, AudioMask_FrontLeft | AudioMask_FrontRight, SampleRate, NumSamples);
                uint32_t **Samples = (uint32_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        Samples[Channel][Sample] = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, Bits2Bytes(BitDepth, RoundingType_Up));
                    }
                }
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
        return Audio;
    }
    
#ifdef __cplusplus
}
#endif

