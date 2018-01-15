#include <stdlib.h>
#include <tgmath.h>

#include "../../include/libModernPNG.h"
#include "../../include/Private/libModernPNG_Types.h"
#include "../../include/Private/Decode/libModernPNG_ReadChunks.h"
#include "../../include/Private/Common/libModernPNG_EntropyCoders.h"

#include "../../../Dependencies/libPCM/Dependencies/BitIO/libBitIO/include/BitIOLog.h"
#include "../../../Dependencies/libPCM/Dependencies/BitIO/libBitIO/include/BitIOMath.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#define strcasecmp _stricmp
#endif
    
    // Hmm, for parsing chunks I should have basically a PNG muxer that pops out bit buffers with the data for each chunk...
    // So, we need to 
    
    void ParseIHDR(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) {
        /* When checking the CRC, you need to skip over the ChunkSize field, but include the ChunkID */
        uint32_t GeneratedCRC     = GenerateCRC32(InputPNG, ChunkSize);
        Dec->iHDR->Width          = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
        Dec->iHDR->Height         = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
        Dec->iHDR->BitDepth       = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
        Dec->iHDR->ColorType      = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
        if (Dec->iHDR->ColorType == 1 || Dec->iHDR->ColorType == 5 || Dec->iHDR->ColorType >= 7) {
            BitIOLog(BitIOLog_ERROR, libModernPNGLibraryName, __func__, u8"Invalid color type: %d", Dec->iHDR->ColorType);
        }
        Dec->iHDR->Compression    = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
        Dec->iHDR->FilterMethod   = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
        Dec->iHDR->IsInterlaced   = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
    	
        uint32_t CRC              = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
        if (GeneratedCRC != CRC) {
            BitIOLog(BitIOLog_ERROR, libModernPNGLibraryName, __func__, u8"CRC Mismatch");
        }
        
        //VerifyCRC(Dec->iHDR, ChunkSize, 1, 1, CRC);
    }
    
    void ParsePLTE(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Palette
        if (Dec->iHDR->BitDepth > 8) { // INVALID
            BitIOLog(BitIOLog_ERROR, libModernPNGLibraryName, __func__, u8"Invalid bit depth %d and palette combination\n", Dec->iHDR->BitDepth);
            BitBuffer_Skip(InputPNG, Bytes2Bits(ChunkSize));
        } else {
            
            if (Dec->iHDR->ColorType == PNG_PalettedRGB || Dec->iHDR->ColorType == PNG_RGB) {
                Dec->PLTE->Palette = calloc(1, (3 * Dec->PLTE->NumEntries) * Bits2Bytes(Dec->iHDR->BitDepth, Yes));
            } else if (Dec->iHDR->ColorType == PNG_RGBA) {
                Dec->PLTE->Palette = calloc(1, (4 * Dec->PLTE->NumEntries) * Bits2Bytes(Dec->iHDR->BitDepth, Yes));
            }
            
            
            for (uint8_t Channel = 0; Channel < ModernPNGChannelsPerColorType[Dec->iHDR->ColorType]; Channel++) {
                for (uint16_t PaletteEntry = 0; PaletteEntry < ChunkSize / 3; PaletteEntry++) {
                    Dec->PLTE->Palette[Channel][PaletteEntry] = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, Dec->iHDR->BitDepth);
                }
            }
        }
    }
    
    void ParseTRNS(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Transparency
        Dec->tRNS->NumEntries = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
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
            BitIOLog(BitIOLog_ERROR, libModernPNGLibraryName, __func__, u8"Failed to allocate enough memory for the TRNS Palette");
        } else {
            for (uint8_t Color = 0; Color < ModernPNGChannelsPerColorType[Dec->iHDR->ColorType]; Color++) {
                Entries[Color]    = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, Bits2Bytes(Dec->iHDR->BitDepth, true));
            }
            //Dec->tRNS->Palette = Entries;
        }
    }
    
    void ParseBKGD(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Background
        for (uint8_t Entry = 0; Entry < 3; Entry++) {
            Dec->bkGD->BackgroundPaletteEntry[Entry] = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
        }
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
    }
    
    void ParseGAMA(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Gamma
        Dec->gAMA->Gamma = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
    }
    
    void ParseOFFS(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Image Offset
        Dec->oFFs->XOffset       = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
        Dec->oFFs->YOffset       = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
        Dec->oFFs->UnitSpecifier = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
    }
    
    void ParsePHYS(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Aspect ratio, Physical pixel size
        Dec->pHYs->PixelsPerUnitXAxis = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
        Dec->pHYs->PixelsPerUnitYAxis = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
        Dec->pHYs->UnitSpecifier      = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
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
    }
    
    void ParsePCAL(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) {
        char CalibrationName[80];
        while (PeekBits(BitIOMSByte, BitIOLSBit, InputPNG, 8) != 0x0) {
            for (uint8_t Byte = 0; Byte < 80; Byte++) {
                CalibrationName[Byte] = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
            }
        }
        Dec->pCAL->CalibrationName     = CalibrationName;
        Dec->pCAL->CalibrationNameSize = UTF8String_GetNumCodePoints(Dec->pCAL->CalibrationName);
    }
    
    void ParseSBIT(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Significant bits per sample
        Dec->sBIT->Red                   = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
        Dec->sBIT->Green                 = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
        Dec->sBIT->Blue                  = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
    }
    
    void ParseSRGB(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) {
        Dec->sRGB->RenderingIntent       = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
    }
    
    void ParseSTER(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) {
        Dec->PNGIs3D                     = true;
        Dec->sTER->StereoType            = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
        
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
            CurrentByte  = PeekBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
            KeywordSize += 1;
        } while (CurrentByte != 0);
        
        uint32_t CommentSize = ChunkSize - KeywordSize;
        
        Dec->NumTextChunks  += 1;
        DecodePNG_Text_Init(Dec, KeywordSize, CommentSize);
        
        
        
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
        Dec->tIMe->Year                  = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 16);
        Dec->tIMe->Month                 = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
        Dec->tIMe->Day                   = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
        Dec->tIMe->Hour                  = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
        Dec->tIMe->Minute                = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
        Dec->tIMe->Second                = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
    }
    
    /* APNG */
    void ParseACTL(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) { // Animation control, part of APNG
        Dec->PNGIsAnimated               = true;
        Dec->acTL->NumFrames             = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
        Dec->acTL->TimesToLoop           = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32); // If 0, loop forever.
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
    }
    /* End APNG */
    
    void ParseHIST(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) {
    }
    
    void ParseICCP(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) {
        uint8_t ProfileNameSize = 0;
        ProfileNameSize = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
    }
    
    void ParseSPLT(DecodePNG *Dec, BitBuffer *InputPNG, uint32_t ChunkSize) {
    }
    
    uint8_t ParsePNGMetadata(BitBuffer *InputPNG, DecodePNG *Dec) {
        uint64_t FileMagic    = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 64);
        
        if (FileMagic == PNGMagic) {
            char     *ChunkID        = calloc(1, 4 * sizeof(uint8_t));
            uint32_t ChunkSize       = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 32);
            
            for (uint8_t ChunkIDSize = 0; ChunkIDSize < 4; ChunkIDSize++) {
                ChunkID[ChunkIDSize] = ReadBits(BitIOMSByte, BitIOLSBit, InputPNG, 8);
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
            BitIOLog(BitIOLog_ERROR, libModernPNGLibraryName, __func__, u8"File Magic 0x%X is not PNG, exiting\n", FileMagic);
            exit(EXIT_FAILURE);
        }
        return EXIT_SUCCESS;
    }
    
#ifdef __cplusplus
}
#endif
