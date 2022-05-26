#include "../../include/EntropyIO/Flate.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h" /* Included for Assertions */
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    /*
     Deflate: Header is written least significant bit first aka MSBitIsLeft
     */
    typedef struct FlateOptions {
        uint32_t Code_Lengths;
        uint16_t NumLiteralCodes; // HLIT
        uint8_t  NumDistanceCodes; // HDIST
        uint8_t  NumLengthCodes; // HCLEN
    } FlateOptions;

    FlateOptions *FlateOpions_Init(void) {
        return calloc(1, sizeof(FlateOptions));
    }
    
    void Flate_ReadZlibHeader(FlateOptions *Options, BitBuffer *BitB) { // This will be the main function for each Zlib block
        AssertIO(BitB != NULL);
        uint8_t  CompressionMethod = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 4);
        uint8_t  CompressionInfo   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 4);
        uint8_t  FCHECK            = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 5);
        bool     FDICT             = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 1);
        uint8_t  FLEVEL            = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 2);
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
            DictID  = (uint32_t) BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32);
        }
    }

    static void Flate_ReadLiteralBlock(FlateOptions *Options, BitBuffer *BitB) {
        BitBuffer_Align(BitB, 1);
        uint16_t LEN  = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 16);
        uint16_t NLEN = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 16) ^ 0xFFFF;
        AssertIO(LEN == NLEN);
        // Copy LEN bytes from the stream to the image
        // Literal block up to 65535 bytes of data
    }

    static void Flate_ReadFixedBlock(FlateOptions *Options, BitBuffer *BitB) {
        HuffmanTree *Length        = HuffmanTree_Init(FixedLiteralTable, 288);
        HuffmanTree *Distance      = HuffmanTree_Init(FixedDistanceTable, 32);
        Flate_ReadHuffman(Options, BitB, Length, Distance, Image);
    }

    static void Flate_ReadDynamicBlock(FlateOptions *Options, BitBuffer *BitB) {
        uint16_t NumLengthCodes               = 257 + BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 5); // HLIT; 21, 26?
        uint8_t  NumDistCodes                 = 1   + BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 5); // HDIST; 17, 9?
        uint8_t  NumCodeLengthCodeLengthCodes = 4   + BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 4); // HCLEN; 13,
        AssertIO(NumLengthCodes <= MaxLiteralLengthCodes);
        AssertIO(NumDistCodes <= MaxDistanceCodes);
        uint16_t *CodeLengthCodeLengths   = calloc(NumMetaCodes, sizeof(uint16_t));

        for (uint8_t CodeLengthCodeLengthCode = 0; CodeLengthCodeLengthCode < NumCodeLengthCodeLengthCodes; CodeLengthCodeLengthCode++) {
            CodeLengthCodeLengths[MetaCodeLengthOrder[CodeLengthCodeLengthCode]] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsLeft, 3);
        }

        HuffmanTree *Tree2DecodeTrees     = HuffmanTree_Init(CodeLengthCodeLengths, NumMetaCodes);
        uint16_t Index = 0;
        do {
            uint64_t Length2Repeat        = 0; // len

            uint16_t Symbol               = ReadSymbol(BitB, Tree2DecodeTrees);
            if (Symbol <= MaxBitsPerSymbol) {
                CodeLengthCodeLengths[Index + 1] = Symbol;
            } else {
                if (Symbol == 16) {
                    Length2Repeat         = CodeLengthCodeLengths[Index - 1];
                    Symbol                = 3 + BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsLeft, 2);
                } else if (Symbol == 17) {
                    Symbol                = 3 + BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsLeft, 3);
                } else {
                    Symbol                = 11 + BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsLeft, 7);
                }
                while (Symbol -= 1) {
                    CodeLengthCodeLengths[Index += 1] = Length2Repeat;
                }
            }
            Index                        += 1;
        } while (Index < NumLengthCodes + NumDistCodes);
    }
    
    void Flate_ReadDeflateBlock(FlateOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        bool BFINAL           = No;
        do {
            BFINAL            = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 1); // Yes
            uint8_t BTYPE     = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 2); // 0b10 aka 2 aka BlockType_Dynamic
            switch (BTYPE) {
                case BlockType_Literal:
                    Flate_ReadLiteralBlock(Options, BitB);
                    break;

                case BlockType_Fixed:
                    Flate_ReadFixedBlock(Options, BitB);
                    break;

                case BlockType_Dynamic:
                    Flate_ReadDynamicBlock(Options, BitB);
                    break;

                case BlockType_Invalid:
                default:
                    AssertIO(BTYPE != BlockType_Invalid);
                    break;
            }
        } while (BFINAL == No);
    }

    static uint64_t ReadSymbol(BitBuffer *BitB, HuffmanTree *Tree) { // EQUILIVENT OF DECODE IN PUFF
        AssertIO(BitB != NULL);
        AssertIO(Tree != NULL);
        uint64_t Symbol              = 0ULL;
        uint16_t Count               = 0;
        uint32_t FirstSymbolOfLength = 0;
        for (uint8_t Bit = 1; Bit <= MaxBitsPerSymbol; Bit++) {
            Symbol             <<= 1;
            Symbol              |= BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 1);
            Count                = Tree->Frequency[Bit];
            if (Symbol - Count < FirstSymbolOfLength) {
                Symbol           = Tree->Symbol[Bit + (Symbol - FirstSymbolOfLength)];
            }
        }
        return Symbol;
    }

    void Flate_ReadHuffman(FlateOptions *Options, BitBuffer *BitB, HuffmanTree *LengthTree, HuffmanTree *DistanceTree) { // Codes in Puff
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        AssertIO(LengthTree != NULL);
        AssertIO(DistanceTree != NULL);
        AssertIO(Image != NULL);
        // Out = ImageContainer array
        uint64_t Symbol = 0ULL;
        uint64_t Offset = 0ULL;
        do {
            Symbol                              = ReadSymbol(BitB, LengthTree);
            if (Symbol > 256) { // length
                Symbol  -= 257;
                uint64_t Length                 = LengthBase[Symbol] + BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsLeft, LengthAdditionalBits[Symbol]);

                Symbol                          = ReadSymbol(BitB, DistanceTree);
                uint64_t Distance               = DistanceBase[Symbol] + BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsLeft, DistanceAdditionalBits[Symbol]);

                uint8_t ****ImageArray          = (uint8_t****) ImageContainer_GetArray(Image);
                AssertIO(ImageArray != NULL);
                for (uint64_t NumBytes2Copy = 0; NumBytes2Copy < Length; NumBytes2Copy++) {
                    // Copy NumBytes2Copde from Offset - Distance

                    // We need to convert the View, Width, Height, and Channel as well as BitDepth into a byte location.
                    // This function might also be useful in ContainerIO as well.
                }
            }
        } while (Symbol != EndOfBlock); // end of block symbol
    }

    /*
     Flate byte ordr: MSByteIsRight MSBitIsLeft
     */
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
