#include "../../include/StreamIO/BMPStream.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h" /* Included for Assertions */
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/TextIO/LogIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    void BMPParseMetadata(BMPOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        BitBuffer_Seek(BitB, 16);                           // Skip BMPMagic
        Options->FileSize = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32);
        BitBuffer_Seek(BitB, 32);                           // 2 16 bit Reserved fields
        Options->Offset                                      = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32);
        if (Options->Offset > 14) { // DIB Header
            Options->DIBSize = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32);
            if (Options->DIBSize >= 40) {
                Options->Width = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32);
                Options->Height = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32);
            } else if (Options->DIBSize == 12) {
                Options->Width = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 16);
                Options->Height = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 16);
            }
            BitBuffer_Seek(BitB, 16);                       // NumPlanes, always 1
            Options->BitDepth                                = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 16);
            Options->CompressionType                               = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32);
            Options->NumBytesUsedBySamples       = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32);
            Options->WidthPixelsPerMeter         = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32);
            Options->HeightPixelsPerMeter        = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32);
            Options->ColorsIndexed               = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32);
            Options->IndexedColorsUsed           = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32);
            if (Options->DIBSize >= 56) {
                Options->ColorSpaceType          = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32);
                if (Options->DIBSize >= 108) {
                    Options->XCoordinate         = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32);
                    Options->YCoordinate         = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32);
                    Options->ZCoordinate         = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32);
                    Options->RGamma              = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32);
                    Options->GGamma              = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32);
                    Options->BGamma              = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32);
                    if (Options->DIBSize >= 124) {
                        Options->ICCIntent       = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32);
                        Options->ICCSize         = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 32);
                        Options->ICCPayload      = calloc(Options->ICCSize, sizeof(uint8_t));
                        AssertIO(Options->ICCPayload != NULL);
                        for (uint32_t ICCByte = 0; ICCByte < Options->ICCSize; ICCByte++) {
                            Options->ICCPayload[ICCByte] = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 8);
                        }
                        BitBuffer_Seek(BitB, 32); // More Reserved data.
                                                  // Ok so when the Height is positive, the image is upside down, the bottom of the image is at the top of the file.
                    }
                }
            }
            BitBuffer_Seek(BitB, Options->DIBSize - BitBuffer_GetPosition(BitB)); // Skip the remaining bits.
        } else if (Options->DIBSize == 40 && (Options->CompressionType == BMP_BitFields || Options->CompressionType == BMP_RGBABitFields)) {
            Options->RMask = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, Options->BitDepth);
            Options->GMask = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, Options->BitDepth);
            Options->BMask = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, Options->BitDepth);
            if (Options->CompressionType == BMP_RGBABitFields) {
                Options->AMask = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, Options->BitDepth);
            }
        } else {
            BitBuffer_Seek(BitB, Bits2Bytes((Options->Offset - 14) - Options->DIBSize, RoundingType_Down));
        }
    }
    
    void BMPExtractImage(BMPOptions *Options, BitBuffer *BitB, ImageContainer *Image) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        AssertIO(Image != NULL);
        uint16_t BitDepth        = Options->BitDepth;
        uint32_t Width           = Options->Width;
        bool     IsUpsideDown    = (Options->Height & 0x80000000) >> 31;
        uint32_t Height          = AbsoluteI(Options->Height);
        ImageChannelMap *Map    = ImageContainer_GetChannelMap(Image);
        
        AssertIO(Options->CompressionType == BMP_RGB);
        
        if (BitDepth <= 8) {
            uint8_t ****Array  = (uint8_t****) ImageContainer_GetArray(Image);
            for (uint64_t View = 0ULL; View < ImageChannelMap_GetNumViews(Map); View++) {
                for (uint64_t W = 0ULL; W < Width; W++) {
                    for (uint64_t H = 0ULL; H < Height; H++) {
                        for (uint64_t Channel = 0ULL; Channel < 3; Channel++) {
                            Array[View][W][H][Channel] = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, Bits2Bytes(BitDepth, RoundingType_Up));
                        }
                    }
                }
            }
        } else if (Options->BitDepth > 8 && Options->BitDepth <= 16) {
            uint16_t ****Array = (uint16_t****) ImageContainer_GetArray(Image);
            for (uint64_t View = 0ULL; View < ImageChannelMap_GetNumViews(Map); View++) {
                for (uint64_t W = 0ULL; W < Width; W++) {
                    for (uint64_t H = 0ULL; H < Height; H++) {
                        for (uint64_t Channel = 0ULL; Channel < 3; Channel++) {
                            Array[View][W][H][Channel] = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, Bits2Bytes(BitDepth, RoundingType_Up));
                        }
                    }
                }
            }
        }
        
        if (IsUpsideDown) { // The Image is upside down, so we need to flip it
            ImageContainer_Flip(Image, FlipType_Vertical);
        }
        
        AssertIO(Options->ColorsIndexed == false);
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

