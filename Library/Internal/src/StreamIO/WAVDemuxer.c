#include "../../../Dependencies/BitIO/libBitIO/include/BitIO.h"

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
        char *Artist              = calloc(ChunkSize, sizeof(char));
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Artist[Byte]          = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 8);
        }
        PCM->AUD->Meta->NumTags  += 1;
        PCM->AUD->Meta->ArtistTag = Artist;
        IFFSkipPadding(BitB, ChunkSize);
    }
    
    static void ReadINFO_ICRD(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) {
        char *ReleaseDate              = calloc(ChunkSize, sizeof(char));
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            ReleaseDate[Byte]          = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 8);
        }
        PCM->AUD->Meta->NumTags       += 1;
        PCM->AUD->Meta->ReleaseDateTag = ReleaseDate;
        IFFSkipPadding(BitB, ChunkSize);
    }
    
    static void ReadINFO_IGNR(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) {
        char *Genre              = calloc(ChunkSize, sizeof(char));
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Genre[Byte]          = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 8);
        }
        PCM->AUD->Meta->NumTags += 1;
        PCM->AUD->Meta->GenreTag = Genre;
        IFFSkipPadding(BitB, ChunkSize);
    }
    
    static void ReadINFO_INAM(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) {
        char *Title                  = calloc(ChunkSize, sizeof(char));
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Title[Byte]              = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 8);
        }
        PCM->AUD->Meta->NumTags     += 1;
        PCM->AUD->Meta->SongTitleTag = Title;
        IFFSkipPadding(BitB, ChunkSize);
    }
    
    static void ReadINFO_IPRD(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) {
        char *Album              = calloc(1, ChunkSize * sizeof(char));
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Album[Byte]          = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 8);
        }
        PCM->AUD->Meta->NumTags += 1;
        PCM->AUD->Meta->AlbumTag = Album;
        IFFSkipPadding(BitB, ChunkSize);
    }
    
    static void ReadINFO_ISFT(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) { // Encoder
        char *Encoder              = calloc(1, ChunkSize * sizeof(char));
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Encoder[Byte]          = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 8);
        }
        PCM->AUD->Meta->NumTags   += 1;
        PCM->AUD->Meta->EncoderTag = Encoder;
        IFFSkipPadding(BitB, ChunkSize);
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
        PCM->NumChannelAgnosticSamples            = ((ChunkSize / PCM->NumChannels) / Bits2Bytes(PCM->BitDepth, true));
    }
    
    static void WAVParseFMTChunk(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) {
        if (PCM == NULL) {
            BitIOLog(LOG_ERROR, libPCMLibraryName, __func__, "Pointer to PCMFile is NULL");
        } else if (BitB == NULL) {
            BitIOLog(LOG_ERROR, libPCMLibraryName, __func__, "Pointer to BitBuffer is NULL");
        } else {
            PCM->AUD->WAVCompressionFormat  = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
            PCM->NumChannels                = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
            PCM->AUD->SampleRate            = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
            PCM->AUD->WAVAvgBytesPerSecond  = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
            PCM->AUD->BlockAlignment        = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
            PCM->BitDepth                   = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
            if (ChunkSize == 18) {
                uint16_t CBSize             = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
                BitBuffer_Skip(BitB, Bytes2Bits(CBSize - 16));
            } else if (ChunkSize == 40) {
                uint16_t CBSize             = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
                uint16_t ValidBitsPerSample = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
                if (ValidBitsPerSample != 0) {
                    PCM->BitDepth = ValidBitsPerSample;
                }
                uint32_t  SpeakerMask       = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                uint8_t  *BinaryGUIDFormat  = ReadGUUID(BitIOBinaryGUUID, BitIOLSByte, BitB);
                BitBuffer_Skip(BitB, Bytes2Bits(CBSize - 22));
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
                break;
            case WAV_WAVE:
                BitBuffer_Skip(BitB, 32);
                break;
            case WAV_DATA:
                WAVParseDATAChunk(PCM, BitB, ChunkSize);
                break;
            default:
                BitIOLog(LOG_ERROR, libPCMLibraryName, __func__, "Invalid ChunkID: 0x%X", ChunkID);
                break;
        }
    }
    
    void WAVExtractSamples(PCMFile *PCM, BitBuffer *BitB, uint64_t NumSamples2Extract, uint32_t **ExtractedSamples) {
        for (uint64_t Sample = 0; Sample < NumSamples2Extract; Sample++) {
            for (uint64_t Channel = 0; Channel < PCM->NumChannels; Channel++) {
                ExtractedSamples[Channel][Sample] = ReadBits(BitIOLSByte, BitIOLSBit, BitB, (uint64_t)Bits2Bytes(PCM->BitDepth, Yes));
            }
        }
    }
    
#ifdef __cplusplus
}
#endif
