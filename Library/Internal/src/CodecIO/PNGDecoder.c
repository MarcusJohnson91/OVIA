#include "../../../../Dependencies/FoundationIO/libFoundationIO/include/Macros.h"
#include "../../../include/Private/Image/PNGCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
#ifdef _WIN32
#define strcasecmp _stricmp
#endif
    
    /*
     bool VerifyChunkCRC(BitBuffer *BitB, uint32_t ChunkSize) {
     // So basically we need to read ChunkSize bytes into an array, then read the following 4 bytes as the CRC
     // then run VerifyCRC over the buffer, and finally compare the generated CRC with the extracted one, and return whether they match.
     // Then call SkipBits(BitB, Bytes2Bits(ChunkSize)); to reset to the beginning of the chunk
     uint8_t *Buffer2CRC = calloc(1, ChunkSize * sizeof(uint8_t));
     for (uint32_t Byte = 0; Byte < ChunkSize; Byte++) {
     Buffer2CRC[Byte] = BitB->Buffer[Bits2Bytes(, false)];
     free(Buffer2CRC);
     
     }
     uint32_t ChunkCRC = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
     bool CRCsMatch = VerifyCRC(Buffer2CRC, ChunkSize, 0x82608EDB, 32, 0xFFFFFFFF, ChunkCRC);
     SkipBits(BitB, -Bytes2Bits(ChunkSize));
     return CRCsMatch;
     }
     */
    
    uint8_t PaethPredictor(int64_t Left, int64_t Above, int64_t UpperLeft) {
        uint8_t Output    = 0;
        
        int64_t Guess     = Left + Above - UpperLeft;
        int64_t DistanceA = Absolute(Guess - Left);
        int64_t DistanceB = Absolute(Guess - Above);
        int64_t DistanceC = Absolute(Guess - UpperLeft);
        
        if (DistanceA <= DistanceB && DistanceA < DistanceC) {
            Output = DistanceA;
        } else if (DistanceB < DistanceC) {
            Output = DistanceB;
        } else {
            Output = DistanceC;
        }
        return Output;
    }
    
    void PNGOVIANonFilter(ImageContainer *Image) {
        if (Image != NULL) {
            uint8_t  NumViews = ImageContainer_GetNumViews(Image);
            uint32_t Width    = ImageContainer_GetWidth(Image);
            uint32_t Height   = ImageContainer_GetHeight(Image);
            
            for (uint8_t StereoView = 0; StereoView < NumViews; StereoView++) {
                for (uint32_t W = 0; W < Width; W++) {
                    for (size_t Byte = 1; Byte < Bits2Bytes(Ovia->iHDR->BitDepth, true); Byte++) {
                        DeFilteredData[StereoView][Width][Byte - 1] = InflatedData[StereoView][Width][Byte]; // Remove filter indicating byte
                    }
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
    }
    
    void PNGOVIASubFilter(ImageContainer *Image) {
        if (Image != NULL) {
            for (uint8_t StereoView = 0; StereoView < Ovia->sTER->StereoType; StereoView++) {
                for (uint32_t Width = 0; Width < Ovia->iHDR->Width; Width++) {
                    for (size_t Byte = 1; Byte < Bits2Bytes(Ovia->iHDR->BitDepth, true); Byte++) {
                        DeFilteredData[StereoView][Line][Byte - 1] = (InflatedData[StereoView][Line][Byte] + InflatedData[StereoView][Line][Byte+1]) & 0xFF; // Byte+1 is wrong, needs to be + bitdepth
                    }
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        
    }
    
    void PNGOVIAUpFilter(OVIA *Ovia, uint8_t ***InflatedData, uint8_t ***DeFilteredData, size_t Line) {
        if (Image != NULL) {
            for (uint8_t StereoView = 0; StereoView < Ovia->sTER->StereoType; StereoView++) {
                for (uint32_t Width = 0; Width < Ovia->iHDR->Width; Width++) {
                    for (size_t Byte = 1; Byte < Bits2Bytes(Ovia->iHDR->BitDepth, true); Byte++) {
                        DeFilteredData[StereoView][Line][Byte - 1] = InflatedData[StereoView][Line][Byte] + InflatedData[StereoView][Line - 1][Byte] & 0xFF;
                    }
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        
    }
    
    void PNGOVIAAverageFilter(OVIA *Ovia, uint8_t ***InflatedData, uint8_t ***DeFilteredData, size_t Line) {
        if (Image != NULL) {
            uint8_t PixelSize = Bits2Bytes(Ovia->iHDR->BitDepth, true);
            for (uint8_t StereoView = 0; StereoView < Ovia->sTER->StereoType; StereoView++) {
                for (uint32_t Width = 0; Width < Ovia->iHDR->Width; Width++) {
                    for (size_t Byte = 1; Byte < Bits2Bytes(Ovia->iHDR->BitDepth, true); Byte++) {
                        uint8_t Average = floor((InflatedData[StereoView][Line][Byte - (PixelSize)] + InflatedData[StereoView][Line - 1][Byte]) / 2);
                        DeFilteredData[StereoView][Line][Byte - 1] = InflatedData[StereoView][Line][Byte] + Average;
                    }
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
        
    }
    
    void PNGOVIAPaethFilter(OVIA *Ovia, uint8_t ***InflatedData, uint8_t ***DeFilteredData, size_t Line) {
        if (Image != NULL) {
            // Filtering is applied to bytes, not pixels
            uint8_t PixelSize = Bits2Bytes(Ovia->iHDR->BitDepth, true);
            for (uint8_t StereoView = 0; StereoView < Ovia->sTER->StereoType; StereoView++) {
                for (uint32_t Width = 0; Width < Ovia->iHDR->Width; Width++) {
                    for (size_t Byte = 1; Byte < Bits2Bytes(Ovia->iHDR->BitDepth, true); Byte++) {
                        if (Line == 0) { // Assume top and top left = 0
                            DeFilteredData[StereoView][Line][Byte] = PaethPredictor(InflatedData[StereoView][Line][Byte], 0, 0);
                        } else {
                            DeFilteredData[StereoView][Line][Byte] = PaethPredictor(InflatedData[StereoView][Line][Byte], InflatedData[StereoView][Line][Byte - PixelSize], InflatedData[StereoView][Line - 1][Byte - PixelSize]);
                        }
                    }
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
    }
    
    // These filters need to be operatable on every line in any order, so we need a main PNGOVIAFilteredImage function.
    // That function then needs to read the first byte of each line to see what filter is used
    // then OVIA each line.
    // ALSO keep in mind concurrency.
    
    libModernPNGFilterTypes ExtractLineFilterType(uint8_t *Line) {
        uint8_t FilterType = Line[0];
        return FilterType;
    }
    
    void PNGOVIAFilteredImage(ImageContainer *Image) {
        if (Image != NULL) {
            uint8_t ***DeFilteredData = calloc((Ovia->iHDR->Height * Ovia->iHDR->Width), Bits2Bytes(Ovia->iHDR->BitDepth, Yes));
            
            for (uint8_t StereoView = 0; StereoView < OVIA_PNG_GetStereoscopicStatus(Ovia); StereoView++) {
                for (size_t Line = 0; Line < Ovia->iHDR->Height; Line++) {
                    libModernPNGFilterTypes FilterType = ExtractLineFilterType(Line);
                    switch (FilterType) {
                        case NotFiltered:
                            // copy the Line except byte 0 (the filter indication byte) to the output buffer.
                            PNGOVIANonFilter(Ovia, InflatedBuffer, DeFilteredData, Line);
                            break;
                        case SubFilter:
                            // SubFilter
                            PNGOVIASubFilter(Ovia, InflatedBuffer, DeFilteredData, Line);
                            break;
                        case UpFilter:
                            // UpFilter
                            PNGOVIAUpFilter(Ovia, InflatedBuffer, DeFilteredData, Line);
                            break;
                        case AverageFilter:
                            // AverageFilter
                            PNGOVIAAverageFilter(Ovia, InflatedBuffer, DeFilteredData, Line);
                            break;
                        case PaethFilter:
                            // PaethFilter
                            PNGOVIAPaethFilter(Ovia, InflatedBuffer, DeFilteredData, Line);
                            break;
                        default:
                            Log(Log_ERROR, __func__, U8("Filter type: %d is invalid"), FilterType);
                            break;
                    }
                }
            }
            free(DeFilteredData);
        } else {
            Log(Log_ERROR, __func__, U8("ImageContainer Pointer is NULL"));
        }
    }
    
    enum DEFLATEBlockTypes {
        UncompressedBlock   = 0,
        FixedHuffmanBlock   = 1,
        DynamicHuffmanBlock = 2,
        
    };
    
    void OVIA_PNG_DecodeDeflateBlock(OVIA *Ovia, BitBuffer *DEFLATEBlock, BitBuffer *OVIAdBlock) {
        // Huffman codes are written MSBit first, everything else is writen LSBit first
        if (Ovia != NULL && BitB != NULL) {
            // DEFLATE Block header:
            bool     BlockIsFinal = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, DEFLATEBlock, 1); // BFINAL
            uint8_t  BlockType    = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, DEFLATEBlock, 2); // BTYPE
            
            if (BlockType == DynamicHuffmanBlock) {
                
            } else if (BlockType == FixedHuffmanBlock) {
                
            } else if (BlockType == UncompressedBlock) {
                
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void OVIAIFDATChunk(OVIA *Ovia, BitBuffer *DAT2OVIA, BitBuffer *OVIAdDAT, uint64_t DATSize) { // OVIAs both fDAT and IDAT chunks
        if (Ovia != NULL && BitB != NULL) {
            // well lets go ahead and allocate a DAT block the size of DATSize
            BitBuffer_Init(DATSize);
            // Now we need to go ahead and parse the ZLIB Header.
            // ok so how do we do that? I wrote some notes on the Zlib header last night...
            
            /* Compression Method and Flags byte */
            uint8_t CompressionMethod = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, DAT2OVIA, 4); // 8
            uint8_t CompressionInfo   = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, DAT2OVIA, 4); // 7
            /* Compression Method and Flags byte */
            
            /* FlagByte */
            uint8_t FCHECK            = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, DAT2OVIA, 5); // 1E
            bool    FDICTPresent      = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, DAT2OVIA, 1); // 0
            uint8_t FLEVEL            = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, DAT2OVIA, 2); // 1
            /* FlagByte */
            
            if (FDICTPresent) {
                // Read TableID which is 4 bytes
                uint32_t DICTID       = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, DAT2OVIA, 32);
            }
            
            // Start reading the DEFLATE block?
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void OVIAData(OVIA *Ovia, BitBuffer *BitB) {
        // read the iDAT/fDAT chunk header, then do the other stuff.
        if (Ovia != NULL && BitB != NULL) {
            while (BitBuffer_GetSize(BitB) > 0) { // 12 is the start of IEND
                uint32_t ChunkSize   = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
                uint32_t ChunkID     = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
                
                
                if (ChunkID == acTLMarker) {
                    ParseACTL(Ovia, BitB, ChunkSize);
                } else if (ChunkID == bKGDMarker) {
                    ParseBKGD(Ovia, BitB, ChunkSize);
                } else if (ChunkID == cHRMMarker) {
                    ParseCHRM(Ovia, BitB, ChunkSize);
                } else if (ChunkID == fcTLMarker) {
                    ParseFCTL(Ovia, BitB, ChunkSize);
                } else if (ChunkID == gAMAMarker) {
                    ParseGAMA(Ovia, BitB, ChunkSize);
                } else if (ChunkID == hISTMarker) {
                    ParseHIST(Ovia, BitB, ChunkSize);
                } else if (ChunkID == iCCPMarker) {
                    ParseOFFS(Ovia, BitB, ChunkSize);
                } else if (ChunkID == IDATMarker || ChunkID == fDATMarker) {
                    OVIAIFDATChunk(Ovia, BitB, NULL, ChunkSize);
                } else if (ChunkID == iHDRMarker) {
                    ParseIHDR(Ovia, BitB, ChunkSize);
                } else if (ChunkID == iTXtMarker) {
                    ParseITXt(Ovia, BitB, ChunkSize);
                } else if (ChunkID == oFFsMarker) {
                    ParseOFFS(Ovia, BitB, ChunkSize);
                } else if (ChunkID == pCALMarker) {
                    ParsePCAL(Ovia, BitB, ChunkSize);
                } else if (ChunkID == pHYsMarker) {
                    ParsePHYS(Ovia, BitB, ChunkSize);
                } else if (ChunkID == PLTEMarker) {
                    ParsePLTE(Ovia, BitB, ChunkSize);
                } else if (ChunkID == sBITMarker) {
                    ParseSBIT(Ovia, BitB, ChunkSize);
                } else if (ChunkID == sRGBMarker) {
                    ParseSRGB(Ovia, BitB, ChunkSize);
                } else if (ChunkID == sTERMarker) {
                    ParseSTER(Ovia, BitB, ChunkSize);
                } else if (ChunkID == tEXtMarker) {
                    ParseTEXt(Ovia, BitB, ChunkSize);
                } else if (ChunkID == zTXtMarker) {
                    ParseZTXt(Ovia, BitB, ChunkSize);
                }
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    // Let's do this library right, by adding attach and delete functions for the various chunks, and let's also have a fancy function that applies color profiles to the pixels.
    // that's kinda a lot of work tho...
    uint16_t ***OVIAImage(OVIA *Ovia, BitBuffer *PNGFile) {
        return NULL;
    }
    
    BitBuffer *OVIAAdam7(OVIA *Ovia, BitBuffer *InterlacedImage) { // Returns the interlaced image
        /*
         Ok, so to interlace an image, we need to loop over every pixel in an 8x8 block?
         */
        BitBuffer *ProgressiveImage = NULL;
        if (Ovia != NULL && BitB != NULL) {
            
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
        return ProgressiveImage;
    }
    
    void ParseIHDR(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) {
        /* When checking the CRC, you need to skip over the ChunkSize field, but include the ChunkID */
        if (Ovia != NULL && BitB != NULL) {
            uint32_t GeneratedCRC     = GenerateCRC32(BitB, ChunkSize);
            Ovia->iHDR->Width          = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            Ovia->iHDR->Height         = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            Ovia->iHDR->BitDepth       = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            Ovia->iHDR->ColorType      = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            if (Ovia->iHDR->ColorType == 1 || Ovia->iHDR->ColorType == 5 || Ovia->iHDR->ColorType >= 7) {
                Log(Log_ERROR, __func__, U8("Invalid color type: %d"), Ovia->iHDR->ColorType);
            }
            Ovia->iHDR->Compression    = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            Ovia->iHDR->FilterMethod   = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            Ovia->iHDR->Progressive    = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            
            uint32_t CRC              = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            if (GeneratedCRC != CRC) {
                Log(Log_ERROR, __func__, U8("CRC Mismatch"));
            }
            
            //VerifyCRC(Ovia->iHDR, ChunkSize, 1, 1, CRC);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void ParsePLTE(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) { // Palette
        if (Ovia != NULL && BitB != NULL) {
            if (Ovia->iHDR->BitDepth > 8) { // INVALID
                Log(Log_ERROR, __func__, U8("Invalid bit depth %d and palette combination"), Ovia->iHDR->BitDepth);
                BitBuffer_Skip(BitB, Bytes2Bits(ChunkSize));
            } else {
            if (Ovia->iHDR->ColorType == PNG_PalettedRGB || Ovia->iHDR->ColorType == PNG_RGB) {
                Ovia->PLTE->Palette = calloc(1, (3 * Ovia->PLTE->NumEntries) * Bits2Bytes(Ovia->iHDR->BitDepth, Yes));
            } else if (Ovia->iHDR->ColorType == PNG_RGBA) {
                Ovia->PLTE->Palette = calloc(1, (4 * Ovia->PLTE->NumEntries) * Bits2Bytes(Ovia->iHDR->BitDepth, Yes));
            }
            
            
            for (uint8_t Channel = 0; Channel < ModernPNGChannelsPerColorType[Ovia->iHDR->ColorType]; Channel++) {
                for (uint16_t PaletteEntry = 0; PaletteEntry < ChunkSize / 3; PaletteEntry++) {
                    Ovia->PLTE->Palette[Channel][PaletteEntry] = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, Ovia->iHDR->BitDepth);
                }
            }
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void ParseTRNS(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) { // Transparency
        if (Ovia != NULL && BitB != NULL) {
            Ovia->tRNS->NumEntries = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            uint16_t **Entries    = NULL;
            if (Ovia->iHDR->ColorType == PNG_RGB) {
                Entries = calloc(3, Bits2Bytes(Ovia->iHDR->BitDepth, true) * sizeof(uint16_t));
            } else if (Ovia->iHDR->ColorType == PNG_RGBA) {
                Entries = calloc(4, Bits2Bytes(Ovia->iHDR->BitDepth, true) * sizeof(uint16_t));
            } else if (Ovia->iHDR->ColorType == PNG_Grayscale) {
                Entries = calloc(1, Bits2Bytes(Ovia->iHDR->BitDepth, true) * sizeof(uint16_t));
            } else if (Ovia->iHDR->ColorType == PNG_GrayAlpha) {
                Entries = calloc(2, Bits2Bytes(Ovia->iHDR->BitDepth, true) * sizeof(uint16_t));
            }
            if (Entries != NULL) {
                for (uint8_t Color = 0; Color < ModernPNGChannelsPerColorType[Ovia->iHDR->ColorType]; Color++) {
                    Entries[Color]    = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, Bits2Bytes(Ovia->iHDR->BitDepth, true));
                }
                //Ovia->tRNS->Palette = Entries;
            } else {
                Log(Log_ERROR, __func__, U8("Failed to allocate enough memory for the TRNS Palette"));
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void ParseBKGD(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) { // Background
        if (Ovia != NULL && BitB != NULL) {
            for (uint8_t Entry = 0; Entry < 3; Entry++) {
                Ovia->bkGD->BackgroundPaletteEntry[Entry] = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void ParseCHRM(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) { // Chromaticities
        if (Ovia != NULL && BitB != NULL) {
            Ovia->cHRM->WhitePointX = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            Ovia->cHRM->WhitePointY = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            Ovia->cHRM->RedX        = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            Ovia->cHRM->RedY        = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            Ovia->cHRM->GreenX      = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            Ovia->cHRM->GreenY      = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            Ovia->cHRM->BlueX       = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            Ovia->cHRM->BlueY       = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void ParseGAMA(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) { // Gamma
        if (Ovia != NULL && BitB != NULL) {
            Ovia->gAMA->Gamma = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void ParseOFFS(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) { // Image Offset
        if (Ovia != NULL && BitB != NULL) {
            Ovia->oFFs->XOffset       = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            Ovia->oFFs->YOffset       = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            Ovia->oFFs->UnitSpecifier = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void ParsePHYS(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) { // Aspect ratio, Physical pixel size
        if (Ovia != NULL && BitB != NULL) {
            Ovia->pHYs->PixelsPerUnitXAxis = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            Ovia->pHYs->PixelsPerUnitYAxis = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            Ovia->pHYs->UnitSpecifier      = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void ParseSCAL(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) { // Physical Scale
        // Ok, so we need to take the size of the chunk into account.
        if (Ovia != NULL && BitB != NULL) {
            Ovia->sCAL->UnitSpecifier = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8); // 1 = Meter, 2 = Radian
            
            uint32_t WidthStringSize  = 0;
            uint32_t HeightStringSize = 0;
            
            for (uint32_t Byte = 0UL; Byte < ChunkSize - 1; Byte++) { // Get the sizes for the field strings
                if (BitBuffer_PeekBits(MSByteFirst, LSBitFirst, BitB, 8) != '\0') {
                    WidthStringSize += 1;
                }
            }
            HeightStringSize = ChunkSize - (WidthStringSize + 1);
            
            UTF8 *WidthString = calloc(WidthStringSize, sizeof(UTF8));
            UTF8 *HeightString = calloc(HeightStringSize, sizeof(UTF8));
            
            for (uint32_t WidthCodePoint = 0; WidthCodePoint < WidthStringSize; WidthCodePoint++) {
                WidthString[WidthCodePoint] = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            }
            BitBuffer_Skip(BitB, 8); // Skip the NULL seperator
            for (uint32_t HeightCodePoint = 0; HeightCodePoint < HeightStringSize; HeightCodePoint++) {
                HeightString[HeightCodePoint] = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            }
            
            Ovia->sCAL->PixelWidth  = UTF8_String2Oviaimal(WidthString);
            Ovia->sCAL->PixelHeight = UTF8_String2Oviaimal(HeightString);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void ParsePCAL(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) {
        if (Ovia != NULL && BitB != NULL) {
            char CalibrationName[80];
            while (BitBuffer_PeekBits(MSByteFirst, LSBitFirst, BitB, 8) != 0x0) {
                for (uint8_t Byte = 0; Byte < 80; Byte++) {
                    CalibrationName[Byte] = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
                }
            }
            Ovia->pCAL->CalibrationName     = CalibrationName;
            Ovia->pCAL->CalibrationNameSize = UTF8_GetStringSizeInCodePoints(Ovia->pCAL->CalibrationName);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void ParseSBIT(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) { // Significant bits per sample
        if (Ovia != NULL && BitB != NULL) {
            Ovia->sBIT->Red                   = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            Ovia->sBIT->Green                 = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            Ovia->sBIT->Blue                  = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void ParseSRGB(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) {
        if (Ovia != NULL && BitB != NULL) {
            Ovia->sRGB->RenderingIntent       = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void ParseSTER(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) {
        if (Ovia != NULL && BitB != NULL) {
            Ovia->PNGIs3D                     = true;
            Ovia->sTER->StereoType            = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            
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
    
    void ParseTEXt(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) { // Uncompressed, ASCII tEXt
        if (Ovia != NULL && BitB != NULL) {
            // Read until you hit a NULL for the name string, then subtract the size of the previous string from the total length to get the number of bytes for the second string
            uint8_t  KeywordSize = 0UL;
            uint8_t  CurrentByte = 0; // 1 is BULLshit
            
            do {
                CurrentByte  = BitBuffer_PeekBits(MSByteFirst, LSBitFirst, BitB, 8);
                KeywordSize += 1;
            } while (CurrentByte != 0);
            
            uint32_t CommentSize = ChunkSize - KeywordSize;
            
            Ovia->NumTextChunks  += 1;
            
            
            
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
    
    void ParseTIME(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) {
        if (Ovia != NULL && BitB != NULL) {
            Ovia->tIMe->Year                  = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 16);
            Ovia->tIMe->Month                 = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            Ovia->tIMe->Day                   = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            Ovia->tIMe->Hour                  = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            Ovia->tIMe->Minute                = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            Ovia->tIMe->Second                = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    /* APNG */
    void ParseACTL(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) { // Animation control, part of APNG
        if (Ovia != NULL && BitB != NULL) {
            Ovia->PNGIsAnimated               = true;
            Ovia->acTL->NumFrames             = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            Ovia->acTL->TimesToLoop           = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32); // If 0, loop forever.
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void ParseFCTL(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) { // Frame Control, part of APNG
        if (Ovia != NULL && BitB != NULL) {
            OVIA_PNG_FCTL_Set
            Ovia->fcTL->FrameNum              = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            Ovia->fcTL->Width                 = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            Ovia->fcTL->Height                = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            Ovia->fcTL->XOffset               = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            Ovia->fcTL->YOffset               = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            Ovia->fcTL->FrameDelayNumerator   = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 16);
            Ovia->fcTL->FrameDelayDenominator = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 16);
            Ovia->fcTL->DisposeMethod         = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            Ovia->fcTL->BlendMethod           = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    /* End APNG */
    
    void ParseHIST(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) {
        if (Ovia != NULL && BitB != NULL) {
            
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void ParseICCP(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) {
        if (Ovia != NULL && BitB != NULL) {
            
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
        uint8_t ProfileNameSize = 0;
        ProfileNameSize = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
    }
    
    void ParseSPLT(OVIA *Ovia, BitBuffer *BitB, uint32_t ChunkSize) {
        if (Ovia != NULL && BitB != NULL) {
            
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    uint8_t ParsePNGMetadata(BitBuffer *BitB, OVIA *Ovia) {
        if (Ovia != NULL && BitB != NULL) {
            char     *ChunkID        = calloc(4, sizeof(uint8_t));
            uint32_t ChunkSize       = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            
            for (uint8_t ChunkIDSize = 0; ChunkIDSize < 4; ChunkIDSize++) {
                ChunkID[ChunkIDSize] = BitBuffer_ReadBits(MSByteFirst, LSBitFirst, BitB, 8);
            }
            BitBuffer_Skip(BitB, -32); // Now we need to skip back so we can read the ChunkID as part of the CRC check.
            // Now we call the VerifyCRC32 function with the ChunkSize
            VerifyCRC32(BitB, ChunkSize);
            
            if (*ChunkID == iHDRMarker) {        // iHDR
                ParseIHDR(Ovia, BitB, ChunkSize);
            } else if (*ChunkID == PLTEMarker) { // PLTE
                ParsePLTE(Ovia, BitB, ChunkSize);
            } else if (*ChunkID == bKGDMarker) { // bKGD
                Ovia->bkGDExists = true;
                ParseBKGD(Ovia, BitB, ChunkSize);
            } else if (*ChunkID == cHRMMarker) { // cHRM
                Ovia->cHRMExists = true;
                ParseCHRM(Ovia, BitB, ChunkSize);
            } else if (*ChunkID == gAMAMarker) { // gAMA
                Ovia->gAMAExists = true;
                ParseGAMA(Ovia, BitB, ChunkSize);
            } else if (*ChunkID == oFFsMarker) { // oFFs
                Ovia->oFFsExists = true;
                ParseOFFS(Ovia, BitB, ChunkSize);
            } else if (*ChunkID == pHYsMarker) { // pHYs
                Ovia->pHYsExists = true;
                ParsePHYS(Ovia, BitB, ChunkSize);
            } else if (*ChunkID == sBITMarker) { // sBIT
                Ovia->sBITExists = true;
                ParseSBIT(Ovia, BitB, ChunkSize);
            } else if (*ChunkID == sCALMarker) { // sCAL
                Ovia->sCALExists = true;
                ParseSCAL(Ovia, BitB, ChunkSize);
            } else if (*ChunkID == sRGBMarker) { // sRGB
                Ovia->sRGBExists = true;
                ParseSRGB(Ovia, BitB, ChunkSize);
            } else if (*ChunkID == sTERMarker) { // sTER
                Ovia->sTERExists = true;
                ParseSTER(Ovia, BitB, ChunkSize);
            } else if (*ChunkID == tEXtMarker) { // tEXt
                Ovia->TextExists = true;
                ParseTEXt(Ovia, BitB, ChunkSize);
            } else if (*ChunkID == zTXtMarker) { // zTXt
                Ovia->TextExists = true;
                ParseZTXt(Ovia, BitB, ChunkSize);
            } else if (*ChunkID == iTXtMarker) { // iTXt
                Ovia->TextExists = true;
                ParseITXt(Ovia, BitB, ChunkSize);
            } else if (*ChunkID == tIMEMarker) { // tIME
                Ovia->tIMEExists = true;
                ParseTIME(Ovia, BitB, ChunkSize);
            } else if (*ChunkID == tRNSMarker) { // tRNS
                Ovia->tRNSExists = true;
                ParseTRNS(Ovia, BitB, ChunkSize);
            } else if (*ChunkID == hISTMarker && Ovia->PLTEExists) { // hIST
                Ovia->hISTExists = true;
                ParseHIST(Ovia, BitB, ChunkSize);
            } else if (*ChunkID == iCCPMarker) { // iCCP
                Ovia->iCCPExists = true;
                ParseICCP(Ovia, BitB, ChunkSize);
            } else if (*ChunkID == pCALMarker) { // pCAL
                Ovia->pCALExists = true;
                ParsePCAL(Ovia, BitB, ChunkSize);
            } else if (*ChunkID == sPLTMarker) { // sPLT
                Ovia->sPLTExists = true;
                ParseSPLT(Ovia, BitB, ChunkSize);
            } else if (*ChunkID == acTLMarker) { // acTL
                Ovia->acTLExists = true;
                ParseACTL(Ovia, BitB, ChunkSize);
            } else if (*ChunkID == fcTLMarker) { // fcTL
                Ovia->fcTLExists = true;
                ParseFCTL(Ovia, BitB, ChunkSize);
            }
            free(ChunkID);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    ImageContainer *PNGExtractImage(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
#ifdef __cplusplus
}
#endif
