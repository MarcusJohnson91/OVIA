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
    
    static const uint8_t BitDepth2SampleSizeInBytes[32] = {
         0,  8,  8,  8,  8,  8,  8,  8,
         8, 16, 16, 16, 16, 16, 16, 16,
        16, 24, 24, 24, 24, 24, 24, 24,
        24, 32, 32, 32, 32, 32, 32, 32
    };
    
    static void ReadINFO_IART(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) {
        if (Ovia != NULL && BitB != NULL) {
            
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
        
        char *Artist              = calloc(ChunkSize, sizeof(char));
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Artist[Byte]          = BitBuffer_ReadBits(LSByteFirst, LSBitFirst, BitB, 8);
        }
        Ovia->Aud->Meta->NumTags  += 1;
        Ovia->Aud->Meta->ArtistTag = Artist;
        IFFSkipPadding(BitB, ChunkSize);
    }
    
    static void ReadINFO_ICRD(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) {
        if (Ovia != NULL && BitB != NULL) {
            
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
        
        char *ReleaseDate              = calloc(ChunkSize, sizeof(char));
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            ReleaseDate[Byte]          = BitBuffer_ReadBits(LSByteFirst, LSBitFirst, BitB, 8);
        }
        Ovia->Aud->Meta->NumTags       += 1;
        Ovia->Aud->Meta->ReleaseDateTag = ReleaseDate;
        IFFSkipPadding(BitB, ChunkSize);
    }
    
    static void ReadINFO_IGNR(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) {
        if (Ovia != NULL && BitB != NULL) {
            
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
        
        char *Genre              = calloc(ChunkSize, sizeof(char));
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Genre[Byte]          = BitBuffer_ReadBits(LSByteFirst, LSBitFirst, BitB, 8);
        }
        Ovia->Aud->Meta->NumTags += 1;
        Ovia->Aud->Meta->GenreTag = Genre;
        IFFSkipPadding(BitB, ChunkSize);
    }
    
    static void ReadINFO_INAM(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) {
        if (Ovia != NULL && BitB != NULL) {
            
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
        
        char *Title                  = calloc(ChunkSize, sizeof(char));
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Title[Byte]              = BitBuffer_ReadBits(LSByteFirst, LSBitFirst, BitB, 8);
        }
        Ovia->Aud->Meta->NumTags     += 1;
        Ovia->Aud->Meta->SongTitleTag = Title;
        IFFSkipPadding(BitB, ChunkSize);
    }
    
    static void ReadINFO_IPRD(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) {
        if (Ovia != NULL && BitB != NULL) {
            
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
        
        char *Album              = calloc(1, ChunkSize * sizeof(char));
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Album[Byte]          = BitBuffer_ReadBits(LSByteFirst, LSBitFirst, BitB, 8);
        }
        Ovia->Aud->Meta->NumTags += 1;
        Ovia->Aud->Meta->AlbumTag = Album;
        IFFSkipPadding(BitB, ChunkSize);
    }
    
    static void ReadINFO_ISFT(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) { // Encoder
        if (Ovia != NULL && BitB != NULL) {
            
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
        
        char *Encoder              = calloc(1, ChunkSize * sizeof(char));
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Encoder[Byte]          = BitBuffer_ReadBits(LSByteFirst, LSBitFirst, BitB, 8);
        }
        Ovia->Aud->Meta->NumTags   += 1;
        Ovia->Aud->Meta->EncoderTag = Encoder;
        IFFSkipPadding(BitB, ChunkSize);
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
    
    static void WAVParseDATAChunk(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) {
        if (Ovia != NULL && BitB != NULL) {
            Ovia->NumChannelAgnosticSamples            = ((ChunkSize / OVIA_GetNumChannels(Ovia)) / Bits2Bytes(Ovia->BitDepth, true));
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void WAVParseFMTChunk(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) {
        if (Ovia != NULL && BitB != NULL) {
            Ovia->Aud->WAVCompressionFormat  = BitBuffer_ReadBits(LSByteFirst, LSBitFirst, BitB, 16);
            OVIA_GetNumChannels(Ovia)                = BitBuffer_ReadBits(LSByteFirst, LSBitFirst, BitB, 16);
            Ovia->Aud->SampleRate            = BitBuffer_ReadBits(LSByteFirst, LSBitFirst, BitB, 32);
            Ovia->Aud->WAVAvgBytesPerSecond  = BitBuffer_ReadBits(LSByteFirst, LSBitFirst, BitB, 32);
            Ovia->Aud->BlockAlignment        = BitBuffer_ReadBits(LSByteFirst, LSBitFirst, BitB, 32);
            Ovia->BitDepth                   = BitBuffer_ReadBits(LSByteFirst, LSBitFirst, BitB, 16);
            if (ChunkSize == 18) {
                uint16_t CBSize             = BitBuffer_ReadBits(LSByteFirst, LSBitFirst, BitB, 16);
                BitBuffer_Skip(BitB, Bytes2Bits(CBSize - 16));
            } else if (ChunkSize == 40) {
                uint16_t CBSize             = BitBuffer_ReadBits(LSByteFirst, LSBitFirst, BitB, 16);
                uint16_t ValidBitsPerSample = BitBuffer_ReadBits(LSByteFirst, LSBitFirst, BitB, 16);
                if (ValidBitsPerSample != 0) {
                    Ovia->BitDepth = ValidBitsPerSample;
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
                    WAVParseFMTChunk(Ovia, BitB, ChunkSize);
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
