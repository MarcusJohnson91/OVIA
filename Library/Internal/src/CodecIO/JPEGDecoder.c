#include "../../include/Private/CodecIO/JPEGCodec.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    void ParseSegment_StartOfFrame(void *Options, BitBuffer *BitB) {
        JPEGOptions *JPEG                                    = Options;
        if (JPEG != NULL && BitB != NULL) {
            uint16_t HeaderSize                              = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16);
            JPEG->BitDepth                                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);      // 12
            JPEG->Height                                     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16);     // 971, Height
            JPEG->Width                                      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16);     // 972, Width
            JPEG->NumChannels                                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);      // 3, Components
            JPEG->Channels                                   = calloc(JPEG->NumChannels, sizeof(ChannelParameters));
            if (JPEG->Channels != NULL) {
                for (uint8_t Channel = 0; Channel < JPEG->NumChannels; Channel++) {
                    JPEG->Channels[Channel].ChannelID        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8); // 0, 1, 2,
                    JPEG->Channels[Channel].HorizontalSF     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 4); // 2, 1, 1,
                    JPEG->Channels[Channel].VerticalSF       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 4); // 2, 1, 1,
                    JPEG->Channels[Channel].TableID          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8); // 0, 1, 1
                }
            }
            BitBuffer_Seek(BitB, Bytes2Bits(HeaderSize - ((JPEG->NumChannels * 3) + 8)));
        } else if (JPEG == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void ParseSegment_JFIF(void *Options, BitBuffer *BitB) {
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

    void ParseSegment_Extension7(void *Options, BitBuffer *BitB) {
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

    void ParseSegment_DefineHuffmanTable(void *Options, BitBuffer *BitB) {
        JPEGOptions *JPEG                              = Options;
        if (JPEG != NULL && BitB != NULL) {
            uint16_t HeaderSize                        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 16);
            JPEG->Huffman->TableClass                  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 4); // 0
            JPEG->Huffman->TableID                     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 4); // 0

            for (uint8_t Count = 0; Count < 16; Count++) {
                JPEG->Huffman->BitLengths[Count]       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 8);
            }

            uint8_t NumEntries                         = 0;
            uint8_t NumBytes                           = 0;

            for (uint8_t Code = 0; Code < 16; Code++) {
                if (JPEG->Huffman->BitLengths[Code] > 0) {
                    NumEntries                        += 1;
                    NumBytes                          += JPEG->Huffman->BitLengths[Code];
                }
            }

            JPEG->Huffman->Values                      = calloc(NumEntries, sizeof(HuffmanValue));

            uint8_t ValueIndex                         = 0;
            for (uint8_t Value = 0; Value < 16; Value++) {
                if (JPEG->Huffman->BitLengths[Value] > 0) {
                     JPEG->Huffman->Values[ValueIndex].BitLength         = Value;
                     JPEG->Huffman->Values[ValueIndex].NumValues         = JPEG->Huffman->BitLengths[Value];
                     JPEG->Huffman->Values[ValueIndex].Values            = calloc(JPEG->Huffman->Values[ValueIndex].NumValues, sizeof(uint16_t));

                     for (uint8_t Index2 = 0; Index2 < JPEG->Huffman->Values[ValueIndex].NumValues; Index2++) {
                        JPEG->Huffman->Values[ValueIndex].Values[Index2] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 8);
                     }
                }
            }
            BitBuffer_Seek(BitB, Bytes2Bits(HeaderSize - (18 + NumBytes)));
        } else if (JPEG == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void ParseSegment_DefineArithmeticTable(void *Options, BitBuffer *BitB) {
        JPEGOptions *JPEG                         = Options;
        if (JPEG != NULL && BitB != NULL) {
            uint16_t HeaderSize                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16);
            JPEG->Arithmetic->CodeLength          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16);
            JPEG->Arithmetic->TableClass          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 4);
            JPEG->Arithmetic->TableDestination    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 4);
            JPEG->Arithmetic->CodeValue           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 8);
            BitBuffer_Seek(BitB, Bytes2Bits(HeaderSize - 6));
        } else if (JPEG == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void ParseSegment_DefineRestartInterval(void *Options, BitBuffer *BitB) {
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

    void ParseSegment_DefineNumberOfLines(void *Options, BitBuffer *BitB) {
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

    void ParseSegment_Comment(void *Options, BitBuffer *BitB) {
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

    void ParseSegment_ApplicationData(void *Options, BitBuffer *BitB) {
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

    void ParseSegment_StartOfScan(void *Options, BitBuffer *BitB) { // 000C0300 00011102 11000001
        JPEGOptions *JPEG                         = Options;
        if (JPEG != NULL && BitB != NULL) {
            uint16_t HeaderSize                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16);
            uint8_t  NumChannels                  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);  // 3
            uint8_t  ChannelID                    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);  // 0
            uint16_t SamplesPerLine               = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16); // 1?
            /*
             if SamplesPerLine > 1:
             the order of interleaved components in the MCU is Cs1 first, Cs2 second, etc.
             the following restriction shall be placed on the image components contained in the scan:
             j = 1 to SamplesPerLine
             HorizontalSF[j] * VerticalSF[j] <= 10
             */
            ChannelParameters *Channels           = calloc(NumChannels, sizeof(ChannelParameters));
            for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                Channels[Channel].ChannelID       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8); // 0
                Channels[Channel].HorizontalSF    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 4); // 0
                Channels[Channel].VerticalSF      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 4); // 0
                Channels[Channel].TableID         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8); // 0
            }
        } else if (JPEG == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    // How should we read the actual data?

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
                Length = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16) - 2;
                BitBuffer_Seek(BitB, Bytes2Bits(Length));
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Lossy JPEG is not supported"));
                break;
            default:
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Marker 0x%X is unknown"), ChunkMarker);
                break;
        }
    }

    void ReadSegments(void *Options, BitBuffer *BitB) {
        JPEGOptions *JPEG                     = Options;
        if (JPEG != NULL && BitB != NULL) {
            uint16_t HeaderSize               = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 16);
            uint16_t Marker                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 16);
            switch (Marker) {
                case Marker_StartOfFrameLossless1: // Huffman, non-Differential
                case Marker_StartOfFrameLossless2: // Huffman, Differential
                case Marker_StartOfFrameLossless3: // Arithmetic, non-Differential
                case Marker_StartOfFrameLossless4: // Arithmetic, Differential
                    ParseSegment_StartOfFrame(Options, BitB);
                    break;
                case Marker_DefineHuffmanTable:
                    ParseSegment_DefineHuffmanTable(Options, BitB);
                    break;
                case Marker_DefineArthimeticTable:
                    ParseSegment_DefineArithmeticTable(Options, BitB);
                    break;
                case Marker_StartOfScan:
                    ParseSegment_StartOfScan(Options, BitB);
                    break;
                case Marker_NumberOfLines:
                    ParseSegment_DefineNumberOfLines(Options, BitB);
                    break;
                case Marker_RestartInterval:
                    ParseSegment_DefineRestartInterval(Options, BitB);
                case Marker_Restart0:
                case Marker_Restart1:
                case Marker_Restart2:
                case Marker_Restart3:
                case Marker_Restart4:
                case Marker_Restart5:
                case Marker_Restart6:
                case Marker_Restart7:
                    if (JPEG->EntropyCoder == EntropyCoder_Huffman) {
                        ParseSegment_DefineHuffmanTable(Options, BitB);
                    } else if (JPEG->EntropyCoder == EntropyCoder_Arithmetic) {
                        ParseSegment_DefineArithmeticTable(Options, BitB);
                    }
                default:
                    SkipUnsupportedSegments(BitB, Marker);
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



    /*
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
     */
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
