#include "../../include/CodecIO/PNGCodec.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h" /* Included for Assertions */
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/TextIO/StringIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    void WriteIHDRChunk(PNGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, 13);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, PNGMarker_iHDR);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, Options->iHDR->Width);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, Options->iHDR->Height);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 8,  Options->iHDR->BitDepth);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 8,  Options->iHDR->ColorType);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 8,  Options->iHDR->Compression);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 8,  Options->iHDR->FilterMethod);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 8,  Options->iHDR->IsInterlaced);
    }
    
    void WriteACTLChunk(PNGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, 8);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, PNGMarker_acTL);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, Options->acTL->NumFrames);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, Options->acTL->TimesToLoop);
    }
    
    void WriteFCTLChunk(PNGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, 29);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, PNGMarker_fcTL);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, Options->fcTL->FrameNum);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, Options->fcTL->Width);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, Options->fcTL->Height);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, Options->fcTL->XOffset);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, Options->fcTL->YOffset);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 16, Options->fcTL->FrameDelayNumerator);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 16, Options->fcTL->FrameDelayDenominator);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 8,  Options->fcTL->DisposeMethod);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 8,  Options->fcTL->BlendMethod);
    }
    
    void WriteFDATChunk(PNGOptions *Options, BitBuffer *BitB, uint8_t *DeflatedFrameData, uint32_t DeflatedFrameDataSize) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, DeflatedFrameData + 8);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, PNGMarker_fDAT);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, Options->fdAT->FrameNum);
    }
    
    void WriteSTERChunk(PNGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, 1);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, PNGMarker_sTER);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 8,  Options->sTER->StereoType);
    }
    
    void WriteBKGDChunk(PNGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        uint8_t  NumChannels   = PNGNumChannelsFromColorType[Options->iHDR->ColorType];
        uint32_t Size          = 0;
        uint8_t  BKGDEntrySize = 0; // in bits
        
        if (Options->iHDR->ColorType == PNGColor_Palette) {
            Size          = 1;
            BKGDEntrySize = Bytes2Bits(1);
        } else if (Options->iHDR->ColorType == PNGColor_Gray || Options->iHDR->ColorType == PNGColor_GrayAlpha) {
            Size          = 2;
            BKGDEntrySize = Bytes2Bits(2);
        } else if (Options->iHDR->ColorType == PNGColor_RGB || Options->iHDR->ColorType == PNGColor_RGBAlpha) {
            Size          = NumChannels * 2;
            BKGDEntrySize = Bytes2Bits(NumChannels * 2);
        }
        
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, Size);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, PNGMarker_bKGD);
        
        if (Options->iHDR->ColorType == PNGColor_Palette || Options->iHDR->ColorType == PNGColor_Gray || Options->iHDR->ColorType == PNGColor_GrayAlpha) {
            for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BKGDEntrySize, Options->bkGD->BackgroundPaletteEntry[Channel]);
            }
        } else if (Options->iHDR->ColorType == PNGColor_RGB || Options->iHDR->ColorType == PNGColor_RGBAlpha) {
            for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, NumChannels * 16, Options->bkGD->BackgroundPaletteEntry[Channel]);
            }
        }
    }
    
    void WriteCHRMChunk(PNGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, 32);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, PNGMarker_cHRM);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, Options->cHRM->WhitePointX);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, Options->cHRM->WhitePointY);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, Options->cHRM->RedX);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, Options->cHRM->RedY);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, Options->cHRM->GreenX);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, Options->cHRM->GreenY);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, Options->cHRM->BlueX);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, Options->cHRM->BlueY);
    }
    
    void WriteGAMAChunk(PNGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, 4);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, PNGMarker_gAMA);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, Options->gAMA->Gamma);
    }
    
    void WriteOFFSChunk(PNGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, 9);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, PNGMarker_oFFs);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, Options->oFFs->XOffset);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, Options->oFFs->YOffset);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 8,  Options->oFFs->UnitSpecifier);
    }
    
    void WriteICCPChunk(PNGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        uint32_t ProfileNameSize       = UTF8_GetStringSizeInCodeUnits(Options->iCCP->ProfileName);
        uint32_t CompressedProfileSize = UTF8_GetStringSizeInCodeUnits(Options->iCCP->CompressedICCPProfile);
        
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, 8 + CompressedProfileSize + ProfileNameSize);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, PNGMarker_iCCP);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, Bytes2Bits(ProfileNameSize), Options->iCCP->ProfileName);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 8, Options->iCCP->CompressionType);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, Bytes2Bits(CompressedProfileSize), Options->iCCP->CompressedICCPProfile);
    }
    
    void WriteSBITChunk(PNGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        uint8_t ChunkSize = 0;
        uint8_t NumChannels = PNGNumChannelsFromColorType[Options->iHDR->ColorType];
        if (Options->iHDR->ColorType == PNGColor_Gray) {
            ChunkSize = 1;
        } else if (Options->iHDR->ColorType == PNGColor_RGB || Options->iHDR->ColorType == PNGColor_Palette) {
            ChunkSize = 3;
        } else if (Options->iHDR->ColorType == PNGColor_GrayAlpha) {
            ChunkSize = 2;
        } else if (Options->iHDR->ColorType == PNGColor_RGBAlpha) {
            ChunkSize = 4;
        }
        
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, ChunkSize);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, PNGMarker_sBIT);
        
        if (Options->iHDR->ColorType == PNGColor_RGB || Options->iHDR->ColorType == PNGColor_Palette) {
            BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 8, Options->sBIT->Red);
            BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 8, Options->sBIT->Green);
            BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 8, Options->sBIT->Blue);
        } else if (Options->iHDR->ColorType == PNGColor_RGBAlpha) {
            BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 8, Options->sBIT->Red);
            BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 8, Options->sBIT->Green);
            BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 8, Options->sBIT->Blue);
            BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 8, Options->sBIT->Alpha);
        } else if (Options->iHDR->ColorType == PNGColor_Gray) {
            BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 8, Options->sBIT->Grayscale);
        } else if (Options->iHDR->ColorType == PNGColor_GrayAlpha) {
            BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 8, Options->sBIT->Grayscale);
            BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 8, Options->sBIT->Alpha);
        }
    }
    
    void WriteSRGBChunk(PNGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 8, 1);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, PNGMarker_sRGB);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 8,  Options->sRGB->RenderingIntent);
    }
    
    void WritePHYSChunk(PNGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 8, 9);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, PNGMarker_pHYs);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, Options->pHYs->PixelsPerUnitXAxis);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, Options->pHYs->PixelsPerUnitYAxis);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 8,  Options->pHYs->UnitSpecifier);
    }
    
    void WritePCALChunk(PNGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        uint32_t ChunkSize = 0;
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, ChunkSize);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, PNGMarker_pCAL);
        
    }
    
    void WriteSCALChunk(PNGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        uint32_t ChunkSize = 0;
    }
    
    void PNGIsAnimated(PNGOptions *Options, const bool PNGIsAnimated) {
        AssertIO(Options != NULL);
        Options->PNGIsAnimated = PNGIsAnimated;
    }
    
    void ComposeAPNGNumFrames(PNGOptions *Options, const uint32_t NumFrames) {
        AssertIO(Options != NULL);
        Options->acTL->NumFrames = NumFrames;
    }
    
    void ComposeAPNGTimes2Loop(PNGOptions *Options, const uint32_t NumTimes2Loop) {
        AssertIO(Options != NULL);
        Options->acTL->TimesToLoop = NumTimes2Loop;
    }
    
    void ComposeAPNGFrameDelay(PNGOptions *Options, const uint32_t FrameDelayNumerator, const uint32_t FrameDelayDenominator) {
        AssertIO(Options != NULL);
        Options->fcTL->FrameDelayNumerator   = FrameDelayNumerator;
        Options->fcTL->FrameDelayDenominator = FrameDelayDenominator;
    }
    
    void PNGSetIHDR(PNGOptions *Options, const uint32_t Height, const uint32_t Width, const uint8_t BitDepth, PNGColorTypes ColorType, const bool Interlace) {
        AssertIO(Options != NULL);
        Options->iHDR->Height       = Height;
        Options->iHDR->Width        = Width;
        Options->iHDR->BitDepth     = BitDepth;
        Options->iHDR->ColorType    = ColorType;
        Options->iHDR->IsInterlaced = Interlace;
    }
    
    void PNGSetSTER(PNGOptions *Options, const bool StereoType) {
        AssertIO(Options != NULL);
        Options->sTER->StereoType = StereoType;
    }
    
    void PNGSetACTL(PNGOptions *Options, const uint32_t NumFrames, const uint32_t Times2Loop) {
        AssertIO(Options != NULL);
        Options->acTL->NumFrames   = NumFrames;
        Options->acTL->TimesToLoop = Times2Loop;
    }
    
    void PNGSetFCTL(PNGOptions *Options, const uint32_t FrameNum, const uint32_t FrameWidth, const uint32_t FrameHeight, uint32_t XOffset, uint32_t YOffset, uint16_t FrameDelayNumerator, uint16_t FrameDelayDenominator, uint8_t DisposalType, uint8_t BlendType) {
        AssertIO(Options != NULL);
        
    }
    
    void ComposePNGCHRMWhitePoint() {
        
    }
    
    BitBuffer *EncodePNGImage(PNGOptions *Options, void ****RawImage2Encode, bool InterlacePNG, bool OptimizePNG) {
        AssertIO(Options != NULL);
        // RawImage2Encode is a void pointer, the contents of Options->iHDR->BitDepth will define the data type, uint8_t or uint16_t
        
        // This is THE main function for encoding a buffer into a PNG file.
        return NULL;
    }
    
    BitBuffer *EncodeAdam7(PNGOptions *Options, BitBuffer *ProgressiveImage) { // Returns the interlaced image
        AssertIO(Options != NULL);
        /*
         Ok, so to interlace an image, we need to loop over every pixel in an 8x8 block?
         */
        BitBuffer *BitB = NULL;
        return BitB;
    }
    
    void PNGEncodeFilterNone(PNGOptions *Options, uint8_t *DeEntropyedData, uint8_t *DeFilteredData, size_t Line) {
        AssertIO(Options != NULL);
        AssertIO(DeEntropyedData != NULL);
        AssertIO(DeFilteredData != NULL);
    }
    
    void PNGEncodeFilterPaeth(PNGOptions *Options, uint8_t *Line, size_t LineSize) {
        // RawData is after decoding the I/f DATs, and after INFLAT'ing and De-LZ77'ing it.
        // Each line is preceded by a filter type byte, so decode it by a line by line basis.
        // Good candidate for multi-threading.
        AssertIO(Options != NULL);
        AssertIO(Line != NULL);
    }
    
    void PNGEncodeFilterSub(PNGOptions *Options, uint8_t *Line, size_t NumPixels) {
        AssertIO(Options != NULL);
        AssertIO(Line != NULL);
        // NumPixel means whole pixel not sub pixel.
        uint16_t *EncodedLine = calloc(1, Options->iHDR->Width * Bits2Bytes(Options->iHDR->BitDepth, Yes) * sizeof(uint16_t));
        for (size_t Pixel = 1; Pixel < NumPixels; Pixel++) {
            if (Pixel == 1) {
                EncodedLine[Pixel] = Line[Pixel];
            } else {
                EncodedLine[Pixel] = Line[Pixel] - Line[Pixel - 1];
            }
        }
        free(EncodedLine);
    }
    
    /*
     For now, I'm just going to store the image in Deflate as literals.
     
     it will be a valid PNG so for now who cares?
     */
    
    void PNGWriteHeader(PNGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        // Write the iHDR chunk, and then go through the list of other chunks to find out if other chunks should be written
    }
    
    void PNGWriteFooter(PNGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        uint32_t iENDSize = 0;
        uint32_t iENDCRC  = 0xAE426082;
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsRight, BitOrder_MSBitIsRight, 32, iENDSize);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsRight, BitOrder_MSBitIsRight, 32, PNGMarker_iEND);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsRight, BitOrder_MSBitIsRight, 32, iENDCRC);
    }
    
    void PNG_DAT_WriteZlibHeader(PNGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        bool FDICT                = 0;
        
        uint8_t CompressionInfo   = 7;
        uint8_t CompressionMethod = 8;
        
        uint16_t FCHECK           = CompressionInfo << 12;
        FCHECK                   |= CompressionMethod << 8;
        FCHECK                   |= FDICT << 6;
        FCHECK                   |= FDICT << 5;
        
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 4, CompressionMethod);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 4, CompressionInfo);
        
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 5, FCHECK);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 1, FDICT);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 2, 0); // Huh?
        
        if (FDICT == Yes) {
            BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 32, Options->DAT->DictID);
        }
    }
    
    void PNG_Flate_WriteLiteralBlock(PNGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        BitBuffer_Align(BitB, 1);
        // How do we know how many bytes to copy?
        // Let's keep a offset in the PNGOptions struct saying the last written pixel so we can keep track.
        uint16_t Bytes2Copy  = 0;
        uint16_t Bytes2Copy2 = Bytes2Copy ^ 0xFFFF;
        
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsRight, BitOrder_MSBitIsRight, 16, Bytes2Copy);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsRight, BitOrder_MSBitIsRight, 16, Bytes2Copy2);
        
        // Now we simply copy bytes from the image container to the BitBuffer; the max num bytes is 65536
        
        /*
         Block max size is 65536.
         
         We need to find out the total number of bytes in the ImageContainer by counting the number of Views * the Width * Height * Num Channels * BitDepth
         */
    }
    
    void PNG_Image_Insert(ImageContainer *Image, BitBuffer *BitB, bool OptimizePNG) {
        AssertIO(Image != NULL);
        AssertIO(BitB != NULL);
        /*
         Loop over the image container, line by line, and try all the filters.
         */
    }
    
    void PNG_Filter_Image(ImageContainer *Image) {
        AssertIO(Image != NULL);
        // Try each filter on each line, get the best by counting the diff between the symbols to decide which to use.
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
