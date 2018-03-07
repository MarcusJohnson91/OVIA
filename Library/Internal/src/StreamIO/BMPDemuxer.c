#include <stdlib.h>

#include "../../../Dependencies/FoundationIO/libFoundationIO/include/BitIO.h"
#include "../../../Dependencies/FoundationIO/libFoundationIO/include/Math.h"
#include "../../../Dependencies/FoundationIO/libFoundationIO/include/Log.h"

#include "../../include/libPCM.h"
#include "../../include/Private/libPCMTypes.h"
#include "../../include/Private/Image/BMPCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    static uint64_t GetBMPRowSize(const uint16_t BitsPerPixel, const uint32_t ImageWidth) {
        return floor((BitsPerPixel * ImageWidth + 31) / 32) * 4; // floor((4 * 1024 + 31) / 32) * 4
    }
    
    static uint64_t GetBMPPixelArraySize(const uint64_t RowSize, const int32_t ImageHeight) { // 5568, 3712, there's 54 extra bytes in the PixelArray...
        return RowSize * ImageHeight;
    }
    
    void BMPRLECompress() {
        // End of Scanline Marker: 0
        // End of File Marker: 1
        // RLE8 Delta: 2; Contains a vertical and horizontal coordinate for the value
        // There are 2 modes, absolute and
        // The first byte is the run length, the second byte is the value; special values as defined above have a run length of 0.
        
    }
    
    void BMPParseMetadata(PCMFile *PCM, BitBuffer *BitB) {
        uint32_t DIBSize                                        = 0UL;
        PCM->FileSize                                           = ReadBits(LSByteFirst, LSBitFirst, BitB, 32); // 1086
        BitBuffer_Skip(BitB, 32); // 2 16 bit Reserved fields
        PCM->Pic->BMPPixelOffset                                = ReadBits(LSByteFirst, LSBitFirst, BitB, 32); // 62
        if (PCM->Pic->BMPPixelOffset > 14) { // DIB Header
            DIBSize                                             = ReadBits(LSByteFirst, LSBitFirst, BitB, 32); // 40
            if (DIBSize > 0) {
                if (DIBSize >= 40) {
                    PCM->Pic->Width                             = ReadBits(LSByteFirst, LSBitFirst, BitB, 32); // 127
                    int32_t Height                              = ReadBits(LSByteFirst, LSBitFirst, BitB, 32); // 64
                    if (Height < 0) {
                        PCM->Pic->ImageIsTopDown                = Yes;
                    } else {
                        PCM->Pic->ImageIsTopDown                = No;
                    }
                    PCM->Pic->Height                            = Absolute(Height);
                } else if (DIBSize == 12) {
                    PCM->Pic->Width                             = ReadBits(LSByteFirst, LSBitFirst, BitB, 16);
                    int16_t Height                              = ReadBits(LSByteFirst, LSBitFirst, BitB, 16);
                    if (Height < 0) {
                        PCM->Pic->ImageIsTopDown                = Yes;
                    } else {
                        PCM->Pic->ImageIsTopDown                = No;
                    }
                    PCM->Pic->Height                            = Absolute(Height);
                }
                BitBuffer_Skip(BitB, 16); // NumPlanes, always 1
                PCM->BitDepth                                   = ReadBits(LSByteFirst, LSBitFirst, BitB, 16); // 1
                PCM->Pic->BMPCompressionType                    = ReadBits(LSByteFirst, LSBitFirst, BitB, 32); // 0
                PCM->Pic->NumBytesUsedBySamples                 = ReadBits(LSByteFirst, LSBitFirst, BitB, 32); // 1024
                PCM->Pic->BMPWidthPixelsPerMeter                = ReadBits(LSByteFirst, LSBitFirst, BitB, 32); // 2835
                PCM->Pic->BMPHeightPixelsPerMeter               = ReadBits(LSByteFirst, LSBitFirst, BitB, 32); // 2835
                PCM->Pic->BMPColorsIndexed                      = ReadBits(LSByteFirst, LSBitFirst, BitB, 32); // 2
                PCM->Pic->BMPIndexColorsUsed                    = ReadBits(LSByteFirst, LSBitFirst, BitB, 32); // 0
                if (DIBSize >= 56) {
                    PCM->Pic->BMPColorSpaceType                 = ReadBits(LSByteFirst, LSBitFirst, BitB, 32); // 0000FF00 aka 16,711,680
                    if (DIBSize >= 108) {
                        PCM->Pic->BMPXYZCoordinates[0]          = ReadBits(LSByteFirst, LSBitFirst, BitB, 32); // 65280
                        PCM->Pic->BMPXYZCoordinates[1]          = ReadBits(LSByteFirst, LSBitFirst, BitB, 32); // 256
                        PCM->Pic->BMPXYZCoordinates[2]          = ReadBits(LSByteFirst, LSBitFirst, BitB, 32); // 4,278,190,080
                        PCM->Pic->BMPRedGamma                   = ReadBits(LSByteFirst, LSBitFirst, BitB, 32); // 1,934,772,034
                        PCM->Pic->BMPGreenGamma                 = ReadBits(LSByteFirst, LSBitFirst, BitB, 32); // 687,194,752
                        PCM->Pic->BMPBlueGamma                  = ReadBits(LSByteFirst, LSBitFirst, BitB, 32); // 354,334,816
                        if (DIBSize >= 124) {
                            PCM->Pic->BMPICCIntent              = ReadBits(LSByteFirst, LSBitFirst, BitB, 32); // 32,212,256
                            PCM->Pic->BMPICCPayload             = ReadBits(LSByteFirst, LSBitFirst, BitB, 32); // 322,122,560
                            PCM->Pic->BMPICCPayloadSize         = ReadBits(LSByteFirst, LSBitFirst, BitB, 32); // 644,245,120
                            BitBuffer_Skip(BitB, 32); // More Reserved data.
                            // Ok so when the Height is positive, the image is upside down, the bottom of the image is at the top of the file.
                        }
                    }
                }
            }
            BitBuffer_Skip(BitB, DIBSize - BitBuffer_GetPosition(BitB)); // Skip the remaining bits.
        } else if (DIBSize == 40 && (PCM->Pic->BMPCompressionType == BMP_BitFields || PCM->Pic->BMPCompressionType == BMP_RGBABitFields)) {
            PCM->Pic->BMPRedMask                                = ReadBits(LSByteFirst, MSBitFirst, BitB, PCM->BitDepth);
            PCM->Pic->BMPGreenMask                              = ReadBits(LSByteFirst, LSBitFirst, BitB, PCM->BitDepth);
            PCM->Pic->BMPBlueMask                               = ReadBits(MSByteFirst, LSBitFirst, BitB, PCM->BitDepth);
            if (PCM->Pic->BMPCompressionType == BMP_RGBABitFields) {
                PCM->Pic->BMPAlphaMask                          = ReadBits(MSByteFirst, LSBitFirst, BitB, PCM->BitDepth);
            }
        } else {
            BitBuffer_Skip(BitB, Bits2Bytes((PCM->Pic->BMPPixelOffset - 14) - DIBSize, false)); // How do we dicker in the BitMasks?
            // Well, if there are bit masks, subtract them from the PixelOffset
        }
    }
    
    void **BMPExtractImage(PCMFile *PCM, BitBuffer *BitB) { // We need to convert the pixels to the Runtime Byte and Bit order.
        void **ExtractedImage = NULL;
        if (PCM != NULL && BitB != NULL) {
            uint8_t LinePadding = 4 - (PCM->Pic->Width % 4);
            if (PCM->Pic->ImageIsTopDown == Yes) {
                // Then read normally
                for (uint64_t Pixel = 0; Pixel < PCM->NumChannelAgnosticSamples; Pixel++) {
                    for (uint16_t Channel = 0; Channel < PCM->NumChannels; Channel++) {
                        // If the width is not divisible by 4, there are padding bytes at the end of each line
                    }
                }
            } else {
                // Read from the bottom to the top.
                for (uint64_t Pixel = PCM->NumChannelAgnosticSamples; Pixel > 0; Pixel++) {
                    for (uint16_t Channel = 0; Channel < PCM->NumChannels; Channel++) {
                        // Ok so we need to loop from left to right normally, but we need to start at the bottom of the image
                        // We need to calculate the offset of the left most, bottom pixel that way we can read each line in properly.
                        // We need to know the width of each sub pixel, the number of sub pixels per pixel, the amount of padding per line, and the padding per subpixel for less than 8 bit bitdepths.
                    }
                }
            }
            if (PCM->Pic->BMPIndexColorsUsed == Yes && PCM->Pic->BMPColorsIndexed > 0) {
                
            }
        }
        return ExtractedImage;
    }
    
    void BMPExtractPixels(PCMFile *PCM, BitBuffer *BitB, uint64_t NumPixels2Extract, uint16_t **ExtractedPixels) { // We need to convert the pixels to the Runtime Byte and Bit order.
        if (PCM != NULL && BitB != NULL && ExtractedPixels != NULL) {
            if (PCM->Pic->ImageIsTopDown == Yes) {
                // Then read normally
            } else {
                // Read from the bottom to the top.
            }
            
            
            
            
            
            
            for (uint64_t Pixel = 0; Pixel < NumPixels2Extract; Pixel++) {
                for (uint16_t Channel = 0; Channel < PCM->NumChannels; Channel++) { // Ok, this works when the bit depth is 8 bits per pixel, but what about 1 bit images, or palettized ones?
                    if (PCM->BitDepth == 1) {
                        ExtractedPixels[Channel][Pixel] = ReadBits(LSByteFirst, MSBitFirst, BitB, 1);
                    } else if (PCM->Pic->BMPColorsIndexed > 0 || PCM->Pic->BMPIndexColorsUsed > 0) {
                        // Indexxed colors, we'll need to convert the bits to the original color by looking it up in the table.
                    } else {
                        ExtractedPixels[Channel][Pixel] = ReadBits(LSByteFirst, LSBitFirst, BitB, PCM->BitDepth);
                    }
                }
            }
        } else if (PCM == NULL) {
            Log(Log_ERROR, __func__, U8("PCM Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (ExtractedPixels == NULL) {
            Log(Log_ERROR, __func__, U8("ExtractedPixels Pointer is NULL"));
        }
    }
    
#ifdef __cplusplus
}
#endif
