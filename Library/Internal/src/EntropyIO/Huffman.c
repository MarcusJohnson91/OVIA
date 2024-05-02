#include "../../include/EntropyIO/Huffman.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/ArrayIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef struct HuffmanNode {
        uint16_t Value; // Raw value the Code represents
        uint16_t Code;
        uint8_t  CodeLengthInBits;
    } HuffmanNode;
        
    typedef struct HuffmanTree {
        size_t       NumNodes;
        HuffmanNode *Nodes;
    } HuffmanTree;

    HuffmanTree *HuffmanTree_Init(size_t NumSymbols, size_t MaxBitLength) {
        HuffmanTree *Options = calloc(1, sizeof(HuffmanTree));
        AssertIO(Options != NULL);
        Options->Nodes = calloc(NumSymbols, sizeof(HuffmanNode));
        AssertIO(Options->Nodes != NULL);
        Options->NumNodes = NumSymbols;
        return Options;
    }

    /*
     So, Huffman can have a variable number of elements in the table, JPEG = 162, Deflate = 288

     So, when we create the Huffman table we need to be able to say the number of symbols it will contain

     Next, we need to be able to read the bitlengths from the JPEG file and create our tree; PNG needs to be able to generate default tables which should be a PNG specific thing
     */

    void Huffman_ReadBitLengths(HuffmanTree *Options, BitBuffer *BitB, size_t NumBitLengths) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);

        size_t NumSymbols = 0;
        uint16_t BitLengths[NumBitLengths];
        for (uint8_t Length = 0; Length < NumBitLengths; Length++) {
            BitLengths[Length] = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Left2Right, 8);
            NumSymbols += BitLengths[Length];
        }

        uint16_t *Symbols = calloc(NumSymbols, sizeof(uint16_t));

        // Ok, now we read NumSymbols from the file and put them where?
        for (size_t Symbol = 0; Symbol < NumSymbols; Symbol++) {
            Symbols[Symbol] = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Left2Right, 8);
            // Ok, so now we're done setting the HuffmanOptions, next we need to create the actual tree from the values set in the tree
        }
        Huffman_BuildTree_Bitlengths(Options, BitLengths, NumBitLengths, Symbols, NumSymbols);
        free(Symbols);
    } 

    void Huffman_BuildTree_Bitlengths(HuffmanTree *Options, uint16_t *BitLengths, size_t NumBitLengths, uint16_t *Symbols, size_t NumSymbols) {
        AssertIO(Options != NULL);
        AssertIO(BitLengths != NULL);
        AssertIO(Symbols != NULL);
        AssertIO(NumBitLengths > 0);
        AssertIO(NumSymbols > 0);

        size_t Entry  = 0;
        size_t Symbol = 1; // Actual value
        /*
         BitLength: 0 1 0 2 | 2 3 1 1 | 1 1 0 0 | 0 0 0 0 = 12

         Symbols: 00 00 0A 0B | 08 09 05 06 | 07 04 03 02 | 01
         */
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

    void Huffman_BuildTree_Histogram(HuffmanTree *Options, ArrayIO_Frequencies *Frequencies) {
        AssertIO(Options != NULL);
        AssertIO(Frequencies != NULL);
        AssertIO(Options->NumNodes >= Frequencies->NumEntries);
        /*
         Code Assignment:

         Loop from the least frequent to the most frequent; Assigning codes from 
         */



        /*
         Make sure that the frequencies are sorted, for now just sort them again
         then we need to take the sorted frequencies and assign the symbols to them as efficently as possible,
         half of the tree needs to be empty so we can prefix longer codes

         if we have say 4 codes to write, we need to leave 1 1bit code as a prefix, 1 2bit code as a prefix

         NumBitsNeeded = Ciel(log2(NumEntries))
         */
        size_t BitDepth = Logarithm(2, Frequencies->NumEntries);
        /*
         Frequencies: 6 Entries
         [0] = 245
         [1] = 238
         [2] = 188
         [3] = 177
         [4] = 92
         [5] = 29

         BitLength = 3
         Values = {1, 2, 3, 4, 5, 6}
         Symbols = {0b1, 0b10, 0b11, 0b100, 0b101, 0b110}

         ok but these symbols can not be the prefix for any other code
         */

        /*
         A histogram shouldn't nessessarily by tighly connected with a raw image or audio container...

         Maybe we should just create ArrayIO that ceates a histogram from various arrays so we can hoist this into it's own thing?
         */
    }

    void Huffman_Deinit(HuffmanTree *Options) {
        free(Options->Nodes);
        free(Options);
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
