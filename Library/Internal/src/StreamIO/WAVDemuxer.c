#include "../../include/StreamIO/WAVStream.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/TextIO/LogIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    /*
     Top level ChunkIDs that DO NOT have a size field
     
     WAVE
     INFO
     */
    
    static void WAVParseDATAChunk(WAVOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        
    }
    
    static void WAVParseFMTChunk(WAVOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        
        Options->CompressionFormat = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 16);
        Options->NumChannels       = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 16);
        Options->SampleRate        = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32);
        Options->ByteRate          = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32);
        Options->BlockAlign        = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32);
        Options->BitDepth          = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 16);
        if (ChunkSize == 18) {
            uint16_t CBSize        = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 16);
            BitBuffer_Seek(BitB, Bytes2Bits(CBSize - 16));
        } else if (ChunkSize == 40) {
            uint16_t CBSize             = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 16);
            uint16_t ValidBitsPerSample = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 16);
            if (ValidBitsPerSample != 0) {
                Options->BitDepth = ValidBitsPerSample;
            }
            uint32_t  SpeakerMask       = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32);
            uint8_t  *BinaryGUIDFormat  = BitBuffer_ReadGUUID(BitB, GUUIDType_BinaryGUID);
            BitBuffer_Seek(BitB, Bytes2Bits(CBSize - 22));
        }
    }
    
    void WAVParseMetadata(WAVOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        uint32_t ChunkID   = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        uint32_t ChunkSize = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32);
        
        switch (ChunkID) {
            case WAV_LIST:
                break;
            case WAV_FMT:
                WAVParseFMTChunk(Options, BitB, ChunkSize);
                break;
            case WAV_DATA:
                WAVParseDATAChunk(Options, BitB, ChunkSize);
                break;
            case WAV_WAVE:
                BitBuffer_Seek(BitB, 32);
                break;
            default:
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Invalid ChunkID: 0x%X"), ChunkID);
                break;
        }
    }
    
    Audio2DContainer *WAVExtractSamples(WAVOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Audio2DContainer *Audio = NULL;
        // Get the ChannelMap to initailize the Audio2DContainer
        uint64_t BitDepth     = Options->BitDepth;
        uint64_t NumChannels  = Options->NumChannels;
        uint64_t SampleRate   = Options->SampleRate;
        uint64_t NumSamples   = Options->NumSamplesWritten;
        AudioChannelMap *Map  = AudioChannelMap_Init(NumChannels);
        // ^ Add the masks for the channels from OVIA and blah blah blah
        if (BitDepth <= 8) {
            Audio = Audio2DContainer_Init(AudioType_Integer8, Map, SampleRate, NumSamples);
            uint8_t **Samples = (uint8_t**) Audio2DContainer_GetArray(Audio);
            for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                    Samples[Channel][Sample] = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, Bits2Bytes(BitDepth, Yes));
                }
            }
        } else if (BitDepth > 8 && BitDepth <= 16) {
            Audio = Audio2DContainer_Init(AudioType_Integer16, Map, SampleRate, NumSamples);
            uint16_t **Samples = (uint16_t**) Audio2DContainer_GetArray(Audio);
            for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                    Samples[Channel][Sample] = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, Bits2Bytes(BitDepth, Yes));
                }
            }
        } else if (BitDepth > 16 && BitDepth <= 32) {
            Audio = Audio2DContainer_Init(AudioType_Integer32, Map, SampleRate, NumSamples);
            uint32_t **Samples = (uint32_t**) Audio2DContainer_GetArray(Audio);
            for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                    Samples[Channel][Sample] = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, Bits2Bytes(BitDepth, Yes));
                }
            }
        }
        return Audio;
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
