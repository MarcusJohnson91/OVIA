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
            PCM->MetaIsPresent = true;
            PCM->Meta          = calloc(1, sizeof(PCMMetadata));
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
        PCM->Meta->ArtistTag = Artist;
        if (IsOdd(ChunkSize) == true) {
            SkipBits(BitB, 8);
        }
        PCM->Meta->NumTags += 1;
    }
    
    void ReadINFO_ICRD(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) {
        char *ReleaseDate = calloc(1, ChunkSize);
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            ReleaseDate[Byte] = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 8);
        }
        PCM->Meta->ReleaseDateTag = ReleaseDate;
        if (IsOdd(ChunkSize) == true) {
            SkipBits(BitB, 8);
        }
        PCM->Meta->NumTags += 1;
    }
    
    void ReadINFO_IGNR(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) {
        char *Genre = calloc(1, ChunkSize);
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Genre[Byte] = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 8);
        }
        PCM->Meta->GenreTag = Genre;
        if (IsOdd(ChunkSize) == true) {
            SkipBits(BitB, 8);
        }
        PCM->Meta->NumTags += 1;
    }
    
    void ReadINFO_INAM(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) {
        char *Title = calloc(1, ChunkSize);
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Title[Byte] = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 8);
        }
        PCM->Meta->SongTitleTag = Title;
        if (IsOdd(ChunkSize) == true) {
            SkipBits(BitB, 8);
        }
        PCM->Meta->NumTags += 1;
    }
    
    void ReadINFO_IPRD(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) {
        char *Album              = calloc(1, ChunkSize);
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Album[Byte]          = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 8);
        }
        PCM->Meta->AlbumTag = Album;
        if (IsOdd(ChunkSize) == true) {
            SkipBits(BitB, 8);
        }
        PCM->Meta->NumTags += 1;
    }
    
    void ReadINFO_ISFT(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) { // Encoder
        char *Encoder     = calloc(1, ChunkSize);
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Encoder[Byte] = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 8);
        }
        PCM->Meta->EncoderTag = Encoder;
        if (IsOdd(ChunkSize) == true) {
            SkipBits(BitB, 8);
        }
        PCM->Meta->NumTags += 1;
    }
    
    void ParseWAVLISTChunk(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) {
        uint32_t ShouldBeINFO  = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
        if (ShouldBeINFO == WAV_INFO) {
            uint32_t ChunkID   = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
            uint32_t ChunkSize = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
            
            switch (ChunkID) {
                case WAV_IART: // Artist
                    ReadINFO_IART(PCM, BitB, ChunkSize);
                    break;
                case WAV_ICRD: // Release date
                    ReadINFO_ICRD(PCM, BitB, ChunkSize);
                    break;
                case WAV_IGNR: // Genre
                    ReadINFO_IGNR(PCM, BitB, ChunkSize);
                    break;
                case WAV_INAM: // Title
                    ReadINFO_INAM(PCM, BitB, ChunkSize);
                    break;
                case WAV_IPRD: // Album
                    ReadINFO_IPRD(PCM, BitB, ChunkSize);
                    break;
                case WAV_ISFT: // Encoder
                    ReadINFO_ISFT(PCM, BitB, ChunkSize);
                    break;
                default:
                    break;
            }
        } else {
            Log(LOG_ERR, "libPCM", "ParseWAVListChunk", "Unknown WAV List Chunk ID: %d", ShouldBeINFO);
        }
    }
    
    void ParseWavDATAChunk(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) {
        PCM->NumSamples            = ((ChunkSize / PCM->NumChannels) / Bits2Bytes(PCM->BitDepth, true));
    }
    
    void ParseWavFMTChunk(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) {
        if (PCM == NULL) {
            Log(LOG_ERR, "libPCM", "ParseWAV", "Pointer to PCMFile is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERR, "libPCM", "ParseWAV", "Pointer to BitBuffer is NULL");
        } else {
            if (ChunkSize == 16) {
                
            } else if (ChunkSize == 40) {
                // WORD = uint16_t
                // DWORD = uint32_t
                uint16_t wFormatTag      = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
                uint16_t nChannels       = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
                uint32_t nSamplesPerSec  = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                uint32_t nAvgBytesPerSec = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                uint16_t nBlockAlign     = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
                uint16_t wBitsPerSample  = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
                uint16_t cbSize          = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
            }
            
            if (ChunkSize > 16) {
                PCM->FMTType            = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
            }
            PCM->WAVW64FormatType       = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
            PCM->NumChannels            = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
            PCM->SampleRate             = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
            PCM->ByteRate               = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
            PCM->BlockSize              = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
            PCM->BitDepth               = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
            if (PCM->FMTType == WAVFormatExtensible) {
                PCM->ExtensionSize      = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
                PCM->ValidBitsPerSample = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
                PCM->SpeakerMask        = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                SkipBits(BitB, PCM->ExtensionSize - 8); // 8 bytes from ^
            }
        }
    }
    
    void WAVExtractSamples(PCMFile *PCM, BitBuffer *BitB, uint64_t NumSamples2Extract) {
        for (uint8_t Channel = 0; Channel < PCM->Channels; Channel++) {
            for (uint64_t Sample = 0; Sample < NumSamples2Extract; Sample++) {
                Samples[Channel][Sample] = ReadBits(BitIOLSByte, BitIOLSBit, BitB, BitDepth2SampleSizeInBytes[BitDepth]);
            }
        }
        NumSamples = NumSamples2Extract;
    }
    
#ifdef __cplusplus
}
#endif
