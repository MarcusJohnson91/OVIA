#include "../../include/Private/JPEGCommon.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    /*
     
     Huffman Decoding:
     
     
     The DECODE procedure decodes an 8-bit value which, for the DC coefficient, determines the difference magnitude category. For the AC coefficient this 8-bit value determines the zero run length and non-zero coefficient category.
     Three tables, HUFFVAL, HUFFCODE, and HUFFSIZE, have been defined in Annex C. This particular implementation of DECODE makes use of the ordering of the Huffman codes in HUFFCODE according to both value and code size. Many other implementations of DECODE are possible.
     NOTE – The values in HUFFVAL are assigned to each code in HUFFCODE and HUFFSIZE in sequence. There are no ordering requirements for the values in HUFFVAL which have assigned codes of the same length.
     The implementation of DECODE described in this subclause uses three tables, MINCODE, MAXCODE and VALPTR, to decode a pointer to the HUFFVAL table. MINCODE, MAXCODE and VALPTR each have 16 entries, one for each possible code size. MINCODE(I) contains the smallest code value for a given length I, MAXCODE(I) contains the largest code value for a given length I, and VALPTR(I) contains the index to the start of the list of values in HUFFVAL which are decoded by code words of length I. The values in MINCODE and MAXCODE are signed 16-bit integers; therefore, a value of –1 sets all of the bits.
     The procedure for generating these tables is shown in Figure F.15. The procedure for DECODE is shown in Figure F.16. Note that the 8-bit “VALUE” is returned to the procedure which invokes DECODE.
     
     */
    
    /*
     
     ReadSegment_
     
     
     */
    
    void BuildHuffmanVales(void *Options, uint8_t *BitLengths, uint8_t *Values, uint8_t Count) {
        JPEGOptions *JPEG = Options;
        if (JPEG != NULL && BitLengths != NULL && Values != NULL && Count > 0) {

        }
        /*
         
         So we basically take the value, and the bitlength, if the bit length is longer than the number of bits the value takes up, prepend it with 0's
         
         that's real god damn easy then.
         
         so now I basically just need to read bits until I hit the stop bit, the number of bits read says the array element to search for the values.
         
         
         
         */
    }
    
    void ReadSegment_StartOfFrame(void *Options, BitBuffer *BitB) { // Size = 17
        JPEGOptions *JPEG                           = Options;
        uint16_t     TableSize                      = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 16) - 2; // 26
        JPEG->BitDepth                              = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 8);      // 14
        JPEG->Height                                = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 16);     // 3528
        JPEG->Width                                 = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 16);     // 2640
        JPEG->NumChannels                           = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 8);      // 2???
        JPEG->Components                            = calloc(JPEG->NumChannels, sizeof(ComponentParameters));
        for (uint8_t Component = 0; Component < JPEG->NumChannels; Component++) {
            JPEG->Components[Component].ComponentID = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 8); // 1, 2,
            JPEG->Components[Component].Horizontal  = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 8); // 17, 17.
            JPEG->Components[Component].Vertical    = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 8); // 0, 0
        }
    }
    
    void ReadSegment_JFIF(void *Options, BitBuffer *BitB) { // 14 bytes
        JPEGOptions *JPEG                           = Options;
        
        // After the App0/JFIF Header the next 4 bytes should be 0x4A464946
    }
    
    void ReadSegment_Extension7(void *Options, BitBuffer *BitB) { // JPEG-LS, 15 bytes
        JPEGOptions *JPEG                           = Options;
        uint16_t     TableSize                      = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 16) - 2; // 26
        
        // 0810 E01E 0003 0111 0002 1100 0311 00
    }
    
    void ReadSegment_DefineHuffmanTable(void *Options, BitBuffer *BitB) { // JPEG DHT
        JPEGOptions *JPEG                                    = Options;
        uint16_t     TableSize                               = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 16) - 2; // 26
        uint16_t     BytesRemaining                          = TableSize;
        while (BytesRemaining > 0) {
            JPEG->HuffmanTableCount                         += 1;
            uint8_t TableCount                               = JPEG->HuffmanTableCount - 1;
            uint8_t TableClass                               = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 4); // 0
            uint8_t TableID                                  = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 4); // 0, 0-3 possible
            if (TableClass == 0 && TableID <= 3) {
                uint16_t NumValues                           = 0;
                uint8_t *BitCounts                           = calloc(NumBitCounts, sizeof(uint8_t)); // BITS
                
                for (uint8_t Count = 0; Count < NumBitCounts; Count++) {
                    BitCounts[Count]                         = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 8);
                    NumValues                               += BitCounts[Count]; // 9
                }
                
                uint8_t *Values                              = calloc(NumValues, sizeof(uint8_t));
                
                for (uint8_t Value = 0; Value < NumValues; Value++) {
                    Values[Value]                            = (uint8_t) BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 8);
                }
                
                /*
                 BITS    = [0, 3, 2, ...]
                 HUFFVAL = [0x43, 0x44, 0x45, 0x41, 0x42]
                 
                 Huffman[0][0] = {0b00, 0x43}
                 Huffman[0][1] = {0b01, 0x44}
                 Huffman[0][2] = {0b10, 0x45}
                 Huffman[1][0] = {0b110, 0x41}
                 Huffman[1][1] = {0b111, 0x42}
                 
                 The 2D representation is while building the codes.
                 
                 we're going for a 1D representation in the struct
                 
                 Huffman->Symbols[Index] = 0b111
                 Huffman->Codes[Index]   = 0x42
                 
                 So, keep track of the previous value used to build the table in a variable, PreviousCode
                 */
                
                JPEG_BuildTable(JPEG, TableID, BitCounts, NumValues, Values);
                
                free(BitCounts);
                free(Values);
                
                
                /* All codes of each bit length with sex bits (0b1, 0b11, 0b111, etc) are reserved as prefixes */
                
                /* Now we need to build the actual HUFFSIZE and HUFFCODE */
                
                /* HUFFSIZE aka C.1 */
                uint16_t *HUFFSIZE       = calloc(256, sizeof(uint8_t));
                uint16_t Index           = 0;
                for (uint16_t Value = 0; Value < NumValues; Value++) {
                    for (uint16_t Bit = 0; Bit < BitCounts[Value]; Bit++) {
                        HUFFSIZE[Index]  = Value + 1;
                        Index           += 1;
                    }
                }
                uint16_t LastK           = Index;
                /* HUFFSIZE aka C.1 */
            } else if (TableClass > 0) {
                Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("TableClass: %u is invalid for Lossless JPEG"), TableClass);
            } else if (JPEG->Huffman[TableCount]->TableID > 3) {
                Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("TableID %u is invalid"), JPEG->Huffman[TableCount]->TableID);
            }
        }
    }
    
    void ReadSegment_DefineArithmeticTable(void *Options, BitBuffer *BitB) {
        JPEGOptions *JPEG                     = Options;
        JPEG->Arithmetic->CodeLength          = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 16);
        JPEG->Arithmetic->TableClass          = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 4);
        JPEG->Arithmetic->TableDestination    = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 4);
        JPEG->Arithmetic->CodeValue           = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 8);
    }
    
    void ReadSegment_DefineRestartInterval(void *Options, BitBuffer *BitB) {
        JPEGOptions *JPEG                     = Options;
        JPEG->RestartInterval                 = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 16);
    }
    
    void ReadSegment_DefineNumberOfLines(void *Options, BitBuffer *BitB) {
        JPEGOptions *JPEG                     = Options;
        JPEG->Height                          = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 16);
    }
    
    void ReadSegment_Comment(void *Options, BitBuffer *BitB) {
        uint16_t CommentSize                  = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 16);
        BitBuffer_Seek(BitB, CommentSize);
    }
    
    void ReadSegment_ApplicationData(void *Options, BitBuffer *BitB) {
        uint16_t AppDataSize                  = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 16);
        BitBuffer_Seek(BitB, AppDataSize);
    }
    
    void ReadSegment_StartOfScan(void *Options, BitBuffer *BitB) { // Size = 12
        JPEGOptions *JPEG                               = (JPEGOptions*) Options;
        uint16_t Length                                 = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 16) - 2;
        uint8_t  NumberOfComponents                     = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 8);  // 3
        if (Length == ((NumberOfComponents * 2) + 4) && NumberOfComponents >= 1 && NumberOfComponents <= 4) {
            JPEG->ComponentSelector                     = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 8);  // 0x52
            JPEG->SamplesPerLine                        = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 8);  // 0x00
            JPEG->Components                            = calloc(NumberOfComponents, sizeof(ComponentParameters));
            for (uint8_t Component = 0; Component < NumberOfComponents; Component++) {
                JPEG->Components[Component].ComponentID = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 8); // 0x52, 0, 0
                JPEG->Components[Component].Horizontal  = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 4); // 0, 0, 0
                JPEG->Components[Component].Vertical    = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 4); // 0, 1, 0
            }
            JPEG->Predictor                             = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 8); // called SS in the standard, Predictor 1
            JPEG->SE                                    = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 8);
            JPEG->Ah                                    = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 4);
            JPEG->Al                                    = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 4);
        } else if (Length != ((NumberOfComponents * 2) + 4)) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("StartOfScan Size: %hu is invalid"), Length);
        } else if (NumberOfComponents < 1 || NumberOfComponents > 4) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Number of Channels: %hhu is invalid"), NumberOfComponents);
        }
    }
    
    static void SkipUnsupportedSegments(BitBuffer *BitB, uint16_t ChunkMarker) {
        uint16_t Length = 0;
        
        switch (ChunkMarker) {
            case 0xFFC0:
            case 0xFFC1:
            case 0xFFC2:
            case 0xFFC5:
            case 0xFFC6:
            case 0xFFC9:
            case 0xFFCA:
            case 0xFFCD:
            case 0xFFCE: // ^All these are Lossy Start of Frame Markers
            case 0xFFDB: // Quantization Table, Lossy.
            case 0xFFDE: // Define Hierarchical Progression, Lossy?
            case 0xFFDF: // Expand Reference Component, Lossy?
                Length = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 16) - 2;
                BitBuffer_Seek(BitB, Bytes2Bits(Length));
                Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Lossy JPEG is not supported"));
                break;
            default:
                Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Marker 0x%X is unknown"), ChunkMarker);
                break;
        }
    }
    
    void JPEG_ReadSegments(void *Options, BitBuffer *BitB) {
        JPEGOptions *JPEG                     = Options;
        uint16_t     Marker                   = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 16);
        switch (Marker) {
            case Marker_StartOfFrameLossless1: // Huffman, non-Differential
            case Marker_StartOfFrameLossless2: // Huffman, Differential
            case Marker_StartOfFrameLossless3: // Arithmetic, non-Differential
            case Marker_StartOfFrameLossless4: // Arithmetic, Differential
                ReadSegment_StartOfFrame(Options, BitB);
                break;
            case Marker_DefineHuffmanTable:
                ReadSegment_DefineHuffmanTable(Options, BitB);
                break;
            case Marker_DefineArthimeticTable:
                ReadSegment_DefineArithmeticTable(Options, BitB);
                break;
            case Marker_StartOfScan:
                ReadSegment_StartOfScan(Options, BitB);
                // Start Decoding
                break;
            case Marker_NumberOfLines:
                ReadSegment_DefineNumberOfLines(Options, BitB);
                break;
            case Marker_RestartInterval:
                ReadSegment_DefineRestartInterval(Options, BitB);
            case Marker_Restart0:
            case Marker_Restart1:
            case Marker_Restart2:
            case Marker_Restart3:
            case Marker_Restart4:
            case Marker_Restart5:
            case Marker_Restart6:
            case Marker_Restart7:
                if (JPEG->EntropyCoder == EntropyCoder_Huffman) {
                    ReadSegment_DefineHuffmanTable(Options, BitB);
                } else if (JPEG->EntropyCoder == EntropyCoder_Arithmetic) {
                    ReadSegment_DefineArithmeticTable(Options, BitB);
                }
            default:
                SkipUnsupportedSegments(BitB, Marker);
                break;
        }
    }
    
