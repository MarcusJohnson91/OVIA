#include "../../../include/Private/Audio/AIFCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    /*
     Like WAV, Chunks are padded if their size is not even, and the ChunkSize field contains the real size without the padding
     the ChunkSize field does NOT include the ChunkID or ChunkSize fields.
     */
    
    void AIFSkipPadding(BitBuffer *BitB, uint32_t SubChunkSize) {
        if (IsOdd(SubChunkSize) == Yes) {
            BitBuffer_Skip(BitB, 8);
        }
    }
    
    static void AIFParseCOMMChunk(OVIA *Ovia, BitBuffer *BitB) {
        uint16_t NumChannels               = ReadBits(MSByteFirst, MSBitFirst, BitB, 16);
        OVIA_SetNumChannels(Ovia, NumChannels);
        uint32_t NumSamples                = ReadBits(MSByteFirst, LSBitFirst, BitB, 32); // A SampleFrame is simply a single sample from all channels.
        OVIA_SetNumSamples(Ovia, NumSamples);
        uint16_t BitDepth                  = ReadBits(MSByteFirst, LSBitFirst, BitB, 16);
        OVIA_SetBitDepth(Ovia, BitDepth);
        uint16_t SampleRateExponent        = ReadBits(MSByteFirst, LSBitFirst, BitB, 16) - 16446;
        uint64_t SampleRateMantissa        = ReadBits(MSByteFirst, LSBitFirst, BitB, 64);
        if (SampleRateExponent >= 0) {
            OVIA_SetSampleRate(Ovia, SampleRateMantissa << SampleRateExponent);
        } else {
            OVIA_SetSampleRate(Ovia, SampleRateMantissa + ((1ULL << (-SampleRateExponent - 1)) >> (-SampleRateExponent)));
        }
    }
    
    static void AIFParseNameChunk(OVIA *Ovia, BitBuffer *BitB) {
        uint32_t TitleSize                 = ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
        UTF8    *Title                     = calloc(TitleSize, sizeof(UTF8));
        for (uint32_t TitleByte = 0; TitleByte < TitleSize; TitleByte++) {
            Title[TitleByte]               = ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
        }
        OVIA_SetTag(Ovia, TitleTag, Title);
    }
    
    static void AIFParseAuthorChunk(OVIA *Ovia, BitBuffer *BitB) {
        uint32_t AuthorSize                = ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
        UTF8    *Author                    = calloc(AuthorSize, sizeof(UTF8));
        for (uint32_t AuthorByte = 0; AuthorByte < AuthorSize; AuthorByte++) {
            Author[AuthorByte]             = ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
        }
        OVIA_SetTag(Ovia, AuthorTag, Author);
    }
    
    static void AIFParseAnnotationChunk(OVIA *Ovia, BitBuffer *BitB) {
        uint32_t AnnotationSize            = ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
        UTF8    *Annotation                = calloc(AnnotationSize, sizeof(UTF8));
        for (uint32_t AnnotationByte = 0; AnnotationByte < AnnotationSize; AnnotationByte++) {
            Annotation[AnnotationByte]     = ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
        }
        OVIA_SetTag(Ovia, AnnotationTag, Annotation);
    }
    
    void AIFParseMetadata(OVIA *Ovia, BitBuffer *BitB) {
        uint32_t FileSize                  = ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
        OVIA_SetFileSize(Ovia, FileSize);
        AIFChunkIDs AIFFChunkID            = ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
        if (AIFFChunkID == AIF_AIFF || AIFFChunkID == AIF_AIFC) {
            AIFSubChunkIDs AIFFSubChunkID  = ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            uint32_t AIFFSubChunkSize      = ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            switch (AIFFSubChunkID) {
                case AIF_AAPL:
                    BitBuffer_Skip(BitB, Bytes2Bits(AIFFSubChunkSize));
                    AIFSkipPadding(BitB, AIFFSubChunkSize);
                    break;
                case AIF_AESD:
                    BitBuffer_Skip(BitB, Bytes2Bits(AIFFSubChunkSize));
                    AIFSkipPadding(BitB, AIFFSubChunkSize);
                    break;
                case AIF_ANNO:
                    BitBuffer_Skip(BitB, Bytes2Bits(AIFFSubChunkSize));
                    AIFSkipPadding(BitB, AIFFSubChunkSize);
                    break;
                case AIF_AUTH:
                    AIFParseAuthorChunk(Ovia, BitB);
                    AIFSkipPadding(BitB, AIFFSubChunkSize);
                    break;
                case AIF_COMM:
                    AIFParseCOMMChunk(Ovia, BitB);
                    AIFSkipPadding(BitB, AIFFSubChunkSize);
                    break;
                case AIF_COMT:
                    BitBuffer_Skip(BitB, Bytes2Bits(AIFFSubChunkSize));
                    AIFSkipPadding(BitB, AIFFSubChunkSize);
                    break;
                case AIF_ID3:
                    BitBuffer_Skip(BitB, Bytes2Bits(AIFFSubChunkSize));
                    AIFSkipPadding(BitB, AIFFSubChunkSize);
                    break;
                case AIF_INST:
                    BitBuffer_Skip(BitB, Bytes2Bits(AIFFSubChunkSize));
                    AIFSkipPadding(BitB, AIFFSubChunkSize);
                    break;
                case AIF_MARK:
                    BitBuffer_Skip(BitB, Bytes2Bits(AIFFSubChunkSize));
                    AIFSkipPadding(BitB, AIFFSubChunkSize);
                    break;
                case AIF_MIDI:
                    BitBuffer_Skip(BitB, Bytes2Bits(AIFFSubChunkSize));
                    AIFSkipPadding(BitB, AIFFSubChunkSize);
                    break;
                case AIF_NAME:
                    AIFParseNameChunk(Ovia, BitB);
                    AIFSkipPadding(BitB, AIFFSubChunkSize);
                    break;
                case AIF_SSND:
                    uint32_t SampleOffset   = ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
                    OVIA_SetSampleOffset(Ovia, SampleOffset);
                    
                    uint32_t BlockSize      = ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
                    OVIA_SetBlockSize(Ovia, BlockSize);
                    BitBuffer_Skip(BitB, BlockSize);
                    break;
            }
        } else {
            Log(Log_ERROR, __func__, U8("Invalid ChunkID 0x%X"), AIFFChunkID);
        }
    }
    
    void AIFExtractSamples(AudioContainer *Ovia, BitBuffer *BitB, uint64_t NumSamples2Extract, uint32_t **ExtractedSamples) { // I should change this so that the user manages their own buffer
        for (uint32_t Sample = 0UL; Sample < NumSamples2Extract; Sample++) {
            for (uint16_t Channel = 0; Channel < Ovia->NumChannels; Channel++) {
                ExtractedSamples[Channel][Sample] = ReadBits(MSByteFirst, MSBitFirst, BitB, Ovia->BitDepth);
                BitBuffer_Skip(BitB, 8 - (Ovia->BitDepth % 8)); // Skip the Zero'd bits
            }
        }
    }
    
#ifdef __cplusplus
}
#endif
