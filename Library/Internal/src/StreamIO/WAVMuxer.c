#include "../../include/Private/WAVCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    static const uint8_t WAVNULLBinaryGUID[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    
    void WAVWriteHeader(WAVOptions *WAV, BitBuffer *BitB) {
        if (WAV != NULL && BitB != NULL) {
            
        } else if (WAV == NULL) {
            Log(Log_DEBUG, __func__, U8("WAVOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void WAVWriteFMTChunk(WAVOptions *WAV, BitBuffer *BitB) {
        if (WAV != NULL && BitB != NULL) {
            uint64_t NumChannels = WAV->NumChannels;
            uint64_t SampleRate  = WAV->SampleRate;
            uint64_t BitDepth    = WAV->BitDepth;
            uint8_t CBSize = 46;
            
            BitBuffer_WriteBits(BitB, LSByteFirst, LSBitFirst, 32, 40); // ChunkSize
            BitBuffer_WriteBits(BitB, LSByteFirst, LSBitFirst, 16, 0xFFFE); // WaveFormatExtensible
            BitBuffer_WriteBits(BitB, LSByteFirst, LSBitFirst, 16, NumChannels);
            BitBuffer_WriteBits(BitB, LSByteFirst, LSBitFirst, 32, SampleRate);
            BitBuffer_WriteBits(BitB, LSByteFirst, LSBitFirst, 32, SampleRate * NumChannels * BitDepth);
            BitBuffer_WriteBits(BitB, LSByteFirst, LSBitFirst, 32, WAV->BlockAlign);
            BitBuffer_WriteBits(BitB, LSByteFirst, LSBitFirst, 16, BitDepth);
            BitBuffer_WriteBits(BitB, LSByteFirst, LSBitFirst, 16, CBSize);
            BitBuffer_WriteBits(BitB, LSByteFirst, LSBitFirst, 16, BitDepth); // ValidBitsPerSample
            BitBuffer_WriteBits(BitB, LSByteFirst, LSBitFirst, 32, WAV->ChannelMask);
            BitBuffer_WriteGUUID(BitB, BinaryGUID, WAVNULLBinaryGUID);
        } else if (WAV == NULL) {
            Log(Log_DEBUG, __func__, U8("WAVOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void WAVWriteLISTChunk(WAVOptions *WAV, BitBuffer *BitB) {
        if (WAV != NULL && BitB != NULL) {
            // Write the tags
        } else if (WAV == NULL) {
            Log(Log_DEBUG, __func__, U8("WAVOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WAVAppendSamples(Audio2DContainer *Audio, BitBuffer *BitB) {
        if (Audio != NULL && BitB != NULL) {
            uint64_t NumChannels   = AudioContainer_GetNumSamples(Audio);
            uint64_t BitDepth      = AudioContainer_GetBitDepth(Audio);
            uint64_t NumSamples    = AudioContainer_GetNumSamples(Audio);
            Audio_Types Type       = Audio2DContainer_GetType(Audio);
            
            if (Type == (AudioType_Unsigned | AudioType_Integer8)) {
                uint8_t **Samples  = (uint8_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        BitBuffer_WriteBits(BitB, LSByteFirst, LSBitFirst, BitDepth, Samples[Channel][Sample]);
                    }
                }
            } else if (Type == (AudioType_Signed | AudioType_Integer8)) {
                int8_t **Samples   = (int8_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        BitBuffer_WriteBits(BitB, LSByteFirst, LSBitFirst, BitDepth, Samples[Channel][Sample]);
                    }
                }
            } else if (Type == (AudioType_Unsigned | AudioType_Integer16)) {
                uint16_t **Samples = (uint16_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        BitBuffer_WriteBits(BitB, LSByteFirst, LSBitFirst, BitDepth, Samples[Channel][Sample]);
                    }
                }
            } else if (Type == (AudioType_Signed | AudioType_Integer16)) {
                int16_t **Samples  = (int16_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        BitBuffer_WriteBits(BitB, LSByteFirst, LSBitFirst, BitDepth, Samples[Channel][Sample]);
                    }
                }
            } else if (Type == (AudioType_Unsigned | AudioType_Integer32)) {
                uint32_t **Samples = (uint32_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        BitBuffer_WriteBits(BitB, LSByteFirst, LSBitFirst, BitDepth, Samples[Channel][Sample]);
                    }
                }
            } else if (Type == (AudioType_Signed | AudioType_Integer32)) {
                int32_t **Samples  = (int32_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        BitBuffer_WriteBits(BitB, LSByteFirst, LSBitFirst, BitDepth, Samples[Channel][Sample]);
                    }
                }
            }
        } else if (Audio == NULL) {
            Log(Log_DEBUG, __func__, U8("Audio2DContainer Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    OVIAEncoder WAVEncoder = {
        .EncoderID             = CodecID_WAV,
        .MediaType             = MediaType_Audio2D,
        .Function_Initialize   = WAVOptions_Init,
        .Function_WriteHeader  = WAVWriteHeader,
        .Function_Encode       = WAVAppendSamples,
        .Function_WriteFooter  = NULL,
        .Function_Deinitialize = WAVOptions_Deinit,
    };
    
#ifdef __cplusplus
}
#endif