#define JPEGNumMagicIDs 1
    
    static const MagicIDSizes JPEGMagicIDSize = {
        .NumSizes              = JPEGNumMagicIDs,
        .Sizes                 = {[0] = 2},
    };
    
    static const MagicIDOffsets JPEGMagicIDOffset = {
        .NumOffsets            = JPEGNumMagicIDs,
        .Offsets               = {[0] = 0},
    };
    
    static const MagicIDNumbers JPEGMagicIDNumber = {
        .NumMagicIDs           = JPEGNumMagicIDs,
        .MagicNumbers          = {[0] = (uint8_t[2]){0xFF, 0xD8}},
    };
    
    static const MagicIDs JPEGMagicIDs = {
        .Sizes                 = &JPEGMagicIDSize,
        .Offsets               = &JPEGMagicIDOffset,
        .Number                = &JPEGMagicIDNumber,
    };
    
    static const OVIADecoder JPEGDecoder = {
        .Function_Initialize   = JPEGOptions_Init,
        .Function_Decode       = JPEG_Extract,
        .Function_Read         = JPEG_ReadSegments,
        .Function_Deinitialize = JPEGOptions_Deinit,
        .MagicID               = &JPEGMagicIDs,
        .MediaType             = MediaType_Image,
        .DecoderID             = CodecID_JPEG,
    };
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
