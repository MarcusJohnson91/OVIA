#include <stdlib.h>

#include "../../../Dependencies/FoundationIO/libFoundationIO/include/BitIO.h"
#include "../../../Dependencies/FoundationIO/libFoundationIO/include/Log.h"
#include "../../../Dependencies/FoundationIO/libFoundationIO/include/Math.h"
#include "../../../Dependencies/FoundationIO/libFoundationIO/include/GUUID.h"

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
            Artist[Byte]          = ReadBits(LSByteFirst, LSBitFirst, BitB, 8);
        }
        PCM->Aud->Meta->NumTags  += 1;
        PCM->Aud->Meta->ArtistTag = Artist;
        IFFSkipPadding(BitB, ChunkSize);
    }
    
    static void ReadINFO_ICRD(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) {
        char *ReleaseDate              = calloc(ChunkSize, sizeof(char));
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            ReleaseDate[Byte]          = ReadBits(LSByteFirst, LSBitFirst, BitB, 8);
        }
        PCM->Aud->Meta->NumTags       += 1;
        PCM->Aud->Meta->ReleaseDateTag = ReleaseDate;
        IFFSkipPadding(BitB, ChunkSize);
    }
    
    static void ReadINFO_IGNR(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) {
        char *Genre              = calloc(ChunkSize, sizeof(char));
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Genre[Byte]          = ReadBits(LSByteFirst, LSBitFirst, BitB, 8);
        }
        PCM->Aud->Meta->NumTags += 1;
        PCM->Aud->Meta->GenreTag = Genre;
        IFFSkipPadding(BitB, ChunkSize);
    }
    
    static void ReadINFO_INAM(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) {
        char *Title                  = calloc(ChunkSize, sizeof(char));
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Title[Byte]              = ReadBits(LSByteFirst, LSBitFirst, BitB, 8);
        }
        PCM->Aud->Meta->NumTags     += 1;
        PCM->Aud->Meta->SongTitleTag = Title;
        IFFSkipPadding(BitB, ChunkSize);
    }
    
    static void ReadINFO_IPRD(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) {
        char *Album              = calloc(1, ChunkSize * sizeof(char));
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Album[Byte]          = ReadBits(LSByteFirst, LSBitFirst, BitB, 8);
        }
        PCM->Aud->Meta->NumTags += 1;
        PCM->Aud->Meta->AlbumTag = Album;
        IFFSkipPadding(BitB, ChunkSize);
    }
    
    static void ReadINFO_ISFT(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) { // Encoder
        char *Encoder              = calloc(1, ChunkSize * sizeof(char));
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Encoder[Byte]          = ReadBits(LSByteFirst, LSBitFirst, BitB, 8);
        }
        PCM->Aud->Meta->NumTags   += 1;
        PCM->Aud->Meta->EncoderTag = Encoder;
        IFFSkipPadding(BitB, ChunkSize);
    }
    
    static void WAVParseLISTChunk(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) {
        uint32_t SubChunkID   = ReadBits(LSByteFirst, LSBitFirst, BitB, 32);
        uint32_t SubChunkSize = ReadBits(LSByteFirst, LSBitFirst, BitB, 32);
        
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
                Log(Log_ERROR, __func__, U8("Unknown LIST Chunk: 0x%X"), SubChunkID);
                break;
        }
    }
    
    static void WAVParseDATAChunk(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) {
        PCM->NumChannelAgnosticSamples            = ((ChunkSize / PCM->NumChannels) / Bits2Bytes(PCM->BitDepth, true));
    }
    
    static void WAVParseFMTChunk(PCMFile *PCM, BitBuffer *BitB, uint32_t ChunkSize) {
        if (PCM == NULL) {
            Log(Log_ERROR, __func__, U8("Pointer to PCMFile is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("Pointer to BitBuffer is NULL"));
        } else {
            PCM->Aud->WAVCompressionFormat  = ReadBits(LSByteFirst, LSBitFirst, BitB, 16);
            PCM->NumChannels                = ReadBits(LSByteFirst, LSBitFirst, BitB, 16);
            PCM->Aud->SampleRate            = ReadBits(LSByteFirst, LSBitFirst, BitB, 32);
            PCM->Aud->WAVAvgBytesPerSecond  = ReadBits(LSByteFirst, LSBitFirst, BitB, 32);
            PCM->Aud->BlockAlignment        = ReadBits(LSByteFirst, LSBitFirst, BitB, 32);
            PCM->BitDepth                   = ReadBits(LSByteFirst, LSBitFirst, BitB, 16);
            if (ChunkSize == 18) {
                uint16_t CBSize             = ReadBits(LSByteFirst, LSBitFirst, BitB, 16);
                BitBuffer_Skip(BitB, Bytes2Bits(CBSize - 16));
            } else if (ChunkSize == 40) {
                uint16_t CBSize             = ReadBits(LSByteFirst, LSBitFirst, BitB, 16);
                uint16_t ValidBitsPerSample = ReadBits(LSByteFirst, LSBitFirst, BitB, 16);
                if (ValidBitsPerSample != 0) {
                    PCM->BitDepth = ValidBitsPerSample;
                }
                uint32_t  SpeakerMask       = ReadBits(LSByteFirst, LSBitFirst, BitB, 32);
                uint8_t  *BinaryGUIDFormat  = ReadGUUID(BinaryGUID, BitB);
                BitBuffer_Skip(BitB, Bytes2Bits(CBSize - 22));
            }
        }
    }
    
    void WAVParseMetadata(PCMFile *PCM, BitBuffer *BitB) {
        uint32_t ChunkID   = ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
        uint32_t ChunkSize = ReadBits(LSByteFirst, LSBitFirst, BitB, 32);
        
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
                Log(Log_ERROR, __func__, U8("Invalid ChunkID: 0x%X"), ChunkID);
                break;
        }
    }
    
    void WAVExtractSamples(PCMFile *PCM, BitBuffer *BitB, uint64_t NumSamples2Extract, uint32_t **ExtractedSamples) {
        if (PCM != NULL && BitB != NULL && ExtractedSamples != NULL) {
            for (uint64_t Sample = 0; Sample < NumSamples2Extract; Sample++) {
                for (uint64_t Channel = 0; Channel < PCM->NumChannels; Channel++) {
                    ExtractedSamples[Channel][Sample] = ReadBits(LSByteFirst, LSBitFirst, BitB, (uint64_t)Bits2Bytes(PCM->BitDepth, Yes));
                }
            }
        } else if (PCM == NULL) {
            Log(Log_ERROR, __func__, U8("PCM Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (ExtractedSamples == NULL) {
            Log(Log_ERROR, __func__, U8("ExtractedSamples Pointer is NULL"));
        }
    }
    
#ifdef __cplusplus
}
#endif
