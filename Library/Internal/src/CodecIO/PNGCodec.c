#include "../include/libModernPNG.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    struct iHDR {
        uint32_t Width;
        uint32_t Height;
        uint8_t  BitDepth;
        uint8_t  ColorType;
        uint8_t  Compression;
        uint8_t  FilterMethod;
        bool     IsInterlaced;
        uint32_t CRC;
    };
    
    struct acTL {
        uint32_t NumFrames;
        uint32_t TimesToLoop;
        uint32_t CRC;
    };
    
    struct fcTL {
        uint32_t NumFrames;
        uint32_t Width;
        uint32_t Height;
        uint32_t XOffset;
        uint32_t YOffset;
        uint16_t Delay1;
        uint16_t Delay2;
        uint8_t  DisposeMethod;
        bool     BlendMethod;
        uint32_t CRC;
    };
    
    struct PLTE {
        uint8_t NumEntries;
        uint8_t  Palette[3][256];
        uint32_t CRC;
    };
    
    struct tRNS {
        uint8_t  NumEntries;
        uint8_t  Palette[3][256];
        uint32_t CRC;
    };
    
    struct bkGD {
        uint8_t  BackgroundPaletteEntry[3];
        uint32_t CRC;
    };
    
    struct sTER {
        bool     ChunkExists:1;
        bool     StereoType:1;
        uint32_t CRC;
    };
    
    struct fdAT {
        uint32_t FrameNum;
    };
    
    struct cHRM { // sRGB or iCCP overrides cHRM
        uint32_t WhitePoint[2]; // X = 0, Y = 1
        uint32_t Red[2];
        uint32_t Green[2];
        uint32_t Blue[2];
    };
    
    struct gAMA { // sRGB or iCCP overrides gAMA
        uint32_t Gamma;
    };
    
    struct iCCP {
        uint8_t ProfileName[80];
        uint8_t NullSeperator; // Basically, once you've seen a null, subtract the bytes left in the chunk from the bytes read in order to find how large the compressed profile is.
        uint8_t CompressionType;
        uint8_t CompressedICCPProfile[];
    };
    
    struct sBIT {
        uint8_t Grayscale;
        uint8_t Red;
        uint8_t Green;
        uint8_t Blue;
        uint8_t Alpha;
    };
    
    struct sRGB {
        uint8_t RenderingIntent;
    };
    
    struct Text { // Replaces:  tEXt, iTXt, zTXt
        uint8_t TextType;
        uint8_t Keyword[80];
        uint8_t NullSeperator;
        uint8_t TextString[];
    };
    
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
        iHDR          *iHDR;
        struct acTL   *acTL;
        fcTL          *fcTL;
        fdAT          *fdAT;
        tRNS          *tRNS;
        cHRM          *cHRM;
        sBIT          *sBIT;
        Text          *Text;
        gAMA          *gAMA;
        iCCP          *iCCP;
        sRGB          *sRGB;
        sTER          *sTER;
        PLTE          *PLTE;
        bkGD          *bkGD;
    };
    
    struct PNGEncoder {
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
        iHDR          *iHDR;
        acTL   *acTL;
        fcTL          *fcTL;
        fdAT          *fdAT;
        tRNS          *tRNS;
        cHRM          *cHRM;
        sBIT          *sBIT;
        Text          *Text;
        gAMA          *gAMA;
        iCCP          *iCCP;
        sRGB          *sRGB;
        sTER          *sTER;
        PLTE          *PLTE;
        bkGD          *bkGD;
    };
    
    static const uint8_t ChannelsPerColorType[8] = {
        1, 0, 3, 3, 4, 4
    };
    
    static const char Number2Month[12][3] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
    };
    
    void CalculateSTERPadding(PNGDecoder *PNG) {
        PNG->LinePadding = 7 - ((PNG->iHDR->Width - 1) % 8);
        PNG->LineWidth   = (PNG->iHDR->Width * 2) + PNG->LinePadding;
    }
    
    /* PNG Decode Filters, Filters operate on bytes, not pixels. */
    void PNGDecodeNonFilter(PNGDecoder *PNG, uint8_t *DeEntropyedData[], uint8_t *DeFilteredData[], size_t Line) {
        for (size_t Byte = 1; Byte < Bits2Bytes(PNG->iHDR->BitDepth); Byte++) {
            DeFilteredData[Byte - 1] = DeEntropyedData[Byte]; // Remove filter indicating byte
        }
    }
    
    void PNGDecodeSubFilter(PNGDecoder *PNG, uint8_t *DeEntropyedData[], uint8_t *DeFilteredData[], size_t Line) {
        for (size_t Byte = 1; Byte < Bits2Bytes(PNG->iHDR->BitDepth); Byte++) {
            DeFilteredData[Line][Byte - 1] = (DeEntropyedData[Line][Byte] + DeEntropyedData[Line][Byte+1]) & 0xFF;
        }
    }
    
    void PNGDecodeUpFilter(PNGDecoder *PNG, uint8_t *DeEntropyedData[], uint8_t *DeFilteredData[], size_t Line) {
        for (size_t Byte = 1; Byte < Bits2Bytes(PNG->iHDR->BitDepth); Byte++) {
            DeFilteredData[Line][Byte - 1] = DeEntropyedData[Line][Byte] + DeEntropyedData[Line - 1][Byte] & 0xFF;
        }
    }
    
    void PNGDecodeAverageFilter(PNGDecoder *PNG, uint8_t *DeEntropyedData[], uint8_t *DeFilteredData[], size_t Line) {
        uint8_t PixelSize = Bits2Bytes(PNG->iHDR->BitDepth);
        for (size_t Byte = 1; Byte < Bits2Bytes(PNG->iHDR->BitDepth); Byte++) {
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
        /*
         function PaethPredictor (a, b, c)
         begin
         ; a = left, b = above, c = upper left
         p := a + b - c        ; initial estimate
         pa := llabs(p - a)      ; distances to a, b, c
         pb := llabs(p - b)
         pc := llabs(p - c)
         ; return nearest of a,b,c,
         ; breaking ties in order a,b,c.
         if pa <= pb AND pa <= pc then return a
         else if pb <= pc then return b
         else return c
         end
         */
    }
    
    void PNGDecodePaethFilter(PNGDecoder *PNG, uint8_t *DeEntropyedData[], uint8_t *DeFilteredData[], size_t Line) {
        // Filtering is applied to bytes, not pixels
        uint8_t PixelSize = Bits2Bytes(PNG->iHDR->BitDepth);
        for (size_t Byte = 1; Byte < Bits2Bytes(PNG->iHDR->BitDepth); Byte++) {
            if (Line == 0) { // Assume top and top left = 0
                DeFilteredData[Line][Byte] = PaethPredictor(DeEntropyedData[Line][Byte], 0, 0);
            } else {
                DeFilteredData[Line][Byte] = PaethPredictor(DeEntropyedData[Line][Byte], DeEntropyedData[Line][Byte - PixelSize], DeEntropyedData[Line - 1][Byte - PixelSize]);
            }
        }
    }
    
    /* PNG Encode Filters */
    void PNGEncodeFilterPaeth(PNGEncoder *PNG, uint8_t *Line, size_t LineSize) {
        // RawData is after decoding the I/f DATs, and after INFLAT'ing and De-LZ77'ing it.
        // Each line is preceded by a filter type byte, so decode it by a line by line basis.
        // Good candidate for multi-threading.
    }
    
    void PNGEncodeFilterSub(PNGEncoder *PNG, uint8_t *Line, size_t NumPixels) {
        // NumPixel means whole pixel not sub pixel.
        uint8_t EncodedLine[NumPixels];
        for (size_t Pixel = 1; Pixel < NumPixels; Pixel++) {
            if (Pixel == 1) {
                EncodedLine[Pixel] = Line[Pixel];
            } else {
                EncodedLine[Pixel] = Line[Pixel] - Line[Pixel - 1];
            }
        }
    }
    
    void PNGEncodeAdam7(PNGEncoder *PNG, uint8_t *RawFrame) {
        // Break image into 8x8 blocks, then break each chunk into 7 layers.
    }
    
    
    
    // When Callocing a struct that contains a variable length array (like the text processing block) postpone initing that struct until it's been detected, AND when you've detected it, simply take the ChunkSize, and subtract the number of bytes for all the fields, and voila! you've got the size of the mufuckin array!!!
    
    void PNGDecodeFilter(PNGDecoder *PNG, uint8_t *InflatedBuffer[]) {
        char Error[BitIOStringSize];
        
        uint8_t DeFilteredData[PNG->iHDR->Height][PNG->iHDR->Width - 1];
        
        for (size_t Line = 0; Line < PNG->iHDR->Height; Line++) {
            uint8_t FilterType = InflatedBuffer[Line][0];
            switch (FilterType) {
                case 0:
                    // copy the Line except byte 0 (the filter indication byte) to the output buffer.
                    break;
                case 1:
                    // SubFilter
                    break;
                case 2:
                    // UpFilter
                    break;
                case 3:
                    // AverageFilter
                    break;
                case 4:
                    // PaethFilter
                    break;
                default:
                    snprintf(Error, BitIOStringSize, "Filter type: %d is invalid\n");
                    Log(LOG_ERR, "NewPNG", "PNGDecodeFilteredLine", Error);
                    break;
            }
        }
    }
    
    void DecodePNGImage(BitInput *BitI, PNGDecoder *PNG, uint8_t *DecodedImage) {
        
    }
    
#ifdef __cplusplus
}
#endif
