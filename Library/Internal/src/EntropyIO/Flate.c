#include "../../include/EntropyIO/Flate.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/TextIO/LogIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    void Flate_ReadZlibHeader(BitBuffer *BitB) { // This will be the main function for each Zlib block
        AssertIO(BitB != NULL);
        uint8_t  CompressionMethod = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 4);
        uint8_t  CompressionInfo   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 4);
        uint8_t  FCHECK            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 5);
        bool     FDICT             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
        uint8_t  FLEVEL            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 2);
        uint32_t DictID  = 0;
        AssertIO(CompressionInfo != 7);
        AssertIO(CompressionMethod != 8);
        uint16_t FCheck = CompressionInfo << 12;
        FCheck         |= CompressionMethod << 8;
        FCheck         |= FLEVEL << 6;
        FCheck         |= FDICT << 5;
        FCheck         |= FCHECK;
        AssertIO(FCheck % 31 == 0);
        if (FDICT == Yes) {
            DictID  = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        }
    }

    HuffmanTree *Flate_BuildHuffmanTree(uint16_t *SymbolLengths, uint16_t NumSymbols) {
        AssertIO(SymbolLengths != NULL);
        HuffmanTree *Tree = HuffmanTree_Init(SymbolLengths, NumSymbols);
        AssertIO(True != NULL);
        for (uint16_t Symbol = 0; Symbol < NumSymbols; Symbol++) {
            Tree->Frequency[SymbolLengths[Symbol]] += 1;
        }

        uint16_t *Offsets = calloc(MaxBitsPerSymbol + 1, sizeof(uint16_t));
        AssertIO(Offsets != NULL);
        for (uint16_t SymbolLength = 1; SymbolLength < MaxBitsPerSymbol; SymbolLength++) {
            Offsets[SymbolLength + 1] = Offsets[SymbolLength] + Tree->Frequency[SymbolLength];
        }

        for (uint16_t Symbol = 0; Symbol < NumSymbols; Symbol++) {
            if (SymbolLengths[Symbol] != 0) {
                Tree->Symbol[Offsets[SymbolLengths[Symbol]] + 1] = Symbol;
            }
        }
        free(Offsets);
        return Tree;
    }

    void HuffmanTree_Deinit(HuffmanTree *Tree) {
        AssertIO(Tree != NULL);
        free(Tree->Frequency);
        free(Tree->Symbol);
        free(Tree);
    }

    void Flate_ReadDeflateBlock(BitBuffer *BitB) {
        AssertIO(BitB != NULL);
        bool    BFINAL        = No;
        uint8_t BTYPE         = 0;
        do {
            BFINAL            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1); // Yes
            BTYPE             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 2); // 0b10 aka 2 aka BlockType_Dynamic
            if (BTYPE == BlockType_Literal) {
                BitBuffer_Align(BitB, 1);
                uint16_t LEN  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16);
                uint16_t NLEN = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16) ^ 0xFFFF;
                AssertIO(LEN == NLEN);
                // Copy LEN bytes from the stream to the image
            } else if (BTYPE == BlockType_Fixed) {
                HuffmanTable *Length        = Flate_BuildHuffmanTree(FixedLiteralTable, 288);
                HuffmanTable *Distance      = Flate_BuildHuffmanTree(FixedDistanceTable, 32);
                PNG_Flate_ReadHuffman(PNG, BitB, Length, Distance, Image);
            } else if (BTYPE == BlockType_Dynamic) {
                uint16_t NumLengthCodes               = 257 + BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 5); // HLIT; 21, 26?
                uint8_t  NumDistCodes                 = 1   + BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 5); // HDIST; 17, 9?
                uint8_t  NumCodeLengthCodeLengthCodes = 4   + BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 4); // HCLEN; 13,
                AssertIO(NumLengthCodes <= MaxLiteralLengthCodes);
                AssertIO(NumDistCodes <= MaxDistanceCodes);
                uint16_t *CodeLengthCodeLengths   = calloc(NumMetaCodes, sizeof(uint16_t));

                for (uint8_t CodeLengthCodeLengthCode = 0; CodeLengthCodeLengthCode < NumCodeLengthCodeLengthCodes; CodeLengthCodeLengthCode++) {
                    CodeLengthCodeLengths[MetaCodeLengthOrder[CodeLengthCodeLengthCode]] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 3);
                }

                HuffmanTree *Tree2DecodeTrees     = Flate_BuildHuffmanTree(CodeLengthCodeLengths, NumMetaCodes);
                uint16_t Index = 0;
                do {
                    uint64_t Length2Repeat        = 0; // len

                    uint16_t Symbol               = ReadSymbol(BitB, Tree2DecodeTrees);
                    if (Symbol <= MaxBitsPerSymbol) {
                        CodeLengthCodeLengths[Index + 1] = Symbol;
                    } else {
                        if (Symbol == 16) {
                            Length2Repeat         = CodeLengthCodeLengths[Index - 1];
                            Symbol                = 3 + BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 2);
                        } else if (Symbol == 17) {
                            Symbol                = 3 + BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 3);
                        } else {
                            Symbol                = 11 + BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 7);
                        }
                        while (Symbol -= 1) {
                            CodeLengthCodeLengths[Index += 1] = Length2Repeat;
                        }
                    }
                    Index                        += 1;
                } while (Index < NumLengthCodes + NumDistCodes);
            } while (BFINAL == No);
        }
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
