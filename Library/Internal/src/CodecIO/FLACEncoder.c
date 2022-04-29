#include "../../include/CodecIO/FLACCodec.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h" /* Included for Assertions */
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/TextIO/FormatIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/TextIO/StringIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    void FLAC_Encode(FLACOptions *Options, Audio2DContainer *Audio, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(Audio != NULL);
        AssertIO(BitB != NULL);
        if (Options->EncodeSubset == true && Audio2DContainer_GetSampleRate(Audio) <= 48000) {
            Options->StreamInfo->MaximumBlockSize = 4608;
            Options->Frame->Sub->LPCFilterOrder   = 12;
            Options->Frame->PartitionOrder        = 8;
        } else {
            Options->StreamInfo->MaximumBlockSize = 16384;
            Options->Frame->Sub->LPCFilterOrder   = 32;
            Options->Frame->PartitionOrder        = 15;
        }
    }

    void FLAC_Compose(FLACOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, FLACMagic);
        bool IsLastMetadataBlock = false;
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1, IsLastMetadataBlock);
        FLAC_BlockTypes BlockType = BlockType_StreamInfo;
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 7, BlockType);
        FLAC_Write_StreamInfo(Options, BitB);
        // Write all the Metadata blocks, then start writing the actual audio in the FLAC_Append function
    }
    
    void FLAC_Write_StreamInfo(FLACOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 24, 34); // StreamInfoSize
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16, Options->StreamInfo->MinimumBlockSize);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16, Options->StreamInfo->MaximumBlockSize);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 24, Options->StreamInfo->MinimumFrameSize);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 24, Options->StreamInfo->MaximumFrameSize);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 20, Options->StreamInfo->CodedSampleRate);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest,  3, Options->StreamInfo->CodedSampleRate - 1);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest,  5, Options->StreamInfo->CodedBitDepth - 1);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 36, Options->StreamInfo->SamplesInStream);
        BitBuffer_Seek(BitB, 128); // Room for the MD5
    }
    
    void FLAC_Write_Vorbis(FLACOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 32, 4);
        UTF8 *OVIAVersion = UTF8_Format(UTF8String("OVIA %d.%d.%d\n"), OVIA_Version_Major, OVIA_Version_Minor, OVIA_Version_Patch);
        BitBuffer_WriteUTF8(BitB, OVIAVersion, StringTerminator_Sized);
        UTF8_Deinit(OVIAVersion);
    }

    void FLAC_Write_Audio(FLACOptions *Options, BitBuffer *BitB, Audio2DContainer *Audio) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        AssertIO(Audio != NULL);
        // Take the Audio2DContainer, encode the contents, and write it out to the BitBuffer
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
