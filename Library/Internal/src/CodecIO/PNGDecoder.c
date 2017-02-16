#include "../include/DecodePNG.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    struct PNGDecoder {
        uint32_t      iHDRSize;
        uint32_t      CurrentFrame;
        uint32_t      LineWidth;
        uint32_t      LinePadding;
        bool          IsVideo:1;
        bool          Is3D:1;
        bool          acTLExists:1;
        bool          fcTLExists:1;
        bool          cHRMExists:1;
        bool          gAMAExists:1;
        bool          iCCPExists:1;
        bool          sRGBExists:1;
        struct iHDR   *iHDR;
        struct acTL   *acTL;
        struct fcTL   *fcTL;
        struct fdAT   *fdAT;
        struct tRNS   *tRNS;
        struct cHRM   *cHRM;
        struct sBIT   *sBIT;
        struct Text   *Text;
        struct gAMA   *gAMA;
        struct oFFs   *oFFs;
        struct iCCP   *iCCP;
        struct sRGB   *sRGB;
        struct sTER   *sTER;
        struct PLTE   *PLTE;
        struct bkGD   *bkGD;
    };
    
    void ParseIHDR(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize) {
        PNG->iHDR->Width          = ReadBits(BitI, 32);
        PNG->iHDR->Height         = ReadBits(BitI, 32);
        PNG->iHDR->BitDepth       = ReadBits(BitI, 8);
        PNG->iHDR->ColorType      = ReadBits(BitI, 8);
        if ((PNG->iHDR->ColorType != 0) || (PNG->iHDR->ColorType != 2) || (PNG->iHDR->ColorType != 3) || (PNG->iHDR->ColorType != 4) || (PNG->iHDR->ColorType != 6)) {
            printf("Invalid color type!\n");
        }
        PNG->iHDR->Compression    = ReadBits(BitI, 8);
        PNG->iHDR->FilterMethod   = ReadBits(BitI, 8);
        PNG->iHDR->IsInterlaced   = ReadBits(BitI, 8);
        SkipBits(BitI, Bytes2Bits(ChunkSize - 13)); // incase the header is longer.
        uint32_t CRC              = ReadBits(BitI, 32);
        //VerifyCRC(PNG->iHDR, ChunkSize, 1, 1, CRC);
    }
    
    void ParsePLTE(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize) { // Palette
        if (PNG->iHDR->BitDepth > 8) { // INVALID
            SkipBits(BitI, Bytes2Bits(ChunkSize));
            return;
        } else {
            uint8_t Palette[4][ChunkSize / 3]; // 1st index is the color plane
            for (uint8_t Channel = 0; Channel < ChannelsPerColorType[PNG->iHDR->ColorType]; Channel++) {
                for (uint16_t Pixel = 0; Pixel < ChunkSize / 3; Pixel++) {
                    Palette[Channel][Pixel] = ReadBits(BitI, PNG->iHDR->BitDepth);
                }
            }
        }
        uint32_t CRC = ReadBits(BitI, 32);
    }
    
    void ParseTRNS(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize) { // Transparency
                                                                          // Alpha values of the PLTE chunk are stored here.
        uint32_t CRC = ReadBits(BitI, 32);
    }
    
    void ParseBKGD(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize) { // Background
                                                                          // WARNING: *MUST* have PLTE chunk.
        for (uint8_t Entry = 0; Entry < 3; Entry++) {
            PNG->bkGD->BackgroundPaletteEntry[Entry] = ReadBits(BitI, 8);
        }
        PNG->bkGD->CRC = ReadBits(BitI, 32);
    }
    
    void ParseCHRM(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize) { // Chromaticities
        PNG->cHRM->WhitePoint[0] = ReadBits(BitI, 32);
        PNG->cHRM->WhitePoint[1] = ReadBits(BitI, 32);
        PNG->cHRM->Red[0]        = ReadBits(BitI, 32);
        PNG->cHRM->Red[1]        = ReadBits(BitI, 32);
        PNG->cHRM->Green[0]      = ReadBits(BitI, 32);
        PNG->cHRM->Green[1]      = ReadBits(BitI, 32);
        PNG->cHRM->Blue[0]       = ReadBits(BitI, 32);
        PNG->cHRM->Blue[1]       = ReadBits(BitI, 32);
        PNG->cHRM->CRC           = ReadBits(BitI, 32);
    }
    
    void ParseGAMA(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize) { // Gamma
        PNG->gAMA->Gamma = ReadBits(BitI, 32);
        PNG->gAMA->CRC = ReadBits(BitI, 32);
    }
    
    void ParseOFFS(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize) { // Image Offset
        PNG->oFFs
        bool Type    = ReadBits(BitI, 8);
        uint32_t X   = ReadBits(BitI, 32);
        uint32_t Y   = ReadBits(BitI, 32);
        uint32_t CRC = ReadBits(BitI, 32);
    }
    
    void ParsePHYS(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize) { // Aspect ratio, Physical pixel size
        bool Type    = ReadBits(BitI, 8);
        uint32_t X   = ReadBits(BitI, 32);
        uint32_t Y   = ReadBits(BitI, 32);
        uint32_t CRC = ReadBits(BitI, 32);
    }
    
    void ParseSCAL(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize) { // Physical Scale
        bool Type    = ReadBits(BitI, 8);
        uint32_t X   = ReadBits(BitI, 32);
        uint32_t Y   = ReadBits(BitI, 32);
        uint32_t CRC = ReadBits(BitI, 32);
    }
    
    void ParseSBIT(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize) { // Significant bits per sample
        uint8_t Red   = ReadBits(BitI, 8);
        uint8_t Green = ReadBits(BitI, 8);
        uint8_t Blue  = ReadBits(BitI, 8);
        uint32_t CRC  = ReadBits(BitI, 32);
    }
    
    void ParseSRGB(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize) {
        uint8_t sRGBRenderingIntent = ReadBits(BitI, 8);
        uint32_t CRC                = ReadBits(BitI, 32);
    }
    
    void ParseSTER(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize) {
        PNG->Is3D = true;
        PNG->sTER->StereoType = ReadBits(BitI, 8);
        uint32_t CRC          = ReadBits(BitI, 32);
        
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
    
    void ParseTEXt(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize) { // tEXt
        uint32_t CRC = ReadBits(BitI, 32);
    }
    
    void ParseZTXt(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize) { // Compressed text
        uint32_t CRC = ReadBits(BitI, 32);
    }
    
    void ParseITXt(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize) { // International Text
        uint32_t CRC = ReadBits(BitI, 32);
    }
    
    void ParseTIME(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize) {
        uint16_t Year   = ReadBits(BitI, 16);
        uint8_t  Month  = ReadBits(BitI, 8);
        uint8_t  Day    = ReadBits(BitI, 8);
        uint8_t  Hour   = ReadBits(BitI, 8);
        uint8_t  Minute = ReadBits(BitI, 8);
        uint8_t  Second = ReadBits(BitI, 8);
        
        uint32_t CRC    = ReadBits(BitI, 32);
    }
    
    /* APNG */
    void ParseACTL(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize) { // Animation control, part of APNG
        PNG->IsVideo = true;
        uint32_t NumberOfFrames = ReadBits(BitI, 32);
        uint32_t TimesToLoop    = ReadBits(BitI, 32); // If 0, loop forever.
        uint32_t CRC            = ReadBits(BitI, 32);
    }
    
    void ParseFCTL(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize) { // Frame Control, part of APNG
        uint32_t FrameNumber    = ReadBits(BitI, 32);
        uint32_t Width          = ReadBits(BitI, 32);
        uint32_t Height         = ReadBits(BitI, 32);
        uint32_t XOffset        = ReadBits(BitI, 32);
        uint32_t YOffset        = ReadBits(BitI, 32);
        uint16_t Delay1         = ReadBits(BitI, 16);
        uint16_t Delay2         = ReadBits(BitI, 16);
        uint8_t  DisposeMethod  = ReadBits(BitI, 8);
        bool     BlendMethod    = ReadBits(BitI, 8);
        
        uint32_t CRC            = ReadBits(BitI, 32);
    }
    /* End APNG */
    
    void ParseIDAT(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize) { // IDAT
        DecodeINFLATE
        uint32_t CRC = ReadBits(BitI, 32);
    }
    
    void ParseHIST(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize) {
        uint32_t CRC = ReadBits(BitI, 32);
    }
    
    void ParseICCP(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize) {
        uint32_t CRC = ReadBits(BitI, 32);
    }
    
    void ParsePCAL(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize) {
        uint32_t CRC = ReadBits(BitI, 32);
    }
    
    void ParseSPLT(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize) {
        uint32_t CRC = ReadBits(BitI, 32);
    }
    
    void ParseFDAT(BitInput *BitI, PNGDecoder *PNG, uint32_t ChunkSize) {
        uint32_t CRC = ReadBits(BitI, 32);
    }
    
    uint8_t ParsePNGMetadata(BitInput *BitI, PNGDecoder *PNG) {
        uint64_t FileMagic    = ReadBits(BitI, 64);
        if (FileMagic != PNGMagic) { // File identification failed.
            char Error[BitIOStringSize];
            snprintf(Error, BitIOStringSize, "File Magic 0x%llX is not PNG, exiting\n", FileMagic);
            Log(LOG_CRIT, "NewFLAC", "ParsePNG", Error);
            exit(EXIT_FAILURE);
        } else {
            char     ChunkID[4];
            uint32_t ChunkSize  = ReadBits(BitI, 32);
            ChunkID[0] = ReadBits(BitI, 8);
            ChunkID[1] = ReadBits(BitI, 8);
            ChunkID[2] = ReadBits(BitI, 8);
            ChunkID[3] = ReadBits(BitI, 8);
            uint32_t ChunkCRC   = ReadBits(BitI, 32);
            
            
            if (strcasecmp(ChunkID, "iHDR") == 0) {        // iHDR
                ParseIHDR(BitI, PNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "PLTE") == 0) { // PLTE
                ParsePLTE(BitI, PNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "bKGD") == 0) { // bKGD
                ParseBKGD(BitI, PNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "cHRM") == 0) { // cHRM
                ParseCHRM(BitI, PNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "gAMA") == 0) { // gAMA
                ParseGAMA(BitI, PNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "oFFs") == 0) { // oFFs
                ParseOFFS(BitI, PNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "pHYs") == 0) { // pHYs
                ParsePHYS(BitI, PNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "sBIT") == 0) { // sBIT
                ParseSBIT(BitI, PNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "sCAL") == 0) { // sCAL
                ParseSCAL(BitI, PNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "sRGB") == 0) { // sRGB
                ParseSRGB(BitI, PNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "sTER") == 0) { // sTER
                ParseSTER(BitI, PNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "tEXt") == 0) { // tEXt
                ParseTEXt(BitI, PNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "zTXt") == 0) { // zTXt
                ParseZTXt(BitI, PNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "iTXt") == 0) { // iTXt
                ParseITXt(BitI, PNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "tIME") == 0) { // tIME
                ParseTIME(BitI, PNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "tRNS") == 0) { // tRNS
                ParseTRNS(BitI, PNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "hIST") == 0) { // hIST
                ParseHIST(BitI, PNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "iCCP") == 0) { // iCCP
                ParseICCP(BitI, PNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "pCAL") == 0) { // pCAL
                ParsePCAL(BitI, PNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "sPLT") == 0) { // sPLT
                ParseSPLT(BitI, PNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "IDAT") == 0) { // iDAT
                ParseIDAT(BitI, PNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "fdAT") == 0) { // fdAT
                ParseFDAT(BitI, PNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "acTL") == 0) { // acTL
                ParseACTL(BitI, PNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "fcTL") == 0) { // fcTL
                ParseFCTL(BitI, PNG, ChunkSize);
            } else if (strcasecmp(ChunkID, "iEND") == 0) {
                SkipBits(BitI, 32); // iEND is 8 bytes long, so you'll need to skip the second half
                return 0;
            } else {
                char ErrorDescription[BitIOStringSize];
                snprintf(ErrorDescription, BitIOStringSize, "Unknown ChunkID: 0x%s, ChunkSize: %d, ChunkCRC: 0x%X\n", ChunkID, ChunkSize, ChunkCRC);
                Log(LOG_ERR, "NewPNG", "ParsePNG", ErrorDescription);
                
                SkipBits(BitI, Bytes2Bits(ChunkSize));
            }
        }
        return EXIT_SUCCESS;
    }
    
    void DecodePNGData(BitInput *BitI, PNGDecoder *PNG) {
        // read the iDAT/fDAT chunk header, then do the other stuff.
    }
    
    void DecodePNGImage(BitInput *BitI, PNGDecoder *PNG, uint8_t *DecodedImage) {
        // Parse all chunks except iDAT, fDAT, and iEND first.
        // When you come across an iDAT or fDAT. you need to store the start address, then return to parsing the meta chunks, then at the end  decode the i/f DATs.
        ParsePNGMetadata(BitI, PNG);
        
        if (PNG->Is3D == true) {
            
        }
        while (BitI->FilePosition < BitI->FileSize) {
            
        }
    }
    
    PNGDecoder *InitPNGDecoder(void) {
        PNGDecoder *Dec = calloc(sizeof(PNGDecoder), 1);
        Dec->iHDR       = calloc(sizeof(iHDR), 1);
        Dec->acTL       = calloc(sizeof(acTL), 1);
        Dec->fdAT       = calloc(sizeof(fdAT), 1);
        Dec->tRNS       = calloc(sizeof(tRNS), 1);
        Dec->cHRM       = calloc(sizeof(cHRM), 1);
        Dec->sBIT       = calloc(sizeof(sBIT), 1);
        Dec->fcTL       = calloc(sizeof(fcTL), 1);
        Dec->Text       = calloc(sizeof(Text), 1);
        Dec->gAMA       = calloc(sizeof(gAMA), 1);
        Dec->oFFs       = calloc(sizeof(oFFs), 1);
        Dec->iCCP       = calloc(sizeof(iCCP), 1);
        Dec->sRGB       = calloc(sizeof(sRGB), 1);
        Dec->sTER       = calloc(sizeof(sTER), 1);
        Dec->PLTE       = calloc(sizeof(PLTE), 1);
        Dec->bkGD       = calloc(sizeof(bkGD), 1);
        return Dec;
    }
    
#ifdef __cplusplus
}
#endif
