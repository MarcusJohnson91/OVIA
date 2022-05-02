#include "../../include/CodecIO/JPEGCodec.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h" /* Included for Assertions */
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/TextIO/LogIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    void JPEG_Read_StartOfFrame(JPEGOptions *Options, BitBuffer *BitB, uint16_t SegmentSize) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->BitDepth                               = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);  // 9
        Options->Height                                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16); // 8, Height
        Options->Width                                  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16); // 8, Width
        Options->NumChannels                            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);  // 3, Components
        Options->Channels                               = calloc(Options->NumChannels, sizeof(JPEG_ChannelParameters));
        if (Options->Channels != NULL) {
            for (uint8_t Channel = 0; Channel < Options->NumChannels; Channel++) {
                Options->Channels[Channel].ChannelID    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8); // 1, 2, 3,
                Options->Channels[Channel].HorizontalSF = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 4); // 1, 1, 1,
                Options->Channels[Channel].VerticalSF   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 4); // 1, 1, 1,
            }
        }
        BitBuffer_Seek(BitB, Bytes2Bits(SegmentSize - ((Options->NumChannels * 3) + 8)));
    }
    
    static void JPEG_Huffman_CreateHUFFSIZETable(uint8_t BITS[16], uint8_t *HUFFVAL) { // Generate_size_table
        
    }
    
    static uint8_t *tjei_huff_get_code_lengths(uint8_t huffsize[/*256*/], uint8_t const *bits) {
        int k = 0;
        for (int i = 0; i < 16; ++i) {
            for (int j = 0; j < bits[i]; ++j) {
                huffsize[k++] = (uint8_t) (i + 1);
            }
            huffsize[k] = 0;
        }
        return huffsize;
    }
    
    static void JPEG_Huffman_CreateHUFFCODETable(uint8_t BITS[16], uint8_t *HUFFVAL) { // Generate_code_table
        
    }
    
    static void JPEG_Huffman_Sort(uint8_t BITS[16], uint8_t *HUFFVAL) { // Order_codes
        
    }
    
    // Huffman values are written in big endian order; most significant to least
    
    void JPEG_Read_DefineHuffmanTable(JPEGOptions *Options, BitBuffer *BitB, uint16_t SegmentSize) { // SegmentSize = 30
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->EntropyCoder                               = EntropyCoder_Huffman;
        Options->Huffman->Values                            = calloc(2, sizeof(HuffmanValue*));
        AssertIO(Options->Huffman->Values != NULL);
        while (SegmentSize > 0) {
            uint8_t TableType                        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 4); // 0
            Options->Huffman->TableID                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 4); // 0
                                                                                                                                           // TableType 0 = DC/Lossless, 1 = AC
            
            if (TableType == 0) {
                uint8_t BitLengths[16]; // 01 00 03 01 | 01 01 01 01 | [01] 01 00 00 | 00 00 00 00
                uint8_t NumSymbols                   = 0;
                
                // BitLengths aka BITS needs to be copied to the specified TableID
                
                for (uint8_t Length = 0; Length < 16; Length++) { // BITS
                    BitLengths[Length]               = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 8);
                    NumSymbols                      += BitLengths[Length];
                    SegmentSize                     -= 1;
                }
                
                Options->Huffman->NumValues[Options->Huffman->TableID]    = NumSymbols; // 12
                
                // Now we create the actual Huffman table from the BitLengths and their associated values called Vij in the Spec.
                /* HUFFVAL = {0, 7, 8, 9, 6, 5, 4, 3, 1, 2, 10}
                 [BitLength = 1] = {0},
                 [BitLength = 3] = {7, 8, 9},
                 [BitLength = 4] = {6},
                 [BitLength = 5] = {5},
                 [BitLength = 6] = {4},
                 [BitLength = 7] = {3},
                 [BitLength = 8] = {1},
                 [BitLength = 9] = {2},
                 [BitLength = 10] = {10},
                 */
                
                Options->Huffman->Values[Options->Huffman->TableID]       = calloc(NumSymbols, sizeof(HuffmanValue));
                if (Options->Huffman->Values[Options->Huffman->TableID] != NULL) {
                    /*
                     [0] = { BITS = 1E 00 01 00 | 03 01 01 01 | 01 01 01 01 | 00 00 00 00
                     [0] = BitLength = 1;
                     },
                     
                     
                     So, this is how Huffman decoding works.
                     
                     How do we Encode, and then package both together in EntropyIO so we can do both?
                     */
                    uint8_t  BitLength                                = 0;
                    uint16_t HuffCode                                 = 0;
                    while (BitLength < 16) {
                        if (BitLengths[BitLength] > 0) {
                            Options->Huffman->Values[Options->Huffman->TableID]->BitLength = BitLength;
                            Options->Huffman->Values[Options->Huffman->TableID]->HuffCode  = HuffCode;
                            HuffCode                                 += 1;
                            Options->Huffman->Values[Options->Huffman->TableID]->Symbol    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 8);
                            SegmentSize                              -= 1;
                        }
                        HuffCode                                    <<= 1; // Shift always
                        BitLength                                    += 1;
                    }
                } else if (TableType == 1) {
                    uint16_t Bytes2Skip = 0;
                    for (uint8_t i = 0; i < 16; i++) {
                        Bytes2Skip                                   += BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 8);
                    }
                    BitBuffer_Seek(BitB, Bytes2Bits(Bytes2Skip));
                    SegmentSize                                      -= Bytes2Skip;
                }
            }
        }
    }
    
    void JPEG_Read_DefineArithmeticTable(JPEGOptions *Options, BitBuffer *BitB, uint16_t SegmentSize) { // SegmentSize = LA
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->Arithmetic->TableType           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 4); // Tc
        Options->Arithmetic->TableID             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 4); // Tb
        
        Options->Arithmetic->CodeValue           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 8); // Cs
        /* For DC (and lossless) conditioning tables Tc shall be zero and Cs shall contain two 4-bit parameters, U and L. U and L shall be in the range 0 ≤ L ≤ U ≤ 15 and the value of Cs shall be L + 16 × U. */
        BitBuffer_Seek(BitB, Bytes2Bits(SegmentSize - 6));
    }
    
    void JPEG_Read_DefineRestartInterval(JPEGOptions *Options, BitBuffer *BitB, uint16_t SegmentSize) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->RestartInterval                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16);
        BitBuffer_Seek(BitB, Bytes2Bits(SegmentSize - 4));
    }
    
    void JPEG_Read_DefineNumberOfLines(JPEGOptions *Options, BitBuffer *BitB, uint16_t SegmentSize) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->Height                      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16);
        BitBuffer_Seek(BitB, Bytes2Bits(SegmentSize - 4));
    }
    
    void JPEG_Read_Comment(JPEGOptions *Options, BitBuffer *BitB, uint16_t SegmentSize) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->CommentSize                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 16);
        if (Options->CommentSize >= 3) {
            Options->Comment                 = calloc(Options->CommentSize - 2, sizeof(CharSet8));
            if (Options->Comment != NULL) {
                for (uint16_t Byte = 0; Byte < Options->CommentSize - 2; Byte++) {
                    Options->Comment[Byte]   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 8);
                }
            }
        }
        BitBuffer_Seek(BitB, Bytes2Bits(SegmentSize - Options->CommentSize));
    }
    
    void JPEG_Read_StartOfScan(JPEGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        uint8_t  NumChannels                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);  // 3
        JPEG_ChannelParameters *Channels       = calloc(NumChannels, sizeof(JPEG_ChannelParameters));
        AssertIO(Channels != NULL);
        for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
            Channels[Channel].ChannelID    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8); // 1, 2, 3,
            Channels[Channel].HorizontalSF = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 4); // 0, 1, 1,
            Channels[Channel].VerticalSF   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 4); // 0, 0, 0
        }
        Options->Predictor                        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8); // 1
        BitBuffer_Seek(BitB, 16); // 3 fields: Se, Ah, Al; 16 bits, should be zero, but otherwise have no meaning.
        /* SoS header parsing is over, now read the data */
        /* So, Markers can occur here, 0xFF is converted to 0xFF 0x00 by the encoder so undo that here */
        /* Image Data: F1 CF D1 3F 91 -47 bytes of just zeros- 0F */
        AssertIO(PlatformIO_Is(Options->EntropyCoder, EntropyCoder_Huffman));
        AssertIO(PlatformIO_Is(Options->EntropyCoder, CodingMode_Hierarchical));
    }
    
    void JPEG_Parse(JPEGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        uint16_t Marker            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 16);
        uint16_t SegmentSize       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 16);
        switch (Marker) {
            case JPEGMarker_StartOfFrameLossless1:
                Options->EntropyCoder = EntropyCoder_Huffman;
                Options->CodingMode   = CodingMode_NonHierarchical;
                JPEG_Read_StartOfFrame(Options, BitB, SegmentSize);
                break;
            case JPEGMarker_StartOfFrameLossless2:
                Options->EntropyCoder = EntropyCoder_Huffman;
                Options->CodingMode   = CodingMode_Hierarchical;
                JPEG_Read_StartOfFrame(Options, BitB, SegmentSize);
                break;
            case JPEGMarker_StartOfFrameLossless3:
                Options->EntropyCoder = EntropyCoder_Arithmetic;
                Options->CodingMode   = CodingMode_NonHierarchical;
                JPEG_Read_StartOfFrame(Options, BitB, SegmentSize);
                break;
            case JPEGMarker_StartOfFrameLossless4:
                Options->EntropyCoder = EntropyCoder_Arithmetic;
                Options->CodingMode   = CodingMode_Hierarchical;
                JPEG_Read_StartOfFrame(Options, BitB, SegmentSize);
                break;
            case JPEGMarker_DefineHuffmanTable:
                JPEG_Read_DefineHuffmanTable(Options, BitB, SegmentSize);
                break;
            case JPEGMarker_DefineArthimeticTable:
                JPEG_Read_DefineArithmeticTable(Options, BitB, SegmentSize);
                break;
            case JPEGMarker_StartOfScan:
                JPEG_Read_StartOfScan(Options, BitB);
                break;
            case JPEGMarker_NumberOfLines:
                JPEG_Read_DefineNumberOfLines(Options, BitB, SegmentSize);
                break;
            case JPEGMarker_RestartInterval:
                JPEG_Read_DefineRestartInterval(Options, BitB, SegmentSize);
            case JPEGMarker_Restart0:
            case JPEGMarker_Restart1:
            case JPEGMarker_Restart2:
            case JPEGMarker_Restart3:
            case JPEGMarker_Restart4:
            case JPEGMarker_Restart5:
            case JPEGMarker_Restart6:
            case JPEGMarker_Restart7:
                JPEG_Read_DefineRestartInterval(Options, BitB, SegmentSize);
            default:
                Log(Severity_WARNING, PlatformIO_FunctionName, UTF8String("Marker 0x%X is unsupported"), Marker);
                BitBuffer_Seek(BitB, Bytes2Bits(SegmentSize - 2));
                break;
        }
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
