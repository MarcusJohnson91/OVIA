#include "../../../Dependencies/FoundationIO/libFoundationIO/include/BitIO.h"
#include "../../../Dependencies/FoundationIO/libFoundationIO/include/Log.h"
#include "../../../Dependencies/FoundationIO/libFoundationIO/include/Math.h"

#include "../../include/libPCM.h"
#include "../../include/Private/libPCMTypes.h"
#include "../../include/Private/Audio/AIFCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef union Integer2Double {
        double   Double;
        uint64_t Integer;
    } Integer2Double;
    
    static double ConvertInteger2Double(uint64_t Number) {
        Integer2Double Cast;
        Cast.Integer = Number;
        return Cast.Double;
    }
    
    static void AIFWriteCOMM(PCMFile *PCM, BitBuffer *BitB) {
        WriteBits(MSByteFirst, LSBitFirst, BitB, 32, AIF_COMM);
        uint16_t COMMSize = 18;
        WriteBits(MSByteFirst, LSBitFirst, BitB, 32, COMMSize);
        WriteBits(MSByteFirst, LSBitFirst, BitB, 16, PCM->NumChannels);
        WriteBits(MSByteFirst, LSBitFirst, BitB, 32, PCM->NumChannelAgnosticSamples);
        WriteBits(MSByteFirst, LSBitFirst, BitB, 16, PCM->BitDepth);
        uint64_t SampleRate = ConvertInteger2Double(PCM->Aud->SampleRate);
        WriteBits(MSByteFirst, LSBitFirst, BitB, 16, (SampleRate >> 52) + 15360); // SampleRate Exponent
        WriteBits(MSByteFirst, LSBitFirst, BitB, 64, 0x8000000000000000LLU | SampleRate << 11); // SampleRate Mantissa
    }
    
    static void AIFWriteTitle(PCMFile *PCM, BitBuffer *BitB) {
        if (PCM->Aud->Meta->SongTitleTag != NULL) {
            WriteBits(MSByteFirst, LSBitFirst, BitB, 32, AIF_NAME);
            WriteBits(MSByteFirst, MSBitFirst, BitB, 32, PCM->Aud->Meta->TitleSize);
            for (uint8_t TitleByte = 0; TitleByte < PCM->Aud->Meta->TitleSize; TitleByte++) {
                WriteBits(MSByteFirst, LSBitFirst, BitB, 8, PCM->Aud->Meta->SongTitleTag[TitleByte]);
            }
        }
        IFFSkipPadding(BitB, PCM->Aud->Meta->TitleSize);
    }
    
    static void AIFWriteArtist(PCMFile *PCM, BitBuffer *BitB) {
        if (PCM->Aud->Meta->ArtistTag != NULL) {
            WriteBits(MSByteFirst, LSBitFirst, BitB, 32, AIF_AUTH);
            WriteBits(MSByteFirst, MSBitFirst, BitB, 32, PCM->Aud->Meta->ArtistTagSize);
            for (uint8_t ArtistByte = 0; ArtistByte < PCM->Aud->Meta->ArtistTagSize; ArtistByte++) {
                WriteBits(MSByteFirst, LSBitFirst, BitB, 8, PCM->Aud->Meta->SongTitleTag[ArtistByte]);
            }
        }
        IFFSkipPadding(BitB, PCM->Aud->Meta->ArtistTagSize);
    }
    
    static void AIFWriteSSND(PCMFile *PCM, BitBuffer *BitB) {
        WriteBits(MSByteFirst, LSBitFirst, BitB, 32, AIF_SSND);
        uint32_t ChunkSize = 8 + ((PCM->NumChannelAgnosticSamples * PCM->NumChannels) * Bits2Bytes(PCM->BitDepth, true));
        WriteBits(MSByteFirst, LSBitFirst, BitB, 32, ChunkSize);
        WriteBits(MSByteFirst, LSBitFirst, BitB, 32, PCM->Aud->AIFOffset);
        WriteBits(MSByteFirst, LSBitFirst, BitB, 32, PCM->Aud->AIFBlockSize);
    }
    
    void AIFWriteHeader(PCMFile *PCM, BitBuffer *BitB) {
        WriteBits(MSByteFirst, LSBitFirst, BitB, 32, AIF_FORM);
        WriteBits(MSByteFirst, LSBitFirst, BitB, 32, ((PCM->NumChannelAgnosticSamples * PCM->NumChannels) * Bits2Bytes(PCM->BitDepth, true))); // FIXME: AIF Size calculation is wrong
        WriteBits(MSByteFirst, LSBitFirst, BitB, 32, AIF_AIFF);
        AIFWriteCOMM(PCM, BitB);
        AIFWriteTitle(PCM, BitB);
        AIFWriteArtist(PCM, BitB);
        AIFWriteSSND(PCM, BitB);
    }
    
    void AIFInsertSamples(PCMFile *PCM, BitBuffer *OutputSamples, uint32_t NumSamples2Write, uint32_t **Samples2Write) {
        if (PCM != NULL && OutputSamples != NULL && Samples2Write != NULL) {
            uint64_t ChannelCount = PCM->NumChannels;
            uint64_t BitDepth     = PCM->BitDepth;
            for (uint32_t Sample = 0; Sample < NumSamples2Write; Sample++) {
                for (uint16_t Channel = 0; Channel < ChannelCount; Channel++) {
                    WriteBits(MSByteFirst, MSBitFirst, OutputSamples, BitDepth, Samples2Write[Channel][Sample]);
                    BitBuffer_Skip(OutputSamples, 8 - (BitDepth % 8));
                }
            }
        } else if (PCM == NULL) {
            Log(Log_ERROR, __func__, U8("PCM Pointer is NULL"));
        } else if (OutputSamples == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (Samples2Write == NULL) {
            Log(Log_ERROR, __func__, U8("Samples2Write Pointer is NULL"));
        }
    }
    
#ifdef __cplusplus
}
#endif
