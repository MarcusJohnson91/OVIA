#include "../../include/Private/JPEGCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void StartOfFrame_Parse(void *Options, BitBuffer *BitB) {
        JPEGOptions *JPEG                           = Options;
        JPEG->BitDepth                              = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8);      // 12
        JPEG->Height                                = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 16);     // 4864, Height
        JPEG->Width                                 = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 16);     // 3064, Width
        JPEG->NumChannels                           = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8);      // 1, Components
        JPEG->Components                            = calloc(JPEG->NumChannels, sizeof(ComponentParameters));
        for (uint8_t Component = 0; Component < JPEG->NumChannels; Component++) {
            JPEG->Components[Component].ComponentID = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8); // 1
            JPEG->Components[Component].Horizontal  = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8); // 17
            JPEG->Components[Component].Vertical    = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8); // 0
        }
    }
    
    void DefineHuffmanTable_Parse(void *Options, BitBuffer *BitB) { // JPEG DHT
        JPEGOptions *JPEG                          = Options;
        JPEG->Huffman->TableClass                  = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 4); // 0
        JPEG->Huffman->TableID                     = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 4); // 0
        
        for (uint8_t Count = 0; Count < 16; Count++) {
            JPEG->Huffman->BitLengths[Count]       = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 8);
            // 0, 3, 0, 3, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0
            // ^Count these values to know how many bytes to read
        }
        
        uint16_t NumCodes                          = 0;
        
        for (uint8_t Code = 0; Code < 16; Code++) {
            NumCodes                              += JPEG->Huffman->BitLengths[Code]; // 9
        }
        
        JPEG->Huffman->NumBits                     = calloc(NumCodes, sizeof(uint8_t)); // NumBits to read to get the diff
        
        // Now I just need to loop over each huffman code length parameter reading as many of the bitstring sizes as it says
        
        /*
         
         We need morre descriptive variable names:
         
         the 0-16 code is the count of codes for each huffman code length.
         
         So call it CountOfBtLengthCodes?
         
         The Array needs to be called BitLengths
         
         
         
         */
        
        for (uint8_t Count = 0; Count < 16; Count++) {
            if (JPEG->Huffman->BitLengths[Count] > 0) {
                for (uint8_t NumCodes = 0; NumCodes < JPEG->Huffman->BitLengths[Count]; NumCodes++) {
                    JPEG->Huffman->NumBits[NumCodes] = BitBuffer_ReadBits(BitB, MSByteFirst, MSBitFirst, 8);
                    /*
                     
                     Also, I should reorganize this to be an array with 2 values, the index of the array is the length in bits of the code, the code is the number of bits to read and the
                     
                     So, there are 3 2-bit codes, the array needs to be represented as [2][X(0-2)][BitString][NumberOfBitsInHuffmanCode]
                     
                     */
                    
                    
                    
                    
                    
                    
                    
                    // 2[4, 5, 6] 4[2, 3, 7] 5[0] 6[1] 7[8] 8[A] 9[9] 10[B] 11[C]
                    
                    // All 1 bit code lengths are reserved
                    
                    // 11[C] = 11[12], so 12 with 11 bits... 0b11000000000? nope, 11111111110 which takes 11 bits, 10 1's 1 stop 0 bit.
                    
                    // BitLength 2, has 3 variables which makes sense because 2^2 = 4,
                    // so 01, 10, 11?
                    // 0b1111111110 =
                }
            }
        }
    }
    
    void DefineArithmeticTable_Parse(void *Options, BitBuffer *BitB) {
        JPEGOptions *JPEG                     = Options;
        JPEG->Arithmetic->CodeLength          = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16);
        JPEG->Arithmetic->TableClass          = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 4);
        JPEG->Arithmetic->TableDestination    = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 4);
        JPEG->Arithmetic->CodeValue           = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 8);
    }
    
    void DefineRestartInterval_Parse(void *Options, BitBuffer *BitB) {
        JPEGOptions *JPEG                     = Options;
        JPEG->RestartInterval                 = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16);
    }
    
    void DefineNumberOfLines_Parse(void *Options, BitBuffer *BitB) {
        JPEGOptions *JPEG                     = Options;
        JPEG->Height                          = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16);
    }
    
    void Comment_Parse(void *Options, BitBuffer *BitB) {
        JPEGOptions *JPEG                     = Options;
        uint16_t CommentSize                  = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16);
        BitBuffer_Seek(BitB, CommentSize);
    }
    
    void ApplicationData_Parse(void *Options, BitBuffer *BitB) {
        JPEGOptions *JPEG                     = Options;
        uint16_t AppDataSize                  = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16);
        BitBuffer_Seek(BitB, AppDataSize);
    }
    
    void StartOfScan_Parse(void *Options, BitBuffer *BitB) {
        JPEGOptions *JPEG                     = Options;
        uint8_t  NumberOfComponents           = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8);  // 1
        uint8_t  ComponentSelector            = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8);  // 1
        uint16_t SamplesPerLine               = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 16); // 1
        ComponentParameters *Components       = calloc(NumberOfComponents, sizeof(ComponentParameters));
        for (uint8_t Component = 0; Component < NumberOfComponents; Component++) {
            Components[Component].ComponentID = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8); // 0
            Components[Component].Horizontal  = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 4); // 0
            Components[Component].Vertical    = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 4); // 0
        }
        // Entropy coded data, 0xFF bytes are padded with 0's to make sure that no invalid marker is incorrectly found seen
        // Now we need to while loop to make sure we don't hit a Huffman End of Block symbol, and check if a byte starts with 0xFF00 aka 65280, if it does read another byte and OR it with the first.
        
        // 0b1111 1111 10 = 10 bits, 0x3FE; read 11 bits for the value: FFBF FC 6F Data: 0xFFFFFFFF & 0x3FFFFF = 0x3FFC6F
        // first 6 bits = 0b111111
        // last 5 bits = 0b11111
        // MSByteMSBit = 0b11111111111 = 0x7FF = 2047
        // MSByteLSBit = 0x46F = 1135, signed so negative = -912?
        //
    }
    
    static void SkipUnsupportedChunks(BitBuffer *BitB, uint16_t ChunkMarker) {
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
            case 0xFFDB: // Quantization Table, Lossy?
            case 0xFFDE: // Define Hierarchical Progression, Lossy?
            case 0xFFDF: // Expand Reference Component, Lossy?
                Length = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16) - 2;
                BitBuffer_Seek(BitB, Bytes2Bits(Length));
                Log(Log_DEBUG, __func__, UTF8String("Lossy JPEG is not supported"));
                break;
            default:
                Log(Log_DEBUG, __func__, UTF8String("Marker 0x%X is unknown"), ChunkMarker);
                break;
        }
    }
    
    void JPEG_Parse(void *Options, BitBuffer *BitB) {
        JPEGOptions *JPEG                     = Options;
        uint16_t     Marker                   = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16);
        switch (Marker) {
            case Marker_StartOfFrameLossless1: // Huffman, non-Differential
            case Marker_StartOfFrameLossless2: // Huffman, Differential
            case Marker_StartOfFrameLossless3: // Arithmetic, non-Differential
            case Marker_StartOfFrameLossless4: // Arithmetic, Differential
                StartOfFrame_Parse(Options, BitB);
                break;
            case Marker_DefineHuffmanTable:
                DefineHuffmanTable_Parse(Options, BitB);
                break;
            case Marker_DefineArthimeticTable:
                DefineArithmeticTable_Parse(Options, BitB);
                break;
            case Marker_StartOfScan:
                StartOfScan_Parse(Options, BitB);
                break;
            case Marker_NumberOfLines:
                DefineNumberOfLines_Parse(Options, BitB);
                break;
            case Marker_RestartInterval:
                DefineRestartInterval_Parse(Options, BitB);
            case Marker_Restart0:
            case Marker_Restart1:
            case Marker_Restart2:
            case Marker_Restart3:
            case Marker_Restart4:
            case Marker_Restart5:
            case Marker_Restart6:
            case Marker_Restart7:
                if (JPEG->EntropyCoder == EntropyCoder_Huffman) {
                    DefineHuffmanTable_Parse(Options, BitB);
                } else if (JPEG->EntropyCoder == EntropyCoder_Arithmetic) {
                    DefineArithmeticTable_Parse(Options, BitB);
                }
            default:
                SkipUnsupportedChunks(BitB, Marker);
                break;
        }
    }
    
    static void RegisterDecoder_JPEG(OVIA *Ovia) {
        Ovia->NumDecoders                                 += 1;
        uint64_t DecoderIndex                              = Ovia->NumDecoders;
        Ovia->Decoders                                     = realloc(Ovia->Decoders, sizeof(OVIADecoder) * Ovia->NumDecoders);
        
        Ovia->Decoders[DecoderIndex].DecoderID             = CodecID_JPEG;
        Ovia->Decoders[DecoderIndex].MediaType             = MediaType_Image;
        Ovia->Decoders[DecoderIndex].NumMagicIDs           = 1;
        Ovia->Decoders[DecoderIndex].MagicIDOffset[0]      = 0;
        Ovia->Decoders[DecoderIndex].MagicIDSize[0]        = 2;
        Ovia->Decoders[DecoderIndex].MagicID[0]            = (uint8_t[2]) {0xFF, 0xD8};
        Ovia->Decoders[DecoderIndex].Function_Initialize   = JPEGOptions_Init;
        Ovia->Decoders[DecoderIndex].Function_Parse        = JPEG_Parse;
        Ovia->Decoders[DecoderIndex].Function_Decode       = JPEG_Extract;
        Ovia->Decoders[DecoderIndex].Function_Deinitialize = JPEGOptions_Deinit;
    }
    
#ifdef __cplusplus
}
#endif
