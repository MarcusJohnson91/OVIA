#include "../include/WAVCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void ReadINFO_IART(BitInput *BitI, PCMFile *PCM, uint32_t ChunkSize) {
        char *Artist = calloc(ChunkSize, 1);
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Artist[Byte] = ReadBits(BitI, 8);
        }
        PCM->WAV->Meta->ArtistTag = Artist;
        if (IsOdd(ChunkSize) == true) {
            SkipBits(BitI, 8);
        }
        PCM->WAV->Meta->NumTags += 1;
    }
    
    void ReadINFO_ICRD(BitInput *BitI, PCMFile *PCM, uint32_t ChunkSize) {
        char *ReleaseDate = calloc(ChunkSize, 1);
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            ReleaseDate[Byte] = ReadBits(BitI, 8);
        }
        PCM->WAV->Meta->ReleaseDateTag = ReleaseDate;
        if (IsOdd(ChunkSize) == true) {
            SkipBits(BitI, 8);
        }
        PCM->WAV->Meta->NumTags += 1;
    }
    
    void ReadINFO_IGNR(BitInput *BitI, PCMFile *PCM, uint32_t ChunkSize) {
        char *Genre = calloc(ChunkSize, 1);
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Genre[Byte] = ReadBits(BitI, 8);
        }
        PCM->WAV->Meta->GenreTag = Genre;
        if (IsOdd(ChunkSize) == true) {
            SkipBits(BitI, 8);
        }
        PCM->WAV->Meta->NumTags += 1;
    }
    
    void ReadINFO_INAM(BitInput *BitI, PCMFile *PCM, uint32_t ChunkSize) {
        char *Title = calloc(ChunkSize, 1);
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Title[Byte] = ReadBits(BitI, 8);
        }
        PCM->WAV->Meta->SongTitleTag = Title;
        if (IsOdd(ChunkSize) == true) {
            SkipBits(BitI, 8);
        }
        PCM->WAV->Meta->NumTags += 1;
    }
    
    void ReadINFO_IPRD(BitInput *BitI, PCMFile *PCM, uint32_t ChunkSize) {
        char *Album = calloc(ChunkSize, 1);
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Album[Byte] = ReadBits(BitI, 8);
        }
        PCM->WAV->Meta->AlbumTag = Album;
        if (IsOdd(ChunkSize) == true) {
            SkipBits(BitI, 8);
        }
        PCM->WAV->Meta->NumTags += 1;
    }
    
    void ReadINFO_ISFT(BitInput *BitI, PCMFile *PCM, uint32_t ChunkSize) { // Encoder
        char *Encoder = calloc(ChunkSize, 1);
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Encoder[Byte] = ReadBits(BitI, 8);
        }
        PCM->WAV->Meta->EncoderTag = Encoder;
        if (IsOdd(ChunkSize) == true) {
            SkipBits(BitI, 8);
        }
        PCM->WAV->Meta->NumTags += 1;
    }
    
    void ParseWavLISTChunk(BitInput *BitI, PCMFile *PCM, uint32_t ChunkSize) {
        uint32_t ShouldBeINFO = ReadBits(BitI, 32);
        if (ShouldBeINFO == WAV_INFO) {
            uint32_t ChunkID   = ReadBits(BitI, 32);
            uint32_t ChunkSize = ReadBits(BitI, 32);
            
            switch (ChunkID) {
                case WAV_IART: // Artist
                    ReadINFO_IART(BitI, PCM, ChunkSize);
                    break;
                case WAV_ICRD: // Release date
                    ReadINFO_ICRD(BitI, PCM, ChunkSize);
                    break;
                case WAV_IGNR: // Genre
                    ReadINFO_IGNR(BitI, PCM, ChunkSize);
                    break;
                case WAV_INAM: // Title
                    ReadINFO_INAM(BitI, PCM, ChunkSize);
                    break;
                case WAV_IPRD: // Album
                    ReadINFO_IPRD(BitI, PCM, ChunkSize);
                    break;
                case WAV_ISFT: // Encoder
                    ReadINFO_ISFT(BitI, PCM, ChunkSize);
                    break;
                default:
                    break;
            }
        } else {
            printf("Unknown Wav LIST Chunk ID: %d\n", ShouldBeINFO);
        }
    }
    
    void ParseWavDATAChunk(BitInput *BitI, PCMFile *PCM, uint32_t ChunkSize) { //
        PCM->WAV->SampleCount            = ((ChunkSize / PCM->WAV->Channels) / Bits2Bytes(PCM->WAV->BitDepth));
    }
    
    void ParseWavFMTChunk(BitInput *BitI, PCMFile *PCM, uint32_t ChunkSize) {
        PCM->WAV->FMT                    = ReadBits(BitI, 32);
        PCM->WAV->FMTSize                = SwapEndian32(ReadBits(BitI, 32));
        if (PCM->WAV->FMTSize > 16) {
            PCM->WAV->FMTType            = SwapEndian16(ReadBits(BitI, 16));
        }
        PCM->WAV->CompressionAlgorithm   = SwapEndian16(ReadBits(BitI, 16));
        if (PCM->WAV->FMTSize == 16) {
            PCM->WAV->Channels           = SwapEndian16(ReadBits(BitI, 16));
        }
        PCM->WAV->SampleRate             = SwapEndian32(ReadBits(BitI, 32));
        PCM->WAV->ByteRate               = SwapEndian32(ReadBits(BitI, 32));
        PCM->WAV->BlockSize              = SwapEndian16(ReadBits(BitI, 16));
        PCM->WAV->BitDepth               = SwapEndian16(ReadBits(BitI, 16));
        if (PCM->WAV->FMTType == WAVFormatExtensible) {
            PCM->WAV->ExtensionSize      = SwapEndian16(ReadBits(BitI, 16));
            PCM->WAV->ValidBitsPerSample = SwapEndian16(ReadBits(BitI, 16));
            PCM->WAV->SpeakerMask        = SwapEndian32(ReadBits(BitI, 32));
            SkipBits(BitI, PCM->WAV->ExtensionSize - 8); // 8 bytes from ^
        }
    }
    
    void WAVExtractSamples(BitInput *BitI, PCMFile *PCM, uint32_t NumSamples2Extract) {
        uint32_t   Samples[PCM->Data->NumChannels][NumSamples2Extract];
        for (uint16_t Channel = 0; Channel < PCM->Data->NumChannels; Channel++) {
            for (uint32_t Sample = 0; Sample < NumSamples2Extract; Sample++) {
                Samples[Channel][Sample] = ReadBits(BitI, Bits2Bytes(PCM->Data->BitDepth));
            }
        }
        PCM->Samples    = *Samples;
        PCM->NumSamples = NumSamples2Extract;
    }
    
#ifdef __cplusplus
}
#endif
