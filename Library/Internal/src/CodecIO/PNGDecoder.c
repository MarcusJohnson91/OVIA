#include "../../include/Private/PNGCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void PNG_Flate_ReadZlibHeader(PNGOptions *PNG, BitBuffer *BitB) {
        if (PNG != NULL && BitB != NULL) {
            PNG->DAT->CMF  = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8);
            PNG->DAT->FLG  = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8);
            bool FDICT     = PNG->DAT->FLG & 0x4 >> 2;
            
            if (FDICT == Yes) {
                PNG->DAT->DictID = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 32);
            }
            
            uint8_t CompressionInfo   = PNG->DAT->CMF & 0xF0 >> 4;
            uint8_t CompressionMethod = PNG->DAT->CMF & 0x0F;
            
            if (CompressionInfo == 7 && CompressionMethod == 8) {
                uint16_t Check = CompressionInfo << 12;
                Check         |= CompressionMethod << 8;
                Check         |= (PNG->DAT->FLG & 0x3) << 6;
                Check         |= FDICT << 5;
                Check         |= PNG->DAT->FLG & 0xF8;
                if (Check % 31 != 0) {
                    Log(Log_DEBUG, __func__, U8("Invalid Flate Header %d"), Check);
                }
            } else if (CompressionInfo != 7) {
                Log(Log_DEBUG, __func__, U8("Compresion Info %d is invalid"), CompressionInfo);
            } else if (CompressionMethod != 8) {
                Log(Log_DEBUG, __func__, U8("Compression Method %d is invalid"), CompressionMethod);
            }
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_Flate_ReadLiteralBlock(PNGOptions *PNG, BitBuffer *BitB) {
        if (PNG != NULL && BitB != NULL) {
            BitBuffer_Align(BitB, 1); // Skip the remaining 5 bits
            uint16_t Bytes2Copy    = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16); // 0x4F42 = 20,290
            uint16_t Bytes2CopyXOR = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 16) ^ 0xFFFF; // 0xB0BD = 0x4F42
            
            if (Bytes2Copy == Bytes2CopyXOR) {
                for (uint16_t Byte = 0ULL; Byte < Bytes2Copy; Byte++) {
                    Array[Byte]    = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 8);
                }
            } else {
                Log(Log_DEBUG, __func__, U8("Data Error: Bytes2Copy does not match Bytes2CopyXOR in literal block"));
            }
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    HuffmanTree *PNG_Flate_BuildTree(PNGOptions *PNG, BitBuffer *BitB, HuffmanTreeTypes TreeType) {
        HuffmanTree *Tree = NULL;
        if (BitB != NULL && TreeType != TreeType_Unknown) {
            Tree          = calloc(1, sizeof(HuffmanTree));
            if (Tree != NULL) {
                if (TreeType == TreeType_Fixed) {
                    Tree->LengthTable          = HuffmanBuildTree(288, FixedLiteralTable);
                    Tree->DistanceTable        = HuffmanBuildTree(32, FixedDistanceTable);
                    
                    for (uint64_t Index = 0ULL; Index < 288; Index++) {
                        uint32_t Length        = 0;
                        uint32_t Value         = ReadHuffman(Tree->LengthTable, BitB);
                        if (Value < 16) {
                            FixedLiteralTable[Index + 1] = Value;
                        } else {
                            if (Value == 16) {
                                Length         = FixedLiteralTable[Index - 1];
                                Value          = 3  + BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 2);
                            } else if (Value == 17) {
                                Value          = 3  + BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 3);
                            } else {
                                Value          = 11 + BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 7);
                            }
                            for (uint32_t Times2Copy = 0ULL; Times2Copy < Length; Times2Copy++) {
                                // What is this?
                            }
                            for (uint32_t Index = Value; Index > 0; Index--) {
                                FixedLiteralTable[Index + 1] = Length;
                            }
                        }
                    }
                } else if (TreeType == TreeType_Dynamic) {
                    uint16_t NumLengthSymbols   = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 5) + 257;
                    uint8_t  NumDistanceSymbols = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 5) + 1;
                    uint8_t  NumMetaCodeLengths = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 4) + 4;
                    
                    uint16_t MetaTable[NumMetaCodes];
                    for (uint8_t MetaCode = 0; MetaCode < NumMetaCodeLengths; MetaCode++) {
                        MetaTable[MetaCodeLengthOrder[MetaCode]] = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 3);
                    }
                    HuffmanTree *MetaTree         = HuffmanBuildTree(19, MetaTable);
                    uint16_t RealLengths[318]      = {0};
                    for (uint64_t Index = 0ULL; Index < NumLengthSymbols + NumDistanceSymbols; Index++) {
                        uint64_t Value             = ReadHuffman(MetaTree, BitB);
                        if (Value <= 15) {
                            RealLengths[Index + 1] = Value;
                        } else {
                            uint16_t Times2Repeat  = 0;
                            if (Value == 16) {
                                Times2Repeat       = RealLengths[Index - 1];
                                Value              = 3  + BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 2);
                            } else if (Value == 17) {
                                Value              = 3  + BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 3);
                            } else {
                                Value              = 11 + BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 7);
                            }
                            for (uint16_t Index2 = Value; Index2 > 0; Index2--) {
                                RealLengths[Index2 + 1] = Times2Repeat;
                            }
                        }
                    }
                    Tree->LengthTable   = HuffmanBuildTree(NumLengthSymbols, RealLengths);
                    Tree->DistanceTable = HuffmanBuildTree(NumLengthSymbols + NumDistanceSymbols, RealLengths); // Change to Distance table
                } else {
                    Log(Log_DEBUG, __func__, U8("Invalid Deflate tyoe"));
                }
            } else {
                Log(Log_DEBUG, __func__, U8("Couldn't allocate HuffmanTree"));
            }
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (TreeType == TreeType_Unknown) {
            Log(Log_DEBUG, __func__, U8("TreeType Unknown is invalid"));
        }
        return Tree;
    }
    
    void PNG_DAT_Decode(PNGOptions *PNG, BitBuffer *BitB, ImageContainer *Image) {
        if (BitB != NULL && Image != NULL) {
            uint8_t *ImageArrayBytes               = (uint8_t*) ImageContainer_GetArray(Image);
            PNG_DAT_SetArray(Ovia, ImageArrayBytes);
            bool     IsFinalBlock                  = false;
            
            do {
                HuffmanTree *Tree                  = NULL;
                IsFinalBlock                       = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 1); // 0
                uint8_t BlockType                  = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 2); // 0b00 = 0
                if (BlockType == BlockType_Literal) {
                    PNG_Flate_ReadLiteralBlock(PNG, BitB);
                } else if (BlockType == BlockType_Fixed) {
                    Tree = PNG_Flate_BuildTree(BitB, TreeType_Fixed);
                } else if (BlockType == BlockType_Dynamic) {
                    Tree = PNG_Flate_BuildTree(BitB, TreeType_Dynamic);
                } else if (BlockType == BlockType_Invalid) {
                    Log(Log_DEBUG, __func__, U8("Invalid Block"));
                }
                PNG_Flate_Decode(PNG, BitB); // Actually read the data
            } while (IsFinalBlock == false);
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (Image == NULL) {
            Log(Log_DEBUG, __func__, U8("ImageContainer Pointer is NULL"));
        }
    }
    
    uint64_t ReadHuffman(PNGOptions *PNG, HuffmanTree *Tree, BitBuffer *BitB) { // EQUILIVENT OF DECODE IN PUFF
        uint64_t Symbol = 0ULL;
        if (Tree != NULL && BitB != NULL) {
            uint32_t FirstSymbolOfLength = 0;
            for (uint8_t Bit = 1; Bit <= MaxBitsPerSymbol; Bit++) {
                Symbol <<= 1;
                Symbol  |= BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 1);
                if (Symbol - Tree->Frequency[Bit] < FirstSymbolOfLength) {
                    Symbol = Tree->Symbols[Bit + (Symbol - FirstSymbolOfLength)];
                }
            }
        } else if (Tree == NULL) {
            Log(Log_DEBUG, __func__, U8("HuffmanTree Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
        return Symbol;
    }
    
    uint16_t PNG_Flate_ReadSymbol(PNGOptions *PNG, HuffmanTree *Tree, BitBuffer *BitB) {
        uint16_t Symbol                        = 0;
        if (Tree != NULL && BitB != NULL) {
            Symbol                             = ReadHuffman(Tree, BitB);
            /*
             do {
             if (Symbol < 256) {
             Array[Offset]              = Symbol;
             Offset                    += 1;
             } else if (Symbol > 256) {
             uint16_t BaseLength        = LengthBase[Symbol - 257];
             uint16_t ExtensionLength   = LengthAdditionalBits[Symbol - 257];
             uint16_t LengthCode        = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, ExtensionLength) + BaseLength;
             
             uint16_t DistanceSymbol    = ReadHuffman(Tree->DistanceTable, BitB);
             uint16_t BaseDistance      = DistanceBase[DistanceSymbol];
             uint16_t ExtensionDistance = DistanceAdditionalBits[DistanceSymbol];
             
             uint16_t DistanceCode      = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, ExtensionDistance) + BaseDistance;
             
             for (uint64_t Start = Offset - LengthCode; Start < Offset + DistanceCode; Start++) {
             Array[Start] = Array[Start - DistanceCode];
             }
             Offset += LengthCode;
             }
             } while (Symbol != 256);
             */
        } else if (Tree == NULL) {
            Log(Log_DEBUG, __func__, U8("HuffmanTree Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
        return Symbol;
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
            Image_Types Type = ImageContainer_GetType(Image);
            if (Type == ImageType_Integer8) {
                uint8_t  *ImageArray = (uint8_t*)  ImageContainer_GetArray(Image);
                
                for (uint8_t StereoView = 0; StereoView < ImageContainer_GetNumViews(Image); StereoView++) {
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
                
                for (uint8_t StereoView = 0; StereoView < ImageContainer_GetNumViews(Image); StereoView++) {
                    for (uint32_t Height = 0UL; Height < ImageContainer_GetHeight(Image); Height++) {
                        for (uint32_t Width = 0UL; Width < ImageContainer_GetWidth(Image); Width++) {
                            for (uint32_t Byte = 0UL; Byte < Bits2Bytes(ImageContainer_GetBitDepth(Image), RoundingType_Up); Byte++) {
                                ImageArray[StereoView * Height * Width * Byte] = (ImageArray[StereoView * Height * Width * Byte] + ImageArray[StereoView * Height * Width * (Byte + 1)]) % 65536;
                            }
                        }
                    }
                }
            }
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (Image == NULL) {
            Log(Log_DEBUG, __func__, U8("ImageContainer Pointer is NULL"));
        }
    }
    
    void PNG_Filter_Up(ImageContainer *Image) {
        if (PNG != NULL && Image != NULL) {
            Image_Types Type = ImageContainer_GetType(Image);
            if (Type == ImageType_Integer8) {
                uint8_t  *ImageArray = (uint8_t*)  ImageContainer_GetArray(Image);
                
                for (uint8_t StereoView = 0; StereoView < ImageContainer_GetNumViews(Image); StereoView++) {
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
                
                for (uint8_t StereoView = 0; StereoView < ImageContainer_GetNumViews(Image); StereoView++) {
                    for (uint32_t Height = 0UL; Height < ImageContainer_GetHeight(Image); Height++) {
                        for (uint32_t Width = 0UL; Width < ImageContainer_GetWidth(Image); Width++) {
                            for (uint32_t Byte = 0UL; Byte < Bits2Bytes(ImageContainer_GetBitDepth(Image), RoundingType_Up); Byte++) {
                                ImageArray[StereoView * Height * Width * Byte] = (ImageArray[StereoView * Height * Width * Byte] + ImageArray[StereoView * (Height - 1) * Width * Byte]) % 65536;
                            }
                        }
                    }
                }
            }
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (Image == NULL) {
            Log(Log_DEBUG, __func__, U8("ImageContainer Pointer is NULL"));
        }
    }
    
    void PNG_Filter_Average(ImageContainer *Image) {
        if (PNG != NULL && Image != NULL) {
            Image_Types Type = ImageContainer_GetType(Image);
            if (Type == ImageType_Integer8) {
                uint8_t  *ImageArray = (uint8_t*)  ImageContainer_GetArray(Image);
                
                for (uint8_t StereoView = 0; StereoView < ImageContainer_GetNumViews(Image); StereoView++) {
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
                
                for (uint8_t StereoView = 0; StereoView < ImageContainer_GetNumViews(Image); StereoView++) {
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
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (Image == NULL) {
            Log(Log_DEBUG, __func__, U8("ImageContainer Pointer is NULL"));
        }
    }
    
    void PNG_Filter_Paeth(ImageContainer *Image) {
        if (PNG != NULL && Image != NULL) {
            Image_Types Type = ImageContainer_GetType(Image);
            if (Type == ImageType_Integer8) {
                uint8_t  *ImageArray = (uint8_t*)  ImageContainer_GetArray(Image);
                
                for (uint8_t StereoView = 0; StereoView < ImageContainer_GetNumViews(Image); StereoView++) {
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
                
                for (uint8_t StereoView = 0; StereoView < ImageContainer_GetNumViews(Image); StereoView++) {
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
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (Image == NULL) {
            Log(Log_DEBUG, __func__, U8("ImageContainer Pointer is NULL"));
        }
    }
    
    // These filters need to be operatable on every line in any order, so we need a main PNG_Defilter function.
    // That function then needs to read the first byte of each line to see what filter is used
    // then OVIA each line.
    // ALSO keep in mind concurrency.
    
    void PNG_Defilter(ImageContainer *Image) {
        if (PNG != NULL && Image != NULL) {
            Image_Types Type = ImageContainer_GetType(Image);
            if (Type == ImageType_Integer8) {
                // Image8
                uint8_t  *ImageArray = (uint8_t*) ImageContainer_GetArray(Image);
                
                for (size_t ScanLine = 0; ScanLine < ImageContainer_GetWidth(Image); ScanLine++) {
                    switch (ImageArray[ScanLine]) {
                        case NotFiltered:
                            // copy the Line except byte 0 (the filter indication byte) to the output buffer.
                            // With the ImageContainer framework the way it is, this is only possible at the end.
                            break;
                        case SubFilter:
                            // SubFilter
                            PNG_Filter_Sub(Ovia, Image);
                            break;
                        case UpFilter:
                            // UpFilter
                            PNG_Filter_Up(Ovia, Image);
                            break;
                        case AverageFilter:
                            // AverageFilter
                            PNG_Filter_Average(Ovia, Image);
                            break;
                        case PaethFilter:
                            // PaethFilter
                            PNG_Filter_Paeth(Ovia, Image);
                            break;
                        default:
                            Log(Log_DEBUG, __func__, U8("Filter type: %d is invalid"), ImageArray[ScanLine]);
                            break;
                    }
                }
            } else {
                // Image16
                uint16_t *ImageArray = (uint16_t*) ImageContainer_GetArray(Image);
                
                for (size_t ScanLine = 0; ScanLine < ImageContainer_GetWidth(Image); ScanLine++) {
                    switch (ImageArray[ScanLine]) {
                        case NotFiltered:
                            // copy the Line except byte 0 (the filter indication byte) to the output buffer.
                            break;
                        case SubFilter:
                            PNG_Filter_Sub(Ovia, Image);
                            break;
                        case UpFilter:
                            PNG_Filter_Up(Ovia, Image);
                            break;
                        case AverageFilter:
                            PNG_Filter_Average(Ovia, Image);
                            break;
                        case PaethFilter:
                            PNG_Filter_Paeth(Ovia, Image);
                            break;
                        default:
                            Log(Log_DEBUG, __func__, U8("Filter type: %d is invalid"), ImageArray[ScanLine]);
                            break;
                    }
                }
            }
        } else {
            Log(Log_DEBUG, __func__, U8("ImageContainer Pointer is NULL"));
        }
    }
    
    enum DEFLATEBlockTypes {
        UncompressedBlock   = 0,
        FixedHuffmanBlock   = 1,
        DynamicHuffmanBlock = 2,
    };
    
    static const uint8_t PNG_NumChannelsPerColorType[7] = {
        1, 0, 3, 3, 4, 0, 4
    };
    
    static const UTF8 PNG_MonthMap[12][4] = {
        u8"Jan", u8"Feb", u8"Mar", u8"Apr", u8"May", u8"Jun",
        u8"Jul", u8"Aug", u8"Sep", u8"Oct", u8"Nov", u8"Dec",
    };
    
    void PNG_DAT_Parse(BitBuffer *BitB, uint32_t DATSize) {
        if (PNG != NULL && BitB != NULL) {
            bool     Is3D           = PNG_STER_GetSterType(Ovia);
            uint8_t  BitDepth       = ImageContainer_GetBitDepth(Image);
            uint8_t  ColorType      = PNG_iHDR_GetColorType(Ovia);
            uint8_t  NumChannels    = PNG_NumChannelsPerColorType[ColorType];
            uint64_t Width          = ImageContainer_GetWidth(Image);
            uint64_t Height         = OVIA_GetHeight(Ovia);
            ImageContainer *Decoded = NULL;
            Image_ChannelMask Mask  = 0;
            
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
                Log(Log_DEBUG, __func__, U8("BitDepth %d is invalid"), BitDepth);
            }
            
            PNG_DAT_Decode(PNG, BitB, Decoded);
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_Adam7_Deinterlace(ImageContainer *Image) {
        if (PNG != NULL && Image != NULL) {
            
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (Image == NULL) {
            Log(Log_DEBUG, __func__, U8("ImageContainer Pointer is NULL"));
        }
    }
    
    void PNG_IHDR_Parse(BitBuffer *BitB, uint32_t ChunkSize) {
        if (PNG != NULL && BitB != NULL) {
            uint32_t Width          = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 32);
            uint32_t Height         = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 32);
            uint8_t  BitDepth       = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8);
            uint8_t  ColorType      = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8);
            if (ColorType == 1 || ColorType == 5 || ColorType >= 7) {
                Log(Log_DEBUG, __func__, U8("Invalid color type: %d"), ColorType);
            }
            uint8_t Compression    = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8);
            uint8_t FilterMethod   = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8);
            uint8_t Progressive    = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8);
            
            PNG_IHDR_SetIHDR(Ovia, Height, Width, BitDepth, ColorType, Progressive);
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_PLTE_Parse(BitBuffer *BitB, uint32_t ChunkSize) {
        if (PNG != NULL && BitB != NULL && ChunkSize % 3 == 0) {
            uint8_t BitDepth = ImageContainer_GetBitDepth(Image);
            if (BitDepth <= 8) {
                uint8_t ColorType = PNG_GetColorType(Ovia);
                if (ColorType == PNG_PalettedRGB || ColorType == PNG_RGB) {
                    PNG_PLTE_Init(Ovia, ChunkSize / 3);
                    for (uint32_t Entry = 0UL; Entry < ChunkSize / 3; Entry++) {
                        uint8_t Red   = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 8);
                        uint8_t Green = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 8);
                        uint8_t Blue  = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 8);
                        PNG_PLTE_SetPalette(Ovia, Entry, Red, Green, Blue);
                    }
                } else if (ColorType == PNG_RGBA) {
                    for (uint32_t Entry = 0UL; Entry < ChunkSize / 3; Entry++) {
                        uint8_t Red   = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 8);
                        uint8_t Green = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 8);
                        uint8_t Blue  = BitBuffer_ReadBits(BitB, LSByteFirst, LSBitFirst, 8);
                        PNG_PLTE_SetPalette(Ovia, Entry, Red, Green, Blue);
                    }
                }
            } else {
                BitBuffer_Seek(BitB, Bytes2Bits(ChunkSize));
                Log(Log_DEBUG, __func__, U8("BitDepth %d can't have a palette"), BitDepth);
            }
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (ChunkSize % 3 != 0) {
            Log(Log_DEBUG, __func__, U8("The ChunkSize MUST be divisible by 3"));
        }
    }
    
    void PNG_TRNS_Parse(BitBuffer *BitB, uint32_t ChunkSize) { // Transparency
        if (PNG != NULL && BitB != NULL && ChunkSize  % 3 == 0) {
            uint32_t NumEntries    = ChunkSize % 3;
            uint16_t **Entries    = NULL;
            if (PNG_GetColorType(Ovia) == PNG_RGB) {
                Entries = calloc(3, Bits2Bytes(ImageContainer_GetBitDepth(Image), RoundingType_Up) * sizeof(uint16_t));
            } else if (PNG_GetColorType(Ovia) == PNG_RGBA) {
                Entries = calloc(4, Bits2Bytes(ImageContainer_GetBitDepth(Image), RoundingType_Up) * sizeof(uint16_t));
            } else if (PNG_GetColorType(Ovia) == PNG_Grayscale) {
                Entries = calloc(1, Bits2Bytes(ImageContainer_GetBitDepth(Image), RoundingType_Up) * sizeof(uint16_t));
            } else if (PNG_GetColorType(Ovia) == PNG_GrayAlpha) {
                Entries = calloc(2, Bits2Bytes(ImageContainer_GetBitDepth(Image), RoundingType_Up) * sizeof(uint16_t));
            }
            if (Entries != NULL) {
                for (uint8_t Color = 0; Color < PNG_NumChannelsPerColorType[PNG_GetColorType(Ovia)]; Color++) {
                    Entries[Color]    = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, Bits2Bytes(ImageContainer_GetBitDepth(Image), RoundingType_Up));
                }
                //Ovia->tRNS->Palette = Entries;
            } else {
                Log(Log_DEBUG, __func__, U8("Failed to allocate enough memory for the TRNS Palette"));
            }
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (ChunkSize % 3 != 0) {
            Log(Log_DEBUG, __func__, U8("The ChunkSize MUST be divisible by 3"));
        }
    }
    
    void PNG_BKGD_Parse(BitBuffer *BitB, uint32_t ChunkSize) { // Background
        if (PNG != NULL && BitB != NULL) {
            for (uint8_t Entry = 0; Entry < 3; Entry++) {
                uint8_t BackgroundEntry = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8);
                PNG_BKGD_SetBackgroundPaletteEntry(Ovia, BackgroundEntry);
            }
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_CHRM_Parse(BitBuffer *BitB, uint32_t ChunkSize) { // Chromaticities
        if (PNG != NULL && BitB != NULL) {
            uint32_t WhitePointX = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 32);
            uint32_t WhitePointY = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 32);
            uint32_t RedX        = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 32);
            uint32_t RedY        = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 32);
            uint32_t GreenX      = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 32);
            uint32_t GreenY      = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 32);
            uint32_t BlueX       = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 32);
            uint32_t BlueY       = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 32);
            
            PNG_CHRM_SetWhitePoint(Ovia, WhitePointX, WhitePointY);
            PNG_CHRM_SetRed(Ovia, RedX, RedY);
            PNG_CHRM_SetGreen(Ovia, GreenX, GreenY);
            PNG_CHRM_SetBlue(Ovia, BlueX, BlueY);
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_GAMA_Parse(BitBuffer *BitB, uint32_t ChunkSize) { // Gamma
        if (PNG != NULL && BitB != NULL) {
            
            uint32_t Gamma = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 32);
            PNG_GAMA_SetGamma(Ovia, Gamma);
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_OFFS_Parse(BitBuffer *BitB, uint32_t ChunkSize) { // Image Offset
        if (PNG != NULL && BitB != NULL) {
            PNG_OFFS_SetXOffset(PNG, BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 32));
            PNG_OFFS_SetYOffset(PNG, BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 32));
            PNG_OFFS_SetSpecifier(PNG, BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8));
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_PHYS_Parse(BitBuffer *BitB, uint32_t ChunkSize) { // Aspect ratio, Physical pixel size
        if (PNG != NULL && BitB != NULL) {
            PNG_PHYS_SetPixelsPerUnitX(PNG, BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 32));
            PNG_PHYS_SetPixelsPerUnitY(PNG, BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 32));
            PNG_PHYS_SetUnitSpecifier(PNG, BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8));
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_SCAL_Parse(BitBuffer *BitB, uint32_t ChunkSize) { // Physical Scale
        if (PNG != NULL && BitB != NULL) {
            uint8_t UnitSpecifier     = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8); // 1 = Meter, 2 = Radian
            
            uint32_t WidthStringSize  = BitBuffer_GetUTF8StringSize(BitB);
            uint32_t HeightStringSize = ChunkSize - WidthStringSize - 1 - 1; // - 1 for the Unit specifier, and the null terminator for string1
            
            UTF8 *WidthString         = BitBuffer_ReadUTF8(BitB, WidthStringSize);
            BitBuffer_Seek(BitB, 8); // Skip the NULL seperator
            UTF8 *HeightString        = BitBuffer_ReadUTF8(BitB, HeightStringSize);
            
            double Width              = UTF8_String2Decimal(WidthString);
            double Height             = UTF8_String2Decimal(HeightString);
            
            free(WidthString);
            free(HeightString);
            
            PNG_SCAL_SetSCAL(Ovia, UnitSpecifier, Width, Height);
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_PCAL_Parse(BitBuffer *BitB, uint32_t ChunkSize) {
        if (PNG != NULL && BitB != NULL) {
            uint8_t CalibrationSize = BitBuffer_GetUTF8StringSize(BitB);
            UTF8   *Calibration     = BitBuffer_ReadUTF8(BitB, CalibrationSize);
            
            PNG_PCAL_SetCalibrationName(Ovia, Calibration);
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_SBIT_Parse(BitBuffer *BitB, uint32_t ChunkSize) { // Significant bits per sample
        if (PNG != NULL && BitB != NULL) {
            PNG_SBIT_SetRed(PNG, BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8));
            PNG_SBIT_SetGreen(PNG, BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8));
            PNG_SBIT_SetBlue(PNG, BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8));
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_SRGB_Parse(BitBuffer *BitB, uint32_t ChunkSize) {
        if (PNG != NULL && BitB != NULL) {
            PNG_SRGB_SetRenderingIntent(PNG, BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8));
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_STER_Parse(BitBuffer *BitB, uint32_t ChunkSize) {
        if (PNG != NULL && BitB != NULL) {
            PNG_STER_SetSterType(PNG, BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8));
            
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
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_TEXT_Parse(BitBuffer *BitB, uint32_t ChunkSize) { // Uncompressed, ASCII tEXt
        if (PNG != NULL && BitB != NULL) {
            // Read until you hit a NULL for the name string, then subtract the size of the previous string from the total length to get the number of bytes for the second string
            uint8_t  KeywordSize = 0UL;
            uint8_t  CurrentByte = 0; // 1 is BULLshit
            
            do {
                CurrentByte  = BitBuffer_PeekBits(BitB, MSByteFirst, LSBitFirst, 8);
                KeywordSize += 1;
            } while (CurrentByte != 0);
            
            uint32_t CommentSize = ChunkSize - KeywordSize;
            
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
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_TIME_Parse(PNGOptions *PNG, BitBuffer *BitB, uint32_t ChunkSize) {
        if (PNG != NULL && BitB != NULL) {
            PNG->tIMe->Year                     = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 16);
            PNG->tIMe->Month                    = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8);
            PNG->tIMe->Day                      = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8);
            PNG->tIMe->Hour                     = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8);
            PNG->tIMe->Minute                   = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8);
            PNG->tIMe->Second                   = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8);
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    /* APNG */
    void PNG_ACTL_Parse(PNGOptions *PNG, BitBuffer *BitB, uint32_t ChunkSize) { // Animation control, part of APNG
        if (PNG != NULL && BitB != NULL) {
            PNG->acTL->NumFrames             = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 32);
            PNG->acTL->TimesToLoop           = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 32); // If 0, loop forever.
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_FCTL_Parse(PNGOptions *PNG, BitBuffer *BitB, uint32_t ChunkSize) { // Frame Control, part of APNG
        if (PNG != NULL && BitB != NULL) {
            PNG->fcTL->FrameNum              = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 32);
            PNG->fcTL->Width                 = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 32);
            PNG->fcTL->Height                = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 32);
            PNG->fcTL->XOffset               = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 32);
            PNG->fcTL->YOffset               = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 32);
            PNG->fcTL->FrameDelayNumerator   = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 16);
            PNG->fcTL->FrameDelayDenominator = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 16);
            PNG->fcTL->DisposeMethod         = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8);
            PNG->fcTL->BlendMethod           = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8);
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    /* End APNG */
    
    void PNG_HIST_Parse(PNGOptions *PNG, BitBuffer *BitB, uint32_t ChunkSize) {
        if (PNG != NULL && BitB != NULL) {
            PNG->hIST->NumColors = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 32);
            for (uint32_t Color = 0; Color < PNG->hIST->NumColors; Color++) {
                PNG->hIST->Histogram[Color] = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, PNG->iHDR->BitDepth);
            }
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_ICCP_Parse(PNGOptions *PNG, BitBuffer *BitB, uint32_t ChunkSize) {
        if (PNG != NULL && BitB != NULL) {
            
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
        uint8_t ProfileNameSize = 0;
        ProfileNameSize = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8);
    }
    
    void PNG_SPLT_Parse(PNGOptions *PNG, BitBuffer *BitB, uint32_t ChunkSize) {
        if (PNG != NULL && BitB != NULL) {
            
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void PNG_ParseChunks(PNGOptions *PNG, BitBuffer *BitB) {
        if (PNG != NULL && BitB != NULL) {
            ImageContainer *Image    = NULL;
            while (BitBuffer_GetSize(BitB) > 0) {
                uint32_t ChunkSize   = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 32);
                uint32_t ChunkID     = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 32);
                
                if (ChunkID == acTLMarker) {
                    PNG_ACTL_Parse(PNG, BitB, ChunkSize);
                } else if (ChunkID == bKGDMarker) {
                    PNG_BKGD_Parse(PNG, BitB, ChunkSize);
                } else if (ChunkID == cHRMMarker) {
                    PNG_CHRM_Parse(PNG, BitB, ChunkSize);
                } else if (ChunkID == fcTLMarker) {
                    PNG_FCTL_Parse(PNG, BitB, ChunkSize);
                } else if (ChunkID == gAMAMarker) {
                    PNG_GAMA_Parse(PNG, BitB, ChunkSize);
                } else if (ChunkID == hISTMarker) {
                    PNG_HIST_Parse(PNG, BitB, ChunkSize);
                } else if (ChunkID == iCCPMarker) {
                    PNG_OFFS_Parse(PNG, BitB, ChunkSize);
                } else if (ChunkID == IDATMarker || ChunkID == fDATMarker) {
                    PNG_DAT_Parse(PNG, BitB, ChunkSize);
                } else if (ChunkID == iHDRMarker) {
                    PNG_IHDR_Parse(PNG, BitB, ChunkSize);
                } else if (ChunkID == oFFsMarker) {
                    PNG_OFFS_Parse(PNG, BitB, ChunkSize);
                } else if (ChunkID == pCALMarker) {
                    PNG_PCAL_Parse(PNG, BitB, ChunkSize);
                } else if (ChunkID == pHYsMarker) {
                    PNG_PHYS_Parse(PNG, BitB, ChunkSize);
                } else if (ChunkID == PLTEMarker) {
                    PNG_PLTE_Parse(PNG, BitB, ChunkSize);
                } else if (ChunkID == sBITMarker) {
                    PNG_SBIT_Parse(PNG, BitB, ChunkSize);
                } else if (ChunkID == sRGBMarker) {
                    PNG_SRGB_Parse(PNG, BitB, ChunkSize);
                } else if (ChunkID == sTERMarker) {
                    PNG_STER_Parse(PNG, BitB, ChunkSize);
                } else if (ChunkID == tEXtMarker || ChunkID == zTXtMarker || ChunkID == iTXtMarker) {
                    PNG_TEXT_Parse(PNG, BitB, ChunkSize);
                } else if (ChunkID == sCALMarker) {
                    PNG_SCAL_Parse(PNG, BitB, ChunkSize);
                } else if (ChunkID == tIMEMarker) {
                    PNG_TIME_Parse(PNG, BitB, ChunkSize);
                } else if (ChunkID == tRNSMarker) {
                    PNG_TRNS_Parse(PNG, BitB, ChunkSize);
                } else if (ChunkID == sPLTMarker) {
                    PNG_SPLT_Parse(PNG, BitB, ChunkSize);
                }
            }
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    ImageContainer *PNGExtractImage(PNGOptions *PNG, BitBuffer *BitB) {
        ImageContainer *Image = NULL;
        if (PNG != NULL && BitB != NULL) {
            
        } else if (PNG == NULL) {
            Log(Log_DEBUG, __func__, U8("PNGOptions Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        }
        return Image;
    }
    
    static const OVIADecoder PNGDecoder = {
        .DecoderID             = CodecID_PNG,
        .MediaType             = MediaType_Image,
        .MagicIDOffset         = 0,
        .MagicIDSize           = 8,
        .MagicID               = (uint8_t[8]) {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A},
        .Function_Initialize   = PNGOptions_Init,
        .Function_Decode       = PNGExtractImage,
        .Function_Deinitialize = PNGOptions_Deinit,
    };
    
    OVIADemuxer PNGDemuxer = {
        .Function_ParseChunks  = PNG_ParseChunks,
    };
    
#ifdef __cplusplus
}
#endif
