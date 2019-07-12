#include "../../include/Private/WAVCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    /*
     This encoder works for WAVE, BroadcastWAVE, RF64
     
     BWF Spcific Chunks:
     BEXT
     UBXT
     
     BW64 Specific Chunks:
     BW64
     DS64
     JUNK
     AXML
     CHNA
     
     BW64 is used instead of RIFF
     DS64 replaces WAVE
     
     */
    
    void WAVParseChunk_DS64(WAVOptions *WAV, BitBuffer *BitB) {
        if (WAV != NULL && BitB != NULL) {
            uint32_t ChunkSize    = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
            uint32_t SizeLow      = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
            uint32_t SizeHigh     = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
            uint32_t DataSizeLow  = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
            uint32_t DataSizeHigh = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
            
        } else if (WAV == NULL) {
            Log(Log_DEBUG, __func__, U8("WAVOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void ReadINFO_TRCK(WAVOptions *WAV, BitBuffer *BitB) {
        if (WAV != NULL && BitB != NULL) {
            uint64_t StringSize            = BitBuffer_GetUTF8StringSize(BitB);
            WAV->Info->Artist              = BitBuffer_ReadUTF8(BitB, StringSize);
        } else if (WAV == NULL) {
            Log(Log_DEBUG, __func__, U8("WAVOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void ReadINFO_IPRT(WAVOptions *WAV, BitBuffer *BitB) {
        if (WAV != NULL && BitB != NULL) {
            uint64_t StringSize            = BitBuffer_GetUTF8StringSize(BitB);
            WAV->Info->Artist              = BitBuffer_ReadUTF8(BitB, StringSize);
        } else if (WAV == NULL) {
            Log(Log_DEBUG, __func__, U8("WAVOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void ReadINFO_IART(WAVOptions *WAV, BitBuffer *BitB) {
        if (WAV != NULL && BitB != NULL) {
            uint64_t StringSize            = BitBuffer_GetUTF8StringSize(BitB);
            WAV->Info->Artist              = BitBuffer_ReadUTF8(BitB, StringSize);
        } else if (WAV == NULL) {
            Log(Log_DEBUG, __func__, U8("WAVOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void ReadINFO_ICRD(WAVOptions *WAV, BitBuffer *BitB) {
        if (WAV != NULL && BitB != NULL) {
            uint64_t StringSize            = BitBuffer_GetUTF8StringSize(BitB);
            WAV->Info->ReleaseDate         = BitBuffer_ReadUTF8(BitB, StringSize);
        } else if (WAV == NULL) {
            Log(Log_DEBUG, __func__, U8("WAVOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void ReadINFO_IGNR(WAVOptions *WAV, BitBuffer *BitB) {
        if (WAV != NULL && BitB != NULL) {
            uint64_t StringSize            = BitBuffer_GetUTF8StringSize(BitB);
            WAV->Info->Genre               = BitBuffer_ReadUTF8(BitB, StringSize);
        } else if (WAV == NULL) {
            Log(Log_DEBUG, __func__, U8("WAVOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void ReadINFO_INAM(WAVOptions *WAV, BitBuffer *BitB) {
        if (WAV != NULL && BitB != NULL) {
            uint64_t StringSize            = BitBuffer_GetUTF8StringSize(BitB);
            WAV->Info->Title               = BitBuffer_ReadUTF8(BitB, StringSize);
        } else if (WAV == NULL) {
            Log(Log_DEBUG, __func__, U8("WAVOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void ReadINFO_IPRD(WAVOptions *WAV, BitBuffer *BitB) {
        if (WAV != NULL && BitB != NULL) {
            uint64_t StringSize            = BitBuffer_GetUTF8StringSize(BitB);
            WAV->Info->Album               = BitBuffer_ReadUTF8(BitB, StringSize);
        } else if (WAV == NULL) {
            Log(Log_DEBUG, __func__, U8("WAVOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void ReadINFO_ISFT(WAVOptions *WAV, BitBuffer *BitB) { // Encoder
        if (WAV != NULL && BitB != NULL) {
            uint64_t StringSize            = BitBuffer_GetUTF8StringSize(BitB);
            WAV->Info->CreationSoftware    = BitBuffer_ReadUTF8(BitB, StringSize);
        } else if (WAV == NULL) {
            Log(Log_DEBUG, __func__, U8("WAVOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void WAVParseLISTChunk(WAVOptions *WAV, BitBuffer *BitB) {
        if (WAV != NULL && BitB != NULL) {
            uint32_t SubChunkID   = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
            uint32_t SubChunkSize = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
            
            switch (SubChunkID) {
                case WAV_IART: // Artist
                    ReadINFO_IART(WAV, BitB);
                    break;
                case WAV_ICRD: // Release date
                    ReadINFO_ICRD(WAV, BitB);
                    break;
                case WAV_IGNR: // Genre
                    ReadINFO_IGNR(WAV, BitB);
                    break;
                case WAV_INAM: // Title
                    ReadINFO_INAM(WAV, BitB);
                    break;
                case WAV_IPRD: // Album
                    ReadINFO_IPRD(WAV, BitB);
                    break;
                case WAV_ISFT: // Encoder
                    ReadINFO_ISFT(WAV, BitB);
                    break;
                default:
                    Log(Log_DEBUG, __func__, U8("Unknown LIST Chunk: 0x%X"), SubChunkID);
                    break;
            }
        } else if (WAV == NULL) {
            Log(Log_DEBUG, __func__, U8("WAVOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    static void WAVParseFMTChunk(WAVOptions *WAV, BitBuffer *BitB) {
        if (WAV != NULL && BitB != NULL) {
            uint32_t ChunkSize     = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
            WAV->CompressionFormat = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16);
            WAV->NumChannels       = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16);
            WAV->SampleRate        = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
            WAV->ByteRate          = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
            WAV->BlockAlign        = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16);
            WAV->BitDepth          = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16);
            if (ChunkSize == 18) {
                uint16_t CBSize             = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16);
                BitBuffer_Seek(BitB, Bytes2Bits(CBSize - 16));
            } else if (ChunkSize == 40) {
                uint16_t CBSize             = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16);
                WAV->ValidBitsPerSample     = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16);
                
                WAV->SpeakerMask            = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
                uint8_t  *BinaryGUIDFormat  = BitBuffer_ReadGUUID(BitB, BinaryGUID);
                BitBuffer_Seek(BitB, Bytes2Bits(CBSize - 22));
            }
        } else if (WAV == NULL) {
            Log(Log_DEBUG, __func__, U8("WAVOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WAVParseMetadata(WAVOptions *WAV, BitBuffer *BitB) {
        if (WAV != NULL && BitB != NULL) {
            uint32_t ChunkID   = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 32);
            uint32_t ChunkSize = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 32);
            
            switch (ChunkID) {
                case WAV_LIST:
                    break;
                case WAV_FMT:
                    WAVParseFMTChunk(WAV, BitB);
                    break;
                case WAV_WAVE:
                    BitBuffer_Seek(BitB, 32);
                    break;
                default:
                    Log(Log_DEBUG, __func__, U8("Invalid ChunkID: 0x%X"), ChunkID);
                    break;
            }
        } else if (WAV == NULL) {
            Log(Log_DEBUG, __func__, U8("WAVOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void WAVExtractSamples(WAVOptions *WAV, BitBuffer *BitB, Audio2DContainer *Audio) {
        if (WAV != NULL && BitB != NULL && Audio != NULL) {
            uint64_t NumSamples        = Audio2DContainer_GetNumSamples(Audio);
            uint8_t  SampleSizeRounded = (uint8_t) Bytes2Bits(Bits2Bytes(WAV->BitDepth, RoundingType_Up));
            if (WAV->BitDepth <= 8) {
                uint8_t **Samples = (uint8_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < WAV->NumChannels; Channel++) {
                        Samples[Channel][Sample] = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, SampleSizeRounded);
                    }
                }
            } else if (WAV->BitDepth <= 16) {
                uint16_t **Samples = (uint16_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < WAV->NumChannels; Channel++) {
                        Samples[Channel][Sample] = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, SampleSizeRounded);
                    }
                }
            } else if (WAV->BitDepth <= 32) {
                uint32_t **Samples = (uint32_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0; Sample < NumSamples; Sample++) {
                    for (uint64_t Channel = 0; Channel < WAV->NumChannels; Channel++) {
                        Samples[Channel][Sample] = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, SampleSizeRounded);
                    }
                }
            }
        } else if (WAV == NULL) {
            Log(Log_DEBUG, __func__, U8("WAVOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (Audio == NULL) {
            Log(Log_DEBUG, __func__, U8("Audio2DContainer Pointer is NULL"));
        }
    }
    
    OVIADecoder WAVDecoder = {
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
