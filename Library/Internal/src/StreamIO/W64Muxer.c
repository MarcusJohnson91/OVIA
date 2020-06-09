#include "../../include/Private/W64Common.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
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
            BitBuffer_WriteBits(BitB, BitIO_ByteOrder_LSByte, BitIO_BitOrder_LSBit, 16, 0);
            BitBuffer_WriteBits(BitB, BitIO_ByteOrder_LSByte, BitIO_BitOrder_LSBit, 16, NumChannels);
            BitBuffer_WriteBits(BitB, BitIO_ByteOrder_LSByte, BitIO_BitOrder_LSBit, 32, SampleRate);
            BitBuffer_WriteBits(BitB, BitIO_ByteOrder_LSByte, BitIO_BitOrder_LSBit, 32, ByteRate);
            BitBuffer_WriteBits(BitB, BitIO_ByteOrder_LSByte, BitIO_BitOrder_LSBit, 32, BlockAlign);
            BitBuffer_WriteBits(BitB, BitIO_ByteOrder_LSByte, BitIO_BitOrder_LSBit, 16, BitDepth);
        } else if (Audio == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Audio2DContainer Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
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
            
            BitBuffer_WriteGUUID(BitB, GUUIDType_GUIDString, W64_RIFF_GUIDString);
            BitBuffer_WriteBits(BitB, BitIO_ByteOrder_LSByte, BitIO_BitOrder_LSBit, 64, W64Size);
            BitBuffer_WriteGUUID(BitB, GUUIDType_GUIDString, W64_WAVE_GUIDString);
            BitBuffer_WriteGUUID(BitB, GUUIDType_GUIDString, W64_FMT_GUIDString);
            BitBuffer_WriteBits(BitB, BitIO_ByteOrder_LSByte, BitIO_BitOrder_LSBit, 64, FMTSize);
            W64WriteFMTChunk(W64, BitB);
            BitBuffer_WriteGUUID(BitB, GUUIDType_GUIDString, W64_DATA_GUIDString);
            BitBuffer_WriteBits(BitB, BitIO_ByteOrder_LSByte, BitIO_BitOrder_LSBit, 64, NumSamples);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Audio2DContainer Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void W64AppendSamples(void *Options, void *Contanier, BitBuffer *BitB) {
        if (Options != NULL && Contanier != NULL && BitB != NULL) {
            W64Options *W64         = Options;
            Audio2DContainer *Audio = Contanier;
            uint64_t NumChannels    = Audio2DContainer_GetNumChannels(Audio);
            uint64_t BitDepth       = Bits2Bytes(Audio2DContainer_GetBitDepth(Audio), RoundingType_Up);
            uint64_t NumSamples     = Audio2DContainer_GetNumSamples(Audio);
            ContainerIO_AudioTypes Type = Audio2DContainer_GetType(Audio);
            if (Type == (AudioType_Signed | AudioType_Integer8)) {
                int8_t **Samples  = (int8_t**)    Audio2DContainer_GetArray(Audio);
                for (uint32_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint16_t Channel = 0; Channel < NumChannels; Channel++) {
                        BitBuffer_WriteBits(BitB, BitIO_ByteOrder_LSByte, BitIO_BitOrder_LSBit, BitDepth, Samples[Channel][Sample]);
                    }
                }
            } else if (Type == (AudioType_Unsigned | AudioType_Integer8)) {
                uint8_t **Samples = (uint8_t**)   Audio2DContainer_GetArray(Audio);
                for (uint32_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint16_t Channel = 0; Channel < NumChannels; Channel++) {
                        BitBuffer_WriteBits(BitB, BitIO_ByteOrder_LSByte, BitIO_BitOrder_LSBit, BitDepth, Samples[Channel][Sample]);
                    }
                }
            } else if (Type == (AudioType_Signed | AudioType_Integer16)) {
                int16_t **Samples = (int16_t**)   Audio2DContainer_GetArray(Audio);
                for (uint32_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint16_t Channel = 0; Channel < NumChannels; Channel++) {
                        BitBuffer_WriteBits(BitB, BitIO_ByteOrder_LSByte, BitIO_BitOrder_LSBit, BitDepth, Samples[Channel][Sample]);
                    }
                }
            } else if (Type == (AudioType_Unsigned | AudioType_Integer16)) {
                uint16_t **Samples = (uint16_t**) Audio2DContainer_GetArray(Audio);
                for (uint32_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint16_t Channel = 0; Channel < NumChannels; Channel++) {
                        BitBuffer_WriteBits(BitB, BitIO_ByteOrder_LSByte, BitIO_BitOrder_LSBit, BitDepth, Samples[Channel][Sample]);
                    }
                }
            } else if (Type == (AudioType_Signed | AudioType_Integer32)) {
                int32_t  **Samples = (int32_t**)  Audio2DContainer_GetArray(Audio);
                for (uint32_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint16_t Channel = 0; Channel < NumChannels; Channel++) {
                        BitBuffer_WriteBits(BitB, BitIO_ByteOrder_LSByte, BitIO_BitOrder_LSBit, BitDepth, Samples[Channel][Sample]);
                    }
                }
            } else if (Type == (AudioType_Unsigned | AudioType_Integer32)) {
                uint32_t **Samples = (uint32_t**) Audio2DContainer_GetArray(Audio);
                for (uint32_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint16_t Channel = 0; Channel < NumChannels; Channel++) {
                        BitBuffer_WriteBits(BitB, BitIO_ByteOrder_LSByte, BitIO_BitOrder_LSBit, BitDepth, Samples[Channel][Sample]);
                    }
                }
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (Contanier == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Container Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
#define NumW64Extensions 1
    
    static const UTF32 *W64Extensions[NumW64Extensions] = {
        [0] = UTF32String("w64"),
    };
    
    static const OVIAEncoder W64Encoder = {
        .EncoderID             = CodecID_PCMAudio,
        .MediaType             = MediaType_Audio2D,
        .NumExtensions         = NumW64Extensions,
        .Extensions            = W64Extensions,
        .Function_Initialize   = W64Options_Init,
        .Function_WriteHeader  = W64WriteHeader,
        .Function_Encode       = W64AppendSamples,
        .Function_Deinitialize = W64Options_Deinit,
    };
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
