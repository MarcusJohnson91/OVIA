#include "../../include/StreamIO/TIFStream.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/TextIO/StringIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    /*
     Nikon specific tag: XMP metadata
     real size: 32768 bytes
     FieldTag: 0xD28C aka 53,900
     FieldType: 0x0300 aka 3
     FieldCount: 0x488D0300 aka 232776
     ValueOffset: 0xC4900300 aka 233668
     */

    static CharSet8 *TIF_ReadASCII(TIFOptions *Options, BitBuffer *BitB) {
        // The value of the Count part of an ASCII field entry includes the NUL
        size_t NumCodeUnits = BitBuffer_GetUTF8StringSize(BitB);
        CharSet8 *String = UTF8_Init(NumCodeUnits);
        AssertIO(String != NULL);
        for (size_t CodeUnit = 0; CodeUnit < NumCodeUnits; CodeUnit++) {
            String[CodeUnit] = BitBuffer_ReadBits(BitB, Options->ByteOrder, BitOrder_MSBitIsRight, 8);
        }
        return String;
    }

    static double TIF_ReadFraction(TIFOptions *Options, BitBuffer *BitB) {
        int32_t Numerator   = BitBuffer_ReadBits(BitB, Options->ByteOrder, BitOrder_MSBitIsRight, 32);
        int32_t Denominator = BitBuffer_ReadBits(BitB, Options->ByteOrder, BitOrder_MSBitIsRight, 32);
        AssertIO(Numerator != 0);
        AssertIO(Denominator != 0);
        return Numerator / Denominator;
    }

    static void TIF_ReadOldSubFileType(TIFOptions *Options, BitBuffer *BitB) {
        // read a 32 bit flag of the purpose of this subfile
        uint16_t Flag = BitBuffer_ReadBits(BitB, Options->ByteOrder, BitOrder_MSBitIsRight, 16);
    }

    static void TIF_ReadNewSubFileType(TIFOptions *Options, BitBuffer *BitB) {
        // read a 32 bit flag of the purpose of this subfile
        uint32_t Flag = BitBuffer_ReadBits(BitB, Options->ByteOrder, BitOrder_MSBitIsRight, 32);
    }

    static void DePackBits(TIFOptions *Options, BitBuffer *BitB) {
        // Read HeaderByte
        int8_t HeaderByte = BitBuffer_ReadBits(BitB, Options->ByteOrder, BitOrder_MSBitIsRight, 8);
        if (HeaderByte >= 0) { // Copy HeaderByte + 1 bytes of data

        } else if (HeaderByte < 0) { // Repeat the byte 1 - HeaderByte times

        } else if (HeaderByte == -128) {
            BitBuffer_Seek(BitB, 8); // Skip, checking the next byte
        }
    }
    
    void TIF_ReadHeader(TIFOptions *Options, BitBuffer *BitB) {
        // Read 2 bytes, depending on byte order, should be at least 42 aka 0x2A00 for LE
        uint16_t ByteOrder        = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsRight, BitOrder_MSBitIsRight, 16); // II aka ii
        if (ByteOrder == TIFF_ByteOrder_LE) {
            Options->ByteOrder    = ByteOrder_MSByteIsRight;
        } else if (ByteOrder == TIFF_ByteOrder_BE) {
            Options->ByteOrder    = ByteOrder_MSByteIsLeft;
        }
        Options->Version          = BitBuffer_ReadBits(BitB, Options->ByteOrder, BitOrder_MSBitIsRight, 16); // 42
        Options->IFD1Offset       = BitBuffer_ReadBits(BitB, Options->ByteOrder, BitOrder_MSBitIsRight, 32) - 8; // 0
        BitBuffer_Seek(BitB, Bytes2Bits(Options->IFD1Offset));
        Options->NumIFDEntries   = BitBuffer_ReadBits(BitB, Options->ByteOrder, BitOrder_MSBitIsRight, 16); // 22
        Options->IFDEntries      = calloc(Options->NumIFDEntries, sizeof(TIFFIFDEntry));
        AssertIO(Options->IFDEntries != NULL);
        for (uint16_t IFDEntry = 0; IFDEntry < Options->NumIFDEntries; IFDEntry++) {
            Options->IFDEntries[IFDEntry].FieldTag    = BitBuffer_ReadBits(BitB, Options->ByteOrder, BitOrder_MSBitIsRight, 16); // 254
            Options->IFDEntries[IFDEntry].FieldType   = BitBuffer_ReadBits(BitB, Options->ByteOrder, BitOrder_MSBitIsRight, 16); // 4
            Options->IFDEntries[IFDEntry].FieldCount  = BitBuffer_ReadBits(BitB, Options->ByteOrder, BitOrder_MSBitIsRight, 32); // 1
            Options->IFDEntries[IFDEntry].ValueOffset = BitBuffer_ReadBits(BitB, Options->ByteOrder, BitOrder_MSBitIsRight, 32); // 1
            switch (Options->IFDEntries[IFDEntry].FieldTag) {
                case TIFFTag_NewSubFileType:
                    TIF_ReadNewSubFileType(Options, BitB);
                    break;
                case TIFFTag_ImageWidth:
                    
                    break;
                case TIFFTag_ImageLength:
                    
                    break;
                case TIFFTag_BitsPerSample:
                    
                    break;
                case TIFFTag_Compression:
                    
                    break;
                case TIFFTag_PhotometricIntrepretation:
                    
                    break;
                case TIFFTag_ImageDescription:
                    
                    break;
                case TIFFTag_Make:
                    
                    break;
                case TIFFTag_Model:
                    
                    break;
                case TIFFTag_StripOffsets:
                    
                    break;
                case TIFFTag_Orientation:
                    
                    break;
                case TIFFTag_SamplesPerPixel:
                    
                    break;
                case TIFFTag_RowsPerStrip:
                    
                    break;
                case TIFFTag_StripByteCounts:
                    
                    break;
                case TIFFTag_XResolution:
                    
                    break;
                case TIFFTag_YResolution:
                    
                    break;
                case TIFFTag_PlanarConfiguration:
                    
                    break;
                case TIFFTag_ResolutionUnit:
                    
                    break;
                case TIFFTag_Software:
                    
                    break;
                case TIFFTag_DateTime:
                    
                    break;
                case TIFFTag_Artist:
                    
                    break;
                case TIFFTag_TileWidth:
                    
                    break;
                case TIFFTag_TileLength:
                    
                    break;
                case TIFFTag_TileOffsets:
                    
                    break;
                case TIFFTag_TileByteCounts:
                    
                    break;
                case TIFFTag_SubIFDs:
                    
                    break;
                case TIFFTag_JPEGTables:
                    
                    break;
                case TIFFTag_YCbCrCoefficients:
                    
                    break;
                case TIFFTag_YCbCrSubsampling:
                    
                    break;
                case TIFFTag_YCbCrPositioning:
                    
                    break;
                case TIFFTag_ReferenceBlackWhite:
                    
                    break;
                case TIFFTag_CFARepeatPatternDim:
                    
                    break;
                case TIFFTag_CFAPattern:
                    
                    break;
                case TIFFTag_BatteryLevel:
                    
                    break;
                case TIFFTag_Copyright:
                    
                    break;
                case TIFFTag_ExposureTime:
                    
                    break;
                case TIFFTag_FNumber:
                    
                    break;
                case TIFFTag_IPTC_NAA:
                    
                    break;
                case TIFFTag_InterColorProfile:
                    
                    break;
                case TIFFTag_ExposureProgram:
                    
                    break;
                case TIFFTag_SpectralSensitivity:
                    
                    break;
                case TIFFTag_GPSInfo:
                    
                    break;
                case TIFFTag_ISOSpeedRatings:
                    
                    break;
                case TIFFTag_OECF:
                    
                    break;
                case TIFFTag_Interlace:
                    
                    break;
                case TIFFTag_TimeZoneOffset:
                    
                    break;
                case TIFFTag_SelfTimerMode:
                    
                    break;
                case TIFFTag_DateTimeOriginal:
                    
                    break;
                case TIFFTag_CompressedBitsPerPixel:
                    
                    break;
                case TIFFTag_ShutterSpeedValue:
                    
                    break;
                case TIFFTag_ApartureValue:
                    
                    break;
                case TIFFTag_BrightnessValue:
                    
                    break;
                case TIFFTag_ExposureBiasValue:
                    
                    break;
                case TIFFTag_MaxApertureValue:
                    
                    break;
                case TIFFTag_SubjectDistance:
                    
                    break;
                case TIFFTag_MeteringMode:
                    
                    break;
                case TIFFTag_LightSource:
                    
                    break;
                case TIFFTag_Flash:
                    
                    break;
                case TIFFTag_FocalLength:
                    
                    break;
                case TIFFTag_FlashEnergy:
                    
                    break;
                case TIFFTag_SpatialFrequencyResponse:
                    
                    break;
                case TIFFTag_Noise:
                    
                    break;
                case TIFFTag_FocalPlaneXResolution:
                    
                    break;
                case TIFFTag_FocalPlaneYResolution:
                    
                    break;
                case TIFFTag_FocalPlaneResolutionUnit:
                    
                    break;
                case TIFFTag_ImageNumber:
                    
                    break;
                case TIFFTag_SecurityClassification:
                    
                    break;
                case TIFFTag_ImageHistory:
                    
                    break;
                case TIFFTag_SubjectLocation:
                    
                    break;
                case TIFFTag_ExposureIndex:
                    
                    break;
                case TIFFTag_TIFFStandardID:
                    
                    break;
                case TIFFTag_SensingMethod:
                    
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
