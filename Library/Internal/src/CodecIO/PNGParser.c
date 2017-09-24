#include "../../include/libModernPNG.h"

#include "../../include/Private/Decode/libModernPNG_ReadChunks.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#define strcasecmp _stricmp
#endif
    
    // Hmm, for parsing chunks I should have basically a PNG muxer that pops out bit buffers with the data for each chunk...
    // So, we need to 
    
    void ParseIHDR(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) {
        Dec->iHDR->Width          = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
        Dec->iHDR->Height         = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
        Dec->iHDR->BitDepth       = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
        Dec->iHDR->ColorType      = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
        if (Dec->iHDR->ColorType == 1 || Dec->iHDR->ColorType == 5 || Dec->iHDR->ColorType >= 7) {
            Log(LOG_ALERT, "ModernPNG", "ParseiHDR", "Invalid color type: %d", Dec->iHDR->ColorType);
        }
        Dec->iHDR->Compression    = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
        Dec->iHDR->FilterMethod   = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
        Dec->iHDR->IsInterlaced   = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
        SkipBits(InputPNG, Bytes2Bits(ChunkSize - 13)); // incase the header is longer.
        uint32_t CRC              = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
        //VerifyCRC(Dec->iHDR, ChunkSize, 1, 1, CRC);
    }
    
    void ParsePLTE(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Palette
        if (Dec->iHDR->BitDepth > 8) { // INVALID
            Log(LOG_ALERT, "ModernPNG", "ParsePLTE", "Invalid bit depth %d and palette combination\n", Dec->iHDR->BitDepth);
            SkipBits(InputPNG, Bytes2Bits(ChunkSize));
        } else {
            
            if (Dec->iHDR->ColorType == PNG_PalettedRGB || Dec->iHDR->ColorType == PNG_RGB) {
                Dec->PLTE->Palette = calloc(3 * Dec->PLTE->NumEntries, 1);
            } else if (Dec->iHDR->ColorType == PNG_RGBA) {
                Dec->PLTE->Palette = calloc(4 * Dec->PLTE->NumEntries, 1);
            }
            
            
            for (uint8_t Channel = 0; Channel < ModernPNGChannelsPerColorType[Dec->iHDR->ColorType]; Channel++) {
                for (uint16_t PaletteEntry = 0; PaletteEntry < ChunkSize / 3; PaletteEntry++) {
                    Dec->PLTE->Palette[Channel][PaletteEntry] = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, Dec->iHDR->BitDepth);
                }
            }
        }
        Dec->PLTE->CRC = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
    }
    
    void ParseTRNS(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Transparency
        Dec->tRNS->NumEntries = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
        uint16_t **Entries = calloc(1, Dec->tRNS->NumEntries);
        for (uint8_t Color = 0; Color < ModernPNGChannelsPerColorType[Dec->iHDR->ColorType]; Color++) {
            Entries[Color] = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, Bits2Bytes(Dec->iHDR->BitDepth, true));
        }
        Dec->tRNS->Palette = Entries;
        Dec->tRNS->CRC = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
    }
    
    void ParseBKGD(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Background
        for (uint8_t Entry = 0; Entry < 3; Entry++) {
            Dec->bkGD->BackgroundPaletteEntry[Entry] = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
        }
        Dec->bkGD->CRC = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
    }
    
    void ParseCHRM(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Chromaticities
        Dec->cHRM->WhitePointX = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
        Dec->cHRM->WhitePointY = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
        Dec->cHRM->RedX        = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
        Dec->cHRM->RedY        = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
        Dec->cHRM->GreenX      = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
        Dec->cHRM->GreenY      = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
        Dec->cHRM->BlueX       = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
        Dec->cHRM->BlueY       = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
        Dec->cHRM->CRC         = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
    }
    
    void ParseGAMA(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Gamma
        Dec->gAMA->Gamma = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
        Dec->gAMA->CRC   = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
    }
    
    void ParseOFFS(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Image Offset
        Dec->oFFs->XOffset       = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
        Dec->oFFs->YOffset       = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
        Dec->oFFs->UnitSpecifier = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
        Dec->oFFs->CRC           = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
    }
    
    void ParsePHYS(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Aspect ratio, Physical pixel size
        Dec->pHYs->PixelsPerUnitXAxis = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
        Dec->pHYs->PixelsPerUnitYAxis = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
        Dec->pHYs->UnitSpecifier      = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
        Dec->pHYs->CRC                = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
    }
    
    void ParseSCAL(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Physical Scale
        // Ok, so we need to take the size of the chunk into account.
        Dec->sCAL->UnitSpecifier = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8); // 1 = Meter, 2 = Radian
        
        char Width[ChunkSize - 1]; // minus 1 for the UnitSpecifier
        char Height[ChunkSize - 1];
        
        for (uint32_t Byte = 0; Byte < ChunkSize - 1; Byte++) {
            if (PeekBits(BitIOMSByte, BitIOLSBit, InputPNG, 8) != 0x00) {
                Width[Byte]  = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
            } else {
                Height[Byte] = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
            }
        }
        
        Dec->sCAL->PixelWidth  = (float) atof(Width);
        Dec->sCAL->PixelHeight = (float) atof(Height);
        Dec->sCAL->CRC         = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
        
    }
    
    void ParsePCAL(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) {
        char CalibrationName[80];
        while (PeekBits(BitIOMSByte, BitIOLSBit, InputPNG, 8) != 0x0) {
            for (uint8_t Byte = 0; Byte < 80; Byte++) {
                CalibrationName[Byte] = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
            }
        }
        Dec->pCAL->CalibrationName     = CalibrationName;
        Dec->pCAL->CalibrationNameSize = strlen(Dec->pCAL->CalibrationName);
        
        Dec->pCAL->CRC                 = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
    }
    
    void ParseSBIT(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Significant bits per sample
        Dec->sBIT->Red                   = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
        Dec->sBIT->Green                 = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
        Dec->sBIT->Blue                  = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
        Dec->sBIT->CRC                   = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
    }
    
    void ParseSRGB(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) {
        Dec->sRGB->RenderingIntent       = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
        Dec->sRGB->CRC                   = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
    }
    
    void ParseSTER(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) {
        Dec->Is3D = true;
        Dec->sTER->StereoType            = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
        uint32_t CRC                     = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
        
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
    
    void ParseTEXt(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // tEXt
        uint32_t CRC = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
    }
    
    void ParseZTXt(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Compressed text
        uint32_t CRC = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
    }
    
    void ParseITXt(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // International Text
        
        uint32_t CRC = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
    }
    
    void ParseTIME(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) {
        Dec->tIMe->Year                  = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 16);
        Dec->tIMe->Month                 = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
        Dec->tIMe->Day                   = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
        Dec->tIMe->Hour                  = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
        Dec->tIMe->Minute                = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
        Dec->tIMe->Second                = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
        Dec->tIMe->CRC                   = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
    }
    
    /* APNG */
    void ParseACTL(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Animation control, part of APNG
        Dec->IsVideo = true;
        Dec->acTL->NumFrames             = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
        Dec->acTL->TimesToLoop           = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32); // If 0, loop forever.
        Dec->acTL->CRC                   = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
    }
    
    void ParseFCTL(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Frame Control, part of APNG
        Dec->fcTL->FrameNum              = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
        Dec->fcTL->Width                 = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
        Dec->fcTL->Height                = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
        Dec->fcTL->XOffset               = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
        Dec->fcTL->YOffset               = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
        Dec->fcTL->FrameDelayNumerator   = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 16);
        Dec->fcTL->FrameDelayDenominator = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 16);
        Dec->fcTL->DisposeMethod         = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
        Dec->fcTL->BlendMethod           = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
        uint32_t CRC                     = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
    }
    /* End APNG */
    
    void ParseIDAT(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // IDAT
                                                                         // DecodeINFLATE
        uint32_t CRC = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
    }
    
    void ParseHIST(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) {
        Dec->hIST->CRC = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
    }
    
    void ParseICCP(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) {
        uint8_t ProfileNameSize = 0;
        ProfileNameSize = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
        uint32_t CRC = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
    }
    
    void ParseSPLT(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) {
        uint32_t CRC = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
    }
    
    void ParseFDAT(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) {
        // DecodeDEFLATE
        uint32_t CRC = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
    }
    
    uint8_t ParsePNGMetadata(BitBuffer *InputPNG, DecodePNG *Dec) {
        uint64_t FileMagic    = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 64);
        
        if (FileMagic == PNGMagic) {
            char     *ChunkID        = calloc(1, 4);
            uint32_t ChunkSize       = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
            
            for (uint8_t ChunkIDSize = 0; ChunkIDSize < 4; ChunkIDSize++) {
                ChunkID[ChunkIDSize] = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
            }
            uint32_t ChunkCRC        = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
            
            if (strcasecmp(ChunkID, "iHDR") == 0) {        // iHDR
                ParseIHDR(Dec, InputPNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "PLTE") == 0) { // PLTE
                Dec->PLTEExists = true;
                ParsePLTE(Dec, InputPNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "bKGD") == 0) { // bKGD
                Dec->bkGDExists = true;
                ParseBKGD(Dec, InputPNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "cHRM") == 0) { // cHRM
                Dec->cHRMExists = true;
                ParseCHRM(Dec, InputPNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "gAMA") == 0) { // gAMA
                Dec->gAMAExists = true;
                ParseGAMA(Dec, InputPNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "oFFs") == 0) { // oFFs
                Dec->oFFsExists = true;
                ParseOFFS(Dec, InputPNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "pHYs") == 0) { // pHYs
                Dec->pHYsExists = true;
                ParsePHYS(Dec, InputPNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "sBIT") == 0) { // sBIT
                Dec->sBITExists = true;
                ParseSBIT(Dec, InputPNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "sCAL") == 0) { // sCAL
                Dec->sCALExists = true;
                ParseSCAL(Dec, InputPNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "sRGB") == 0) { // sRGB
                Dec->sRGBExists = true;
                ParseSRGB(Dec, InputPNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "sTER") == 0) { // sTER
                Dec->sTERExists = true;
                ParseSTER(Dec, InputPNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "tEXt") == 0) { // tEXt
                Dec->TextExists = true;
                ParseTEXt(Dec, InputPNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "zTXt") == 0) { // zTXt
                Dec->TextExists = true;
                ParseZTXt(Dec, InputPNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "iTXt") == 0) { // iTXt
                Dec->TextExists = true;
                ParseITXt(Dec, InputPNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "tIME") == 0) { // tIME
                Dec->tIMEExists = true;
                ParseTIME(Dec, InputPNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "tRNS") == 0) { // tRNS
                Dec->tRNSExists = true;
                ParseTRNS(Dec, InputPNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "hIST") == 0 && Dec->PLTEExists) { // hIST
                Dec->hISTExists = true;
                ParseHIST(Dec, InputPNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "iCCP") == 0) { // iCCP
                Dec->iCCPExists = true;
                ParseICCP(Dec, InputPNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "pCAL") == 0) { // pCAL
                Dec->pCALExists = true;
                ParsePCAL(Dec, InputPNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "sPLT") == 0) { // sPLT
                Dec->sPLTExists = true;
                ParseSPLT(Dec, InputPNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "acTL") == 0) { // acTL
                Dec->acTLExists = true;
                ParseACTL(Dec, InputPNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "fcTL") == 0) { // fcTL
                Dec->fcTLExists = true;
                ParseFCTL(Dec, InputPNG, ChunkSize);
            }
            free(ChunkID);
        } else {
            Log(LOG_CRIT, "ModernPNG", "ParsePNGMetadata", "File Magic 0x%X is not PNG, exiting\n", FileMagic);
            exit(EXIT_FAILURE);
        }
        
        return EXIT_SUCCESS;
    }
    
    uint32_t GetPNGWidth(DecodePNG *Dec) {
        return Dec->iHDR->Width;
    }
    
    uint32_t GetPNGHeight(DecodePNG *Dec) {
        return Dec->iHDR->Height;
    }
    
    uint8_t GetPNGBitDepth(DecodePNG *Dec) {
        return Dec->iHDR->BitDepth;
    }
    
    uint8_t GetPNGColorType(DecodePNG *Dec) {
        return Dec->iHDR->ColorType;
    }
    
    bool GetPNGInterlaceStatus(DecodePNG *Dec) {
        return Dec->iHDR->IsInterlaced;
    }
    
    bool IsPNGStereoscopic(DecodePNG *Dec) {
        return Dec->Is3D;
    }
    
    uint32_t GetPNGWhitepointX(DecodePNG *Dec) {
        return Dec->cHRM->WhitePointX;
    }
    
    uint32_t GetPNGWhitepointY(DecodePNG *Dec) {
        return Dec->cHRM->WhitePointY;
    }
    
    uint32_t GetPNGGamma(DecodePNG *Dec) {
        return Dec->gAMA->Gamma;
    }
    
    const char *GetPNGColorProfileName(DecodePNG *Dec) {
        return Dec->iCCP->ProfileName;
    }
    
    uint8_t *GetColorProfile(DecodePNG *Dec) {
        //return DecompressDEFLATE(Dec->iCCP->CompressedICCPProfile);
        return NULL;
    }
    
#ifdef __cplusplus
}
#endif
