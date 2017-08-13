#include "../../include/Private/Common/libModernPNGFlate.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    struct HuffmanNode {
        int64_t            LeftHuffmanCode;
        int64_t            RightHuffmanCode;
        int64_t           *LeftNode;
        int64_t           *RightNode;
    };
    
    struct HuffmanTree {
        uint64_t           NumNodes;
        HuffmanNode       *Node;
        uint64_t          *SymbolFrequency;
        bool               TableIsUsable;
        const uint8_t     *Table;
    };
    
    /* Deflate (encode deflate) */
    void ParseDeflateHeader(const BitBuffer *DeflatedData) {
        if (DeflatedData == NULL) {
            Log(LOG_ERR, "libBitIO", "ParseDeflateHeader", "BitBuffer pointer is NULL");
        } else {
            // stored in big endian byte order, bits are stored LSB first
            uint8_t CompressionMethod  = ReadBits(DeflatedData, 4, true); // 8 = DEFLATE
            uint8_t CompressionInfo    = ReadBits(DeflatedData, 4, true); // 7 = LZ77 window size 32k
            uint8_t CheckCode          = ReadBits(DeflatedData, 5, true); // 1, for the previous 2 fields, MUST be multiple of 31
            bool    DictionaryPresent  = ReadBits(DeflatedData, 1, true); //
            uint8_t CompressionLevel   = ReadBits(DeflatedData, 2, true); // 0
            if (DictionaryPresent == true) {
                uint32_t DictionaryID  = ReadBits(DeflatedData, 32, true); // 0xEDC1
            }
            if (CompressionMethod == 8) {
                //ParseDeflateBlock(DeflatedData, BlockSize[CompressionInfo]);
            } else {
                Log(LOG_ERR, "libBitIO", "ParseDeflateHeader", "Invalid DEFLATE compression method %d", CompressionMethod);
            }
        }
    }
    
    void DecodeHuffman(const BitBuffer *BitB, uint64_t HuffmanSize) {
        // 3 alphabets, literal, "alphabet of bytes", or <length 8, distance 15> the first 2 are combined, 0-255 = literal, 256 = End of Block, 257-285 = length
        // FIXME: The Tilde ~ symbol is the negation symbol in C!!!!! XOR = ^
        
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "DecodeHuffman", "BitBuffer pointer is NULL");
        } else {
            uint8_t  DecodedData[32768]     = {0};
            bool     IsLastHuffmanBlock     = ReadBits(BitB, 1, true);
            uint8_t  HuffmanCompressionType = ReadBits(BitB, 2, true); // 0 = none, 1 = fixed, 2 = dynamic, 3 = invalid.
            uint32_t DataLength             = 0;
            uint32_t OnesComplimentOfLength = 0; // Ones Compliment of DataLength
            
            if (OnesCompliment2TwosCompliment(OnesComplimentOfLength) != HuffmanSize) { // Make sure the numbers match up
                Log(LOG_WARNING, "libBitIO", "DecodeHuffman", "One's Compliment of Length: %d != Length %d", OnesComplimentOfLength, DataLength);
            }
            
            if (IsLastHuffmanBlock == true) {
                
            }
            
            if (HuffmanCompressionType == 0) { // No compression.
                AlignBitBuffer(BitB, 1); // Skip the rest of the current byte
                DataLength             = ReadBits(BitB, 32, true);
                OnesComplimentOfLength = ReadBits(BitB, 32, true);
                if (OnesCompliment2TwosCompliment(OnesComplimentOfLength) != DataLength) {
                    // Exit because there's an issue.
                }
                for (uint32_t Byte = 0; Byte < DataLength; Byte++) {
                    DecodedData[Byte] = ReadBits(BitB, 8, true);
                }
            } else if (HuffmanCompressionType == 1) { // Static Huffman.
                uint8_t  Length   = ReadBits(BitB, 8, true) - 254;
                uint16_t Distance = ReadBits(BitB, 5, true);
                
            } else if (HuffmanCompressionType == 2) { // Dynamic Huffman.
                /*
                 Huff->Dynamic->Length     = ReadBits(BitI, 5) + 257;
                 Huff->Dynamic->Distance   = ReadBits(BitI, 5) + 1;
                 Huff->Dynamic->CodeLength = ReadBits(BitI, 4) + 4;
                 */
            } else { // Invalid.
                     // Reject the stream.
            }
            /*
             if compressed with dynamic Huffman codes
             read representation of code trees (see
             subsection below)
             loop (until end of block code recognized)
             decode literal/length value from input stream
             if value < 256
             copy value (literal byte) to output stream
             otherwise
             if value = end of block (256)
             break from loop
             otherwise (value = 257..285)
             decode distance from input stream
             
             move backwards distance bytes in the output
             stream, and copy length bytes from this
             position to the output stream.
             end loop
             while not last block
             /
             }
             */
        }
    }
    
    uint64_t GetHuffmanCode(HuffmanTree *Tree, int64_t **SymbolsAndProbabilities, int64_t Symbol, uint64_t NumSymbols) {
        if (Tree == NULL) {
            Log(LOG_ERR, "libBitIO", "GetHuffmanCode", "Pointer to HuffmanTree is NULL");
        } else if (SymbolsAndProbabilities == NULL) {
            Log(LOG_ERR, "libBitIO", "GetHuffmanCode", "Pointer to SymbolsAndProbabilities is NULL");
        } else {
            uint64_t SymbolPlace = 0;
            for (uint64_t Index = 0; Index < NumSymbols; Index++) {
                // Find the probability of Symbol, then build the actual huffman code from where in the tree that symbol was assigned
                
                // No, then you'd have to do 2 lookups per symbol instead of just one.
                // So, we should just take in a Symbol to find, and the tree to find it in
                // But, to do that, we need to store the symbol with the probability, also we should think about building a table from this tree and hitting that first?
                
                // No, we should simply do a search through all the nodes, and when we find the symbol, create the Huffman code.
                // Also, tree traversal for each fucking code will be slow should we just store the code alongside the symbol to speed up Decoding/Encoding?
                
                // Honestly, you have to build a table to embed in the PNG anyway, so I might as well store the table in the tree pointer.
                // Now, I just need to write a function to build a tree from the data, and include a bool so it knows when to use the tree.
                
                
                
                
                
                // Search through the nodes by finding the probability of the submitted symbol, then go down the tree tracing that step.
                // Lets say E is the symbol we're supposed to find, which is the fifth highest probability overall.
                // We go to the root node, which has a 0. then the right since we're in position 1, not 5.
                // the right branch of the root node has value 1, we go to the right node again since we're only as position 2.
                // the code so far is 01, we're at the second right node, and we need to go right again. the code is now 011
                // we're at position 4, so we need to go down 1 more node. the code is currently 0111
                // since we're at position 5, we need to go left? the code is now 01110
                // the Huffman code for the symbol E, is 0b01110, or 0xE (LOL HOLY SHIT WHA	T A COINCIDENCE!)
            }
        }
        return 0;
    }
    
    HuffmanTree *BuildHuffmanTree(HuffmanTree *Tree2Build, int64_t **SymbolsAndProbabilities, uint64_t NumSymbols) {
        if (Tree2Build == NULL) {
            Log(LOG_ERR, "libBitIO", "BuildHuffmanTree", "Pointer to HuffmanTree is NULL");
        } else if (SymbolsAndProbabilities == NULL) {
            Log(LOG_ERR, "libBitIO", "BuildHuffmanTree", "Pointer to SymbolsAndProbabilities is NULL");
        } else {
            // Well start by taking the lowest probability pair (the bottom 2 regardless of value) symbols, and assigning them to nodes.
            Tree2Build->NumNodes = NumSymbols / 2;
            
            for (uint64_t Symbol = NumSymbols; Symbol > 0; Symbol -= 2) { // What do we do if the number of symbols is uneven?
                Tree2Build->Node[0].LeftNode  = SymbolsAndProbabilities[Symbol]; // FIXME: Not sure if "SymbolPair - 1" is gonna work?
                Tree2Build->Node[0].RightNode = SymbolsAndProbabilities[Symbol + 1];
            }
        }
        return Tree2Build; // Is this seriously it?
                           // Make sure to remove any codes that do not occur (frequency = 0)
                           // When multiple values have the same frequency sort by intensity.
    }
    
    void ParseDeflateBlock(const BitBuffer *BitB, uint16_t BlockSize) {
        if (BitB == NULL) {
            Log(LOG_ERR, "libBitIO", "ParseDeflateBlock", "Pointer to BitBuffer is NULL");
        } else {
            bool    IsLastBlock    = ReadBits(BitB, 1, true); // no
            uint8_t EncodingMethod = ReadBits(BitB, 2, true); // 3
            
            switch (EncodingMethod) {
                case 0:
                    // Stored.
                    //DecodeStoredHuffman(BitB);
                    break;
                case 1:
                    // Static Huffman + preagreed table
                    //DecodeStaticHuffman(BitB);
                    break;
                case 2:
                    // Dynamic Huffman
                    //DecodeDynamicHuffman(BitB);
                    break;
                default:
                    Log(LOG_EMERG, "libBitIO", "ParseDeflateBlock", "Invalid Deflate encoding method: %d", EncodingMethod);
                    break;
            }
        }
    }
    
    void EncodeLZ77(const BitBuffer *RawBuffer, const BitBuffer *LZ77Buffer, const uint64_t WindowSize, const uint64_t DistanceLength, const uint64_t SymbolSize) {
        // The dictionary is simply the current buffer, at the current buffer position -(WindowSize / 2) +(WindowSize / 2)
        // So, the first thing you write is the distance from the cursor to the previous string.
        // Then you write the length of the largest match you can find.
        // Then write the next byte in the stream.
        // Then move the cursor the length of the longest match + 1
        // When we're at the start of the match, simply record the byte and the length of the match (1).
        
        
        // We need to have a BitBuffer to read from, and a BitBuffer to write to.
        if (RawBuffer == NULL) {
            Log(LOG_ERR, "libBitIO", "EncodeLZ77", "The pointer to the raw buffer is NULL");
        } else if (LZ77Buffer == NULL) {
            Log(LOG_ERR, "libBitIO", "EncodeLZ77", "The pointer to the LZ77 buffer is NULL");
        } else {
            uint64_t WindowBits   = NumBits2ReadSymbols(WindowSize);
            uint64_t DistanceBits = NumBits2ReadSymbols(DistanceLength);
            
            uint64_t CurrentSymbol = 0;
            uint64_t PreviousSymbol = 0;
            
            while (GetBitBufferPosition(RawBuffer) != GetBitBufferSize(RawBuffer)) {
                CurrentSymbol = ReadBits(RawBuffer, SymbolSize, true);
                if (CurrentSymbol == PreviousSymbol) {
                    // find the largest string of symbols that matches the current one
                }
            }
            
            
            
            /* ------------OLD SHIT----------------
             
             // Now, we just need to read the RawBuffer (which must contain ALL the data to be encoded) with ReadBits(SymbolSize)
             // then start writing LZ77Buffer with our discoveries.
             // So, loop over RawBuffer, if RawByte == 0, just code the longest string you can, or the first 3 bytes (if they're all different)
             for (uint64_t RawByte = 0; RawByte < RawBuffer->BitsUnavailable; RawByte++) {
             if (RawByte == 0) {
             if (RawBuffer->Buffer[RawByte] == RawBuffer->Buffer[RawByte + 1] || RawBuffer->Buffer[RawByte + 1] == RawBuffer->Buffer[RawByte + 2]) {
             
             }
             }
             }
             */
        }
    }
    
#ifdef __cplusplus
}
#endif
