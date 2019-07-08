#include "../../include/Private/FLACCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void   FLACWriteMetadata(BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, FLACMagic);
            bool IsLastMetadataBlock = false;
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 1, IsLastMetadataBlock);
            uint8_t MetadataBlockType = 1;
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 7, MetadataBlockType);
        } else if (Ovia == NULL) {
            Log(Log_DEBUG, __func__, U8("FLACOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void FLAC_Encode(FLACOptions *FLAC, Audio2DContainer *Audio, BitBuffer *BitB) {
        if (FLAC != NULL && Audio && BitB != NULL) {
            if (FLAC->EncodeSubset == true && Audio2DContainer_GetSampleRate(Audio) <= 48000) {
                FLAC->StreamInfo->MaximumBlockSize = 4608;
                FLAC->Frame->Sub->LPCFilterOrder   = 12;
                FLAC->Frame->PartitionOrder        = 8;
            } else {
                FLAC->StreamInfo->MaximumBlockSize = 16384;
                FLAC->Frame->Sub->LPCFilterOrder   = 32;
                FLAC->Frame->PartitionOrder        = 15;
            }
        } else if (FLAC == NULL) {
            Log(Log_DEBUG, __func__, U8("FLACOptions Pointer is NULL"));
        } else if (Audio == NULL) {
            Log(Log_DEBUG, __func__, U8("Audio2DContainer Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void FLAC_WriteStreamInfo(FLACOptions *FLAC, BitBuffer *BitB) {
        if (FLAC != NULL && BitB != NULL) {
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 24, 34); // StreamInfoSize
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 16, FLAC->StreamInfo->MinimumBlockSize);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 16, FLAC->StreamInfo->MaximumBlockSize);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 24, FLAC->StreamInfo->MinimumFrameSize);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 24, FLAC->StreamInfo->MaximumFrameSize);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 20, FLAC->StreamInfo->CodedSampleRate);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst,  3, FLAC->StreamInfo->CodedSampleRate - 1);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst,  5, FLAC->StreamInfo->CodedBitDepth - 1);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 36, FLAC->StreamInfo->SamplesInStream));
            BitBuffer_Seek(BitB, 128); // Room for the MD5
        } else if (FLAC == NULL) {
            Log(Log_DEBUG, __func__, U8("FLACOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void   FLAC_WriteTags(BitBuffer *BitB) {
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
            Log(Log_DEBUG, __func__, U8("FLACOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
#ifdef __cplusplus
}
#endif
