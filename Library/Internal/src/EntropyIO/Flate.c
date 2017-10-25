#include "../../include/Private/Common/libModernPNG_EntropyCoders.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    // LZ77, Lazy parsing is most efficent.
    
    // A Huffman tree has 288 nodes. 0-287
    
    /*
     DEFLATE details:
     Uncompressed blocks are limited to 65535 bytes max.
     The Huffman tree for each block is independent.
     LZ77's window size is up to 32768 bytes previously, regardless of huffman block.
     There are 2 huffman trees per block; The first describes the representation of the data, and the second is for the data.
     The Huffman compressed data is composed of 2 types, literal byte strings, and LZ77 <length,backward distance> tuples, called pointers in RFC1951.
     DEFLATE limits the distance to 32,768 bytes back, and lengths to 258 bytes; but DOES NOT limit the size of a compressed block.
     There are 2 huffman trees, the first tree stores just literal values and lengths, and another for the distance.
     LZ77 uses LSBit first, Huffman codes use MSBit first.
     
     Huffman Tree:
     0 is always on the left, 1 is always on the right.
     Non-Leaf nodes are always 0, leaf nodes are always 1. so the code ends with a 1.
     */
    
    enum DEFLATEConstants {
        HuffmanMaxLiteral   = 65535,
        LZ77MaxDistance     = 32768,
        LZ77MaxLenth        = 258,
    };
    
    struct LZ77Tuple {
        uint16_t            Distance;
        uint16_t            Length;
    };
    
    struct HuffmanNode {
        uint64_t            Symbol;
        struct HuffmanNode *LeftNode;
        struct HuffmanNode *RightNode;
    };
    
    struct HuffmanTree {
        uint64_t           NumNodes;
        HuffmanNode       *Node;
        uint64_t          *SymbolFrequency;
        bool               TableIsUsable;
        const uint8_t     *Table;
    };
    
    
    typedef struct LZ77Tuple   LZ77Tuple;
    typedef struct HuffmanNode HuffmanNode;
    typedef struct HuffmanTree HuffmanTree;
    
    void DecodeLZ77Tuple(BitBuffer *LZ77EncodedData, BitBuffer *DecodedData) {
        
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    /* Deflate (encode deflate) */
    void ParseDeflateHeader(const BitBuffer *DeflatedData) {
        if (DeflatedData == NULL) {
            Log(LOG_ERR, "libBitIO", "ParseDeflateHeader", "BitBuffer pointer is NULL");
        } else {
            // stored in big endian byte order, bits are stored LSB first
            uint8_t CompressionMethod  = ReadBits(BitIOMSByte, BitIOLSBit, DeflatedData, 4);
            uint8_t CompressionInfo    = ReadBits(BitIOMSByte, BitIOLSBit, DeflatedData, 4); // 7 = LZ77 window size 32k
            uint8_t CheckCode          = ReadBits(BitIOMSByte, BitIOLSBit, DeflatedData, 5); // 1, for the previous 2 fields, MUST be multiple of 31
            bool    DictionaryPresent  = ReadBits(BitIOMSByte, BitIOLSBit, DeflatedData, 1);
            uint8_t CompressionLevel   = ReadBits(BitIOMSByte, BitIOLSBit, DeflatedData, 2);
            if (DictionaryPresent == true) {
                uint32_t DictionaryID  = ReadBits(BitIOMSByte, BitIOLSBit, DeflatedData, 32); // 0xEDC1
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
            bool     IsLastHuffmanBlock     = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            uint8_t  HuffmanCompressionType = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 2); // 0 = none, 1 = fixed, 2 = dynamic, 3 = invalid.
            uint32_t DataLength             = 0;
            uint32_t OnesComplimentOfLength = 0; // Ones Compliment of DataLength
            
            if (OnesCompliment2TwosCompliment(OnesComplimentOfLength) != HuffmanSize) { // Make sure the numbers match up
                Log(LOG_WARNING, "libBitIO", "DecodeHuffman", "One's Compliment of Length: %d != Length %d", OnesComplimentOfLength, DataLength);
            }
            
            if (IsLastHuffmanBlock == true) {
                
            }
            
            if (HuffmanCompressionType == 0) { // No compression.
                AlignBitBuffer(BitB, 1); // Skip the rest of the current byte
                DataLength             = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32);
                OnesComplimentOfLength = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32);
                if (OnesCompliment2TwosCompliment(OnesComplimentOfLength) != DataLength) {
                    // Exit because there's an issue.
                }
                for (uint32_t Byte = 0; Byte < DataLength; Byte++) {
                    DecodedData[Byte] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
                }
            } else if (HuffmanCompressionType == 1) { // Static Huffman.
                uint8_t  Length   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8) - 254;
                uint16_t Distance = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 5);
                
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
            bool    IsLastBlock    = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1); // no
            uint8_t EncodingMethod = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 2); // 3
            
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
                CurrentSymbol = ReadBits(BitIOMSByte, BitIOLSBit, RawBuffer, SymbolSize);
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
    
    uint32_t GenerateCRC32(BitBuffer *BitB, const uint64_t BitOffset, const uint64_t BitLength) {
        uint64_t CRC32Polynomial = 0x104C11DB7;
        uint32_t CRC32PolyInit   =  0xFFFFFFFF;
        uint8_t  CurrentBit      = 0;
        for (uint64_t Bit = BitOffset; Bit < BitOffset + BitLength; Bit++) {
            // Now we need to select a bit from the buffer
            CurrentBit = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            
        }
        /*
         Ok, so we already know the layout of the data hasn't been changed, and it's faster to iterate over the BitBuffer than copy variables back and forth.
         We also know the polynomial, size of the polynomial, and the init for the Poly, these are all constants.
         So, all we really have to do is Init the poly, and feed bits through the algorithm.
         Thanks to the PNG algorithm, all of our offsets should be at byte boundaries, so we should be able to loop over bytes.
         */
        
        return 0ULL;
    }
    
    uint64_t GenerateCRC(BitBuffer *Data2CRC, const uint64_t BitOffset, const uint64_t BitLength, const uint64_t Polynomial, const uint8_t PolySize, const uint64_t PolyInit) {
        /*
        if (PolySize % 8 != 0) {
            // Ok, so we also need to add the ability to do incremental CRC generation for the iDAT/fDAT chunks in PNG
            
            
            // You have to do it bitwise
        } else if (DataSize % PolySize || DataSize > PolySize) {
            // do it word wise aka grab PolySize bits from Data2CRC at once
        } else {
            // Do it byte-wise
        }
        
        uint16_t CRCResult = 0;
        for (uint64_t Byte = 0; Byte < DataSize; Byte++) {
            CRCResult = ReciprocalPoly ^ Data2CRC[Byte] << 8;
            for (uint8_t Bit = 0; Bit < 8; Bit++) {
                if ((CRCResult & 0x8000) == true) {
                } else {
                    CRCResult <<= 1;
                }
            }
        }
         */
        return 0ULL;
    }
    
    bool VerifyCRC(BitBuffer *Data2CRC, const uint64_t BitOffeset, const uint64_t BitLength, const uint64_t Polynomial, const uint8_t PolySize, const uint64_t PolyInit, const uint64_t PrecomputedCRC) {
        /*
         So, the CRC generator need to stream input, which means it needs to be aligned to the CRC size, so 16 bit alignment for CRC16, 32 for CRC32, etc.
         The real probelem is in the CRC function being called multiple times, or it handling it's own input and output itself...
         Out of those 2 choices, it being called multiple times is a simplier option.
         So, if that's the case, we should have a Previous CRC argument, and a bool saying if this has been repeated before?
         */
        return false;
    }
    
    uint32_t GenerateAdler32(const uint8_t *Data, const uint64_t DataSize) {
        uint32_t Adler  = 1;
        uint32_t Sum1   = Adler & 0xFFFF;
        uint32_t Sum2   = (Adler >> 16) & 0xFFFF;
        
        for (uint64_t Byte = 0; Byte < DataSize; Byte++) {
            Sum1 += Data[Byte] % 65521;
            Sum2 += Sum1 % 65521;
        }
        return (Sum2 << 16) + Sum1;
    }
    
    bool     VerifyAdler32(const uint8_t *Data, const uint64_t DataSize, const uint32_t EmbeddedAdler32) {
        uint32_t GeneratedAdler32 = GenerateAdler32(Data, DataSize);
        if (GeneratedAdler32 != EmbeddedAdler32) {
            return false;
        } else {
            return true;
        }
    }
    
#ifdef __cplusplus
}
#endif
