#include "../../../Dependencies/BitIO/libBitIO/include/BitIO.h"
#include "../../../Dependencies/BitIO/libBitIO/include/BitIOLog.h"
#include "../../../Dependencies/BitIO/libBitIO/include/BitIOMath.h"

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
        WriteBits(BitIOMSByte, BitIOLSBit, BitB, 32, AIF_COMM);
        uint16_t COMMSize = 18;
        WriteBits(BitIOMSByte, BitIOLSBit, BitB, 32, COMMSize);
        WriteBits(BitIOMSByte, BitIOLSBit, BitB, 16, PCM->NumChannels);
        WriteBits(BitIOMSByte, BitIOLSBit, BitB, 32, PCM->NumChannelAgnosticSamples);
        WriteBits(BitIOMSByte, BitIOLSBit, BitB, 16, PCM->BitDepth);
        uint64_t SampleRate = ConvertInteger2Double(PCM->AUD->SampleRate);
        WriteBits(BitIOMSByte, BitIOLSBit, BitB, 16, (SampleRate >> 52) + 15360); // SampleRate Exponent
        WriteBits(BitIOMSByte, BitIOLSBit, BitB, 64, 0x8000000000000000LLU | SampleRate << 11); // SampleRate Mantissa
    }
    
    static void AIFWriteTitle(PCMFile *PCM, BitBuffer *BitB) {
        if (PCM->AUD->Meta->SongTitleTag != NULL) {
            WriteBits(BitIOMSByte, BitIOLSBit, BitB, 32, AIF_NAME);
            WriteBits(BitIOMSByte, BitIOMSBit, BitB, 32, PCM->AUD->Meta->TitleSize);
            for (uint8_t TitleByte = 0; TitleByte < PCM->AUD->Meta->TitleSize; TitleByte++) {
                WriteBits(BitIOMSByte, BitIOLSBit, BitB, 8, PCM->AUD->Meta->SongTitleTag[TitleByte]);
            }
        }
        IFFSkipPadding(BitB, PCM->AUD->Meta->TitleSize);
    }
    
    static void AIFWriteArtist(PCMFile *PCM, BitBuffer *BitB) {
        if (PCM->AUD->Meta->ArtistTag != NULL) {
            WriteBits(BitIOMSByte, BitIOLSBit, BitB, 32, AIF_AUTH);
            WriteBits(BitIOMSByte, BitIOMSBit, BitB, 32, PCM->AUD->Meta->ArtistTagSize);
            for (uint8_t ArtistByte = 0; ArtistByte < PCM->AUD->Meta->ArtistTagSize; ArtistByte++) {
                WriteBits(BitIOMSByte, BitIOLSBit, BitB, 8, PCM->AUD->Meta->SongTitleTag[ArtistByte]);
            }
        }
        IFFSkipPadding(BitB, PCM->AUD->Meta->ArtistTagSize);
    }
    
    static void AIFWriteSSND(PCMFile *PCM, BitBuffer *BitB) {
        WriteBits(BitIOMSByte, BitIOLSBit, BitB, 32, AIF_SSND);
        uint32_t ChunkSize = 8 + ((PCM->NumChannelAgnosticSamples * PCM->NumChannels) * Bits2Bytes(PCM->BitDepth, true));
        WriteBits(BitIOMSByte, BitIOLSBit, BitB, 32, ChunkSize);
        WriteBits(BitIOMSByte, BitIOLSBit, BitB, 32, PCM->AUD->AIFOffset);
        WriteBits(BitIOMSByte, BitIOLSBit, BitB, 32, PCM->AUD->AIFBlockSize);
    }
    
    void AIFWriteHeader(PCMFile *PCM, BitBuffer *BitB) {
        WriteBits(BitIOMSByte, BitIOLSBit, BitB, 32, AIF_FORM);
        WriteBits(BitIOMSByte, BitIOLSBit, BitB, 32, ((PCM->NumChannelAgnosticSamples * PCM->NumChannels) * Bits2Bytes(PCM->BitDepth, true))); // FIXME: AIF Size calculation is wrong
        WriteBits(BitIOMSByte, BitIOLSBit, BitB, 32, AIF_AIFF);
        AIFWriteCOMM(PCM, BitB);
        AIFWriteTitle(PCM, BitB);
        AIFWriteArtist(PCM, BitB);
        AIFWriteSSND(PCM, BitB);
    }
    
    void AIFInsertSamples(PCMFile *PCM, BitBuffer *OutputSamples, uint32_t NumSamples2Write, uint32_t **Samples2Write) {
        if (PCM == NULL) {
            BitIOLog(BitIOLog_ERROR, libPCMLibraryName, __func__, "PCM Pointer is NULL");
        } else if (OutputSamples == NULL) {
            BitIOLog(BitIOLog_ERROR, libPCMLibraryName, __func__, "BitBuffer Pointer is NULL");
        } else {
            uint64_t ChannelCount = PCM->NumChannels;
            uint64_t BitDepth     = PCM->BitDepth;
            for (uint32_t Sample = 0; Sample < NumSamples2Write; Sample++) {
                for (uint16_t Channel = 0; Channel < ChannelCount; Channel++) {
                    WriteBits(BitIOMSByte, BitIOMSBit, OutputSamples, BitDepth, Samples2Write[Channel][Sample]);
                    BitBuffer_Skip(OutputSamples, 8 - (BitDepth % 8));
                }
            }
        }
    }
    
#ifdef __cplusplus
}
#endif
