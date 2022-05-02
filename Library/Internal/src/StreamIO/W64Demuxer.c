#include "../../include/StreamIO/W64Stream.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h" /* Included for Assertions */
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    /* Format Decoding */
    static void W64ReadFMTChunk(W64Options *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->CompressionFormat           = (uint16_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16);
        Options->NumChannels                 = (uint16_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16);
        Options->SampleRate                  = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
        Options->BlockAlign                  = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
        Options->BlockAlignment              = (uint16_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16);
        Options->BitDepth                    = (uint16_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16);
        // Read the SpeakerMask
    }
    
    static void W64ReadBEXTChunk(W64Options *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
    }
    
    static void W64ReadLEVLChunk(W64Options *Options, BitBuffer *BitB) { // aka Peak Envelope Chunk
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
    }
    
    void W64ReadMetadata(W64Options *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        uint8_t *ChunkUUIDString = BitBuffer_ReadGUUID(BitB, GUUIDType_BinaryGUID);
        uint64_t W64Size         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 64);
        if (GUUID_Compare(GUUIDType_BinaryGUID, ChunkUUIDString, W64_RIFF_GUIDString) == true) {
            
        } else if (GUUID_Compare(GUUIDType_BinaryGUID, ChunkUUIDString, W64_WAVE_GUIDString) == true) {
            
        } else if (GUUID_Compare(GUUIDType_BinaryGUID, ChunkUUIDString, W64_FMT_GUIDString) == true) {
            W64ReadFMTChunk(Options, BitB);
        } else if (GUUID_Compare(GUUIDType_BinaryGUID, ChunkUUIDString, W64_DATA_GUIDString) == true) {
            
        } else if (GUUID_Compare(GUUIDType_BinaryGUID, ChunkUUIDString, W64_LEVL_GUIDString) == true) {
            
        } else if (GUUID_Compare(GUUIDType_BinaryGUID, ChunkUUIDString, W64_BEXT_GUIDString) == true) {
            W64ReadBEXTChunk(Options, BitB);
        } else if (GUUID_Compare(GUUIDType_BinaryGUID, ChunkUUIDString, W64_FACT_GUIDString) == true) {
            
        } else if (GUUID_Compare(GUUIDType_BinaryGUID, ChunkUUIDString, W64_JUNK_GUIDString) == true) {
            
        } else if (GUUID_Compare(GUUIDType_BinaryGUID, ChunkUUIDString, W64_MRKR_GUIDString) == true) {
            
        } else if (GUUID_Compare(GUUIDType_BinaryGUID, ChunkUUIDString, W64_SUMM_GUIDString) == true) {
            
        } else if (GUUID_Compare(GUUIDType_BinaryGUID, ChunkUUIDString, W64_LIST_GUIDString) == true) {
            
        }
    }
    
    void *W64ExtractSamples(W64Options *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Audio2DContainer *Audio = NULL;
        uint64_t BitDepth      = Options->BitDepth;
        uint64_t NumChannels   = Options->NumChannels;
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
        return Audio;
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
