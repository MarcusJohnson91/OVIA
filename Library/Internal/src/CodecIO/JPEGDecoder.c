#include "../../include/Private/CodecIO/JPEGCodec.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    void JPEG_ReadSegment_StartOfFrame(void *Options, BitBuffer *BitB) {
        JPEGOptions *JPEG                                = Options;
        if (JPEG != NULL && BitB != NULL) {
            uint16_t HeaderSize                          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16); // 17
            JPEG->BitDepth                               = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);  // 9
            JPEG->Height                                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16); // 8, Height
            JPEG->Width                                  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16); // 8, Width
            JPEG->NumChannels                            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);  // 3, Components
            JPEG->Channels                               = calloc(JPEG->NumChannels, sizeof(JPEG_ChannelParameters));
            if (JPEG->Channels != NULL) {
                for (uint8_t Channel = 0; Channel < JPEG->NumChannels; Channel++) {
                    JPEG->Channels[Channel].ChannelID    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8); // 1, 2, 3,
                    JPEG->Channels[Channel].HorizontalSF = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 4); // 1, 1, 1,
                    JPEG->Channels[Channel].VerticalSF   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 4); // 1, 1, 1,
                    JPEG->Channels[Channel].TableID      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8); // 0, 0, 0
                }
            }
            BitBuffer_Seek(BitB, Bytes2Bits(HeaderSize - ((JPEG->NumChannels * 3) + 8)));
        } else if (JPEG == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void JPEG_ReadSegment_JFIF(void *Options, BitBuffer *BitB) {
        JPEGOptions *JPEG                           = Options;
        if (JPEG != NULL && BitB != NULL) {
            uint16_t HeaderSize                     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16);
            BitBuffer_Seek(BitB, Bytes2Bits(HeaderSize));
        } else if (JPEG == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }

    }

    void JPEG_ReadSegment_Extension7(void *Options, BitBuffer *BitB) {
        JPEGOptions *JPEG                           = Options;
        if (JPEG != NULL && BitB != NULL) {
            uint16_t HeaderSize                     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16);
            BitBuffer_Seek(BitB, Bytes2Bits(HeaderSize));
        } else if (JPEG == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
        // 0810 E01E 0003 0111 0002 1100 0311 00
    }

    void JPEG_ReadSegment_DefineHuffmanTable(void *Options, BitBuffer *BitB) {
        JPEGOptions *JPEG                                    = Options;
        if (JPEG != NULL && BitB != NULL) {
            uint16_t HeaderRemaining                         = JPEG->HeaderSize;
            JPEG->EntropyCoder                               = EntropyCoder_Huffman;
            JPEG->Huffman->Values                            = calloc(2, sizeof(HuffmanValue*));
            while (HeaderRemaining > 0) {
                if (JPEG->Huffman->Values != NULL) {
                    uint8_t TableType                        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 4); // 1
                    uint8_t TableID                          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 4); // 1

                    if (TableType == 0) {
                        uint8_t BitLengths[16];
                        uint8_t NumSymbols                   = 0;

                        for (uint8_t Count = 0; Count < 16; Count++) {
                            BitLengths[Count]                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 8);
                            if (BitLengths[Count] > 0) {
                                NumSymbols                  += BitLengths[Count];
                            }
                            HeaderRemaining                -= 1;
                        }

                        JPEG->Huffman->NumValues[TableID]    = NumSymbols;

                        JPEG->Huffman->Values[TableID]       = calloc(NumSymbols, sizeof(HuffmanValue));
                        if (JPEG->Huffman->Values[TableID] != NULL) {
                            /*
                             [0] = { BITS= 00 01 05 01 01 [01] [01] 01 01 00 00 00 00 00 00 00
                                 [1] = BitLength=2: {HuffCode = 0b00, Symbol = 0x0}
                                 [2] = BitLength=3: {HuffCode = 0b010, Symbol = 0x1}, {HuffCode = 0b011, Symbol = 0x2}, {HuffCode = 0b100, Symbol = 0x3}, {HuffCode = 0b101, Symbol = 0x4}, {HuffCode = 0b110, Symbol = 0x5}
                                 [3] = BitLength=4: {HuffCode = 0b1110, Symbol = 0x6},
                                 [4] = BitLength=5: {HuffCode = 0b11110, Symbol = 0x7},
                                 [5] = BitLength=6: {HuffCode = 0b111110, Symbol = 0x8},
                                 [6] = BitLength=7: {HuffCode = 0b1111110, Symbol = 0x9},
                                 [7] = BitLength=8: {HuffCode = 0b11111110, Symbol = 0xA},
                                 [8] = BitLength=9: {HuffCode = 0b111111110, Symbol = 0xb},
                             },
                             [1] = { BITS= 00 03 01 01 01 01 01 01 01 01 01 00 00 00 00 00
                                [1] = BitLength=2: {HuffCode = 0b00, Symbol = 0x0}, {HuffCode = 0b01, Symbol = 0x1}, {HuffCode = 0b10, Symbol = 0x2}
                                [2] = BitLength=3: {HuffCode = 0b110, Symbol = 0x3}
                                [3] = BitLength=4: {HuffCode = 0b1110, Symbol = 0x4}
                                [4] = BitLength=5: {HuffCode = 0b11110, Symbol = 0x5}
                                [5] = BitLength=6: {HuffCode = 0b111110, Symbol = 0x6}
                                [6] = BitLength=7: {HuffCode = 0b1111110, Symbol = 0x7}
                                [7] = BitLength=8: {HuffCode = 0b11111110, Symbol = 0x8}
                                [8] = BitLength=9: {HuffCode = 0b111111110, Symbol = 0x9}
                                [9] = BitLength=A: {HuffCode = 0b1111111110, Symbol = 0xA}
                                [A] = BitLength=B: {HuffCode = 0b11111111110, Symbol = 0xB}
                             },

                             So, this is how Huffman decoding works.

                             How do we Encode, and then package both together in EntropyIO so we can do both?
                             */
                            uint8_t  BitLength                                = 0;
                            uint16_t HuffCode                                 = 0;
                            while (BitLength < 16) {
                                if (BitLengths[BitLength] > 0) {
                                    JPEG->Huffman->Values[TableID]->BitLength = BitLength;
                                    JPEG->Huffman->Values[TableID]->HuffCode  = HuffCode;
                                    HuffCode                                 += 1;
                                    JPEG->Huffman->Values[TableID]->Symbol    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 8);
                                    HeaderRemaining                          -= 1;
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
                            HeaderRemaining                                  -= Bytes2Skip;
                        }
                    }
                } else {
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate Huffman Tables"));
                }
            }
        } else if (JPEG == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void JPEG_ReadSegment_DefineArithmeticTable(void *Options, BitBuffer *BitB) {
        JPEGOptions *JPEG                         = Options;
        if (JPEG != NULL && BitB != NULL) {
            uint16_t HeaderSize                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16);
            JPEG->Arithmetic->CodeLength          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16);
            JPEG->Arithmetic->TableType           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 4);
            JPEG->Arithmetic->TableDestination    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 4);
            JPEG->Arithmetic->CodeValue           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 8);
            BitBuffer_Seek(BitB, Bytes2Bits(HeaderSize - 6));
        } else if (JPEG == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void JPEG_ReadSegment_DefineRestartInterval(void *Options, BitBuffer *BitB) {
        JPEGOptions *JPEG                         = Options;
        if (JPEG != NULL && BitB != NULL) {
            uint16_t HeaderSize                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16);
            JPEG->RestartInterval                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16);
            BitBuffer_Seek(BitB, Bytes2Bits(HeaderSize - 4));
        } else if (JPEG == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void JPEG_ReadSegment_DefineNumberOfLines(void *Options, BitBuffer *BitB) {
        JPEGOptions *JPEG                     = Options;
        if (JPEG != NULL && BitB != NULL) {
            uint16_t HeaderSize               = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16);
            JPEG->Height                      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16);
            BitBuffer_Seek(BitB, Bytes2Bits(HeaderSize - 4));
        } else if (JPEG == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void JPEG_ReadSegment_Comment(void *Options, BitBuffer *BitB) {
        JPEGOptions *JPEG                     = Options;
        if (JPEG != NULL && BitB != NULL) {
            uint16_t HeaderSize               = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16);
            JPEG->CommentSize                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 16);
            if (JPEG->CommentSize >= 3) {
                JPEG->Comment                 = calloc(JPEG->CommentSize - 2, sizeof(CharSet8));
                if (JPEG->Comment != NULL) {
                    for (uint16_t Byte = 0; Byte < JPEG->CommentSize - 2; Byte++) {
                        JPEG->Comment[Byte]   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 8);
                    }
                }
            }
            BitBuffer_Seek(BitB, Bytes2Bits(HeaderSize - JPEG->CommentSize));
        } else if (JPEG == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void JPEG_ReadSegment_ApplicationData(void *Options, BitBuffer *BitB) {
        JPEGOptions *JPEG                     = Options;
        if (JPEG != NULL && BitB != NULL) {
            uint16_t AppDataSize              = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16);
            BitBuffer_Seek(BitB, Bytes2Bits(AppDataSize));
        } else if (JPEG == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void JPEG_ReadSegment_StartOfScan(void *Options, BitBuffer *BitB) {
        JPEGOptions *JPEG                          = Options;
        if (JPEG != NULL && BitB != NULL) {
            uint16_t HeaderSize                    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16); // 12
            uint8_t  NumChannels                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);  // 3
            JPEG_ChannelParameters *Channels       = calloc(NumChannels, sizeof(JPEG_ChannelParameters));
            if (Channels != NULL) {
                for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                    Channels[Channel].ChannelID    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8); // 1, 2, 3,
                    Channels[Channel].HorizontalSF = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 4); // 0, 1, 1,
                    Channels[Channel].VerticalSF   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 4); // 0, 0, 0
                }
            } else {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate %d channels"), NumChannels);
            }
            uint8_t Predictor                      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8); // 1
            BitBuffer_Seek(BitB, 16); // 3 fields: Se, Ah, Al; 16 bits, should be zero, but otherwise have no meaning.
            /* SoS header parsing is over, now read the data */
            /* So, Markers can occur here, 0xFF is converted to 0xFF 0x00 by the encoder so undo that here */
            /* Image Data: F1 CF D1 3F 91 -47 bytes of just zeros- 0F */
            if (JPEG->EntropyCoder == EntropyCoder_Huffman) {
                if (JPEG->CodingMode == CodingMode_NonHierarchical) {
                    // Ok, so what do we do, how do we read this tree?
                } else if (JPEG->CodingMode == CodingMode_Hierarchical) {
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Hierarchical coding is not yet implemented"));
                }
            } else if (JPEG->EntropyCoder == EntropyCoder_Arithmetic) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Arithmetic coding is not yet implemented"));
            }
        } else if (JPEG == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void JPEG_ReadSegments(void *Options, BitBuffer *BitB) {
        JPEGOptions *JPEG                     = Options;
        if (JPEG != NULL && BitB != NULL) {
            uint16_t Marker                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 16);
            uint16_t HeaderSize               = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 16);
            switch (Marker) {
                case JPEGMarker_StartOfFrameLossless1: // Huffman, non-Hierarchical
                    Options->EntropyCoder = EntropyCoder_Huffman;
                    Options->CodingMode   = CodingMode_NonHierarchical;
                    JPEG_ReadSegment_StartOfFrame(Options, BitB);
                    break;
                case JPEGMarker_StartOfFrameLossless2: // Huffman, Hierarchical
                    Options->EntropyCoder = EntropyCoder_Huffman;
                    Options->CodingMode   = CodingMode_Hierarchical;
                    JPEG_ReadSegment_StartOfFrame(Options, BitB);
                    break;
                case JPEGMarker_StartOfFrameLossless3: // Arithmetic, non-Hierarchical
                    Options->EntropyCoder = EntropyCoder_Arithmetic;
                    Options->CodingMode   = CodingMode_NonHierarchical;
                    JPEG_ReadSegment_StartOfFrame(Options, BitB);
                    break;
                case JPEGMarker_StartOfFrameLossless4: // Arithmetic, Hierarchical
                    Options->EntropyCoder = EntropyCoder_Arithmetic;
                    Options->CodingMode   = CodingMode_Hierarchical;
                    JPEG_ReadSegment_StartOfFrame(Options, BitB);
                    break;
                case JPEGMarker_DefineHuffmanTable:
                    JPEG_ReadSegment_DefineHuffmanTable(Options, BitB);
                    break;
                case JPEGMarker_DefineArthimeticTable:
                    JPEG_ReadSegment_DefineArithmeticTable(Options, BitB);
                    break;
                case JPEGMarker_StartOfScan:
                    JPEG_ReadSegment_StartOfScan(Options, BitB);
                    break;
                case JPEGMarker_NumberOfLines:
                    JPEG_ReadSegment_DefineNumberOfLines(Options, BitB);
                    break;
                case JPEGMarker_RestartInterval:
                    JPEG_ReadSegment_DefineRestartInterval(Options, BitB);
                case JPEGMarker_Restart0:
                case JPEGMarker_Restart1:
                case JPEGMarker_Restart2:
                case JPEGMarker_Restart3:
                case JPEGMarker_Restart4:
                case JPEGMarker_Restart5:
                case JPEGMarker_Restart6:
                case JPEGMarker_Restart7:
                    if (JPEG->EntropyCoder == EntropyCoder_Huffman) {
                        JPEG_ReadSegment_DefineHuffmanTable(Options, BitB);
                    } else if (JPEG->EntropyCoder == EntropyCoder_Arithmetic) {
                        JPEG_ReadSegment_DefineArithmeticTable(Options, BitB);
                    }
                default:
                    BitBuffer_Seek(BitB, Bytes2Bits(HeaderSize - 2));
                    break;
            }
        } else if (JPEG == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    /*
     What all do I need to get everything together?

     I need OVIA Registry to be finished, and I blame the last fucking YEAR of stalling on the fucking registry.

     once thats in I'll be able to do everything.

     Well, not just the registry, we also need to know the limitations of each codec.

     then maybe each CodecLimits struct should contain pointers to the decoder and encoder
     */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
