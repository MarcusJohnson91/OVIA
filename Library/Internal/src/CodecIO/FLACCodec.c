#include "../../include/Private/FLACCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void *FLACOptions_Init(void) {
        FLACOptions *FLAC     = calloc(1, sizeof(FLACOptions));
        if (FLAC != NULL) {
            FLAC->StreamInfo  = calloc(1, sizeof(StreamInfo));
            FLAC->CueSheet    = calloc(1, sizeof(FLACCueSheet));
        } else {
            Log(Log_DEBUG, __func__, UTF8String("Couldn't allocate FLACOptions"));
        }
        return FLAC;
    }
    
    uint32_t Adler32(BitBuffer *BitB, uint64_t Start, uint64_t NumBytes) {
        uint32_t Output = 0;
        if (BitB != NULL && Start * 8 < BitBuffer_GetSize(BitB) && (Start + NumBytes) * 8 <= BitBuffer_GetSize(BitB)) {
            uint16_t A = 1;
            uint16_t B = 0;
            
            for (uint64_t Byte = Start; Byte < NumBytes - 1; Byte++) {
                uint8_t Value = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8);
                A = (A + Value) % 65521;
                B = (B + A)     % 65521;
            }
            
            Output = (B << 16) | A;
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, UTF8String("BitBuffer Pointer is NULL"));
        } else if (Start * 8 < BitBuffer_GetSize(BitB)) {
            Log(Log_DEBUG, __func__, UTF8String("Start: %lld is larger than the BitBuffer %lld"), Start * 8, BitBuffer_GetSize(BitB));
        } else if ((Start + NumBytes) * 8 <= BitBuffer_GetSize(BitB)) {
            Log(Log_DEBUG, __func__, UTF8String("End: %lld is larger than the BitBuffer %lld"), (Start + NumBytes) * 8, BitBuffer_GetSize(BitB));
        }
        return Output;
    }
    
    uint32_t CRC32(BitBuffer *BitB, uint64_t Start, uint64_t NumBytes) {
        uint32_t Output = -1;
        if (BitB != NULL && Start * 8 < BitBuffer_GetSize(BitB) && (Start + NumBytes) * 8 <= BitBuffer_GetSize(BitB)) {
            for (uint64_t Byte = Start; Byte < NumBytes - 1; Byte++) {
                uint32_t Polynomial = 0x82608EDB;
                uint8_t  Data       = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8);
                
                Output               ^= Data;
                for (uint8_t Bit = 0; Bit < 8; Bit++) {
                    if (Output & 1) {
                        Output = (Output >> 1) ^ Polynomial;
                    } else {
                        Output >>= 1;
                    }
                }
            }
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, UTF8String("BitBuffer Pointer is NULL"));
        } else if (Start * 8 < BitBuffer_GetSize(BitB)) {
            Log(Log_DEBUG, __func__, UTF8String("Start: %lld is larger than the BitBuffer %lld"), Start * 8, BitBuffer_GetSize(BitB));
        } else if ((Start + NumBytes) * 8 <= BitBuffer_GetSize(BitB)) {
            Log(Log_DEBUG, __func__, UTF8String("End: %lld is larger than the BitBuffer %lld"), (Start + NumBytes) * 8, BitBuffer_GetSize(BitB));
        }
        return ~Output;
    }
    
    uint8_t FLAC_GetNumChannels(void *Options) {
        uint8_t NumChannels = 0;
        if (Options != NULL) {
            FLACOptions *FLAC = Options;
            if (FLAC->Frame->ChannelLayout <= 7) {
                NumChannels = FLAC->Frame->ChannelLayout + 1;
            } else if (FLAC->Frame->ChannelLayout <= 11) {
                NumChannels = 2;
            }
        } else {
            Log(Log_DEBUG, __func__, UTF8String("Options Pointer is NULL"));
        }
        return NumChannels;
    }
    
    uint16_t FLAC_GetBlockSizeInSamples(void *Options) {
        uint16_t SamplesInBlock = 0;
        FLACOptions *FLAC = Options;
        if (FLAC->StreamInfo->CodedSampleRate == 1) {
            SamplesInBlock = 192;
        } else if (FLAC_GetBlockSizeInSamples >= 2 && FLAC_GetBlockSizeInSamples <= 5) {
            SamplesInBlock = 576 * Exponentiate(2, FLAC->Frame->CodedBlockSize - 2);
        } else if (FLAC_GetBlockSizeInSamples == 6) {
            // get 8 bit block from the end of the header
        } else if (FLAC_GetBlockSizeInSamples == 7) {
            // get 16 bit block from the end of the header
        } else if (FLAC_GetBlockSizeInSamples >= 8 && FLAC_GetBlockSizeInSamples <= 15) {
            SamplesInBlock = 256 * Exponentiate(2, FLAC->Frame->CodedBlockSize - 8);
        } else {
            // Reserved
        }
        return SamplesInBlock;
    }
    
    void FLACOptions_Deinit(void *Options) {
        FLACOptions *FLAC = Options;
        free(FLAC->StreamInfo);
        free(FLAC->CueSheet);
        free(FLAC);
    }
    
#ifdef __cplusplus
}
#endif
