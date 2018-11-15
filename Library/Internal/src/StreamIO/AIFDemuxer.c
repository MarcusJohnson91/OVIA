#include "../../../../Dependencies/FoundationIO/libFoundationIO/include/Macros.h"
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
            BitBuffer_Seek(BitB, 8);
        }
    }
    
    static void AIFParseCOMMChunk(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            uint16_t NumChannels               = BitBuffer_ReadBits(MSByteFirst, MSBitFirst, BitB, 16);
            OVIA_SetNumChannels(Ovia, NumChannels);
            uint32_t NumSamples                = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32); // A SampleFrame is simply a single sample from all channels.
            OVIA_SetNumSamples(Ovia, NumSamples);
            uint16_t BitDepth                  = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 16);
            OVIA_SetBitDepth(Ovia, BitDepth);
            uint16_t SampleRateExponent        = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 16) - 16446;
            uint64_t SampleRateMantissa        = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 64);
            if (SampleRateExponent >= 0) {
                OVIA_SetSampleRate(Ovia, SampleRateMantissa << SampleRateExponent);
            } else {
                OVIA_SetSampleRate(Ovia, SampleRateMantissa + ((1ULL << (-SampleRateExponent - 1)) >> (-SampleRateExponent)));
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void AIFParseNameChunk(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            uint32_t TitleSize                 = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            UTF8    *Title                     = calloc(TitleSize, sizeof(UTF8));
            for (uint32_t TitleByte = 0; TitleByte < TitleSize; TitleByte++) {
                Title[TitleByte]               = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            }
            OVIA_SetTag(Ovia, TitleTag, Title);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void AIFParseAuthorChunk(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            uint32_t AuthorSize                = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            UTF8    *Author                    = calloc(AuthorSize, sizeof(UTF8));
            for (uint32_t AuthorByte = 0; AuthorByte < AuthorSize; AuthorByte++) {
                Author[AuthorByte]             = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            }
            OVIA_SetTag(Ovia, AuthorTag, Author);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void AIFParseAnnotationChunk(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            uint32_t AnnotationSize            = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            UTF8    *Annotation                = calloc(AnnotationSize, sizeof(UTF8));
            for (uint32_t AnnotationByte = 0; AnnotationByte < AnnotationSize; AnnotationByte++) {
                Annotation[AnnotationByte]     = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            }
            OVIA_SetTag(Ovia, AnnotationTag, Annotation);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void AIFParseMetadata(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            uint32_t FileSize                  = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            OVIA_SetFileSize(Ovia, FileSize);
            AIFChunkIDs AIFFChunkID            = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            if (AIFFChunkID == AIF_AIFF || AIFFChunkID == AIF_AIFC) {
                AIFSubChunkIDs AIFFSubChunkID  = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
                uint32_t AIFFSubChunkSize      = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
                uint32_t SampleOffset          = 0;
                uint32_t BlockSize             = 0;
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
                        BitBuffer_Seek(BitB, Bytes2Bits(AIFFSubChunkSize));
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
                        BitBuffer_Seek(BitB, Bytes2Bits(AIFFSubChunkSize));
                        AIFSkipPadding(BitB, AIFFSubChunkSize);
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
                        AIFParseNameChunk(Ovia, BitB);
                        AIFSkipPadding(BitB, AIFFSubChunkSize);
                        break;
                    case AIF_SSND:
                        SampleOffset = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
                        OVIA_SetSampleRate(Ovia, SampleOffset);
                        BlockSize    = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
                        OVIA_SetBlockSize(Ovia, BlockSize);
                        break;
                }
            } else {
                Log(Log_ERROR, __func__, U8("Invalid ChunkID 0x%X"), AIFFChunkID);
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    AudioContainer *AIFExtractSamples(OVIA *Ovia, BitBuffer *BitB) { // I should change this so that the user manages their own buffer
        AudioContainer *Audio = NULL;
        if (Ovia != NULL && BitB != NULL) {
            uint64_t BitDepth     = OVIA_GetBitDepth(Ovia);
            uint64_t NumChannels  = OVIA_GetNumChannels(Ovia);
            uint64_t SampleRate   = OVIA_GetSampleRate(Ovia);
            uint64_t NumSamples   = OVIA_GetNumSamples(Ovia);
            if (BitDepth <= 8) {
                Audio = AudioContainer_Init(AudioType_Integer8, BitDepth, NumChannels, SampleRate, NumSamples);
                uint8_t **Samples = (uint8_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        Samples[Channel][Sample] = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, Bits2Bytes(BitDepth, Yes));
                    }
                }
            } else if (BitDepth > 8 && BitDepth <= 16) {
                Audio = AudioContainer_Init(AudioType_Integer16, BitDepth, NumChannels, SampleRate, NumSamples);
                uint16_t **Samples = (uint16_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        Samples[Channel][Sample] = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, Bits2Bytes(BitDepth, Yes));
                    }
                }
            } else if (BitDepth > 16 && BitDepth <= 32) {
                Audio = AudioContainer_Init(AudioType_Integer32, BitDepth, NumChannels, SampleRate, NumSamples);
                uint32_t **Samples = (uint32_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        Samples[Channel][Sample] = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, Bits2Bytes(BitDepth, Yes));
                    }
                }
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
        return Audio;
    }
    
#ifdef __cplusplus
}
#endif
