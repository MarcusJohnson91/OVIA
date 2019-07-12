#include "../../include/Private/W64Common.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    /* Format Decoding */
    static void W64ParseFMTChunk(W64Options *W64, BitBuffer *BitB) {
        if (W64 != NULL && BitB != NULL) {
            W64->CompressionFormat           = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16);
            W64->NumChannels                 = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16);
            W64->SampleRate                  = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
            W64->BlockAlign                  = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
            W64->BlockAlignment              = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16);
            W64->BitDepth                    = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16);
            // Read the SpeakerMask
        } else if (W64 == NULL) {
            Log(Log_DEBUG, __func__, U8("W64Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void W64ParseBEXTChunk(W64Options *W64, BitBuffer *BitB) {
        if (W64 != NULL && BitB != NULL) {
            
        } else if (W64 == NULL) {
            Log(Log_DEBUG, __func__, U8("W64Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void W64ParseLEVLChunk(W64Options *W64, BitBuffer *BitB) { // aka Peak Envelope Chunk
        if (W64 != NULL && BitB != NULL) {
            
        } else if (W64 == NULL) {
            Log(Log_DEBUG, __func__, U8("W64Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void W64ParseMetadata(W64Options *W64, BitBuffer *BitB) {
        if (W64 != NULL && BitB != NULL) {
            uint8_t *ChunkUUIDString = BitBuffer_ReadGUUID(BitB, BinaryGUID);
            uint64_t W64Size         = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 64);
            if (GUUID_Compare(BinaryGUID, ChunkUUIDString, W64_RIFF_GUIDString) == true) {
                
            } else if (GUUID_Compare(BinaryGUID, ChunkUUIDString, W64_WAVE_GUIDString) == true) {
                
            } else if (GUUID_Compare(BinaryGUID, ChunkUUIDString, W64_FMT_GUIDString) == true) {
                W64ParseFMTChunk(W64, BitB);
            } else if (GUUID_Compare(BinaryGUID, ChunkUUIDString, W64_DATA_GUIDString) == true) {
                
            } else if (GUUID_Compare(BinaryGUID, ChunkUUIDString, W64_LEVL_GUIDString) == true) {
                
            } else if (GUUID_Compare(BinaryGUID, ChunkUUIDString, W64_BEXT_GUIDString) == true) {
                W64ParseBEXTChunk(W64, BitB);
            } else if (GUUID_Compare(BinaryGUID, ChunkUUIDString, W64_FACT_GUIDString) == true) {
                
            } else if (GUUID_Compare(BinaryGUID, ChunkUUIDString, W64_JUNK_GUIDString) == true) {
                
            } else if (GUUID_Compare(BinaryGUID, ChunkUUIDString, W64_MRKR_GUIDString) == true) {
                
            } else if (GUUID_Compare(BinaryGUID, ChunkUUIDString, W64_SUMM_GUIDString) == true) {
                
            } else if (GUUID_Compare(BinaryGUID, ChunkUUIDString, W64_LIST_GUIDString) == true) {
                
            }
        } else if (W64 == NULL) {
            Log(Log_DEBUG, __func__, U8("W64Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void W64ExtractSamples(W64Options *W64, BitBuffer *BitB, Audio2DContainer *Audio, uint64_t NumSamples) {
        if (W64 != NULL && BitB != NULL && Audio != NULL) {
            uint64_t BitDepth      = W64->BitDepth;
            uint64_t NumChannels   = W64->NumChannels;
            uint8_t  BitDepthRound = Bytes2Bits(Bits2Bytes(BitDepth, RoundingType_Up));
            if (BitDepth <= 8) {
                uint8_t **Samples = (uint8_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        Samples[Channel][Sample] = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, BitDepthRound);
                    }
                }
            } else if (BitDepth <= 16) {
                uint16_t **Samples = (uint16_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        Samples[Channel][Sample] = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, BitDepthRound);
                    }
                }
            } else if (BitDepth <= 32) {
                 uint32_t **Samples = (uint32_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        Samples[Channel][Sample] = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, BitDepthRound);
                    }
                }
            }
        } else if (W64 == NULL) {
            Log(Log_DEBUG, __func__, U8("W64Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (Audio == NULL) {
            Log(Log_DEBUG, __func__, U8("Audio2DContainer is NULL"));
        }
    }
    
    OVIADecoder W64Decoder = {
        .DecoderID             = CodecID_W64,
        .MediaType             = MediaType_Audio2D,
        .MagicIDOffset         = 0,
        .MagicIDSize           = 16,
        .MagicID               = {0x72, 0x69, 0x66, 0x66, 0x2E, 0x91, 0xCF, 0x11, 0xA5, 0xD6, 0x28, 0xDB, 0x04, 0xC1, 0x00, 0x00},
        .Function_Initialize   = W64Options_Init,
        .Function_Parse        = W64ParseMetadata,
        .Function_Decode       = W64ExtractSamples,
        .Function_Deinitialize = W64Options_Deinit,
    };
    
#ifdef __cplusplus
}
#endif

