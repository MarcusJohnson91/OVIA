#include "../../include/libPCM.h"
#include "../../include/Private/libPCMTypes.h"
#include "../../include/Private/Image/BMPCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    enum BMPCompressionMethods {
        BMP_RGB           = 0,
        BMP_RLE_8Bit      = 1,
        BMP_RLE_4Bit      = 2,
        BMP_BitFields     = 3,
        BMP_JPEG          = 4,
        BMP_PNG           = 5,
        BMP_RGBABitFields = 6,
        BMP_CMYK          = 11,
        BMP_CMYK_RLE_8Bit = 12,
        BMP_CMYK_RLE_4Bit = 13,
    };
    
    void ParseBMPHeader(PCMFile *PCM, BitBuffer *BitB) {
        PCM->BMP->FileSize                                          = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
        PCM->BMP->Reserved1                                         = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
        PCM->BMP->Reserved2                                         = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
        PCM->BMP->PixelOffset                                       = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
        if (PCM->BMP->PixelOffset > 0) {
            PCM->BMP->DIBSize                                       = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
            if (PCM->BMP->DIBSize >= 12) {
                PCM->BMP->Width                                     = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
                PCM->BMP->Height                                    = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
                PCM->BMP->BiPlanes                                  = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
                PCM->BMP->BitDepth                                  = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 16);
                if (PCM->BMP->DIBSize >= 40) {
                    PCM->BMP->CompressionType                       = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                    PCM->BMP->ImageSize                             = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                    PCM->BMP->WidthPixelsPerMeter                   = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                    PCM->BMP->HeightPixelsPerMeter                  = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                    PCM->BMP->NumColorsInIndexUsed                  = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                    PCM->BMP->NumImportantColorsInIndex             = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                    if (PCM->BMP->DIBSize >= 52) {
                        PCM->BMP->RedMask                           = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                        PCM->BMP->GreenMask                         = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                        PCM->BMP->BlueMask                          = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                        PCM->BMP->AlphaMask                         = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                        if (PCM->BMP->DIBSize >= 56) {
                            PCM->BMP->ColorSpaceType                = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                            if (PCM->BMP->DIBSize >= 108) {
                                PCM->BMP->XYZCoordinates[0]         = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                                PCM->BMP->XYZCoordinates[1]         = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                                PCM->BMP->XYZCoordinates[2]         = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                                PCM->BMP->GammaRed                  = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                                PCM->BMP->GammaGreen                = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                                PCM->BMP->GammaBlue                 = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                                if (PCM->BMP->DIBSize >= 124) {
                                    PCM->BMP->ICCIntent             = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                                    PCM->BMP->ICCProfilePayload     = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                                    PCM->BMP->ICCProfilePayloadSize = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                                    PCM->BMP->Reserved              = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    uint8_t **BMPExtractSamples(PCMFile *PCM, BitBuffer *BitB, uint64_t Pixels2Extract) {
        uint8_t **ExtractedPixels = NULL;
        uint64_t PixelArraySize = Pixels2Extract * PCM->BMP->NumChannels * sizeof(uint8_t);
        ExtractedPixels = calloc(1, PixelArraySize);
        if (ExtractedPixels == NULL) {
            Log(LOG_ERR, "libPCM", "BMPExtractPixels", "Couldn't allocate memory for the Sample Array, %d", PixelArraySize);
        } else {
            for (uint64_t Channel = 0; Channel < PCM->BMP->NumChannels; Channel++) {
                for (uint64_t Pixel = 0; Pixel < Pixels2Extract; Pixel++) {
                    ExtractedPixels[Channel][Pixel] = ReadBits(BitIOLSByte, BitIOLSBit, BitB, PCM->BMP->BitDepth);
                }
            }
        }
        
        return ExtractedPixels;
    }
    
#ifdef __cplusplus
}
#endif
