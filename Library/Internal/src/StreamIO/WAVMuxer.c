#include "../../include/StreamIO/WAVStream.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    static const uint8_t WAVNULLBinaryGUID[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    
    void WAVWriteHeader(WAVOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        /*
         We need to make all WV files Broadcast Wave compatible, by writing a JUNK chunk and renaming it ds64 if the number of samples grows too large for standard WAVE.
         
         So, that means that we also need to use the WriteFooter function pointer to create a function that checks the size, and if the number of samples times the number of channels times the bitdepth is greater than the 4GB barried, we renae the JUNK chunk to ds64.
         */
    }
    
    static void WAVWriteFMTChunk(WAVOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        uint64_t NumChannels = Options->NumChannels;
        uint64_t SampleRate  = Options->SampleRate;
        uint64_t BitDepth    = Options->BitDepth;
        uint8_t CBSize = 46;
        
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32, 40); // ChunkSize
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 16, 0xFFFE); // WaveFormatExtensible
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 16, NumChannels);
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32, SampleRate);
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32, SampleRate * NumChannels * BitDepth);
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32, Options->BlockAlign);
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 16, BitDepth);
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 16, CBSize);
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 16, BitDepth); // ValidBitsPerSample
        BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32, Options->ChannelMask);
        BitBuffer_WriteGUUID(BitB, GUUIDType_BinaryGUID, WAVNULLBinaryGUID);
    }
    
    static void WAVWriteLISTChunk(WAVOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        // Write the tags
    }
    
    void WAVAppendSamples(WAVOptions *Options, AudioScape2D *Container, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(Container != NULL);
        AssertIO(BitB != NULL);
        
        uint64_t NumChannels    = AudioScape2D_GetNumChannels(Container);
        uint64_t BitDepth       = AudioScape2D_GetBitDepth(Container);
        uint64_t NumSamples     = AudioScape2D_GetNumSamples(Container);
        PlatformIOTypes Type = AudioScape2D_GetType(Container);
        
        if (Type == (PlatformIOType_Unsigned | PlatformIOType_Integer8)) {
            uint8_t **Samples  = (uint8_t**) AudioScape2D_GetArray(Container);
            for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                    BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, BitDepth, Samples[Channel][Sample]);
                }
            }
        } else if (Type == (PlatformIOType_Signed | PlatformIOType_Integer8)) {
            int8_t **Samples   = (int8_t**) AudioScape2D_GetArray(Container);
            for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                    BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, BitDepth, Samples[Channel][Sample]);
                }
            }
        } else if (Type == (PlatformIOType_Unsigned | PlatformIOType_Integer16)) {
            uint16_t **Samples = (uint16_t**) AudioScape2D_GetArray(Container);
            for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                    BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, BitDepth, Samples[Channel][Sample]);
                }
            }
        } else if (Type == (PlatformIOType_Signed | PlatformIOType_Integer16)) {
            int16_t **Samples  = (int16_t**) AudioScape2D_GetArray(Container);
            for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                    BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, BitDepth, Samples[Channel][Sample]);
                }
            }
        } else if (Type == (PlatformIOType_Unsigned | PlatformIOType_Integer32)) {
            uint32_t **Samples = (uint32_t**) AudioScape2D_GetArray(Container);
            for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                    BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, BitDepth, Samples[Channel][Sample]);
                }
            }
        } else if (Type == (PlatformIOType_Signed | PlatformIOType_Integer32)) {
            int32_t **Samples  = (int32_t**) AudioScape2D_GetArray(Container);
            for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                    BitBuffer_WriteBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, BitDepth, Samples[Channel][Sample]);
                }
            }
        }
    }
    
    void WAVWriteFooter(WAVOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        
        uint64_t    WAVSampleSize = Bits2Bytes(Options->NumSamplesWritten * Options->NumChannels * Options->BitDepth, RoundingType_Up); // Don't forget padding
        if (WAVSampleSize > 0xFFFFFFFF) {
            // Rename the JUNK chunk to ds64,
            // So save the current bit position
            // What do we do if the BitBuffer was written to the file?
            // Well, we could store the current offset, reload the buffer with the first kilobyte or whatever the header may take up, then change it, rewrite that section of the file, then reload where we were...
            // that probably wouldn't work, and if it did, it wouldn't work for true streaming applications.
            
            // A better idea is to just set the WAVE FMT header to 0xFFFFFFFF and set the ds64 block by default.
        }
        // Now check the size of the samples.
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
