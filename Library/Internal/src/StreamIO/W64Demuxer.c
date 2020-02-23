#include "../../include/Private/W64Common.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    /* Format Decoding */
    static void W64ReadFMTChunk(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            W64Options *W64                  = Options;
            W64->CompressionFormat           = (uint16_t) BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16);
            W64->NumChannels                 = (uint16_t) BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16);
            W64->SampleRate                  = (uint32_t) BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
            W64->BlockAlign                  = (uint32_t) BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
            W64->BlockAlignment              = (uint16_t) BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16);
            W64->BitDepth                    = (uint16_t) BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16);
            // Read the SpeakerMask
        } else if (Options == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    static void W64ReadBEXTChunk(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            W64Options *W64                  = Options;
        } else if (Options == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    static void W64ReadLEVLChunk(void *Options, BitBuffer *BitB) { // aka Peak Envelope Chunk
        if (Options != NULL && BitB != NULL) {
            W64Options *W64                  = Options;
        } else if (Options == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void W64ReadMetadata(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            W64Options *W64          = Options;
            uint8_t *ChunkUUIDString = BitBuffer_ReadGUUID(BitB, BinaryGUID);
            uint64_t W64Size         = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 64);
            if (GUUID_Compare(BinaryGUID, ChunkUUIDString, W64_RIFF_GUIDString) == true) {
                
            } else if (GUUID_Compare(BinaryGUID, ChunkUUIDString, W64_WAVE_GUIDString) == true) {
                
            } else if (GUUID_Compare(BinaryGUID, ChunkUUIDString, W64_FMT_GUIDString) == true) {
                W64ReadFMTChunk(W64, BitB);
            } else if (GUUID_Compare(BinaryGUID, ChunkUUIDString, W64_DATA_GUIDString) == true) {
                
            } else if (GUUID_Compare(BinaryGUID, ChunkUUIDString, W64_LEVL_GUIDString) == true) {
                
            } else if (GUUID_Compare(BinaryGUID, ChunkUUIDString, W64_BEXT_GUIDString) == true) {
                W64ReadBEXTChunk(W64, BitB);
            } else if (GUUID_Compare(BinaryGUID, ChunkUUIDString, W64_FACT_GUIDString) == true) {
                
            } else if (GUUID_Compare(BinaryGUID, ChunkUUIDString, W64_JUNK_GUIDString) == true) {
                
            } else if (GUUID_Compare(BinaryGUID, ChunkUUIDString, W64_MRKR_GUIDString) == true) {
                
            } else if (GUUID_Compare(BinaryGUID, ChunkUUIDString, W64_SUMM_GUIDString) == true) {
                
            } else if (GUUID_Compare(BinaryGUID, ChunkUUIDString, W64_LIST_GUIDString) == true) {
                
            }
        } else if (Options == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void *W64ExtractSamples(void *Options, BitBuffer *BitB) {
        Audio2DContainer *Audio = NULL;
        if (Options != NULL && BitB != NULL) {
            W64Options *W64        = Options;
            uint64_t BitDepth      = W64->BitDepth;
            uint64_t NumChannels   = W64->NumChannels;
            uint8_t  BitDepthRound = Bytes2Bits(Bits2Bytes(BitDepth, RoundingType_Up));
            uint8_t  NumSamples    = 3; // FIXME: This is complete bullshit
            if (BitDepth <= 8) {
                uint8_t **Samples = (uint8_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        Samples[Channel][Sample] = (uint8_t) BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, BitDepthRound);
                    }
                }
            } else if (BitDepth <= 16) {
                uint16_t **Samples = (uint16_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        Samples[Channel][Sample] = (uint16_t) BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, BitDepthRound);
                    }
                }
            } else if (BitDepth <= 32) {
                 uint32_t **Samples = (uint32_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        Samples[Channel][Sample] = (uint32_t) BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, BitDepthRound);
                    }
                }
            }
        } else if (Options == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (Audio == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Audio2DContainer is NULL"));
        }
        return Audio;
    }
    
    static const MagicIDSizes W64MagicIDSize = {
        .NumSizes = 3,
        .Sizes    = {
            [0]   = 2,
            [1]   = 2,
            [2]   = 2,
            [3]   = 2,
            [4]   = 2,
            [5]   = 2,
            [6]   = 2,
        },
    };
    
    static const MagicIDOffsets W64MagicIDOffset = {
        .NumOffsets = 3,
        .Offsets    = {
            [0]     = 0,
            [1]     = 0,
            [2]     = 0,
            [3]     = 0,
            [4]     = 0,
            [5]     = 0,
            [6]     = 0,
        },
    };
    
    static const MagicIDNumbers W64MagicIDNumber = {
        .NumMagicIDs  = 7,
        .MagicNumbers = {
            [0]       = (uint8_t[2]){0x50, 0x31},
            [1]       = (uint8_t[2]){0x50, 0x32},
            [2]       = (uint8_t[2]){0x50, 0x33},
            [3]       = (uint8_t[2]){0x50, 0x34},
            [4]       = (uint8_t[2]){0x50, 0x35},
            [5]       = (uint8_t[2]){0x50, 0x36},
            [6]       = (uint8_t[2]){0x50, 0x37},
        },
    };
    
    static const MagicIDs W64MagicIDs = {
        .Sizes                 = &W64MagicIDSize,
        .Offsets               = &W64MagicIDOffset,
        .Number                = &W64MagicIDNumber,
    };
    
    static const OVIADecoder W64Decoder = {
        .Function_Initialize   = W64Options_Init,
        .Function_Decode       = W64ExtractSamples,
        .Function_Read         = W64ReadMetadata,
        .Function_Deinitialize = W64Options_Deinit,
        .MagicID               = &W64MagicIDs,
        .MediaType             = MediaType_Audio2D,
        .DecoderID             = CodecID_W64,
    };
    
#ifdef __cplusplus
}
#endif
