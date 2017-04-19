#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include "../../../Dependencies/BitIO/libBitIO/include/BitIO.h"

#include "../../include/libModernPNG.h"
#include "../../include/Encode/EncodePNG.h"
#include "../../include/PNGTypes.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void WriteIHDRChunk(BitBuffer *BitB, EncodePNG *Enc) {
        WriteBits(BitB, 13, 32, true);
        WriteBits(BitB, iHDRMarker, 32, true);
        WriteBits(BitB, Enc->iHDR->Width, 32, true);
        WriteBits(BitB, Enc->iHDR->Height, 32, true);
        WriteBits(BitB, Enc->iHDR->BitDepth, 8, true);
        WriteBits(BitB, Enc->iHDR->ColorType, 8, true);
        WriteBits(BitB, Enc->iHDR->Compression, 8, true);
        WriteBits(BitB, Enc->iHDR->FilterMethod, 8, true);
        WriteBits(BitB, Enc->iHDR->IsInterlaced, 8, true);
        //WriteBits(BitB, GeneratedCRC, 32, true);
    }
    
    void WriteACTLChunk(BitBuffer *BitB, EncodePNG *Enc) {
        WriteBits(BitB, 8, 32, true);
        WriteBits(BitB, acTLMarker, 32, true);
        WriteBits(BitB, Enc->acTL->NumFrames, 32, true);
        WriteBits(BitB, Enc->acTL->TimesToLoop, 32, true);
        /*
         uint32_t GeneratedCRC = 0;
         GenerateCRC(Enc->acTL, 8, GeneratedCRC);
         WriteBits(BitB, GeneratedCRC, 32, true);
         */
    }
    
    void WriteFCTLChunk(BitBuffer *BitB, EncodePNG *Enc) {
        WriteBits(BitB, 29, 32, true);
        WriteBits(BitB, fcTLMarker, 32, true);
        WriteBits(BitB, Enc->fcTL->FrameNum, 32, true);
        WriteBits(BitB, Enc->fcTL->Width, 32, true);
        WriteBits(BitB, Enc->fcTL->Height, 32, true);
        WriteBits(BitB, Enc->fcTL->XOffset, 32, true);
        WriteBits(BitB, Enc->fcTL->YOffset, 32, true);
        WriteBits(BitB, Enc->fcTL->FrameDelayNumerator, 16, true);
        WriteBits(BitB, Enc->fcTL->FrameDelayDenominator, 16, true);
        WriteBits(BitB, Enc->fcTL->DisposeMethod, 8, true);
        WriteBits(BitB, Enc->fcTL->BlendMethod, 8, true);
        /*
         uint32_t GeneratedCRC = 0;
         GenerateCRC(Enc->fcTL, 29, GeneratedCRC);
         WriteBits(BitB, GeneratedCRC, 32, true);
         */
    }
    
    void WriteFDATChunk(BitBuffer *BitB, EncodePNG *Enc, uint8_t *DeflatedFrameData, uint32_t DeflatedFrameDataSize) {
        WriteBits(BitB, DeflatedFrameData + 8, 32, true);
        WriteBits(BitB, fDATMarker, 32, true);
        WriteBits(BitB, Enc->fdAT->FrameNum, 32, true);
        /*
         uint32_t GeneratedCRC = 0;
         GenerateCRC(Enc->fdAT, 8, GeneratedCRC);
         for (uint32_t Byte = 0; Byte < DeflatedFrameDataSize; Byte++) {
         GenerateCRC(&DeflatedFrameData[Byte], 8, GeneratedCRC); // Generate payload CRC as it's written
         WriteBits(BitB, DeflatedFrameData[Byte], 8, true);
         }
         WriteBits(BitB, GeneratedCRC, 32, true);
         */
    }
    
    void WriteSTERChunk(BitBuffer *BitB, EncodePNG *Enc) {
        WriteBits(BitB, 1, 32, true);
        WriteBits(BitB, sTERMarker, 32, true);
        WriteBits(BitB, Enc->sTER->StereoType, 8, true);
        /*
         uint32_t GeneratedCRC = 0;
         GenerateCRC(Enc->sTER->StereoType, 1, GeneratedCRC);
         WriteBits(BitB, GeneratedCRC, 32, true);
         */
    }
    
    void WriteBKGDChunk(BitBuffer *BitB, EncodePNG *Enc) {
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
        
        WriteBits(BitB, Size, 32, true);
        WriteBits(BitB, bKGDMarker, 32, true);
        
        if (Enc->iHDR->ColorType == PNG_PalettedRGB || Enc->iHDR->ColorType == PNG_Grayscale || Enc->iHDR->ColorType == PNG_GrayAlpha) {
            for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                WriteBits(BitB, Enc->bkGD->BackgroundPaletteEntry[Channel], BKGDEntrySize, true);
            }
        } else if (Enc->iHDR->ColorType == PNG_RGB || Enc->iHDR->ColorType == PNG_RGBA) {
            for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                WriteBits(BitB, Enc->bkGD->BackgroundPaletteEntry[Channel], NumChannels * 16, true);
            }
        }
        /*
         uint32_t GeneratedCRC = 0;
         GenerateCRC(Enc->bkGD->BackgroundPaletteEntry, BKGDEntrySize, GeneratedCRC);
         WriteBits(BitB, GeneratedCRC, 32, true);
         */
    }
    
    void WriteCHRMChunk(BitBuffer *BitB, EncodePNG *Enc) {
        WriteBits(BitB, 32, 32, true);
        WriteBits(BitB, cHRMMarker, 32, true);
        WriteBits(BitB, Enc->cHRM->WhitePointX, 32, true);
        WriteBits(BitB, Enc->cHRM->WhitePointY, 32, true);
        WriteBits(BitB, Enc->cHRM->RedX, 32, true);
        WriteBits(BitB, Enc->cHRM->RedY, 32, true);
        WriteBits(BitB, Enc->cHRM->GreenX, 32, true);
        WriteBits(BitB, Enc->cHRM->GreenY, 32, true);
        WriteBits(BitB, Enc->cHRM->BlueX, 32, true);
        WriteBits(BitB, Enc->cHRM->BlueY, 32, true);
        /*
         uint32_t GeneratedCRC = 0;
         GenerateCRC(Enc->cHRM, 32, GeneratedCRC);
         WriteBits(BitB, GeneratedCRC, 32, true);
         */
    }
    
    void WriteGAMAChunk(BitBuffer *BitB, EncodePNG *Enc) {
        WriteBits(BitB, 4, 32, true);
        WriteBits(BitB, gAMAMarker, 32, true);
        WriteBits(BitB, Enc->gAMA->Gamma, 32, true);
        /*
         uint32_t GeneratedCRC = 0;
         GenerateCRC(Enc->gAMA->Gamma, 32, GeneratedCRC);
         WriteBits(BitB, GeneratedCRC, 32, true);
         */
    }
    
    void WriteOFFSChunk(BitBuffer *BitB, EncodePNG *Enc) {
        WriteBits(BitB, 9, 32, true);
        WriteBits(BitB, oFFsMarker, 32, true);
        WriteBits(BitB, Enc->oFFs->XOffset, 32, true);
        WriteBits(BitB, Enc->oFFs->YOffset, 32, true);
        WriteBits(BitB, Enc->oFFs->UnitSpecifier, 8, true);
        /*
         uint32_t GeneratedCRC = 0;
         GenerateCRC(Enc->oFFs, 32, GeneratedCRC);
         WriteBits(BitB, GeneratedCRC, 32, true);
         */
    }
    /*
    void WriteICCPChunk(BitBuffer *BitB, EncodePNG *Enc) {
        WriteBits(BitB, Enc->iCCP->CompressedICCPProfileSize + Enc->iCCP->ProfileNameSize + 8, 32, true);
        WriteBits(BitB, iCCPMarker, 32, true);
        WriteBits(BitB, Enc->iCCP->ProfileName, Bytes2Bits(Enc->iCCP->ProfileName), true);
        WriteBits(BitB, Enc->iCCP->CompressionType, 8, true);
        WriteBits(BitB, Enc->iCCP->CompressedICCPProfile, Bytes2Bits(Enc->iCCP->CompressedICCPProfileSize), true);
        /
         uint32_t GeneratedCRC = 0;
         GenerateCRC(Enc->iCCP, 32, GeneratedCRC);
         WriteBits(BitB, GeneratedCRC, 32, true);
         /
    }
    */
    void WriteSBITChunk(BitBuffer *BitB, EncodePNG *Enc) {
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
        
        WriteBits(BitB, ChunkSize, 32, true);
        WriteBits(BitB, sBITMarker, 32, true);
        
        if (Enc->iHDR->ColorType == PNG_RGB || Enc->iHDR->ColorType == PNG_PalettedRGB) {
            WriteBits(BitB, Enc->sBIT->Red, 8, true);
            WriteBits(BitB, Enc->sBIT->Green, 8, true);
            WriteBits(BitB, Enc->sBIT->Blue, 8, true);
        } else if (Enc->iHDR->ColorType == PNG_RGBA) {
            WriteBits(BitB, Enc->sBIT->Red, 8, true);
            WriteBits(BitB, Enc->sBIT->Green, 8, true);
            WriteBits(BitB, Enc->sBIT->Blue, 8, true);
            WriteBits(BitB, Enc->sBIT->Alpha, 8, true);
        } else if (Enc->iHDR->ColorType == PNG_Grayscale) {
            WriteBits(BitB, Enc->sBIT->Grayscale, 8, true);
        } else if (Enc->iHDR->ColorType == PNG_GrayAlpha) {
            WriteBits(BitB, Enc->sBIT->Grayscale, 8, true);
            WriteBits(BitB, Enc->sBIT->Alpha, 8, true);
        }
        /*
         uint32_t GeneratedCRC = 0;
         GenerateCRC(Enc->sBIT, 32, GeneratedCRC); // Make sure it skips 0s
         WriteBits(BitB, GeneratedCRC, 32, true);
         */
    }
    
    void WriteSRGBChunk(BitBuffer *BitB, EncodePNG *Enc) {
        WriteBits(BitB, 1, 8, true);
        WriteBits(BitB, sRGBMarker, 32, true);
        WriteBits(BitB, Enc->sRGB->RenderingIntent, 8, true);
        /*
         uint32_t GeneratedCRC = 0;
         GenerateCRC(Enc->sRGB, 32, GeneratedCRC);
         WriteBits(BitB, GeneratedCRC, 32, true);
         */
    }
    
    void WritePHYSChunk(BitBuffer *BitB, EncodePNG *Enc) {
        WriteBits(BitB, 9, 8, true);
        WriteBits(BitB, pHYsMarker, 32, true);
        WriteBits(BitB, Enc->pHYs->PixelsPerUnitXAxis, 32, true);
        WriteBits(BitB, Enc->pHYs->PixelsPerUnitYAxis, 32, true);
        WriteBits(BitB, Enc->pHYs->UnitSpecifier, 8, true);
        /*
         uint32_t GeneratedCRC = 0;
         GenerateCRC(Enc->pHYs, 32, GeneratedCRC);
         WriteBits(BitB, GeneratedCRC, 32, true);
         */
    }
    
    void WritePCALChunk(BitBuffer *BitB, EncodePNG *Enc) {
        uint32_t ChunkSize = 0;
        WriteBits(BitB, ChunkSize, 32, true);
        WriteBits(BitB, pCALMarker, 32, true);
        
    }
    
    void WriteSCALChunk(BitBuffer *BitB, EncodePNG *Enc) {
        uint32_t ChunkSize = 0;
    }
    
#ifdef __cplusplus
}
#endif
