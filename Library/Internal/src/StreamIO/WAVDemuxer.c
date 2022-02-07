#include "../../include/StreamIO/WAVStream.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    /*
     Top level ChunkIDs that DO NOT have a size field
     
     WAVE
     INFO
     */
    
    void WAVSkipPadding(BitBuffer *BitB, uint32_t SubChunkSize) {
        if (IsOdd(SubChunkSize) == true) {
            BitBuffer_Seek(BitB, 8);
        }
    }
    
    static void WAVParseDATAChunk(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            uint32_t ChunkSize = BitBuffer_ReadBits(ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, BitB, 32);
            OVIA_SetNumSamples(Ovia, ChunkSize / OVIA_GetNumChannels(Ovia));
        } else if (Ovia == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    static void WAVParseFMTChunk(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            uint32_t ChunkSize = BitBuffer_ReadBits(ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, BitB, 32);
            
            OVIA_WAV_SetCompressionType(Ovia, BitBuffer_ReadBits(ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, BitB, 16)); // 1
            OVIA_SetNumChannels(Ovia, BitBuffer_ReadBits(ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, BitB, 16)); // 2
            OVIA_SetSampleRate(Ovia, BitBuffer_ReadBits(ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, BitB, 32)); // 44100
            BitBuffer_Seek(BitB, 32); // ByteRate
            BitBuffer_Seek(BitB, 16); // BlockAlign
            //OVIA_WAV_SetBlockAlignment(Ovia, BitBuffer_ReadBits(ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, BitB, 32));
            OVIA_SetBitDepth(Ovia, BitBuffer_ReadBits(ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, BitB, 16)); // 16
            if (ChunkSize == 18) {
                uint16_t CBSize             = BitBuffer_ReadBits(ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, BitB, 16);
                BitBuffer_Seek(BitB, Bytes2Bits(CBSize - 16));
            } else if (ChunkSize == 40) {
                uint16_t CBSize             = BitBuffer_ReadBits(ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, BitB, 16);
                uint16_t ValidBitsPerSample = BitBuffer_ReadBits(ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, BitB, 16);
                if (ValidBitsPerSample != 0) {
                    OVIA_SetBitDepth(Ovia, ValidBitsPerSample);
                }
                uint32_t  SpeakerMask       = BitBuffer_ReadBits(ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, BitB, 32);
                uint8_t  *BinaryGUIDFormat  = BitBuffer_ReadGUUID(BitB, BinaryGUID);
                BitBuffer_Seek(BitB, Bytes2Bits(CBSize - 22));
            }
        } else if (Ovia == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void WAVParseMetadata(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            uint32_t ChunkID   = BitBuffer_ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 32);
            uint32_t ChunkSize = BitBuffer_ReadBits(ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, BitB, 32);
            
            switch (ChunkID) {
                case WAV_LIST:
                    break;
                case WAV_FMT:
                    WAVParseFMTChunk(Ovia, BitB);
                    break;
                case WAV_WAVE:
                    BitBuffer_Seek(BitB, 32);
                    break;
                case WAV_DATA:
                    WAVParseDATAChunk(Ovia, BitB);
                    break;
                default:
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Invalid ChunkID: 0x%X"), ChunkID);
                    break;
            }
        } else if (Ovia == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    AudioContainer *WAVExtractSamples(OVIA *Ovia, BitBuffer *BitB) {
        AudioContainer *Audio = NULL;
        if (Ovia != NULL && BitB != NULL) {
            uint64_t BitDepth     = OVIA_GetBitDepth(Ovia);
            uint64_t NumChannels  = OVIA_GetNumChannels(Ovia);
            uint64_t SampleRate   = OVIA_GetSampleRate(Ovia);
            uint64_t NumSamples   = OVIA_GetNumSamples(Ovia);
            if (BitDepth <= 8) {
                Audio = AudioContainer_Init(AudioType_Integer8, BitDepth, NumChannels, SampleRate, NumSamples);
                uint8_t **Samples = (uint8_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        Samples[Channel][Sample] = BitBuffer_ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Bits2Bytes(BitDepth, Yes));
                    }
                }
            } else if (BitDepth > 8 && BitDepth <= 16) {
                Audio = AudioContainer_Init(AudioType_Integer16, BitDepth, NumChannels, SampleRate, NumSamples);
                uint16_t **Samples = (uint16_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        Samples[Channel][Sample] = BitBuffer_ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Bits2Bytes(BitDepth, Yes));
                    }
                }
            } else if (BitDepth > 16 && BitDepth <= 32) {
                Audio = AudioContainer_Init(AudioType_Integer32, BitDepth, NumChannels, SampleRate, NumSamples);
                uint32_t **Samples = (uint32_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        Samples[Channel][Sample] = BitBuffer_ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Bits2Bytes(BitDepth, Yes));
                    }
                }
            }
        } else if (Ovia == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
        return Audio;
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
