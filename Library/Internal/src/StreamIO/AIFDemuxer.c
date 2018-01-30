#include <stdlib.h>

#include "../../../Dependencies/BitIO/libBitIO/include/BitIO.h"
#include "../../../Dependencies/BitIO/libBitIO/include/BitIOLog.h"
#include "../../../Dependencies/BitIO/libBitIO/include/BitIOMath.h"

#include "../../include/libPCM.h"
#include "../../include/Private/libPCMTypes.h"
#include "../../include/Private/Audio/AIFCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    /*
     Like WAV, Chunks are padded if their size is not even, and the ChunkSize field contains the real size without the padding
     the ChunkSize field does NOT include the ChunkID or ChunkSize fields.
     */
    
    static void AIFParseCOMMChunk(PCMFile *PCM, BitBuffer *BitB) {
        PCM->NumChannels                   = ReadBits(BitIOMSByteFirst, BitIOLSBitFirst, BitB, 16);
        PCM->NumChannelAgnosticSamples     = ReadBits(BitIOMSByteFirst, BitIOLSBitFirst, BitB, 32); // A SampleFrame is simply a single sample from all channels.
        PCM->BitDepth                      = ReadBits(BitIOMSByteFirst, BitIOLSBitFirst, BitB, 16);
        uint16_t SampleRateExponent        = ReadBits(BitIOMSByteFirst, BitIOLSBitFirst, BitB, 16) - 16446;
        uint64_t SampleRateMantissa        = ReadBits(BitIOMSByteFirst, BitIOLSBitFirst, BitB, 64);
        if (SampleRateExponent >= 0) {
            PCM->AUD->SampleRate           = SampleRateMantissa << SampleRateExponent;
        } else {
            PCM->AUD->SampleRate           = (SampleRateMantissa + ((1LLU << (-SampleRateExponent - 1)) >> (-SampleRateExponent)));
        }
    }
    
    static void AIFParseNameChunk(PCMFile *PCM, BitBuffer *BitB) {
        uint32_t AIFFNameSize              = ReadBits(BitIOMSByteFirst, BitIOLSBitFirst, BitB, 32);
        char *SongTitleTagString           = calloc(1, AIFFNameSize * sizeof(char));
        for (uint32_t TagByte = 0UL; TagByte < AIFFNameSize; TagByte++) {
            SongTitleTagString[TagByte]    = ReadBits(BitIOMSByteFirst, BitIOLSBitFirst, BitB, 8);
        }
        PCM->AUD->Meta->SongTitleTag       = SongTitleTagString;
    }
    
    static void AIFParseAuthorChunk(PCMFile *PCM, BitBuffer *BitB) {
        uint32_t AIFFAuthSize              = ReadBits(BitIOMSByteFirst, BitIOLSBitFirst, BitB, 32);
        char *AuthorString                 = calloc(1, AIFFAuthSize * sizeof(char));
        for (uint32_t TagByte = 0UL; TagByte < AIFFAuthSize; TagByte++) {
            AuthorString[TagByte]          = ReadBits(BitIOMSByteFirst, BitIOLSBitFirst, BitB, 8);
        }
        PCM->AUD->Meta->ArtistTag          = AuthorString;
    }
    
    static void AIFParseAnnotationChunk(PCMFile *PCM, BitBuffer *BitB) {
        uint32_t AIFFAnnoSize              = ReadBits(BitIOMSByteFirst, BitIOLSBitFirst, BitB, 32);
        char *AnnotationString             = calloc(1, AIFFAnnoSize * sizeof(char));
        for (uint32_t TagByte = 0UL; TagByte < AIFFAnnoSize; TagByte++) {
            AnnotationString[TagByte]      = ReadBits(BitIOMSByteFirst, BitIOLSBitFirst, BitB, 8);
        }
        PCM->AUD->Meta->NumANNOChunks     += 1;
        PCM->AUD->Meta->AnnoChunks[PCM->AUD->Meta->NumANNOChunks - 1] = AnnotationString;
    }
    
    void AIFParseMetadata(PCMFile *PCM, BitBuffer *BitB) {
        PCM->FileSize                      = ReadBits(BitIOMSByteFirst, BitIOLSBitFirst, BitB, 32);
        AIFChunkIDs AIFFChunkID            = ReadBits(BitIOMSByteFirst, BitIOLSBitFirst, BitB, 32);
        if (AIFFChunkID == AIF_AIFF || AIFFChunkID == AIF_AIFC) {
            AIFSubChunkIDs AIFFSubChunkID  = ReadBits(BitIOMSByteFirst, BitIOLSBitFirst, BitB, 32);
            uint32_t AIFFSubChunkSize      = ReadBits(BitIOMSByteFirst, BitIOLSBitFirst, BitB, 32);
            switch (AIFFSubChunkID) {
                case AIF_AAPL:
                    BitBuffer_Skip(BitB, Bytes2Bits(AIFFSubChunkSize));
                    IFFSkipPadding(BitB, AIFFSubChunkSize);
                    break;
                case AIF_AESD:
                    BitBuffer_Skip(BitB, Bytes2Bits(AIFFSubChunkSize));
                    IFFSkipPadding(BitB, AIFFSubChunkSize);
                    break;
                case AIF_ANNO:
                    BitBuffer_Skip(BitB, Bytes2Bits(AIFFSubChunkSize));
                    IFFSkipPadding(BitB, AIFFSubChunkSize);
                    break;
                case AIF_AUTH:
                    AIFParseAuthorChunk(PCM, BitB);
                    IFFSkipPadding(BitB, AIFFSubChunkSize);
                    break;
                case AIF_COMM:
                    AIFParseCOMMChunk(PCM, BitB);
                    IFFSkipPadding(BitB, AIFFSubChunkSize);
                    break;
                case AIF_COMT:
                    BitBuffer_Skip(BitB, Bytes2Bits(AIFFSubChunkSize));
                    IFFSkipPadding(BitB, AIFFSubChunkSize);
                    break;
                case AIF_ID3:
                    BitBuffer_Skip(BitB, Bytes2Bits(AIFFSubChunkSize));
                    IFFSkipPadding(BitB, AIFFSubChunkSize);
                    break;
                case AIF_INST:
                    BitBuffer_Skip(BitB, Bytes2Bits(AIFFSubChunkSize));
                    IFFSkipPadding(BitB, AIFFSubChunkSize);
                    break;
                case AIF_MARK:
                    BitBuffer_Skip(BitB, Bytes2Bits(AIFFSubChunkSize));
                    IFFSkipPadding(BitB, AIFFSubChunkSize);
                    break;
                case AIF_MIDI:
                    BitBuffer_Skip(BitB, Bytes2Bits(AIFFSubChunkSize));
                    IFFSkipPadding(BitB, AIFFSubChunkSize);
                    break;
                case AIF_NAME:
                    AIFParseNameChunk(PCM, BitB);
                    IFFSkipPadding(BitB, AIFFSubChunkSize);
                    break;
                case AIF_SSND:
                    PCM->AUD->AIFOffset    = ReadBits(BitIOMSByteFirst, BitIOLSBitFirst, BitB, 32);
                    PCM->AUD->AIFBlockSize = ReadBits(BitIOMSByteFirst, BitIOLSBitFirst, BitB, 32);
                    BitBuffer_Skip(BitB, Bytes2Bits(PCM->AUD->AIFOffset));
                    break;
            }
        } else {
            BitIOLog(BitIOLog_ERROR, __func__, "Invalid ChunkID 0x%X", AIFFChunkID);
        }
    }
    
    void AIFExtractSamples(PCMFile *PCM, BitBuffer *BitB, uint64_t NumSamples2Extract, uint32_t **ExtractedSamples) { // I should change this so that the user manages their own buffer
        for (uint32_t Sample = 0UL; Sample < NumSamples2Extract; Sample++) {
            for (uint16_t Channel = 0; Channel < PCM->NumChannels; Channel++) {
                ExtractedSamples[Channel][Sample] = ReadBits(BitIOMSByteFirst, BitIOMSBitFirst, BitB, PCM->BitDepth);
                BitBuffer_Skip(BitB, 8 - (PCM->BitDepth % 8)); // Skip the Zero'd bits
            }
        }
    }
    
#ifdef __cplusplus
}
#endif
