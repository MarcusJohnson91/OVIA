#include "../../include/Private/FLACCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void OVIA_FLAC_CUE_SetTrack(uint64_t Offset, bool IsAudio, bool PreEmphasis, UTF8 *ISRC) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->CueSheet->NumTracks += 1;
            
            uint8_t CurrentTrack = Ovia->FLACInfo->CueSheet->NumTracks - 1;
            
            Ovia->FLACInfo->CueSheet->Tracks[CurrentTrack].TrackOffset = Offset;
            Ovia->FLACInfo->CueSheet->Tracks[CurrentTrack].IsAudio     = IsAudio;
            Ovia->FLACInfo->CueSheet->Tracks[CurrentTrack].PreEmphasis = PreEmphasis;
            Ovia->FLACInfo->CueSheet->Tracks[CurrentTrack].ISRC        = UTF8_Clone(ISRC);
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void   FLACWriteMetadata(BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, FLACMagic);
            bool IsLastMetadataBlock = false;
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 1, IsLastMetadataBlock);
            uint8_t MetadataBlockType = 1;
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 7, MetadataBlockType);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_Encode(Audio2DContainer *Audio, BitBuffer *BitB) {
        if (Ovia != NULL && Audio && BitB != NULL) {
            if (OVIA_FLAC_GetEncodeSubset(Ovia) == true && OVIA_GetSampleRate(Ovia) <= 48000) {
                OVIA_FLAC_SetMaxBlockSize(Ovia, 4608);
                OVIA_FLAC_SetMaxFilterOrder(Ovia, 12);
                OVIA_FLAC_SetMaxRicePartitionOrder(Ovia, 8);
            } else {
                OVIA_FLAC_SetMaxBlockSize(Ovia, 16384);
                OVIA_FLAC_SetMaxFilterOrder(Ovia, 32);
                OVIA_FLAC_SetMaxRicePartitionOrder(Ovia, 15);
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (Audio == NULL) {
            Log(Log_ERROR, __func__, U8("Audio2DContainer Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void   OVIA_FLAC_WriteStreamInfo(BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 24, 34); // StreamInfoSize
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 16, OVIA_FLAC_GetMinBlockSize(Ovia));
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 16, OVIA_FLAC_GetMaxBlockSize(Ovia));
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 24, OVIA_FLAC_GetMinFrameSize(Ovia));
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 24, OVIA_FLAC_GetMaxFrameSize(Ovia));
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 20, OVIA_GetSampleRate(Ovia));
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst,  3, OVIA_GetSampleRate(Ovia) - 1);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst,  5, OVIA_GetBitDepth(Ovia) - 1);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 36, OVIA_GetNumSamples(Ovia));
            BitBuffer_Seek(BitB, 128); // Room for the MD5
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void   OVIA_FLAC_WriteTags(BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            UTF8 OVIA_Vendor[] = U8("libOVIA");
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, 7);
            for (uint32_t TagByte = 0; TagByte < 7; TagByte++) {
                BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 8, OVIA_Vendor[TagByte]);
            }
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, OVIA_GetNumTags(Ovia));
            for (uint32_t UserTag = 0; UserTag < OVIA_GetNumTags(Ovia); UserTag++) {
                BitBuffer_WriteUTF8(Ovia, OVIA_GetTag(Ovia, UserTag));
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
#ifdef __cplusplus
}
#endif
