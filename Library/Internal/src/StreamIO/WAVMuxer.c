#include "../../include/StreamIO/WAVStream.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    static const uint8_t WAVNULLBinaryGUID[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    
    void WAVWriteHeader(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            /*
             We need to make all WV files Broadcast Wave compatible, by writing a JUNK chunk and renaming it ds64 if the number of samples grows too large for standard WAVE.
             
             So, that means that we also need to use the WriteFooter function pointer to create a function that checks the size, and if the number of samples times the number of channels times the bitdepth is greater than the 4GB barried, we renae the JUNK chunk to ds64.
             */
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    static void WAVWriteFMTChunk(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            WAVOptions *WAV      = Options;
            uint64_t NumChannels = WAV->NumChannels;
            uint64_t SampleRate  = WAV->SampleRate;
            uint64_t BitDepth    = WAV->BitDepth;
            uint8_t CBSize = 46;
            
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32, 40); // ChunkSize
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16, 0xFFFE); // WaveFormatExtensible
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16, NumChannels);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32, SampleRate);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32, SampleRate * NumChannels * BitDepth);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32, WAV->BlockAlign);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16, BitDepth);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16, CBSize);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16, BitDepth); // ValidBitsPerSample
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32, WAV->ChannelMask);
            BitBuffer_WriteGUUID(BitB, GUUIDType_BinaryGUID, WAVNULLBinaryGUID);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    static void WAVWriteLISTChunk(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
             WAVOptions *WAV      = Options;
            // Write the tags
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void WAVAppendSamples(void *Options, void *Contanier, BitBuffer *BitB) { // void *Options, void *Contanier, BitBuffer *BitB
        if (Options != NULL && Contanier != NULL && BitB != NULL) {
            WAVOptions *WAV         = Options;
            Audio2DContainer *Audio = Contanier;
            uint64_t NumChannels    = Audio2DContainer_GetNumChannels(Audio);
            uint64_t BitDepth       = Audio2DContainer_GetBitDepth(Audio);
            uint64_t NumSamples     = Audio2DContainer_GetNumSamples(Audio);
            MediaIO_AudioTypes Type        = Audio2DContainer_GetType(Audio);
            
            if (Type == (AudioType_Unsigned | AudioType_Integer8)) {
                uint8_t **Samples  = (uint8_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, BitDepth, Samples[Channel][Sample]);
                    }
                }
            } else if (Type == (AudioType_Signed | AudioType_Integer8)) {
                int8_t **Samples   = (int8_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, BitDepth, Samples[Channel][Sample]);
                    }
                }
            } else if (Type == (AudioType_Unsigned | AudioType_Integer16)) {
                uint16_t **Samples = (uint16_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, BitDepth, Samples[Channel][Sample]);
                    }
                }
            } else if (Type == (AudioType_Signed | AudioType_Integer16)) {
                int16_t **Samples  = (int16_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, BitDepth, Samples[Channel][Sample]);
                    }
                }
            } else if (Type == (AudioType_Unsigned | AudioType_Integer32)) {
                uint32_t **Samples = (uint32_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, BitDepth, Samples[Channel][Sample]);
                    }
                }
            } else if (Type == (AudioType_Signed | AudioType_Integer32)) {
                int32_t **Samples  = (int32_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, BitDepth, Samples[Channel][Sample]);
                    }
                }
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (Contanier == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Contanier Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void WAVWriteFooter(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
             WAVOptions *WAV          = Options;
            uint64_t    WAVSampleSize = Bits2Bytes(WAV->NumSamplesWritten * WAV->NumChannels * WAV->BitDepth, RoundingType_Up); // Don't forget padding
            if (WAVSampleSize > 0xFFFFFFFF) {
                // Rename the JUNK chunk to ds64,
                // So save the current bit position
                // What do we do if the BitBuffer was written to the file?
                // Well, we could store the current offset, reload the buffer with the first kilobyte or whatever the header may take up, then change it, rewrite that section of the file, then reload where we were...
                // that probably wouldn't work, and if it did, it wouldn't work for true streaming applications.
                
                // A better idea is to just set the WAVE FMT header to 0xFFFFFFFF and set the ds64 block by default.
            }
            // Now check the size of the samples.
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    static const OVIA_Extensions WAVExtensions = {
        .NumExtensions = 3,
        .Extensions    = {
            [0] = UTF32String("wave"),
            [1] = UTF32String("wav"),
            [2] = UTF32String("bwf"),
            [3] = UTF32String("rf64"),
        },
    };
    
    static const OVIAEncoder WAVEncoder = {
        .EncoderID             = CodecID_PCMAudio,
        .MediaType             = MediaType_Audio2D,
        .Extensions            = &WAVExtensions,
        .Function_Initialize   = WAVOptions_Init,
        .Function_WriteHeader  = WAVWriteHeader,
        .Function_Encode       = WAVAppendSamples,
        .Function_WriteFooter  = NULL,
        .Function_Deinitialize = WAVOptions_Deinit,
    };
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
