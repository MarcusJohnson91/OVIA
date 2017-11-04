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
    
    static const uint8_t BitDepth2SampleSizeInBytes[32] = {
         0,  8,  8,  8,  8,  8,  8,  8,
         8, 16, 16, 16, 16, 16, 16, 16,
        16, 24, 24, 24, 24, 24, 24, 24,
        24, 32, 32, 32, 32, 32, 32, 32
    };
    
    static void ReadINFO_IART(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) {
        char *Artist = calloc(1, ChunkSize * sizeof(char));
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Artist[Byte] = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 8);
        }
        PCM->AUD->Meta->ArtistTag = Artist;
        if (IsOdd(ChunkSize) == Yes) {
            BitBufferSkip(BitB, 8);
        }
        PCM->AUD->Meta->NumTags += 1;
    }
    
    static void ReadINFO_ICRD(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) {
        char *ReleaseDate = calloc(1, ChunkSize * sizeof(char));
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            ReleaseDate[Byte] = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 8);
        }
        PCM->AUD->Meta->ReleaseDateTag = ReleaseDate;
        if (IsOdd(ChunkSize) == Yes) {
            BitBufferSkip(BitB, 8);
        }
        PCM->AUD->Meta->NumTags += 1;
    }
    
    static void ReadINFO_IGNR(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) {
        char *Genre = calloc(1, ChunkSize * sizeof(char));
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Genre[Byte] = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 8);
        }
        PCM->AUD->Meta->GenreTag = Genre;
        if (IsOdd(ChunkSize) == Yes) {
            BitBufferSkip(BitB, 8);
        }
        PCM->AUD->Meta->NumTags += 1;
    }
    
    static void ReadINFO_INAM(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) {
        char *Title = calloc(1, ChunkSize * sizeof(char));
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Title[Byte] = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 8);
        }
        PCM->AUD->Meta->SongTitleTag = Title;
        if (IsOdd(ChunkSize) == Yes) {
            BitBufferSkip(BitB, 8);
        }
        PCM->AUD->Meta->NumTags += 1;
    }
    
    static void ReadINFO_IPRD(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) {
        char *Album              = calloc(1, ChunkSize * sizeof(char));
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Album[Byte]          = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 8);
        }
        PCM->AUD->Meta->AlbumTag = Album;
        if (IsOdd(ChunkSize) == Yes) {
            BitBufferSkip(BitB, 8);
        }
        PCM->AUD->Meta->NumTags += 1;
    }
    
    static void ReadINFO_ISFT(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) { // Encoder
        char *Encoder     = calloc(1, ChunkSize * sizeof(char));
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Encoder[Byte] = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 8);
        }
        PCM->AUD->Meta->EncoderTag = Encoder;
        if (IsOdd(ChunkSize) == Yes) {
            BitBufferSkip(BitB, 8);
        }
        PCM->AUD->Meta->NumTags += 1;
    }
    
    static void WAVParseLISTChunk(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) {
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
                BitIOLog(LOG_ERROR, libPCMLibraryName, __func__, "Unknown LIST Chunk: 0x%X", SubChunkID);
                break;
        }
    }
    
    static void WAVParseDATAChunk(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) {
        PCM->AUD->NumSamples            = ((ChunkSize / PCM->AUD->NumChannels) / Bits2Bytes(PCM->AUD->BitDepth, true));
    }
    
    static void WAVParseFMTChunk(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) {
        if (PCM == NULL) {
            BitIOLog(LOG_ERROR, libPCMLibraryName, __func__, "Pointer to PCMFile is NULL");
        } else if (BitB == NULL) {
            BitIOLog(LOG_ERROR, libPCMLibraryName, __func__, "Pointer to BitBuffer is NULL");
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
    
    void WAVParseMetadata(PCMFile *PCM, BitBuffer *BitB) {
        uint32_t ChunkID   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32);
        uint32_t ChunkSize = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
        
        switch (ChunkID) {
            case WAV_LIST:
                break;
            case WAV_FMT:
                WAVParseFMTChunk(PCM, BitB, ChunkSize);
                if (IsOdd(ChunkSize) == Yes) { // Skip the IFF container padding byte
                    BitBufferSkip(BitB, 8);
                }
                break;
            case WAV_WAVE:
                BitBufferSkip(BitB, 32);
                if (IsOdd(ChunkSize) == Yes) { // Skip the IFF container padding byte
                    BitBufferSkip(BitB, 8);
                }
                break;
            case WAV_DATA:
                WAVParseDATAChunk(PCM, BitB, ChunkSize);
                if (IsOdd(ChunkSize) == Yes) { // Skip the IFF container padding byte
                    BitBufferSkip(BitB, 8);
                }
                break;
            default:
                BitIOLog(LOG_ERROR, libPCMLibraryName, __func__, "Invalid ChunkID: 0x%X", ChunkID);
                break;
        }
    }
    
    uint32_t **WAVExtractSamples(PCMFile *PCM, BitBuffer *BitB, uint64_t NumSamples2Extract) {
        uint64_t ExtractedSamplesSize = NumSamples2Extract * BitDepth2SampleSizeInBytes[PCM->AUD->BitDepth] * PCM->AUD->NumChannels;
        uint32_t **ExtractedSamples   = calloc(1, ExtractedSamplesSize * sizeof(uint32_t));
        if (ExtractedSamples == NULL) {
            BitIOLog(LOG_ERROR, libPCMLibraryName, __func__, "Couldn't allocate enough memory for the Extracted samples, %d", ExtractedSamplesSize);
        } else {
            for (uint64_t Channel = 0; Channel < PCM->AUD->NumChannels; Channel++) {
                for (uint64_t Sample = 0; Sample < NumSamples2Extract; Sample++) {
                    ExtractedSamples[Channel][Sample] = ReadBits(BitIOLSByte, BitIOLSBit, BitB, BitDepth2SampleSizeInBytes[PCM->AUD->BitDepth]);
                }
            }
        }
        return ExtractedSamples;
    }
    
#ifdef __cplusplus
}
#endif
