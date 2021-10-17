#include "../../../include/Private/StreamIO/TIFFStream.h"
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/TextIO/LogIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    void TIFF_ReadHeader(void *Options, BitBuffer *BitB) {
        // Read 2 bytes, depending on byte order, should be at least 42 aka 0x2A00 for LE
        TIFFOptions *TIFF        = Options;
        uint16_t       ByteOrder = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsNearest, 16); // LL
        if (ByteOrder == TIFF_ByteOrder_LE) {
            TIFF->ByteOrder      = ByteOrder_LSByteIsNearest;
        } else if (ByteOrder == TIFF_ByteOrder_BE) {
            TIFF->ByteOrder      = ByteOrder_LSByteIsFarthest;
        }
        TIFF->Version          = BitBuffer_ReadBits(BitB, TIFF->ByteOrder, BitOrder_LSBitIsNearest, 16); // 42
        TIFF->IFD1Offset       = BitBuffer_ReadBits(BitB, TIFF->ByteOrder, BitOrder_LSBitIsNearest, 32) - 8; // 0
        BitBuffer_Seek(BitB, Bytes2Bits(TIFF->IFD1Offset));
        TIFF->NumIFDEntries   = BitBuffer_ReadBits(BitB, TIFF->ByteOrder, BitOrder_LSBitIsNearest, 16); // 54
        TIFF->IFDEntries      = calloc(TIFF->NumIFDEntries, sizeof(TIFFIFDEntry));
        for (uint16_t IFDEntry = 0; IFDEntry < TIFF->NumIFDEntries; IFDEntry++) {
            TIFF->IFDEntries[IFDEntry].FieldTag    = BitBuffer_ReadBits(BitB, TIFF->ByteOrder, BitOrder_LSBitIsNearest, 16); // 254
            TIFF->IFDEntries[IFDEntry].FieldType   = BitBuffer_ReadBits(BitB, TIFF->ByteOrder, BitOrder_LSBitIsNearest, 16); // 4
            TIFF->IFDEntries[IFDEntry].FieldCount  = BitBuffer_ReadBits(BitB, TIFF->ByteOrder, BitOrder_LSBitIsNearest, 32); // 1
            TIFF->IFDEntries[IFDEntry].ValueOffset = BitBuffer_ReadBits(BitB, TIFF->ByteOrder, BitOrder_LSBitIsNearest, 32); // 1
            switch (TIFF->IFDEntries[IFDEntry].FieldTag) {
                case TIFFTag_NewSubFileType:
                    <#code#>
                    break;
                case TIFFTag_ImageWidth:
                    <#code#>
                    break;
                case TIFFTag_ImageLength:
                    <#code#>
                    break;
                case TIFFTag_BitsPerSample:
                    <#code#>
                    break;
                case TIFFTag_Compression:
                    <#code#>
                    break;
                case TIFFTag_PhotometricIntrepretation:
                    <#code#>
                    break;
                case TIFFTag_ImageDescription:
                    <#code#>
                    break;
                case TIFFTag_Make:
                    <#code#>
                    break;
                case TIFFTag_Model:
                    <#code#>
                    break;
                case TIFFTag_StripOffsets:
                    <#code#>
                    break;
                case TIFFTag_Orientation:
                    <#code#>
                    break;
                case TIFFTag_SamplesPerPixel:
                    <#code#>
                    break;
                case TIFFTag_RowsPerStrip:
                    <#code#>
                    break;
                case TIFFTag_StripByteCounts:
                    <#code#>
                    break;
                case TIFFTag_XResolution:
                    <#code#>
                    break;
                case TIFFTag_YResolution:
                    <#code#>
                    break;
                case TIFFTag_PlanarConfiguration:
                    <#code#>
                    break;
                case TIFFTag_ResolutionUnit:
                    <#code#>
                    break;
                case TIFFTag_Software:
                    <#code#>
                    break;
                case TIFFTag_DateTime:
                    <#code#>
                    break;
                case TIFFTag_Artist:
                    <#code#>
                    break;
                case TIFFTag_TileWidth:
                    <#code#>
                    break;
                case TIFFTag_TileLength:
                    <#code#>
                    break;
                case TIFFTag_TileOffsets:
                    <#code#>
                    break;
                case TIFFTag_TileByteCounts:
                    <#code#>
                    break;
                case TIFFTag_SubIFDs:
                    <#code#>
                    break;
                case TIFFTag_JPEGTables:
                    <#code#>
                    break;
                case TIFFTag_YCbCrCoefficients:
                    <#code#>
                    break;
                case TIFFTag_YCbCrSubsampling:
                    <#code#>
                    break;
                case TIFFTag_YCbCrPositioning:
                    <#code#>
                    break;
                case TIFFTag_ReferenceBlackWhite:
                    <#code#>
                    break;
                case TIFFTag_CFARepeatPatternDim:
                    <#code#>
                    break;
                case TIFFTag_CFAPattern:
                    <#code#>
                    break;
                case TIFFTag_BatteryLevel:
                    <#code#>
                    break;
                case TIFFTag_Copyright:
                    <#code#>
                    break;
                case TIFFTag_ExposureTime:
                    <#code#>
                    break;
                case TIFFTag_FNumber:
                    <#code#>
                    break;
                case TIFFTag_IPTC_NAA:
                    <#code#>
                    break;
                case TIFFTag_InterColorProfile:
                    <#code#>
                    break;
                case TIFFTag_ExposureProgram:
                    <#code#>
                    break;
                case TIFFTag_SpectralSensitivity:
                    <#code#>
                    break;
                case TIFFTag_GPSInfo:
                    <#code#>
                    break;
                case TIFFTag_ISOSpeedRatings:
                    <#code#>
                    break;
                case TIFFTag_OECF:
                    <#code#>
                    break;
                case TIFFTag_Interlace:
                    <#code#>
                    break;
                case TIFFTag_TimeZoneOffset:
                    <#code#>
                    break;
                case TIFFTag_SelfTimerMode:
                    <#code#>
                    break;
                case TIFFTag_DateTimeOriginal:
                    <#code#>
                    break;
                case TIFFTag_CompressedBitsPerPixel:
                    <#code#>
                    break;
                case TIFFTag_ShutterSpeedValue:
                    <#code#>
                    break;
                case TIFFTag_ApartureValue:
                    <#code#>
                    break;
                case TIFFTag_BrightnessValue:
                    <#code#>
                    break;
                case TIFFTag_ExposureBiasValue:
                    <#code#>
                    break;
                case TIFFTag_MaxApertureValue:
                    <#code#>
                    break;
                case TIFFTag_SubjectDistance:
                    <#code#>
                    break;
                case TIFFTag_MeteringMode:
                    <#code#>
                    break;
                case TIFFTag_LightSource:
                    <#code#>
                    break;
                case TIFFTag_Flash:
                    <#code#>
                    break;
                case TIFFTag_FocalLength:
                    <#code#>
                    break;
                case TIFFTag_FlashEnergy:
                    <#code#>
                    break;
                case TIFFTag_SpatialFrequencyResponse:
                    <#code#>
                    break;
                case TIFFTag_Noise:
                    <#code#>
                    break;
                case TIFFTag_FocalPlaneXResolution:
                    <#code#>
                    break;
                case TIFFTag_FocalPlaneYResolution:
                    <#code#>
                    break;
                case TIFFTag_FocalPlaneResolutionUnit:
                    <#code#>
                    break;
                case TIFFTag_ImageNumber:
                    <#code#>
                    break;
                case TIFFTag_SecurityClassification:
                    <#code#>
                    break;
                case TIFFTag_ImageHistory:
                    <#code#>
                    break;
                case TIFFTag_SubjectLocation:
                    <#code#>
                    break;
                case TIFFTag_ExposureIndex:
                    <#code#>
                    break;
                case TIFFTag_TIFFStandardID:
                    <#code#>
                    break;
                case TIFFTag_SensingMethod:
                    <#code#>
                    break;
            }
            /*
             0: 254, 4, 1, 1 = TIFFTag_NewSubFieldType, uint32, 1, 1 = Thumbnail.
             1: 256, 4, 1, 0 = TIFFTag_ImageWidth, uint32, 1, 0
             2: 257, 4, 1, 171 = TIFFTag_ImageLength, uint32, 1, 171
             3: 258, 3, 3, 662, = TIFFTag_BitsPerSample, uint16, 1, 662
             4: 259, 3, 1, 1 = TIFFTag_Compression
             5: 262, 3, 1, 2, = TIFFTag_PhotometricIntrepretation
             6: 271, 2, 6, 668 = TIFFTag_Make, String, 6, 668
             7: 272, 2, 23, 674, = TIFFTag_Model, String, 23, 674
             8: 273, 4, 1, 6241370
             9: 274, 3, 1, 1
             10: 277, 3, 1, 3
             11: 278, 4, 1, 171
             12: 279, 4, 1, 131328
             13: 284, 3, 1, 1
             14: 306, 2, 20, 698,
             15: 315, 2, 8, 718,
             16: 330, 4, 2, 726,
             17: 34665, 4, 1, 6241016
             18: 34858, 8, 2, -196612
             19: 37393, 4, 1, 176
             */
        }
        BitBuffer_Seek(BitB, Bytes2Bits(4)); // 4 bytes of 0's after the IFD
    }


#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
