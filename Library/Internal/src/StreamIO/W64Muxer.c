#include "../../include/StreamIO/W64Stream.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h" /* Included for Assertions */
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    // To encode W64 i'll need to read the values from the struct and write it to the file
    
    static void W64WriteFMTChunk(Audio2DContainer *Audio, BitBuffer *BitB) {
        AssertIO(Audio != NULL);
        AssertIO(BitB != NULL);
        uint64_t NumChannels = Audio2DContainer_GetNumSamples(Audio);
        uint64_t SampleRate  = Audio2DContainer_GetSampleRate(Audio);
        uint8_t  BitDepth    = Bits2Bytes(Audio2DContainer_GetBitDepth(Audio), RoundingType_Up);
        uint64_t ByteRate    = CalculateW64ByteRate(NumChannels, BitDepth, SampleRate);
        uint64_t BlockAlign  = CalculateW64BlockAlign(NumChannels, BitDepth);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsRight, BitOrder_MSBitIsRight, 16, 0);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsRight, BitOrder_MSBitIsRight, 16, NumChannels);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsRight, BitOrder_MSBitIsRight, 32, SampleRate);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsRight, BitOrder_MSBitIsRight, 32, ByteRate);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsRight, BitOrder_MSBitIsRight, 32, BlockAlign);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsRight, BitOrder_MSBitIsRight, 16, BitDepth);
    }
    
    void W64WriteHeader(W64Options *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        W64Options *W64      = Options;
        uint64_t NumChannels = W64->NumChannels;
        uint64_t NumSamples  = W64->CompressionFormat; // FIXME:
        uint8_t  BitDepth    = Bits2Bytes(W64->BitDepth, RoundingType_Up);
        uint64_t W64Size     = (NumSamples * NumChannels * BitDepth);
        uint64_t FMTSize     = 40;

        Audio2DContainer *Audio = NULL;
        
        BitBuffer_WriteGUUID(BitB, GUUIDType_GUIDString, W64_RIFF_GUIDString);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsRight, BitOrder_MSBitIsRight, 64, W64Size);
        BitBuffer_WriteGUUID(BitB, GUUIDType_GUIDString, W64_WAVE_GUIDString);
        BitBuffer_WriteGUUID(BitB, GUUIDType_GUIDString, W64_FMT_GUIDString);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsRight, BitOrder_MSBitIsRight, 64, FMTSize);
        W64WriteFMTChunk(Audio, BitB);
        BitBuffer_WriteGUUID(BitB, GUUIDType_GUIDString, W64_DATA_GUIDString);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsRight, BitOrder_MSBitIsRight, 64, NumSamples);
    }
    
    void W64AppendSamples(W64Options *Options, void *Container, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(Container != NULL);
        AssertIO(BitB != NULL);
        W64Options *W64         = Options;
        Audio2DContainer *Audio = Container;
        uint64_t NumChannels    = Audio2DContainer_GetNumChannels(Audio);
        uint64_t BitDepth       = Bits2Bytes(Audio2DContainer_GetBitDepth(Audio), RoundingType_Up);
        uint64_t NumSamples     = Audio2DContainer_GetNumSamples(Audio);
        MediaIO_AudioTypes Type = Audio2DContainer_GetType(Audio);
        if (Type == (AudioType_Signed | AudioType_Integer8)) {
            int8_t **Samples  = (int8_t**)    Audio2DContainer_GetArray(Audio);
            for (uint32_t Sample = 0; Sample < NumSamples; Sample++) {
                for (uint16_t Channel = 0; Channel < NumChannels; Channel++) {
                    BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsRight, BitOrder_MSBitIsRight, BitDepth, Samples[Channel][Sample]);
                }
            }
        } else if (Type == (AudioType_Unsigned | AudioType_Integer8)) {
            uint8_t **Samples = (uint8_t**)   Audio2DContainer_GetArray(Audio);
            for (uint32_t Sample = 0; Sample < NumSamples; Sample++) {
                for (uint16_t Channel = 0; Channel < NumChannels; Channel++) {
                    BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsRight, BitOrder_MSBitIsRight, BitDepth, Samples[Channel][Sample]);
                }
            }
        } else if (Type == (AudioType_Signed | AudioType_Integer16)) {
            int16_t **Samples = (int16_t**)   Audio2DContainer_GetArray(Audio);
            for (uint32_t Sample = 0; Sample < NumSamples; Sample++) {
                for (uint16_t Channel = 0; Channel < NumChannels; Channel++) {
                    BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsRight, BitOrder_MSBitIsRight, BitDepth, Samples[Channel][Sample]);
                }
            }
        } else if (Type == (AudioType_Unsigned | AudioType_Integer16)) {
            uint16_t **Samples = (uint16_t**) Audio2DContainer_GetArray(Audio);
            for (uint32_t Sample = 0; Sample < NumSamples; Sample++) {
                for (uint16_t Channel = 0; Channel < NumChannels; Channel++) {
                    BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsRight, BitOrder_MSBitIsRight, BitDepth, Samples[Channel][Sample]);
                }
            }
        } else if (Type == (AudioType_Signed | AudioType_Integer32)) {
            int32_t  **Samples = (int32_t**)  Audio2DContainer_GetArray(Audio);
            for (uint32_t Sample = 0; Sample < NumSamples; Sample++) {
                for (uint16_t Channel = 0; Channel < NumChannels; Channel++) {
                    BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsRight, BitOrder_MSBitIsRight, BitDepth, Samples[Channel][Sample]);
                }
            }
        } else if (Type == (AudioType_Unsigned | AudioType_Integer32)) {
            uint32_t **Samples = (uint32_t**) Audio2DContainer_GetArray(Audio);
            for (uint32_t Sample = 0; Sample < NumSamples; Sample++) {
                for (uint16_t Channel = 0; Channel < NumChannels; Channel++) {
                    BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsRight, BitOrder_MSBitIsRight, BitDepth, Samples[Channel][Sample]);
                }
            }
        }
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
