#include "../../include/Private/WAVCommon.h"

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
            BitBuffer_Seek(BitB, 8);
        }
    }
    
    static void ReadINFO_IART(BitBuffer *BitB, uint32_t ChunkSize) {
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
    
    static void ReadINFO_ICRD(BitBuffer *BitB, uint32_t ChunkSize) {
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
    
    static void ReadINFO_IGNR(BitBuffer *BitB, uint32_t ChunkSize) {
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
    
    static void ReadINFO_INAM(BitBuffer *BitB, uint32_t ChunkSize) {
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
    
    static void ReadINFO_IPRD(BitBuffer *BitB, uint32_t ChunkSize) {
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
    
    static void ReadINFO_ISFT(BitBuffer *BitB, uint32_t ChunkSize) { // Encoder
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
    
    static void WAVParseLISTChunk(BitBuffer *BitB, uint32_t ChunkSize) {
        if (Ovia != NULL && BitB != NULL) {
            uint32_t SubChunkID   = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
            uint32_t SubChunkSize = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
            
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
    
    static void WAVParseDATAChunk(BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            uint32_t ChunkSize = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
            OVIA_SetNumSamples(Ovia, ChunkSize / OVIA_GetNumChannels(Ovia));
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void WAVParseFMTChunk(BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            uint32_t ChunkSize = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
            
            OVIA_WAV_SetCompressionType(Ovia, BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16)); // 1
            OVIA_SetNumChannels(Ovia, BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16)); // 2
            OVIA_SetSampleRate(Ovia, BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32)); // 44100
            BitBuffer_Seek(BitB, 32); // ByteRate
            BitBuffer_Seek(BitB, 16); // BlockAlign
            //OVIA_WAV_SetBlockAlignment(Ovia, BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32));
            OVIA_SetBitDepth(Ovia, BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16)); // 16
            if (ChunkSize == 18) {
                uint16_t CBSize             = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16);
                BitBuffer_Seek(BitB, Bytes2Bits(CBSize - 16));
            } else if (ChunkSize == 40) {
                uint16_t CBSize             = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16);
                uint16_t ValidBitsPerSample = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16);
                if (ValidBitsPerSample != 0) {
                    OVIA_SetBitDepth(Ovia, ValidBitsPerSample);
                }
                
                uint32_t  SpeakerMask       = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
                OVIA_WAV_SetSpeakerMask(Ovia, SpeakerMask);
                uint8_t  *BinaryGUIDFormat  = BitBuffer_ReadGUUID(BitB, BinaryGUID);
                BitBuffer_Seek(BitB, Bytes2Bits(CBSize - 22));
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WAVParseMetadata(BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            uint32_t ChunkID   = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 32);
            uint32_t ChunkSize = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
            
            switch (ChunkID) {
                case WAV_LIST:
                    break;
                case WAV_FMT:
                    WAVParseFMTChunk(Ovia, BitB);
                    break;
                case WAV_WAVE:
                    BitBuffer_Seek(BitB, 32);
                    break;
                case WAV_DATA:
                    WAVParseDATAChunk(Ovia, BitB);
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
    
    Audio2DContainer *WAVExtractSamples(BitBuffer *BitB) {
        Audio2DContainer *Audio = NULL;
        if (Ovia != NULL && BitB != NULL) {
            uint64_t BitDepth     = OVIA_GetBitDepth(Ovia);
            uint64_t NumChannels  = OVIA_GetNumChannels(Ovia);
            uint64_t SampleRate   = OVIA_GetSampleRate(Ovia);
            uint64_t NumSamples   = OVIA_GetNumSamples(Ovia);
            if (BitDepth <= 8) {
                Audio             = AudioContainer_Init(AudioType_Integer8, AudioMask_FrontLeft | AudioMask_FrontRight, SampleRate, NumSamples);
                uint8_t **Samples = (uint8_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        Samples[Channel][Sample] = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, Bits2Bytes(BitDepth, RoundingType_Up));
                    }
                }
            } else if (BitDepth > 8 && BitDepth <= 16) {
                Audio             = AudioContainer_Init(AudioType_Integer16, AudioMask_FrontLeft | AudioMask_FrontRight, SampleRate, NumSamples);
                uint16_t **Samples = (uint16_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        Samples[Channel][Sample] = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, Bits2Bytes(BitDepth, RoundingType_Up));
                    }
                }
            } else if (BitDepth > 16 && BitDepth <= 32) {
                Audio             = AudioContainer_Init(AudioType_Integer32, AudioMask_FrontLeft | AudioMask_FrontRight, SampleRate, NumSamples);
                uint32_t **Samples = (uint32_t**) AudioContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < NumChannels; Channel++) {
                        Samples[Channel][Sample] = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, Bits2Bytes(BitDepth, RoundingType_Up));
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
    
    OVIADecoder W64Decoder = {
        .DecoderID             = CodecID_WAV,
        .MediaType             = MediaType_Audio2D,
        .MagicIDOffset         = 0,
        .MagicIDSize           = 4,
        .MagicID               = {0x52, 0x49, 0x46, 0x46},
        .Function_Initialize   = WAVOptions_Init,
        .Function_Parse        = WAVParseMetadata,
        .Function_Decode       = WAVExtractSamples,
        .Function_Deinitialize = WAVOptions_Deinit,
    };
    
#ifdef __cplusplus
}
#endif
