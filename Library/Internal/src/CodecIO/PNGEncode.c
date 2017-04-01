#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include "../../../Dependencies/BitIO/libBitIO/include/BitIO.h"

#include "../../include/libModernPNG.h"
#include "../../include/Encoder/EncodePNG.h"
#include "../../include/PNGTypes.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    struct PNGEncoder {
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
    };
    
    EncodePNG *InitEncodePNG(void) { // InitPNGEncode
        EncodePNG *Enc  = calloc(1, sizeof(EncodePNG));
        Enc->acTL       = calloc(1, sizeof(acTL));
        Enc->bkGD       = calloc(1, sizeof(bkGD));
        Enc->cHRM       = calloc(1, sizeof(cHRM));
        Enc->fcTL       = calloc(1, sizeof(fcTL));
        Enc->fdAT       = calloc(1, sizeof(fdAT));
        Enc->gAMA       = calloc(1, sizeof(gAMA));
        Enc->hIST       = calloc(1, sizeof(hIST));
        Enc->iCCP       = calloc(1, sizeof(iCCP));
        Enc->iHDR       = calloc(1, sizeof(iHDR));
        Enc->oFFs       = calloc(1, sizeof(oFFs));
        Enc->pCAL       = calloc(1, sizeof(pCAL));
        Enc->PLTE       = calloc(1, sizeof(PLTE));
        Enc->sBIT       = calloc(1, sizeof(sBIT));
        Enc->sRGB       = calloc(1, sizeof(sRGB));
        Enc->sTER       = calloc(1, sizeof(sTER));
        Enc->Text       = calloc(1, sizeof(Text));
        Enc->tIMe       = calloc(1, sizeof(tIMe));
        Enc->tRNS       = calloc(1, sizeof(tRNS));
        return Enc;
    }
    
    void WriteIHDRChunk(BitOutput *BitO, EncodePNG *Enc) {
        WriteBits(BitO, 13, 32, true);
        WriteBits(BitO, iHDRMarker, 32, true);
        WriteBits(BitO, Enc->iHDR->Width, 32, true);
        WriteBits(BitO, Enc->iHDR->Height, 32, true);
        WriteBits(BitO, Enc->iHDR->BitDepth, 8, true);
        WriteBits(BitO, Enc->iHDR->ColorType, 8, true);
        WriteBits(BitO, Enc->iHDR->Compression, 8, true);
        WriteBits(BitO, Enc->iHDR->FilterMethod, 8, true);
        WriteBits(BitO, Enc->iHDR->IsInterlaced, 8, true);
        //WriteBits(BitO, GeneratedCRC, 32, true);
        
    }
    
    void WriteACTLChunk(BitOutput *BitO, EncodePNG *Enc) {
        WriteBits(BitO, 8, 32, true);
        WriteBits(BitO, acTLMarker, 32, true);
        WriteBits(BitO, Enc->acTL->NumFrames, 32, true);
        WriteBits(BitO, Enc->acTL->TimesToLoop, 32, true);
        /*
         uint32_t GeneratedCRC = 0;
         GenerateCRC(Enc->acTL, 8, GeneratedCRC);
         WriteBits(BitO, GeneratedCRC, 32, true);
         */
    }
    
    void WriteFCTLChunk(BitOutput *BitO, EncodePNG *Enc) {
        WriteBits(BitO, 29, 32, true);
        WriteBits(BitO, fcTLMarker, 32, true);
        WriteBits(BitO, Enc->fcTL->FrameNum, 32, true);
        WriteBits(BitO, Enc->fcTL->Width, 32, true);
        WriteBits(BitO, Enc->fcTL->Height, 32, true);
        WriteBits(BitO, Enc->fcTL->XOffset, 32, true);
        WriteBits(BitO, Enc->fcTL->YOffset, 32, true);
        WriteBits(BitO, Enc->fcTL->FrameDelayNumerator, 16, true);
        WriteBits(BitO, Enc->fcTL->FrameDelayDenominator, 16, true);
        WriteBits(BitO, Enc->fcTL->DisposeMethod, 8, true);
        WriteBits(BitO, Enc->fcTL->BlendMethod, 8, true);
        /*
         uint32_t GeneratedCRC = 0;
         GenerateCRC(Enc->fcTL, 29, GeneratedCRC);
         WriteBits(BitO, GeneratedCRC, 32, true);
         */
    }
    
    void WriteFDATChunk(BitOutput *BitO, EncodePNG *Enc, uint8_t *DeflatedFrameData, uint32_t DeflatedFrameDataSize) {
        WriteBits(BitO, DeflatedFrameData + 8, 32, true);
        WriteBits(BitO, fDATMarker, 32, true);
        WriteBits(BitO, Enc->fdAT->FrameNum, 32, true);
        /*
         uint32_t GeneratedCRC = 0;
         GenerateCRC(Enc->fdAT, 8, GeneratedCRC);
         for (uint32_t Byte = 0; Byte < DeflatedFrameDataSize; Byte++) {
         GenerateCRC(&DeflatedFrameData[Byte], 8, GeneratedCRC); // Generate payload CRC as it's written
         WriteBits(BitO, DeflatedFrameData[Byte], 8, true);
         }
         WriteBits(BitO, GeneratedCRC, 32, true);
         */
    }
    
    void WriteSTERChunk(BitOutput *BitO, EncodePNG *Enc) {
        WriteBits(BitO, 1, 32, true);
        WriteBits(BitO, sTERMarker, 32, true);
        WriteBits(BitO, Enc->sTER->StereoType, 8, true);
        /*
         uint32_t GeneratedCRC = 0;
         GenerateCRC(Enc->sTER->StereoType, 1, GeneratedCRC);
         WriteBits(BitO, GeneratedCRC, 32, true);
         */
    }
    
    void WriteBKGDChunk(BitOutput *BitO, EncodePNG *Enc) {
        uint8_t  NumChannels   = ChannelsPerColorType[Enc->iHDR->ColorType];
        uint32_t Size          = 0;
        uint8_t  BKGDEntrySize = 0; // in bits
        
        if (Enc->iHDR->ColorType == PNG_PalettedRGB) {
            Size          = 1;
            BKGDEntrySize = Bytes2Bits(1);
        } else if (Enc->iHDR->ColorType == PNG_Grayscale || Enc->iHDR->ColorType == PNG_GrayAlpha) {
            Size          = 2;
            BKGDEntrySize = Bytes2Bits(2);
        } else if (Enc->iHDR->ColorType == PNG_RGB || Enc->iHDR->ColorType == PNG_RGBA) {
            Size          = NumChannels * 2;
            BKGDEntrySize = Bytes2Bits(NumChannels * 2);
        }
        
        WriteBits(BitO, Size, 32, true);
        WriteBits(BitO, bKGDMarker, 32, true);
        
        if (Enc->iHDR->ColorType == PNG_PalettedRGB || Enc->iHDR->ColorType == PNG_Grayscale || Enc->iHDR->ColorType == PNG_GrayAlpha) {
            for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                WriteBits(BitO, Enc->bkGD->BackgroundPaletteEntry[Channel], BKGDEntrySize, true);
            }
        } else if (Enc->iHDR->ColorType == PNG_RGB || Enc->iHDR->ColorType == PNG_RGBA) {
            for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                WriteBits(BitO, Enc->bkGD->BackgroundPaletteEntry[Channel], NumChannels * 16, true);
            }
        }
        /*
         uint32_t GeneratedCRC = 0;
         GenerateCRC(Enc->bkGD->BackgroundPaletteEntry, BKGDEntrySize, GeneratedCRC);
         WriteBits(BitO, GeneratedCRC, 32, true);
         */
    }
    
    void WriteCHRMChunk(BitOutput *BitO, EncodePNG *Enc) {
        WriteBits(BitO, 32, 32, true);
        WriteBits(BitO, cHRMMarker, 32, true);
        WriteBits(BitO, Enc->cHRM->WhitePointX, 32, true);
        WriteBits(BitO, Enc->cHRM->WhitePointY, 32, true);
        WriteBits(BitO, Enc->cHRM->RedX, 32, true);
        WriteBits(BitO, Enc->cHRM->RedY, 32, true);
        WriteBits(BitO, Enc->cHRM->GreenX, 32, true);
        WriteBits(BitO, Enc->cHRM->GreenY, 32, true);
        WriteBits(BitO, Enc->cHRM->BlueX, 32, true);
        WriteBits(BitO, Enc->cHRM->BlueY, 32, true);
        /*
         uint32_t GeneratedCRC = 0;
         GenerateCRC(Enc->cHRM, 32, GeneratedCRC);
         WriteBits(BitO, GeneratedCRC, 32, true);
         */
    }
    
    void WriteGAMAChunk(BitOutput *BitO, EncodePNG *Enc) {
        WriteBits(BitO, 4, 32, true);
        WriteBits(BitO, gAMAMarker, 32, true);
        WriteBits(BitO, Enc->gAMA->Gamma, 32, true);
        /*
         uint32_t GeneratedCRC = 0;
         GenerateCRC(Enc->gAMA->Gamma, 32, GeneratedCRC);
         WriteBits(BitO, GeneratedCRC, 32, true);
         */
    }
    
    void WriteOFFSChunk(BitOutput *BitO, EncodePNG *Enc) {
        WriteBits(BitO, 9, 32, true);
        WriteBits(BitO, oFFsMarker, 32, true);
        WriteBits(BitO, Enc->oFFs->XOffset, 32, true);
        WriteBits(BitO, Enc->oFFs->YOffset, 32, true);
        WriteBits(BitO, Enc->oFFs->UnitSpecifier, 8, true);
        /*
         uint32_t GeneratedCRC = 0;
         GenerateCRC(Enc->oFFs, 32, GeneratedCRC);
         WriteBits(BitO, GeneratedCRC, 32, true);
         */
    }
    
    void WriteICCPChunk(BitOutput *BitO, EncodePNG *Enc) {
        WriteBits(BitO, Enc->iCCP->CompressedICCPProfileSize + Enc->iCCP->ProfileNameSize + 8, 32, true);
        WriteBits(BitO, iCCPMarker, 32, true);
        WriteBits(BitO, Enc->iCCP->ProfileName, Bytes2Bits(Enc->iCCP->ProfileName), true);
        WriteBits(BitO, Enc->iCCP->CompressionType, 8, true);
        WriteBits(BitO, Enc->iCCP->CompressedICCPProfile, Bytes2Bits(Enc->iCCP->CompressedICCPProfileSize), true);
        /*
         uint32_t GeneratedCRC = 0;
         GenerateCRC(Enc->iCCP, 32, GeneratedCRC);
         WriteBits(BitO, GeneratedCRC, 32, true);
         */
    }
    
    void WriteSBITChunk(BitOutput *BitO, EncodePNG *Enc) {
        uint8_t ChunkSize = 0;
        uint8_t NumChannels = ChannelsPerColorType[Enc->iHDR->ColorType];
        if (Enc->iHDR->ColorType == PNG_Grayscale) {
            ChunkSize = 1;
        } else if (Enc->iHDR->ColorType == PNG_RGB || Enc->iHDR->ColorType == PNG_PalettedRGB) {
            ChunkSize = 3;
        } else if (Enc->iHDR->ColorType == PNG_GrayAlpha) {
            ChunkSize = 2;
        } else if (Enc->iHDR->ColorType == PNG_RGBA) {
            ChunkSize = 4;
        }
        
        WriteBits(BitO, ChunkSize, 32, true);
        WriteBits(BitO, sBITMarker, 32, true);
        
        if (Enc->iHDR->ColorType == PNG_RGB || Enc->iHDR->ColorType == PNG_PalettedRGB) {
            WriteBits(BitO, Enc->sBIT->Red, 8, true);
            WriteBits(BitO, Enc->sBIT->Green, 8, true);
            WriteBits(BitO, Enc->sBIT->Blue, 8, true);
        } else if (Enc->iHDR->ColorType == PNG_RGBA) {
            WriteBits(BitO, Enc->sBIT->Red, 8, true);
            WriteBits(BitO, Enc->sBIT->Green, 8, true);
            WriteBits(BitO, Enc->sBIT->Blue, 8, true);
            WriteBits(BitO, Enc->sBIT->Alpha, 8, true);
        } else if (Enc->iHDR->ColorType == PNG_Grayscale) {
            WriteBits(BitO, Enc->sBIT->Grayscale, 8, true);
        } else if (Enc->iHDR->ColorType == PNG_GrayAlpha) {
            WriteBits(BitO, Enc->sBIT->Grayscale, 8, true);
            WriteBits(BitO, Enc->sBIT->Alpha, 8, true);
        }
        /*
         uint32_t GeneratedCRC = 0;
         GenerateCRC(Enc->sBIT, 32, GeneratedCRC); // Make sure it skips 0s
         WriteBits(BitO, GeneratedCRC, 32, true);
         */
    }
    
    void WriteSRGBChunk(BitOutput *BitO, EncodePNG *Enc) {
        WriteBits(BitO, 1, 8, true);
        WriteBits(BitO, sRGBMarker, 32, true);
        WriteBits(BitO, Enc->sRGB->RenderingIntent, 8, true);
        /*
         uint32_t GeneratedCRC = 0;
         GenerateCRC(Enc->sRGB, 32, GeneratedCRC);
         WriteBits(BitO, GeneratedCRC, 32, true);
         */
    }
    
    void WritePHYSChunk(BitOutput *BitO, EncodePNG *Enc) {
        WriteBits(BitO, 9, 8, true);
        WriteBits(BitO, pHYsMarker, 32, true);
        WriteBits(BitO, Enc->pHYs->PixelsPerUnitXAxis, 32, true);
        WriteBits(BitO, Enc->pHYs->PixelsPerUnitYAxis, 32, true);
        WriteBits(BitO, Enc->pHYs->UnitSpecifier, 8, true);
        /*
         uint32_t GeneratedCRC = 0;
         GenerateCRC(Enc->pHYs, 32, GeneratedCRC);
         WriteBits(BitO, GeneratedCRC, 32, true);
         */
    }
    
    void WritePCALChunk(BitOutput *BitO, EncodePNG *Enc) {
        uint32_t ChunkSize = 0;
        WriteBits(BitO, ChunkSize, 32, true);
        WriteBits(BitO, pCALMarker, 32, true);
        
    }
    
    void WriteSCALChunk(BitOutput *BitO, EncodePNG *Enc) {
        uint32_t ChunkSize = 0;
    }
    
    void PNGEncodeFilterPaeth(EncodePNG *Enc, uint8_t *Line, size_t LineSize) {
        // RawData is after decoding the I/f DATs, and after INFLAT'ing and De-LZ77'ing it.
        // Each line is preceded by a filter type byte, so decode it by a line by line basis.
        // Good candidate for multi-threading.
    }
    
    void PNGEncodeFilterSub(EncodePNG *Enc, uint8_t *Line, size_t NumPixels) {
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
    
    void PNGEncodeAdam7(EncodePNG *Enc, uint8_t ****RawFrame) {
        uint8_t WidthPadding = 0, HeightPadding = 0, WidthStart = 0, HeightStart = 0;
        // Break image into 8x8 blocks, then break each chunk into 7 layers.
        if (Enc->iHDR->Width % 8 != 0) {
            WidthPadding = 8 - (Enc->iHDR->Width % 8);
        }
        if (Enc->iHDR->Height % 8 != 0) {
            HeightPadding = 8 - (Enc->iHDR->Height % 8);
        }
        // Pad the RawFrame by WidthPadding and HeightPadding
        // Then Split the image into 8x8 chunks, or maybe I should just have a few loops?
        
        // Create an array with size (Enc->iHDR->Width + WidthPadding) * (Enc->iHDR->Height + HeightPadding)
        uint16_t ****Image = calloc(((Enc->iHDR->Width + WidthPadding) * (Enc->iHDR->Height + HeightPadding)) * (Enc->Is3D * 2), 1);
        // Now copy the image into the new array, in the middle so the amount of padding is even on both sized
        // If the padding amount is odd, start at (Padding / 2)
        
        for (uint8_t StereoChannel = 0; StereoChannel < Enc->Is3D; StereoChannel++) {
            for (uint8_t Color = 0; Color < ChannelsPerColorType[Enc->iHDR->ColorType]; Color++) {
                for (uint64_t Height = HeightPadding / 2; Height < Enc->iHDR->Height; Height++) {
                    for (uint64_t Width = WidthPadding / 2; Width < Enc->iHDR->Width; Width++) {
                        Image[StereoChannel][Color][Height][Width] = RawFrame[StereoChannel][Color][Height][Width];
                    }
                }
            }
        }
        
        // Now I need to break up the image into 8x8 blocks
    }
    
    void OptimizeAdam7(EncodePNG *Enc, uint8_t ****Image) {
        
    }
    
    void OptimizePNG(EncodePNG *Enc, uint8_t ****Image) {
        // call PNGEncodeAdam7, then take the resulting image and try all the filters on an 8x8 block.
        // Actually, no. we should have a PNGFilterAdam7 function...
        // } else {
        // Just take the lines and try all the filters on each one, comparing them by comparing the difference between the bytes.
    }
    
    void PNGEncodeImage(EncodePNG *Enc, BitOutput *BitO) {
        
    }
    
#ifdef __cplusplus
}
#endif
