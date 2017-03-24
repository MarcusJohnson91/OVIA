#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include "../../../Dependencies/BitIO/libBitIO/include/BitIO.h"

#include "../../include/libModernPNG.h"
#include "../../include/Decoder/DecodePNG.h"
#include "../../include/PNGTypes.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    struct PNGDecoder {
        uint32_t      CurrentFrame;
        uint32_t      LineWidth;
        uint32_t      LinePadding;
        bool          IsVideo:1;
        bool          Is3D:1;
        bool          acTLExists:1;
        bool          bkGDExists:1;
        bool          cHRMExists:1;
        bool          fcTLExists:1;
        bool          gAMAExists:1;
        bool          hISTExists:1;
        bool          iCCPExists:1;
        bool          oFFsExists:1;
        bool          pCALExists:1;
        bool          pHYsExists:1;
        bool          PLTEExists:1;
        bool          sBITExists:1;
        bool          sCALExists:1;
        bool          sPLTExists:1;
        bool          sRGBExists:1;
        bool          sTERExists:1;
        bool          TextExists:1;
        bool          tIMEExists:1;
        bool          tRNSExists:1;
        struct acTL   *acTL;
        struct bkGD   *bkGD;
        struct cHRM   *cHRM;
        struct fcTL   *fcTL;
        struct fdAT   *fdAT;
        struct gAMA   *gAMA;
        struct hIST   *hIST;
        struct iCCP   *iCCP;
        struct iHDR   *iHDR;
        struct oFFs   *oFFs;
        struct pCAL   *pCAL;
        struct pHYs   *pHYs;
        struct PLTE   *PLTE;
        struct sBIT   *sBIT;
        struct sCAL   *sCAL;
        struct sRGB   *sRGB;
        struct sTER   *sTER;
        struct Text   *Text;
        struct tIMe   *tIMe;
        struct tRNS   *tRNS;
        uint16_t    ***DecodedImage;
    };
    
    DecodePNG *InitDecodePNG(void) {
        DecodePNG *Dec  = calloc(sizeof(DecodePNG), 1);
        Dec->acTL       = calloc(sizeof(acTL),1);
        Dec->bkGD       = calloc(sizeof(bkGD),1);
        Dec->cHRM       = calloc(sizeof(cHRM),1);
        Dec->fcTL       = calloc(sizeof(fcTL),1);
        Dec->fdAT       = calloc(sizeof(fdAT),1);
        Dec->gAMA       = calloc(sizeof(gAMA),1);
        Dec->hIST       = calloc(sizeof(hIST),1);
        Dec->iCCP       = calloc(sizeof(iCCP),1);
        Dec->iHDR       = calloc(sizeof(iHDR),1);
        Dec->oFFs       = calloc(sizeof(oFFs),1);
        Dec->pCAL       = calloc(sizeof(pCAL),1);
        Dec->PLTE       = calloc(sizeof(PLTE),1);
        Dec->sBIT       = calloc(sizeof(sBIT),1);
        Dec->sRGB       = calloc(sizeof(sRGB),1);
        Dec->sTER       = calloc(sizeof(sTER),1);
        Dec->Text       = calloc(sizeof(Text),1);
        Dec->tIMe       = calloc(sizeof(tIMe),1);
        Dec->tRNS       = calloc(sizeof(tRNS),1);
        return Dec;
    }
    
    /*
    bool VerifyChunkCRC(BitInput *BitI, uint32_t ChunkSize) {
        // So basically we need to read ChunkSize bytes into an array, then read the following 4 bytes as the CRC
        // then run VerifyCRC over the buffer, and finally compare the generated CRC with the extracted one, and return whether they match.
        // Then call SkipBits(BitI, Bytes2Bits(ChunkSize)); to reset to the beginning of the chunk
        uint8_t *Buffer2CRC = calloc(ChunkSize, 1);
        for (uint32_t Byte = 0; Byte < ChunkSize; Byte++) {
            Buffer2CRC[Byte] = BitI->Buffer[Bits2Bytes(, false)];
            
        }
        uint32_t ChunkCRC = ReadBits(BitI, 32, true);
        bool CRCsMatch = VerifyCRC(Buffer2CRC, ChunkSize, 0x82608EDB, 32, 0xFFFFFFFF, ChunkCRC);
        SkipBits(BitI, -Bytes2Bits(ChunkSize));
        return CRCsMatch;
    }
    */
    
    void CalculateSTERPadding(DecodePNG *Dec) {
        Dec->LinePadding = 7 - ((Dec->iHDR->Width - 1) % 8);
        Dec->LineWidth   = (Dec->iHDR->Width * 2) + Dec->LinePadding;
    }
    
    void ParseIHDR(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize) {
        Dec->iHDR->Width          = ReadBits(BitI, 32, true);
        Dec->iHDR->Height         = ReadBits(BitI, 32, true);
        Dec->iHDR->BitDepth       = ReadBits(BitI, 8, true);
        Dec->iHDR->ColorType      = ReadBits(BitI, 8, true);
        if (Dec->iHDR->ColorType == 1 || Dec->iHDR->ColorType == 5 || Dec->iHDR->ColorType >= 7) {
            Log(LOG_ALERT, "ModernPNG", "ParseiHDR", "Invalid color type: %d", Dec->iHDR->ColorType);
        }
        Dec->iHDR->Compression    = ReadBits(BitI, 8, true);
        Dec->iHDR->FilterMethod   = ReadBits(BitI, 8, true);
        Dec->iHDR->IsInterlaced   = ReadBits(BitI, 8, true);
        SkipBits(BitI, Bytes2Bits(ChunkSize - 13)); // incase the header is longer.
        uint32_t CRC              = ReadBits(BitI, 32, true);
        //VerifyCRC(Dec->iHDR, ChunkSize, 1, 1, CRC);
    }
    
    void ParsePLTE(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize) { // Palette
        if (Dec->iHDR->BitDepth > 8) { // INVALID
            Log(LOG_ALERT, "ModernPNG", "ParsePLTE", "Invalid bit depth %d and palette combination\n", Dec->iHDR->BitDepth);
            SkipBits(BitI, Bytes2Bits(ChunkSize));
        } else {
            uint8_t **Palette;
            
            if (Dec->iHDR->ColorType == PNG_PalettedRGB || Dec->iHDR->ColorType == PNG_RGB) {
                Palette = calloc(3 * Dec->PLTE->NumEntries, 1);
            } else if (Dec->iHDR->ColorType == PNG_RGBA) {
                Palette = calloc(4 * Dec->PLTE->NumEntries, 1);
            }
            
            
            for (uint8_t Channel = 0; Channel < ChannelsPerColorType[Dec->iHDR->ColorType]; Channel++) {
                for (uint16_t Pixel = 0; Pixel < ChunkSize / 3; Pixel++) {
                    Palette[Channel][Pixel] = ReadBits(BitI, Dec->iHDR->BitDepth, true);
                }
            }
        }
        Dec->PLTE->CRC = ReadBits(BitI, 32, true);
    }
    
    void ParseTRNS(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize) { // Transparency
        Dec->tRNS->NumEntries = ReadBits(BitI, 32, true);
        uint16_t **Entries = calloc(Dec->tRNS->NumEntries, 1);
        for (uint8_t Color = 0; Color < ChannelsPerColorType[Dec->iHDR->ColorType]; Color++) {
            Entries[Color] = ReadBits(BitI, Bits2Bytes(Dec->iHDR->BitDepth, true), true);
        }
        Dec->tRNS->Palette = Entries;
        Dec->tRNS->CRC = ReadBits(BitI, 32, true);
    }
    
    void ParseBKGD(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize) { // Background
        for (uint8_t Entry = 0; Entry < 3; Entry++) {
            Dec->bkGD->BackgroundPaletteEntry[Entry] = ReadBits(BitI, 8, true);
        }
        Dec->bkGD->CRC = ReadBits(BitI, 32, true);
    }
    
    void ParseCHRM(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize) { // Chromaticities
        Dec->cHRM->WhitePointX = ReadBits(BitI, 32, true);
        Dec->cHRM->WhitePointY = ReadBits(BitI, 32, true);
        Dec->cHRM->RedX        = ReadBits(BitI, 32, true);
        Dec->cHRM->RedY        = ReadBits(BitI, 32, true);
        Dec->cHRM->GreenX      = ReadBits(BitI, 32, true);
        Dec->cHRM->GreenY      = ReadBits(BitI, 32, true);
        Dec->cHRM->BlueX       = ReadBits(BitI, 32, true);
        Dec->cHRM->BlueY       = ReadBits(BitI, 32, true);
        Dec->cHRM->CRC         = ReadBits(BitI, 32, true);
    }
    
    void ParseGAMA(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize) { // Gamma
        Dec->gAMA->Gamma = ReadBits(BitI, 32, true);
        Dec->gAMA->CRC = ReadBits(BitI, 32, true);
    }
    
    void ParseOFFS(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize) { // Image Offset
        Dec->oFFs->XOffset       = ReadBits(BitI, 32, true);
        Dec->oFFs->YOffset       = ReadBits(BitI, 32, true);
        Dec->oFFs->UnitSpecifier = ReadBits(BitI, 8, true);
        Dec->oFFs->CRC           = ReadBits(BitI, 32, true);
    }
    
    void ParsePHYS(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize) { // Aspect ratio, Physical pixel size
        Dec->pHYs->PixelsPerUnitXAxis = ReadBits(BitI, 32, true);
        Dec->pHYs->PixelsPerUnitYAxis = ReadBits(BitI, 32, true);
        Dec->pHYs->UnitSpecifier      = ReadBits(BitI, 8, true);
        Dec->pHYs->CRC                = ReadBits(BitI, 32, true);
    }
    
    void ParseSCAL(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize) { // Physical Scale
        char Width[80];
        Dec->sCAL->UnitSpecifier = ReadBits(BitI, 8, true);
        while (PeekBits(BitI, 8, true) != 0x00) {
            for (uint8_t Byte = 0; Byte < 80; Byte++) {
                Width[Byte] = ReadBits(BitI, 8, true);
            }
        }
        Dec->sCAL->PixelWidth = &*Width;
        
        bool Type    = ReadBits(BitI, 8, true);
        uint32_t X   = ReadBits(BitI, 32, true);
        uint32_t Y   = ReadBits(BitI, 32, true);
        uint32_t CRC = ReadBits(BitI, 32, true);
    }
    
    void ParsePCAL(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize) {
        char CalibrationName[80];
        while (PeekBits(BitI, 8, true) != 0x0) {
            for (uint8_t Byte = 0; Byte < 80; Byte++) {
                CalibrationName[Byte] = ReadBits(BitI, 8, true);
            }
        }
        Dec->pCAL->CalibrationName = CalibrationName;
        
    }
    
    void ParseSBIT(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize) { // Significant bits per sample
        Dec->sBIT->Red                   = ReadBits(BitI, 8, true);
        Dec->sBIT->Green                 = ReadBits(BitI, 8, true);
        Dec->sBIT->Blue                  = ReadBits(BitI, 8, true);
        Dec->sBIT->CRC                   = ReadBits(BitI, 32, true);
    }
    
    void ParseSRGB(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize) {
        Dec->sRGB->RenderingIntent       = ReadBits(BitI, 8, true);
        Dec->sRGB->CRC                   = ReadBits(BitI, 32, true);
    }
    
    void ParseSTER(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize) {
        Dec->Is3D = true;
        Dec->sTER->StereoType            = ReadBits(BitI, 8, true);
        uint32_t CRC                     = ReadBits(BitI, 32, true);
        
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
    
    void ParseTEXt(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize) { // tEXt
        uint32_t CRC = ReadBits(BitI, 32, true);
    }
    
    void ParseZTXt(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize) { // Compressed text
        uint32_t CRC = ReadBits(BitI, 32, true);
    }
    
    void ParseITXt(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize) { // International Text
        
        uint32_t CRC = ReadBits(BitI, 32, true);
    }
    
    void ParseTIME(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize) {
        Dec->tIMe->Year                  = ReadBits(BitI, 16, true);
        Dec->tIMe->Month                 = ReadBits(BitI, 8, true);
        Dec->tIMe->Day                   = ReadBits(BitI, 8, true);
        Dec->tIMe->Hour                  = ReadBits(BitI, 8, true);
        Dec->tIMe->Minute                = ReadBits(BitI, 8, true);
        Dec->tIMe->Second                = ReadBits(BitI, 8, true);
        Dec->tIMe->CRC                   = ReadBits(BitI, 32, true);
    }
    
    /* APNG */
    void ParseACTL(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize) { // Animation control, part of APNG
        Dec->IsVideo = true;
        Dec->acTL->NumFrames             = ReadBits(BitI, 32, true);
        Dec->acTL->TimesToLoop           = ReadBits(BitI, 32, true); // If 0, loop forever.
        Dec->acTL->CRC                   = ReadBits(BitI, 32, true);
    }
    
    void ParseFCTL(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize) { // Frame Control, part of APNG
        Dec->fcTL->FrameNum              = ReadBits(BitI, 32, true);
        Dec->fcTL->Width                 = ReadBits(BitI, 32, true);
        Dec->fcTL->Height                = ReadBits(BitI, 32, true);
        Dec->fcTL->XOffset               = ReadBits(BitI, 32, true);
        Dec->fcTL->YOffset               = ReadBits(BitI, 32, true);
        Dec->fcTL->FrameDelayNumerator   = ReadBits(BitI, 16, true);
        Dec->fcTL->FrameDelayDenominator = ReadBits(BitI, 16, true);
        Dec->fcTL->DisposeMethod         = ReadBits(BitI, 8, true);
        Dec->fcTL->BlendMethod           = ReadBits(BitI, 8, true);
        uint32_t CRC                     = ReadBits(BitI, 32, true);
    }
    /* End APNG */
    
    void ParseIDAT(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize) { // IDAT
        // DecodeINFLATE
        uint32_t CRC = ReadBits(BitI, 32, true);
    }
    
    void ParseHIST(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize) {
        Dec->hIST->CRC = ReadBits(BitI, 32, true);
    }
    
    void ParseICCP(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize) {
        uint32_t CRC = ReadBits(BitI, 32, true);
    }
    
    void ParseSPLT(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize) {
        uint32_t CRC = ReadBits(BitI, 32, true);
    }
    
    void ParseFDAT(BitInput *BitI, DecodePNG *Dec, uint32_t ChunkSize) {
        // DecodeDEFLATE
        uint32_t CRC = ReadBits(BitI, 32, true);
    }
    
    uint8_t ParsePNGMetadata(BitInput *BitI, DecodePNG *Dec) {
        uint64_t FileMagic    = ReadBits(BitI, 64, true);
        if (FileMagic != PNGMagic) { // File identification failed.
            char Error[BitIOStringSize];
            snprintf(Error, BitIOStringSize, "File Magic 0x%llX is not PNG, exiting\n", FileMagic);
            Log(LOG_CRIT, "NewFLAC", "ParsePNG", Error);
            exit(EXIT_FAILURE);
        } else {
            char     ChunkID[4];
            uint32_t ChunkSize  = ReadBits(BitI, 32, true);
            ChunkID[0] = ReadBits(BitI, 8, true);
            ChunkID[1] = ReadBits(BitI, 8, true);
            ChunkID[2] = ReadBits(BitI, 8, true);
            ChunkID[3] = ReadBits(BitI, 8, true);
            uint32_t ChunkCRC   = ReadBits(BitI, 32, true);
            
            if (strcasecmp(ChunkID, "iHDR") == 0) {        // iHDR
                ParseIHDR(BitI, Dec, ChunkSize);
            } else if (strcasecmp(ChunkID, "PLTE") == 0) { // PLTE
                Dec->PLTEExists = true;
                ParsePLTE(BitI, Dec, ChunkSize);
            } else if (strcasecmp(ChunkID, "bKGD") == 0) { // bKGD
                Dec->bkGDExists = true;
                ParseBKGD(BitI, Dec, ChunkSize);
            } else if (strcasecmp(ChunkID, "cHRM") == 0) { // cHRM
                Dec->cHRMExists = true;
                ParseCHRM(BitI, Dec, ChunkSize);
            } else if (strcasecmp(ChunkID, "gAMA") == 0) { // gAMA
                Dec->gAMAExists = true;
                ParseGAMA(BitI, Dec, ChunkSize);
            } else if (strcasecmp(ChunkID, "oFFs") == 0) { // oFFs
                Dec->oFFsExists = true;
                ParseOFFS(BitI, Dec, ChunkSize);
            } else if (strcasecmp(ChunkID, "pHYs") == 0) { // pHYs
                Dec->pHYsExists = true;
                ParsePHYS(BitI, Dec, ChunkSize);
            } else if (strcasecmp(ChunkID, "sBIT") == 0) { // sBIT
                Dec->sBITExists = true;
                ParseSBIT(BitI, Dec, ChunkSize);
            } else if (strcasecmp(ChunkID, "sCAL") == 0) { // sCAL
                Dec->sCALExists = true;
                ParseSCAL(BitI, Dec, ChunkSize);
            } else if (strcasecmp(ChunkID, "sRGB") == 0) { // sRGB
                Dec->sRGBExists = true;
                ParseSRGB(BitI, Dec, ChunkSize);
            } else if (strcasecmp(ChunkID, "sTER") == 0) { // sTER
                Dec->sTERExists = true;
                ParseSTER(BitI, Dec, ChunkSize);
            } else if (strcasecmp(ChunkID, "tEXt") == 0) { // tEXt
                Dec->TextExists = true;
                ParseTEXt(BitI, Dec, ChunkSize);
            } else if (strcasecmp(ChunkID, "zTXt") == 0) { // zTXt
                Dec->TextExists = true;
                ParseZTXt(BitI, Dec, ChunkSize);
            } else if (strcasecmp(ChunkID, "iTXt") == 0) { // iTXt
                Dec->TextExists = true;
                ParseITXt(BitI, Dec, ChunkSize);
            } else if (strcasecmp(ChunkID, "tIME") == 0) { // tIME
                Dec->tIMEExists = true;
                ParseTIME(BitI, Dec, ChunkSize);
            } else if (strcasecmp(ChunkID, "tRNS") == 0) { // tRNS
                Dec->tRNSExists = true;
                ParseTRNS(BitI, Dec, ChunkSize);
            } else if (strcasecmp(ChunkID, "hIST") == 0 && Dec->PLTEExists) { // hIST
                Dec->hISTExists = true;
                ParseHIST(BitI, Dec, ChunkSize);
            } else if (strcasecmp(ChunkID, "iCCP") == 0) { // iCCP
                Dec->iCCPExists = true;
                ParseICCP(BitI, Dec, ChunkSize);
            } else if (strcasecmp(ChunkID, "pCAL") == 0) { // pCAL
                Dec->pCALExists = true;
                ParsePCAL(BitI, Dec, ChunkSize);
            } else if (strcasecmp(ChunkID, "sPLT") == 0) { // sPLT
                Dec->sPLTExists = true;
                ParseSPLT(BitI, Dec, ChunkSize);
            } else if (strcasecmp(ChunkID, "acTL") == 0) { // acTL
                Dec->acTLExists = true;
                ParseACTL(BitI, Dec, ChunkSize);
            } else if (strcasecmp(ChunkID, "fcTL") == 0) { // fcTL
                Dec->fcTLExists = true;
                ParseFCTL(BitI, Dec, ChunkSize);
            }
        }
        return EXIT_SUCCESS;
    }
    
    void PNGDecodeNonFilter(DecodePNG *Dec, uint8_t *DeEntropyedData, uint8_t *DeFilteredData, size_t Line) {
        for (size_t Byte = 1; Byte < Bits2Bytes(Dec->iHDR->BitDepth, true); Byte++) {
            DeFilteredData[Byte - 1] = DeEntropyedData[Byte]; // Remove filter indicating byte
        }
    }
    
    void PNGDecodeSubFilter(DecodePNG *Dec, uint8_t **DeEntropyedData, uint8_t **DeFilteredData, size_t Line) {
        for (size_t Byte = 1; Byte < Bits2Bytes(Dec->iHDR->BitDepth, true); Byte++) {
            DeFilteredData[Line][Byte - 1] = (DeEntropyedData[Line][Byte] + DeEntropyedData[Line][Byte+1]) & 0xFF;
        }
    }
    
    void PNGDecodeUpFilter(DecodePNG *Dec, uint8_t **DeEntropyedData, uint8_t **DeFilteredData, size_t Line) {
        for (size_t Byte = 1; Byte < Bits2Bytes(Dec->iHDR->BitDepth, true); Byte++) {
            DeFilteredData[Line][Byte - 1] = DeEntropyedData[Line][Byte] + DeEntropyedData[Line - 1][Byte] & 0xFF;
        }
    }
    
    void PNGDecodeAverageFilter(DecodePNG *Dec, uint8_t **DeEntropyedData, uint8_t **DeFilteredData, size_t Line) {
        uint8_t PixelSize = Bits2Bytes(Dec->iHDR->BitDepth, true);
        for (size_t Byte = 1; Byte < Bits2Bytes(Dec->iHDR->BitDepth, true); Byte++) {
            uint8_t Average = floor((DeEntropyedData[Line][Byte - (PixelSize)] + DeEntropyedData[Line - 1][Byte]) / 2);
            DeFilteredData[Line][Byte - 1] = DeEntropyedData[Line][Byte] + Average;
        }
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
    
    void PNGDecodePaethFilter(DecodePNG *Dec, uint8_t **DeEntropyedData, uint8_t **DeFilteredData, size_t Line) {
        // Filtering is applied to bytes, not pixels
        uint8_t PixelSize = Bits2Bytes(Dec->iHDR->BitDepth, true);
        for (size_t Byte = 1; Byte < Bits2Bytes(Dec->iHDR->BitDepth, true); Byte++) {
            if (Line == 0) { // Assume top and top left = 0
                DeFilteredData[Line][Byte] = PaethPredictor(DeEntropyedData[Line][Byte], 0, 0);
            } else {
                DeFilteredData[Line][Byte] = PaethPredictor(DeEntropyedData[Line][Byte], DeEntropyedData[Line][Byte - PixelSize], DeEntropyedData[Line - 1][Byte - PixelSize]);
            }
        }
    }
    
    void PNGDecodeFilter(DecodePNG *Dec, uint8_t ***InflatedBuffer) {
        char Error[BitIOStringSize];
        
        uint8_t DeFilteredData[Dec->iHDR->Height][Dec->iHDR->Width - 1];
        
        for (size_t Line = 0; Line < Dec->iHDR->Height; Line++) {
            uint8_t FilterType = *InflatedBuffer[Line][0];
            switch (FilterType) {
                case 0:
                    // copy the Line except byte 0 (the filter indication byte) to the output buffer.
                    PNGDecodeNonFilter(Dec, InflatedBuffer, DeFilteredData, Line);
                    break;
                case 1:
                    // SubFilter
                    PNGDecodeSubFilter(Dec, InflatedBuffer, DeFilteredData, Line);
                    break;
                case 2:
                    // UpFilter
                    PNGDecodeUpFilter(Dec, InflatedBuffer, DeFilteredData, Line);
                    break;
                case 3:
                    // AverageFilter
                    PNGDecodeAverageFilter(Dec, InflatedBuffer, DeFilteredData, Line);
                    break;
                case 4:
                    // PaethFilter
                    PNGDecodePaethFilter(Dec, InflatedBuffer, DeFilteredData, Line);
                    break;
                default:
                    snprintf(Error, BitIOStringSize, "Filter type: %d is invalid\n", FilterType);
                    Log(LOG_ERR, "ModernPNG", "PNGDecodeFilteredLine", Error);
                    break;
            }
        }
    }
    
    void DecodePNGData(BitInput *BitI, DecodePNG *Dec) {
        // read the iDAT/fDAT chunk header, then do the other stuff.
        while (BytesRemainingInFile(BitI) > 0) { // 12 is the start of IEND
            uint32_t ChunkSize = ReadBits(BitI, 32, true);
            uint32_t ChunkID   = ReadBits(BitI, 32, true);
            
            if (strcasecmp(ChunkID, "iDAT") == 0) {
                ParseIDAT(BitI, Dec, ChunkSize);
            } else if (strcasecmp(ChunkID, "acTL") == 0) {
                
            } else if (strcasecmp(ChunkID, "fdAT") == 0) {
                ParseFDAT(BitI, Dec, ChunkSize);
            }
        }
    }
    
    void DecodePNGImage(BitInput *BitI, DecodePNG *Dec, uint16_t *DecodedImage) {
        // Parse all chunks except iDAT, fDAT, and iEND first.
        // When you come across an iDAT or fDAT. you need to store the start address, then return to parsing the meta chunks, then at the end  decode the i/f DATs.
        ParsePNGMetadata(BitI, Dec);
        
        if (Dec->Is3D == true) {
            
        }
        while (BytesRemainingInFile(BitI) > 0) {
            
        }
    }
    
    uint16_t **DecodeAdam7(DecodePNG *Dec, uint16_t **DecodedImage) {
        // Break the image into 8x8 blocks.
        // MARK: if the image is not a multiple of 8, I assume you pad the edge blocks?
        
        
        
        return 0;
    }
    
    // Let's do this library right, by adding attach and delete functions for the various chunks, and let's also have a fancy function that applies color profiles to the pixels.
    // that's kinda a lot of work tho...
    
    void SeperateStereoImage(DecodePNG *Dec, uint16_t **DecodedImage) {
        
    }
    
    void PNGReadMetadata(BitInput *BitI, DecodePNG *Dec) {
        ParsePNGMetadata(BitI, Dec);
    }
    
#ifdef __cplusplus
}
#endif
