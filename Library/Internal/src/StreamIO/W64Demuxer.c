#include "../../include/Private/StreamIO/RIFFStream.h"
#include "../../../Dependencies/FoundationIO/Library/include/TextIO/LogIO.h"
#include "../../../Dependencies/FoundationIO/Library/include/MathIO.h"

#ifdef __cplusplus
extern "C" {
#endif

    /* Format Decoding */
    static void W64ParseFMTChunk(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            W64Options *W64                  = Options;
            W64->CompressionFormat           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16);
            W64->NumChannels                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16);
            W64->SampleRate                  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
            W64->BlockAlign                  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
            W64->BlockAlignment              = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16);
            W64->BitDepth                    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16);
            // Read the SpeakerMask
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    static void W64ParseBEXTChunk(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            W64Options *W64                  = Options;
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    static void W64ParseLEVLChunk(void *Options, BitBuffer *BitB) { // aka Peak Envelope Chunk
        if (Options != NULL && BitB != NULL) {
            W64Options *W64                  = Options;
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void W64ParseMetadata(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            W64Options *W64          = Options;
            uint8_t *ChunkUUIDString = BitBuffer_ReadGUUID(BitB, BinaryGUID);
            uint64_t W64Size         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 64);
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
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void W64ExtractSamples(void *Options, BitBuffer *BitB, Audio2DContainer *Audio, uint64_t NumSamples) {
        if (Options != NULL && BitB != NULL && Audio != NULL) {
            W64Options *W64        = Options;
            uint64_t BitDepth      = W64->BitDepth;
            uint64_t NumChannels   = W64->NumChannels;
            uint8_t  BitDepthRound = Bytes2Bits(Bits2Bytes(BitDepth, RoundingType_Up));
            if (BitDepth <= 8) {
                uint8_t **Samples = (uint8_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        Samples[Channel][Sample] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitDepthRound);
                    }
                }
            } else if (BitDepth <= 16) {
                uint16_t **Samples = (uint16_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        Samples[Channel][Sample] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitDepthRound);
                    }
                }
            } else if (BitDepth <= 32) {
                uint32_t **Samples = (uint32_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        Samples[Channel][Sample] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitDepthRound);
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
    }

#ifdef __cplusplus
}
#endif

