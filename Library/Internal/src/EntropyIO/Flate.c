#include "../../../Dependencies/libPCM/Dependencies/BitIO/libBitIO/include/BitIOMath.h"
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
    
    enum ModernPNGCRC32Constants {
        ChunkIDSize         = 4,
    };
    
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
        struct HuffmanNode *LeftNode;
        struct HuffmanNode *RightNode;
        uint64_t            Symbol;
    };
    
    struct HuffmanTree {
        HuffmanNode       *Node;
        uint64_t          *SymbolFrequency;
        uint64_t           NumNodes;
        const uint8_t     *Table;
        bool               TableIsUsable;
    };
    
    
    typedef struct LZ77Tuple   LZ77Tuple;
    typedef struct HuffmanNode HuffmanNode;
    typedef struct HuffmanTree HuffmanTree;
    
    void DecodeDEFLATE(BitBuffer *EncodedData, BitBuffer *DecodedData) {
        
    }
    
    uint32_t GenerateCRC32(BitBuffer *BitB, const uint64_t ChunkSize) {
        uint64_t CRC32Polynomial = 0x104C11DB7;
        uint32_t CRC32PolyInit   = 0xFFFFFFFF;
        uint8_t  CurrentBit      = 0;
        bool     Reflect         = No;
        bool     Reverse         = No;
        /*
        for (uint64_t Bit = BitOffset; Bit < BitOffset + BitLength; Bit++) {
            // Now we need to select a bit from the buffer
            CurrentBit = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            
        }
         */
        /*
         Ok, so we already know the layout of the data hasn't been changed, and it's faster to iterate over the BitBuffer than copy variables back and forth.
         We also know the polynomial, size of the polynomial, and the init for the Poly, these are all constants.
         So, all we really have to do is Init the poly, and feed bits through the algorithm.
         Thanks to the PNG algorithm, all of our offsets should be at byte boundaries, so we should be able to loop over bytes.
         */
        
        return 0ULL;
    }
    
    bool VerifyCRC32(BitBuffer *BitB, uint64_t ChunkSize) {
        bool CRCsMatch = No;
        // Lets wrap Data2CRC in a BitBuffer
        
        uint32_t CalculatedCRC = 0UL;
        
        for (uint64_t Bit = 0; Bit < (uint64_t) Bytes2Bits(ChunkSize + ChunkIDSize); Bit++) {
            /*
             So, we read a bit, what do we do with it after that?
             Well, we need to build a XOR gate based on the CRC32 polynomial, x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x+1 aka 0x104C11DB7
             The highest bit is always 0, so the XOR'd value is always 1, so lets go ahead and remove that from the Polynomial.
             Polynomial: 0x4C11DB7
             */
            
        }
        uint32_t EmbeddedCRC = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
        if (CRCsMatch == EmbeddedCRC) {
            CRCsMatch  = Yes;
        }
        return CRCsMatch;
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
    
    bool VerifyAdler32(const uint8_t *Data, const uint64_t DataSize, const uint32_t EmbeddedAdler32) {
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
