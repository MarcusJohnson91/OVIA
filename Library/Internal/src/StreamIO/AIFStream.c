#include "../../include/Private/StreamIO/AIFStream.h"
#include "../../../Dependencies/FoundationIO/Library/include/TextIO/LogIO.h"
#include "../../../Dependencies/FoundationIO/Library/include/MathIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef enum AIFSpeakerMask {
        AIFFrontLeft          = 0x1,
        AIFFrontRight         = 0x2,
        AIFFrontCenter        = 0x4,
        AIFRearLeft           = 0x8,
        AIFRearRight          = 0x10,
        AIFSurround           = 0x20,
        AIFLeftCenter         = 0x40,
        AIFRightCenter        = 0x80,
    } AIFSpeakerMask;

    typedef enum AIFTimeStamp {
        AIFFSecondsBeforeUNIX = 2082844800,
    } AIFTimeStamp;

    typedef enum AIFChunkIDs {
        AIF_FORM              = 0x464F524D,
        AIF_AIFF              = 0x41494646,
        AIF_AIFC              = 0x41494643,
    } AIFChunkIDs;

    typedef enum AIFSubChunkIDs {
        AIF_AAPL              = 0x4150504c,
        AIF_AESD              = 0x41455344,
        AIF_ANNO              = 0x414e4e4f,
        AIF_AUTH              = 0x41555448,
        AIF_COMM              = 0x434F4D4D,
        AIF_COMT              = 0x434f4d54,
        AIF_ID3               = 0x49443320,
        AIF_INST              = 0x494e5354,
        AIF_MARK              = 0x4d41524b,
        AIF_MIDI              = 0x4d494449,
        AIF_NAME              = 0x4E414D45,
        AIF_SSND              = 0x53534E44,
    } AIFSubChunkIDs;

    typedef struct AIFOptions {
        uint64_t       SampleRate;
        uint64_t       SampleRate_Mantissa;
        uint32_t       FileSize;
        uint32_t       NumSamples;
        uint32_t       SampleOffset;
        uint32_t       BlockSize;
        int16_t        SampleRate_Exponent;
        uint16_t       NumChannels;
        uint16_t       BitDepth;
    } AIFOptions;

    static const OVIA_Extensions AIFExtensions = {
        .NumExtensions = 3,
        .Extensions    = {
            [0]        = UTF32String("aifc"),
            [1]        = UTF32String("aiff"),
            [2]        = UTF32String("aif"),
        },
    };

    static const OVIA_MIMETypes AIFMIMETypes = {
        .NumMIMETypes = 2,
        .MIMETypes    = {
            [0]       = UTF32String("audio/aiff"),
            [1]       = UTF32String("audio/x-aiff"),
        },
    };

    static const OVIA_MagicIDs AIFMagicIDs = {
        .NumMagicIDs         = 1,
        .MagicIDOffsetInBits = 0,
        .MagicIDSizeInBits   = 32,
        .MagicIDNumber = {
            [0] = (uint8_t[4]){0x46, 0x4F, 0x52, 0x4D},
        },
    };

    static void AIFWriteCOMM(AIFOptions *AIF, BitBuffer *BitB) {
        if (AIF != NULL && BitB != NULL) {
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32, AIF_COMM);
            uint16_t COMMSize = 18;
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32, COMMSize);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16, AIF->NumChannels); // 2
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32, AIF->NumSamples); // 7979748
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16, AIF->BitDepth); // 16
            uint64_t SampleRate = ConvertInteger2Double(AIF->SampleRate);
            // 44100  = 0x400E 0xAC44000000000000
            // 48000  = 0x400E 0xBB80000000000000
            // 88200  = 0x400F 0xAC44000000000000
            // 96000  = 0x400F 0xBB80000000000000
            // 192000 = 0x4010 0xBB80000000000000

            // Treat them as 2 seperate numbers.
            // The first is Unknown.
            // The second is the sample rate

            // So, 0x400X = Unsigned, Exponent = {,,}



            // e = (((int32_t)ext.exponent[0]&0x7F)<<8) | ext.exponent[1];
            // Exponent = (0x4X & 0x7F << 8) | 0x01
            // So, remove the sign bit from the exponent.
            // Shift the Exponent then bitwise add the second half of the exponent...

            // Literally how does that differ from reading the sign bit, then reading the Exponent...
            // If the Exponent is 0x7FFF AND the Mantissa is greater than 0, return 0.
            // e -= 16383 + 63;
            // Subtract 0x403E from the Exponent? that makes no sense...

            // Maybe it's supposed to be Exponent = (Exponent - 0x3FFF) + 63?
            // 0x4010 - (0x4010 - 0x3FFF) = 17
            // 17 + 63 = 80...

            // If (Exponent & 0x8000) >> 15 == 1, the Mantissa is negative.

            // if Exponent & 0x7FFF > 0, Mantissa << Exponent.
            // If Exponent & 0x7FFF < 0, Mantissa >> -Exponent.

            // Well, let's try it.
            // Sign = 0, positive.
            // Exponent = 0x4010 - 0x3FFF = 17, 17 + 63 = 80.
            // Mantissa is positive.
            // Mantissa << 80? that seems excessive as fuck but ok
            // 0xAC44






            // LibAV's Algorithm for extrcting the sample rate:

            // Read the first 16 bits as the Exponent. (Includes sign bit)
            // Read the next  64 bits as the Mantissa.

            // Call ldexp with the Mantissa first, followed by the exponent - 16383 - 63 aka 0x3FFF - 0x3F = 0x3FC0
            // ldexp multiplies the Mantissa by 2 to the power of Exponent.
            // aka SampleRate = Mantissa * 2^Exponent.
            // SampleRate = 0xBB80000000000000 * 0x4010 - 0x3FC0 = 80

            // Subtract 16383 from the exponent.
            // 17, 48000.
            // So, subtract 11 from the result of Exponent - 16383 to get the times to multiply it by...
            // 17 - 11 = 6, which is wrong, it needs to be 13.

            // 2 ^ 12 = 4096, 3904 more.
            // FA = 250


            // Ok, easy way to do this I think.

            // Exponent - 16383.
            // Subtract 13 from that.
            // 13 - 12 = -1, or
            // That is the number of times you multiply the Mantissa by.
            // 0xFA = 250
            // 2 * 250 = 500


            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16, (SampleRate >> 52) + 15360);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 64, 0x8000000000000000LLU | SampleRate << 11); // SampleRate Mantissa
        } else if (AIF == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("AIFOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    static void AIFWriteTitle(AIFOptions *AIF, BitBuffer *BitB) {
        if (AIF != NULL && BitB != NULL) {
            AIFSkipPadding(BitB, 0);
        } else if (AIF == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    static void AIFWriteArtist(AIFOptions *AIF, BitBuffer *BitB) {
        if (AIF != NULL && BitB != NULL) {
            AIFSkipPadding(BitB, 0);
        } else if (AIF == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    static void AIFWriteSSND(AIFOptions *AIF, BitBuffer *BitB) {
        if (AIF != NULL && BitB != NULL) {
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32, AIF_SSND);
            uint64_t NumSamples  = AIF->NumSamples;
            uint64_t NumChannels = AIF->NumChannels;
            uint64_t BitDepth    = Bytes2Bits(Bits2Bytes(AIF->BitDepth, RoundingType_Up));
            uint64_t Offset      = AIF->SampleOffset;
            uint64_t BlockSize   = AIF->BlockSize;

            uint32_t ChunkSize   = (uint32_t) 8 + ((NumSamples * NumChannels) * Bits2Bytes(BitDepth, RoundingType_Up));
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32, ChunkSize);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32, Offset);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32, BlockSize);
        } else if (AIF == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void AIFWriteHeader(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            AIFOptions *AIF      = Options;
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32, AIF_SSND);
            uint64_t NumSamples  = AIF->NumSamples;
            uint64_t NumChannels = AIF->NumChannels;
            uint64_t BitDepth    = Bytes2Bits(Bits2Bytes(AIF->BitDepth, RoundingType_Up));

            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32, AIF_FORM);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32, NumSamples * NumChannels * BitDepth); // FIXME: AIF Size calculation is wrong
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32, AIF_AIFF);
            AIFWriteCOMM(AIF, BitB);
            AIFWriteTitle(AIF, BitB);
            AIFWriteArtist(AIF, BitB);
            AIFWriteSSND(AIF, BitB);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void AIFAppendSamples(void *Options, void *Container, BitBuffer *BitB) {
        if (Options != NULL && Container != NULL && BitB != NULL) {
            AIFOptions *AIF          = Options;
            Audio2DContainer *Audio  = Container;
            uint64_t NumSamples      = Audio2DContainer_GetNumSamples(Audio);
            uint64_t NumChannels     = Audio2DContainer_GetNumChannels(Audio);
            uint64_t BitDepth        = Audio2DContainer_GetBitDepth(Audio);
            uint64_t Offset          = AIF->SampleOffset;
            uint64_t BlockSize       = AIF->BlockSize;
            uint8_t  BitDepthRounded = Bytes2Bits(Bits2Bytes(AIF->BitDepth, RoundingType_Up));

            MediaIO_AudioTypes AudioType = Audio2DContainer_GetType(Audio);
            if (AudioType == (AudioType_Signed | AudioType_Integer8)) {
                int8_t **Samples  = (int8_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, BitDepthRounded, Samples[Channel][Sample]);
                    }
                }
            } else if (AudioType == (AudioType_Unsigned | AudioType_Integer8)) {
                uint8_t **Samples  = (uint8_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, BitDepthRounded, Samples[Channel][Sample]);
                    }
                }
            } else if (AudioType == (AudioType_Signed | AudioType_Integer16)) {
                int16_t **Samples  = (int16_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, BitDepthRounded, Samples[Channel][Sample]);
                    }
                }
            } else if (AudioType == (AudioType_Unsigned | AudioType_Integer16)) {
                uint16_t **Samples  = (uint16_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, BitDepthRounded, Samples[Channel][Sample]);
                    }
                }
            } else if (AudioType == (AudioType_Signed | AudioType_Integer32)) {
                int32_t **Samples  = (int32_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, BitDepthRounded, Samples[Channel][Sample]);
                    }
                }
            } else if (AudioType == (AudioType_Unsigned | AudioType_Integer32)) {
                uint32_t **Samples  = (uint32_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, BitDepthRounded, Samples[Channel][Sample]);
                    }
                }
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (Container == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Audio2DContainer Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    /*
     Like WAV, Chunks are padded if their size is not even, and the ChunkSize field contains the real size without the padding
     the ChunkSize field does NOT include the ChunkID or ChunkSize fields.
     */

    static void AIFReadCOMMChunk(AIFOptions *AIF, BitBuffer *BitB) {
        if (AIF != NULL && BitB != NULL) {
            AIF->NumChannels                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16);
            AIF->NumSamples                    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32); // A SampleFrame is simply a single sample from all channels.
            AIF->BitDepth                      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16);
            AIF->SampleRate_Exponent           = 16446 - BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16);
            AIF->SampleRate_Mantissa           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 64);
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

    void AIFReadMetadata(TagIO_Tags *Tags, BitBuffer *BitB) {
        if (Tags != NULL && BitB != NULL) {
            uint32_t AIFFSubChunkID        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
            uint32_t AIFFSubChunkSize      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
            switch (AIFFSubChunkID) {
                case AIF_AAPL:
                    BitBuffer_Seek(BitB, Bytes2Bits(AIFFSubChunkSize));
                    AIFSkipPadding(BitB, AIFFSubChunkSize);
                    break;
                case AIF_AESD:
                    BitBuffer_Seek(BitB, Bytes2Bits(AIFFSubChunkSize));
                    AIFSkipPadding(BitB, AIFFSubChunkSize);
                    break;
                case AIF_ANNO:
                    AIFReadAnnotationChunk(Tags, BitB);
                    AIFSkipPadding(BitB, AIFFSubChunkSize);
                    break;
                case AIF_AUTH:
                    AIFReadAuthorChunk(Tags, BitB);
                    AIFSkipPadding(BitB, AIFFSubChunkSize);
                    break;
                case AIF_COMM:
                    AIFReadCOMMChunk(Tags, BitB);
                    AIFSkipPadding(BitB, AIFFSubChunkSize);
                    break;
                case AIF_COMT:
                    AIFReadCommentChunk(Tags, BitB);
                    break;
                case AIF_ID3:
                    BitBuffer_Seek(BitB, Bytes2Bits(AIFFSubChunkSize));
                    AIFSkipPadding(BitB, AIFFSubChunkSize);
                    break;
                case AIF_INST:
                    BitBuffer_Seek(BitB, Bytes2Bits(AIFFSubChunkSize));
                    AIFSkipPadding(BitB, AIFFSubChunkSize);
                    break;
                case AIF_MARK:
                    BitBuffer_Seek(BitB, Bytes2Bits(AIFFSubChunkSize));
                    AIFSkipPadding(BitB, AIFFSubChunkSize);
                    break;
                case AIF_MIDI:
                    BitBuffer_Seek(BitB, Bytes2Bits(AIFFSubChunkSize));
                    AIFSkipPadding(BitB, AIFFSubChunkSize);
                    break;
                case AIF_NAME:
                    AIFReadNameChunk(Tags, BitB);
                    AIFSkipPadding(BitB, AIFFSubChunkSize);
                    break;
                case AIF_SSND:
                    /*
                    AIF->SampleOffset = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
                    AIF->BlockSize    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 32);
                     */
                     break;
            }
        } else if (Tags == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Tags Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void AIFExtractSamples(void *Options, BitBuffer *BitB, Audio2DContainer *Audio) { // I should change this so that the user manages their own buffer
        if (Options != NULL && BitB != NULL) {
            AIFOptions *AIF       = Options;
            uint64_t BitDepth     = Bits2Bytes(AIF->BitDepth, RoundingType_Up);
            uint64_t NumChannels  = AIF->NumChannels;
            uint64_t NumSamples   = AIF->NumSamples;

            if (BitDepth <= 8) {
                uint8_t **Samples = (uint8_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        Samples[Channel][Sample] = (uint8_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, BitDepth);
                    }
                }
            } else if (BitDepth > 8 && BitDepth <= 16) {
                uint16_t **Samples = (uint16_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        Samples[Channel][Sample] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, BitDepth);
                    }
                }
            } else if (BitDepth > 16 && BitDepth <= 32) {
                uint32_t **Samples = (uint32_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        Samples[Channel][Sample] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, BitDepth);
                    }
                }
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void *AIFOptions_Init(void) {
        AIFOptions *AIF = calloc(1, sizeof(AIFOptions));
        return AIF;
    }

    void AIFSkipPadding(BitBuffer *BitB, uint32_t SubChunkSize) {
        if (IsOdd(SubChunkSize) == true) {
            BitBuffer_Seek(BitB, 8);
        }
    }

    void AIFOptions_Deinit(void *Options) {
        free(Options);
    }

    static const OVIADecoder AIFDecoder = {
        .Initalize   = AIFOptions_Init,
        .Parse       = AIFReadMetadata,
        .Decode      = AIFExtractSamples,
        .Deinitalize = AIFOptions_Deinit,
        .MagicIDs    = &AIFMagicIDs,
        .Extensions  = &AIFExtensions,
        .MediaType   = MediaType_Audio2D,
        .DecoderID   = CodecID_PCMAudio,
    };

    static const OVIAEncoder AIFEncoder = {
        .Initalize    = AIFOptions_Init,
        .WriteHeader  = AIFWriteHeader,
        .Encode       = AIFAppendSamples,
        .Deinitalize  = AIFOptions_Deinit,
        .EncoderID    = CodecID_PCMAudio,
        .MediaType    = MediaType_Audio2D,
        .Extensions   = &AIFExtensions,
    };

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
