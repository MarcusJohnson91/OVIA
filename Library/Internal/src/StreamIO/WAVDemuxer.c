#include "../../include/libPCM.h"
#include "../../include/Private/libPCMTypes.h"
#include "../../include/Private/Audio/WAVCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    /*
     Top level ChunkIDs that DO NOT have a size field
     
     WAVE
     INFO
     */
    
    void ParseWAVINFO(PCMFile *PCM, BitBuffer *BitB) {
        if (PCM == NULL) {
            Log(LOG_ERR, "libPCM", "ParseWAV", "Pointer to PCMFile is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libPCM", "ParseWAV", "Pointer to BitBuffer is NULL");
        } else {
        }
    }
    
    void ParseWAV(PCMFile *PCM, BitBuffer *BitB) {
        if (PCM == NULL) {
            Log(LOG_ERR, "libPCM", "ParseWAV", "Pointer to PCMFile is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libPCM", "ParseWAV", "Pointer to BitBuffer is NULL");
        } else {
            /*
             So, this is where we've already read the RIFF and RIFFSize fields, so now we need to check if it's WAVE of INFO
             */
            uint8_t ChunkID[4];
            for (uint8_t ChunkIDByte = 0; ChunkIDByte < 4; ChunkIDByte++) {
                ChunkID[ChunkIDByte] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
            }
            uint32_t ChunkSize = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
            if (strcasecmp(ChunkID, "WAVE") == 0) {
                
            } else if (strcasecmp(ChunkID, "INFO") == 0) {
                
            } else {
                Log(LOG_ERR, "libPCM", "ParseWAV", "Unrecognized Chunk ID: %s", ChunkID);
            }
        }
    }
    
    const static uint8_t BitDepth2SampleSizeInBytes[32] = {
         0,  8,  8,  8,  8,  8,  8,  8,
         8, 16, 16, 16, 16, 16, 16, 16,
        16, 24, 24, 24, 24, 24, 24, 24,
        24, 32, 32, 32, 32, 32, 32, 32
    };
    
    void ReadINFO_IART(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) {
        char *Artist = calloc(1, ChunkSize);
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Artist[Byte] = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 8);
        }
        PCM->AUD->Meta->ArtistTag = Artist;
        if (IsOdd(ChunkSize) == true) {
            BitBufferSkip(BitB, 8);
        }
        PCM->AUD->Meta->NumTags += 1;
    }
    
    void ReadINFO_ICRD(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) {
        char *ReleaseDate = calloc(1, ChunkSize);
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            ReleaseDate[Byte] = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 8);
        }
        PCM->AUD->Meta->ReleaseDateTag = ReleaseDate;
        if (IsOdd(ChunkSize) == true) {
            BitBufferSkip(BitB, 8);
        }
        PCM->AUD->Meta->NumTags += 1;
    }
    
    void ReadINFO_IGNR(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) {
        char *Genre = calloc(1, ChunkSize);
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Genre[Byte] = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 8);
        }
        PCM->AUD->Meta->GenreTag = Genre;
        if (IsOdd(ChunkSize) == true) {
            BitBufferSkip(BitB, 8);
        }
        PCM->AUD->Meta->NumTags += 1;
    }
    
    void ReadINFO_INAM(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) {
        char *Title = calloc(1, ChunkSize);
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Title[Byte] = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 8);
        }
        PCM->AUD->Meta->SongTitleTag = Title;
        if (IsOdd(ChunkSize) == true) {
            BitBufferSkip(BitB, 8);
        }
        PCM->AUD->Meta->NumTags += 1;
    }
    
    void ReadINFO_IPRD(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) {
        char *Album              = calloc(1, ChunkSize);
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Album[Byte]          = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 8);
        }
        PCM->AUD->Meta->AlbumTag = Album;
        if (IsOdd(ChunkSize) == true) {
            BitBufferSkip(BitB, 8);
        }
        PCM->AUD->Meta->NumTags += 1;
    }
    
    void ReadINFO_ISFT(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) { // Encoder
        char *Encoder     = calloc(1, ChunkSize);
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Encoder[Byte] = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 8);
        }
        PCM->AUD->Meta->EncoderTag = Encoder;
        if (IsOdd(ChunkSize) == true) {
            BitBufferSkip(BitB, 8);
        }
        PCM->AUD->Meta->NumTags += 1;
    }
    
    void ParseWAVLISTChunk(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) {
        uint32_t SubChunkID   = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
        uint32_t SubChunkSize = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
        
        switch (SubChunkID) {
            case WAV_IART: // Artist
                ReadINFO_IART(PCM, BitB, SubChunkSize);
                break;
            case WAV_ICRD: // Release date
                ReadINFO_ICRD(PCM, BitB, SubChunkSize);
                break;
            case WAV_IGNR: // Genre
                ReadINFO_IGNR(PCM, BitB, SubChunkSize);
                break;
            case WAV_INAM: // Title
                ReadINFO_INAM(PCM, BitB, SubChunkSize);
                break;
            case WAV_IPRD: // Album
                ReadINFO_IPRD(PCM, BitB, SubChunkSize);
                break;
            case WAV_ISFT: // Encoder
                ReadINFO_ISFT(PCM, BitB, SubChunkSize);
                break;
            default:
                Log(LOG_ERR, "libPCM", "ParseWAVLISTChunk", "Unknown LIST Chunk: 0x%X", SubChunkID);
                break;
        }
    }
    
    void ParseWavDATAChunk(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) {
        PCM->AUD->NumSamples            = ((ChunkSize / PCM->AUD->NumChannels) / Bits2Bytes(PCM->AUD->BitDepth, true));
    }
    
    void ParseWavFMTChunk(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) {
        if (PCM == NULL) {
            Log(LOG_ERR, "libPCM", "ParseWAV", "Pointer to PCMFile is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libPCM", "ParseWAV", "Pointer to BitBuffer is NULL");
        } else {
            uint16_t wFormatTag                  = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
            PCM->AUD->NumChannels                = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
            PCM->AUD->SampleRate                 = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
            uint32_t AvgBytesPerSec              = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
            PCM->AUD->BlockAlignment             = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
            PCM->AUD->BitDepth                   = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
            
            if (ChunkSize == 18) {
                uint16_t CBSize             = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
                BitBufferSkip(BitB, Bytes2Bits(CBSize - 16));
            } else if (ChunkSize == 40) {
                uint16_t CBSize             = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
                uint16_t ValidBitsPerSample = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
                if (ValidBitsPerSample != 0) {
                    PCM->AUD->BitDepth = ValidBitsPerSample;
                }
                uint32_t  SpeakerMask       = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                uint8_t  *BinaryGUIDFormat  = ReadGUUID(BitIOBinaryGUID, BitB);
                BitBufferSkip(BitB, Bytes2Bits(CBSize - 22));
            }
        }
    }
    
    uint32_t **WAVExtractSamples(PCMFile *PCM, BitBuffer *BitB, uint64_t NumSamples2Extract) {
        uint64_t ExtractedSamplesSize = NumSamples2Extract * BitDepth2SampleSizeInBytes[PCM->AUD->BitDepth] * PCM->AUD->NumChannels;
        uint32_t **ExtractedSamples   = calloc(1, ExtractedSamplesSize);
        if (ExtractedSamples == NULL) {
            Log(LOG_ERR, "libPCM", "WAVExtractSamples", "Couldn't allocate enough memory for the Extracted samples, %d", ExtractedSamplesSize);
        } else {
            for (uint64_t Channel = 0; Channel < PCM->AUD->NumChannels; Channel++) {
                for (uint64_t Sample = 0; Sample < NumSamples2Extract; Sample++) {
                    ExtractedSamples[Channel][Sample] = ReadBits(BitIOLSByte, BitIOLSBit, BitB, BitDepth2SampleSizeInBytes[PCM->AUD->BitDepth]);
                }
            }
        }
        return ExtractedSamples;
    }
    
    void ParseWAVFile(PCMFile *PCM, BitBuffer *BitB) {
        uint32_t ChunkID   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32);
        uint32_t ChunkSize = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
        
        switch (ChunkID) {
            case WAV_LIST:
                ParseWavLISTChunk(PCM, BitB, ChunkSize);
                break;
            case WAV_FMT:
                ParseWavFMTChunk(PCM, BitB, ChunkSize);
                break;
            case WAV_WAVE:
                BitBufferSkip(BitB, 32);
                break;
            case WAV_DATA:
                ParseWavDATAChunk(PCM, BitB, ChunkSize);
                break;
            default:
                Log(LOG_ERR, "libPCM", "ParseWAVFile", "Invalid ChunkID: 0x%X", ChunkID);
                break;
        }
    }
    
#ifdef __cplusplus
}
#endif
