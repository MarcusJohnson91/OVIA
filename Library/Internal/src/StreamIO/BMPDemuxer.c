#include <stdlib.h>
#include <math.h>

#include "../../../Dependencies/BitIO/libBitIO/include/BitIO.h"
#include "../../../Dependencies/BitIO/libBitIO/include/BitIOMath.h"

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
        return RowSize * Absolute(ImageHeight);
    }
    
    void BMPParseMetadata(PCMFile *PCM, BitBuffer *BitB) {
        uint32_t DIBSize = 0UL;
        PCM->FileSize                                           = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32); // 524406
        BitBuffer_Skip(BitB, 32); // 2 16 bit Reserved fields
        PCM->PIC->BMPPixelOffset                                = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32); // 118 from the start, so subtract 112 bits (14 from the magic) to get the real offset from here.
        if (PCM->PIC->BMPPixelOffset > 14) { // DIB Header
            DIBSize                                             = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32); // 40
            if (DIBSize > 0) {
                if (DIBSize == 40) {
                    PCM->PIC->Width                             = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32); // 1024
                    PCM->PIC->Height                            = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32); // 1024, Convert this to unsigned cuz it's negative
                } else if (DIBSize == 12) {
                    PCM->PIC->Width                             = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
                    PCM->PIC->Height                            = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
                }
                BitBuffer_Skip(BitB, 16); // NumPlanes, always 1
                PCM->BitDepth                                   = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16); // 4
                PCM->PIC->BMPCompressionType                    = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32); // 0
                PCM->PIC->NumBytesUsedBySamples                 = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32); // 524288 (Width * Height * BitDepth) / 8, so ImageSizeInBytes?
                PCM->PIC->BMPWidthPixelsPerMeter                = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32); // 0
                PCM->PIC->BMPHeightPixelsPerMeter               = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32); // 0
                PCM->PIC->BMPColorsIndexed                      = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32); // 0
                PCM->PIC->BMPIndexColorsUsed                    = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32); // 0
                if (DIBSize >= 56) {
                    PCM->PIC->BMPColorSpaceType                 = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                    if (DIBSize >= 108) {
                        PCM->PIC->BMPXYZCoordinates[0]          = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                        PCM->PIC->BMPXYZCoordinates[1]          = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                        PCM->PIC->BMPXYZCoordinates[2]          = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                        PCM->PIC->BMPRedGamma                   = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                        PCM->PIC->BMPGreenGamma                 = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                        PCM->PIC->BMPBlueGamma                  = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                        if (DIBSize >= 124) {
                            PCM->PIC->BMPICCIntent              = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                            PCM->PIC->BMPICCPayload             = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                            PCM->PIC->BMPICCPayloadSize         = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                            BitBuffer_Skip(BitB, 32); // More Reserved data.
                        }
                    }
                }
            }
        } else if (DIBSize == 40 && (PCM->PIC->BMPCompressionType == BMP_BitFields || PCM->PIC->BMPCompressionType == BMP_RGBABitFields)) {
            PCM->PIC->BMPRedMask                                = ReadBits(BitIOLSByte, BitIOMSBit, BitB, PCM->BitDepth);
            PCM->PIC->BMPGreenMask                              = ReadBits(BitIOLSByte, BitIOLSBit, BitB, PCM->BitDepth);
            PCM->PIC->BMPBlueMask                               = ReadBits(BitIOMSByte, BitIOLSBit, BitB, PCM->BitDepth);
            if (PCM->PIC->BMPCompressionType == BMP_RGBABitFields) {
                PCM->PIC->BMPAlphaMask                          = ReadBits(BitIOMSByte, BitIOLSBit, BitB, PCM->BitDepth);
            }
        } else {
            BitBuffer_Skip(BitB, Bits2Bytes((PCM->PIC->BMPPixelOffset - 14) - DIBSize, false)); // How do we dicker in the BitMasks?
            // Well, if there are bit masks, subtract them from the PixelOffset
        }
    }
    
    void BMPExtractPixels(PCMFile *PCM, BitBuffer *BitB, uint64_t NumPixels2Extract, uint16_t **ExtractedPixels) { // We need to convert the pixels to the Runtime Byte and Bit order.
        for (uint64_t Pixel = 0; Pixel < NumPixels2Extract; Pixel++) {
            for (uint16_t Channel = 0; Channel < PCM->NumChannels; Channel++) { // Ok, this works when the bit depth is 8 bits per pixel, but what about 1 bit images, or palettized ones?
                if (PCM->BitDepth == 1) {
                    ExtractedPixels[Channel][Pixel] = ReadBits(BitIOLSByte, BitIOMSBit, BitB, 1);
                } else if (PCM->PIC->BMPColorsIndexed > 0 || PCM->PIC->BMPIndexColorsUsed > 0) {
                    // Indexxed colors, we'll need to convert the bits to the original color by looking it up in the table.
                } else {
                    ExtractedPixels[Channel][Pixel] = ReadBits(BitIOLSByte, BitIOLSBit, BitB, PCM->BitDepth);
                }
            }
        }
    }
    
#ifdef __cplusplus
}
#endif
