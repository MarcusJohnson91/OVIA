#include "../../../include/Private/Audio/AIFCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    /*
     Like WAV, Chunks are padded if their size is not even, and the ChunkSize field contains the real size without the padding
     the ChunkSize field does NOT include the ChunkID or ChunkSize fields.
     */
    
    static void AIFParseCOMMChunk(PCMFile *PCM, BitBuffer *BitB) {
        PCM->NumChannels                   = ReadBits(MSByteFirst, LSBitFirst, BitB, 16);
        PCM->NumChannelAgnosticSamples     = ReadBits(MSByteFirst, LSBitFirst, BitB, 32); // A SampleFrame is simply a single sample from all channels.
        PCM->BitDepth                      = ReadBits(MSByteFirst, LSBitFirst, BitB, 16);
        uint16_t SampleRateExponent        = ReadBits(MSByteFirst, LSBitFirst, BitB, 16) - 16446;
        uint64_t SampleRateMantissa        = ReadBits(MSByteFirst, LSBitFirst, BitB, 64);
        if (SampleRateExponent >= 0) {
            PCM->Aud->SampleRate           = SampleRateMantissa << SampleRateExponent;
        } else {
            PCM->Aud->SampleRate           = (SampleRateMantissa + ((1LLU << (-SampleRateExponent - 1)) >> (-SampleRateExponent)));
        }
    }
    
    static void AIFParseNameChunk(PCMFile *PCM, BitBuffer *BitB) {
        uint32_t AIFFNameSize              = ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
        char *SongTitleTagString           = calloc(1, AIFFNameSize * sizeof(char));
        for (uint32_t TagByte = 0UL; TagByte < AIFFNameSize; TagByte++) {
            SongTitleTagString[TagByte]    = ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
        }
        PCM->Aud->Meta->SongTitleTag       = SongTitleTagString;
    }
    
    static void AIFParseAuthorChunk(PCMFile *PCM, BitBuffer *BitB) {
        uint32_t AIFFAuthSize              = ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
        char *AuthorString                 = calloc(1, AIFFAuthSize * sizeof(char));
        for (uint32_t TagByte = 0UL; TagByte < AIFFAuthSize; TagByte++) {
            AuthorString[TagByte]          = ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
        }
        PCM->Aud->Meta->ArtistTag          = AuthorString;
    }
    
    static void AIFParseAnnotationChunk(PCMFile *PCM, BitBuffer *BitB) {
        uint32_t AIFFAnnoSize              = ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
        char *AnnotationString             = calloc(1, AIFFAnnoSize * sizeof(char));
        for (uint32_t TagByte = 0UL; TagByte < AIFFAnnoSize; TagByte++) {
            AnnotationString[TagByte]      = ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
        }
        PCM->Aud->Meta->NumANNOChunks     += 1;
        PCM->Aud->Meta->AnnoChunks[PCM->Aud->Meta->NumANNOChunks - 1] = AnnotationString;
    }
    
    void AIFParseMetadata(PCMFile *PCM, BitBuffer *BitB) {
        PCM->FileSize                      = ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
        AIFChunkIDs AIFFChunkID            = ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
        if (AIFFChunkID == AIF_AIFF || AIFFChunkID == AIF_AIFC) {
            AIFSubChunkIDs AIFFSubChunkID  = ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            uint32_t AIFFSubChunkSize      = ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
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
                    PCM->Aud->AIFOffset    = ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
                    PCM->Aud->AIFBlockSize = ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
                    BitBuffer_Skip(BitB, Bytes2Bits(PCM->Aud->AIFOffset));
                    break;
            }
        } else {
            Log(Log_ERROR, __func__, U8("Invalid ChunkID 0x%X"), AIFFChunkID);
        }
    }
    
    void AIFExtractSamples(AudioContainer *PCM, BitBuffer *BitB, uint64_t NumSamples2Extract, uint32_t **ExtractedSamples) { // I should change this so that the user manages their own buffer
        for (uint32_t Sample = 0UL; Sample < NumSamples2Extract; Sample++) {
            for (uint16_t Channel = 0; Channel < PCM->NumChannels; Channel++) {
                ExtractedSamples[Channel][Sample] = ReadBits(MSByteFirst, MSBitFirst, BitB, PCM->BitDepth);
                BitBuffer_Skip(BitB, 8 - (PCM->BitDepth % 8)); // Skip the Zero'd bits
            }
        }
    }
    
#ifdef __cplusplus
}
#endif
