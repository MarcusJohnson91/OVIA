#include "../../../../Dependencies/FoundationIO/libFoundationIO/include/Macros.h"
#include "../../../include/Private/Audio/WAVCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    /*
     Top level ChunkIDs that DO NOT have a size field
     
     WAVE
     INFO
     */
    
    void WAVSkipPadding(BitBuffer *BitB, uint32_t SubChunkSize) {
        if (IsOdd(SubChunkSize) == true) {
            BitBuffer_Skip(BitB, 8);
        }
    }
    
    static void ReadINFO_IART(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) {
        if (Ovia != NULL && BitB != NULL) {
            UTF8 *Artist                   = calloc(ChunkSize, sizeof(UTF8));
            Artist                         = BitBuffer_ReadUTF8(BitB, ChunkSize);
            OVIA_SetNumTags(Ovia, OVIA_GetNumTags(Ovia) + 1);
            OVIA_SetTag(Ovia, ArtistTag, Artist);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void ReadINFO_ICRD(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) {
        if (Ovia != NULL && BitB != NULL) {
            UTF8 *ReleaseDate              = calloc(ChunkSize, sizeof(UTF8));
            ReleaseDate                    = BitBuffer_ReadUTF8(BitB, ChunkSize);
            OVIA_SetNumTags(Ovia, OVIA_GetNumTags(Ovia) + 1);
            OVIA_SetTag(Ovia, ReleaseTag, ReleaseDate);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void ReadINFO_IGNR(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) {
        if (Ovia != NULL && BitB != NULL) {
            UTF8 *Genre              = calloc(ChunkSize, sizeof(UTF8));
            Genre                    = BitBuffer_ReadUTF8(BitB, ChunkSize);
            OVIA_SetNumTags(Ovia, OVIA_GetNumTags(Ovia) + 1);
            OVIA_SetTag(Ovia, GenreTag, Genre);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void ReadINFO_INAM(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) {
        if (Ovia != NULL && BitB != NULL) {
            UTF8 *Title                  = calloc(ChunkSize, sizeof(UTF8));
            Title = BitBuffer_ReadUTF8(BitB, ChunkSize);
            OVIA_SetNumTags(Ovia, OVIA_GetNumTags(Ovia) + 1);
            OVIA_SetTag(Ovia, TitleTag, Title);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void ReadINFO_IPRD(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) {
        if (Ovia != NULL && BitB != NULL) {
            char *Album              = calloc(1, ChunkSize * sizeof(char));
            Album                    = BitBuffer_ReadUTF8(BitB, ChunkSize);
            OVIA_SetNumTags(Ovia, OVIA_GetNumTags(Ovia) + 1);
            OVIA_SetTag(Ovia, AlbumTag, Album);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void ReadINFO_ISFT(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) { // Encoder
        if (Ovia != NULL && BitB != NULL) {
            char *CreationSoftware   = calloc(1, ChunkSize * sizeof(char));
            CreationSoftware         = BitBuffer_ReadUTF8(BitB, ChunkSize);
            OVIA_SetNumTags(Ovia, OVIA_GetNumTags(Ovia) + 1);
            OVIA_SetTag(Ovia, CreatingSoftware, CreationSoftware);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void WAVParseLISTChunk(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) {
        if (Ovia != NULL && BitB != NULL) {
            uint32_t SubChunkID   = BitBuffer_ReadBits(LSByteFirst, LSBitFirst, BitB, 32);
            uint32_t SubChunkSize = BitBuffer_ReadBits(LSByteFirst, LSBitFirst, BitB, 32);
            
            switch (SubChunkID) {
                case WAV_IART: // Artist
                    ReadINFO_IART(Ovia, BitB, SubChunkSize);
                    break;
                case WAV_ICRD: // Release date
                    ReadINFO_ICRD(Ovia, BitB, SubChunkSize);
                    break;
                case WAV_IGNR: // Genre
                    ReadINFO_IGNR(Ovia, BitB, SubChunkSize);
                    break;
                case WAV_INAM: // Title
                    ReadINFO_INAM(Ovia, BitB, SubChunkSize);
                    break;
                case WAV_IPRD: // Album
                    ReadINFO_IPRD(Ovia, BitB, SubChunkSize);
                    break;
                case WAV_ISFT: // Encoder
                    ReadINFO_ISFT(Ovia, BitB, SubChunkSize);
                    break;
                default:
                    Log(Log_ERROR, __func__, U8("Unknown LIST Chunk: 0x%X"), SubChunkID);
                    break;
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void WAVParseDATAChunk(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            uint32_t ChunkSize = BitBuffer_ReadBits(LSByteFirst, LSBitFirst, BitB, 32);
            OVIA_SetNumSamples(Ovia, ChunkSize / OVIA_GetNumChannels(Ovia));
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void WAVParseFMTChunk(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            uint32_t ChunkSize = BitBuffer_ReadBits(LSByteFirst, LSBitFirst, BitB, 32);
            
            OVIA_WAV_SetCompressionType(Ovia, BitBuffer_ReadBits(LSByteFirst, LSBitFirst, BitB, 16)); // 1
            OVIA_SetNumChannels(Ovia, BitBuffer_ReadBits(LSByteFirst, LSBitFirst, BitB, 16)); // 2
            OVIA_SetSampleRate(Ovia, BitBuffer_ReadBits(LSByteFirst, LSBitFirst, BitB, 32)); // 44100
            BitBuffer_Seek(BitB, 32); // ByteRate
            BitBuffer_Seek(BitB, 16); // BlockAlign
            //OVIA_WAV_SetBlockAlignment(Ovia, BitBuffer_ReadBits(LSByteFirst, LSBitFirst, BitB, 32));
            OVIA_SetBitDepth(Ovia, BitBuffer_ReadBits(LSByteFirst, LSBitFirst, BitB, 16)); // 16
            if (ChunkSize == 18) {
                uint16_t CBSize             = BitBuffer_ReadBits(LSByteFirst, LSBitFirst, BitB, 16);
                BitBuffer_Skip(BitB, Bytes2Bits(CBSize - 16));
            } else if (ChunkSize == 40) {
                uint16_t CBSize             = BitBuffer_ReadBits(LSByteFirst, LSBitFirst, BitB, 16);
                uint16_t ValidBitsPerSample = BitBuffer_ReadBits(LSByteFirst, LSBitFirst, BitB, 16);
                if (ValidBitsPerSample != 0) {
                    OVIA_SetBitDepth(Ovia, ValidBitsPerSample);
                }
                uint32_t  SpeakerMask       = BitBuffer_ReadBits(LSByteFirst, LSBitFirst, BitB, 32);
                uint8_t  *BinaryGUIDFormat  = BitBuffer_ReadGUUID(BinaryGUID, BitB);
                BitBuffer_Skip(BitB, Bytes2Bits(CBSize - 22));
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WAVParseMetadata(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            uint32_t ChunkID   = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            uint32_t ChunkSize = BitBuffer_ReadBits(LSByteFirst, LSBitFirst, BitB, 32);
            
            switch (ChunkID) {
                case WAV_LIST:
                    break;
                case WAV_FMT:
                    WAVParseFMTChunk(Ovia, BitB);
                    break;
                case WAV_WAVE:
                    BitBuffer_Skip(BitB, 32);
                    break;
                case WAV_DATA:
                    WAVParseDATAChunk(Ovia, BitB, ChunkSize);
                    break;
                default:
                    Log(Log_ERROR, __func__, U8("Invalid ChunkID: 0x%X"), ChunkID);
                    break;
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    AudioContainer *WAVExtractSamples(OVIA *Ovia, BitBuffer *BitB) {
        AudioContainer *Audio = NULL;
        if (Ovia != NULL && BitB != NULL) {
            uint64_t BitDepth     = OVIA_GetBitDepth(Ovia);
            uint64_t NumChannels  = OVIA_GetNumChannels(Ovia);
            uint64_t SampleRate   = OVIA_GetSampleRate(Ovia);
            uint64_t NumSamples   = OVIA_GetNumSamples(Ovia);
            if (BitDepth <= 8) {
                Audio = AudioContainer_Init(AudioContainer_UInteger8, BitDepth, NumChannels, SampleRate, NumSamples);
                uint8_t **Samples = (uint8_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        Samples[Channel][Sample] = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, Bits2Bytes(BitDepth, Yes));
                    }
                }
            } else if (BitDepth > 8 && BitDepth <= 16) {
                Audio = AudioContainer_Init(AudioContainer_UInteger16, BitDepth, NumChannels, SampleRate, NumSamples);
                uint16_t **Samples = (uint16_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        Samples[Channel][Sample] = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, Bits2Bytes(BitDepth, Yes));
                    }
                }
            } else if (BitDepth > 16 && BitDepth <= 32) {
                Audio = AudioContainer_Init(AudioContainer_UInteger32, BitDepth, NumChannels, SampleRate, NumSamples);
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
