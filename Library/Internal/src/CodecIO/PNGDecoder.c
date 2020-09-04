#include "../../include/Private/CodecIO/PNGCodec.h"

#include "../../../Dependencies/FoundationIO/Library/include/MathIO.h"
#include "../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../Dependencies/FoundationIO/Library/include/TextIO/LogIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    void ParseIHDR(PNGDecoder *Dec, BitBuffer *BitB, uint32_t ChunkSize) {
        uint32_t GeneratedCRC     = GenerateCRC32(BitB, ChunkSize);
        Dec->iHDR->Width          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        Dec->iHDR->Height         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        Dec->iHDR->BitDepth       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        Dec->iHDR->ColorType      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        if (Dec->iHDR->ColorType == 1 || Dec->iHDR->ColorType == 5 || Dec->iHDR->ColorType >= 7) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Invalid color type: %d"), Dec->iHDR->ColorType);
        }
        Dec->iHDR->Compression    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        Dec->iHDR->FilterMethod   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        Dec->iHDR->IsInterlaced   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);

        uint32_t CRC              = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        if (GeneratedCRC != CRC) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CRC Mismatch"));
        }

        //VerifyCRC(Dec->iHDR, ChunkSize, 1, 1, CRC);
    }

    void ParsePLTE(PNGDecoder *Dec, BitBuffer *BitB, uint32_t ChunkSize) { // Palette
        if (Dec->iHDR->BitDepth > 8) { // INVALID
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Invalid bit depth %d and palette combination"), Dec->iHDR->BitDepth);
            BitBuffer_Seek(BitB, Bytes2Bits(ChunkSize));
        } else {

            if (Dec->iHDR->ColorType == PNGColor_Palette || Dec->iHDR->ColorType == PNGColor_RGB) {
                Dec->PLTE->Palette = calloc(1, (3 * Dec->PLTE->NumEntries) * Bits2Bytes(Dec->iHDR->BitDepth, RoundingType_Down));
            } else if (Dec->iHDR->ColorType == PNGColor_RGBAlpha) {
                Dec->PLTE->Palette = calloc(1, (4 * Dec->PLTE->NumEntries) * Bits2Bytes(Dec->iHDR->BitDepth, RoundingType_Down));
            }


            for (uint8_t Channel = 0; Channel < PNGNumChannelsFromColorType[Dec->iHDR->ColorType]; Channel++) {
                for (uint16_t PaletteEntry = 0; PaletteEntry < ChunkSize / 3; PaletteEntry++) {
                    Dec->PLTE->Palette[Channel][PaletteEntry] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, Dec->iHDR->BitDepth);
                }
            }
        }
    }

    void ParseTRNS(PNGDecoder *Dec, BitBuffer *BitB, uint32_t ChunkSize) { // Transparency
        Dec->tRNS->NumEntries = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        uint16_t **Entries    = NULL;
        if (Dec->iHDR->ColorType == PNGColor_RGB) {
            Entries = calloc(3, Bits2Bytes(Dec->iHDR->BitDepth, RoundingType_Down) * sizeof(uint16_t));
        } else if (Dec->iHDR->ColorType == PNGColor_RGBAlpha) {
            Entries = calloc(4, Bits2Bytes(Dec->iHDR->BitDepth, RoundingType_Down) * sizeof(uint16_t));
        } else if (Dec->iHDR->ColorType == PNGColor_Gray) {
            Entries = calloc(1, Bits2Bytes(Dec->iHDR->BitDepth, RoundingType_Down) * sizeof(uint16_t));
        } else if (Dec->iHDR->ColorType == PNGColor_GrayAlpha) {
            Entries = calloc(2, Bits2Bytes(Dec->iHDR->BitDepth, RoundingType_Down) * sizeof(uint16_t));
        }
        if (Entries == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Failed to allocate enough memory for the TRNS Palette"));
        } else {
            for (uint8_t Color = 0; Color < PNGNumChannelsFromColorType[Dec->iHDR->ColorType]; Color++) {
                Entries[Color]    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, Bits2Bytes(Dec->iHDR->BitDepth, RoundingType_Down));
            }
            //Dec->tRNS->Palette = Entries;
        }
    }

    void ParseBKGD(PNGDecoder *Dec, BitBuffer *BitB, uint32_t ChunkSize) { // Background
        for (uint8_t Entry = 0; Entry < 3; Entry++) {
            Dec->bkGD->BackgroundPaletteEntry[Entry] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        }
    }

    void ParseCHRM(PNGDecoder *Dec, BitBuffer *BitB, uint32_t ChunkSize) { // Chromaticities
        Dec->cHRM->WhitePointX = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        Dec->cHRM->WhitePointY = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        Dec->cHRM->RedX        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        Dec->cHRM->RedY        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        Dec->cHRM->GreenX      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        Dec->cHRM->GreenY      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        Dec->cHRM->BlueX       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        Dec->cHRM->BlueY       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
    }

    void ParseGAMA(PNGDecoder *Dec, BitBuffer *BitB, uint32_t ChunkSize) { // Gamma
        Dec->gAMA->Gamma = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
    }

    void ParseOFFS(PNGDecoder *Dec, BitBuffer *BitB, uint32_t ChunkSize) { // Image Offset
        Dec->oFFs->XOffset       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        Dec->oFFs->YOffset       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        Dec->oFFs->UnitSpecifier = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
    }

    void ParsePHYS(PNGDecoder *Dec, BitBuffer *BitB, uint32_t ChunkSize) { // Aspect ratio, Physical pixel size
        Dec->pHYs->PixelsPerUnitXAxis = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        Dec->pHYs->PixelsPerUnitYAxis = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        Dec->pHYs->UnitSpecifier      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
    }

    void ParseSCAL(PNGDecoder *Dec, BitBuffer *BitB, uint32_t ChunkSize) { // Physical Scale
                                                                              // Ok, so we need to take the size of the chunk into account.
        Dec->sCAL->UnitSpecifier = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8); // 1 = Meter, 2 = Radian

        uint32_t WidthStringSize  = 0;
        uint32_t HeightStringSize = 0;

        for (uint32_t Byte = 0UL; Byte < ChunkSize - 1; Byte++) { // Get the sizes for the field strings
            if (BitBuffer_PeekBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8) != '\0') {
                WidthStringSize += 1;
            }
        }
        HeightStringSize = ChunkSize - (WidthStringSize + 1);

        UTF8 *WidthString = calloc(WidthStringSize, sizeof(UTF8));
        UTF8 *HeightString = calloc(HeightStringSize, sizeof(UTF8));

        for (uint32_t WidthCodePoint = 0; WidthCodePoint < WidthStringSize; WidthCodePoint++) {
            WidthString[WidthCodePoint] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        }
        BitBuffer_Seek(BitB, 8); // Skip the NULL seperator
        for (uint32_t HeightCodePoint = 0; HeightCodePoint < HeightStringSize; HeightCodePoint++) {
            HeightString[HeightCodePoint] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        }

        Dec->sCAL->PixelWidth  = UTF8_String2Decimal(WidthString);
        Dec->sCAL->PixelHeight = UTF8_String2Decimal(HeightString);
    }

    void ParsePCAL(PNGDecoder *Dec, BitBuffer *BitB, uint32_t ChunkSize) {
        char CalibrationName[80];
        while (BitBuffer_PeekBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8) != 0x0) {
            for (uint8_t Byte = 0; Byte < 80; Byte++) {
                CalibrationName[Byte] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
            }
        }
        Dec->pCAL->CalibrationName     = CalibrationName;
        Dec->pCAL->CalibrationNameSize = UTF8_GetStringSizeInCodePoints(Dec->pCAL->CalibrationName);
    }

    void ParseSBIT(PNGDecoder *Dec, BitBuffer *BitB, uint32_t ChunkSize) { // Significant bits per sample
        Dec->sBIT->Red                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        Dec->sBIT->Green                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        Dec->sBIT->Blue                  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
    }

    void ParseSRGB(PNGDecoder *Dec, BitBuffer *BitB, uint32_t ChunkSize) {
        Dec->sRGB->RenderingIntent       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
    }

    void ParseSTER(PNGDecoder *Dec, BitBuffer *BitB, uint32_t ChunkSize) {
        Dec->PNGIs3D                     = true;
        Dec->sTER->StereoType            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);

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

    void ParseTEXt(PNGDecoder *Dec, BitBuffer *BitB, uint32_t ChunkSize) { // Uncompressed, ASCII tEXt
                                                                              // Read until you hit a NULL for the name string, then subtract the size of the previous string from the total length to get the number of bytes for the second string
        uint8_t  KeywordSize = 0UL;
        uint8_t  CurrentByte = 0; // 1 is BULLshit

        do {
            CurrentByte  = BitBuffer_PeekBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
            KeywordSize += 1;
        } while (CurrentByte != 0);

        uint32_t CommentSize = ChunkSize - KeywordSize;

        Dec->NumTextChunks  += 1;



        // first we need to get the size of the strings

        // for that we would theoretically use BitBuffer_PeekBits, but BitBuffer_PeekBits is limited to 64 bits read at once, max.
        // there can be up to 800 bits before we hit the NULL, so what do...

        // Well, we can use SeekBits to jump byte by byte but it's not optimal...



        // Ok, once we've gotten the size of the Keyword string


        // Then read the Comment string.

        // Now, how do we handle multiple text strings?
        // Why not just have a count of the text chunks?
        // Store a variable in PNGDecoder called NumTextChunks, then store a type called Comment or something that stores both the comment type as a string and the actual comment...




    }

    void ParseZTXt(PNGDecoder *Dec, BitBuffer *BitB, uint32_t ChunkSize) { // Compressed text
    }

    void ParseITXt(PNGDecoder *Dec, BitBuffer *BitB, uint32_t ChunkSize) { // International Text
    }

    void ParseTIME(PNGDecoder *Dec, BitBuffer *BitB, uint32_t ChunkSize) {
        Dec->tIMe->Year                  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16);
        Dec->tIMe->Month                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        Dec->tIMe->Day                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        Dec->tIMe->Hour                  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        Dec->tIMe->Minute                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        Dec->tIMe->Second                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
    }

    /* APNG */
    void ParseACTL(PNGDecoder *Dec, BitBuffer *BitB, uint32_t ChunkSize) { // Animation control, part of APNG
        Dec->PNGIsAnimated               = true;
        Dec->acTL->NumFrames             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        Dec->acTL->TimesToLoop           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32); // If 0, loop forever.
    }

    void ParseFCTL(PNGDecoder *Dec, BitBuffer *BitB, uint32_t ChunkSize) { // Frame Control, part of APNG
        Dec->fcTL->FrameNum              = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        Dec->fcTL->Width                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        Dec->fcTL->Height                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        Dec->fcTL->XOffset               = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        Dec->fcTL->YOffset               = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        Dec->fcTL->FrameDelayNumerator   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16);
        Dec->fcTL->FrameDelayDenominator = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16);
        Dec->fcTL->DisposeMethod         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        Dec->fcTL->BlendMethod           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
    }
    /* End APNG */

    void ParseHIST(PNGDecoder *Dec, BitBuffer *BitB, uint32_t ChunkSize) {
    }

    void ParseICCP(PNGDecoder *Dec, BitBuffer *BitB, uint32_t ChunkSize) {
        uint8_t ProfileNameSize = 0;
        ProfileNameSize = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
    }

    void ParseSPLT(PNGDecoder *Dec, BitBuffer *BitB, uint32_t ChunkSize) {
    }

    uint8_t ParsePNGMetadata(BitBuffer *BitB, PNGDecoder *Dec) {
        uint64_t FileMagic    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 64);

        if (FileMagic == PNGMagic) {
            char     *ChunkID        = calloc(4, sizeof(uint8_t));
            uint32_t ChunkSize       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);

            for (uint8_t ChunkIDSize = 0; ChunkIDSize < 4; ChunkIDSize++) {
                ChunkID[ChunkIDSize] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
            }
            BitBuffer_Seek(BitB, -32); // Now we need to skip back so we can read the ChunkID as part of the CRC check.
                                           // Now we call the VerifyCRC32 function with the ChunkSize
            VerifyCRC32(BitB, ChunkSize);

            if (*ChunkID == PNGMarker_iHDR) {        // iHDR
                ParseIHDR(Dec, BitB, ChunkSize);
            } else if (*ChunkID == PNGMarker_PLTE) { // PLTE
                Dec->PLTEExists = true;
                ParsePLTE(Dec, BitB, ChunkSize);
            } else if (*ChunkID == PNGMarker_bKGD) { // bKGD
                Dec->bkGDExists = true;
                ParseBKGD(Dec, BitB, ChunkSize);
            } else if (*ChunkID == PNGMarker_cHRM) { // cHRM
                Dec->cHRMExists = true;
                ParseCHRM(Dec, BitB, ChunkSize);
            } else if (*ChunkID == PNGMarker_gAMA) { // gAMA
                Dec->gAMAExists = true;
                ParseGAMA(Dec, BitB, ChunkSize);
            } else if (*ChunkID == PNGMarker_oFFs) { // oFFs
                Dec->oFFsExists = true;
                ParseOFFS(Dec, BitB, ChunkSize);
            } else if (*ChunkID == PNGMarker_pHYs) { // pHYs
                Dec->pHYsExists = true;
                ParsePHYS(Dec, BitB, ChunkSize);
            } else if (*ChunkID == PNGMarker_sBIT) { // sBIT
                Dec->sBITExists = true;
                ParseSBIT(Dec, BitB, ChunkSize);
            } else if (*ChunkID == PNGMarker_sCAL) { // sCAL
                Dec->sCALExists = true;
                ParseSCAL(Dec, BitB, ChunkSize);
            } else if (*ChunkID == PNGMarker_sRGB) { // sRGB
                Dec->sRGBExists = true;
                ParseSRGB(Dec, BitB, ChunkSize);
            } else if (*ChunkID == PNGMarker_sTER) { // sTER
                Dec->sTERExists = true;
                ParseSTER(Dec, BitB, ChunkSize);
            } else if (*ChunkID == PNGMarker_tEXt) { // tEXt
                Dec->TextExists = true;
                ParseTEXt(Dec, BitB, ChunkSize);
            } else if (*ChunkID == PNGMarker_zTXt) { // zTXt
                Dec->TextExists = true;
                ParseZTXt(Dec, BitB, ChunkSize);
            } else if (*ChunkID == PNGMarker_iTXt) { // iTXt
                Dec->TextExists = true;
                ParseITXt(Dec, BitB, ChunkSize);
            } else if (*ChunkID == PNGMarker_tIME) { // tIME
                Dec->tIMEExists = true;
                ParseTIME(Dec, BitB, ChunkSize);
            } else if (*ChunkID == PNGMarker_tRNS) { // tRNS
                Dec->tRNSExists = true;
                ParseTRNS(Dec, BitB, ChunkSize);
            } else if (*ChunkID == PNGMarker_hIST && Dec->PLTEExists) { // hIST
                Dec->hISTExists = true;
                ParseHIST(Dec, BitB, ChunkSize);
            } else if (*ChunkID == PNGMarker_iCCP) { // iCCP
                Dec->iCCPExists = true;
                ParseICCP(Dec, BitB, ChunkSize);
            } else if (*ChunkID == PNGMarker_pCAL) { // pCAL
                Dec->pCALExists = true;
                ParsePCAL(Dec, BitB, ChunkSize);
            } else if (*ChunkID == PNGMarker_sPLT) { // sPLT
                Dec->sPLTExists = true;
                ParseSPLT(Dec, BitB, ChunkSize);
            } else if (*ChunkID == PNGMarker_acTL) { // acTL
                Dec->acTLExists = true;
                ParseACTL(Dec, BitB, ChunkSize);
            } else if (*ChunkID == PNGMarker_fcTL) { // fcTL
                Dec->fcTLExists = true;
                ParseFCTL(Dec, BitB, ChunkSize);
            }
            free(ChunkID);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("File Magic 0x%X is not PNG, exiting"), FileMagic);
            exit(EXIT_FAILURE);
        }
        return EXIT_SUCCESS;
    }

    bool VerifyChunkCRC(BitBuffer *BitB, uint32_t ChunkSize) {
        // So basically we need to read ChunkSize bytes into an array, then read the following 4 bytes as the CRC
        // then run VerifyCRC over the buffer, and finally compare the generated CRC with the extracted one, and return whether they match.
        // Then call BitBuffer_SeekBits(BitB, Bytes2Bits(ChunkSize)); to reset to the beginning of the chunk
        uint8_t *Buffer2CRC = calloc(1, ChunkSize * sizeof(uint8_t));
        uint8_t *Buffer     = BitBuffer_GetArray(BitB);
        uint64_t Offset     = BitBuffer_GetPosition(BitB);
        for (uint32_t Byte = 0; Byte < ChunkSize; Byte++) {
            Buffer2CRC[Byte] = Buffer[Bits2Bytes(Offset, RoundingType_Down)];
            free(Buffer2CRC);

        }
        uint32_t ChunkCRC = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        uint32_t ContentCRC = GenerateCRC32(BitB, ChunkSize);
        bool CRCsMatch = No;
        if (ContentCRC == ChunkCRC) {
            CRCsMatch  = Yes;
        }
        return CRCsMatch;
    }

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

    void PNGDecodeNonFilter(PNGDecoder *Dec, uint8_t ***InflatedData, uint8_t ***DeFilteredData, size_t Line) {
        for (uint8_t StereoView = 0; StereoView < Dec->sTER->StereoType; StereoView++) {
            for (uint32_t Width = 0; Width < Dec->iHDR->Width; Width++) {
                for (size_t Byte = 1; Byte < Bits2Bytes(Dec->iHDR->BitDepth, RoundingType_Down); Byte++) {
                    DeFilteredData[StereoView][Width][Byte - 1] = InflatedData[StereoView][Width][Byte]; // Remove filter indicating byte
                }
            }
        }
    }

    void PNGDecodeSubFilter(PNGDecoder *Dec, uint8_t ***InflatedData, uint8_t ***DeFilteredData, size_t Line) {
        for (uint8_t StereoView = 0; StereoView < Dec->sTER->StereoType; StereoView++) {
            for (uint32_t Width = 0; Width < Dec->iHDR->Width; Width++) {
                for (size_t Byte = 1; Byte < Bits2Bytes(Dec->iHDR->BitDepth, RoundingType_Down); Byte++) {
                    DeFilteredData[StereoView][Line][Byte - 1] = (InflatedData[StereoView][Line][Byte] + InflatedData[StereoView][Line][Byte+1]) & 0xFF; // Byte+1 is wrong, needs to be + bitdepth
                }
            }
        }
    }

    void PNGDecodeUpFilter(PNGDecoder *Dec, uint8_t ***InflatedData, uint8_t ***DeFilteredData, size_t Line) {
        for (uint8_t StereoView = 0; StereoView < Dec->sTER->StereoType; StereoView++) {
            for (uint32_t Width = 0; Width < Dec->iHDR->Width; Width++) {
                for (size_t Byte = 1; Byte < Bits2Bytes(Dec->iHDR->BitDepth, RoundingType_Down); Byte++) {
                    DeFilteredData[StereoView][Line][Byte - 1] = InflatedData[StereoView][Line][Byte] + InflatedData[StereoView][Line - 1][Byte] & 0xFF;
                }
            }
        }
    }

    void PNGDecodeAverageFilter(PNGDecoder *Dec, uint8_t ***InflatedData, uint8_t ***DeFilteredData, size_t Line) {
        uint8_t PixelSize = Bits2Bytes(Dec->iHDR->BitDepth, RoundingType_Down);
        for (uint8_t StereoView = 0; StereoView < Dec->sTER->StereoType; StereoView++) {
            for (uint32_t Width = 0; Width < Dec->iHDR->Width; Width++) {
                for (size_t Byte = 1; Byte < Bits2Bytes(Dec->iHDR->BitDepth, RoundingType_Down); Byte++) {
                    uint8_t Average = floor((InflatedData[StereoView][Line][Byte - (PixelSize)] + InflatedData[StereoView][Line - 1][Byte]) / 2);
                    DeFilteredData[StereoView][Line][Byte - 1] = InflatedData[StereoView][Line][Byte] + Average;
                }
            }
        }
    }

    void PNGDecodePaethFilter(PNGDecoder *Dec, uint8_t ***InflatedData, uint8_t ***DeFilteredData, size_t Line) {
        // Filtering is applied to bytes, not pixels
        uint8_t PixelSize = Bits2Bytes(Dec->iHDR->BitDepth, RoundingType_Down);
        for (uint8_t StereoView = 0; StereoView < Dec->sTER->StereoType; StereoView++) {
            for (uint32_t Width = 0; Width < Dec->iHDR->Width; Width++) {
                for (size_t Byte = 1; Byte < Bits2Bytes(Dec->iHDR->BitDepth, RoundingType_Down); Byte++) {
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

    PNGFilterTypes ExtractLineFilterType(uint8_t *Line) {
        uint8_t FilterType = Line[0];
        return FilterType;
    }

    void PNGDecodeFilteredImage(PNGDecoder *Dec, uint8_t ***InflatedBuffer) {

        uint8_t ***DeFilteredData = calloc((Dec->iHDR->Height * Dec->iHDR->Width), Bits2Bytes(Dec->iHDR->BitDepth, RoundingType_Down));

        for (uint8_t StereoView = 0; StereoView < PNGGetStereoscopicStatus(Dec); StereoView++) {
            for (size_t Line = 0; Line < Dec->iHDR->Height; Line++) {
                PNGFilterTypes FilterType = ExtractLineFilterType(Line);
                switch (FilterType) {
                    case PNGFilter_Unfiltered:
                        // copy the Line except byte 0 (the filter indication byte) to the output buffer.
                        PNGDecodeNonFilter(Dec, InflatedBuffer, DeFilteredData, Line);
                        break;
                    case PNGFilter_Sub:
                        // SubFilter
                        PNGDecodeSubFilter(Dec, InflatedBuffer, DeFilteredData, Line);
                        break;
                    case PNGFilter_Up:
                        // UpFilter
                        PNGDecodeUpFilter(Dec, InflatedBuffer, DeFilteredData, Line);
                        break;
                    case PNGFilter_Average:
                        // AverageFilter
                        PNGDecodeAverageFilter(Dec, InflatedBuffer, DeFilteredData, Line);
                        break;
                    case PNGFilter_Paeth:
                        // PaethFilter
                        PNGDecodePaethFilter(Dec, InflatedBuffer, DeFilteredData, Line);
                        break;
                    default:
                        Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Filter type: %d is invalid"), FilterType);
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

    void DecodeDEFLATEBlock(PNGDecoder *Dec, BitBuffer *DEFLATEBlock, BitBuffer *DecodedBlock) { // LSByteFirst
                                                                                                // Huffman codes are written MSBit first, everything else is writen LSBit first

        // DEFLATE Block header:
        bool     BlockIsFinal = BitBuffer_ReadBits(DEFLATEBlock, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1); // BFINAL
        uint8_t  BlockType    = BitBuffer_ReadBits(DEFLATEBlock, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 2); // BTYPE

        if (BlockType == DynamicHuffmanBlock) {

        } else if (BlockType == FixedHuffmanBlock) {

        } else if (BlockType == UncompressedBlock) {

        }
    }

    void DecodeIFDATChunk(PNGDecoder *Dec, BitBuffer *DAT2Decode, BitBuffer *DecodedDAT, uint64_t DATSize) { // Decodes both fDAT and IDAT chunks
                                                                                                            // well lets go ahead and allocate a DAT block the size of DATSize
        BitBuffer_Init(DATSize);
        // Now we need to go ahead and parse the ZLIB Header.
        // ok so how do we do that? I wrote some notes on the Zlib header last night...

        /* Compression Method and Flags byte */
        uint8_t CompressionMethod = BitBuffer_ReadBits(DAT2Decode, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 4); // 8
        uint8_t CompressionInfo   = BitBuffer_ReadBits(DAT2Decode, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 4); // 7
        /* Compression Method and Flags byte */

        /* FlagByte */
        uint8_t FCHECK            = BitBuffer_ReadBits(DAT2Decode, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 5); // 1E
        bool    FDICTPresent      = BitBuffer_ReadBits(DAT2Decode, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1); // 0
        uint8_t FLEVEL            = BitBuffer_ReadBits(DAT2Decode, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 2); // 1
        /* FlagByte */

        if (FDICTPresent) {
            // Read TableID which is 4 bytes
            uint32_t DICTID       = BitBuffer_ReadBits(DAT2Decode, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        }

        // Start reading the DEFLATE block?

    }

    void DecodePNGData(PNGDecoder *Dec, BitBuffer *BitB) {
        // read the iDAT/fDAT chunk header, then do the other stuff.
        while (BitBuffer_GetSize(BitB) > 0) { // 12 is the start of IEND
            uint32_t ChunkSize   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
            uint32_t ChunkID     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);


            if (ChunkID == PNGMarker_acTL) {
                ParseACTL(Dec, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_bKGD) {
                ParseBKGD(Dec, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_cHRM) {
                ParseCHRM(Dec, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_fcTL) {
                ParseFCTL(Dec, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_gAMA) {
                ParseGAMA(Dec, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_hIST) {
                ParseHIST(Dec, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_iCCP) {
                ParseOFFS(Dec, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_iDAT || ChunkID == PNGMarker_fDAT) {
                DecodeIFDATChunk(Dec, BitB, NULL, ChunkSize);
            } else if (ChunkID == PNGMarker_iHDR) {
                ParseIHDR(Dec, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_iTXt) {
                ParseITXt(Dec, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_oFFs) {
                ParseOFFS(Dec, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_pCAL) {
                ParsePCAL(Dec, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_pHYs) {
                ParsePHYS(Dec, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_PLTE) {
                ParsePLTE(Dec, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_sBIT) {
                ParseSBIT(Dec, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_sRGB) {
                ParseSRGB(Dec, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_sTER) {
                ParseSTER(Dec, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_tEXt) {
                ParseTEXt(Dec, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_zTXt) {
                ParseZTXt(Dec, BitB, ChunkSize);
            }
        }
    }

    // Let's do this library right, by adding attach and delete functions for the various chunks, and let's also have a fancy function that applies color profiles to the pixels.
    // that's kinda a lot of work tho...
    uint16_t ***DecodePNGImage(PNGDecoder *Dec, BitBuffer *BitB) {
        return NULL;
    }

    BitBuffer *DecodeAdam7(PNGDecoder *Dec, BitBuffer *InterlacedImage) { // Returns the interlaced image
        /*
         Ok, so to interlace an image, we need to loop over every pixel in an 8x8 block?
         */
        BitBuffer *ProgressiveImage = NULL;
        return ProgressiveImage;
    }

    ImageContainer *PNGDecodeImage2(PNGDecoder *Dec, BitBuffer *PNG2Decode, uint16_t GammaCorrect) {
        return NULL;
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
