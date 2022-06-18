#include "../../include/CodecIO/PNGCodec.h"
#include "../../include/EntropyIO/Flate.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h" /* Included for Assertions */
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/CryptographyIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/TextIO/StringIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/TextIO/LogIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    void PNG_Read_IHDR(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        /*
         ReadBits wanings to disable:
         warn_impcast_integer_precision
         warn_impcast_integer_64_32
         */
        Options->iHDR->Width          = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->iHDR->Height         = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->iHDR->BitDepth       = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        Options->iHDR->ColorType      = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        AssertIO(Options->iHDR->ColorType != 1);
        AssertIO(Options->iHDR->ColorType != 5);
        AssertIO(Options->iHDR->ColorType < 7);
        Options->Map = ImageChannelMap_Init(Options->sTERExists ? 2 : 1, PNG_GetNumChannels(Options->iHDR->ColorType));
        Options->iHDR->Compression    = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        Options->iHDR->FilterMethod   = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        Options->iHDR->IsInterlaced   = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        
        uint32_t ComputedCRC          = BitBuffer_CalculateCRC(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, BitBuffer_GetPosition(BitB) - 104, 13, 0xFFFFFFFF, CRCPolynomial_IEEE802_3);
        
        uint32_t CRC                  = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        AssertIO(ComputedCRC == CRC);
    }
    
    void PNG_Read_PLTE(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) { // Palette
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        if (Options->iHDR->ColorType == PNGColor_Palette || Options->iHDR->ColorType == PNGColor_RGB) {
            Options->PLTE->Palette = calloc(1, (3 * Options->PLTE->NumEntries) * Bits2Bytes(Options->iHDR->BitDepth, RoundingType_Down));
        } else if (Options->iHDR->ColorType == PNGColor_RGBAlpha) {
            Options->PLTE->Palette = calloc(1, (4 * Options->PLTE->NumEntries) * Bits2Bytes(Options->iHDR->BitDepth, RoundingType_Down));
        }
        
        
        for (uint8_t Channel = 0; Channel < PNG_GetNumChannels(Options->iHDR->ColorType); Channel++) {
            for (uint16_t PaletteEntry = 0; PaletteEntry < ChunkSize / 3; PaletteEntry++) {
                Options->PLTE->Palette[Channel][PaletteEntry] = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, Options->iHDR->BitDepth);
            }
        }
    }
    
    void PNG_Read_TRNS(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) { // Transparency
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->tRNS->NumEntries = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        uint16_t *Entries    = NULL;
        if (Options->iHDR->ColorType == PNGColor_RGB) {
            Entries = calloc(3, Bits2Bytes(Options->iHDR->BitDepth, RoundingType_Down) * sizeof(uint16_t));
        } else if (Options->iHDR->ColorType == PNGColor_RGBAlpha) {
            Entries = calloc(4, Bits2Bytes(Options->iHDR->BitDepth, RoundingType_Down) * sizeof(uint16_t));
        } else if (Options->iHDR->ColorType == PNGColor_Gray) {
            Entries = calloc(1, Bits2Bytes(Options->iHDR->BitDepth, RoundingType_Down) * sizeof(uint16_t));
        } else if (Options->iHDR->ColorType == PNGColor_GrayAlpha) {
            Entries = calloc(2, Bits2Bytes(Options->iHDR->BitDepth, RoundingType_Down) * sizeof(uint16_t));
        }
        AssertIO(Entries != NULL);
        for (uint8_t Color = 0; Color < PNG_GetNumChannels(Options->iHDR->ColorType); Color++) {
            Entries[Color]    = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, Bits2Bytes(Options->iHDR->BitDepth, RoundingType_Down));
        }
    }
    
    void PNG_Read_BKGD(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) { // Background
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        for (uint8_t Entry = 0; Entry < 3; Entry++) {
            Options->bkGD->BackgroundPaletteEntry[Entry] = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        }
    }
    
    void PNG_Read_CHRM(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) { // Chromaticities
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->cHRM->WhitePointX = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->cHRM->WhitePointY = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->cHRM->RedX        = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->cHRM->RedY        = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->cHRM->GreenX      = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->cHRM->GreenY      = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->cHRM->BlueX       = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->cHRM->BlueY       = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
    }
    
    void PNG_Read_GAMA(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) { // Gamma
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->gAMA->Gamma = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
    }
    
    void PNG_Read_OFFS(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) { // Image Offset
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->oFFs->XOffset       = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->oFFs->YOffset       = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->oFFs->UnitSpecifier = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
    }
    
    void PNG_Read_PHYS(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) { // Aspect ratio, Physical pixel size
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->pHYs->PixelsPerUnitXAxis = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->pHYs->PixelsPerUnitYAxis = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->pHYs->UnitSpecifier      = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
    }
    
    void PNG_Read_SCAL(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) { // Physical Scale
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        // Ok, so we need to take the size of the chunk into account.
        Options->sCAL->UnitSpecifier = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8); // 1 = Meter, 2 = Radian
        
        uint32_t WidthStringSize  = 0;
        uint32_t HeightStringSize = 0;
        
        for (uint32_t Byte = 0UL; Byte < ChunkSize - 1; Byte++) { // Get the sizes for the field strings
            if (BitBuffer_PeekBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8) != '\0') {
                WidthStringSize += 1;
            }
        }
        HeightStringSize = ChunkSize - (WidthStringSize + 1);
        
        UTF8 *WidthString = calloc(WidthStringSize, sizeof(UTF8));
        UTF8 *HeightString = calloc(HeightStringSize, sizeof(UTF8));
        
        for (uint32_t WidthCodePoint = 0; WidthCodePoint < WidthStringSize; WidthCodePoint++) {
            WidthString[WidthCodePoint] = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        }
        BitBuffer_Seek(BitB, 8); // Skip the NULL seperator
        for (uint32_t HeightCodePoint = 0; HeightCodePoint < HeightStringSize; HeightCodePoint++) {
            HeightString[HeightCodePoint] = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        }
        
        Options->sCAL->PixelWidth  = UTF8_String2Decimal(Base_Decimal | Base_Radix10, WidthString);
        Options->sCAL->PixelHeight = UTF8_String2Decimal(Base_Decimal | Base_Radix10, HeightString);
    }
    
    void PNG_Read_PCAL(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        UTF8 CalibrationName[80];
        while (BitBuffer_PeekBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8) != 0x0) {
            for (uint8_t Byte = 0; Byte < 80; Byte++) {
                CalibrationName[Byte] = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
            }
        }
        Options->pCAL->CalibrationName     = CalibrationName;
        Options->pCAL->CalibrationNameSize = UTF8_GetStringSizeInCodePoints(Options->pCAL->CalibrationName);
    }
    
    void PNG_Read_SBIT(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) { // Significant bits per sample
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->sBIT->Red                   = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        Options->sBIT->Green                 = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        Options->sBIT->Blue                  = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
    }
    
    void PNG_Read_SRGB(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->sRGB->RenderingIntent       = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
    }
    
    void PNG_Read_STER(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->sTER->StereoType            = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        
        // No matter what StereoType is used, both images are arranged side by side, and the left edge is aligned on a boundary of the 8th column in case interlacing is used.
        // The two sub images must have the same dimensions after padding is removed.
        // CROSS_FUSE_LAYOUT = 0, DIVERGING_FUSE_LAYOUT = 1
        // When CROSS_FUSE_LAYOUT, the right image is on the left, and the left image on the right.
        // When DIVERGING_FUSE_LAYOUT, The left eye view is on the left, and the right eye view is on the right.
        // The Left eye view is always in index 0 of the pixel array, so we need to swap this if nessicary.
        // So, make a function that strips padding from the stream, then make a function that decodes the sTER image the way it should be.
        // The standard recommends we use CROSS_FUSE_LAYOUT, so I'll probably end up using that.
        // So, let's say we have 2 1080p images, and we want to make a single Stereoscopic Options from them.
        //
    }
    
    void PNG_Read_TEXT(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) { // Uncompressed, ASCII tEXt
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        // Read until you hit a NULL for the name string, then subtract the size of the previous string from the total length to get the number of bytes for the second string
        uint8_t  KeywordSize = 0UL;
        uint8_t  CurrentByte = 0; // 1 is BULLshit
        
        do {
            CurrentByte  = BitBuffer_PeekBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
            KeywordSize += 1;
        } while (CurrentByte != 0);
        
        uint32_t CommentSize = ChunkSize - KeywordSize;
        
        Options->NumTextChunks  += 1;
        
        
        
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
    
    void PNG_Read_ZTXT(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) { // Compressed text
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
    }
    
    void PNG_Read_ITXT(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) { // International Text
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
    }
    
    void PNG_Read_TIME(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->tIMe->Year                  = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 16);
        Options->tIMe->Month                 = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        Options->tIMe->Day                   = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        Options->tIMe->Hour                  = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        Options->tIMe->Minute                = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        Options->tIMe->Second                = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
    }
    
    /* APNG */
    void PNG_Read_ACTL(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) { // Animation control, part of APNG
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->PNGIsAnimated               = true;
        Options->acTL->NumFrames             = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->acTL->TimesToLoop           = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32); // If 0, loop forever.
    }
    
    void PNG_Read_FCTL(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) { // Frame Control, part of APNG
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->fcTL->FrameNum              = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->fcTL->Width                 = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->fcTL->Height                = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->fcTL->XOffset               = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->fcTL->YOffset               = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->fcTL->FrameDelayNumerator   = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 16);
        Options->fcTL->FrameDelayDenominator = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 16);
        Options->fcTL->DisposeMethod         = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        Options->fcTL->BlendMethod           = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
    }
    /* End APNG */
    
    void PNG_Read_HIST(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
    }
    
    void PNG_Read_ICCP(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        uint8_t ProfileNameSize = 0;
        ProfileNameSize = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
    }
    
    void PNG_Read_SPLT(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
    }
    
    uint8_t PNG_ReadChunks(BitBuffer *BitB, PNGOptions *Options) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);

        uint32_t ChunkID         = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        uint32_t ChunkSize       = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        BitBuffer_Seek(BitB, -32); // Now we need to skip back so we can read the ChunkID as part of the CRC check.

        switch (ChunkID) {
            case PNGMarker_iHDR:
                PNG_Read_IHDR(Options, BitB, ChunkSize);
                break;
            case PNGMarker_PLTE:
                Options->PLTEExists = true;
                PNG_Read_PLTE(Options, BitB, ChunkSize);
                break;
            case PNGMarker_bKGD:
                Options->bkGDExists = true;
                PNG_Read_BKGD(Options, BitB, ChunkSize);
                break;
            case PNGMarker_cHRM:
                Options->cHRMExists = true;
                PNG_Read_CHRM(Options, BitB, ChunkSize);
                break;
            case PNGMarker_gAMA:
                Options->gAMAExists = true;
                PNG_Read_GAMA(Options, BitB, ChunkSize);
                break;
            case PNGMarker_oFFs:
                Options->oFFsExists = true;
                PNG_Read_OFFS(Options, BitB, ChunkSize);
                break;
            case PNGMarker_pHYs:
                Options->pHYsExists = true;
                PNG_Read_PHYS(Options, BitB, ChunkSize);
                break;
            case PNGMarker_sBIT:
                Options->sBITExists = true;
                PNG_Read_SBIT(Options, BitB, ChunkSize);
                break;
            case PNGMarker_sCAL:
                Options->sCALExists = true;
                PNG_Read_SCAL(Options, BitB, ChunkSize);
                break;
            case PNGMarker_sRGB:
                Options->sRGBExists = true;
                PNG_Read_SRGB(Options, BitB, ChunkSize);
                break;
            case PNGMarker_sTER:
                Options->sTERExists = true;
                PNG_Read_STER(Options, BitB, ChunkSize);
                break;
            case PNGMarker_tEXt:
                Options->TextExists = true;
                PNG_Read_TEXT(Options, BitB, ChunkSize);
                break;
            case PNGMarker_zTXt:
                Options->TextExists = true;
                PNG_Read_ZTXT(Options, BitB, ChunkSize);
                break;
            case PNGMarker_iTXt:
                Options->TextExists = true;
                PNG_Read_ITXT(Options, BitB, ChunkSize);
                break;
            case PNGMarker_tIME:
                Options->tIMEExists = true;
                PNG_Read_TIME(Options, BitB, ChunkSize);
                break;
            case PNGMarker_tRNS:
                Options->tRNSExists = true;
                PNG_Read_TRNS(Options, BitB, ChunkSize);
                break;
            case PNGMarker_hIST:
                // Make sure PLTe Exists
                AssertIO(Options->PLTEExists == true);
                Options->hISTExists = true;
                PNG_Read_HIST(Options, BitB, ChunkSize);
                break;
            case PNGMarker_iCCP:
                Options->iCCPExists = true;
                PNG_Read_ICCP(Options, BitB, ChunkSize);
                break;
            case PNGMarker_pCAL:
                Options->pCALExists = true;
                PNG_Read_PCAL(Options, BitB, ChunkSize);
                break;
            case PNGMarker_sPLT:
                Options->sPLTExists = true;
                PNG_Read_SPLT(Options, BitB, ChunkSize);
                break;
            case PNGMarker_acTL:
                Options->acTLExists = true;
                PNG_Read_ACTL(Options, BitB, ChunkSize);
                break;
            case PNGMarker_fcTL:
                Options->fcTLExists = true;
                PNG_Read_FCTL(Options, BitB, ChunkSize);
                break;
            default:
                Log(Severity_WARNING, PlatformIO_FunctionName, UTF8String("Unknown ChunkID = '0x%X', Skipping"), ChunkID);
                break;
        }
        return EXIT_SUCCESS;
    }
    
    bool VerifyChunkCRC(BitBuffer *BitB, uint32_t ChunkSize) {
        AssertIO(BitB != NULL);
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
        uint32_t ChunkCRC = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        uint32_t ContentCRC = BitBuffer_CalculateCRC(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, BitBuffer_GetPosition(BitB) - 104, ChunkSize, 0xFFFFFFFF, CRCPolynomial_IEEE802_3);
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
    
    void PNGDecodeNonFilter(PNGOptions *Options, uint8_t ***InflatedData, uint8_t ***DeFilteredData, size_t Line) {
        AssertIO(Options != NULL);
        AssertIO(InflatedData != NULL);
        AssertIO(DeFilteredData != NULL);
        for (uint8_t StereoView = 0; StereoView < Options->sTER->StereoType; StereoView++) {
            for (uint32_t Width = 0; Width < Options->iHDR->Width; Width++) {
                for (size_t Byte = 1; Byte < Bits2Bytes(Options->iHDR->BitDepth, RoundingType_Down); Byte++) {
                    DeFilteredData[StereoView][Width][Byte - 1] = InflatedData[StereoView][Width][Byte]; // Remove filter indicating byte
                }
            }
        }
    }
    
    void PNGDecodeSubFilter(PNGOptions *Options, uint8_t ***InflatedData, uint8_t ***DeFilteredData, size_t Line) {
        AssertIO(Options != NULL);
        AssertIO(InflatedData != NULL);
        AssertIO(DeFilteredData != NULL);
        for (uint8_t StereoView = 0; StereoView < Options->sTER->StereoType; StereoView++) {
            for (uint32_t Width = 0; Width < Options->iHDR->Width; Width++) {
                for (size_t Byte = 1; Byte < Bits2Bytes(Options->iHDR->BitDepth, RoundingType_Down); Byte++) {
                    DeFilteredData[StereoView][Line][Byte - 1] = (InflatedData[StereoView][Line][Byte] + InflatedData[StereoView][Line][Byte+1]) & 0xFF; // Byte+1 is wrong, needs to be + bitdepth
                }
            }
        }
    }
    
    void PNGDecodeUpFilter(PNGOptions *Options, uint8_t ***InflatedData, uint8_t ***DeFilteredData, size_t Line) {
        AssertIO(Options != NULL);
        AssertIO(InflatedData != NULL);
        AssertIO(DeFilteredData != NULL);
        for (uint8_t StereoView = 0; StereoView < Options->sTER->StereoType; StereoView++) {
            for (uint32_t Width = 0; Width < Options->iHDR->Width; Width++) {
                for (size_t Byte = 1; Byte < Bits2Bytes(Options->iHDR->BitDepth, RoundingType_Down); Byte++) {
                    DeFilteredData[StereoView][Line][Byte - 1] = InflatedData[StereoView][Line][Byte] + InflatedData[StereoView][Line - 1][Byte] & 0xFF;
                }
            }
        }
    }
    
    void PNGDecodeAverageFilter(PNGOptions *Options, uint8_t ***InflatedData, uint8_t ***DeFilteredData, size_t Line) {
        AssertIO(Options != NULL);
        AssertIO(InflatedData != NULL);
        AssertIO(DeFilteredData != NULL);
        uint8_t PixelSize = Bits2Bytes(Options->iHDR->BitDepth, RoundingType_Down);
        for (uint8_t StereoView = 0; StereoView < Options->sTER->StereoType; StereoView++) {
            for (uint32_t Width = 0; Width < Options->iHDR->Width; Width++) {
                for (size_t Byte = 1; Byte < Bits2Bytes(Options->iHDR->BitDepth, RoundingType_Down); Byte++) {
                    uint8_t Average = floor((InflatedData[StereoView][Line][Byte - (PixelSize)] + InflatedData[StereoView][Line - 1][Byte]) / 2);
                    DeFilteredData[StereoView][Line][Byte - 1] = InflatedData[StereoView][Line][Byte] + Average;
                }
            }
        }
    }
    
    void PNGDecodePaethFilter(PNGOptions *Options, uint8_t ***InflatedData, uint8_t ***DeFilteredData, size_t Line) {
        AssertIO(Options != NULL);
        AssertIO(InflatedData != NULL);
        AssertIO(DeFilteredData != NULL);
        // Filtering is applied to bytes, not pixels
        uint8_t PixelSize = Bits2Bytes(Options->iHDR->BitDepth, RoundingType_Down);
        for (uint8_t StereoView = 0; StereoView < Options->sTER->StereoType; StereoView++) {
            for (uint32_t Width = 0; Width < Options->iHDR->Width; Width++) {
                for (size_t Byte = 1; Byte < Bits2Bytes(Options->iHDR->BitDepth, RoundingType_Down); Byte++) {
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
    
    void PNGDecodeFilteredImage(PNGOptions *Options, uint8_t ***InflatedBuffer) {
        AssertIO(Options != NULL);
        AssertIO(InflatedBuffer != NULL);
        
        uint8_t ***DeFilteredData = calloc((Options->iHDR->Height * Options->iHDR->Width), Bits2Bytes(Options->iHDR->BitDepth, RoundingType_Down));
        
        for (uint8_t StereoView = 0; StereoView < ImageChannelMap_GetNumViews(Options->Map); StereoView++) {
            for (size_t Line = 0; Line < Options->iHDR->Height; Line++) {
                PNGFilterTypes FilterType = ExtractLineFilterType(*DeFilteredData[Line]);
                switch (FilterType) {
                    case PNGFilter_Unfiltered:
                        PNGDecodeNonFilter(Options, InflatedBuffer, DeFilteredData, Line);
                        break;
                    case PNGFilter_Sub:
                        PNGDecodeSubFilter(Options, InflatedBuffer, DeFilteredData, Line);
                        break;
                    case PNGFilter_Up:
                        PNGDecodeUpFilter(Options, InflatedBuffer, DeFilteredData, Line);
                        break;
                    case PNGFilter_Average:
                        PNGDecodeAverageFilter(Options, InflatedBuffer, DeFilteredData, Line);
                        break;
                    case PNGFilter_Paeth:
                        PNGDecodePaethFilter(Options, InflatedBuffer, DeFilteredData, Line);
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
    
    void DecodeDEFLATEBlock(PNGOptions *Options, BitBuffer *DEFLATEBlock, BitBuffer *DecodedBlock) { // ByteOrder_Right2Left
        AssertIO(Options != NULL);
        AssertIO(DEFLATEBlock != NULL);
        AssertIO(DecodedBlock != NULL);
        // Huffman codes are written MSBit first, everything else is writen LSBit first
        
        // DEFLATE Block header:
        bool     BlockIsFinal = BitBuffer_ReadBits(DEFLATEBlock, ByteOrder_Left2Right, BitOrder_Right2Left, 1); // BFINAL
        uint8_t  BlockType    = BitBuffer_ReadBits(DEFLATEBlock, ByteOrder_Left2Right, BitOrder_Right2Left, 2); // BTYPE
        
        if (BlockType == DynamicHuffmanBlock) {
            
        } else if (BlockType == FixedHuffmanBlock) {
            
        } else if (BlockType == UncompressedBlock) {
            
        }
    }
    
    void DecodeIFDATChunk(PNGOptions *Options, BitBuffer *DAT2Decode, BitBuffer *DecodedDAT, uint64_t DATSize) { // Decodes both fDAT and IDAT chunks
                                                                                                                 // well lets go ahead and allocate a DAT block the size of DATSize
        AssertIO(Options != NULL);
        AssertIO(DAT2Decode != NULL);
        AssertIO(DecodedDAT != NULL);
        BitBuffer_Init(DATSize);
        // Now we need to go ahead and parse the ZLIB Header.
        // ok so how do we do that? I wrote some notes on the Zlib header last night...
        
        /* Compression Method and Flags byte */
        uint8_t CompressionMethod = BitBuffer_ReadBits(DAT2Decode, ByteOrder_Left2Right, BitOrder_Right2Left, 4); // 8
        uint8_t CompressionInfo   = BitBuffer_ReadBits(DAT2Decode, ByteOrder_Left2Right, BitOrder_Right2Left, 4); // 7
        /* Compression Method and Flags byte */
        
        /* FlagByte */
        uint8_t FCHECK            = BitBuffer_ReadBits(DAT2Decode, ByteOrder_Left2Right, BitOrder_Right2Left, 5); // 1E
        bool    FDICTPresent      = BitBuffer_ReadBits(DAT2Decode, ByteOrder_Left2Right, BitOrder_Right2Left, 1); // 0
        uint8_t FLEVEL            = BitBuffer_ReadBits(DAT2Decode, ByteOrder_Left2Right, BitOrder_Right2Left, 2); // 1
        /* FlagByte */
        
        if (FDICTPresent) {
            // Read TableID which is 4 bytes
            uint32_t DICTID       = BitBuffer_ReadBits(DAT2Decode, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        }
        
        // Start reading the DEFLATE block?
        
    }
    
    void DecodePNGData(PNGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        // read the iDAT/fDAT chunk header, then do the other stuff.
        while (BitBuffer_GetSize(BitB) > 0) { // 12 is the start of IEND
            uint32_t ChunkSize   = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
            uint32_t ChunkID     = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
            
            
            if (ChunkID == PNGMarker_acTL) {
                PNG_Read_ACTL(Options, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_bKGD) {
                PNG_Read_BKGD(Options, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_cHRM) {
                PNG_Read_CHRM(Options, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_fcTL) {
                PNG_Read_FCTL(Options, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_gAMA) {
                PNG_Read_GAMA(Options, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_hIST) {
                PNG_Read_HIST(Options, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_iCCP) {
                PNG_Read_OFFS(Options, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_iDAT || ChunkID == PNGMarker_fDAT) {
                DecodeIFDATChunk(Options, BitB, NULL, ChunkSize);
            } else if (ChunkID == PNGMarker_iHDR) {
                PNG_Read_IHDR(Options, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_iTXt) {
                PNG_Read_ITXT(Options, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_oFFs) {
                PNG_Read_OFFS(Options, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_pCAL) {
                PNG_Read_PCAL(Options, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_pHYs) {
                PNG_Read_PHYS(Options, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_PLTE) {
                PNG_Read_PLTE(Options, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_sBIT) {
                PNG_Read_SBIT(Options, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_sRGB) {
                PNG_Read_SRGB(Options, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_sTER) {
                PNG_Read_STER(Options, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_tEXt) {
                PNG_Read_TEXT(Options, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_zTXt) {
                PNG_Read_ZTXT(Options, BitB, ChunkSize);
            }
        }
    }
    
    // Let's do this library right, by adding attach and delete functions for the various chunks, and let's also have a fancy function that applies color profiles to the pixels.
    // that's kinda a lot of work tho...
    uint16_t ***DecodePNGImage(PNGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        return NULL;
    }
    
    BitBuffer *DecodeAdam7(PNGOptions *Options, BitBuffer *BitB) { // Returns the interlaced image
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        /*
         Ok, so to interlace an image, we need to loop over every pixel in an 8x8 block?
         */
        BitBuffer *ProgressiveImage = NULL;
        return ProgressiveImage;
    }
    
    /* FIXME: Old code below */
    void PNG_DAT_Decode(PNGOptions *Options, BitBuffer *BitB, ImageContainer *Image) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        AssertIO(Image != NULL);
        
        uint8_t     ****ImageArrayBytes        = (uint8_t****) ImageContainer_GetArray(Image);
        bool     IsFinalBlock                  = false;
        do {
            IsFinalBlock                       = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 1); // 0
            Flate_BlockTypes BlockType         = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 2); // 0b00 = 0
            if (BlockType == BlockType_Literal) {
                BitBuffer_Align(BitB, 1); // Skip the remaining 5 bits
                uint16_t Bytes2Copy    = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 16); // 0x4F42 = 20,290
                uint16_t Bytes2CopyXOR = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 16) ^ 0xFFFF; // 0xB0BD = 0x4F42
                AssertIO(Bytes2Copy == Bytes2CopyXOR);
                for (uint16_t Byte = 0ULL; Byte < Bytes2Copy; Byte++) {
                    ImageArrayBytes[0][0][0][Byte] = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 8);
                }
            } else if (BlockType == BlockType_Fixed) {
                //Tree = HuffmanTree_Init();
            } else if (BlockType == BlockType_Dynamic) {
                //Tree = HuffmanTree_Init();
            }
            //PNG_Flate_Decode(Options, BitB); // Actually read the data
        } while (IsFinalBlock == false);
    }
    /* Old code above */
    
    void PNG_Filter_Sub(ImageContainer *Image) {
        AssertIO(Image != NULL);
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
    }
    
    void PNG_Filter_Up(ImageContainer *Image) {
        AssertIO(Image != NULL);
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
    }
    
    void PNG_Filter_Average(ImageContainer *Image) {
        AssertIO(Image != NULL);
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
    }
    
    void PNG_Filter_Paeth(ImageContainer *Image) {
        AssertIO(Image != NULL);
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
    }
    
    // These filters need to be operatable on every line in any order, so we need a main PNG_Defilter function.
    // That function then needs to read the first byte of each line to see what filter is used
    // then OVIA each line.
    // ALSO keep in mind concurrency.
    
    void PNG_Defilter(ImageContainer *Image) {
        AssertIO(Image != NULL);
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
    }
    
    void PNG_DAT_Parse(PNGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        bool     Is3D           = Options->sTER->StereoType > 0 ? Yes : No;
        uint8_t  BitDepth       = Options->iHDR->BitDepth;
        uint8_t  ColorType      = Options->iHDR->ColorType;
        uint8_t  NumChannels    = PNG_GetNumChannels(ColorType);
        uint64_t Width          = Options->iHDR->Width;
        uint64_t Height         = Options->iHDR->Height;
        ImageContainer *Decoded = NULL;
        MediaIO_ImageMask Mask  = 0;
        FlateOptions *Flate = FlateOptions_Init();
        Flate_ReadZlibHeader(Flate, BitB);
        
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
        AssertIO(BitDepth <= 16);
        if (BitDepth <= 8) {
            Decoded = ImageContainer_Init(ImageType_Integer8, Mask, Width, Height);
        } else if (BitDepth <= 16) {
            Decoded = ImageContainer_Init(ImageType_Integer16, Mask, Width, Height);
        }
        
        //PNG_DAT_Decode(Options, BitB, Decoded);
        Flate_ReadDeflateBlock(Options, BitB);
    }
    
    void PNG_Adam7_Deinterlace(ImageContainer *Image) {
        AssertIO(Image != NULL);
    }
    
    void PNG_IHDR_Parse(PNGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->iHDR->Width          = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->iHDR->Height         = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->iHDR->BitDepth       = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        Options->iHDR->ColorType      = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        AssertIO(Options->iHDR->ColorType != 1);
        AssertIO(Options->iHDR->ColorType != 5);
        AssertIO(Options->iHDR->ColorType < 7);
        Options->iHDR->Compression    = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        Options->iHDR->FilterMethod   = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        Options->iHDR->IsInterlaced   = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
    }
    
    void PNG_PLTE_Parse(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        uint8_t BitDepth = Options->iHDR->BitDepth;
        if (BitDepth <= 8) {
            uint8_t ColorType = Options->iHDR->ColorType;
            if (ColorType == PNGColor_Palette || ColorType == PNGColor_RGB) {
                //PNG_PLTE_Init(Ovia);
                for (uint32_t Entry = 0UL; Entry < ChunkSize / 3; Entry++) {
                    Options->PLTE->Palette[0] = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 8);
                    Options->PLTE->Palette[1] = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 8);
                    Options->PLTE->Palette[2] = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 8);
                }
            } else if (ColorType == PNGColor_RGBAlpha) {
                for (uint32_t Entry = 0UL; Entry < ChunkSize / 3; Entry++) {
                    Options->PLTE->Palette[0] = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 8);
                    Options->PLTE->Palette[1] = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 8);
                    Options->PLTE->Palette[2] = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 8);
                    Options->PLTE->Palette[3] = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 8);
                }
            }
        }
    }
    
    void PNG_TRNS_Parse(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) { // Transparency
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        uint32_t NumEntries   = ChunkSize % 3;
        uint16_t **Entries    = NULL;
        if (Options->iHDR->ColorType == PNGColor_RGB) {
            Entries = calloc(3, Bits2Bytes(Options->iHDR->BitDepth, RoundingType_Up) * sizeof(uint16_t));
        } else if (Options->iHDR->ColorType == PNGColor_RGBAlpha) {
            Entries = calloc(4, Bits2Bytes(Options->iHDR->BitDepth, RoundingType_Up) * sizeof(uint16_t));
        } else if (Options->iHDR->ColorType == PNGColor_Gray) {
            Entries = calloc(1, Bits2Bytes(Options->iHDR->BitDepth, RoundingType_Up) * sizeof(uint16_t));
        } else if (Options->iHDR->ColorType == PNGColor_GrayAlpha) {
            Entries = calloc(2, Bits2Bytes(Options->iHDR->BitDepth, RoundingType_Up) * sizeof(uint16_t));
        }
        AssertIO(Entries != NULL);
        for (uint8_t Color = 0; Color < PNG_GetNumChannels(Options->iHDR->ColorType); Color++) {
            Entries[Color]    = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, Bits2Bytes(Options->iHDR->BitDepth, RoundingType_Up));
        }
        //Ovia->tRNS->Palette = Entries;
    }
    
    void PNG_BKGD_Parse(PNGOptions *Options, BitBuffer *BitB) { // Background
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        for (uint8_t Entry = 0; Entry < PNG_GetNumChannels(Options->iHDR->ColorType); Entry++) {
            Options->bkGD->BackgroundPaletteEntry[Entry] = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, Options->iHDR->BitDepth);
        }
    }
    
    void PNG_CHRM_Parse(PNGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->cHRM->WhitePointX = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->cHRM->WhitePointY = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->cHRM->RedX        = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->cHRM->RedY        = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->cHRM->GreenX      = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->cHRM->GreenY      = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->cHRM->BlueX       = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->cHRM->BlueY       = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
    }
    
    void PNG_GAMA_Parse(PNGOptions *Options, BitBuffer *BitB) { // Gamma
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->gAMA->Gamma = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
    }
    
    void PNG_OFFS_Parse(PNGOptions *Options, BitBuffer *BitB) { // Image Offset
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->oFFs->XOffset       = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->oFFs->YOffset       = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->oFFs->UnitSpecifier = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
    }
    
    void PNG_PHYS_Parse(PNGOptions *Options, BitBuffer *BitB) { // Aspect ratio, Physical pixel size
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->pHYs->PixelsPerUnitXAxis = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->pHYs->PixelsPerUnitYAxis = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->pHYs->UnitSpecifier      = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
    }
    
    void PNG_SCAL_Parse(PNGOptions *Options, BitBuffer *BitB) { // Physical Scale
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->sCAL->UnitSpecifier  = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8); // 1 = Meter, 2 = Radian
        
        uint32_t WidthStringSize  = BitBuffer_GetUTF8StringSize(BitB);
        uint32_t HeightStringSize = BitBuffer_GetUTF8StringSize(BitB);
        
        UTF8 *WidthString         = BitBuffer_ReadUTF8(BitB, WidthStringSize);
        UTF8 *HeightString        = BitBuffer_ReadUTF8(BitB, HeightStringSize);
        
        Options->sCAL->PixelWidth     = UTF8_String2Decimal(Base_Decimal | Base_Radix10, WidthString);
        Options->sCAL->PixelHeight    = UTF8_String2Decimal(Base_Decimal | Base_Radix10, HeightString);
        
        free(WidthString);
        free(HeightString);
    }
    
    void PNG_PCAL_Parse(PNGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        uint8_t CalibrationSize              = BitBuffer_GetUTF8StringSize(BitB);
        Options->pCAL->CalibrationName           = BitBuffer_ReadUTF8(BitB, CalibrationSize);
        Options->pCAL->OriginalZero              = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->pCAL->OriginalMax               = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->pCAL->EquationType              = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        Options->pCAL->NumParams                 = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        uint8_t UnitNameSize                 = BitBuffer_GetUTF8StringSize(BitB);
        Options->pCAL->UnitName                  = BitBuffer_ReadUTF8(BitB, UnitNameSize);
        BitBuffer_Seek(BitB, 8); // NULL seperator
        Options->pCAL->Parameters                = calloc(Options->pCAL->NumParams, sizeof(double));
        if (Options->pCAL->Parameters != NULL) {
            for (uint8_t Param = 0; Param < Options->pCAL->NumParams; Param++) {
                uint8_t ParameterSize        = BitBuffer_GetUTF8StringSize(BitB);
                UTF8   *ParameterString      = BitBuffer_ReadUTF8(BitB, ParameterSize);
                Options->pCAL->Parameters[Param] = UTF8_String2Decimal(Base_Decimal | Base_Radix10, ParameterString);
            }
        }
    }
    
    void PNG_SBIT_Parse(PNGOptions *Options, BitBuffer *BitB) { // Significant bits per sample
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        PNGColorTypes ColorType  = Options->iHDR->ColorType;
        if (ColorType == PNGColor_Gray) {
            Options->sBIT->Grayscale = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        } else if (ColorType == PNGColor_RGB || ColorType == PNGColor_Palette) {
            Options->sBIT->Red       = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
            Options->sBIT->Green     = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
            Options->sBIT->Blue      = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        } else if (ColorType == PNGColor_GrayAlpha) {
            Options->sBIT->Grayscale = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
            Options->sBIT->Alpha     = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        } else if (ColorType == PNGColor_RGBAlpha) {
            Options->sBIT->Red       = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
            Options->sBIT->Green     = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
            Options->sBIT->Blue      = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
            Options->sBIT->Alpha     = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        }
    }
    
    void PNG_SRGB_Parse(PNGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->sRGB->RenderingIntent = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
    }
    
    void PNG_STER_Parse(PNGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->sTER->StereoType = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        // No matter what StereoType is used, both images are arranged side by side, and the left edge is aligned on a boundary of the 8th column in case interlacing is used.
        // The two sub images must have the same dimensions after padding is removed.
        // CROSS_FUSE_LAYOUT = 0, DIVERGING_FUSE_LAYOUT = 1
        // When CROSS_FUSE_LAYOUT, the right image is on the left, and the left image on the right.
        // When DIVERGING_FUSE_LAYOUT, The left eye view is on the left, and the right eye view is on the right.
        // The Left eye view is always in index 0 of the pixel array, so we need to swap this if nessicary.
        // So, make a function that strips padding from the stream, then make a function that OVIAs the sTER image the way it should be.
        // The standard recommends we use CROSS_FUSE_LAYOUT, so I'll probably end up using that.
        // So, let's say we have 2 1080p images, and we want to make a single Stereoscopic Options from them.
        //
    }
    
    void PNG_TEXT_Parse(PNGOptions *Options, BitBuffer *BitB) { // Uncompressed, ASCII tEXt
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        for (uint32_t TextChunk = 0UL; TextChunk < Options->NumTextChunks; TextChunk++) {
            if (Options->Text[TextChunk].TextType == PNGTextType_tEXt) { // ASCII aka Latin-1
                
            } else if (Options->Text[TextChunk].TextType == PNGTextType_iTXt) { // Unicode, UTF-8
                
            } else if (Options->Text[TextChunk].TextType == PNGTextType_zTXt) { // Compressed
                
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
    }
    
    void PNG_TIME_Parse(PNGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->tIMe->Year                     = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 16);
        Options->tIMe->Month                    = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        Options->tIMe->Day                      = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        Options->tIMe->Hour                     = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        Options->tIMe->Minute                   = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        Options->tIMe->Second                   = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
    }
    
    /* APNG */
    void PNG_ACTL_Parse(PNGOptions *Options, BitBuffer *BitB) { // Animation control, part of APNG
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->acTL->NumFrames             = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->acTL->TimesToLoop           = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32); // If 0, loop forever.
    }
    
    void PNG_FCTL_Parse(PNGOptions *Options, BitBuffer *BitB) { // Frame Control, part of APNG
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->fcTL->FrameNum              = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->fcTL->Width                 = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->fcTL->Height                = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->fcTL->XOffset               = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->fcTL->YOffset               = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->fcTL->FrameDelayNumerator   = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 16);
        Options->fcTL->FrameDelayDenominator = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 16);
        Options->fcTL->DisposeMethod         = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        Options->fcTL->BlendMethod           = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
    }
    /* End APNG */
    
    void PNG_HIST_Parse(PNGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->hIST->NumColors = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        for (uint32_t Color = 0; Color < Options->hIST->NumColors; Color++) {
            Options->hIST->Histogram[Color] = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, Options->iHDR->BitDepth);
        }
    }
    
    void PNG_ICCP_Parse(PNGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        uint8_t ProfileNameSize    = BitBuffer_GetUTF8StringSize(BitB);
        Options->iCCP->ProfileName     = BitBuffer_ReadUTF8(BitB, ProfileNameSize);
        Options->iCCP->CompressionType = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        // Decompress the data with Zlib
        ProfileNameSize = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
    }
    
    void PNG_SPLT_Parse(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        PNGColorTypes ColorType                 = Options->iHDR->ColorType;
        uint8_t BitDepthInBytes                 = Bits2Bytes(Options->iHDR->BitDepth, RoundingType_Up);
        uint8_t PaletteNameSize                 = BitBuffer_GetUTF8StringSize(BitB);
        Options->sPLT->NumEntries                   = ChunkSize - (PaletteNameSize + TextIO_NULLTerminator) + 1;
        if (ColorType == PNGColor_RGBAlpha || ColorType == PNGColor_GrayAlpha) {
            Options->sPLT->NumEntries              /= 4;
        } else {
            Options->sPLT->NumEntries              /= 3;
        }
        Options->sPLT[Options->NumSPLTChunks - 1].PaletteName = BitBuffer_ReadUTF8(BitB, PaletteNameSize);
        for (uint32_t Entry = 0; Entry < Options->sPLT->NumEntries; Entry++) {
            Options->sPLT->Palette[Entry].Red       = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, Bytes2Bits(BitDepthInBytes));
            Options->sPLT->Palette[Entry].Green     = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, Bytes2Bits(BitDepthInBytes));
            Options->sPLT->Palette[Entry].Blue      = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, Bytes2Bits(BitDepthInBytes));
            if (ColorType == PNGColor_RGBAlpha || ColorType == PNGColor_GrayAlpha) {
                Options->sPLT->Palette[Entry].Alpha = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, Bytes2Bits(BitDepthInBytes));
            }
        }
    }
    
    void PNG_Parse(PNGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        while (BitBuffer_GetSize(BitB) > 0) {
            uint32_t ChunkSize   = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
            uint32_t ChunkID     = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
            
            if (ChunkID == PNGMarker_acTL) {
                PNG_ACTL_Parse(Options, BitB);
            } else if (ChunkID == PNGMarker_bKGD) {
                PNG_BKGD_Parse(Options, BitB);
            } else if (ChunkID == PNGMarker_cHRM) {
                PNG_CHRM_Parse(Options, BitB);
            } else if (ChunkID == PNGMarker_fcTL) {
                PNG_FCTL_Parse(Options, BitB);
            } else if (ChunkID == PNGMarker_gAMA) {
                PNG_GAMA_Parse(Options, BitB);
            } else if (ChunkID == PNGMarker_hIST) {
                PNG_HIST_Parse(Options, BitB);
            } else if (ChunkID == PNGMarker_iCCP) {
                PNG_OFFS_Parse(Options, BitB);
            } else if (ChunkID == PNGMarker_iDAT || ChunkID == PNGMarker_fDAT) {
                PNG_DAT_Parse(Options, BitB);
            } else if (ChunkID == PNGMarker_iHDR) {
                PNG_IHDR_Parse(Options, BitB);
            } else if (ChunkID == PNGMarker_oFFs) {
                PNG_OFFS_Parse(Options, BitB);
            } else if (ChunkID == PNGMarker_pCAL) {
                PNG_PCAL_Parse(Options, BitB);
            } else if (ChunkID == PNGMarker_pHYs) {
                PNG_PHYS_Parse(Options, BitB);
            } else if (ChunkID == PNGMarker_PLTE) {
                PNG_PLTE_Parse(Options, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_sBIT) {
                PNG_SBIT_Parse(Options, BitB);
            } else if (ChunkID == PNGMarker_sRGB) {
                PNG_SRGB_Parse(Options, BitB);
            } else if (ChunkID == PNGMarker_sTER) {
                PNG_STER_Parse(Options, BitB);
            } else if (ChunkID == PNGMarker_tEXt) {
                Options->NumTextChunks += 1;
                Options->Text[Options->NumTextChunks - 1].TextType = PNGMarker_tEXt;
                PNG_TEXT_Parse(Options, BitB);
            } else if (ChunkID == PNGMarker_zTXt) {
                Options->NumTextChunks += 1;
                Options->Text[Options->NumTextChunks - 1].TextType = PNGTextType_zTXt;
                PNG_TEXT_Parse(Options, BitB);
            } else if (ChunkID == PNGMarker_iTXt) {
                Options->NumTextChunks += 1;
                Options->Text[Options->NumTextChunks - 1].TextType = PNGTextType_iTXt;
                PNG_TEXT_Parse(Options, BitB);
            } else if (ChunkID == PNGMarker_sCAL) {
                PNG_SCAL_Parse(Options, BitB);
            } else if (ChunkID == PNGMarker_tIME) {
                PNG_TIME_Parse(Options, BitB);
            } else if (ChunkID == PNGMarker_tRNS) {
                PNG_TRNS_Parse(Options, BitB, ChunkSize);
            } else if (ChunkID == PNGMarker_sPLT) {
                PNG_SPLT_Parse(Options, BitB, ChunkSize);
            }
        }
    }
    
    void PNG_Extract(PNGOptions *Options, BitBuffer *BitB, void *Container) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        AssertIO(Container != NULL);
    }
    
    // Hmm, for parsing chunks I should have basically a Options muxer that pops out bit buffers with the data for each chunk...
    // So, we need to
    
    void ParseIHDR(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        /* When checking the CRC, you need to skip over the ChunkSize field, but include the ChunkID */
        uint32_t GeneratedCRC         = BitBuffer_CalculateCRC(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 0, 13, 0x00000000, CRCPolynomial_IEEE802_3);
        Options->iHDR->Width          = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->iHDR->Height         = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->iHDR->BitDepth       = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        Options->iHDR->ColorType      = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        AssertIO(Options->iHDR->ColorType != 1);
        AssertIO(Options->iHDR->ColorType != 5);
        AssertIO(Options->iHDR->ColorType < 7);
        Options->iHDR->Compression    = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left,8);
        Options->iHDR->FilterMethod   = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left,8);
        Options->iHDR->IsInterlaced   = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left,8);
        
        uint32_t CRC              = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left,32);
        AssertIO(GeneratedCRC != CRC);
        
        //VerifyCRC(Options->iHDR, ChunkSize, 1, 1, CRC);
    }
    
    void ParsePLTE(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) { // Palette
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        if (Options->iHDR->ColorType == PNGColor_Palette || Options->iHDR->ColorType == PNGColor_RGB) {
            Options->PLTE->Palette = calloc(1, (3 * Options->PLTE->NumEntries) * Bits2Bytes(Options->iHDR->BitDepth, Yes));
        } else if (Options->iHDR->ColorType == PNGColor_RGBAlpha) {
            Options->PLTE->Palette = calloc(1, (4 * Options->PLTE->NumEntries) * Bits2Bytes(Options->iHDR->BitDepth, Yes));
        }
        
        
        for (uint8_t Channel = 0; Channel < PNG_GetNumChannels(Options->iHDR->ColorType); Channel++) {
            for (uint16_t PaletteEntry = 0; PaletteEntry < ChunkSize / 3; PaletteEntry++) {
                Options->PLTE->Palette[Channel][PaletteEntry] = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, Options->iHDR->BitDepth);
            }
        }
    }
    
    void ParseTRNS(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) { // Transparency
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->tRNS->NumEntries = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        uint16_t **Entries    = NULL;
        if (Options->iHDR->ColorType == PNGColor_RGB) {
            Entries = calloc(3, Bits2Bytes(Options->iHDR->BitDepth, true) * sizeof(uint16_t));
        } else if (Options->iHDR->ColorType == PNGColor_RGBAlpha) {
            Entries = calloc(4, Bits2Bytes(Options->iHDR->BitDepth, true) * sizeof(uint16_t));
        } else if (Options->iHDR->ColorType == PNGColor_Gray) {
            Entries = calloc(1, Bits2Bytes(Options->iHDR->BitDepth, true) * sizeof(uint16_t));
        } else if (Options->iHDR->ColorType == PNGColor_GrayAlpha) {
            Entries = calloc(2, Bits2Bytes(Options->iHDR->BitDepth, true) * sizeof(uint16_t));
        }
        AssertIO(Entries != NULL);
        for (uint8_t Color = 0; Color < PNG_GetNumChannels(Options->iHDR->ColorType); Color++) {
            Entries[Color]    = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, Bits2Bytes(Options->iHDR->BitDepth, true));
        }
        //Options->tRNS->Palette = Entries;
    }
    
    void ParseBKGD(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) { // Background
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        for (uint8_t Entry = 0; Entry < 3; Entry++) {
            Options->bkGD->BackgroundPaletteEntry[Entry] = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        }
    }
    
    void ParseCHRM(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) { // Chromaticities
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->cHRM->WhitePointX = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->cHRM->WhitePointY = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->cHRM->RedX        = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->cHRM->RedY        = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->cHRM->GreenX      = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->cHRM->GreenY      = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->cHRM->BlueX       = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->cHRM->BlueY       = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
    }
    
    void ParseGAMA(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) { // Gamma
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->gAMA->Gamma = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
    }
    
    void ParseOFFS(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) { // Image Offset
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->oFFs->XOffset       = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->oFFs->YOffset       = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->oFFs->UnitSpecifier = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
    }
    
    void ParsePHYS(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) { // Aspect ratio, Physical pixel size
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->pHYs->PixelsPerUnitXAxis = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->pHYs->PixelsPerUnitYAxis = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->pHYs->UnitSpecifier      = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
    }
    
    void ParseSCAL(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) { // Physical Scale
                                                                               // Ok, so we need to take the size of the chunk into account.
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->sCAL->UnitSpecifier = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8); // 1 = Meter, 2 = Radian
        
        uint32_t WidthStringSize  = 0;
        uint32_t HeightStringSize = 0;
        
        for (uint32_t Byte = 0UL; Byte < ChunkSize - 1; Byte++) { // Get the sizes for the field strings
            if (BitBuffer_PeekBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8) != '\0') {
                WidthStringSize += 1;
            }
        }
        HeightStringSize = ChunkSize - (WidthStringSize + 1);
        
        UTF8 *WidthString = calloc(WidthStringSize, sizeof(UTF8));
        UTF8 *HeightString = calloc(HeightStringSize, sizeof(UTF8));
        
        for (uint32_t WidthCodePoint = 0; WidthCodePoint < WidthStringSize; WidthCodePoint++) {
            WidthString[WidthCodePoint] = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        }
        BitBuffer_Seek(BitB, 8); // Skip the NULL seperator
        for (uint32_t HeightCodePoint = 0; HeightCodePoint < HeightStringSize; HeightCodePoint++) {
            HeightString[HeightCodePoint] = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        }
        
        Options->sCAL->PixelWidth  = UTF8_String2Decimal(Base_Integer | Base_Radix10, WidthString);
        Options->sCAL->PixelHeight = UTF8_String2Decimal(Base_Integer | Base_Radix10, HeightString);
    }
    
    void ParsePCAL(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->pCAL->CalibrationNameSize = BitBuffer_GetUTF8StringSize(BitB);
        Options->pCAL->CalibrationName     = BitBuffer_ReadUTF8(BitB, Options->pCAL->CalibrationNameSize);
    }
    
    void ParseSBIT(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) { // Significant bits per sample
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->sBIT->Red                   = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        Options->sBIT->Green                 = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        Options->sBIT->Blue                  = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
    }
    
    void ParseSRGB(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->sRGB->RenderingIntent       = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
    }
    
    void ParseSTER(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->sTER->StereoType            = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        
        // No matter what StereoType is used, both images are arranged side by side, and the left edge is aligned on a boundary of the 8th column in case interlacing is used.
        // The two sub images must have the same dimensions after padding is removed.
        // CROSS_FUSE_LAYOUT = 0, DIVERGING_FUSE_LAYOUT = 1
        // When CROSS_FUSE_LAYOUT, the right image is on the left, and the left image on the right.
        // When DIVERGING_FUSE_LAYOUT, The left eye view is on the left, and the right eye view is on the right.
        // The Left eye view is always in index 0 of the pixel array, so we need to swap this if nessicary.
        // So, make a function that strips padding from the stream, then make a function that decodes the sTER image the way it should be.
        // The standard recommends we use CROSS_FUSE_LAYOUT, so I'll probably end up using that.
        // So, let's say we have 2 1080p images, and we want to make a single Stereoscopic Options from them.
        //
    }
    
    void ParseTEXt(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) { // Uncompressed, ASCII tEXt
                                                                               // Read until you hit a NULL for the name string, then subtract the size of the previous string from the total length to get the number of bytes for the second string
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        uint8_t  KeywordSize = 0UL;
        uint8_t  CurrentByte = 0; // 1 is BULLshit
        
        do {
            CurrentByte  = BitBuffer_PeekBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
            KeywordSize += 1;
        } while (CurrentByte != 0);
        
        uint32_t CommentSize = ChunkSize - KeywordSize;
        
        Options->NumTextChunks  += 1;
        
        
        
        // first we need to get the size of the strings
        
        // for that we would theoretically use PeekBits, but PeekBits is limited to 64 bits read at once, max.
        // there can be up to 800 bits before we hit the NULL, so what do...
        
        // Well, we can use SeekBits to jump byte by byte but it's not optimal...
        
        
        
        // Ok, once we've gotten the size of the Keyword string
        
        
        // Then read the Comment string.
        
        // Now, how do we handle multiple text strings?
        // Why not just have a count of the text chunks?
        // Store a variable in PNGOptions called NumTextChunks, then store a type called Comment or something that stores both the comment type as a string and the actual comment...
        
        
        
        
    }
    
    void ParseZTXt(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) { // Compressed text
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
    }
    
    void ParseITXt(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) { // International Text
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
    }
    
    void ParseTIME(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->tIMe->Year                  = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 16);
        Options->tIMe->Month                 = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        Options->tIMe->Day                   = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        Options->tIMe->Hour                  = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        Options->tIMe->Minute                = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        Options->tIMe->Second                = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
    }
    
    /* APNG */
    void ParseACTL(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) { // Animation control, part of APNG
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->PNGIsAnimated               = true;
        Options->acTL->NumFrames             = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->acTL->TimesToLoop           = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32); // If 0, loop forever.
    }
    
    void ParseFCTL(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) { // Frame Control, part of APNG
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->fcTL->FrameNum              = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->fcTL->Width                 = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->fcTL->Height                = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->fcTL->XOffset               = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->fcTL->YOffset               = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 32);
        Options->fcTL->FrameDelayNumerator   = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 16);
        Options->fcTL->FrameDelayDenominator = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 16);
        Options->fcTL->DisposeMethod         = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
        Options->fcTL->BlendMethod           = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
    }
    /* End APNG */
    
    void ParseHIST(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
    }
    
    void ParseICCP(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        uint8_t ProfileNameSize = 0;
        ProfileNameSize = BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Right2Left, 8);
    }
    
    void ParseSPLT(PNGOptions *Options, BitBuffer *BitB, uint32_t ChunkSize) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
