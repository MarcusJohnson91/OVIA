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
        uint32_t ChunkCRC = ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
        bool CRCsMatch = VerifyCRC(Buffer2CRC, ChunkSize, 0x82608EDB, 32, 0xFFFFFFFF, ChunkCRC);
        SkipBits(BitB, -Bytes2Bits(ChunkSize));
        return CRCsMatch;
    }
    */
    
    uint8_t PaethPredictor(int64_t Left, int64_t Above, int64_t UpperLeft) {
        int64_t Guess     = Left + Above - UpperLeft;
        int64_t DistanceA = llabs(Guess - Left);
        int64_t DistanceB = llabs(Guess - Above);
        int64_t DistanceC = llabs(Guess - UpperLeft);
        
        uint8_t Output = 0;
        if (DistanceA <= DistanceB && DistanceA < DistanceC) {
            Output = DistanceA;
        } else if (DistanceB < DistanceC) {
            Output = DistanceB;
        } else {
            Output = DistanceC;
        }
        return Output;
    }
    
    void PNGDecodeNonFilter(DecodePNG *Dec, uint8_t ***InflatedData, uint8_t ***DeFilteredData, size_t Line) {
        for (uint8_t StereoView = 0; StereoView < Dec->sTER->StereoType; StereoView++) {
            for (uint32_t Width = 0; Width < Dec->iHDR->Width; Width++) {
                for (size_t Byte = 1; Byte < Bits2Bytes(Dec->iHDR->BitDepth, true); Byte++) {
                    DeFilteredData[StereoView][Width][Byte - 1] = InflatedData[StereoView][Width][Byte]; // Remove filter indicating byte
                }
            }
        }
    }
    
    void PNGDecodeSubFilter(DecodePNG *Dec, uint8_t ***InflatedData, uint8_t ***DeFilteredData, size_t Line) {
        for (uint8_t StereoView = 0; StereoView < Dec->sTER->StereoType; StereoView++) {
            for (uint32_t Width = 0; Width < Dec->iHDR->Width; Width++) {
                for (size_t Byte = 1; Byte < Bits2Bytes(Dec->iHDR->BitDepth, true); Byte++) {
                    DeFilteredData[StereoView][Line][Byte - 1] = (InflatedData[StereoView][Line][Byte] + InflatedData[StereoView][Line][Byte+1]) & 0xFF; // Byte+1 is wrong, needs to be + bitdepth
                }
            }
        }
    }
    
    void PNGDecodeUpFilter(DecodePNG *Dec, uint8_t ***InflatedData, uint8_t ***DeFilteredData, size_t Line) {
        for (uint8_t StereoView = 0; StereoView < Dec->sTER->StereoType; StereoView++) {
            for (uint32_t Width = 0; Width < Dec->iHDR->Width; Width++) {
                for (size_t Byte = 1; Byte < Bits2Bytes(Dec->iHDR->BitDepth, true); Byte++) {
                    DeFilteredData[StereoView][Line][Byte - 1] = InflatedData[StereoView][Line][Byte] + InflatedData[StereoView][Line - 1][Byte] & 0xFF;
                }
            }
        }
    }
    
    void PNGDecodeAverageFilter(DecodePNG *Dec, uint8_t ***InflatedData, uint8_t ***DeFilteredData, size_t Line) {
        uint8_t PixelSize = Bits2Bytes(Dec->iHDR->BitDepth, true);
        for (uint8_t StereoView = 0; StereoView < Dec->sTER->StereoType; StereoView++) {
        for (uint32_t Width = 0; Width < Dec->iHDR->Width; Width++) {
            for (size_t Byte = 1; Byte < Bits2Bytes(Dec->iHDR->BitDepth, true); Byte++) {
                uint8_t Average = floor((InflatedData[StereoView][Line][Byte - (PixelSize)] + InflatedData[StereoView][Line - 1][Byte]) / 2);
                DeFilteredData[StereoView][Line][Byte - 1] = InflatedData[StereoView][Line][Byte] + Average;
            }
        }
        }
    }
    
    void PNGDecodePaethFilter(DecodePNG *Dec, uint8_t ***InflatedData, uint8_t ***DeFilteredData, size_t Line) {
        // Filtering is applied to bytes, not pixels
        uint8_t PixelSize = Bits2Bytes(Dec->iHDR->BitDepth, true);
        for (uint8_t StereoView = 0; StereoView < Dec->sTER->StereoType; StereoView++) {
        for (uint32_t Width = 0; Width < Dec->iHDR->Width; Width++) {
            for (size_t Byte = 1; Byte < Bits2Bytes(Dec->iHDR->BitDepth, true); Byte++) {
                if (Line == 0) { // Assume top and top left = 0
                    DeFilteredData[StereoView][Line][Byte] = PaethPredictor(InflatedData[StereoView][Line][Byte], 0, 0);
                } else {
                    DeFilteredData[StereoView][Line][Byte] = PaethPredictor(InflatedData[StereoView][Line][Byte], InflatedData[StereoView][Line][Byte - PixelSize], InflatedData[StereoView][Line - 1][Byte - PixelSize]);
                }
            }
        }
        }
    }
    
    // These filters need to be operatable on every line in any order, so we need a main PNGDecodeFilteredImage function.
    // That function then needs to read the first byte of each line to see what filter is used
    // then decode each line.
    // ALSO keep in mind concurrency.
    
    libModernPNGFilterTypes ExtractLineFilterType(uint8_t *Line) {
        uint8_t FilterType = Line[0];
        return FilterType;
    }
    
    void PNGDecodeFilteredImage(DecodePNG *Dec, uint8_t ***InflatedBuffer) {
        
		uint8_t ***DeFilteredData = calloc((Dec->iHDR->Height * Dec->iHDR->Width), Bits2Bytes(Dec->iHDR->BitDepth, Yes));
        
        for (uint8_t StereoView = 0; StereoView < PNGGetStereoscopicStatus(Dec); StereoView++) {
            for (size_t Line = 0; Line < Dec->iHDR->Height; Line++) {
                libModernPNGFilterTypes FilterType = ExtractLineFilterType(Line);
                switch (FilterType) {
                    case NotFiltered:
                        // copy the Line except byte 0 (the filter indication byte) to the output buffer.
                        PNGDecodeNonFilter(Dec, InflatedBuffer, DeFilteredData, Line);
                        break;
                    case SubFilter:
                        // SubFilter
                        PNGDecodeSubFilter(Dec, InflatedBuffer, DeFilteredData, Line);
                        break;
                    case UpFilter:
                        // UpFilter
                        PNGDecodeUpFilter(Dec, InflatedBuffer, DeFilteredData, Line);
                        break;
                    case AverageFilter:
                        // AverageFilter
                        PNGDecodeAverageFilter(Dec, InflatedBuffer, DeFilteredData, Line);
                        break;
                    case PaethFilter:
                        // PaethFilter
                        PNGDecodePaethFilter(Dec, InflatedBuffer, DeFilteredData, Line);
                        break;
                    default:
                        Log(Log_ERROR, __func__, U8("Filter type: %d is invalid"), FilterType);
                        break;
                }
            }
        }
        free(DeFilteredData);
    }
    
    enum DEFLATEBlockTypes {
        UncompressedBlock   = 0,
        FixedHuffmanBlock   = 1,
        DynamicHuffmanBlock = 2,
        
    };
    
    void DecodeDEFLATEBlock(DecodePNG *Dec, BitBuffer *DEFLATEBlock, BitBuffer *DecodedBlock) { // LSByteFirst
        // Huffman codes are written MSBit first, everything else is writen LSBit first
        
        // DEFLATE Block header:
        bool     BlockIsFinal = ReadBits(MSByteFirst, LSBitFirst, DEFLATEBlock, 1); // BFINAL
        uint8_t  BlockType    = ReadBits(MSByteFirst, LSBitFirst, DEFLATEBlock, 2); // BTYPE
        
        if (BlockType == DynamicHuffmanBlock) {
            
        } else if (BlockType == FixedHuffmanBlock) {
            
        } else if (BlockType == UncompressedBlock) {
            
        }
    }
    
    void DecodeIFDATChunk(DecodePNG *Dec, BitBuffer *DAT2Decode, BitBuffer *DecodedDAT, uint64_t DATSize) { // Decodes both fDAT and IDAT chunks
        // well lets go ahead and allocate a DAT block the size of DATSize
        BitBuffer_Init(DATSize);
        // Now we need to go ahead and parse the ZLIB Header.
        // ok so how do we do that? I wrote some notes on the Zlib header last night...
        
        /* Compression Method and Flags byte */
        uint8_t CompressionMethod = ReadBits(MSByteFirst, LSBitFirst, DAT2Decode, 4); // 8
        uint8_t CompressionInfo   = ReadBits(MSByteFirst, LSBitFirst, DAT2Decode, 4); // 7
        /* Compression Method and Flags byte */
        
        /* FlagByte */
        uint8_t FCHECK            = ReadBits(MSByteFirst, LSBitFirst, DAT2Decode, 5); // 1E
        bool    FDICTPresent      = ReadBits(MSByteFirst, LSBitFirst, DAT2Decode, 1); // 0
        uint8_t FLEVEL            = ReadBits(MSByteFirst, LSBitFirst, DAT2Decode, 2); // 1
        /* FlagByte */
        
        if (FDICTPresent) {
            // Read TableID which is 4 bytes
            uint32_t DICTID       = ReadBits(MSByteFirst, LSBitFirst, DAT2Decode, 32);
        }
        
        // Start reading the DEFLATE block?
         
    }
    
    void DecodePNGData(DecodePNG *Dec, BitBuffer *BitB) {
        // read the iDAT/fDAT chunk header, then do the other stuff.
        while (BitBuffer_GetSize(BitB) > 0) { // 12 is the start of IEND
            uint32_t ChunkSize   = ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            uint32_t ChunkID     = ReadBits(MSByteFirst, LSBitFirst, BitB, 32);
            
            
            if (ChunkID == acTLMarker) {
                ParseACTL(Dec, BitB, ChunkSize);
            } else if (ChunkID == bKGDMarker) {
                ParseBKGD(Dec, BitB, ChunkSize);
            } else if (ChunkID == cHRMMarker) {
                ParseCHRM(Dec, BitB, ChunkSize);
            } else if (ChunkID == fcTLMarker) {
                ParseFCTL(Dec, BitB, ChunkSize);
            } else if (ChunkID == gAMAMarker) {
                ParseGAMA(Dec, BitB, ChunkSize);
            } else if (ChunkID == hISTMarker) {
                ParseHIST(Dec, BitB, ChunkSize);
            } else if (ChunkID == iCCPMarker) {
                ParseOFFS(Dec, BitB, ChunkSize);
            } else if (ChunkID == IDATMarker || ChunkID == fDATMarker) {
                DecodeIFDATChunk(Dec, BitB, NULL, ChunkSize);
            } else if (ChunkID == iHDRMarker) {
                ParseIHDR(Dec, BitB, ChunkSize);
            } else if (ChunkID == iTXtMarker) {
                ParseITXt(Dec, BitB, ChunkSize);
            } else if (ChunkID == oFFsMarker) {
                ParseOFFS(Dec, BitB, ChunkSize);
            } else if (ChunkID == pCALMarker) {
                ParsePCAL(Dec, BitB, ChunkSize);
            } else if (ChunkID == pHYsMarker) {
                ParsePHYS(Dec, BitB, ChunkSize);
            } else if (ChunkID == PLTEMarker) {
                ParsePLTE(Dec, BitB, ChunkSize);
            } else if (ChunkID == sBITMarker) {
                ParseSBIT(Dec, BitB, ChunkSize);
            } else if (ChunkID == sRGBMarker) {
                ParseSRGB(Dec, BitB, ChunkSize);
            } else if (ChunkID == sTERMarker) {
                ParseSTER(Dec, BitB, ChunkSize);
            } else if (ChunkID == tEXtMarker) {
                ParseTEXt(Dec, BitB, ChunkSize);
            } else if (ChunkID == zTXtMarker) {
                ParseZTXt(Dec, BitB, ChunkSize);
            }
        }
    }
    
    // Let's do this library right, by adding attach and delete functions for the various chunks, and let's also have a fancy function that applies color profiles to the pixels.
    // that's kinda a lot of work tho...
    uint16_t ***DecodePNGImage(DecodePNG *Dec, BitBuffer *PNGFile) {
        return NULL;
    }
    
    BitBuffer *DecodeAdam7(DecodePNG *Dec, BitBuffer *InterlacedImage) { // Returns the interlaced image
        /*
         Ok, so to interlace an image, we need to loop over every pixel in an 8x8 block?
         */
        BitBuffer *ProgressiveImage = NULL;
        return ProgressiveImage;
    }
    
    Container  *PNGDecodeImage2(DecodePNG *Dec, BitBuffer *PNG2Decode, uint16_t GammaCorrect) {
        return NULL;
    }
    
    void ParseIHDR(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) {
        /* When checking the CRC, you need to skip over the ChunkSize field, but include the ChunkID */
        uint32_t GeneratedCRC     = GenerateCRC32(InputPNG, ChunkSize);
        Dec->iHDR->Width          = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 32);
        Dec->iHDR->Height         = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 32);
        Dec->iHDR->BitDepth       = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 8);
        Dec->iHDR->ColorType      = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 8);
        if (Dec->iHDR->ColorType == 1 || Dec->iHDR->ColorType == 5 || Dec->iHDR->ColorType >= 7) {
            Log(Log_ERROR, __func__, U8("Invalid color type: %d"), Dec->iHDR->ColorType);
        }
        Dec->iHDR->Compression    = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 8);
        Dec->iHDR->FilterMethod   = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 8);
        Dec->iHDR->IsInterlaced   = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 8);
        
        uint32_t CRC              = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 32);
        if (GeneratedCRC != CRC) {
            Log(Log_ERROR, __func__, U8("CRC Mismatch"));
        }
        
        //VerifyCRC(Dec->iHDR, ChunkSize, 1, 1, CRC);
    }
    
    void ParsePLTE(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Palette
        if (Dec->iHDR->BitDepth > 8) { // INVALID
            Log(Log_ERROR, __func__, U8("Invalid bit depth %d and palette combination"), Dec->iHDR->BitDepth);
            BitBuffer_Skip(InputPNG, Bytes2Bits(ChunkSize));
        } else {
            
            if (Dec->iHDR->ColorType == PNG_PalettedRGB || Dec->iHDR->ColorType == PNG_RGB) {
                Dec->PLTE->Palette = calloc(1, (3 * Dec->PLTE->NumEntries) * Bits2Bytes(Dec->iHDR->BitDepth, Yes));
            } else if (Dec->iHDR->ColorType == PNG_RGBA) {
                Dec->PLTE->Palette = calloc(1, (4 * Dec->PLTE->NumEntries) * Bits2Bytes(Dec->iHDR->BitDepth, Yes));
            }
            
            
            for (uint8_t Channel = 0; Channel < ModernPNGChannelsPerColorType[Dec->iHDR->ColorType]; Channel++) {
                for (uint16_t PaletteEntry = 0; PaletteEntry < ChunkSize / 3; PaletteEntry++) {
                    Dec->PLTE->Palette[Channel][PaletteEntry] = ReadBits(MSByteFirst, LSBitFirst, InputPNG, Dec->iHDR->BitDepth);
                }
            }
        }
    }
    
    void ParseTRNS(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Transparency
        Dec->tRNS->NumEntries = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 32);
        uint16_t **Entries    = NULL;
        if (Dec->iHDR->ColorType == PNG_RGB) {
            Entries = calloc(3, Bits2Bytes(Dec->iHDR->BitDepth, true) * sizeof(uint16_t));
        } else if (Dec->iHDR->ColorType == PNG_RGBA) {
            Entries = calloc(4, Bits2Bytes(Dec->iHDR->BitDepth, true) * sizeof(uint16_t));
        } else if (Dec->iHDR->ColorType == PNG_Grayscale) {
            Entries = calloc(1, Bits2Bytes(Dec->iHDR->BitDepth, true) * sizeof(uint16_t));
        } else if (Dec->iHDR->ColorType == PNG_GrayAlpha) {
            Entries = calloc(2, Bits2Bytes(Dec->iHDR->BitDepth, true) * sizeof(uint16_t));
        }
        if (Entries == NULL) {
            Log(Log_ERROR, __func__, U8("Failed to allocate enough memory for the TRNS Palette"));
        } else {
            for (uint8_t Color = 0; Color < ModernPNGChannelsPerColorType[Dec->iHDR->ColorType]; Color++) {
                Entries[Color]    = ReadBits(MSByteFirst, LSBitFirst, InputPNG, Bits2Bytes(Dec->iHDR->BitDepth, true));
            }
            //Dec->tRNS->Palette = Entries;
        }
    }
    
    void ParseBKGD(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Background
        for (uint8_t Entry = 0; Entry < 3; Entry++) {
            Dec->bkGD->BackgroundPaletteEntry[Entry] = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 8);
        }
    }
    
    void ParseCHRM(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Chromaticities
        Dec->cHRM->WhitePointX = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 32);
        Dec->cHRM->WhitePointY = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 32);
        Dec->cHRM->RedX        = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 32);
        Dec->cHRM->RedY        = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 32);
        Dec->cHRM->GreenX      = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 32);
        Dec->cHRM->GreenY      = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 32);
        Dec->cHRM->BlueX       = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 32);
        Dec->cHRM->BlueY       = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 32);
    }
    
    void ParseGAMA(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Gamma
        Dec->gAMA->Gamma = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 32);
    }
    
    void ParseOFFS(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Image Offset
        Dec->oFFs->XOffset       = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 32);
        Dec->oFFs->YOffset       = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 32);
        Dec->oFFs->UnitSpecifier = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 8);
    }
    
    void ParsePHYS(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Aspect ratio, Physical pixel size
        Dec->pHYs->PixelsPerUnitXAxis = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 32);
        Dec->pHYs->PixelsPerUnitYAxis = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 32);
        Dec->pHYs->UnitSpecifier      = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 8);
    }
    
    void ParseSCAL(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Physical Scale
        // Ok, so we need to take the size of the chunk into account.
        Dec->sCAL->UnitSpecifier = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 8); // 1 = Meter, 2 = Radian
        
        uint32_t WidthStringSize  = 0;
        uint32_t HeightStringSize = 0;
        
        for (uint32_t Byte = 0UL; Byte < ChunkSize - 1; Byte++) { // Get the sizes for the field strings
            if (PeekBits(MSByteFirst, LSBitFirst, InputPNG, 8) != '\0') {
                WidthStringSize += 1;
            }
        }
        HeightStringSize = ChunkSize - (WidthStringSize + 1);
        
        UTF8 *WidthString = calloc(WidthStringSize, sizeof(UTF8));
        UTF8 *HeightString = calloc(HeightStringSize, sizeof(UTF8));
        
        for (uint32_t WidthCodePoint = 0; WidthCodePoint < WidthStringSize; WidthCodePoint++) {
            WidthString[WidthCodePoint] = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 8);
        }
        BitBuffer_Skip(InputPNG, 8); // Skip the NULL seperator
        for (uint32_t HeightCodePoint = 0; HeightCodePoint < HeightStringSize; HeightCodePoint++) {
            HeightString[HeightCodePoint] = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 8);
        }
        
        Dec->sCAL->PixelWidth  = UTF8_String2Decimal(WidthString);
        Dec->sCAL->PixelHeight = UTF8_String2Decimal(HeightString);
    }
    
    void ParsePCAL(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) {
        char CalibrationName[80];
        while (PeekBits(MSByteFirst, LSBitFirst, InputPNG, 8) != 0x0) {
            for (uint8_t Byte = 0; Byte < 80; Byte++) {
                CalibrationName[Byte] = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 8);
            }
        }
        Dec->pCAL->CalibrationName     = CalibrationName;
        Dec->pCAL->CalibrationNameSize = UTF8_GetStringSizeInCodePoints(Dec->pCAL->CalibrationName);
    }
    
    void ParseSBIT(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Significant bits per sample
        Dec->sBIT->Red                   = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 8);
        Dec->sBIT->Green                 = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 8);
        Dec->sBIT->Blue                  = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 8);
    }
    
    void ParseSRGB(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) {
        Dec->sRGB->RenderingIntent       = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 8);
    }
    
    void ParseSTER(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) {
        Dec->PNGIs3D                     = true;
        Dec->sTER->StereoType            = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 8);
        
        // No matter what StereoType is used, both images are arranged side by side, and the left edge is aligned on a boundary of the 8th column in case interlacing is used.
        // The two sub images must have the same dimensions after padding is removed.
        // CROSS_FUSE_LAYOUT = 0, DIVERGING_FUSE_LAYOUT = 1
        // When CROSS_FUSE_LAYOUT, the right image is on the left, and the left image on the right.
        // When DIVERGING_FUSE_LAYOUT, The left eye view is on the left, and the right eye view is on the right.
        // The Left eye view is always in index 0 of the pixel array, so we need to swap this if nessicary.
        // So, make a function that strips padding from the stream, then make a function that decodes the sTER image the way it should be.
        // The standard recommends we use CROSS_FUSE_LAYOUT, so I'll probably end up using that.
        // So, let's say we have 2 1080p images, and we want to make a single Stereoscopic PNG from them.
        //
    }
    
    void ParseTEXt(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Uncompressed, ASCII tEXt
        // Read until you hit a NULL for the name string, then subtract the size of the previous string from the total length to get the number of bytes for the second string
        uint8_t  KeywordSize = 0UL;
        uint8_t  CurrentByte = 0; // 1 is BULLshit
        
        do {
            CurrentByte  = PeekBits(MSByteFirst, LSBitFirst, InputPNG, 8);
            KeywordSize += 1;
        } while (CurrentByte != 0);
        
        uint32_t CommentSize = ChunkSize - KeywordSize;
        
        Dec->NumTextChunks  += 1;
        
        
        
        // first we need to get the size of the strings
        
        // for that we would theoretically use PeekBits, but PeekBits is limited to 64 bits read at once, max.
        // there can be up to 800 bits before we hit the NULL, so what do...
        
        // Well, we can use SeekBits to jump byte by byte but it's not optimal...
        
        
        
        // Ok, once we've gotten the size of the Keyword string
        
        
        // Then read the Comment string.
        
        // Now, how do we handle multiple text strings?
        // Why not just have a count of the text chunks?
        // Store a variable in DecodePNG called NumTextChunks, then store a type called Comment or something that stores both the comment type as a string and the actual comment...
        
        
        
        
    }
    
    void ParseZTXt(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Compressed text
    }
    
    void ParseITXt(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // International Text
    }
    
    void ParseTIME(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) {
        Dec->tIMe->Year                  = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 16);
        Dec->tIMe->Month                 = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 8);
        Dec->tIMe->Day                   = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 8);
        Dec->tIMe->Hour                  = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 8);
        Dec->tIMe->Minute                = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 8);
        Dec->tIMe->Second                = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 8);
    }
    
    /* APNG */
    void ParseACTL(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Animation control, part of APNG
        Dec->PNGIsAnimated               = true;
        Dec->acTL->NumFrames             = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 32);
        Dec->acTL->TimesToLoop           = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 32); // If 0, loop forever.
    }
    
    void ParseFCTL(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Frame Control, part of APNG
        Dec->fcTL->FrameNum              = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 32);
        Dec->fcTL->Width                 = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 32);
        Dec->fcTL->Height                = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 32);
        Dec->fcTL->XOffset               = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 32);
        Dec->fcTL->YOffset               = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 32);
        Dec->fcTL->FrameDelayNumerator   = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 16);
        Dec->fcTL->FrameDelayDenominator = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 16);
        Dec->fcTL->DisposeMethod         = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 8);
        Dec->fcTL->BlendMethod           = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 8);
    }
    /* End APNG */
    
    void ParseHIST(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) {
    }
    
    void ParseICCP(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) {
        uint8_t ProfileNameSize = 0;
        ProfileNameSize = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 8);
    }
    
    void ParseSPLT(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) {
    }
    
    uint8_t ParsePNGMetadata(BitBuffer *InputPNG, DecodePNG *Dec) {
        uint64_t FileMagic    = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 64);
        
        if (FileMagic == PNGMagic) {
            char     *ChunkID        = calloc(4, sizeof(uint8_t));
            uint32_t ChunkSize       = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 32);
            
            for (uint8_t ChunkIDSize = 0; ChunkIDSize < 4; ChunkIDSize++) {
                ChunkID[ChunkIDSize] = ReadBits(MSByteFirst, LSBitFirst, InputPNG, 8);
            }
            BitBuffer_Skip(InputPNG, -32); // Now we need to skip back so we can read the ChunkID as part of the CRC check.
            // Now we call the VerifyCRC32 function with the ChunkSize
            VerifyCRC32(InputPNG, ChunkSize);
            
            if (*ChunkID == iHDRMarker) {        // iHDR
                ParseIHDR(Dec, InputPNG, ChunkSize);
            } else if (*ChunkID == PLTEMarker) { // PLTE
                Dec->PLTEExists = true;
                ParsePLTE(Dec, InputPNG, ChunkSize);
            } else if (*ChunkID == bKGDMarker) { // bKGD
                Dec->bkGDExists = true;
                ParseBKGD(Dec, InputPNG, ChunkSize);
            } else if (*ChunkID == cHRMMarker) { // cHRM
                Dec->cHRMExists = true;
                ParseCHRM(Dec, InputPNG, ChunkSize);
            } else if (*ChunkID == gAMAMarker) { // gAMA
                Dec->gAMAExists = true;
                ParseGAMA(Dec, InputPNG, ChunkSize);
            } else if (*ChunkID == oFFsMarker) { // oFFs
                Dec->oFFsExists = true;
                ParseOFFS(Dec, InputPNG, ChunkSize);
            } else if (*ChunkID == pHYsMarker) { // pHYs
                Dec->pHYsExists = true;
                ParsePHYS(Dec, InputPNG, ChunkSize);
            } else if (*ChunkID == sBITMarker) { // sBIT
                Dec->sBITExists = true;
                ParseSBIT(Dec, InputPNG, ChunkSize);
            } else if (*ChunkID == sCALMarker) { // sCAL
                Dec->sCALExists = true;
                ParseSCAL(Dec, InputPNG, ChunkSize);
            } else if (*ChunkID == sRGBMarker) { // sRGB
                Dec->sRGBExists = true;
                ParseSRGB(Dec, InputPNG, ChunkSize);
            } else if (*ChunkID == sTERMarker) { // sTER
                Dec->sTERExists = true;
                ParseSTER(Dec, InputPNG, ChunkSize);
            } else if (*ChunkID == tEXtMarker) { // tEXt
                Dec->TextExists = true;
                ParseTEXt(Dec, InputPNG, ChunkSize);
            } else if (*ChunkID == zTXtMarker) { // zTXt
                Dec->TextExists = true;
                ParseZTXt(Dec, InputPNG, ChunkSize);
            } else if (*ChunkID == iTXtMarker) { // iTXt
                Dec->TextExists = true;
                ParseITXt(Dec, InputPNG, ChunkSize);
            } else if (*ChunkID == tIMEMarker) { // tIME
                Dec->tIMEExists = true;
                ParseTIME(Dec, InputPNG, ChunkSize);
            } else if (*ChunkID == tRNSMarker) { // tRNS
                Dec->tRNSExists = true;
                ParseTRNS(Dec, InputPNG, ChunkSize);
            } else if (*ChunkID == hISTMarker && Dec->PLTEExists) { // hIST
                Dec->hISTExists = true;
                ParseHIST(Dec, InputPNG, ChunkSize);
            } else if (*ChunkID == iCCPMarker) { // iCCP
                Dec->iCCPExists = true;
                ParseICCP(Dec, InputPNG, ChunkSize);
            } else if (*ChunkID == pCALMarker) { // pCAL
                Dec->pCALExists = true;
                ParsePCAL(Dec, InputPNG, ChunkSize);
            } else if (*ChunkID == sPLTMarker) { // sPLT
                Dec->sPLTExists = true;
                ParseSPLT(Dec, InputPNG, ChunkSize);
            } else if (*ChunkID == acTLMarker) { // acTL
                Dec->acTLExists = true;
                ParseACTL(Dec, InputPNG, ChunkSize);
            } else if (*ChunkID == fcTLMarker) { // fcTL
                Dec->fcTLExists = true;
                ParseFCTL(Dec, InputPNG, ChunkSize);
            }
            free(ChunkID);
        } else {
            Log(Log_ERROR, __func__, U8("File Magic 0x%X is not PNG, exiting"), FileMagic);
            exit(EXIT_FAILURE);
        }
        return EXIT_SUCCESS;
    }
    
#ifdef __cplusplus
}
#endif
