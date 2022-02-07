#include "../../include/EntropyIO.h"
#include "../../include/CodecIO/PNGCodec.h"
#include "../../../../Dependencies/FoundationIO/Library/include/TextIO/StringIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/CryptographyIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    void PNG_Read_IHDR(PNGOptions *Dec, BitBuffer *BitB, uint32_t ChunkSize) {
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

        uint32_t ComputedCRC      = BitBuffer_CalculateCRC(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitBuffer_GetPosition(BitB) - 104, 13, 0xFFFFFFFF, CRCPolynomial_IEEE802_3);

        uint32_t CRC              = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        if (ComputedCRC != CRC) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CRC Mismatch"));
        }
    }

    void PNG_Read_PLTE(PNGOptions *Dec, BitBuffer *BitB, uint32_t ChunkSize) { // Palette
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

    void PNG_Read_TRNS(PNGOptions *Dec, BitBuffer *BitB, uint32_t ChunkSize) { // Transparency
        Dec->tRNS->NumEntries = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        uint16_t *Entries    = NULL;
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
        }
    }

    void PNG_Read_BKGD(PNGOptions *Dec, BitBuffer *BitB, uint32_t ChunkSize) { // Background
        for (uint8_t Entry = 0; Entry < 3; Entry++) {
            Dec->bkGD->BackgroundPaletteEntry[Entry] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        }
    }

    void PNG_Read_CHRM(PNGOptions *Dec, BitBuffer *BitB, uint32_t ChunkSize) { // Chromaticities
        Dec->cHRM->WhitePointX = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        Dec->cHRM->WhitePointY = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        Dec->cHRM->RedX        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        Dec->cHRM->RedY        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        Dec->cHRM->GreenX      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        Dec->cHRM->GreenY      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        Dec->cHRM->BlueX       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        Dec->cHRM->BlueY       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
    }

    void PNG_Read_GAMA(PNGOptions *Dec, BitBuffer *BitB, uint32_t ChunkSize) { // Gamma
        Dec->gAMA->Gamma = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
    }

    void PNG_Read_OFFS(PNGOptions *Dec, BitBuffer *BitB, uint32_t ChunkSize) { // Image Offset
        Dec->oFFs->XOffset       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        Dec->oFFs->YOffset       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        Dec->oFFs->UnitSpecifier = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
    }

    void PNG_Read_PHYS(PNGOptions *Dec, BitBuffer *BitB, uint32_t ChunkSize) { // Aspect ratio, Physical pixel size
        Dec->pHYs->PixelsPerUnitXAxis = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        Dec->pHYs->PixelsPerUnitYAxis = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        Dec->pHYs->UnitSpecifier      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
    }

    void PNG_Read_SCAL(PNGOptions *Dec, BitBuffer *BitB, uint32_t ChunkSize) { // Physical Scale
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

        Dec->sCAL->PixelWidth  = UTF8_String2Decimal(Base_Decimal | Base_Radix10, WidthString);
        Dec->sCAL->PixelHeight = UTF8_String2Decimal(Base_Decimal | Base_Radix10, HeightString);
    }

    void PNG_Read_PCAL(PNGOptions *Dec, BitBuffer *BitB, uint32_t ChunkSize) {
        UTF8 CalibrationName[80];
        while (BitBuffer_PeekBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8) != 0x0) {
            for (uint8_t Byte = 0; Byte < 80; Byte++) {
                CalibrationName[Byte] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
            }
        }
        Dec->pCAL->CalibrationName     = CalibrationName;
        Dec->pCAL->CalibrationNameSize = UTF8_GetStringSizeInCodePoints(Dec->pCAL->CalibrationName);
    }

    void PNG_Read_SBIT(PNGOptions *Dec, BitBuffer *BitB, uint32_t ChunkSize) { // Significant bits per sample
        Dec->sBIT->Red                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        Dec->sBIT->Green                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        Dec->sBIT->Blue                  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
    }

    void PNG_Read_SRGB(PNGOptions *Dec, BitBuffer *BitB, uint32_t ChunkSize) {
        Dec->sRGB->RenderingIntent       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
    }

    void PNG_Read_STER(PNGOptions *Dec, BitBuffer *BitB, uint32_t ChunkSize) {
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

    void PNG_Read_TEXT(PNGOptions *Dec, BitBuffer *BitB, uint32_t ChunkSize) { // Uncompressed, ASCII tEXt
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
        // Store a variable in PNGOptions called NumTextChunks, then store a type called Comment or something that stores both the comment type as a string and the actual comment...
    }

    void PNG_Read_ZTXT(PNGOptions *Dec, BitBuffer *BitB, uint32_t ChunkSize) { // Compressed text
    }

    void PNG_Read_ITXT(PNGOptions *Dec, BitBuffer *BitB, uint32_t ChunkSize) { // International Text
    }

    void PNG_Read_TIME(PNGOptions *Dec, BitBuffer *BitB, uint32_t ChunkSize) {
        Dec->tIMe->Year                  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16);
        Dec->tIMe->Month                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        Dec->tIMe->Day                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        Dec->tIMe->Hour                  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        Dec->tIMe->Minute                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        Dec->tIMe->Second                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
    }

    /* APNG */
    void PNG_Read_ACTL(PNGOptions *Dec, BitBuffer *BitB, uint32_t ChunkSize) { // Animation control, part of APNG
        Dec->PNGIsAnimated               = true;
        Dec->acTL->NumFrames             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        Dec->acTL->TimesToLoop           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32); // If 0, loop forever.
    }

    void PNG_Read_FCTL(PNGOptions *Dec, BitBuffer *BitB, uint32_t ChunkSize) { // Frame Control, part of APNG
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

    void PNG_Read_HIST(PNGOptions *Dec, BitBuffer *BitB, uint32_t ChunkSize) {
    }

    void PNG_Read_ICCP(PNGOptions *Dec, BitBuffer *BitB, uint32_t ChunkSize) {
        uint8_t ProfileNameSize = 0;
        ProfileNameSize = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
    }

    void PNG_Read_SPLT(PNGOptions *Dec, BitBuffer *BitB, uint32_t ChunkSize) {
    }

    uint8_t ParsePNGMetadata(BitBuffer *BitB, PNGOptions *Dec) {
        uint64_t FileMagic    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 64);

        if (FileMagic == PNGMagic) {
            char     ChunkID[4];
            uint32_t ChunkID2 = 0;
            for (uint8_t i = 0; i < 4; i++) {
                ChunkID2 <<= 8;
                ChunkID2   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
            }

            uint32_t ChunkSize       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);

            for (uint8_t ChunkIDSize = 0; ChunkIDSize < 4; ChunkIDSize++) {
                ChunkID[ChunkIDSize] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
            }
            BitBuffer_Seek(BitB, -32); // Now we need to skip back so we can read the ChunkID as part of the CRC check.

            if (ChunkID2 == PNGMarker_iHDR) {        // iHDR
                PNG_Read_IHDR(Dec, BitB, ChunkSize);
            } else if (ChunkID2 == PNGMarker_PLTE) { // PLTE
                Dec->PLTEExists = true;
                PNG_Read_PLTE(Dec, BitB, ChunkSize);
            } else if (ChunkID2 == PNGMarker_bKGD) { // bKGD
                Dec->bkGDExists = true;
                PNG_Read_BKGD(Dec, BitB, ChunkSize);
            } else if (ChunkID2 == PNGMarker_cHRM) { // cHRM
                Dec->cHRMExists = true;
                PNG_Read_CHRM(Dec, BitB, ChunkSize);
            } else if (ChunkID2 == PNGMarker_gAMA) { // gAMA
                Dec->gAMAExists = true;
                PNG_Read_GAMA(Dec, BitB, ChunkSize);
            } else if (ChunkID2 == PNGMarker_oFFs) { // oFFs
                Dec->oFFsExists = true;
                PNG_Read_OFFS(Dec, BitB, ChunkSize);
            } else if (ChunkID2 == PNGMarker_pHYs) { // pHYs
                Dec->pHYsExists = true;
                PNG_Read_PHYS(Dec, BitB, ChunkSize);
            } else if (ChunkID2 == PNGMarker_sBIT) { // sBIT
                Dec->sBITExists = true;
                PNG_Read_SBIT(Dec, BitB, ChunkSize);
            } else if (ChunkID2 == PNGMarker_sCAL) { // sCAL
                Dec->sCALExists = true;
                PNG_Read_SCAL(Dec, BitB, ChunkSize);
            } else if (ChunkID2 == PNGMarker_sRGB) { // sRGB
                Dec->sRGBExists = true;
                PNG_Read_SRGB(Dec, BitB, ChunkSize);
            } else if (ChunkID2 == PNGMarker_sTER) { // sTER
                Dec->sTERExists = true;
                PNG_Read_STER(Dec, BitB, ChunkSize);
            } else if (ChunkID2 == PNGMarker_tEXt) { // tEXt
                Dec->TextExists = true;
                PNG_Read_TEXT(Dec, BitB, ChunkSize);
            } else if (ChunkID2 == PNGMarker_zTXt) { // zTXt
                Dec->TextExists = true;
                PNG_Read_ZTXT(Dec, BitB, ChunkSize);
            } else if (ChunkID2 == PNGMarker_iTXt) { // iTXt
                Dec->TextExists = true;
                PNG_Read_ITXT(Dec, BitB, ChunkSize);
            } else if (ChunkID2 == PNGMarker_tIME) { // tIME
                Dec->tIMEExists = true;
                PNG_Read_TIME(Dec, BitB, ChunkSize);
            } else if (ChunkID2 == PNGMarker_tRNS) { // tRNS
                Dec->tRNSExists = true;
                PNG_Read_TRNS(Dec, BitB, ChunkSize);
            } else if (ChunkID2 == PNGMarker_hIST && Dec->PLTEExists) { // hIST
                Dec->hISTExists = true;
                PNG_Read_HIST(Dec, BitB, ChunkSize);
            } else if (ChunkID2 == PNGMarker_iCCP) { // iCCP
                Dec->iCCPExists = true;
                PNG_Read_ICCP(Dec, BitB, ChunkSize);
            } else if (ChunkID2 == PNGMarker_pCAL) { // pCAL
                Dec->pCALExists = true;
                PNG_Read_PCAL(Dec, BitB, ChunkSize);
            } else if (ChunkID2 == PNGMarker_sPLT) { // sPLT
                Dec->sPLTExists = true;
                PNG_Read_SPLT(Dec, BitB, ChunkSize);
            } else if (ChunkID2 == PNGMarker_acTL) { // acTL
                Dec->acTLExists = true;
                PNG_Read_ACTL(Dec, BitB, ChunkSize);
            } else if (ChunkID2 == PNGMarker_fcTL) { // fcTL
                Dec->fcTLExists = true;
                PNG_Read_FCTL(Dec, BitB, ChunkSize);
            }
            free(ChunkID);
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("File Magic 0x%llX is not PNG, exiting"), FileMagic);
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
        uint32_t ContentCRC = BitBuffer_CalculateCRC(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitBuffer_GetPosition(BitB) - 104, ChunkSize, 0xFFFFFFFF, CRCPolynomial_IEEE802_3);
        bool CRCsMatch = No;
        if (ContentCRC == ChunkCRC) {
            CRCsMatch  = Yes;
        }
        return CRCsMatch;
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

    void PNGDecodeNonFilter(PNGOptions *Dec, uint8_t ***InflatedData, uint8_t ***DeFilteredData, size_t Line) {
        for (uint8_t StereoView = 0; StereoView < Dec->sTER->StereoType; StereoView++) {
            for (uint32_t Width = 0; Width < Dec->iHDR->Width; Width++) {
                for (size_t Byte = 1; Byte < Bits2Bytes(Dec->iHDR->BitDepth, RoundingType_Down); Byte++) {
                    DeFilteredData[StereoView][Width][Byte - 1] = InflatedData[StereoView][Width][Byte]; // Remove filter indicating byte
                }
            }
        }
    }

    void PNGDecodeSubFilter(PNGOptions *Dec, uint8_t ***InflatedData, uint8_t ***DeFilteredData, size_t Line) {
        for (uint8_t StereoView = 0; StereoView < Dec->sTER->StereoType; StereoView++) {
            for (uint32_t Width = 0; Width < Dec->iHDR->Width; Width++) {
                for (size_t Byte = 1; Byte < Bits2Bytes(Dec->iHDR->BitDepth, RoundingType_Down); Byte++) {
                    DeFilteredData[StereoView][Line][Byte - 1] = (InflatedData[StereoView][Line][Byte] + InflatedData[StereoView][Line][Byte+1]) & 0xFF; // Byte+1 is wrong, needs to be + bitdepth
                }
            }
        }
    }

    void PNGDecodeUpFilter(PNGOptions *Dec, uint8_t ***InflatedData, uint8_t ***DeFilteredData, size_t Line) {
        for (uint8_t StereoView = 0; StereoView < Dec->sTER->StereoType; StereoView++) {
            for (uint32_t Width = 0; Width < Dec->iHDR->Width; Width++) {
                for (size_t Byte = 1; Byte < Bits2Bytes(Dec->iHDR->BitDepth, RoundingType_Down); Byte++) {
                    DeFilteredData[StereoView][Line][Byte - 1] = InflatedData[StereoView][Line][Byte] + InflatedData[StereoView][Line - 1][Byte] & 0xFF;
                }
            }
        }
    }

    void PNGDecodeAverageFilter(PNGOptions *Dec, uint8_t ***InflatedData, uint8_t ***DeFilteredData, size_t Line) {
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

    void PNGDecodePaethFilter(PNGOptions *Dec, uint8_t ***InflatedData, uint8_t ***DeFilteredData, size_t Line) {
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

    void PNGDecodeFilteredImage(PNGOptions *Dec, uint8_t ***InflatedBuffer) {

        uint8_t ***DeFilteredData = calloc((Dec->iHDR->Height * Dec->iHDR->Width), Bits2Bytes(Dec->iHDR->BitDepth, RoundingType_Down));

        for (uint8_t StereoView = 0; StereoView < PNGGetStereoscopicStatus(Dec); StereoView++) {
            for (size_t Line = 0; Line < Dec->iHDR->Height; Line++) {
                PNGFilterTypes FilterType = ExtractLineFilterType(*DeFilteredData[Line]);
                switch (FilterType) {
                    case PNGFilter_Unfiltered:
                        PNGDecodeNonFilter(Dec, InflatedBuffer, DeFilteredData, Line);
                        break;
                    case PNGFilter_Sub:
                        PNGDecodeSubFilter(Dec, InflatedBuffer, DeFilteredData, Line);
                        break;
                    case PNGFilter_Up:
                        PNGDecodeUpFilter(Dec, InflatedBuffer, DeFilteredData, Line);
                        break;
                    case PNGFilter_Average:
                        PNGDecodeAverageFilter(Dec, InflatedBuffer, DeFilteredData, Line);
                        break;
                    case PNGFilter_Paeth:
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

    enum DEFLATEFLAC_BlockTypes {
        UncompressedBlock   = 0,
        FixedHuffmanBlock   = 1,
        DynamicHuffmanBlock = 2,

    };

    void DecodeDEFLATEBlock(PNGOptions *Dec, BitBuffer *DEFLATEBlock, BitBuffer *DecodedBlock) { // ByteOrder_LSByteIsNearest
        // Huffman codes are written MSBit first, everything else is writen LSBit first

        // DEFLATE Block header:
        bool     BlockIsFinal = BitBuffer_ReadBits(DEFLATEBlock, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1); // BFINAL
        uint8_t  BlockType    = BitBuffer_ReadBits(DEFLATEBlock, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 2); // BTYPE

        if (BlockType == DynamicHuffmanBlock) {

        } else if (BlockType == FixedHuffmanBlock) {

        } else if (BlockType == UncompressedBlock) {

        }
    }

    void DecodeIFDATChunk(PNGOptions *Dec, BitBuffer *DAT2Decode, BitBuffer *DecodedDAT, uint64_t DATSize) { // Decodes both fDAT and IDAT chunks
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

    void DecodePNGData(PNGOptions *Dec, BitBuffer *BitB) {
        // read the iDAT/fDAT chunk header, then do the other stuff.
        while (BitBuffer_GetSize(BitB) > 0) { // 12 is the start of IEND
            uint32_t ChunkSize   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
            uint32_t ChunkID     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);


            if (ChunkID == PNGMarker_acTL) {
                PNG_Read_ACTL(Dec, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_bKGD) {
                PNG_Read_BKGD(Dec, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_cHRM) {
                PNG_Read_CHRM(Dec, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_fcTL) {
                PNG_Read_FCTL(Dec, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_gAMA) {
                PNG_Read_GAMA(Dec, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_hIST) {
                PNG_Read_HIST(Dec, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_iCCP) {
                PNG_Read_OFFS(Dec, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_iDAT || ChunkID == PNGMarker_fDAT) {
                DecodeIFDATChunk(Dec, BitB, NULL, ChunkSize);
            } else if (ChunkID == PNGMarker_iHDR) {
                PNG_Read_IHDR(Dec, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_iTXt) {
                PNG_Read_ITXT(Dec, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_oFFs) {
                PNG_Read_OFFS(Dec, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_pCAL) {
                PNG_Read_PCAL(Dec, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_pHYs) {
                PNG_Read_PHYS(Dec, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_PLTE) {
                PNG_Read_PLTE(Dec, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_sBIT) {
                PNG_Read_SBIT(Dec, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_sRGB) {
                PNG_Read_SRGB(Dec, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_sTER) {
                PNG_Read_STER(Dec, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_tEXt) {
                PNG_Read_TEXT(Dec, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_zTXt) {
                PNG_Read_ZTXT(Dec, BitB, ChunkSize);
            }
        }
    }

    // Let's do this library right, by adding attach and delete functions for the various chunks, and let's also have a fancy function that applies color profiles to the pixels.
    // that's kinda a lot of work tho...
    uint16_t ***DecodePNGImage(PNGOptions *Dec, BitBuffer *BitB) {
        return NULL;
    }

    BitBuffer *DecodeAdam7(PNGOptions *Dec, BitBuffer *InterlacedImage) { // Returns the interlaced image
        /*
         Ok, so to interlace an image, we need to loop over every pixel in an 8x8 block?
         */
        BitBuffer *ProgressiveImage = NULL;
        return ProgressiveImage;
    }

    /* FIXME: Old code below */
    /*
    void PNG_DAT_Decode(void *Options, BitBuffer *BitB, ImageContainer *Image) {
        if (BitB != NULL && Image != NULL) {
            PNGOptions *PNG                        = Options;
            uint8_t     ****ImageArrayBytes        = (uint8_t****) ImageContainer_GetArray(Image);
            HuffmanTree     *Tree                  = NULL;
            bool     IsFinalBlock                  = false;
            do {
                IsFinalBlock                       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1); // 0
                uint8_t BlockType                  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 2); // 0b00 = 0
                if (BlockType == BlockType_Literal) {
                    BitBuffer_Align(BitB, 1); // Skip the remaining 5 bits
                    uint16_t Bytes2Copy    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16); // 0x4F42 = 20,290
                    uint16_t Bytes2CopyXOR = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16) ^ 0xFFFF; // 0xB0BD = 0x4F42

                    if (Bytes2Copy == Bytes2CopyXOR) {
                        for (uint16_t Byte = 0ULL; Byte < Bytes2Copy; Byte++) {
                            ImageArrayBytes[0][0][0][Byte] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 8);
                        }
                    } else {
                        Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Data Error: Bytes2Copy does not match Bytes2CopyXOR in literal block"));
                    }
                } else if (BlockType == BlockType_Fixed) {
                    //Tree = Flate_BuildHuffmanTree();
                } else if (BlockType == BlockType_Dynamic) {
                    //Tree = Flate_BuildHuffmanTree();
                } else {
                    Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("PNG Invalid DAT Block"));
                }
                //PNG_Flate_Decode(PNG, BitB); // Actually read the data
            } while (IsFinalBlock == false);
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (Image == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        }
    }

    uint64_t ReadSymbol(BitBuffer *BitB, HuffmanTree *Tree) { // EQUILIVENT OF DECODE IN PUFF
        uint64_t Symbol              = 0ULL;
        uint16_t Count               = 0;
        if (BitB != NULL && Tree != NULL) {
            uint32_t FirstSymbolOfLength = 0;
            for (uint8_t Bit = 1; Bit <= MaxBitsPerSymbol; Bit++) {
                Symbol             <<= 1;
                Symbol              |= BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1);
                Count                = Tree->Frequency[Bit];
                if (Symbol - Count < FirstSymbolOfLength) {
                    Symbol           = Tree->Symbol[Bit + (Symbol - FirstSymbolOfLength)];
                }
            }
        } else if (Tree == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("HuffmanTree Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
        return Symbol;
    }

    void PNG_Flate_ReadHuffman(void *Options, BitBuffer *BitB, HuffmanTree *LengthTree, HuffmanTree *DistanceTree, ImageContainer *Image) { // Codes in Puff
        if (Options != NULL && BitB != NULL && LengthTree != NULL && DistanceTree != NULL && Image != NULL) {
            // Out = ImageContainer array
            PNGOptions *PNG = Options;
            uint64_t Symbol = 0ULL;
            uint64_t Offset = 0ULL;
            do {
                Symbol                              = ReadSymbol(BitB, LengthTree);;
                if (Symbol > 256) { // length
                    Symbol  -= 257;
                    uint64_t Length                 = LengthBase[Symbol] + BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, LengthAdditionalBits[Symbol]);

                    Symbol                          = ReadSymbol(BitB, DistanceTree);
                    uint64_t Distance               = DistanceBase[Symbol] + BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, DistanceAdditionalBits[Symbol]);

                    uint8_t ****ImageArray          = (uint8_t****) ImageContainer_GetArray(Image);
                    if (ImageArray != NULL) {
                        for (uint64_t NumBytes2Copy = 0; NumBytes2Copy < Length; NumBytes2Copy++) {
                            // Copy NumBytes2Copde from Offset - Distance

                            // We need to convert the View, Width, Height, and Channel as well as BitDepth into a byte location.
                            // This function might also be useful in ContainerIO as well.
                        }
                    } else {
                        Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't get ImageArray"));
                    }
                }
            } while (Symbol != EndOfBlock); // end of block symbol
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (LengthTree == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("LengthTree Pointer is NULL"));
        } else if (DistanceTree == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("DistanceTree Pointer is NULL"));
        } else if (Image == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        }
    }
    */

    void PNG_Filter_Sub(ImageContainer *Image) {
        if (Image != NULL) {
            MediaIO_ImageTypes Type = ImageContainer_GetType(Image);
            ImageChannelMap   *Map  = ImageContainer_GetChannelMap(Image);
            if (Type == ImageType_Integer8) {
                uint8_t  *ImageArray = (uint8_t*)  ImageContainer_GetArray(Image);

                for (uint8_t StereoView = 0; StereoView < ImageChannelMap_GetNumViews(Map); StereoView++) {
                    for (uint32_t Height = 0UL; Height < ImageContainer_GetHeight(Image); Height++) {
                        for (uint32_t Width = 0UL; Width < ImageContainer_GetWidth(Image); Width++) {
                            for (uint32_t Byte = 0UL; Byte < Bits2Bytes(ImageType_GetBitDepth(Type), RoundingType_Up); Byte++) {
                                ImageArray[StereoView * Height * Width * Byte] = (ImageArray[StereoView * Height * Width * Byte] + ImageArray[StereoView * Height * Width * (Byte + 1)]) % 256;
                            }
                        }
                    }
                }
            } else {
                uint16_t *ImageArray = (uint16_t*) ImageContainer_GetArray(Image);

                for (uint8_t StereoView = 0; StereoView < ImageChannelMap_GetNumViews(Map); StereoView++) {
                    for (uint32_t Height = 0UL; Height < ImageContainer_GetHeight(Image); Height++) {
                        for (uint32_t Width = 0UL; Width < ImageContainer_GetWidth(Image); Width++) {
                            for (uint32_t Byte = 0UL; Byte < Bits2Bytes(ImageType_GetBitDepth(Type), RoundingType_Up); Byte++) {
                                ImageArray[StereoView * Height * Width * Byte] = (ImageArray[StereoView * Height * Width * Byte] + ImageArray[StereoView * Height * Width * (Byte + 1)]) % 65536;
                            }
                        }
                    }
                }
            }
        } else if (Image == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        }
    }

    void PNG_Filter_Up(ImageContainer *Image) {
        if (Image != NULL) {
            MediaIO_ImageTypes Type = ImageContainer_GetType(Image);
            ImageChannelMap   *Map  = ImageContainer_GetChannelMap(Image);
            if (Type == ImageType_Integer8) {
                uint8_t  *ImageArray = (uint8_t*)  ImageContainer_GetArray(Image);

                for (uint8_t StereoView = 0; StereoView < ImageChannelMap_GetNumViews(Map); StereoView++) {
                    for (uint32_t Height = 0UL; Height < ImageContainer_GetHeight(Image); Height++) {
                        for (uint32_t Width = 0UL; Width < ImageContainer_GetWidth(Image); Width++) {
                            for (uint32_t Byte = 0UL; Byte < Bits2Bytes(ImageType_GetBitDepth(Type), RoundingType_Up); Byte++) {
                                ImageArray[StereoView * Height * Width * Byte] = (ImageArray[StereoView * Height * Width * Byte] + ImageArray[StereoView * (Height - 1) * Width * Byte]) % 256;
                            }
                        }
                    }
                }
            } else {
                uint16_t *ImageArray = (uint16_t*) ImageContainer_GetArray(Image);

                for (uint8_t StereoView = 0; StereoView < ImageChannelMap_GetNumViews(Map); StereoView++) {
                    for (uint32_t Height = 0UL; Height < ImageContainer_GetHeight(Image); Height++) {
                        for (uint32_t Width = 0UL; Width < ImageContainer_GetWidth(Image); Width++) {
                            for (uint32_t Byte = 0UL; Byte < Bits2Bytes(ImageType_GetBitDepth(Type), RoundingType_Up); Byte++) {
                                ImageArray[StereoView * Height * Width * Byte] = (ImageArray[StereoView * Height * Width * Byte] + ImageArray[StereoView * (Height - 1) * Width * Byte]) % 65536;
                            }
                        }
                    }
                }
            }
        } else if (Image == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        }
    }

    void PNG_Filter_Average(ImageContainer *Image) {
        if (Image != NULL) {
            MediaIO_ImageTypes Type = ImageContainer_GetType(Image);
            ImageChannelMap   *Map  = ImageContainer_GetChannelMap(Image);
            if (Type == ImageType_Integer8) {
                uint8_t  *ImageArray = (uint8_t*)  ImageContainer_GetArray(Image);

                for (uint8_t StereoView = 0; StereoView < ImageChannelMap_GetNumViews(Map); StereoView++) {
                    for (uint32_t Height = 0UL; Height < ImageContainer_GetHeight(Image); Height++) {
                        for (uint32_t Width = 0UL; Width < ImageContainer_GetWidth(Image); Width++) {
                            for (uint32_t Byte = 0UL; Byte < Bits2Bytes(ImageType_GetBitDepth(Type), RoundingType_Up); Byte++) {
                                float   PreAverage = (ImageArray[StereoView * Height * Width * (Byte - Bits2Bytes(ImageType_GetBitDepth(Type), RoundingType_Up))] + ImageArray[StereoView * Height * (Width - 1) * Byte]) / 2;
                                uint8_t Average    = FloorF(PreAverage) % 256;

                                ImageArray[StereoView * Height * Width * Byte] = Average;
                            }
                        }
                    }
                }
            } else {
                uint16_t *ImageArray = (uint16_t*) ImageContainer_GetArray(Image);

                for (uint8_t StereoView = 0; StereoView < ImageChannelMap_GetNumViews(Map); StereoView++) {
                    for (uint32_t Height = 0UL; Height < ImageContainer_GetHeight(Image); Height++) {
                        for (uint32_t Width = 0UL; Width < ImageContainer_GetWidth(Image); Width++) {
                            for (uint32_t Byte = 0UL; Byte < Bits2Bytes(ImageType_GetBitDepth(Type), RoundingType_Up); Byte++) {
                                float   PreAverage = (ImageArray[StereoView * Height * Width * (Byte - Bits2Bytes(ImageType_GetBitDepth(Type), RoundingType_Up))] + ImageArray[StereoView * Height * (Width - 1) * Byte]) / 2;
                                uint16_t Average   = FloorF(PreAverage) % 65536;

                                ImageArray[StereoView * Height * Width * Byte] = Average;
                            }
                        }
                    }
                }
            }
        } else if (Image == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        }
    }

    void PNG_Filter_Paeth(ImageContainer *Image) {
        if (Image != NULL) {
            MediaIO_ImageTypes Type = ImageContainer_GetType(Image);
            ImageChannelMap   *Map  = ImageContainer_GetChannelMap(Image);
            if (Type == ImageType_Integer8) {
                uint8_t  *ImageArray = (uint8_t*)  ImageContainer_GetArray(Image);

                for (uint8_t StereoView = 0; StereoView < ImageChannelMap_GetNumViews(Map); StereoView++) {
                    for (uint32_t Height = 0UL; Height < ImageContainer_GetHeight(Image); Height++) {
                        for (uint32_t Width = 0UL; Width < ImageContainer_GetWidth(Image); Width++) {
                            for (uint32_t Byte = 0UL; Byte < Bits2Bytes(ImageType_GetBitDepth(Type), RoundingType_Up); Byte++) {
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

                for (uint8_t StereoView = 0; StereoView < ImageChannelMap_GetNumViews(Map); StereoView++) {
                    for (uint32_t Height = 0UL; Height < ImageContainer_GetHeight(Image); Height++) {
                        for (uint32_t Width = 0UL; Width < ImageContainer_GetWidth(Image); Width++) {
                            for (uint32_t Byte = 0UL; Byte < Bits2Bytes(ImageType_GetBitDepth(Type), RoundingType_Up); Byte++) {
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
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
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
                        case PNGFilter_Unfiltered:
                            // copy the Line except byte 0 (the filter indication byte) to the output buffer.
                            // With the ImageContainer framework the way it is, this is only possible at the end.
                            break;
                        case PNGFilter_Sub:
                            PNG_Filter_Sub(Image);
                            break;
                        case PNGFilter_Up:
                            PNG_Filter_Up(Image);
                            break;
                        case PNGFilter_Average:
                            PNG_Filter_Average(Image);
                            break;
                        case PNGFilter_Paeth:
                            PNG_Filter_Paeth(Image);
                            break;
                        default:
                            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Filter type: %d is invalid"), ImageArray[0][ScanLine][0][0]);
                            break;
                    }
                }
            } else {
                // Image16
                uint16_t ****ImageArray = (uint16_t****) ImageContainer_GetArray(Image);

                for (size_t ScanLine = 0; ScanLine < ImageContainer_GetWidth(Image); ScanLine++) {
                    switch (ImageArray[0][ScanLine][0][0]) {
                        case PNGFilter_Unfiltered:
                            // copy the Line except byte 0 (the filter indication byte) to the output buffer.
                            break;
                        case PNGFilter_Sub:
                            PNG_Filter_Sub(Image);
                            break;
                        case PNGFilter_Up:
                            PNG_Filter_Up(Image);
                            break;
                        case PNGFilter_Average:
                            PNG_Filter_Average(Image);
                            break;
                        case PNGFilter_Paeth:
                            PNG_Filter_Paeth(Image);
                            break;
                        default:
                            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Filter type: %d is invalid"), ImageArray[0][ScanLine][0][0]);
                            break;
                    }
                }
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        }
    }

    void PNG_DAT_Parse(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG         = Options;
            bool     Is3D           = PNG->sTER->StereoType > 0 ? Yes : No;
            uint8_t  BitDepth       = PNG->iHDR->BitDepth;
            uint8_t  ColorType      = PNG->iHDR->ColorType;
            uint8_t  NumChannels    = PNGNumChannelsFromColorType[ColorType];
            uint64_t Width          = PNG->iHDR->Width;
            uint64_t Height         = PNG->iHDR->Height;
            ImageContainer *Decoded = NULL;
            Image_ChannelMask Mask  = 0;

            Flate_ReadZlibHeader(PNG, BitB);

            if (Is3D == true) {
                Mask += ImageMask_3D_L;
                Mask += ImageMask_3D_R;
            }

            if (ColorType == PNGColor_Gray) {
                Mask += ImageMask_Luma;
            } else if (ColorType == PNGColor_GrayAlpha) {
                Mask += ImageMask_Luma;
                Mask += ImageMask_Alpha;
            } else if (ColorType == PNGColor_RGB || ColorType == PNGColor_Palette) {
                Mask += ImageMask_Red;
                Mask += ImageMask_Green;
                Mask += ImageMask_Blue;
            } else if (ColorType == PNGColor_RGBAlpha) {
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
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitDepth %d is invalid"), BitDepth);
            }

            //PNG_DAT_Decode(PNG, BitB, Decoded);
            Flate_ReadDeflateBlock(PNG, BitB, Decoded);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void PNG_Adam7_Deinterlace(ImageContainer *Image) {
        if (Image != NULL) {

        } else if (Image == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        }
    }

    void PNG_IHDR_Parse(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG           = Options;
            PNG->iHDR->Width          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
            PNG->iHDR->Height         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
            PNG->iHDR->BitDepth       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
            PNG->iHDR->ColorType      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
            if (PNG->iHDR->ColorType == 1 || PNG->iHDR->ColorType == 5 || PNG->iHDR->ColorType >= 7) {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Invalid color type: %d"), PNG->iHDR->ColorType);
            }
            PNG->iHDR->Compression    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
            PNG->iHDR->FilterMethod   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
            PNG->iHDR->IsInterlaced   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void PNG_PLTE_Parse(void *Options, BitBuffer *BitB, uint32_t ChunkSize) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG  = Options;
            uint8_t BitDepth = PNG->iHDR->BitDepth;
            if (BitDepth <= 8) {
                uint8_t ColorType = PNG->iHDR->ColorType;
                if (ColorType == PNGColor_Palette || ColorType == PNGColor_RGB) {
                    //PNG_PLTE_Init(Ovia);
                    for (uint32_t Entry = 0UL; Entry < ChunkSize / 3; Entry++) {
                        PNG->PLTE->Palette[0] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 8);
                        PNG->PLTE->Palette[1] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 8);
                        PNG->PLTE->Palette[2] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 8);
                    }
                } else if (ColorType == PNGColor_RGBAlpha) {
                    for (uint32_t Entry = 0UL; Entry < ChunkSize / 3; Entry++) {
                        PNG->PLTE->Palette[0] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 8);
                        PNG->PLTE->Palette[1] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 8);
                        PNG->PLTE->Palette[2] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 8);
                        PNG->PLTE->Palette[3] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 8);
                    }
                }
            } else {
                BitBuffer_Seek(BitB, Bytes2Bits(ChunkSize));
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitDepth %d can't have a palette"), BitDepth);
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void PNG_TRNS_Parse(void *Options, BitBuffer *BitB, uint32_t ChunkSize) { // Transparency
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG       = Options;
            uint32_t NumEntries   = ChunkSize % 3;
            uint16_t **Entries    = NULL;
            if (PNG->iHDR->ColorType == PNGColor_RGB) {
                Entries = calloc(3, Bits2Bytes(PNG->iHDR->BitDepth, RoundingType_Up) * sizeof(uint16_t));
            } else if (PNG->iHDR->ColorType == PNGColor_RGBAlpha) {
                Entries = calloc(4, Bits2Bytes(PNG->iHDR->BitDepth, RoundingType_Up) * sizeof(uint16_t));
            } else if (PNG->iHDR->ColorType == PNGColor_Gray) {
                Entries = calloc(1, Bits2Bytes(PNG->iHDR->BitDepth, RoundingType_Up) * sizeof(uint16_t));
            } else if (PNG->iHDR->ColorType == PNGColor_GrayAlpha) {
                Entries = calloc(2, Bits2Bytes(PNG->iHDR->BitDepth, RoundingType_Up) * sizeof(uint16_t));
            }
            if (Entries != NULL) {
                for (uint8_t Color = 0; Color < PNGNumChannelsFromColorType[PNG->iHDR->ColorType]; Color++) {
                    Entries[Color]    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, Bits2Bytes(PNG->iHDR->BitDepth, RoundingType_Up));
                }
                //Ovia->tRNS->Palette = Entries;
            } else {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Failed to allocate enough memory for the TRNS Palette"));
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        } else if (ChunkSize % 3 != 0) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("The ChunkSize MUST be divisible by 3"));
        }
    }

    void PNG_BKGD_Parse(void *Options, BitBuffer *BitB) { // Background
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG = Options;
            for (uint8_t Entry = 0; Entry < PNGNumChannelsFromColorType[PNG->iHDR->ColorType]; Entry++) {
                PNG->bkGD->BackgroundPaletteEntry[Entry] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, PNG->iHDR->BitDepth);
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void PNG_CHRM_Parse(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG        = Options;
            PNG->cHRM->WhitePointX = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
            PNG->cHRM->WhitePointY = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
            PNG->cHRM->RedX        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
            PNG->cHRM->RedY        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
            PNG->cHRM->GreenX      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
            PNG->cHRM->GreenY      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
            PNG->cHRM->BlueX       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
            PNG->cHRM->BlueY       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void PNG_GAMA_Parse(void *Options, BitBuffer *BitB) { // Gamma
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG  = Options;
            PNG->gAMA->Gamma = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void PNG_OFFS_Parse(void *Options, BitBuffer *BitB) { // Image Offset
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG          = Options;
            PNG->oFFs->XOffset       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
            PNG->oFFs->YOffset       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
            PNG->oFFs->UnitSpecifier = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void PNG_PHYS_Parse(void *Options, BitBuffer *BitB) { // Aspect ratio, Physical pixel size
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG               = Options;
            PNG->pHYs->PixelsPerUnitXAxis = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
            PNG->pHYs->PixelsPerUnitYAxis = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
            PNG->pHYs->UnitSpecifier      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void PNG_SCAL_Parse(void *Options, BitBuffer *BitB) { // Physical Scale
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG           = Options;
            PNG->sCAL->UnitSpecifier  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8); // 1 = Meter, 2 = Radian

            uint32_t WidthStringSize  = BitBuffer_GetUTF8StringSize(BitB);
            uint32_t HeightStringSize = BitBuffer_GetUTF8StringSize(BitB);

            UTF8 *WidthString         = BitBuffer_ReadUTF8(BitB, WidthStringSize);
            UTF8 *HeightString        = BitBuffer_ReadUTF8(BitB, HeightStringSize);

            PNG->sCAL->PixelWidth     = UTF8_String2Decimal(Base_Decimal | Base_Radix10, WidthString);
            PNG->sCAL->PixelHeight    = UTF8_String2Decimal(Base_Decimal | Base_Radix10, HeightString);

            free(WidthString);
            free(HeightString);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void PNG_PCAL_Parse(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG                      = Options;
            uint8_t CalibrationSize              = BitBuffer_GetUTF8StringSize(BitB);
            PNG->pCAL->CalibrationName           = BitBuffer_ReadUTF8(BitB, CalibrationSize);
            PNG->pCAL->OriginalZero              = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
            PNG->pCAL->OriginalMax               = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
            PNG->pCAL->EquationType              = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
            PNG->pCAL->NumParams                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
            uint8_t UnitNameSize                 = BitBuffer_GetUTF8StringSize(BitB);
            PNG->pCAL->UnitName                  = BitBuffer_ReadUTF8(BitB, UnitNameSize);
            BitBuffer_Seek(BitB, 8); // NULL seperator
            PNG->pCAL->Parameters                = calloc(PNG->pCAL->NumParams, sizeof(double));
            if (PNG->pCAL->Parameters != NULL) {
                for (uint8_t Param = 0; Param < PNG->pCAL->NumParams; Param++) {
                    uint8_t ParameterSize        = BitBuffer_GetUTF8StringSize(BitB);
                    UTF8   *ParameterString      = BitBuffer_ReadUTF8(BitB, ParameterSize);
                    PNG->pCAL->Parameters[Param] = UTF8_String2Decimal(Base_Decimal | Base_Radix10, ParameterString);
                }
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void PNG_SBIT_Parse(void *Options, BitBuffer *BitB) { // Significant bits per sample
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG          = Options;
            PNGColorTypes ColorType  = PNG->iHDR->ColorType;
            if (ColorType == PNGColor_Gray) {
                PNG->sBIT->Grayscale = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
            } else if (ColorType == PNGColor_RGB || ColorType == PNGColor_Palette) {
                PNG->sBIT->Red       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
                PNG->sBIT->Green     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
                PNG->sBIT->Blue      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
            } else if (ColorType == PNGColor_GrayAlpha) {
                PNG->sBIT->Grayscale = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
                PNG->sBIT->Alpha     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
            } else if (ColorType == PNGColor_RGBAlpha) {
                PNG->sBIT->Red       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
                PNG->sBIT->Green     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
                PNG->sBIT->Blue      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
                PNG->sBIT->Alpha     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void PNG_SRGB_Parse(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG            = Options;
            PNG->sRGB->RenderingIntent = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void PNG_STER_Parse(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG       = Options;
            PNG->sTER->StereoType = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
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
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void PNG_TEXT_Parse(void *Options, BitBuffer *BitB) { // Uncompressed, ASCII tEXt
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG = Options;
            for (uint32_t TextChunk = 0UL; TextChunk < PNG->NumTextChunks; TextChunk++) {
                if (PNG->Text[TextChunk].TextType == PNGTextType_tEXt) { // ASCII aka Latin-1

                } else if (PNG->Text[TextChunk].TextType == PNGTextType_iTXt) { // Unicode, UTF-8

                } else if (PNG->Text[TextChunk].TextType == PNGTextType_zTXt) { // Compressed

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
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void PNG_TIME_Parse(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG                     = Options;
            PNG->tIMe->Year                     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16);
            PNG->tIMe->Month                    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
            PNG->tIMe->Day                      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
            PNG->tIMe->Hour                     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
            PNG->tIMe->Minute                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
            PNG->tIMe->Second                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    /* APNG */
    void PNG_ACTL_Parse(void *Options, BitBuffer *BitB) { // Animation control, part of APNG
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG                  = Options;
            PNG->acTL->NumFrames             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
            PNG->acTL->TimesToLoop           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32); // If 0, loop forever.
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void PNG_FCTL_Parse(void *Options, BitBuffer *BitB) { // Frame Control, part of APNG
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG                  = Options;
            PNG->fcTL->FrameNum              = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
            PNG->fcTL->Width                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
            PNG->fcTL->Height                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
            PNG->fcTL->XOffset               = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
            PNG->fcTL->YOffset               = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
            PNG->fcTL->FrameDelayNumerator   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16);
            PNG->fcTL->FrameDelayDenominator = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16);
            PNG->fcTL->DisposeMethod         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
            PNG->fcTL->BlendMethod           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    /* End APNG */

    void PNG_HIST_Parse(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG      = Options;
            PNG->hIST->NumColors = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
            for (uint32_t Color = 0; Color < PNG->hIST->NumColors; Color++) {
                PNG->hIST->Histogram[Color] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, PNG->iHDR->BitDepth);
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void PNG_ICCP_Parse(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG            = Options;
            uint8_t ProfileNameSize    = BitBuffer_GetUTF8StringSize(BitB);
            PNG->iCCP->ProfileName     = BitBuffer_ReadUTF8(BitB, ProfileNameSize);
            PNG->iCCP->CompressionType = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
            // Decompress the data with Zlib
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
        uint8_t ProfileNameSize = 0;
        ProfileNameSize = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8);
    }

    void PNG_SPLT_Parse(void *Options, BitBuffer *BitB, uint32_t ChunkSize) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG                         = Options;
            PNGColorTypes ColorType                 = PNG->iHDR->ColorType;
            uint8_t BitDepthInBytes                 = Bits2Bytes(PNG->iHDR->BitDepth, RoundingType_Up);
            uint8_t PaletteNameSize                 = BitBuffer_GetUTF8StringSize(BitB);
            PNG->sPLT->NumEntries                   = ChunkSize - (PaletteNameSize + TextIO_NULLTerminator) + 1;
            if (ColorType == PNGColor_RGBAlpha || ColorType == PNGColor_GrayAlpha) {
                PNG->sPLT->NumEntries              /= 4;
            } else {
                PNG->sPLT->NumEntries              /= 3;
            }
            PNG->sPLT[PNG->NumSPLTChunks - 1].PaletteName = BitBuffer_ReadUTF8(BitB, PaletteNameSize);
            for (uint32_t Entry = 0; Entry < PNG->sPLT->NumEntries; Entry++) {
                PNG->sPLT->Palette[Entry].Red       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, Bytes2Bits(BitDepthInBytes));
                PNG->sPLT->Palette[Entry].Green     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, Bytes2Bits(BitDepthInBytes));
                PNG->sPLT->Palette[Entry].Blue      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, Bytes2Bits(BitDepthInBytes));
                if (ColorType == PNGColor_RGBAlpha || ColorType == PNGColor_GrayAlpha) {
                    PNG->sPLT->Palette[Entry].Alpha = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, Bytes2Bits(BitDepthInBytes));
                }
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void PNG_Parse(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG          = Options;
            while (BitBuffer_GetSize(BitB) > 0) {
                uint32_t ChunkSize   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);
                uint32_t ChunkID     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32);

                if (ChunkID == PNGMarker_acTL) {
                    PNG_ACTL_Parse(PNG, BitB);
                } else if (ChunkID == PNGMarker_bKGD) {
                    PNG_BKGD_Parse(PNG, BitB);
                } else if (ChunkID == PNGMarker_cHRM) {
                    PNG_CHRM_Parse(PNG, BitB);
                } else if (ChunkID == PNGMarker_fcTL) {
                    PNG_FCTL_Parse(PNG, BitB);
                } else if (ChunkID == PNGMarker_gAMA) {
                    PNG_GAMA_Parse(PNG, BitB);
                } else if (ChunkID == PNGMarker_hIST) {
                    PNG_HIST_Parse(PNG, BitB);
                } else if (ChunkID == PNGMarker_iCCP) {
                    PNG_OFFS_Parse(PNG, BitB);
                } else if (ChunkID == PNGMarker_iDAT || ChunkID == PNGMarker_fDAT) {
                    PNG_DAT_Parse(PNG, BitB);
                } else if (ChunkID == PNGMarker_iHDR) {
                    PNG_IHDR_Parse(PNG, BitB);
                } else if (ChunkID == PNGMarker_oFFs) {
                    PNG_OFFS_Parse(PNG, BitB);
                } else if (ChunkID == PNGMarker_pCAL) {
                    PNG_PCAL_Parse(PNG, BitB);
                } else if (ChunkID == PNGMarker_pHYs) {
                    PNG_PHYS_Parse(PNG, BitB);
                } else if (ChunkID == PNGMarker_PLTE) {
                    PNG_PLTE_Parse(PNG, BitB, ChunkSize);
                } else if (ChunkID == PNGMarker_sBIT) {
                    PNG_SBIT_Parse(PNG, BitB);
                } else if (ChunkID == PNGMarker_sRGB) {
                    PNG_SRGB_Parse(PNG, BitB);
                } else if (ChunkID == PNGMarker_sTER) {
                    PNG_STER_Parse(PNG, BitB);
                } else if (ChunkID == PNGMarker_tEXt) {
                    PNG->NumTextChunks += 1;
                    PNG->Text[PNG->NumTextChunks - 1].TextType = PNGMarker_tEXt;
                    PNG_TEXT_Parse(PNG, BitB);
                } else if (ChunkID == PNGMarker_zTXt) {
                    PNG->NumTextChunks += 1;
                    PNG->Text[PNG->NumTextChunks - 1].TextType = PNGTextType_zTXt;
                    PNG_TEXT_Parse(PNG, BitB);
                } else if (ChunkID == PNGMarker_iTXt) {
                    PNG->NumTextChunks += 1;
                    PNG->Text[PNG->NumTextChunks - 1].TextType = PNGTextType_iTXt;
                    PNG_TEXT_Parse(PNG, BitB);
                } else if (ChunkID == PNGMarker_sCAL) {
                    PNG_SCAL_Parse(PNG, BitB);
                } else if (ChunkID == PNGMarker_tIME) {
                    PNG_TIME_Parse(PNG, BitB);
                } else if (ChunkID == PNGMarker_tRNS) {
                    PNG_TRNS_Parse(PNG, BitB, ChunkSize);
                } else if (ChunkID == PNGMarker_sPLT) {
                    PNG_SPLT_Parse(PNG, BitB, ChunkSize);
                }
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void PNG_Extract(void *Options, BitBuffer *BitB, void *Container) {
        if (Options != NULL && BitB != NULL) {
            PNGOptions *PNG = Options;
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    // Hmm, for parsing chunks I should have basically a PNG muxer that pops out bit buffers with the data for each chunk...
    // So, we need to

    void ParseIHDR(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) {
        /* When checking the CRC, you need to skip over the ChunkSize field, but include the ChunkID */
        uint32_t GeneratedCRC     = GenerateCRC32(InputPNG, ChunkSize);
        Dec->iHDR->Width          = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 32);
        Dec->iHDR->Height         = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 32);
        Dec->iHDR->BitDepth       = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 8);
        Dec->iHDR->ColorType      = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 8);
        if (Dec->iHDR->ColorType == 1 || Dec->iHDR->ColorType == 5 || Dec->iHDR->ColorType >= 7) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Invalid color type: %d"), Dec->iHDR->ColorType);
        }
        Dec->iHDR->Compression    = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 8);
        Dec->iHDR->FilterMethod   = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 8);
        Dec->iHDR->IsInterlaced   = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 8);

        uint32_t CRC              = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 32);
        if (GeneratedCRC != CRC) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("CRC Mismatch"));
        }

        //VerifyCRC(Dec->iHDR, ChunkSize, 1, 1, CRC);
    }

    void ParsePLTE(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Palette
        if (Dec->iHDR->BitDepth > 8) { // INVALID
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Invalid bit depth %d and palette combination"), Dec->iHDR->BitDepth);
            BitBuffer_Seek(InputPNG, Bytes2Bits(ChunkSize));
        } else {

            if (Dec->iHDR->ColorType == PNG_PalettedRGB || Dec->iHDR->ColorType == PNG_RGB) {
                Dec->PLTE->Palette = calloc(1, (3 * Dec->PLTE->NumEntries) * Bits2Bytes(Dec->iHDR->BitDepth, Yes));
            } else if (Dec->iHDR->ColorType == PNG_RGBA) {
                Dec->PLTE->Palette = calloc(1, (4 * Dec->PLTE->NumEntries) * Bits2Bytes(Dec->iHDR->BitDepth, Yes));
            }


            for (uint8_t Channel = 0; Channel < ModernPNGChannelsPerColorType[Dec->iHDR->ColorType]; Channel++) {
                for (uint16_t PaletteEntry = 0; PaletteEntry < ChunkSize / 3; PaletteEntry++) {
                    Dec->PLTE->Palette[Channel][PaletteEntry] = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, Dec->iHDR->BitDepth);
                }
            }
        }
    }

    void ParseTRNS(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Transparency
        Dec->tRNS->NumEntries = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 32);
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
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Failed to allocate enough memory for the TRNS Palette"));
        } else {
            for (uint8_t Color = 0; Color < ModernPNGChannelsPerColorType[Dec->iHDR->ColorType]; Color++) {
                Entries[Color]    = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, Bits2Bytes(Dec->iHDR->BitDepth, true));
            }
            //Dec->tRNS->Palette = Entries;
        }
    }

    void ParseBKGD(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Background
        for (uint8_t Entry = 0; Entry < 3; Entry++) {
            Dec->bkGD->BackgroundPaletteEntry[Entry] = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 8);
        }
    }

    void ParseCHRM(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Chromaticities
        Dec->cHRM->WhitePointX = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 32);
        Dec->cHRM->WhitePointY = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 32);
        Dec->cHRM->RedX        = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 32);
        Dec->cHRM->RedY        = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 32);
        Dec->cHRM->GreenX      = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 32);
        Dec->cHRM->GreenY      = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 32);
        Dec->cHRM->BlueX       = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 32);
        Dec->cHRM->BlueY       = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 32);
    }

    void ParseGAMA(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Gamma
        Dec->gAMA->Gamma = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 32);
    }

    void ParseOFFS(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Image Offset
        Dec->oFFs->XOffset       = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 32);
        Dec->oFFs->YOffset       = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 32);
        Dec->oFFs->UnitSpecifier = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 8);
    }

    void ParsePHYS(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Aspect ratio, Physical pixel size
        Dec->pHYs->PixelsPerUnitXAxis = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 32);
        Dec->pHYs->PixelsPerUnitYAxis = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 32);
        Dec->pHYs->UnitSpecifier      = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 8);
    }

    void ParseSCAL(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Physical Scale
                                                                              // Ok, so we need to take the size of the chunk into account.
        Dec->sCAL->UnitSpecifier = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 8); // 1 = Meter, 2 = Radian

        uint32_t WidthStringSize  = 0;
        uint32_t HeightStringSize = 0;

        for (uint32_t Byte = 0UL; Byte < ChunkSize - 1; Byte++) { // Get the sizes for the field strings
            if (PeekBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 8) != '\0') {
                WidthStringSize += 1;
            }
        }
        HeightStringSize = ChunkSize - (WidthStringSize + 1);

        UTF8 *WidthString = calloc(WidthStringSize, sizeof(UTF8));
        UTF8 *HeightString = calloc(HeightStringSize, sizeof(UTF8));

        for (uint32_t WidthCodePoint = 0; WidthCodePoint < WidthStringSize; WidthCodePoint++) {
            WidthString[WidthCodePoint] = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 8);
        }
        BitBuffer_Seek(InputPNG, 8); // Skip the NULL seperator
        for (uint32_t HeightCodePoint = 0; HeightCodePoint < HeightStringSize; HeightCodePoint++) {
            HeightString[HeightCodePoint] = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 8);
        }

        Dec->sCAL->PixelWidth  = UTF8_String2Decimal(WidthString);
        Dec->sCAL->PixelHeight = UTF8_String2Decimal(HeightString);
    }

    void ParsePCAL(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) {
        char CalibrationName[80];
        while (PeekBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 8) != 0x0) {
            for (uint8_t Byte = 0; Byte < 80; Byte++) {
                CalibrationName[Byte] = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 8);
            }
        }
        Dec->pCAL->CalibrationName     = CalibrationName;
        Dec->pCAL->CalibrationNameSize = UTF8_GetStringSizeInCodePoints(Dec->pCAL->CalibrationName);
    }

    void ParseSBIT(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Significant bits per sample
        Dec->sBIT->Red                   = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 8);
        Dec->sBIT->Green                 = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 8);
        Dec->sBIT->Blue                  = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 8);
    }

    void ParseSRGB(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) {
        Dec->sRGB->RenderingIntent       = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 8);
    }

    void ParseSTER(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) {
        Dec->PNGIs3D                     = true;
        Dec->sTER->StereoType            = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 8);

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
            CurrentByte  = PeekBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 8);
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
        Dec->tIMe->Year                  = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 16);
        Dec->tIMe->Month                 = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 8);
        Dec->tIMe->Day                   = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 8);
        Dec->tIMe->Hour                  = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 8);
        Dec->tIMe->Minute                = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 8);
        Dec->tIMe->Second                = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 8);
    }

    /* APNG */
    void ParseACTL(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Animation control, part of APNG
        Dec->PNGIsAnimated               = true;
        Dec->acTL->NumFrames             = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 32);
        Dec->acTL->TimesToLoop           = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 32); // If 0, loop forever.
    }

    void ParseFCTL(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Frame Control, part of APNG
        Dec->fcTL->FrameNum              = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 32);
        Dec->fcTL->Width                 = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 32);
        Dec->fcTL->Height                = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 32);
        Dec->fcTL->XOffset               = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 32);
        Dec->fcTL->YOffset               = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 32);
        Dec->fcTL->FrameDelayNumerator   = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 16);
        Dec->fcTL->FrameDelayDenominator = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 16);
        Dec->fcTL->DisposeMethod         = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 8);
        Dec->fcTL->BlendMethod           = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 8);
    }
    /* End APNG */

    void ParseHIST(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) {
    }

    void ParseICCP(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) {
        uint8_t ProfileNameSize = 0;
        ProfileNameSize = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 8);
    }

    void ParseSPLT(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) {
    }

    uint8_t ParsePNGMetadata(BitBuffer *InputPNG, DecodePNG *Dec) {
        uint64_t FileMagic    = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 64);

        if (FileMagic == PNGMagic) {
            char     *ChunkID        = calloc(4, sizeof(uint8_t));
            uint32_t ChunkSize       = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 32);

            for (uint8_t ChunkIDSize = 0; ChunkIDSize < 4; ChunkIDSize++) {
                ChunkID[ChunkIDSize] = ReadBits(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, InputPNG, 8);
            }
            BitBuffer_Seek(InputPNG, -32); // Now we need to skip back so we can read the ChunkID as part of the CRC check.
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
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("File Magic 0x%X is not PNG, exiting"), FileMagic);
            exit(EXIT_FAILURE);
        }
        return EXIT_SUCCESS;
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
