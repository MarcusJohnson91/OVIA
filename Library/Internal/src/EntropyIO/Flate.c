#include "../../include/Private/InternalOVIA.h"
#include "../../include/Private/Image/Flate.h"

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
            Log(Log_ERROR, __func__, U8("Start: %lld is larger than the BitBuffer %lld"), Start * 8, BitBuffer_GetSize(BitB));
        } else if ((Start + NumBytes) * 8 <= BitBuffer_GetSize(BitB)) {
            Log(Log_ERROR, __func__, U8("End: %lld is larger than the BitBuffer %lld"), (Start + NumBytes) * 8, BitBuffer_GetSize(BitB));
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
            Log(Log_ERROR, __func__, U8("Start: %lld is larger than the BitBuffer %lld"), Start * 8, BitBuffer_GetSize(BitB));
        } else if ((Start + NumBytes) * 8 <= BitBuffer_GetSize(BitB)) {
            Log(Log_ERROR, __func__, U8("End: %lld is larger than the BitBuffer %lld"), (Start + NumBytes) * 8, BitBuffer_GetSize(BitB));
        }
        return ~CRC32;
    }
    
    HuffmanTable *OVIA_PNG_Huffman_BuildTree(uint64_t NumSymbols, const uint16_t *CodeLengths) {
        HuffmanTable *Tree   = calloc(1, sizeof(HuffmanTable));
        if (Tree != NULL) {
            Tree->Symbols    = calloc(NumSymbols, sizeof(uint16_t));
            Tree->Frequency  = calloc(NumSymbols, sizeof(uint16_t));
            uint64_t *Offset = calloc(NumSymbols, sizeof(uint64_t));
            
            if (Tree->Symbols != NULL && Tree->Frequency != NULL) {
                for (uint64_t Value = 0ULL; Value < NumSymbols - 1; Value++) {
                    Tree->Frequency[CodeLengths[Value]] += 1;
                }
                
                if (Tree->Frequency[0] == NumSymbols) {
                    Log(Log_ERROR, __func__, U8("All frequencies are zero, that doesn't make sense..."));
                }
                
                for (uint64_t Length = 1ULL; Length < MaxBitsPerSymbol; Length++) {
                    Offset[Length + 1]   = Offset[Length] + Tree->Frequency[Length];
                }
                
                for (uint64_t Symbol = 0ULL; Symbol < NumSymbols; Symbol++) {
                    if (CodeLengths[Symbol] != 0) {
                        uint16_t Index       = Offset[CodeLengths[Symbol]] + 1;
                        Tree->Symbols[Index] = Symbol;
                    }
                }
            } else if (Tree->Frequency == NULL) {
                Log(Log_ERROR, __func__, U8("Could not allocate Huffman Symbols"));
            } else if (Tree->Frequency == NULL) {
                Log(Log_ERROR, __func__, U8("Could not allocate Huffman Frequency"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("Could not allocate Huffman Tree"));
        }
        return Tree;
    }
    
#ifdef __cplusplus
}
#endif
