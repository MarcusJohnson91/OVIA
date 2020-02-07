#include "../../include/Private/W64Common.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    // To encode W64 i'll need to read the values from the struct and write it to the file
    
    static void W64WriteFMTChunk(Audio2DContainer *Audio, BitBuffer *BitB) {
        if (Audio != NULL && BitB != NULL) {
            uint64_t NumChannels = Audio2DContainer_GetNumSamples(Audio);
            uint64_t SampleRate  = Audio2DContainer_GetSampleRate(Audio);
            uint8_t  BitDepth    = Bits2Bytes(Audio2DContainer_GetBitDepth(Audio), RoundingType_Up);
            uint64_t ByteRate    = CalculateW64ByteRate(NumChannels, BitDepth, SampleRate);
            uint64_t BlockAlign  = CalculateW64BlockAlign(NumChannels, BitDepth);
            BitBuffer_WriteBits(BitB, LSByteFirst, LSBitFirst, 16, 0);
            BitBuffer_WriteBits(BitB, LSByteFirst, LSBitFirst, 16, NumChannels);
            BitBuffer_WriteBits(BitB, LSByteFirst, LSBitFirst, 32, SampleRate);
            BitBuffer_WriteBits(BitB, LSByteFirst, LSBitFirst, 32, ByteRate);
            BitBuffer_WriteBits(BitB, LSByteFirst, LSBitFirst, 32, BlockAlign);
            BitBuffer_WriteBits(BitB, LSByteFirst, LSBitFirst, 16, BitDepth);
        } else if (Audio == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Audio2DContainer Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void W64WriteHeader(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            W64Options *W64      = Options;
            uint64_t NumChannels = W64->NumChannels;
            uint64_t NumSamples  = W64->CompressionFormat; // FIXME:
            uint8_t  BitDepth    = Bits2Bytes(W64->BitDepth, RoundingType_Up);
            uint64_t W64Size     = (NumSamples * NumChannels * BitDepth);
            uint64_t FMTSize     = 40;
            
            BitBuffer_WriteGUUID(BitB, GUIDString, W64_RIFF_GUIDString);
            BitBuffer_WriteBits(BitB, LSByteFirst, LSBitFirst, 64, W64Size);
            BitBuffer_WriteGUUID(BitB, GUIDString, W64_WAVE_GUIDString);
            BitBuffer_WriteGUUID(BitB, GUIDString, W64_FMT_GUIDString);
            BitBuffer_WriteBits(BitB, LSByteFirst, LSBitFirst, 64, FMTSize);
            W64WriteFMTChunk(W64, BitB);
            BitBuffer_WriteGUUID(BitB, GUIDString, W64_DATA_GUIDString);
            BitBuffer_WriteBits(BitB, LSByteFirst, LSBitFirst, 64, NumSamples);
        } else if (Options == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Audio2DContainer Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void W64AppendSamples(void *Options, void *Contanier, BitBuffer *BitB) {
        if (Options != NULL && Contanier != NULL && BitB != NULL) {
            W64Options *W64         = Options;
            Audio2DContainer *Audio = Contanier;
            uint64_t NumChannels    = Audio2DContainer_GetNumChannels(Audio);
            uint64_t BitDepth       = Bits2Bytes(Audio2DContainer_GetBitDepth(Audio), RoundingType_Up);
            uint64_t NumSamples     = Audio2DContainer_GetNumSamples(Audio);
            Audio_Types Type        = Audio2DContainer_GetType(Audio);
            if (Type == (AudioType_Signed | AudioType_Integer8)) {
                int8_t **Samples  = (int8_t**)    Audio2DContainer_GetArray(Audio);
                for (uint32_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint16_t Channel = 0; Channel < NumChannels; Channel++) {
                        BitBuffer_WriteBits(BitB, LSByteFirst, LSBitFirst, BitDepth, Samples[Channel][Sample]);
                    }
                }
            } else if (Type == (AudioType_Unsigned | AudioType_Integer8)) {
                uint8_t **Samples = (uint8_t**)   Audio2DContainer_GetArray(Audio);
                for (uint32_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint16_t Channel = 0; Channel < NumChannels; Channel++) {
                        BitBuffer_WriteBits(BitB, LSByteFirst, LSBitFirst, BitDepth, Samples[Channel][Sample]);
                    }
                }
            } else if (Type == (AudioType_Signed | AudioType_Integer16)) {
                int16_t **Samples = (int16_t**)   Audio2DContainer_GetArray(Audio);
                for (uint32_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint16_t Channel = 0; Channel < NumChannels; Channel++) {
                        BitBuffer_WriteBits(BitB, LSByteFirst, LSBitFirst, BitDepth, Samples[Channel][Sample]);
                    }
                }
            } else if (Type == (AudioType_Unsigned | AudioType_Integer16)) {
                uint16_t **Samples = (uint16_t**) Audio2DContainer_GetArray(Audio);
                for (uint32_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint16_t Channel = 0; Channel < NumChannels; Channel++) {
                        BitBuffer_WriteBits(BitB, LSByteFirst, LSBitFirst, BitDepth, Samples[Channel][Sample]);
                    }
                }
            } else if (Type == (AudioType_Signed | AudioType_Integer32)) {
                int32_t  **Samples = (int32_t**)  Audio2DContainer_GetArray(Audio);
                for (uint32_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint16_t Channel = 0; Channel < NumChannels; Channel++) {
                        BitBuffer_WriteBits(BitB, LSByteFirst, LSBitFirst, BitDepth, Samples[Channel][Sample]);
                    }
                }
            } else if (Type == (AudioType_Unsigned | AudioType_Integer32)) {
                uint32_t **Samples = (uint32_t**) Audio2DContainer_GetArray(Audio);
                for (uint32_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint16_t Channel = 0; Channel < NumChannels; Channel++) {
                        BitBuffer_WriteBits(BitB, LSByteFirst, LSBitFirst, BitDepth, Samples[Channel][Sample]);
                    }
                }
            }
        } else if (Options == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Options Pointer is NULL"));
        } else if (Contanier == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Container Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    static void RegisterEncoder_W64(OVIA *Ovia) {
        Ovia->NumEncoders                                    += 1;
        uint64_t EncoderIndex                                 = Ovia->NumEncoders;
        Ovia->Encoders                                        = realloc(Ovia->Encoders, sizeof(OVIAEncoder) * Ovia->NumEncoders);
        
        Ovia->Encoders[EncoderIndex].EncoderID                = CodecID_W64;
        Ovia->Encoders[EncoderIndex].MediaType                = MediaType_Audio2D;
        Ovia->Encoders[EncoderIndex].Function_Initialize[0]   = W64Options_Init;
        Ovia->Encoders[EncoderIndex].Function_WriteHeader[0]  = W64WriteHeader;
        Ovia->Encoders[EncoderIndex].Function_Encode[0]       = W64AppendSamples;
        Ovia->Encoders[EncoderIndex].Function_WriteFooter[0]  = NULL;
        Ovia->Encoders[EncoderIndex].Function_Deinitialize[0] = W64Options_Deinit;
    }
    
    static OVIACodecRegistry Register_W64Encoder = {
        .Function_RegisterEncoder[CodecID_W64 - 1]            = RegisterEncoder_W64,
    };
    
#ifdef __cplusplus
}
#endif
