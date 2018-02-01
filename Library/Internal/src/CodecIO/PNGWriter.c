#include <string.h>

#include "../../../Dependencies/libPCM/Dependencies/BitIO/libBitIO/include/BitIOMath.h"

#include "../../include/libModernPNG.h"
#include "../../include/Private/libModernPNG_Types.h"

#include "../../include/Private/Encode/libModernPNG_Encode.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void WriteIHDRChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, 13);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, iHDRMarker);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, Enc->iHDR->Width);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, Enc->iHDR->Height);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 8,  Enc->iHDR->BitDepth);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 8,  Enc->iHDR->ColorType);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 8,  Enc->iHDR->Compression);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 8,  Enc->iHDR->FilterMethod);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 8,  Enc->iHDR->IsInterlaced);
    }
    
    void WriteACTLChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, 8);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, acTLMarker);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, Enc->acTL->NumFrames);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, Enc->acTL->TimesToLoop);
    }
    
    void WriteFCTLChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, 29);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, fcTLMarker);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, Enc->fcTL->FrameNum);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, Enc->fcTL->Width);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, Enc->fcTL->Height);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, Enc->fcTL->XOffset);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, Enc->fcTL->YOffset);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 16, Enc->fcTL->FrameDelayNumerator);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 16, Enc->fcTL->FrameDelayDenominator);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 8,  Enc->fcTL->DisposeMethod);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 8,  Enc->fcTL->BlendMethod);
    }
    
    void WriteFDATChunk(EncodePNG *Enc, BitBuffer *OutputPNG, uint8_t *DeflatedFrameData, uint32_t DeflatedFrameDataSize) {
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, DeflatedFrameData + 8);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, fDATMarker);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, Enc->fdAT->FrameNum);
    }
    
    void WriteSTERChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, 1);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, sTERMarker);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 8,  Enc->sTER->StereoType);
    }
    
    void WriteBKGDChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        uint8_t  NumChannels   = ModernPNGChannelsPerColorType[Enc->iHDR->ColorType];
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
        
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, Size);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, bKGDMarker);
        
        if (Enc->iHDR->ColorType == PNG_PalettedRGB || Enc->iHDR->ColorType == PNG_Grayscale || Enc->iHDR->ColorType == PNG_GrayAlpha) {
            for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, BKGDEntrySize, Enc->bkGD->BackgroundPaletteEntry[Channel]);
            }
        } else if (Enc->iHDR->ColorType == PNG_RGB || Enc->iHDR->ColorType == PNG_RGBA) {
            for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, NumChannels * 16, Enc->bkGD->BackgroundPaletteEntry[Channel]);
            }
        }
    }
    
    void WriteCHRMChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, 32);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, cHRMMarker);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, Enc->cHRM->WhitePointX);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, Enc->cHRM->WhitePointY);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, Enc->cHRM->RedX);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, Enc->cHRM->RedY);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, Enc->cHRM->GreenX);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, Enc->cHRM->GreenY);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, Enc->cHRM->BlueX);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, Enc->cHRM->BlueY);
    }
    
    void WriteGAMAChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, 4);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, gAMAMarker);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, Enc->gAMA->Gamma);
    }
    
    void WriteOFFSChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, 9);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, oFFsMarker);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, Enc->oFFs->XOffset);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, Enc->oFFs->YOffset);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 8,  Enc->oFFs->UnitSpecifier);
    }
    
    void WriteICCPChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        uint32_t ProfileNameSize       = strlen(Enc->iCCP->ProfileName);
        uint32_t CompressedProfileSize = strlen(Enc->iCCP->CompressedICCPProfile);
        
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, 8 + CompressedProfileSize + ProfileNameSize);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, iCCPMarker);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, Bytes2Bits(ProfileNameSize), Enc->iCCP->ProfileName);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 8, Enc->iCCP->CompressionType);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, Bytes2Bits(CompressedProfileSize), Enc->iCCP->CompressedICCPProfile);
    }
    
    void WriteSBITChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        uint8_t ChunkSize = 0;
        uint8_t NumChannels = ModernPNGChannelsPerColorType[Enc->iHDR->ColorType];
        if (Enc->iHDR->ColorType == PNG_Grayscale) {
            ChunkSize = 1;
        } else if (Enc->iHDR->ColorType == PNG_RGB || Enc->iHDR->ColorType == PNG_PalettedRGB) {
            ChunkSize = 3;
        } else if (Enc->iHDR->ColorType == PNG_GrayAlpha) {
            ChunkSize = 2;
        } else if (Enc->iHDR->ColorType == PNG_RGBA) {
            ChunkSize = 4;
        }
        
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, ChunkSize);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, sBITMarker);
        
        if (Enc->iHDR->ColorType == PNG_RGB || Enc->iHDR->ColorType == PNG_PalettedRGB) {
            WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 8, Enc->sBIT->Red);
            WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 8, Enc->sBIT->Green);
            WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 8, Enc->sBIT->Blue);
        } else if (Enc->iHDR->ColorType == PNG_RGBA) {
            WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 8, Enc->sBIT->Red);
            WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 8, Enc->sBIT->Green);
            WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 8, Enc->sBIT->Blue);
            WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 8, Enc->sBIT->Alpha);
        } else if (Enc->iHDR->ColorType == PNG_Grayscale) {
            WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 8, Enc->sBIT->Grayscale);
        } else if (Enc->iHDR->ColorType == PNG_GrayAlpha) {
            WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 8, Enc->sBIT->Grayscale);
            WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 8, Enc->sBIT->Alpha);
        }
    }
    
    void WriteSRGBChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 8, 1);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, sRGBMarker);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 8,  Enc->sRGB->RenderingIntent);
    }
    
    void WritePHYSChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 8, 9);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, pHYsMarker);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, Enc->pHYs->PixelsPerUnitXAxis);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, Enc->pHYs->PixelsPerUnitYAxis);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 8,  Enc->pHYs->UnitSpecifier);
    }
    
    void WritePCALChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        uint32_t ChunkSize = 0;
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, ChunkSize);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputPNG, 32, pCALMarker);
        
    }
    
    void WriteSCALChunk(EncodePNG *Enc, BitBuffer *OutputPNG) {
        uint32_t ChunkSize = 0;
    }
    
#ifdef __cplusplus
}
#endif
