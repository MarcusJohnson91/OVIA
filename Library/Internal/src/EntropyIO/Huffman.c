#include "../../include/EntropyIO/Huffman.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef struct HuffmanEntry {
        uint16_t Value;
        uint8_t  Bitlength;
    } HuffmanEntry;

    typedef struct HuffmanOptions {
        HuffmanEntry *Entries;
        size_t        NumEntries;
    } HuffmanOptions;

    HuffmanOptions *HuffmanOptions_Init(size_t NumSymbols, size_t MaxBitLength) {
        HuffmanOptions *Options = calloc(1, sizeof(HuffmanOptions));
        AssertIO(Options != NULL);
        Options->Entries = calloc(NumSymbols, sizeof(HuffmanEntry));
        AssertIO(Options->Entries != NULL);
        Options->NumEntries = NumSymbols;
        return Options;
    }

    /*
     So, Huffman can have a variable number of elements in the table, JPEG = 162, Deflate = 288

     So, when we create the Huffman table we need to be able to say the number of symbols it will contain

     Next, we need to be able to read the bitlengths from the JPEG file and create our tree; PNG needs to be able to generate default tables which should be a PNG specific thing
     */

    void Huffman_ReadBitLengths(HuffmanOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);

        size_t NumSymbols = 0;
        uint16_t BitLengths[16];
        for (uint8_t Length = 0; Length < NumBitlengths; Length++) {
            BitLengths[Length] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsLeft, 8);
            NumSymbols += BitLengths[Length];
        }

        uint16_t *Symbols = calloc(NumSymbols, sizeof(uint16_t));

        // Ok, now we read NumSymbols from the file and put them where?
        for (size_t Symbol = 0; Symbol < NumSymbols; Symbol++) {
            Symbols[Symbol] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsLeft, 8);
            // Ok, so now we're done setting the HuffmanOptions, next we need to create the actual tree from the values set in the tree
        }
        Huffman_BuildTreeFromBitlengths(Options, BitLengths, NumBitlengths, Symbols, NumSymbols);
        free(Symbols);
    }

    void Huffman_BuildTreeFromBitlengths(HuffmanOptions *Options, uint16_t *BitLengths, size_t NumBitLengths, uint16_t *Symbols, size_t NumSymbols) {
        AssertIO(Options != NULL);
        AssertIO(BitLengths != NULL);
        AssertIO(Symbols != NULL);
        AssertIO(NumBitLengths > 0);
        AssertIO(NumSymbols > 0);

        size_t Entry  = 0;
        size_t Symbol = 1; // Actual value
        for (size_t BitLength = 0; BitLength < NumBitLengths; BitLength++) {
            for (size_t Length = 0; Length < BitLengths[BitLength]; Length++) {
                Options->Entries[Entry].Bitlength = BitLengths[BitLength] + 1;
                Options->Entries[Entry].Value     = (Symbol + 1) << ((BitLengths[BitLength] + 1) - BitLengths[BitLength]);
                Symbol += 1;
                Entry  += 1;
            }
        }
        Options->NumEntries = NumSymbols;
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
