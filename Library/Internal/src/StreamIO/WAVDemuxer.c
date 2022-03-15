#include "../../include/StreamIO/WAVStream.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/TextIO/LogIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    /*
     Top level ChunkIDs that DO NOT have a size field
     
     WAVE
     INFO
     */
    
    void WAVSkipPadding(BitBuffer *BitB, uint32_t SubChunkSize) {
        AssertIO(BitB != NULL);
        if (IsOdd(SubChunkSize) == true) {
            BitBuffer_Seek(BitB, 8);
        }
    }
    
    static void WAVParseDATAChunk(OVIA *Ovia, BitBuffer *BitB) {
        AssertIO(Ovia != NULL);
        AssertIO(BitB != NULL);
        uint32_t ChunkSize = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
        OVIA_SetNumSamples(Ovia, ChunkSize / OVIA_GetNumChannels(Ovia));
    }
    
    static void WAVParseFMTChunk(OVIA *Ovia, BitBuffer *BitB) {
        AssertIO(Ovia != NULL);
        AssertIO(BitB != NULL);
        uint32_t ChunkSize = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);

        OVIA_WAV_SetCompressionType(Ovia, BitBuffer_ReadBits(ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, BitB, 16)); // 1
        OVIA_SetNumChannels(Ovia, BitBuffer_ReadBits(ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, BitB, 16)); // 2
        OVIA_SetSampleRate(Ovia, BitBuffer_ReadBits(ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, BitB, 32)); // 44100
        BitBuffer_Seek(BitB, 32); // ByteRate
        BitBuffer_Seek(BitB, 16); // BlockAlign
                                  //OVIA_WAV_SetBlockAlignment(Ovia, BitBuffer_ReadBits(ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, BitB, 32));
        OVIA_SetBitDepth(Ovia, BitBuffer_ReadBits(ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, BitB, 16)); // 16
        if (ChunkSize == 18) {
            uint16_t CBSize             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16);
            BitBuffer_Seek(BitB, Bytes2Bits(CBSize - 16));
        } else if (ChunkSize == 40) {
            uint16_t CBSize             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16);
            uint16_t ValidBitsPerSample = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16);
            if (ValidBitsPerSample != 0) {
                OVIA_SetBitDepth(Ovia, ValidBitsPerSample);
            }
            uint32_t  SpeakerMask       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
            uint8_t  *BinaryGUIDFormat  = BitBuffer_ReadGUUID(BitB, GUUIDType_BinaryGUID);
            BitBuffer_Seek(BitB, Bytes2Bits(CBSize - 22));
        }
    }
    
    void WAVParseMetadata(OVIA *Ovia, BitBuffer *BitB) {
        AssertIO(Ovia != NULL);
        AssertIO(BitB != NULL);
        uint32_t ChunkID   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        uint32_t ChunkSize = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);

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
    }

    Audio2DContainer *WAVExtractSamples(OVIA *Ovia, BitBuffer *BitB) {
        AssertIO(Ovia != NULL);
        AssertIO(BitB != NULL);
        Audio2DContainer *Audio = NULL;
        // Get the ChannelMap to initailize the Audio2DContainer
        uint64_t BitDepth     = OVIA_GetBitDepth(Ovia);
        uint64_t NumChannels  = OVIA_GetNumChannels(Ovia);
        uint64_t SampleRate   = OVIA_GetSampleRate(Ovia);
        uint64_t NumSamples   = OVIA_GetNumSamples(Ovia);
        if (BitDepth <= 8) {
            Audio = Audio2DContainer_Init(AudioType_Integer8, BitDepth, NumChannels, SampleRate, NumSamples);
            uint8_t **Samples = (uint8_t**) Audio2DContainer_GetArray(Audio);
            for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                    Samples[Channel][Sample] = BitBuffer_ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Bits2Bytes(BitDepth, Yes));
                }
            }
        } else if (BitDepth > 8 && BitDepth <= 16) {
            Audio = Audio2DContainer_Init(AudioType_Integer16, BitDepth, NumChannels, SampleRate, NumSamples);
            uint16_t **Samples = (uint16_t**) Audio2DContainer_GetArray(Audio);
            for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                    Samples[Channel][Sample] = BitBuffer_ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Bits2Bytes(BitDepth, Yes));
                }
            }
        } else if (BitDepth > 16 && BitDepth <= 32) {
            Audio = Audio2DContainer_Init(AudioType_Integer32, BitDepth, NumChannels, SampleRate, NumSamples);
            uint32_t **Samples = (uint32_t**) Audio2DContainer_GetArray(Audio);
            for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                    Samples[Channel][Sample] = BitBuffer_ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Bits2Bytes(BitDepth, Yes));
                }
            }
        }
        return Audio;
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
