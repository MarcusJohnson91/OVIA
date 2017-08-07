#include "../../include/libPCM.h"
#include "../../include/Private/libPCMTypes.h"
#include "../../include/Private/Audio/WAVCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    const static uint8_t BitDepth2SampleSizeInBytes[32] = {
         0,  8,  8,  8,  8,  8,  8,  8,
         8, 16, 16, 16, 16, 16, 16, 16,
        16, 24, 24, 24, 24, 24, 24, 24,
        24, 32, 32, 32, 32, 32, 32, 32
    };
    
    void ReadINFO_IART(WAVHeader *Wav, BitBuffer *BitB, uint32_t ChunkSize) {
        char *Artist = calloc(1, ChunkSize);
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Artist[Byte] = ReadBits(BitB, 8, true);
        }
        Wav->Meta->ArtistTag = Artist;
        if (IsOdd(ChunkSize) == true) {
            SkipBits(BitB, 8);
        }
        Wav->Meta->NumTags += 1;
    }
    
    void ReadINFO_ICRD(WAVHeader *Wav, BitBuffer *BitB, uint32_t ChunkSize) {
        char *ReleaseDate = calloc(1, ChunkSize);
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            ReleaseDate[Byte] = ReadBits(BitB, 8, true);
        }
        Wav->Meta->ReleaseDateTag = ReleaseDate;
        if (IsOdd(ChunkSize) == true) {
            SkipBits(BitB, 8);
        }
        Wav->Meta->NumTags += 1;
    }
    
    void ReadINFO_IGNR(WAVHeader *Wav, BitBuffer *BitB, uint32_t ChunkSize) {
        char *Genre = calloc(1, ChunkSize);
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Genre[Byte] = ReadBits(BitB, 8, true);
        }
        Wav->Meta->GenreTag = Genre;
        if (IsOdd(ChunkSize) == true) {
            SkipBits(BitB, 8);
        }
        Wav->Meta->NumTags += 1;
    }
    
    void ReadINFO_INAM(WAVHeader *Wav, BitBuffer *BitB, uint32_t ChunkSize) {
        char *Title = calloc(1, ChunkSize);
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Title[Byte] = ReadBits(BitB, 8, true);
        }
        Wav->Meta->SongTitleTag = Title;
        if (IsOdd(ChunkSize) == true) {
            SkipBits(BitB, 8);
        }
        Wav->Meta->NumTags += 1;
    }
    
    void ReadINFO_IPRD(WAVHeader *Wav, BitBuffer *BitB, uint32_t ChunkSize) {
        char *Album              = calloc(1, ChunkSize);
        for (uint8_t Byte        = 0; Byte < ChunkSize; Byte++) {
            Album[Byte]          = ReadBits(BitB, 8, true);
        }
        Wav->Meta->AlbumTag = Album;
        if (IsOdd(ChunkSize) == true) {
            SkipBits(BitB, 8);
        }
        Wav->Meta->NumTags += 1;
    }
    
    void ReadINFO_ISFT(WAVHeader *Wav, BitBuffer *BitB, uint32_t ChunkSize) { // Encoder
        char *Encoder     = calloc(1, ChunkSize);
        for (uint8_t Byte = 0; Byte < ChunkSize; Byte++) {
            Encoder[Byte] = ReadBits(BitB, 8, true);
        }
        Wav->Meta->EncoderTag = Encoder;
        if (IsOdd(ChunkSize) == true) {
            SkipBits(BitB, 8);
        }
        Wav->Meta->NumTags += 1;
    }
    
    void ParseWavLISTChunk(WAVHeader *Wav, BitBuffer *BitB, uint32_t ChunkSize) {
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
    
    void ParseWavDATAChunk(WAVHeader *Wav, BitBuffer *BitB, uint32_t ChunkSize) { //
        Wav->SampleCount            = ((ChunkSize / Wav->Channels) / Bits2Bytes(Wav->BitDepth, true));
    }
    
    void ParseWavFMTChunk(WAVHeader *Wav, BitBuffer *BitB, uint32_t ChunkSize) {
        Wav->FMT                    = ReadBits(BitB, 32, true);
        Wav->FMTSize                = SwapEndian32(ReadBits(BitB, 32, true));
        if (Wav->FMTSize > 16) {
            Wav->FMTType            = SwapEndian16(ReadBits(BitB, 16, true));
        }
        Wav->CompressionAlgorithm   = SwapEndian16(ReadBits(BitB, 16, true));
        if (Wav->FMTSize == 16) {
            Wav->Channels           = SwapEndian16(ReadBits(BitB, 16, true));
        }
        Wav->SampleRate             = SwapEndian32(ReadBits(BitB, 32, true));
        Wav->ByteRate               = SwapEndian32(ReadBits(BitB, 32, true));
        Wav->BlockSize              = SwapEndian16(ReadBits(BitB, 16, true));
        Wav->BitDepth               = SwapEndian16(ReadBits(BitB, 16, true));
        if (Wav->FMTType == WAVFormatExtensible) {
            Wav->ExtensionSize      = SwapEndian16(ReadBits(BitB, 16, true));
            Wav->ValidBitsPerSample = SwapEndian16(ReadBits(BitB, 16, true));
            Wav->SpeakerMask        = SwapEndian32(ReadBits(BitB, 32, true));
            SkipBits(BitB, Wav->ExtensionSize - 8); // 8 bytes from ^
        }
    }
    
    void WAVExtractSamples(WAVHeader *Wav, BitBuffer *BitB, uint64_t NumSamples2Extract) {
        for (uint8_t Channel = 0; Channel < Wav->Channels; Channel++) {
            for (uint64_t Sample = 0; Sample < NumSamples2Extract; Sample++) {
                Samples[Channel][Sample] = ReadBits(BitB, BitDepth2SampleSizeInBytes[BitDepth], false);
            }
        }
        NumSamples = NumSamples2Extract;
    }
    
#ifdef __cplusplus
}
#endif
