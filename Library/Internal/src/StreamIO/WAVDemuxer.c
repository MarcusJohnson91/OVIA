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
                ChunkID[ChunkIDByte] = ReadBits(BitB, 8, true);
            }
            uint32_t ChunkSize = ReadBits(BitB, 32, false);
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
            Artist[Byte] = ReadBits(BitB, 8, true);
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
            ReleaseDate[Byte] = ReadBits(BitB, 8, true);
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
            Genre[Byte] = ReadBits(BitB, 8, true);
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
            Title[Byte] = ReadBits(BitB, 8, true);
        }
        PCM->Meta->SongTitleTag = Title;
        if (IsOdd(ChunkSize) == true) {
            SkipBits(BitB, 8);
        }
        PCM->Meta->NumTags += 1;
    }
    
    void ReadINFO_IPRD(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) {
        char *Album              = calloc(1, ChunkSize);
        for (uint8_t Byte        = 0; Byte < ChunkSize; Byte++) {
            Album[Byte]          = ReadBits(BitB, 8, true);
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
            Encoder[Byte] = ReadBits(BitB, 8, true);
        }
        PCM->Meta->EncoderTag = Encoder;
        if (IsOdd(ChunkSize) == true) {
            SkipBits(BitB, 8);
        }
        PCM->Meta->NumTags += 1;
    }
    
    void ParseWavLISTChunk(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) {
        uint32_t ShouldBeINFO  = ReadBits(BitB, 32, true);
        if (ShouldBeINFO == WAV_INFO) {
            uint32_t ChunkID   = ReadBits(BitB, 32, true);
            uint32_t ChunkSize = ReadBits(BitB, 32, true);
            
            switch (ChunkID) {
                case WAV_IART: // Artist
                    ReadINFO_IART(Wav, BitB, ChunkSize);
                    break;
                case WAV_ICRD: // Release date
                    ReadINFO_ICRD(Wav, BitB, ChunkSize);
                    break;
                case WAV_IGNR: // Genre
                    ReadINFO_IGNR(Wav, BitB, ChunkSize);
                    break;
                case WAV_INAM: // Title
                    ReadINFO_INAM(Wav, BitB, ChunkSize);
                    break;
                case WAV_IPRD: // Album
                    ReadINFO_IPRD(Wav, BitB, ChunkSize);
                    break;
                case WAV_ISFT: // Encoder
                    ReadINFO_ISFT(Wav, BitB, ChunkSize);
                    break;
                default:
                    break;
            }
        } else {
            printf("Unknown Wav LIST Chunk ID: %d\n", ShouldBeINFO);
        }
    }
    
    void ParseWavDATAChunk(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) { //
        PCM->SampleCount            = ((ChunkSize / PCM->Channels) / Bits2Bytes(PCM->BitDepth, true));
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
                uint16_t wFormatTag      = ReadBits(BitB, 16, false);
                uint16_t nChannels       = ReadBits(BitB, 16, false);
                uint32_t nSamplesPerSec  = ReadBits(BitB, 32, false);
                uint32_t nAvgBytesPerSec = ReadBits(BitB, 32, false);
                uint16_t nBlockAlign     = ReadBits(BitB, 16, false);
                uint16_t wBitsPerSample  = ReadBits(BitB, 16, false);
                uint16_t cbSize          = ReadBits(BitB, 16, false);
            }
            
            
            
            
            
            if (ChunkSize > 16) {
                PCM->FMTType            = SwapEndian16(ReadBits(BitB, 16, true));
            }
            PCM->WAVW64FormatType       = ReadBits(BitB, 16, false);
            PCM->NumChannels            = SwapEndian16(ReadBits(BitB, 16, true));
            PCM->SampleRate             = SwapEndian32(ReadBits(BitB, 32, true));
            PCM->ByteRate               = SwapEndian32(ReadBits(BitB, 32, true));
            PCM->BlockSize              = SwapEndian16(ReadBits(BitB, 16, true));
            PCM->BitDepth               = SwapEndian16(ReadBits(BitB, 16, true));
            if (PCM->FMTType == WAVFormatExtensible) {
                PCM->ExtensionSize      = SwapEndian16(ReadBits(BitB, 16, true));
                PCM->ValidBitsPerSample = SwapEndian16(ReadBits(BitB, 16, true));
                PCM->SpeakerMask        = SwapEndian32(ReadBits(BitB, 32, true));
                SkipBits(BitB, PCM->ExtensionSize - 8); // 8 bytes from ^
            }
        }
    }
    
    void WAVExtractSamples(PCMFile *PCM, BitBuffer *BitB, uint64_t NumSamples2Extract) {
        for (uint8_t Channel = 0; Channel < PCM->Channels; Channel++) {
            for (uint64_t Sample = 0; Sample < NumSamples2Extract; Sample++) {
                Samples[Channel][Sample] = ReadBits(BitB, BitDepth2SampleSizeInBytes[BitDepth], false);
            }
        }
        NumSamples = NumSamples2Extract;
    }
    
#ifdef __cplusplus
}
#endif
