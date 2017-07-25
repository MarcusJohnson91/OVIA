#include <math.h>

#include "../../include/libModernPNG.h"
#include "../../include/Encode/EncodePNG.h"
#include "../../include/ModernPNGTypes.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void WriteIHDRChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        WriteBits(OutputPNG, 13, 32, true);
        WriteBits(OutputPNG, iHDRMarker, 32, true);
        WriteBits(OutputPNG, Enc->iHDR->Width, 32, true);
        WriteBits(OutputPNG, Enc->iHDR->Height, 32, true);
        WriteBits(OutputPNG, Enc->iHDR->BitDepth, 8, true);
        WriteBits(OutputPNG, Enc->iHDR->ColorType, 8, true);
        WriteBits(OutputPNG, Enc->iHDR->Compression, 8, true);
        WriteBits(OutputPNG, Enc->iHDR->FilterMethod, 8, true);
        WriteBits(OutputPNG, Enc->iHDR->IsInterlaced, 8, true);
        //WriteBits(OutputPNG, GeneratedCRC, 32, true);
    }
    
    void WriteACTLChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        WriteBits(OutputPNG, 8, 32, true);
        WriteBits(OutputPNG, acTLMarker, 32, true);
        WriteBits(OutputPNG, Enc->acTL->NumFrames, 32, true);
        WriteBits(OutputPNG, Enc->acTL->TimesToLoop, 32, true);
        /*
         uint32_t GeneratedCRC = 0;
         GenerateCRC(Enc->acTL, 8, GeneratedCRC);
         WriteBits(OutputPNG, GeneratedCRC, 32, true);
         */
    }
    
    void WriteFCTLChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        WriteBits(OutputPNG, 29, 32, true);
        WriteBits(OutputPNG, fcTLMarker, 32, true);
        WriteBits(OutputPNG, Enc->fcTL->FrameNum, 32, true);
        WriteBits(OutputPNG, Enc->fcTL->Width, 32, true);
        WriteBits(OutputPNG, Enc->fcTL->Height, 32, true);
        WriteBits(OutputPNG, Enc->fcTL->XOffset, 32, true);
        WriteBits(OutputPNG, Enc->fcTL->YOffset, 32, true);
        WriteBits(OutputPNG, Enc->fcTL->FrameDelayNumerator, 16, true);
        WriteBits(OutputPNG, Enc->fcTL->FrameDelayDenominator, 16, true);
        WriteBits(OutputPNG, Enc->fcTL->DisposeMethod, 8, true);
        WriteBits(OutputPNG, Enc->fcTL->BlendMethod, 8, true);
        /*
         uint32_t GeneratedCRC = 0;
         GenerateCRC(Enc->fcTL, 29, GeneratedCRC);
         WriteBits(OutputPNG, GeneratedCRC, 32, true);
         */
    }
    
    void WriteFDATChunk(EncodePNG *Enc, BitBuffer *OutputPNG, uint8_t *DeflatedFrameData, uint32_t DeflatedFrameDataSize) {
        WriteBits(OutputPNG, DeflatedFrameData + 8, 32, true);
        WriteBits(OutputPNG, fDATMarker, 32, true);
        WriteBits(OutputPNG, Enc->fdAT->FrameNum, 32, true);
        /*
         uint32_t GeneratedCRC = 0;
         GenerateCRC(Enc->fdAT, 8, GeneratedCRC);
         for (uint32_t Byte = 0; Byte < DeflatedFrameDataSize; Byte++) {
         GenerateCRC(&DeflatedFrameData[Byte], 8, GeneratedCRC); // Generate payload CRC as it's written
         WriteBits(OutputPNG, DeflatedFrameData[Byte], 8, true);
         }
         WriteBits(OutputPNG, GeneratedCRC, 32, true);
         */
    }
    
    void WriteSTERChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        WriteBits(OutputPNG, 1, 32, true);
        WriteBits(OutputPNG, sTERMarker, 32, true);
        WriteBits(OutputPNG, Enc->sTER->StereoType, 8, true);
        /*
         uint32_t GeneratedCRC = 0;
         GenerateCRC(Enc->sTER->StereoType, 1, GeneratedCRC);
         WriteBits(OutputPNG, GeneratedCRC, 32, true);
         */
    }
    
    void WriteBKGDChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        uint8_t  NumChannels   = PNGChannelsPerColorType[Enc->iHDR->ColorType];
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
        
        WriteBits(OutputPNG, Size, 32, true);
        WriteBits(OutputPNG, bKGDMarker, 32, true);
        
        if (Enc->iHDR->ColorType == PNG_PalettedRGB || Enc->iHDR->ColorType == PNG_Grayscale || Enc->iHDR->ColorType == PNG_GrayAlpha) {
            for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                WriteBits(OutputPNG, Enc->bkGD->BackgroundPaletteEntry[Channel], BKGDEntrySize, true);
            }
        } else if (Enc->iHDR->ColorType == PNG_RGB || Enc->iHDR->ColorType == PNG_RGBA) {
            for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                WriteBits(OutputPNG, Enc->bkGD->BackgroundPaletteEntry[Channel], NumChannels * 16, true);
            }
        }
        /*
         uint32_t GeneratedCRC = 0;
         GenerateCRC(Enc->bkGD->BackgroundPaletteEntry, BKGDEntrySize, GeneratedCRC);
         WriteBits(OutputPNG, GeneratedCRC, 32, true);
         */
    }
    
    void WriteCHRMChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        WriteBits(OutputPNG, 32, 32, true);
        WriteBits(OutputPNG, cHRMMarker, 32, true);
        WriteBits(OutputPNG, Enc->cHRM->WhitePointX, 32, true);
        WriteBits(OutputPNG, Enc->cHRM->WhitePointY, 32, true);
        WriteBits(OutputPNG, Enc->cHRM->RedX, 32, true);
        WriteBits(OutputPNG, Enc->cHRM->RedY, 32, true);
        WriteBits(OutputPNG, Enc->cHRM->GreenX, 32, true);
        WriteBits(OutputPNG, Enc->cHRM->GreenY, 32, true);
        WriteBits(OutputPNG, Enc->cHRM->BlueX, 32, true);
        WriteBits(OutputPNG, Enc->cHRM->BlueY, 32, true);
        /*
         uint32_t GeneratedCRC = 0;
         GenerateCRC(Enc->cHRM, 32, GeneratedCRC);
         WriteBits(OutputPNG, GeneratedCRC, 32, true);
         */
    }
    
    void WriteGAMAChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        WriteBits(OutputPNG, 4, 32, true);
        WriteBits(OutputPNG, gAMAMarker, 32, true);
        WriteBits(OutputPNG, Enc->gAMA->Gamma, 32, true);
        /*
         uint32_t GeneratedCRC = 0;
         GenerateCRC(Enc->gAMA->Gamma, 32, GeneratedCRC);
         WriteBits(OutputPNG, GeneratedCRC, 32, true);
         */
    }
    
    void WriteOFFSChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        WriteBits(OutputPNG, 9, 32, true);
        WriteBits(OutputPNG, oFFsMarker, 32, true);
        WriteBits(OutputPNG, Enc->oFFs->XOffset, 32, true);
        WriteBits(OutputPNG, Enc->oFFs->YOffset, 32, true);
        WriteBits(OutputPNG, Enc->oFFs->UnitSpecifier, 8, true);
        /*
         uint32_t GeneratedCRC = 0;
         GenerateCRC(Enc->oFFs, 32, GeneratedCRC);
         WriteBits(OutputPNG, GeneratedCRC, 32, true);
         */
    }
    /*
    void WriteICCPChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        WriteBits(OutputPNG, Enc->iCCP->CompressedICCPProfileSize + Enc->iCCP->ProfileNameSize + 8, 32, true);
        WriteBits(OutputPNG, iCCPMarker, 32, true);
        WriteBits(OutputPNG, Enc->iCCP->ProfileName, Bytes2Bits(Enc->iCCP->ProfileName), true);
        WriteBits(OutputPNG, Enc->iCCP->CompressionType, 8, true);
        WriteBits(OutputPNG, Enc->iCCP->CompressedICCPProfile, Bytes2Bits(Enc->iCCP->CompressedICCPProfileSize), true);
        /
         uint32_t GeneratedCRC = 0;
         GenerateCRC(Enc->iCCP, 32, GeneratedCRC);
         WriteBits(OutputPNG, GeneratedCRC, 32, true);
         /
    }
    */
    void WriteSBITChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        uint8_t ChunkSize = 0;
        uint8_t NumChannels = PNGChannelsPerColorType[Enc->iHDR->ColorType];
        if (Enc->iHDR->ColorType == PNG_Grayscale) {
            ChunkSize = 1;
        } else if (Enc->iHDR->ColorType == PNG_RGB || Enc->iHDR->ColorType == PNG_PalettedRGB) {
            ChunkSize = 3;
        } else if (Enc->iHDR->ColorType == PNG_GrayAlpha) {
            ChunkSize = 2;
        } else if (Enc->iHDR->ColorType == PNG_RGBA) {
            ChunkSize = 4;
        }
        
        WriteBits(OutputPNG, ChunkSize, 32, true);
        WriteBits(OutputPNG, sBITMarker, 32, true);
        
        if (Enc->iHDR->ColorType == PNG_RGB || Enc->iHDR->ColorType == PNG_PalettedRGB) {
            WriteBits(OutputPNG, Enc->sBIT->Red, 8, true);
            WriteBits(OutputPNG, Enc->sBIT->Green, 8, true);
            WriteBits(OutputPNG, Enc->sBIT->Blue, 8, true);
        } else if (Enc->iHDR->ColorType == PNG_RGBA) {
            WriteBits(OutputPNG, Enc->sBIT->Red, 8, true);
            WriteBits(OutputPNG, Enc->sBIT->Green, 8, true);
            WriteBits(OutputPNG, Enc->sBIT->Blue, 8, true);
            WriteBits(OutputPNG, Enc->sBIT->Alpha, 8, true);
        } else if (Enc->iHDR->ColorType == PNG_Grayscale) {
            WriteBits(OutputPNG, Enc->sBIT->Grayscale, 8, true);
        } else if (Enc->iHDR->ColorType == PNG_GrayAlpha) {
            WriteBits(OutputPNG, Enc->sBIT->Grayscale, 8, true);
            WriteBits(OutputPNG, Enc->sBIT->Alpha, 8, true);
        }
        /*
         uint32_t GeneratedCRC = 0;
         GenerateCRC(Enc->sBIT, 32, GeneratedCRC); // Make sure it skips 0s
         WriteBits(OutputPNG, GeneratedCRC, 32, true);
         */
    }
    
    void WriteSRGBChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        WriteBits(OutputPNG, 1, 8, true);
        WriteBits(OutputPNG, sRGBMarker, 32, true);
        WriteBits(OutputPNG, Enc->sRGB->RenderingIntent, 8, true);
        /*
         uint32_t GeneratedCRC = 0;
         GenerateCRC(Enc->sRGB, 32, GeneratedCRC);
         WriteBits(OutputPNG, GeneratedCRC, 32, true);
         */
    }
    
    void WritePHYSChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        WriteBits(OutputPNG, 9, 8, true);
        WriteBits(OutputPNG, pHYsMarker, 32, true);
        WriteBits(OutputPNG, Enc->pHYs->PixelsPerUnitXAxis, 32, true);
        WriteBits(OutputPNG, Enc->pHYs->PixelsPerUnitYAxis, 32, true);
        WriteBits(OutputPNG, Enc->pHYs->UnitSpecifier, 8, true);
        /*
         uint32_t GeneratedCRC = 0;
         GenerateCRC(Enc->pHYs, 32, GeneratedCRC);
         WriteBits(OutputPNG, GeneratedCRC, 32, true);
         */
    }
    
    void WritePCALChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        uint32_t ChunkSize = 0;
        WriteBits(OutputPNG, ChunkSize, 32, true);
        WriteBits(OutputPNG, pCALMarker, 32, true);
        
    }
    
    void WriteSCALChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        uint32_t ChunkSize = 0;
    }
    
#ifdef __cplusplus
}
#endif
