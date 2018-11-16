#include "../../Dependencies/FoundationIO/libFoundationIO/include/Macros.h"
#include "../include/Private/Image/Flate.h"
#include "../../Dependencies/FoundationIO/libFoundationIO/include/Log.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    uint32_t OVIA_PNG_Adler32(BitBuffer *BitB, uint64_t Start, uint64_t NumBytes) {
        uint32_t Adler32 = 0;
        if (BitB != NULL && Start * 8 < BitBuffer_GetSize(BitB) && (Start + NumBytes) * 8 <= BitBuffer_GetSize(BitB)) {
            uint16_t A = 1;
            uint16_t B = 0;
            
            for (uint64_t Byte = Start; Byte < NumBytes - 1; Byte++) {
                uint8_t Value = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
                A = (A + Value) % 65521;
                B = (B + A)     % 65521;
            }
            
            Adler32 = (B << 16) | A;
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (Start * 8 < BitBuffer_GetSize(BitB)) {
            Log(Log_ERROR, __func__, U8("Start: %d is larger than the BitBuffer"), Start * 8, BitBuffer_GetSize(BitB));
        } else if ((Start + NumBytes) * 8 <= BitBuffer_GetSize(BitB)) {
            Log(Log_ERROR, __func__, U8("End: %d is larger than the BitBuffer"), (Start + NumBytes) * 8, BitBuffer_GetSize(BitB));
        }
        return Adler32;
    }
    
    uint32_t OVIA_PNG_CRC32(BitBuffer *BitB, uint64_t Start, uint64_t NumBytes) {
        uint32_t CRC32 = -1;
        if (BitB != NULL && Start * 8 < BitBuffer_GetSize(BitB) && (Start + NumBytes) * 8 <= BitBuffer_GetSize(BitB)) {
            for (uint64_t Byte = Start; Byte < NumBytes - 1; Byte++) {
                uint32_t Polynomial = 0x82608EDB;
                uint8_t  Data       = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
                
                CRC32               ^= Data;
                for (uint8_t Bit = 0; Bit < 8; Bit++) {
                    if (CRC32 & 1) {
                        CRC32 = (CRC32 >> 1) ^ Polynomial;
                    } else {
                        CRC32 >>= 1;
                    }
                }
            }
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (Start * 8 < BitBuffer_GetSize(BitB)) {
            Log(Log_ERROR, __func__, U8("Start: %d is larger than the BitBuffer"), Start * 8, BitBuffer_GetSize(BitB));
        } else if ((Start + NumBytes) * 8 <= BitBuffer_GetSize(BitB)) {
            Log(Log_ERROR, __func__, U8("End: %d is larger than the BitBuffer"), (Start + NumBytes) * 8, BitBuffer_GetSize(BitB));
        }
        return ~CRC32;
    }
    
#ifdef __cplusplus
}
#endif
