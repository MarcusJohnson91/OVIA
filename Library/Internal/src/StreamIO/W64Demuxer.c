#include "../../include/Private/W64Common.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    /* Format Decoding */
    static void W64ReadFMTChunk(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            W64Options *W64                  = Options;
            W64->CompressionFormat           = (uint16_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16);
            W64->NumChannels                 = (uint16_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16);
            W64->SampleRate                  = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
            W64->BlockAlign                  = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
            W64->BlockAlignment              = (uint16_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16);
            W64->BitDepth                    = (uint16_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16);
            // Read the SpeakerMask
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    static void W64ReadBEXTChunk(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            W64Options *W64                  = Options;
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    static void W64ReadLEVLChunk(void *Options, BitBuffer *BitB) { // aka Peak Envelope Chunk
        if (Options != NULL && BitB != NULL) {
            W64Options *W64                  = Options;
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void W64ReadMetadata(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            W64Options *W64          = Options;
            uint8_t *ChunkUUIDString = BitBuffer_ReadGUUID(BitB, GUUIDType_BinaryGUID);
            uint64_t W64Size         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 64);
            if (GUUID_Compare(GUUIDType_BinaryGUID, ChunkUUIDString, W64_RIFF_GUIDString) == true) {
                
            } else if (GUUID_Compare(GUUIDType_BinaryGUID, ChunkUUIDString, W64_WAVE_GUIDString) == true) {
                
            } else if (GUUID_Compare(GUUIDType_BinaryGUID, ChunkUUIDString, W64_FMT_GUIDString) == true) {
                W64ReadFMTChunk(W64, BitB);
            } else if (GUUID_Compare(GUUIDType_BinaryGUID, ChunkUUIDString, W64_DATA_GUIDString) == true) {
                
            } else if (GUUID_Compare(GUUIDType_BinaryGUID, ChunkUUIDString, W64_LEVL_GUIDString) == true) {
                
            } else if (GUUID_Compare(GUUIDType_BinaryGUID, ChunkUUIDString, W64_BEXT_GUIDString) == true) {
                W64ReadBEXTChunk(W64, BitB);
            } else if (GUUID_Compare(GUUIDType_BinaryGUID, ChunkUUIDString, W64_FACT_GUIDString) == true) {
                
            } else if (GUUID_Compare(GUUIDType_BinaryGUID, ChunkUUIDString, W64_JUNK_GUIDString) == true) {
                
            } else if (GUUID_Compare(GUUIDType_BinaryGUID, ChunkUUIDString, W64_MRKR_GUIDString) == true) {
                
            } else if (GUUID_Compare(GUUIDType_BinaryGUID, ChunkUUIDString, W64_SUMM_GUIDString) == true) {
                
            } else if (GUUID_Compare(GUUIDType_BinaryGUID, ChunkUUIDString, W64_LIST_GUIDString) == true) {
                
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
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
                        Samples[Channel][Sample] = (uint8_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitDepthRound);
                    }
                }
            } else if (BitDepth <= 16) {
                uint16_t **Samples = (uint16_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        Samples[Channel][Sample] = (uint16_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitDepthRound);
                    }
                }
            } else if (BitDepth <= 32) {
                 uint32_t **Samples = (uint32_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        Samples[Channel][Sample] = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitDepthRound);
                    }
                }
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (Audio == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Audio2DContainer is NULL"));
        }
        return Audio;
    }
    
    static const OVIADecoder W64Decoder = {
        .Function_Initialize   = W64Options_Init,
        .Function_Decode       = W64ExtractSamples,
        .Function_Read         = W64ReadMetadata,
        .Function_Deinitialize = W64Options_Deinit,
        .MagicIDs              = &W64MagicIDs,
        .MediaType             = MediaType_Audio2D,
        .DecoderID             = CodecID_PCMAudio,
    };
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
