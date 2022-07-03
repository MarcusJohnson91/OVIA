#include "../../include/EntropyIO/Flate.h"
#include "../../include/EntropyIO/Huffman.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h" /* Included for Assertions */
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    /*
     Flate byte order: MSByteIsRight MSBitIsLeft
     */
    typedef struct FlateOptions {
        HuffmanOptions *Length;
        HuffmanOptions *Distance;
    } FlateOptions;

    FlateOptions *Flate_Init(void) {
        FlateOptions *Options  = calloc(1, sizeof(FlateOptions));
        AssertIO(Options != NULL);
        Options->Length = calloc(1, sizeof(HuffmanOptions));
        Options->Distance      = calloc(1, sizeof(HuffmanOptions));
        return Options;
    }

    static void Flate_ReadBlockHeader(FlateOptions *Options, BitBuffer *BitB, Flate_BlockTypes BlockType) {
        if (BlockType == BlockType_Literal) {
            BitBuffer_Align(BitB, 1);
            uint16_t LEN  = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 16);
            uint16_t NLEN = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 16) ^ 0xFFFF;
            AssertIO(LEN == NLEN);
            // Copy LEN bytes from the stream to the image
            // Literal block up to 65535 bytes of data
        } else if (BlockType == BlockType_Fixed) {
            Options->Length->NumEntries = 288;
            for (size_t Length = 0; Length < 288; Length++) {
                if (Length <= 143) {
                    size_t Symbol = 0x30 + Length;
                    Options->Length->Entries[Length].Bitlength = 8;
                    Options->Length->Entries[Length].Symbol    = Symbol;
                } else if (Length <= 255) {
                    size_t Symbol = 0x190 + Length;
                    Options->Length->Entries[Length].Bitlength = 9;
                    Options->Length->Entries[Length].Symbol    = Symbol;
                } else if (Length <= 279) {
                    size_t Symbol = 0x0 + Length;
                    Options->Length->Entries[Length].Bitlength = 7;
                    Options->Length->Entries[Length].Symbol    = Symbol;
                } else if (Length <= 287) {
                    size_t Symbol = 0xC0 + Length;
                    Options->Length->Entries[Length].Bitlength = 8;
                    Options->Length->Entries[Length].Symbol    = Symbol;
                }
            }
            Options->Distance->NumEntries = 31;
            for (size_t Distance = 0; Distance < 31; Distance++) {
                Options->Distance->Entries[Distance].Bitlength = 5;

            }
        } else if (BlockType == BlockType_Dynamic) {
            uint16_t NumLengthCodes               = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 5) + 257; // HLIT; 21, 26?
            uint8_t  NumDistCodes                 = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 5) + 1; // HDIST; 17, 9?
            uint8_t  NumCodeLengthCodes           = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 4) + 4; // HCLEN; 13,
            AssertIO(NumLengthCodes <= MaxLiteralLengthCodes);
            AssertIO(NumDistCodes <= MaxDistanceCodes);

            uint8_t *CodeLengthCodes = calloc(NumCodeLengthCodes, sizeof(uint8_t));

            for (uint8_t CodeLengthCode = 0; CodeLengthCode < NumCodeLengthCodes; CodeLengthCode++) {
                CodeLengthCodes[MetaCodeLengthOrder[CodeLengthCode]] = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Left2Right, 3);
            }

            HuffmanOptions *Tree2DecodeTrees      = HuffmanTree_Init(CodeLengthCodeLengths, NumMetaCodes);
            uint16_t Index = 0;
            do {
                uint64_t Length2Repeat        = 0; // len

                uint16_t Symbol               = ReadSymbol(BitB, Tree2DecodeTrees);
                if (Symbol <= MaxBitsPerSymbol) {
                    CodeLengthCodeLengths[Index + 1] = Symbol;
                } else {
                    if (Symbol == 16) {
                        Length2Repeat         = CodeLengthCodeLengths[Index - 1];
                        Symbol                = 3 + BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Left2Right, 2);
                    } else if (Symbol == 17) {
                        Symbol                = 3 + BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Left2Right, 3);
                    } else {
                        Symbol                = 11 + BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Left2Right, 7);
                    }
                    while (Symbol -= 1) {
                        CodeLengthCodeLengths[Index += 1] = Length2Repeat;
                    }
                }
                Index                        += 1;
            } while (Index < NumLengthCodes + NumDistCodes);
        }
    }

    /*
     Deflate: Header is written least significant bit first aka MSBitIsLeft
     */

    static bool Flate_VerifyZlibHeader(uint16_t Values, uint16_t FCHECK) {
        AssertIO((Values & 0xF0) == 7); // Compression Info
        AssertIO((Values & 0x0F) == 8); // Compression Method
        uint16_t FCheck = (Values & 0xF000) << 12;
        FCheck         |= (Values & 0x0F00) << 8;
        FCheck         |= (Values & 0x00C0) << 6;
        FCheck         |= (Values & 0x0020) << 5; // FDICT
        FCheck         |= (Values & 0x001F); // FCHECK
        if (FCheck % 31 == 0) {
            return true;
        } else {
            return false;
        }
    }

    void Flate_ReadZlibHeader(FlateOptions *Options, BitBuffer *BitB) { // This will be the main function for each Zlib block
        AssertIO(BitB != NULL);
        uint16_t Values   = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 16);
        uint16_t FCHECK   = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 16);
        bool HeaderIsGood = Flate_VerifyZlibHeader(Values, FCHECK);
        if (HeaderIsGood) {
            BitBuffer_Seek(BitB, -32);
            uint8_t  CompressionMethod = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 4); // 8
            uint8_t  CompressionInfo   = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 4); // 7
            uint8_t  FLEVEL            = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 2); // 0b00
            bool     FDICT             = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1); // 0b0
            uint8_t  FCHECK            = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 5); // 0b00001
            if (FDICT) {
                uint32_t DictID        = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
            }
            /* Deflate compressed data continues... */
            /*
             BFINAL = 0?
             BTYPE  = 1? aka Fixed Huffman
             Build Huffman tree for fixed huffman

             288 bytees for Length table = {ECDB43D6 24001084 C11C676B 6CDBB66D DBB6EEBF FE2F50B5 CF7EEF8B 7B84A61C 0000B366 CFA9CC9D 375FD180 05EE0C86 02928DDC 1A0B4836 716BA180 648BDC5A AC64C012 B7962A19 B0CCADE5 4A06AC70 6BA59201 ABDC5AAD 64C01AB7 D60A48B6 CEADF502 926D706B A380649B DCDA2C20 D916B7B6 0A48B6CD 9DED02A2 ED7067A7 8068BBDC D92D20DA 1E77E60A 88B6D79D 91806CFB DCD82F20 DB01370E 2A1B70C8 8DC3CA06 1C71E3A8 B2019369 2D6CC031 378E2B1B 30CF8D13 CA069C74 E394B201 A7DD38A3 6CC05937 CE291B70 DE8D0BCA 065C74E3 92B20197 DDB8A26C C05537AE 291B70DD 8D1BCA06 DC746DA0 70C02DD7 6E2B1C70 C7B5BB0A 07DC736D 8EC201F7 5D7BA074 C043971E 291DF0D8 A5274A07 3C75E999 8074CF5D 7A2120DD 4B975E09}


             */
        }
    }

    static uint64_t ReadSymbol(HuffmanOptions *Options, BitBuffer *BitB) { // EQUILIVENT OF DECODE IN PUFF
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        uint64_t Symbol              = 0ULL;
        uint16_t Count               = 0;
        uint32_t FirstSymbolOfLength = 0;
        for (uint8_t Bit = 1; Bit <= MaxBitsPerSymbol; Bit++) {
            Symbol             <<= 1;
            Symbol              |= BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1);
            Count                = Options->Entries[Bit].Bitlength;
            if (Symbol - Count < FirstSymbolOfLength) {
                Symbol           = Options->Entries[Bit + (Symbol - FirstSymbolOfLength)].Symbol;
            }
        }
        return Symbol;
    }

    void Flate_ReadHuffman(FlateOptions *Options, BitBuffer *BitB) { // Codes in Puff
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        uint64_t Symbol = 0ULL;
        uint64_t Offset = 0ULL;
        do {
            Symbol                              = ReadSymbol(Options->Length, BitB);
            if (Symbol > 256) { // length
                Symbol  -= 257;
                uint64_t Length                 = LengthBase[Symbol] + BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Left2Right, LengthAdditionalBits[Symbol]);

                Symbol                          = ReadSymbol(Options->Distance, BitB);
                uint64_t Distance               = DistanceBase[Symbol] + BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Left2Right, DistanceAdditionalBits[Symbol]);

                for (uint64_t NumBytes2Copy = 0; NumBytes2Copy < Length; NumBytes2Copy++) {
                    // Copy NumBytes2Copde from Offset - Distance

                    // We need to convert the View, Width, Height, and Channel as well as BitDepth into a byte location.
                    // This function might also be useful in ContainerIO as well.
                }
            }
        } while (Symbol != EndOfBlock); // end of block symbol
    }

    void Flate_Deinit(FlateOptions *Options) {
        Huffman_Deinit(Options->Length);
        Huffman_Deinit(Options->Distance);
        free(Options);
    }
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
