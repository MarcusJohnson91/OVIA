#include "../../include/Private/Common/ModernFLACEntropyCoders.h"

#ifdef __cplusplus
extern "C" {
#endif 
    
    static const uint8_t MD5_STable[4][16] = {
        {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22},
        {5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20},
        {4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23},
        {6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21}
    };
    
    static const uint32_t MD5Radian[4][16] = {
        {
            3614090360, 3905402710,  606105819, 3250441966,
            4118548399, 1200080426, 2821735955, 4249261313,
            1770035416, 2336552879, 4294925233, 2304563134,
            1804603682, 4254626195, 2792965006, 1236535329
        },
        {
            4129170786, 3225465664,  643717713, 3921069994,
            3593408605,   38016083, 3634488961, 3889429448,
            568446438,  3275163606, 4107603335, 1163531501,
            2850285829, 4243563512, 1735328473, 2368359562
        },
        {
            4294588738, 2272392833, 1839030562, 4259657740,
            2763975236, 1272893353, 4139469664, 3200236656,
            681279174, 3936430074, 3572445317,   76029189,
            3654602809, 3873151461,  530742520, 3299628645
        },
        {
            4096336452, 1126891415, 2878612391, 4237533241,
            1700485571, 2399980690, 4293915773, 2240044497,
            1873313359, 4264355552, 2734768916, 1309151649,
            4149444226, 3174756917,  718787259, 3951481745
        }
    };
    
    // TODO: For MD5 I'm just going to do it a whole file at a time, not block by block.
    // The would introduce the possibility of padding data changing it, or having to have yet another buffer; it's just not worth the effort.
    uint32_t Stage1(int A, int B, int C, int D, int E, int F, uint32_t G) { // MD5_FF
        A += (D + E + G) ^ (B & (C ^ (D + E + G)));
        A  = ((A << F) | (A >> (32 - F)));
        A += B;
        return A;
    }
    
    uint32_t Stage2(int A, int B, int C, int D, int E, int F, uint32_t G) { // MD5_GG
        A += C ^ ((D + E + G) & (B ^ C));
        A  = ((A << F) | (A >> (32 - F)));
        A += B;
        return A;
    }
    
    uint32_t Stage3(int A, int B, int C, int D, int E, int F, uint32_t G) { // MD5_HH
        A += B ^ (C ^ (D + E + G));
        A  = ((A << F) | (A >> (32 - F)));
        A += B;
        return A;
    }
    
    uint32_t Stage4(int A, int B, int C, int D, int E, int F, uint32_t G) { // MD5_II, eye NOT ell
        A += C ^ (B | ~(D + E + G));
        A  = ((A << F) | (A >> (32 - F)));
        A += B;
        return A;
    }
    
    uint8_t *CreateBlocks(uint8_t *Buffer, size_t BufferSize) {
        uint8_t PreBlock[64];
        if (BufferSize % 64 != 0 && BufferSize > 64) { // basically break the data up into 512 bit blocks, if there's not enough data for that, then pad.
            
        }
        
        for (size_t Byte = 0; Byte < BufferSize; Byte++) {
            PreBlock[Byte] = Buffer[Byte]; // This is a stupid way to do it, you're just wasting CPU cycles.
        }
        
        // How do I go through each block
        
        return 0;
    }
    
    char *GenerateMD5(uint8_t *Buffer, size_t BufferSize, bool PadData, char *MD5String) { // This calls UpdateMD5Page for all 64 byte pages
        uint64_t CurrentByte      = 0;
        uint32_t MiniBlock        = 0;
        uint32_t SwappedMiniBlock = 0;
        uint32_t Init1            = 0x01234567;
        uint32_t Init2            = 0x89ABCDEF;
        uint32_t Init3            = 0xFEDCBA98;
        uint32_t Init4            = 0x76543210;
        
        uint32_t X[16]            = {0}; // X becomes a 32 bit chunk of the data to process.
        
        
        if (BufferSize < 448) { // Pad this block
            uint8_t PaddingBits = Bytes2Bits(BufferSize % 448);
            // Insert padding data at the end of the array
            
            // Next we need to go through and cut out the audio, and then pad it.
        }
        
        // Just run the algorithm
        
        // then we need to break the array up into 32 bit blocks, swap them, and feed them into the MD5 loop.
        // We need 4 sub functions, that take X,Y,Z as input and output F, G, H, & I.
        // Init the MD5 variables in here, seperate functions aren't for constants lol.
        
        for (uint64_t Byte = CurrentByte; Byte < BufferSize; Byte++) {
            for (uint8_t BlockPiece = 0; BlockPiece < 4; BlockPiece++) {
                MiniBlock <<= 8;
                MiniBlock  += Buffer[Byte];
            }
        }
        
        // Break the data up into 64 byte blocks, I assume if the end block is smaller you need to pad it with 0s.
        // The bits are read from lsb to msb
        
        // Time to break the Buffer into 32 bit words, LSB first aka swap endian
        
        
        SwappedMiniBlock = SwapEndian32(MiniBlock); // Block is ready to be consumed by the hashing algorithm
        uint32_t Result  = 0;
        
        for (uint8_t Byte = 0; Byte < 16; Byte++) { // FF
            for (uint8_t Round = 0; Round < 4; Round++) {
                Result  = Stage1(Init1, Init2, Init3, Init4, X[Byte], MD5_STable[0][Round], MD5Radian[0][Byte]);
                Result += Stage2(Init1, Init2, Init3, Init4, X[Byte], MD5_STable[1][Round], MD5Radian[1][Byte]);
                Result += Stage3(Init1, Init2, Init3, Init4, X[Byte], MD5_STable[2][Round], MD5Radian[2][Byte]);
                Result += Stage4(Init1, Init2, Init3, Init4, X[Byte], MD5_STable[3][Round], MD5Radian[3][Byte]);
            }
        }
        
        // The message aka buffer needs to be a multiple of 448 bits in length.
        // When a message needs to be padded, the first bit of padding is a 1, all the rest in the 448 bit block are 0s.
        // Then append the number of padding bits added to the end of the padding, which must take up 64 bits, but is written as 2 32 bit ints, least significant word first (so swap the 64 bit int, then extract the first 4 bytes)
        
        /* IDK what this means
         At this point the resulting message (after padding with bits and with
         b) has a length that is an exact multiple of 512 bits. Equivalently,
         this message has a length that is an exact multiple of 16 (32-bit)
         words. Let M[0 ... N-1] denote the words of the resulting message,
         where N is a multiple of 16.
         */
        
        // Init the MD5 with 4 32 bit words
        // InitA = 0x01234567
        // InitB = 0x89abcdef
        // InitC = 0xfedcba98
        // InitD = 0x76543210
        
        // Process message in 512 bit blocks.
        return MD5String;
    }
    
    bool VerifyMD5(uint8_t *Buffer, size_t BufferSize, bool PadData, char *MD5String, char *PrecomputedMD5) {
        char *GeneratedMD5;
        GeneratedMD5 = GenerateMD5(Buffer, BufferSize, PadData, MD5String);
        MD5String = GeneratedMD5;
        // Now verify that GeneratedMD5 matches PrecomputedMD5
        return false;
    }
    
#ifdef __cplusplus
}
#endif
