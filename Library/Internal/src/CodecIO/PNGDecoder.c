#include "../../include/Private/PNGCommon.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    /*
     How do we handle DAT blocks?
     
     Shit, let the main chunk parser handle it.
     
     Each time a i/fDAT chunk appears, we need to decode it then and there.
     
     Also, we need to have all the DAT functions take the ImageContainer as a chunk.
     
     So, we need our DAT parser, and then we need a Huffman block parser
     */
    
    void PNG_Flate_ReadZlibHeader(void *Options, BitBuffer *BitB) { // This will be the main function for each Zlib block
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG            = Options;
            uint8_t  CompressionMethod = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 4);
            uint8_t  CompressionInfo   = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 4);
            uint8_t  FCHECK            = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 5);
            bool     FDICT             = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 1);
            uint8_t  FLEVEL            = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 2);
            uint32_t DictID  = 0;
            
            if (CompressionInfo == 7 && CompressionMethod == 8) {
                uint16_t FCheck = CompressionInfo << 12;
                FCheck         |= CompressionMethod << 8;
                FCheck         |= FLEVEL << 6;
                FCheck         |= FDICT << 5;
                FCheck         |= FCHECK;
                if (FCheck % 31 == 0) {
                    if (FDICT == Yes) {
                        DictID  = (uint32_t) BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 32);
                    }
                } else {
                    Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Invalid Flate Header %d"), FCheck);
                }
            } else if (CompressionInfo != 7) {
                Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Compresion Info %d is invalid"), CompressionInfo);
            } else if (CompressionMethod != 8) {
                Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Compression Method %d is invalid"), CompressionMethod);
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_Flate_ReadDeflateBlock(void *Options, BitBuffer *BitB, ImageContainer *Image) {
        if (Options != NULL && BitB != NULL && Image != NULL) {
            PNGOptions *PNG       = Options;
            bool    BFINAL        = No;
            uint8_t BTYPE         = 0;
            do {
                BFINAL            = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 1); // Yes
                BTYPE             = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 2); // 0b10 aka 2 aka BlockType_Dynamic
                if (BTYPE == BlockType_Literal) {
                    BitBuffer_Align(BitB, 1);
                    uint16_t LEN  = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 16);
                    uint16_t NLEN = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 16) ^ 0xFFFF;
                    if (LEN == NLEN) {
                        // Copy LEN bytes from the stream to the image
                    } else {
                        Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Literal Block Length and 1's Complement Length do not match"));
                    }
                } else if (BTYPE == BlockType_Fixed) {
                    HuffmanTree *Length        = PNG_Flate_BuildHuffmanTree(FixedLiteralTable, 288);
                    HuffmanTree *Distance      = PNG_Flate_BuildHuffmanTree(FixedDistanceTable, 32);
                    PNG_Flate_ReadHuffman(PNG, BitB, Length, Distance, Image);
                } else if (BTYPE == BlockType_Dynamic) {
                    uint16_t NumLengthCodes               = 257 + BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 5); // HLIT; 21, 26?
                    uint8_t  NumDistCodes                 = 1   + BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 5); // HDIST; 17, 9?
                    uint8_t  NumCodeLengthCodeLengthCodes = 4   + BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 4); // HCLEN; 13,
                    
                    if (NumLengthCodes <= MaxLiteralLengthCodes && NumDistCodes <= MaxDistanceCodes) {
                        uint16_t *CodeLengthCodeLengths   = calloc(NumMetaCodes, sizeof(uint16_t));
                        
                        for (uint8_t CodeLengthCodeLengthCode = 0; CodeLengthCodeLengthCode < NumCodeLengthCodeLengthCodes; CodeLengthCodeLengthCode++) {
                            CodeLengthCodeLengths[MetaCodeLengthOrder[CodeLengthCodeLengthCode]] = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 3);
                        }
                        
                        HuffmanTree *Tree2DecodeTrees     = PNG_Flate_BuildHuffmanTree(CodeLengthCodeLengths, NumMetaCodes);
                        uint16_t Index = 0;
                        do {
                            uint64_t Length2Repeat        = 0; // len
                            
                            uint16_t Symbol               = ReadSymbol(BitB, Tree2DecodeTrees);
                            if (Symbol <= MaxBitsPerSymbol) {
                                CodeLengthCodeLengths[Index + 1] = Symbol;
                            } else {
                                if (Symbol == 16) {
                                    Length2Repeat         = CodeLengthCodeLengths[Index - 1];
                                    Symbol                = 3 + BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 2);
                                } else if (Symbol == 17) {
                                    Symbol                = 3 + BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 3);
                                } else {
                                    Symbol                = 11 + BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 7);
                                }
                                while (Symbol -= 1) {
                                    CodeLengthCodeLengths[Index += 1] = Length2Repeat;
                                }
                            }
                            Index                        += 1;
                        } while (Index < NumLengthCodes + NumDistCodes);
                    } else if (NumLengthCodes > MaxLiteralLengthCodes) {
                        Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Too many length codes %d, max is %d"), NumLengthCodes, MaxLiteralLengthCodes);
                    } else if (NumDistCodes > MaxDistanceCodes) {
                        Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Too many distance codes %d, max is %d"), NumDistCodes, MaxDistanceCodes);
                    }
                } else {
                    Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Invalid Block Type in Deflate block"));
                }
            } while (BFINAL == No);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (Image == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        }
    }
    
    void PNG_DAT_Decode(void *Options, BitBuffer *BitB, ImageContainer *Image) {
        if (BitB != NULL && Image != NULL) {
            uint8_t     ****ImageArrayBytes        = (uint8_t****) ImageContainer_GetArray(Image);
            HuffmanTree     *Tree                  = NULL;
            bool             IsFinalBlock          = false;
            do {
                IsFinalBlock                       = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 1); // 0
                uint8_t BlockType                  = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 2); // 0b00 = 0
                if (BlockType == BlockType_Literal) {
                    BitBuffer_Align(BitB, 1); // Skip the remaining 5 bits
                    uint16_t Bytes2Copy            = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_LSByte, BitIO_BitOrder_LSBit, 16); // 0x4F42 = 20,290
                    uint16_t Bytes2CopyXOR         = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_LSByte, BitIO_BitOrder_LSBit, 16) ^ 0xFFFF; // 0xB0BD = 0x4F42
                    
                    if (Bytes2Copy == Bytes2CopyXOR) {
                        for (uint16_t Byte = 0ULL; Byte < Bytes2Copy; Byte++) {
                            ImageArrayBytes[0][0][0][Byte] = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_LSByte, BitIO_BitOrder_LSBit, 8);
                        }
                    } else {
                        Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Data Error: Bytes2Copy does not match Bytes2CopyXOR in literal block"));
                    }
                } else if (BlockType == BlockType_Fixed) {
                    //Tree = PNG_Flate_BuildHuffmanTree();
                } else if (BlockType == BlockType_Dynamic) {
                    //Tree = PNG_Flate_BuildHuffmanTree();
                } else {
                    Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("PNG Invalid DAT Block"));
                }
                //PNG_Flate_Decode(PNG, BitB); // Actually read the data
            } while (IsFinalBlock == false);
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (Image == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        }
    }
    
    uint64_t ReadSymbol(BitBuffer *BitB, HuffmanTree *Tree) { // EQUILIVENT OF DECODE IN PUFF
        uint64_t Symbol              = 0ULL;
        uint16_t Count               = 0;
        if (BitB != NULL && Tree != NULL) {
            uint32_t FirstSymbolOfLength = 0;
            for (uint8_t Bit = 1; Bit <= MaxBitsPerSymbol; Bit++) {
                Symbol             <<= 1;
                Symbol              |= BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 1);
                Count                = Tree->Frequency[Bit];
                if (Symbol - Count < FirstSymbolOfLength) {
                    Symbol           = Tree->Symbol[Bit + (Symbol - FirstSymbolOfLength)];
                }
            }
        } else if (Tree == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("HuffmanTree Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
        return Symbol;
    }
    
    void PNG_Flate_ReadHuffman(void *Options, BitBuffer *BitB, HuffmanTree *LengthTree, HuffmanTree *DistanceTree, ImageContainer *Image) { // Codes in Puff
        if (Options != NULL && BitB != NULL && LengthTree != NULL && DistanceTree != NULL && Image != NULL) {
            // Out = ImageContainer array
            uint64_t Symbol = 0ULL;
            uint64_t Offset = 0ULL;
            do {
                Symbol                              = ReadSymbol(BitB, LengthTree);;
                if (Symbol > 256) { /* length */
                    Symbol  -= 257;
                    uint64_t Length                 = LengthBase[Symbol] + BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, LengthAdditionalBits[Symbol]);
                    
                    Symbol                          = ReadSymbol(BitB, DistanceTree);
                    uint64_t Distance               = DistanceBase[Symbol] + BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, DistanceAdditionalBits[Symbol]);
                    
                    uint8_t ****ImageArray          = (uint8_t****) ImageContainer_GetArray(Image);
                    if (ImageArray != NULL) {
                        for (uint64_t NumBytes2Copy = 0; NumBytes2Copy < Length; NumBytes2Copy++) {
                            // Copy NumBytes2Copde from Offset - Distance
                            
                            // We need to convert the View, Width, Height, and Channel as well as BitDepth into a byte location.
                            // This function might also be useful in ContainerIO as well.
                        }
                    } else {
                        Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Couldn't get ImageArray"));
                    }
                }
            } while (Symbol != EndOfBlock); /* end of block symbol */
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (LengthTree == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("LengthTree Pointer is NULL"));
        } else if (DistanceTree == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("DistanceTree Pointer is NULL"));
        } else if (Image == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        }
    }
    
    uint8_t PaethPredictor(int64_t Left, int64_t Above, int64_t UpperLeft) {
        uint8_t Output    = 0;
        
        int64_t Guess     = Left + Above - UpperLeft;
        int64_t DistanceA = AbsoluteI(Guess - Left);
        int64_t DistanceB = AbsoluteI(Guess - Above);
        int64_t DistanceC = AbsoluteI(Guess - UpperLeft);
        
        if (DistanceA <= DistanceB && DistanceA <= DistanceC) {
            Output = DistanceA;
        } else if (DistanceB <= DistanceC) {
            Output = DistanceB;
        } else {
            Output = DistanceC;
        }
        return Output;
    }
    
    void PNG_Filter_Sub(ImageContainer *Image) {
        if (Image != NULL) {
            MediaIO_ImageTypes Type = ImageContainer_GetType(Image);
            ImageChannelMap *Map = ImageContainer_GetChannelMap(Image);
            uint8_t     NumViews = ImageChannelMap_GetNumViews(Map);
            if (Type == ImageType_Integer8) {
                uint8_t  *ImageArray = (uint8_t*)  ImageContainer_GetArray(Image);
                
                for (uint8_t StereoView = 0; StereoView < NumViews; StereoView++) {
                    for (uint32_t Height = 0UL; Height < ImageContainer_GetHeight(Image); Height++) {
                        for (uint32_t Width = 0UL; Width < ImageContainer_GetWidth(Image); Width++) {
                            for (uint32_t Byte = 0UL; Byte < Bits2Bytes(ImageContainer_GetBitDepth(Image), RoundingType_Up); Byte++) {
                                ImageArray[StereoView * Height * Width * Byte] = (ImageArray[StereoView * Height * Width * Byte] + ImageArray[StereoView * Height * Width * (Byte + 1)]) % 256;
                            }
                        }
                    }
                }
            } else {
                uint16_t *ImageArray = (uint16_t*) ImageContainer_GetArray(Image);
                
                for (uint8_t StereoView = 0; StereoView < NumViews; StereoView++) {
                    for (uint32_t Height = 0UL; Height < ImageContainer_GetHeight(Image); Height++) {
                        for (uint32_t Width = 0UL; Width < ImageContainer_GetWidth(Image); Width++) {
                            for (uint32_t Byte = 0UL; Byte < Bits2Bytes(ImageContainer_GetBitDepth(Image), RoundingType_Up); Byte++) {
                                ImageArray[StereoView * Height * Width * Byte] = (ImageArray[StereoView * Height * Width * Byte] + ImageArray[StereoView * Height * Width * (Byte + 1)]) % 65536;
                            }
                        }
                    }
                }
            }
        } else if (Image == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        }
    }
    
    void PNG_Filter_Up(ImageContainer *Image) {
        if (Image != NULL) {
            MediaIO_ImageTypes Type = ImageContainer_GetType(Image);
            ImageChannelMap *Map = ImageContainer_GetChannelMap(Image);
            uint8_t     NumViews = ImageChannelMap_GetNumViews(Map);
            if (Type == ImageType_Integer8) {
                uint8_t  *ImageArray = (uint8_t*)  ImageContainer_GetArray(Image);
                
                for (uint8_t StereoView = 0; StereoView < NumViews; StereoView++) {
                    for (uint32_t Height = 0UL; Height < ImageContainer_GetHeight(Image); Height++) {
                        for (uint32_t Width = 0UL; Width < ImageContainer_GetWidth(Image); Width++) {
                            for (uint32_t Byte = 0UL; Byte < Bits2Bytes(ImageContainer_GetBitDepth(Image), RoundingType_Up); Byte++) {
                                ImageArray[StereoView * Height * Width * Byte] = (ImageArray[StereoView * Height * Width * Byte] + ImageArray[StereoView * (Height - 1) * Width * Byte]) % 256;
                            }
                        }
                    }
                }
            } else {
                uint16_t *ImageArray = (uint16_t*) ImageContainer_GetArray(Image);
                
                for (uint8_t StereoView = 0; StereoView < NumViews; StereoView++) {
                    for (uint32_t Height = 0UL; Height < ImageContainer_GetHeight(Image); Height++) {
                        for (uint32_t Width = 0UL; Width < ImageContainer_GetWidth(Image); Width++) {
                            for (uint32_t Byte = 0UL; Byte < Bits2Bytes(ImageContainer_GetBitDepth(Image), RoundingType_Up); Byte++) {
                                ImageArray[StereoView * Height * Width * Byte] = (ImageArray[StereoView * Height * Width * Byte] + ImageArray[StereoView * (Height - 1) * Width * Byte]) % 65536;
                            }
                        }
                    }
                }
            }
        } else if (Image == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        }
    }
    
    void PNG_Filter_Average(ImageContainer *Image) {
        if (Image != NULL) {
            MediaIO_ImageTypes Type = ImageContainer_GetType(Image);
            ImageChannelMap *Map = ImageContainer_GetChannelMap(Image);
            uint8_t     NumViews = ImageChannelMap_GetNumViews(Map);
            if (Type == ImageType_Integer8) {
                uint8_t  *ImageArray = (uint8_t*)  ImageContainer_GetArray(Image);
                
                for (uint8_t StereoView = 0; StereoView < NumViews; StereoView++) {
                    for (uint32_t Height = 0UL; Height < ImageContainer_GetHeight(Image); Height++) {
                        for (uint32_t Width = 0UL; Width < ImageContainer_GetWidth(Image); Width++) {
                            for (uint32_t Byte = 0UL; Byte < Bits2Bytes(ImageContainer_GetBitDepth(Image), RoundingType_Up); Byte++) {
                                float   PreAverage = (ImageArray[StereoView * Height * Width * (Byte - Bits2Bytes(ImageContainer_GetBitDepth(Image), RoundingType_Up))] + ImageArray[StereoView * Height * (Width - 1) * Byte]) / 2;
                                uint8_t Average    = FloorF(PreAverage) % 256;
                                
                                ImageArray[StereoView * Height * Width * Byte] = Average;
                            }
                        }
                    }
                }
            } else {
                uint16_t *ImageArray = (uint16_t*) ImageContainer_GetArray(Image);
                
                for (uint8_t StereoView = 0; StereoView < NumViews; StereoView++) {
                    for (uint32_t Height = 0UL; Height < ImageContainer_GetHeight(Image); Height++) {
                        for (uint32_t Width = 0UL; Width < ImageContainer_GetWidth(Image); Width++) {
                            for (uint32_t Byte = 0UL; Byte < Bits2Bytes(ImageContainer_GetBitDepth(Image), RoundingType_Up); Byte++) {
                                float   PreAverage = (ImageArray[StereoView * Height * Width * (Byte - Bits2Bytes(ImageContainer_GetBitDepth(Image), true))] + ImageArray[StereoView * Height * (Width - 1) * Byte]) / 2;
                                uint16_t Average   = FloorF(PreAverage) % 65536;
                                
                                ImageArray[StereoView * Height * Width * Byte] = Average;
                            }
                        }
                    }
                }
            }
        } else if (Image == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        }
    }
    
    void PNG_Filter_Paeth(ImageContainer *Image) {
        if (Image != NULL) {
            MediaIO_ImageTypes Type = ImageContainer_GetType(Image);
            ImageChannelMap *Map = ImageContainer_GetChannelMap(Image);
            uint8_t     NumViews = ImageChannelMap_GetNumViews(Map);
            if (Type == ImageType_Integer8) {
                uint8_t  *ImageArray = (uint8_t*)  ImageContainer_GetArray(Image);
                
                for (uint8_t StereoView = 0; StereoView < NumViews; StereoView++) {
                    for (uint32_t Height = 0UL; Height < ImageContainer_GetHeight(Image); Height++) {
                        for (uint32_t Width = 0UL; Width < ImageContainer_GetWidth(Image); Width++) {
                            for (uint32_t Byte = 0UL; Byte < Bits2Bytes(ImageContainer_GetBitDepth(Image), RoundingType_Up); Byte++) {
                                uint8_t Current = (ImageArray[StereoView * Height * Width * Byte]);
                                
                                uint8_t Left    = ImageArray[StereoView * Height * Width * (Byte - 1)];
                                uint8_t Above   = ImageArray[StereoView * Height * Width * Byte];
                                uint8_t UpLeft  = ImageArray[StereoView * (Height - 1) * Width * (Byte - 1)];
                                uint8_t Paeth   = PaethPredictor(Left, Above, UpLeft);
                                
                                ImageArray[StereoView * Height * Width * Byte] = (Current - Paeth) % 256;
                            }
                        }
                    }
                }
            } else {
                uint16_t *ImageArray = (uint16_t*) ImageContainer_GetArray(Image);
                
                for (uint8_t StereoView = 0; StereoView < NumViews; StereoView++) {
                    for (uint32_t Height = 0UL; Height < ImageContainer_GetHeight(Image); Height++) {
                        for (uint32_t Width = 0UL; Width < ImageContainer_GetWidth(Image); Width++) {
                            for (uint32_t Byte = 0UL; Byte < Bits2Bytes(ImageContainer_GetBitDepth(Image), RoundingType_Up); Byte++) {
                                uint16_t Current = (ImageArray[StereoView * Height * Width * Byte]);
                                
                                uint16_t Left    = ImageArray[StereoView * Height * Width * (Byte - 1)];
                                uint16_t Above   = ImageArray[StereoView * Height * Width * Byte];
                                uint16_t UpLeft  = ImageArray[StereoView * (Height - 1) * Width * (Byte - 1)];
                                uint16_t Paeth   = PaethPredictor(Left, Above, UpLeft);
                                
                                ImageArray[StereoView * Height * Width * Byte] = (Current - Paeth) % 65536;
                            }
                        }
                    }
                }
            }
        } else if (Image == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        }
    }
    
    // These filters need to be operatable on every line in any order, so we need a main PNG_Defilter function.
    // That function then needs to read the first byte of each line to see what filter is used
    // then OVIA each line.
    // ALSO keep in mind concurrency.
    
    void PNG_Defilter(ImageContainer *Image) {
        if (Image != NULL) {
            MediaIO_ImageTypes Type = ImageContainer_GetType(Image);
            if (Type == ImageType_Integer8) {
                // Image8
                uint8_t  ****ImageArray = (uint8_t****) ImageContainer_GetArray(Image);
                
                for (size_t ScanLine = 0; ScanLine < ImageContainer_GetWidth(Image); ScanLine++) {
                    switch (ImageArray[0][ScanLine][0][0]) {
                        case NotFiltered:
                            // copy the Line except byte 0 (the filter indication byte) to the output buffer.
                            // With the ImageContainer framework the way it is, this is only possible at the end.
                            break;
                        case SubFilter:
                            // SubFilter
                            PNG_Filter_Sub(Image);
                            break;
                        case UpFilter:
                            // UpFilter
                            PNG_Filter_Up(Image);
                            break;
                        case AverageFilter:
                            // AverageFilter
                            PNG_Filter_Average(Image);
                            break;
                        case PaethFilter:
                            // PaethFilter
                            PNG_Filter_Paeth(Image);
                            break;
                        default:
                            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Filter type: %d is invalid"), ImageArray[0][ScanLine][0][0]);
                            break;
                    }
                }
            } else {
                // Image16
                uint16_t ****ImageArray = (uint16_t****) ImageContainer_GetArray(Image);
                
                for (size_t ScanLine = 0; ScanLine < ImageContainer_GetWidth(Image); ScanLine++) {
                    switch (ImageArray[0][ScanLine][0][0]) {
                        case NotFiltered:
                            // copy the Line except byte 0 (the filter indication byte) to the output buffer.
                            break;
                        case SubFilter:
                            PNG_Filter_Sub(Image);
                            break;
                        case UpFilter:
                            PNG_Filter_Up(Image);
                            break;
                        case AverageFilter:
                            PNG_Filter_Average(Image);
                            break;
                        case PaethFilter:
                            PNG_Filter_Paeth(Image);
                            break;
                        default:
                            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Filter type: %d is invalid"), ImageArray[0][ScanLine][0][0]);
                            break;
                    }
                }
            }
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        }
    }
    
    void PNG_DAT_Read(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG         = Options;
            bool     Is3D           = PNG->sTER->StereoType > 0 ? Yes : No;
            uint8_t  BitDepth       = PNG->iHDR->BitDepth;
            uint8_t  ColorType      = PNG->iHDR->ColorType;
            uint8_t  NumChannels    = PNG_NumChannelsPerColorType[ColorType];
            uint64_t Width          = PNG->iHDR->Width;
            uint64_t Height         = PNG->iHDR->Height;
            ImageContainer *Decoded = NULL;
            MediaIO_ImageChannelMask  Mask  = 0;
            
            PNG_Flate_ReadZlibHeader(PNG, BitB);
            
            if (Is3D == true) {
                Mask += ImageMask_3D_L;
                Mask += ImageMask_3D_R;
            }
            
            if (ColorType == PNG_Grayscale) {
                Mask += ImageMask_Luma;
            } else if (ColorType == PNG_GrayAlpha) {
                Mask += ImageMask_Luma;
                Mask += ImageMask_Alpha;
            } else if (ColorType == PNG_RGB || ColorType == PNG_PalettedRGB) {
                Mask += ImageMask_Red;
                Mask += ImageMask_Green;
                Mask += ImageMask_Blue;
            } else if (ColorType == PNG_RGBA) {
                Mask += ImageMask_Red;
                Mask += ImageMask_Green;
                Mask += ImageMask_Blue;
                Mask += ImageMask_Alpha;
            }
            
            if (BitDepth <= 8) {
                Decoded = ImageContainer_Init(ImageType_Integer8, Mask, Width, Height);
            } else if (BitDepth <= 16) {
                Decoded = ImageContainer_Init(ImageType_Integer16, Mask, Width, Height);
            } else {
                Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitDepth %d is invalid"), BitDepth);
            }
            
            PNG_DAT_Decode(PNG, BitB, Decoded);
            PNG_Flate_ReadDeflateBlock(PNG, BitB, Decoded);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_Adam7_Deinterlace(ImageContainer *Image) {
        if (Image != NULL) {
            
        } else if (Image == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        }
    }
    
    void PNG_IHDR_Read(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG           = Options;
            PNG->iHDR->Width          = (uint32_t) BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 32);
            PNG->iHDR->Height         = (uint32_t) BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 32);
            PNG->iHDR->BitDepth       = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8);
            PNG->iHDR->ColorType      = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8);
            if (PNG->iHDR->ColorType == 1 || PNG->iHDR->ColorType == 5 || PNG->iHDR->ColorType >= 7) {
                Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Invalid color type: %d"), PNG->iHDR->ColorType);
            }
            PNG->iHDR->Compression    = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8);
            PNG->iHDR->FilterMethod   = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8);
            PNG->iHDR->Progressive    = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_PLTE_Read(void *Options, BitBuffer *BitB, uint32_t ChunkSize) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG  = Options;
            uint8_t BitDepth = PNG->iHDR->BitDepth;
            if (BitDepth <= 8) {
                uint8_t ColorType = PNG->iHDR->ColorType;
                if (ColorType == PNG_PalettedRGB || ColorType == PNG_RGB) {
                    //PNG_PLTE_Init(Ovia);
                    for (uint32_t Entry = 0UL; Entry < ChunkSize / 3; Entry++) {
                        PNG->PLTE->Palette[0] = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_LSByte, BitIO_BitOrder_LSBit, 8);
                        PNG->PLTE->Palette[1] = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_LSByte, BitIO_BitOrder_LSBit, 8);
                        PNG->PLTE->Palette[2] = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_LSByte, BitIO_BitOrder_LSBit, 8);
                    }
                } else if (ColorType == PNG_RGBA) {
                    for (uint32_t Entry = 0UL; Entry < ChunkSize / 3; Entry++) {
                        PNG->PLTE->Palette[0] = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_LSByte, BitIO_BitOrder_LSBit, 8);
                        PNG->PLTE->Palette[1] = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_LSByte, BitIO_BitOrder_LSBit, 8);
                        PNG->PLTE->Palette[2] = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_LSByte, BitIO_BitOrder_LSBit, 8);
                        PNG->PLTE->Palette[3] = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_LSByte, BitIO_BitOrder_LSBit, 8);
                    }
                }
            } else {
                BitBuffer_Seek(BitB, Bytes2Bits(ChunkSize));
                Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitDepth %d can't have a palette"), BitDepth);
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_TRNS_Read(void *Options, BitBuffer *BitB, uint32_t ChunkSize) { // Transparency
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG       = Options;
            uint32_t   NumEntries = ChunkSize % 3;
            uint16_t  *Entries    = NULL;
            if (PNG->iHDR->ColorType == PNG_RGB) {
                Entries = calloc(3, Bits2Bytes(PNG->iHDR->BitDepth, RoundingType_Up) * sizeof(uint16_t));
            } else if (PNG->iHDR->ColorType == PNG_RGBA) {
                Entries = calloc(4, Bits2Bytes(PNG->iHDR->BitDepth, RoundingType_Up) * sizeof(uint16_t));
            } else if (PNG->iHDR->ColorType == PNG_Grayscale) {
                Entries = calloc(1, Bits2Bytes(PNG->iHDR->BitDepth, RoundingType_Up) * sizeof(uint16_t));
            } else if (PNG->iHDR->ColorType == PNG_GrayAlpha) {
                Entries = calloc(2, Bits2Bytes(PNG->iHDR->BitDepth, RoundingType_Up) * sizeof(uint16_t));
            }
            if (Entries != NULL) {
                for (uint8_t Color = 0; Color < PNG_NumChannelsPerColorType[PNG->iHDR->ColorType]; Color++) {
                    Entries[Color]    = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, Bits2Bytes(PNG->iHDR->BitDepth, RoundingType_Up));
                }
                //Ovia->tRNS->Palette = Entries;
            } else {
                Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Failed to allocate enough memory for the TRNS Palette"));
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (ChunkSize % 3 != 0) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("The ChunkSize MUST be divisible by 3"));
        }
    }
    
    void PNG_BKGD_Read(void *Options, BitBuffer *BitB) { // Background
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG = Options;
            for (uint8_t Entry = 0; Entry < PNG_NumChannelsPerColorType[PNG->iHDR->ColorType]; Entry++) {
                PNG->bkGD->BackgroundPaletteEntry[Entry] = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, PNG->iHDR->BitDepth);
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_CHRM_Read(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG        = Options;
            PNG->cHRM->WhitePointX = (uint32_t) BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 32);
            PNG->cHRM->WhitePointY = (uint32_t) BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 32);
            PNG->cHRM->RedX        = (uint32_t) BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 32);
            PNG->cHRM->RedY        = (uint32_t) BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 32);
            PNG->cHRM->GreenX      = (uint32_t) BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 32);
            PNG->cHRM->GreenY      = (uint32_t) BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 32);
            PNG->cHRM->BlueX       = (uint32_t) BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 32);
            PNG->cHRM->BlueY       = (uint32_t) BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 32);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_GAMA_Read(void *Options, BitBuffer *BitB) { // Gamma
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG  = Options;
            PNG->gAMA->Gamma = (uint32_t) BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 32);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_OFFS_Read(void *Options, BitBuffer *BitB) { // Image Offset
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG          = Options;
            PNG->oFFs->XOffset       = (uint32_t) BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 32);
            PNG->oFFs->YOffset       = (uint32_t) BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 32);
            PNG->oFFs->UnitSpecifier = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_PHYS_Read(void *Options, BitBuffer *BitB) { // Aspect ratio, Physical pixel size
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG               = Options;
            PNG->pHYs->PixelsPerUnitXAxis = (uint32_t) BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 32);
            PNG->pHYs->PixelsPerUnitYAxis = (uint32_t) BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 32);
            PNG->pHYs->UnitSpecifier      = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_SCAL_Read(void *Options, BitBuffer *BitB) { // Physical Scale
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG           = Options;
            PNG->sCAL->UnitSpecifier  = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8); // 1 = Meter, 2 = Radian
            
            uint32_t WidthStringSize  = (uint32_t) BitBuffer_GetUTF8StringSize(BitB);
            uint32_t HeightStringSize = (uint32_t) BitBuffer_GetUTF8StringSize(BitB);
            
            UTF8 *WidthString         = BitBuffer_ReadUTF8(BitB, WidthStringSize);
            UTF8 *HeightString        = BitBuffer_ReadUTF8(BitB, HeightStringSize);
            
            PNG->sCAL->PixelWidth     = UTF8_String2Decimal(WidthString, Base_Decimal | Base_Radix10);
            PNG->sCAL->PixelHeight    = UTF8_String2Decimal(HeightString, Base_Decimal | Base_Radix10);
            
            free(WidthString);
            free(HeightString);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_PCAL_Read(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG                      = Options;
            uint8_t CalibrationSize              = BitBuffer_GetUTF8StringSize(BitB);
            PNG->pCAL->CalibrationName           = BitBuffer_ReadUTF8(BitB, CalibrationSize);
            PNG->pCAL->OriginalZero              = (uint32_t) BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 32);
            PNG->pCAL->OriginalMax               = (uint32_t) BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 32);
            PNG->pCAL->EquationType              = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8);
            PNG->pCAL->NumParams                 = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8);
            uint8_t UnitNameSize                 = BitBuffer_GetUTF8StringSize(BitB);
            PNG->pCAL->UnitName                  = BitBuffer_ReadUTF8(BitB, UnitNameSize);
            BitBuffer_Seek(BitB, 8); // NULL seperator
            PNG->pCAL->Parameters                = calloc(PNG->pCAL->NumParams, sizeof(double));
            if (PNG->pCAL->Parameters != NULL) {
                for (uint8_t Param = 0; Param < PNG->pCAL->NumParams; Param++) {
                    uint8_t ParameterSize        = BitBuffer_GetUTF8StringSize(BitB);
                    UTF8   *ParameterString      = BitBuffer_ReadUTF8(BitB, ParameterSize);
                    PNG->pCAL->Parameters[Param] = UTF8_String2Decimal(ParameterString, Base_Decimal | Base_Radix10);
                }
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_SBIT_Read(void *Options, BitBuffer *BitB) { // Significant bits per sample
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG          = Options;
            PNG_ColorTypes ColorType = PNG->iHDR->ColorType;
            if (ColorType == PNG_Grayscale) {
                PNG->sBIT->Grayscale = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8);
            } else if (ColorType == PNG_RGB || ColorType == PNG_PalettedRGB) {
                PNG->sBIT->Red       = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8);
                PNG->sBIT->Green     = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8);
                PNG->sBIT->Blue      = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8);
            } else if (ColorType == PNG_GrayAlpha) {
                PNG->sBIT->Grayscale = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8);
                PNG->sBIT->Alpha     = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8);
            } else if (ColorType == PNG_RGBA) {
                PNG->sBIT->Red       = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8);
                PNG->sBIT->Green     = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8);
                PNG->sBIT->Blue      = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8);
                PNG->sBIT->Alpha     = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8);
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_SRGB_Read(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG            = Options;
            PNG->sRGB->RenderingIntent = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_STER_Read(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG       = Options;
            PNG->sTER->StereoType = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8);
            // No matter what StereoType is used, both images are arranged side by side, and the left edge is aligned on a boundary of the 8th column in case interlacing is used.
            // The two sub images must have the same dimensions after padding is removed.
            // CROSS_FUSE_LAYOUT = 0, DIVERGING_FUSE_LAYOUT = 1
            // When CROSS_FUSE_LAYOUT, the right image is on the left, and the left image on the right.
            // When DIVERGING_FUSE_LAYOUT, The left eye view is on the left, and the right eye view is on the right.
            // The Left eye view is always in index 0 of the pixel array, so we need to swap this if nessicary.
            // So, make a function that strips padding from the stream, then make a function that OVIAs the sTER image the way it should be.
            // The standard recommends we use CROSS_FUSE_LAYOUT, so I'll probably end up using that.
            // So, let's say we have 2 1080p images, and we want to make a single Stereoscopic PNG from them.
            //
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_TEXT_Read(void *Options, BitBuffer *BitB) { // Uncompressed, ASCII tEXt
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG = Options;
            for (uint32_t TextChunk = 0UL; TextChunk < PNG->NumTextChunks; TextChunk++) {
                if (PNG->Text[TextChunk]->TextType == tEXt) { // ASCII aka Latin-1
                    
                } else if (PNG->Text[TextChunk]->TextType == iTXt) { // Unicode, UTF-8
                    
                } else if (PNG->Text[TextChunk]->TextType == zTXt) { // Compressed
                    
                }
            }
            
            
            //Ovia->NumTextChunks  += 1;
            
            
            
            // first we need to get the size of the strings
            
            // for that we would theoretically use BitBuffer_PeekBits, but BitBuffer_PeekBits is limited to 64 bits read at once, max.
            // there can be up to 800 bits before we hit the NULL, so what do...
            
            // Well, we can use SeekBits to jump byte by byte but it's not optimal...
            
            
            
            // Ok, once we've gotten the size of the Keyword string
            
            
            // Then read the Comment string.
            
            // Now, how do we handle multiple text strings?
            // Why not just have a count of the text chunks?
            // Store a variable in OVIA called NumTextChunks, then store a type called Comment or something that stores both the comment type as a string and the actual comment...
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_TIME_Read(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG                     = Options;
            PNG->tIMe->Year                     = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 16);
            PNG->tIMe->Month                    = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8);
            PNG->tIMe->Day                      = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8);
            PNG->tIMe->Hour                     = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8);
            PNG->tIMe->Minute                   = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8);
            PNG->tIMe->Second                   = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    /* APNG */
    void PNG_ACTL_Read(void *Options, BitBuffer *BitB) { // Animation control, part of APNG
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG                  = Options;
            PNG->acTL->NumFrames             = (uint32_t) BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 32);
            PNG->acTL->TimesToLoop           = (uint32_t) BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 32); // If 0, loop forever.
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_FCTL_Read(void *Options, BitBuffer *BitB) { // Frame Control, part of APNG
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG                  = Options;
            PNG->fcTL->FrameNum              = (uint32_t) BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 32);
            PNG->fcTL->Width                 = (uint32_t) BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 32);
            PNG->fcTL->Height                = (uint32_t) BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 32);
            PNG->fcTL->XOffset               = (uint32_t) BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 32);
            PNG->fcTL->YOffset               = (uint32_t) BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 32);
            PNG->fcTL->FrameDelayNumerator   = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 16);
            PNG->fcTL->FrameDelayDenominator = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 16);
            PNG->fcTL->DisposeMethod         = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8);
            PNG->fcTL->BlendMethod           = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    /* End APNG */
    
    void PNG_HIST_Read(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG      = Options;
            PNG->hIST->NumColors = (uint32_t) BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 32);
            for (uint32_t Color = 0; Color < PNG->hIST->NumColors; Color++) {
                PNG->hIST->Histogram[Color] = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, PNG->iHDR->BitDepth);
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_ICCP_Read(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG         = Options;
            uint8_t ProfileNameSize = BitBuffer_GetUTF8StringSize(BitB);
            PNG->iCCP->ProfileName  = BitBuffer_ReadUTF8(BitB, ProfileNameSize);
            PNG->iCCP->CompressionType = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8);
            // Decompress the data with Zlib
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
        uint8_t ProfileNameSize = 0;
        ProfileNameSize = BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 8);
    }
    
    void PNG_SPLT_Read(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG                          = Options;
            PNG_ColorTypes ColorType                 = PNG->iHDR->ColorType;
            uint8_t BitDepthInBytes                  = Bits2Bytes(PNG->iHDR->BitDepth, RoundingType_Up);
            uint8_t PaletteNameSize                  = BitBuffer_GetUTF8StringSize(BitB);
            PNG->sPLT[PNG->NumSPLTChunks - 1]->Name  = BitBuffer_ReadUTF8(BitB, PaletteNameSize);
            PNG->sPLT[PNG->NumSPLTChunks - 1]->Red   = (uint16_t) BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, Bytes2Bits(BitDepthInBytes));
            PNG->sPLT[PNG->NumSPLTChunks - 1]->Green = (uint16_t) BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, Bytes2Bits(BitDepthInBytes));
            PNG->sPLT[PNG->NumSPLTChunks - 1]->Blue  = (uint16_t) BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, Bytes2Bits(BitDepthInBytes));
            if (ColorType == PNG_RGBA || ColorType == PNG_GrayAlpha) {
                PNG->sPLT[PNG->NumSPLTChunks - 1]->Alpha = (uint16_t) BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, Bytes2Bits(BitDepthInBytes));
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_ReadChunks(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG          = Options;
            ImageContainer *Image    = NULL;
            while (BitBuffer_GetSize(BitB) > 0) {
                uint32_t ChunkSize   = (uint32_t) BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 32);
                uint32_t ChunkID     = (uint32_t) BitBuffer_ReadBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 32);
                
                if (ChunkID == acTLMarker) {
                    PNG_ACTL_Read(PNG, BitB);
                } else if (ChunkID == bKGDMarker) {
                    PNG_BKGD_Read(PNG, BitB);
                } else if (ChunkID == cHRMMarker) {
                    PNG_CHRM_Read(PNG, BitB);
                } else if (ChunkID == fcTLMarker) {
                    PNG_FCTL_Read(PNG, BitB);
                } else if (ChunkID == gAMAMarker) {
                    PNG_GAMA_Read(PNG, BitB);
                } else if (ChunkID == hISTMarker) {
                    PNG_HIST_Read(PNG, BitB);
                } else if (ChunkID == iCCPMarker) {
                    PNG_OFFS_Read(PNG, BitB);
                } else if (ChunkID == IDATMarker || ChunkID == fDATMarker) {
                    PNG_DAT_Read(PNG, BitB);
                } else if (ChunkID == iHDRMarker) {
                    PNG_IHDR_Read(PNG, BitB);
                } else if (ChunkID == oFFsMarker) {
                    PNG_OFFS_Read(PNG, BitB);
                } else if (ChunkID == pCALMarker) {
                    PNG_PCAL_Read(PNG, BitB);
                } else if (ChunkID == pHYsMarker) {
                    PNG_PHYS_Read(PNG, BitB);
                } else if (ChunkID == PLTEMarker) {
                    PNG_PLTE_Read(PNG, BitB, ChunkSize);
                } else if (ChunkID == sBITMarker) {
                    PNG_SBIT_Read(PNG, BitB);
                } else if (ChunkID == sRGBMarker) {
                    PNG_SRGB_Read(PNG, BitB);
                } else if (ChunkID == sTERMarker) {
                    PNG_STER_Read(PNG, BitB);
                } else if (ChunkID == tEXtMarker) {
                    PNG->NumTextChunks += 1;
                    PNG->Text[PNG->NumTextChunks - 1]->TextType = tEXt;
                    PNG_TEXT_Read(PNG, BitB);
                } else if (ChunkID == zTXtMarker) {
                    PNG->NumTextChunks += 1;
                    PNG->Text[PNG->NumTextChunks - 1]->TextType = zTXt;
                    PNG_TEXT_Read(PNG, BitB);
                } else if (ChunkID == iTXtMarker) {
                    PNG->NumTextChunks += 1;
                    PNG->Text[PNG->NumTextChunks - 1]->TextType = iTXt;
                    PNG_TEXT_Read(PNG, BitB);
                } else if (ChunkID == sCALMarker) {
                    PNG_SCAL_Read(PNG, BitB);
                } else if (ChunkID == tIMEMarker) {
                    PNG_TIME_Read(PNG, BitB);
                } else if (ChunkID == tRNSMarker) {
                    PNG_TRNS_Read(PNG, BitB, ChunkSize);
                } else if (ChunkID == sPLTMarker) {
                    PNG_SPLT_Read(PNG, BitB);
                }
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNGDeinterlace(void *Options, ImageContainer *Image, BitBuffer *BitB) {
        
    }
    
    void *PNGExtractImage(void *Options, BitBuffer *BitB) {
        ImageContainer *Image = NULL;
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG = Options;
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
        return Image;
    }
    
    static const OVIADecoder PNGDecoder = {
        .Function_Initialize   = PNGOptions_Init,
        .Function_Decode       = PNGExtractImage,
        .Function_Read         = PNG_ReadChunks,
        .Function_Deinitialize = PNGOptions_Deinit,
        .MagicIDs              = &PNGMagicIDs,
        .MediaType             = MediaType_Image,
        .DecoderID             = CodecID_PNG,
    };
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
