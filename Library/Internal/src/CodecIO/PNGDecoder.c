#include "../../../../Dependencies/FoundationIO/libFoundationIO/include/Macros.h"
#include "../../../include/Private/Image/PNGCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
#ifdef _WIN32
#define strcasecmp _stricmp
#endif
    
    uint8_t PaethPredictor(int64_t Left, int64_t Above, int64_t UpperLeft) {
        uint8_t Output    = 0;
        
        int64_t Guess     = Left + Above - UpperLeft;
        int64_t DistanceA = Absolute(Guess - Left);
        int64_t DistanceB = Absolute(Guess - Above);
        int64_t DistanceC = Absolute(Guess - UpperLeft);
        
        if (DistanceA <= DistanceB && DistanceA <= DistanceC) {
            Output = DistanceA;
        } else if (DistanceB <= DistanceC) {
            Output = DistanceB;
        } else {
            Output = DistanceC;
        }
        return Output;
    }
    
    void OVIA_PNG_Filter_Sub(OVIA *Ovia, ImageContainer *Image) {
        if (Ovia != NULL && Image != NULL) {
            Image_Types Type = ImageContainer_GetType(Image);
            if (Type == ImageType_Integer8) {
                uint8_t  ****ImageArray = (uint8_t****)  ImageContainer_GetArray(Image);
                
                for (uint8_t StereoView = 0; StereoView < ImageContainer_GetNumViews(Image); StereoView++) {
                    for (uint32_t Height = 0UL; Height < ImageContainer_GetHeight(Image); Height++) {
                        for (uint32_t Width = 0UL; Width < OVIA_GetWidth(Ovia); Width++) {
                            for (uint32_t Byte = 0UL; Byte < Bits2Bytes(OVIA_GetBitDepth(Ovia), true); Byte++) {
                                ImageArray[StereoView][Height][Width][Byte] = (ImageArray[StereoView][Height][Width][Byte] + ImageArray[StereoView][Height][Width][Byte + 1]) % 256;
                            }
                        }
                    }
                }
            } else {
                uint16_t ****ImageArray = (uint16_t****) ImageContainer_GetArray(Image);
                
                for (uint8_t StereoView = 0; StereoView < ImageContainer_GetNumViews(Image); StereoView++) {
                    for (uint32_t Height = 0UL; Height < ImageContainer_GetHeight(Image); Height++) {
                        for (uint32_t Width = 0UL; Width < OVIA_GetWidth(Ovia); Width++) {
                            for (uint32_t Byte = 0UL; Byte < Bits2Bytes(OVIA_GetBitDepth(Ovia), true); Byte++) {
                                ImageArray[StereoView][Height][Width][Byte] = (ImageArray[StereoView][Height][Width][Byte] + ImageArray[StereoView][Height][Width][Byte + 1]) % 256;
                            }
                        }
                    }
                }
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (Image == NULL) {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_Filter_Up(OVIA *Ovia, ImageContainer *Image) {
        if (Ovia != NULL && Image != NULL) {
            Image_Types Type = ImageContainer_GetType(Image);
            if (Type == ImageType_Integer8) {
                uint8_t  ****ImageArray = (uint8_t****)  ImageContainer_GetArray(Image);
                
                for (uint8_t StereoView = 0; StereoView < ImageContainer_GetNumViews(Image); StereoView++) {
                    for (uint32_t Height = 0UL; Height < ImageContainer_GetHeight(Image); Height++) {
                        for (uint32_t Width = 0UL; Width < OVIA_GetWidth(Ovia); Width++) {
                            for (uint32_t Byte = 0UL; Byte < Bits2Bytes(OVIA_GetBitDepth(Ovia), true); Byte++) {
                                ImageArray[StereoView][Height][Width][Byte] = (ImageArray[StereoView][Height][Width][Byte] + ImageArray[StereoView][Height - 1][Width][Byte]) % 256;
                            }
                        }
                    }
                }
            } else {
                uint16_t ****ImageArray = (uint16_t****) ImageContainer_GetArray(Image);
                
                for (uint8_t StereoView = 0; StereoView < ImageContainer_GetNumViews(Image); StereoView++) {
                    for (uint32_t Height = 0UL; Height < ImageContainer_GetHeight(Image); Height++) {
                        for (uint32_t Width = 0UL; Width < OVIA_GetWidth(Ovia); Width++) {
                            for (uint32_t Byte = 0UL; Byte < Bits2Bytes(OVIA_GetBitDepth(Ovia), true); Byte++) {
                                ImageArray[StereoView][Height][Width][Byte] = (ImageArray[StereoView][Height][Width][Byte] + ImageArray[StereoView][Height - 1][Width][Byte]) % 256;
                            }
                        }
                    }
                }
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (Image == NULL) {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_Filter_Average(OVIA *Ovia, ImageContainer *Image) {
        if (Ovia != NULL && Image != NULL) {
            Image_Types Type = ImageContainer_GetType(Image);
            if (Type == ImageType_Integer8) {
                uint8_t  ****ImageArray = (uint8_t****)  ImageContainer_GetArray(Image);
                
                for (uint8_t StereoView = 0; StereoView < ImageContainer_GetNumViews(Image); StereoView++) {
                    for (uint32_t Height = 0UL; Height < ImageContainer_GetHeight(Image); Height++) {
                        for (uint32_t Width = 0UL; Width < OVIA_GetWidth(Ovia); Width++) {
                            for (uint32_t Byte = 0UL; Byte < Bits2Bytes(OVIA_GetBitDepth(Ovia), true); Byte++) {
                                float   PreAverage = (ImageArray[StereoView][Height][Width][Byte - Bits2Bytes(OVIA_GetBitDepth(Ovia), true)] + ImageArray[StereoView][Height][Width - 1][Byte]) / 2;
                                uint8_t Average    = Floor(PreAverage) % 256;
                                
                                ImageArray[StereoView][Height][Width][Byte] = Average;
                            }
                        }
                    }
                }
            } else {
                uint16_t ****ImageArray = (uint16_t****) ImageContainer_GetArray(Image);
                
                for (uint8_t StereoView = 0; StereoView < ImageContainer_GetNumViews(Image); StereoView++) {
                    for (uint32_t Height = 0UL; Height < ImageContainer_GetHeight(Image); Height++) {
                        for (uint32_t Width = 0UL; Width < OVIA_GetWidth(Ovia); Width++) {
                            for (uint32_t Byte = 0UL; Byte < Bits2Bytes(OVIA_GetBitDepth(Ovia), true); Byte++) {
                                float   PreAverage = (ImageArray[StereoView][Height][Width][Byte - Bits2Bytes(OVIA_GetBitDepth(Ovia), true)] + ImageArray[StereoView][Height][Width - 1][Byte]) / 2;
                                uint16_t Average   = Floor(PreAverage) % 65536;
                                
                                ImageArray[StereoView][Height][Width][Byte] = Average;
                            }
                        }
                    }
                }
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (Image == NULL) {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_Filter_Paeth(OVIA *Ovia, ImageContainer *Image) {
        if (Ovia != NULL && Image != NULL) {
            Image_Types Type = ImageContainer_GetType(Image);
            if (Type == ImageType_Integer8) {
                uint8_t  ****ImageArray = (uint8_t****)  ImageContainer_GetArray(Image);
                
                for (uint8_t StereoView = 0; StereoView < ImageContainer_GetNumViews(Image); StereoView++) {
                    for (uint32_t Height = 0UL; Height < ImageContainer_GetHeight(Image); Height++) {
                        for (uint32_t Width = 0UL; Width < OVIA_GetWidth(Ovia); Width++) {
                            for (uint32_t Byte = 0UL; Byte < Bits2Bytes(OVIA_GetBitDepth(Ovia), true); Byte++) {
                                uint8_t Current = (ImageArray[StereoView][Height][Width][Byte]);
                                
                                uint8_t Left    = ImageArray[StereoView][Height][Width][Byte - 1];
                                uint8_t Above   = ImageArray[StereoView][Height - 1][Width][Byte];
                                uint8_t UpLeft  = ImageArray[StereoView][Height - 1][Width][Byte - 1];
                                uint8_t Paeth   = PaethPredictor(Left, Above, UpLeft);
                                
                                ImageArray[StereoView][Height][Width][Byte] = (Current - Paeth) % 256;
                            }
                        }
                    }
                }
            } else {
                uint16_t ****ImageArray = (uint16_t****) ImageContainer_GetArray(Image);
                
                for (uint8_t StereoView = 0; StereoView < ImageContainer_GetNumViews(Image); StereoView++) {
                    for (uint32_t Height = 0UL; Height < ImageContainer_GetHeight(Image); Height++) {
                        for (uint32_t Width = 0UL; Width < OVIA_GetWidth(Ovia); Width++) {
                            for (uint32_t Byte = 0UL; Byte < Bits2Bytes(OVIA_GetBitDepth(Ovia), true); Byte++) {
                                uint8_t Current = (ImageArray[StereoView][Height][Width][Byte]);
                                
                                uint8_t Left    = ImageArray[StereoView][Height][Width][Byte - 1];
                                uint8_t Above   = ImageArray[StereoView][Height - 1][Width][Byte];
                                uint8_t UpLeft  = ImageArray[StereoView][Height - 1][Width][Byte - 1];
                                uint8_t Paeth   = PaethPredictor(Left, Above, UpLeft);
                                
                                ImageArray[StereoView][Height][Width][Byte] = (Current - Paeth) % 256;
                            }
                        }
                    }
                }
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (Image == NULL) {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
    }
    
    // These filters need to be operatable on every line in any order, so we need a main OVIA_PNG_Defilter function.
    // That function then needs to read the first byte of each line to see what filter is used
    // then OVIA each line.
    // ALSO keep in mind concurrency.
    
    void OVIA_PNG_Defilter(OVIA *Ovia, ImageContainer *Image) {
        if (Ovia != NULL && Image != NULL) {
            Image_Types Type = ImageContainer_GetType(Image);
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
                            OVIA_PNG_Filter_Sub(Ovia, Image);
                            break;
                        case UpFilter:
                            // UpFilter
                            OVIA_PNG_Filter_Up(Ovia, Image);
                            break;
                        case AverageFilter:
                            // AverageFilter
                            OVIA_PNG_Filter_Average(Ovia, Image);
                            break;
                        case PaethFilter:
                            // PaethFilter
                            OVIA_PNG_Filter_Paeth(Ovia, Image);
                            break;
                        default:
                            Log(Log_ERROR, __func__, U8("Filter type: %d is invalid"), ImageArray[0][ScanLine][0][0]);
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
                            OVIA_PNG_Filter_Sub(Ovia, Image);
                            break;
                        case UpFilter:
                            OVIA_PNG_Filter_Up(Ovia, Image);
                            break;
                        case AverageFilter:
                            OVIA_PNG_Filter_Average(Ovia, Image);
                            break;
                        case PaethFilter:
                            OVIA_PNG_Filter_Paeth(Ovia, Image);
                            break;
                        default:
                            Log(Log_ERROR, __func__, U8("Filter type: %d is invalid"), ImageArray[0][ScanLine][0][0]);
                            break;
                    }
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
    }
    
    enum DEFLATEBlockTypes {
        UncompressedBlock   = 0,
        FixedHuffmanBlock   = 1,
        DynamicHuffmanBlock = 2,
        
    };
    
    void OVIA_PNG_DecodeDeflateBlock(OVIA *Ovia, BitBuffer *BitB, ImageContainer *Image) {
        // Huffman codes are written MSBit first, everything else is writen LSBit first
        if (Ovia != NULL && BitB != NULL && Image != NULL) {
            // DEFLATE Block header:
            bool     BlockIsFinal = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 1); // BFINAL
            uint8_t  BlockType    = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 2); // BTYPE
            
            if (BlockType == DynamicHuffmanBlock) {
                
            } else if (BlockType == FixedHuffmanBlock) {
                
            } else if (BlockType == UncompressedBlock) {
                
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (Image == NULL) {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_DAT_Parse(OVIA *Ovia, BitBuffer *BitB, uint32_t DATSize) { // OVIAs both fDAT and IDAT chunks
        if (Ovia != NULL && BitB != NULL) {
            // well lets go ahead and allocate a DAT block the size of DATSize
            BitBuffer_Init(DATSize);
            // Now we need to go ahead and parse the ZLIB Header.
            // ok so how do we do that? I wrote some notes on the Zlib header last night...
            
            OVIA_PNG_DAT_SetCMF(Ovia, BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8));
            OVIA_PNG_DAT_SetFLG(Ovia, BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8));
            
            /* Compression Method and Flags byte */
            uint8_t CompressionMethod = OVIA_PNG_DAT_GetCompressionMethod(Ovia); // 8
            uint8_t CompressionInfo   = OVIA_PNG_DAT_GetCompressionInfo(Ovia); // 7
            /* Compression Method and Flags byte */
            
            /* FlagByte */
            uint8_t FCHECK            = OVIA_PNG_DAT_GetFCHECK(Ovia); // 1E
            bool    FDICTPresent      = OVIA_PNG_DAT_GetFDICT(Ovia); // 0
            uint8_t FLEVEL            = OVIA_PNG_DAT_GetFLEVEL(Ovia); // 1
            /* FlagByte */
            
            if (OVIA_PNG_DAT_GetFDICT(Ovia)) {
                OVIA_PNG_DAT_SetDictID(Ovia, BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32));
            }
            
            // Start reading the DEFLATE block?
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_Adam7_Deinterlace(OVIA *Ovia, ImageContainer *Image) {
        if (Ovia != NULL && Image != NULL) {
            
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (Image == NULL) {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_IHDR_Parse(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) {
        if (Ovia != NULL && BitB != NULL) {
            uint32_t Width          = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            uint32_t Height         = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            uint8_t  BitDepth       = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            uint8_t  ColorType      = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            if (ColorType == 1 || ColorType == 5 || ColorType >= 7) {
                Log(Log_ERROR, __func__, U8("Invalid color type: %d"), ColorType);
            }
            uint8_t Compression    = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            uint8_t FilterMethod   = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            uint8_t Progressive    = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            
            OVIA_PNG_IHDR_SetIHDR(Ovia, Height, Width, BitDepth, ColorType, Progressive);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_PLTE_Parse(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) {
        if (Ovia != NULL && BitB != NULL && ChunkSize % 3 == 0) {
            uint8_t BitDepth = OVIA_GetBitDepth(Ovia);
            if (BitDepth <= 8) {
                uint8_t ColorType = OVIA_PNG_GetColorType(Ovia);
                if (ColorType == PNG_PalettedRGB || ColorType == PNG_RGB) {
                    OVIA_PNG_PLTE_Init(Ovia, ChunkSize / 3);
                    for (uint32_t Entry = 0UL; Entry < ChunkSize / 3; Entry++) {
                        uint8_t Red   = BitBuffer_ReadBits(LSByteFirst, LSBitFirst, BitB, 8);
                        uint8_t Green = BitBuffer_ReadBits(LSByteFirst, LSBitFirst, BitB, 8);
                        uint8_t Blue  = BitBuffer_ReadBits(LSByteFirst, LSBitFirst, BitB, 8);
                        OVIA_PNG_PLTE_SetPalette(Ovia, Entry, Red, Green, Blue);
                    }
                } else if (ColorType == PNG_RGBA) {
                    for (uint32_t Entry = 0UL; Entry < ChunkSize / 3; Entry++) {
                        uint8_t Red   = BitBuffer_ReadBits(LSByteFirst, LSBitFirst, BitB, 8);
                        uint8_t Green = BitBuffer_ReadBits(LSByteFirst, LSBitFirst, BitB, 8);
                        uint8_t Blue  = BitBuffer_ReadBits(LSByteFirst, LSBitFirst, BitB, 8);
                        OVIA_PNG_PLTE_SetPalette(Ovia, Entry, Red, Green, Blue);
                    }
                }
            } else {
                BitBuffer_Seek(BitB, Bytes2Bits(ChunkSize));
                Log(Log_ERROR, __func__, U8("BitDepth %d can't have a palette"), BitDepth);
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (ChunkSize % 3 != 0) {
            Log(Log_ERROR, __func__, U8("The ChunkSize MUST be divisible by 3"));
        }
    }
    
    void OVIA_PNG_TRNS_Parse(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) { // Transparency
        if (Ovia != NULL && BitB != NULL && ChunkSize  % 3 == 0) {
            uint32_t NumEntries    = ChunkSize % 3;
            uint16_t **Entries    = NULL;
            if (OVIA_PNG_GetColorType(Ovia) == PNG_RGB) {
                Entries = calloc(3, Bits2Bytes(OVIA_GetBitDepth(Ovia), true) * sizeof(uint16_t));
            } else if (OVIA_PNG_GetColorType(Ovia) == PNG_RGBA) {
                Entries = calloc(4, Bits2Bytes(OVIA_GetBitDepth(Ovia), true) * sizeof(uint16_t));
            } else if (OVIA_PNG_GetColorType(Ovia) == PNG_Grayscale) {
                Entries = calloc(1, Bits2Bytes(OVIA_GetBitDepth(Ovia), true) * sizeof(uint16_t));
            } else if (OVIA_PNG_GetColorType(Ovia) == PNG_GrayAlpha) {
                Entries = calloc(2, Bits2Bytes(OVIA_GetBitDepth(Ovia), true) * sizeof(uint16_t));
            }
            if (Entries != NULL) {
                for (uint8_t Color = 0; Color < ModernPNGChannelsPerColorType[OVIA_PNG_GetColorType(Ovia)]; Color++) {
                    Entries[Color]    = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, Bits2Bytes(OVIA_GetBitDepth(Ovia), true));
                }
                //Ovia->tRNS->Palette = Entries;
            } else {
                Log(Log_ERROR, __func__, U8("Failed to allocate enough memory for the TRNS Palette"));
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (ChunkSize % 3 != 0) {
            Log(Log_ERROR, __func__, U8("The ChunkSize MUST be divisible by 3"));
        }
    }
    
    void OVIA_PNG_BKGD_Parse(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) { // Background
        if (Ovia != NULL && BitB != NULL) {
            for (uint8_t Entry = 0; Entry < 3; Entry++) {
                uint8_t BackgroundEntry = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
                OVIA_PNG_BKGD_SetBackgroundPaletteEntry(Ovia, BackgroundEntry);
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_CHRM_Parse(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) { // Chromaticities
        if (Ovia != NULL && BitB != NULL) {
            uint32_t WhitePointX = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            uint32_t WhitePointY = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            uint32_t RedX        = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            uint32_t RedY        = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            uint32_t GreenX      = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            uint32_t GreenY      = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            uint32_t BlueX       = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            uint32_t BlueY       = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            
            OVIA_PNG_CHRM_SetWhitePoint(Ovia, WhitePointX, WhitePointY);
            OVIA_PNG_CHRM_SetRed(Ovia, RedX, RedY);
            OVIA_PNG_CHRM_SetGreen(Ovia, GreenX, GreenY);
            OVIA_PNG_CHRM_SetBlue(Ovia, BlueX, BlueY);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_GAMA_Parse(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) { // Gamma
        if (Ovia != NULL && BitB != NULL) {
            
            uint32_t Gamma = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            OVIA_PNG_GAMA_SetGamma(Ovia, Gamma);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_OFFS_Parse(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) { // Image Offset
        if (Ovia != NULL && BitB != NULL) {
            OVIA_PNG_OFFS_SetXOffset(Ovia, BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32));
            OVIA_PNG_OFFS_SetYOffset(Ovia, BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32));
            OVIA_PNG_OFFS_SetSpecifier(Ovia, BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8));
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_PHYS_Parse(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) { // Aspect ratio, Physical pixel size
        if (Ovia != NULL && BitB != NULL) {
            OVIA_PNG_PHYS_SetPixelsPerUnitX(Ovia, BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32));
            OVIA_PNG_PHYS_SetPixelsPerUnitY(Ovia, BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32));
            OVIA_PNG_PHYS_SetUnitSpecifier(Ovia, BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8));
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_SCAL_Parse(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) { // Physical Scale
        if (Ovia != NULL && BitB != NULL) {
            uint8_t UnitSpecifier     = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8); // 1 = Meter, 2 = Radian
            
            uint32_t WidthStringSize  = BitBuffer_GetUTF8StringSize(BitB);
            uint32_t HeightStringSize = ChunkSize - WidthStringSize - 1 - 1; // - 1 for the Unit specifier, and the null terminator for string1
            
            UTF8 *WidthString         = BitBuffer_ReadUTF8(BitB, WidthStringSize);
            BitBuffer_Seek(BitB, 8); // Skip the NULL seperator
            UTF8 *HeightString        = BitBuffer_ReadUTF8(BitB, HeightStringSize);
            
            double Width              = UTF8_String2Decimal(WidthString);
            double Height             = UTF8_String2Decimal(HeightString);
            
            free(WidthString);
            free(HeightString);
            
            OVIA_PNG_SCAL_SetSCAL(Ovia, UnitSpecifier, Width, Height);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_PCAL_Parse(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) {
        if (Ovia != NULL && BitB != NULL) {
            uint8_t CalibrationSize = BitBuffer_GetUTF8StringSize(BitB);
            UTF8   *Calibration     = BitBuffer_ReadUTF8(BitB, CalibrationSize);
            
            OVIA_PNG_PCAL_SetCalibrationName(Ovia, Calibration);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_SBIT_Parse(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) { // Significant bits per sample
        if (Ovia != NULL && BitB != NULL) {
            OVIA_PNG_SBIT_SetRed(Ovia, BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8));
            OVIA_PNG_SBIT_SetGreen(Ovia, BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8));
            OVIA_PNG_SBIT_SetBlue(Ovia, BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8));
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_SRGB_Parse(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) {
        if (Ovia != NULL && BitB != NULL) {
            OVIA_PNG_SRGB_SetRenderingIntent(Ovia, BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8));
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_STER_Parse(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) {
        if (Ovia != NULL && BitB != NULL) {
            OVIA_PNG_STER_SetSterType(Ovia, BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8));
            
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
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_TEXT_Parse(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) { // Uncompressed, ASCII tEXt
        if (Ovia != NULL && BitB != NULL) {
            // Read until you hit a NULL for the name string, then subtract the size of the previous string from the total length to get the number of bytes for the second string
            uint8_t  KeywordSize = 0UL;
            uint8_t  CurrentByte = 0; // 1 is BULLshit
            
            do {
                CurrentByte  = BitBuffer_PeekBits(MSByteFirst, LSBitFirst, BitB, 8);
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
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_TIME_Parse(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) {
        if (Ovia != NULL && BitB != NULL) {
            uint16_t Year                     = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 16);
            uint8_t  Month                    = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            uint8_t  Day                      = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            uint8_t  Hour                     = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            uint8_t  Minute                   = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            uint8_t  Second                   = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            OVIA_PNG_TIME_SetTime(Ovia, Year, Month, Day, Hour, Minute, Second);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    /* APNG */
    void OVIA_PNG_ACTL_Parse(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) { // Animation control, part of APNG
        if (Ovia != NULL && BitB != NULL) {
            uint32_t NumFrames             = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            uint32_t TimesToLoop           = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32); // If 0, loop forever.
            OVIA_PNG_ACTL_SetACTL(Ovia, NumFrames, TimesToLoop);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_FCTL_Parse(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) { // Frame Control, part of APNG
        if (Ovia != NULL && BitB != NULL) {
            uint32_t FrameNum              = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            uint32_t Width                 = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            uint32_t Height                = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            uint32_t XOffset               = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            uint32_t YOffset               = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            uint16_t FrameDelayNumerator   = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 16);
            uint16_t FrameDelayDenominator = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 16);
            uint8_t  DisposeMethod         = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            uint8_t  BlendMethod           = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            OVIA_PNG_FCTL_SetFrameNum(Ovia, FrameNum);
            OVIA_PNG_FCTL_SetWidth(Ovia, Width);
            OVIA_PNG_FCTL_SetHeight(Ovia, Height);
            OVIA_PNG_FCTL_SetXOffset(Ovia, XOffset);
            OVIA_PNG_FCTL_SetFrameDelayNumerator(Ovia, FrameDelayNumerator);
            OVIA_PNG_FCTL_SetFrameDelayDenominator(Ovia, FrameDelayDenominator);
            OVIA_PNG_FCTL_SetDisposeMethod(Ovia, DisposeMethod);
            OVIA_PNG_FCTL_SetBlendMethod(Ovia, BlendMethod);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    /* End APNG */
    
    void OVIA_PNG_HIST_Parse(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) {
        if (Ovia != NULL && BitB != NULL) {
            
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_ICCP_Parse(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) {
        if (Ovia != NULL && BitB != NULL) {
            
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
        uint8_t ProfileNameSize = 0;
        ProfileNameSize = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
    }
    
    void OVIA_PNG_SPLT_Parse(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) {
        if (Ovia != NULL && BitB != NULL) {
            
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_ParseChunks(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            while (BitBuffer_GetSize(BitB) > 0) { // 12 is the start of IEND
                uint32_t ChunkSize   = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
                uint32_t ChunkID     = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
                
                if (ChunkID == acTLMarker) {
                    OVIA_PNG_ACTL_Parse(Ovia, BitB, ChunkSize);
                } else if (ChunkID == bKGDMarker) {
                    OVIA_PNG_BKGD_Parse(Ovia, BitB, ChunkSize);
                } else if (ChunkID == cHRMMarker) {
                    OVIA_PNG_CHRM_Parse(Ovia, BitB, ChunkSize);
                } else if (ChunkID == fcTLMarker) {
                    OVIA_PNG_FCTL_Parse(Ovia, BitB, ChunkSize);
                } else if (ChunkID == gAMAMarker) {
                    OVIA_PNG_GAMA_Parse(Ovia, BitB, ChunkSize);
                } else if (ChunkID == hISTMarker) {
                    OVIA_PNG_HIST_Parse(Ovia, BitB, ChunkSize);
                } else if (ChunkID == iCCPMarker) {
                    OVIA_PNG_OFFS_Parse(Ovia, BitB, ChunkSize);
                } else if (ChunkID == IDATMarker || ChunkID == fDATMarker) {
                    OVIA_PNG_DAT_Parse(Ovia, BitB, ChunkSize);
                } else if (ChunkID == iHDRMarker) {
                    OVIA_PNG_IHDR_Parse(Ovia, BitB, ChunkSize);
                } else if (ChunkID == oFFsMarker) {
                    OVIA_PNG_OFFS_Parse(Ovia, BitB, ChunkSize);
                } else if (ChunkID == pCALMarker) {
                    OVIA_PNG_PCAL_Parse(Ovia, BitB, ChunkSize);
                } else if (ChunkID == pHYsMarker) {
                    OVIA_PNG_PHYS_Parse(Ovia, BitB, ChunkSize);
                } else if (ChunkID == PLTEMarker) {
                    OVIA_PNG_PLTE_Parse(Ovia, BitB, ChunkSize);
                } else if (ChunkID == sBITMarker) {
                    OVIA_PNG_SBIT_Parse(Ovia, BitB, ChunkSize);
                } else if (ChunkID == sRGBMarker) {
                    OVIA_PNG_SRGB_Parse(Ovia, BitB, ChunkSize);
                } else if (ChunkID == sTERMarker) {
                    OVIA_PNG_STER_Parse(Ovia, BitB, ChunkSize);
                } else if (ChunkID == tEXtMarker || ChunkID == zTXtMarker || ChunkID == iTXtMarker) {
                    OVIA_PNG_TEXT_Parse(Ovia, BitB, ChunkSize);
                } else if (ChunkID == sCALMarker) {
                    OVIA_PNG_SCAL_Parse(Ovia, BitB, ChunkSize);
                } else if (ChunkID == tIMEMarker) {
                    OVIA_PNG_TIME_Parse(Ovia, BitB, ChunkSize);
                } else if (ChunkID == tRNSMarker) {
                    OVIA_PNG_TRNS_Parse(Ovia, BitB, ChunkSize);
                } else if (ChunkID == sPLTMarker) {
                    OVIA_PNG_SPLT_Parse(Ovia, BitB, ChunkSize);
                }
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    ImageContainer *PNGExtractImage(OVIA *Ovia, BitBuffer *BitB) {
        ImageContainer *Image = NULL;
        if (Ovia != NULL && BitB != NULL) {
            
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
        return Image;
    }
    
#ifdef __cplusplus
}
#endif
