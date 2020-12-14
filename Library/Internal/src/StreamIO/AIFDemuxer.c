#include "../../include/Private/StreamIO/AIFStream.h"
#include "../../include/Private/TagIO/AIFTags.h"
#include "../../../Dependencies/FoundationIO/Library/include/TextIO/LogIO.h"
#include "../../../Dependencies/FoundationIO/Library/include/MathIO.h"

#ifdef __cplusplus
extern "C" {
#endif

    /*
     Like WAV, Chunks are padded if their size is not even, and the ChunkSize field contains the real size without the padding
     the ChunkSize field does NOT include the ChunkID or ChunkSize fields.
     */

    static void AIF_Read_Comm(AIFOptions *AIF, BitBuffer *BitB) {
        if (AIF != NULL && BitB != NULL) {
            AIF->NumChannels                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16);
            AIF->NumSamples                    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32); // A SampleFrame is simply a single sample from all channels.
            AIF->BitDepth                      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16);
            AIF->SampleRate_Exponent           = 16446 - BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16);
            AIF->SampleRate_Mantissa           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 64);
            if (AIF->SampleRate_Exponent >= 0) {
                AIF->SampleRate                = AIF->SampleRate_Mantissa << AIF->SampleRate_Exponent;
            } else {
                uint64_t NegatedExponent       = ~AIF->SampleRate_Exponent;
                AIF->SampleRate                = AIF->SampleRate_Mantissa + ((1 << (NegatedExponent - 1)) >> NegatedExponent);
            }
        } else if (AIF == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void AIFParseMetadata(AIFOptions *AIF, BitBuffer *BitB) {
        if (AIF != NULL && BitB != NULL) {
            AIF->FileSize                      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
            AIFChunkIDs AIFFChunkID            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
            if (AIFFChunkID == AIF_AIFF || AIFFChunkID == AIF_AIFC) {
                AIFSubChunkIDs AIFFSubChunkID  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
                uint32_t AIFFSubChunkSize      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
                uint32_t SampleOffset          = 0;
                uint32_t BlockSize             = 0;
                switch (AIFFSubChunkID) {
                    case AIF_ANNO:
                        AIF_Read_Annotation(AIF, BitB);
                        AIFSkipPadding(BitB, AIFFSubChunkSize);
                        break;
                    case AIF_AUTH:
                        AIF_Read_Author(AIF, BitB);
                        AIFSkipPadding(BitB, AIFFSubChunkSize);
                        break;
                    case AIF_COMM:
                        AIF_Read_Comm(AIF, BitB);
                        AIFSkipPadding(BitB, AIFFSubChunkSize);
                        break;
                    case AIF_NAME:
                        AIF_Read_Name(AIF, BitB);
                        AIFSkipPadding(BitB, AIFFSubChunkSize);
                        break;
                    case AIF_SSND:
                        AIF->SampleOffset = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
                        AIF->BlockSize    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
                        break;
                    default:
                        BitBuffer_Seek(BitB, Bytes2Bits(AIFFSubChunkSize));
                        AIFSkipPadding(BitB, AIFFSubChunkSize);
                        break;
                }
            } else {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Invalid ChunkID 0x%X"), AIFFChunkID);
            }
        } else if (AIF == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void AIFExtractSamples(AIFOptions *AIF, BitBuffer *BitB, Audio2DContainer *Audio) { // I should change this so that the user manages their own buffer
        if (AIF != NULL && BitB != NULL && Audio) {
            uint64_t BitDepth     = Bits2Bytes(Audio2DContainer_GetBitDepth(Audio), RoundingType_Up);
            uint64_t NumChannels  = Audio2DContainer_GetNumChannels(Audio);
            uint64_t NumSamples   = Audio2DContainer_GetNumSamples(Audio);
            if (BitDepth <= 8) {
                uint8_t **Samples = (uint8_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        Samples[Channel][Sample] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitDepth);
                    }
                }
            } else if (BitDepth > 8 && BitDepth <= 16) {
                uint16_t **Samples = (uint16_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        Samples[Channel][Sample] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitDepth);
                    }
                }
            } else if (BitDepth > 16 && BitDepth <= 32) {
                uint32_t **Samples = (uint32_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        Samples[Channel][Sample] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitDepth);
                    }
                }
            }
        } else if (AIF == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (Audio == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Audio2DContainer Pointer is NULL"));
        }
    }

#ifdef __cplusplus
}
#endif

