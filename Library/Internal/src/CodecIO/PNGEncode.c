#include "../include/libModernPNG.h"
#include "../include/EncodePNG.h"
#include "../include/PNGTypes.h"

#ifdef __cplusplus
extern "C" {
#endif
    
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
        uint32_t GeneratedCRC = 0;
        GenerateCRC(Enc->iHDR, 13, GeneratedCRC);
        WriteBits(BitO, GeneratedCRC, 32, true);
        
    }
    
    void WriteACTLChunk(BitOutput *BitO, EncodePNG *Enc) {
        WriteBits(BitO, 8, 32, true);
        WriteBits(BitO, acTLMarker, 32, true);
        WriteBits(BitO, Enc->acTL->NumFrames, 32, true);
        WriteBits(BitO, Enc->acTL->TimesToLoop, 32, true);
        uint32_t GeneratedCRC = 0;
        GenerateCRC(Enc->acTL, 8, GeneratedCRC);
        WriteBits(BitO, GeneratedCRC, 32, true);
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
        uint32_t GeneratedCRC = 0;
        GenerateCRC(Enc->fcTL, 29, GeneratedCRC);
        WriteBits(BitO, GeneratedCRC, 32, true);
    }
    
    void WriteFDATChunk(BitOutput *BitO, EncodePNG *Enc, uint8_t *DeflatedFrameData, uint32_t DeflatedFrameDataSize) {
        WriteBits(BitO, DeflatedFrameData + 8, 32, true);
        WriteBits(BitO, fDATMarker, 32, true);
        WriteBits(BitO, Enc->fdAT->FrameNum, 32, true);
        uint32_t GeneratedCRC = 0;
        GenerateCRC(Enc->fdAT, 8, GeneratedCRC);
        for (uint32_t Byte = 0; Byte < DeflatedFrameDataSize; Byte++) {
            GenerateCRC(&DeflatedFrameData[Byte], 8, GeneratedCRC); // Generate payload CRC as it's written
            WriteBits(BitO, DeflatedFrameData[Byte], 8, true);
        }
        WriteBits(BitO, GeneratedCRC, 32, true);
    }
    
    void WriteSTERChunk(BitOutput *BitO, EncodePNG *Enc) {
        WriteBits(BitO, 1, 32, true);
        WriteBits(BitO, sTERMarker, 32, true);
        WriteBits(BitO, Enc->sTER->StereoType, 8, true);
        uint32_t GeneratedCRC = 0;
        GenerateCRC(Enc->sTER->StereoType, 1, GeneratedCRC);
        WriteBits(BitO, GeneratedCRC, 32, true);
    }
    
    void WriteBKGDChunk(BitOutput *BitO, EncodePNG *Enc) {
        uint8_t NumChannels    = ChannelsPerColorType[Enc->iHDR->ColorType];
        uint32_t Size          = 0;
        uint8_t  BKGDEntrySize = 0; // in bits
        
        if (Enc->iHDR->ColorType == PalettedRGB) {
            Size          = 1;
            BKGDEntrySize = Bytes2Bits(1);
        } else if (Enc->iHDR->ColorType == Grayscale || Enc->iHDR->ColorType == GrayAlpha) {
            Size          = 2;
            BKGDEntrySize = Bytes2Bits(2);
        } else if (Enc->iHDR->ColorType == RGB || Enc->iHDR->ColorType == RGBA) {
            Size          = NumChannels * 2;
            BKGDEntrySize = Bytes2Bits(NumChannels * 2);
        }
        
        WriteBits(BitO, Size, 32, true);
        WriteBits(BitO, bKGDMarker, 32, true);
        
        if (Enc->iHDR->ColorType == PalettedRGB || Enc->iHDR->ColorType == Grayscale || Enc->iHDR->ColorType == GrayAlpha) {
            WriteBits(BitO, Enc->bkGD->BackgroundPaletteEntry, BKGDEntrySize, true);
        } else if (Enc->iHDR->ColorType == RGB || Enc->iHDR->ColorType == RGBA) {
            for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                WriteBits(BitO, Enc->bkGD->BackgroundPaletteEntry[Channel], NumChannels * 16, true);
            }
        }
        
        uint32_t GeneratedCRC = 0;
        GenerateCRC(Enc->bkGD->BackgroundPaletteEntry, BKGDEntrySize, GeneratedCRC);
        WriteBits(BitO, GeneratedCRC, 32, true);
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
    
    void PNGEncodeAdam7(EncodePNG *Enc, uint8_t *RawFrame) {
        // Break image into 8x8 blocks, then break each chunk into 7 layers.
    }
    
#ifdef __cplusplus
}
#endif
