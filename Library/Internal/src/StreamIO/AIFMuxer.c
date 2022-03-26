#include "../../include/StreamIO/AIFStream.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    static void AIFWriteCOMM(Audio2DContainer *Audio, BitBuffer *BitB) {
        AssertIO(Audio != NULL);
        AssertIO(BitB != NULL);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, AIF_COMM);
        uint16_t COMMSize = 18;
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, COMMSize);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16, Audio2DContainer_GetNumChannels(Audio)); // 2
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, Audio2DContainer_GetNumSamples(Audio)); // 7979748
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16, Audio2DContainer_GetBitDepth(Audio)); // 16
        uint64_t SampleRate = ConvertInteger2Double(Audio2DContainer_GetSampleRate(Audio));
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


        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16, (SampleRate >> 52) + 15360);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 64, 0x8000000000000000LLU | SampleRate << 11); // SampleRate Mantissa
    }

    static void AIFWriteTitle(AIFOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        AIFSkipPadding(BitB, 0);
    }

    static void AIFWriteArtist(AIFOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        AIFSkipPadding(BitB, 0);
    }

    static void AIFWriteSSND(AIFOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, AIF_SSND);
        uint64_t NumSamples  = Options->NumSamples;
        uint64_t NumChannels = Options->NumChannels;
        uint64_t BitDepth    = Bytes2Bits(Bits2Bytes(Options->BitDepth, RoundingType_Up));
        uint64_t Offset      = Options->SampleOffset;
        uint64_t BlockSize   = Options->BlockSize;

        uint32_t ChunkSize   = 8 + ((NumSamples * NumChannels) * Bits2Bytes(BitDepth, RoundingType_Up));
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, ChunkSize);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, Offset);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, BlockSize);
    }

    void AIFWriteHeader(AIFOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, AIF_SSND);
        uint64_t NumSamples  = Options->NumSamples;
        uint64_t NumChannels = Options->NumChannels;
        uint64_t BitDepth    = Bytes2Bits(Bits2Bytes(Options->BitDepth, RoundingType_Up));

        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, AIF_FORM);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, NumSamples * NumChannels * BitDepth); // FIXME: Options Size calculation is wrong
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, AIF_AIFF);
        AIFWriteCOMM(Options, BitB);
        AIFWriteTitle(Options, BitB);
        AIFWriteArtist(Options, BitB);
        AIFWriteSSND(Options, BitB);
    }

    void AIFAppendSamples(AIFOptions *Options, BitBuffer *BitB, Audio2DContainer *Audio) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        AssertIO(Audio != NULL);
        uint64_t NumSamples      = Audio2DContainer_GetNumSamples(Audio);
        uint64_t NumChannels     = Audio2DContainer_GetNumChannels(Audio);
        uint64_t BitDepth        = Audio2DContainer_GetBitDepth(Audio);
        uint64_t Offset          = Options->SampleOffset;
        uint64_t BlockSize       = Options->BlockSize;
        uint8_t  BitDepthRounded = Bytes2Bits(Bits2Bytes(Options->BitDepth, RoundingType_Up));

        MediaIO_AudioTypes AudioType = Audio2DContainer_GetType(Audio);
        if (AudioType == (AudioType_Signed | AudioType_Integer8)) {
            int8_t **Samples  = (int8_t**) Audio2DContainer_GetArray(Audio);
            for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                    BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitDepthRounded, Samples[Channel][Sample]);
                }
            }
        } else if (AudioType == (AudioType_Unsigned | AudioType_Integer8)) {
            uint8_t **Samples  = (uint8_t**) Audio2DContainer_GetArray(Audio);
            for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                    BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitDepthRounded, Samples[Channel][Sample]);
                }
            }
        } else if (AudioType == (AudioType_Signed | AudioType_Integer16)) {
            int16_t **Samples  = (int16_t**) Audio2DContainer_GetArray(Audio);
            for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                    BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitDepthRounded, Samples[Channel][Sample]);
                }
            }
        } else if (AudioType == (AudioType_Unsigned | AudioType_Integer16)) {
            uint16_t **Samples  = (uint16_t**) Audio2DContainer_GetArray(Audio);
            for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                    BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitDepthRounded, Samples[Channel][Sample]);
                }
            }
        } else if (AudioType == (AudioType_Signed | AudioType_Integer32)) {
            int32_t **Samples  = (int32_t**) Audio2DContainer_GetArray(Audio);
            for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                    BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitDepthRounded, Samples[Channel][Sample]);
                }
            }
        } else if (AudioType == (AudioType_Unsigned | AudioType_Integer32)) {
            uint32_t **Samples  = (uint32_t**) Audio2DContainer_GetArray(Audio);
            for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                    BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitDepthRounded, Samples[Channel][Sample]);
                }
            }
        }
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

