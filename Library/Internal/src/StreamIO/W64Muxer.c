#include "../../include/StreamIO/W64Stream.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h" /* Included for Assertions */
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    // To encode W64 i'll need to read the values from the struct and write it to the file
    
    static void W64WriteFMTChunk(AudioScape2D *Audio, BitBuffer *BitB) {
        AssertIO(Audio != NULL);
        AssertIO(BitB != NULL);
        uint64_t NumChannels = AudioScape2D_GetNumSamples(Audio);
        uint64_t SampleRate  = AudioScape2D_GetSampleRate(Audio);
        uint8_t  BitDepth    = Bits2Bytes(AudioScape2D_GetBitDepth(Audio), RoundingType_Up);
        uint64_t ByteRate    = CalculateW64ByteRate(NumChannels, BitDepth, SampleRate);
        uint64_t BlockAlign  = CalculateW64BlockAlign(NumChannels, BitDepth);
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 16, 0);
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 16, NumChannels);
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32, SampleRate);
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32, ByteRate);
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32, BlockAlign);
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 16, BitDepth);
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

        AudioScape2D *Audio = NULL;
        
        BitBuffer_WriteGUUID(BitB, GUUIDType_GUIDString, W64_RIFF_GUIDString);
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 64, W64Size);
        BitBuffer_WriteGUUID(BitB, GUUIDType_GUIDString, W64_WAVE_GUIDString);
        BitBuffer_WriteGUUID(BitB, GUUIDType_GUIDString, W64_FMT_GUIDString);
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 64, FMTSize);
        W64WriteFMTChunk(Audio, BitB);
        BitBuffer_WriteGUUID(BitB, GUUIDType_GUIDString, W64_DATA_GUIDString);
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 64, NumSamples);
    }
    
    void W64AppendSamples(W64Options *Options, void *Container, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(Container != NULL);
        AssertIO(BitB != NULL);
        W64Options *W64         = Options;
        AudioScape2D *Audio = Container;
        uint64_t NumChannels    = AudioScape2D_GetNumChannels(Audio);
        uint64_t BitDepth       = Bits2Bytes(AudioScape2D_GetBitDepth(Audio), RoundingType_Up);
        uint64_t NumSamples     = AudioScape2D_GetNumSamples(Audio);
        PlatformIOTypes Type = AudioScape2D_GetType(Audio);
        if (Type == (PlatformIOType_Signed | PlatformIOType_Integer8)) {
            int8_t **Samples  = (int8_t**)    AudioScape2D_GetArray(Audio);
            for (uint32_t Sample = 0; Sample < NumSamples; Sample++) {
                for (uint16_t Channel = 0; Channel < NumChannels; Channel++) {
                    BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, BitDepth, Samples[Channel][Sample]);
                }
            }
        } else if (Type == (PlatformIOType_Unsigned | PlatformIOType_Integer8)) {
            uint8_t **Samples = (uint8_t**)   AudioScape2D_GetArray(Audio);
            for (uint32_t Sample = 0; Sample < NumSamples; Sample++) {
                for (uint16_t Channel = 0; Channel < NumChannels; Channel++) {
                    BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, BitDepth, Samples[Channel][Sample]);
                }
            }
        } else if (Type == (PlatformIOType_Signed | PlatformIOType_Integer16)) {
            int16_t **Samples = (int16_t**)   AudioScape2D_GetArray(Audio);
            for (uint32_t Sample = 0; Sample < NumSamples; Sample++) {
                for (uint16_t Channel = 0; Channel < NumChannels; Channel++) {
                    BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, BitDepth, Samples[Channel][Sample]);
                }
            }
        } else if (Type == (PlatformIOType_Unsigned | PlatformIOType_Integer16)) {
            uint16_t **Samples = (uint16_t**) AudioScape2D_GetArray(Audio);
            for (uint32_t Sample = 0; Sample < NumSamples; Sample++) {
                for (uint16_t Channel = 0; Channel < NumChannels; Channel++) {
                    BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, BitDepth, Samples[Channel][Sample]);
                }
            }
        } else if (Type == (PlatformIOType_Signed | PlatformIOType_Integer32)) {
            int32_t  **Samples = (int32_t**)  AudioScape2D_GetArray(Audio);
            for (uint32_t Sample = 0; Sample < NumSamples; Sample++) {
                for (uint16_t Channel = 0; Channel < NumChannels; Channel++) {
                    BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, BitDepth, Samples[Channel][Sample]);
                }
            }
        } else if (Type == (PlatformIOType_Unsigned | PlatformIOType_Integer32)) {
            uint32_t **Samples = (uint32_t**) AudioScape2D_GetArray(Audio);
            for (uint32_t Sample = 0; Sample < NumSamples; Sample++) {
                for (uint16_t Channel = 0; Channel < NumChannels; Channel++) {
                    BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, BitDepth, Samples[Channel][Sample]);
                }
            }
        }
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
