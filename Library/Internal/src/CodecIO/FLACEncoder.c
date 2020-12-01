#include "../../include/Private/CodecIO/FLACCodec.h"
#include "../../include/Private/Version.h"
#include "../../../Dependencies/FoundationIO/Library/include/TextIO/FormatIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    void FLAC_Encode(void *Options, Audio2DContainer *Audio, BitBuffer *BitB) {
        if (Options != NULL && Audio && BitB != NULL) {
            FLACOptions *FLAC = Options;
            if (FLAC->EncodeSubset == true && Audio2DContainer_GetSampleRate(Audio) <= 48000) {
                FLAC->StreamInfo->MaximumBlockSize = 4608;
                FLAC->Frame->Sub->LPCFilterOrder   = 12;
                FLAC->Frame->PartitionOrder        = 8;
            } else {
                FLAC->StreamInfo->MaximumBlockSize = 16384;
                FLAC->Frame->Sub->LPCFilterOrder   = 32;
                FLAC->Frame->PartitionOrder        = 15;
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (Audio == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Audio2DContainer Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void FLAC_Compose(void *Options, BitBuffer *BitB) {
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 32, FLACMagic);
        bool IsLastMetadataBlock = false;
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 1, IsLastMetadataBlock);
        FLAC_BlockTypes BlockType = BlockType_StreamInfo;
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 7, BlockType);
        FLAC_Write_StreamInfo(Options, BitB);
        // Write all the Metadata blocks, then start writing the actual audio in the FLAC_Append function
    }
    
    void FLAC_Write_StreamInfo(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            FLACOptions *FLAC = Options;
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 24, 34); // StreamInfoSize
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16, FLAC->StreamInfo->MinimumBlockSize);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16, FLAC->StreamInfo->MaximumBlockSize);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 24, FLAC->StreamInfo->MinimumFrameSize);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 24, FLAC->StreamInfo->MaximumFrameSize);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 20, FLAC->StreamInfo->CodedSampleRate);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest,  3, FLAC->StreamInfo->CodedSampleRate - 1);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest,  5, FLAC->StreamInfo->CodedBitDepth - 1);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 36, FLAC->StreamInfo->SamplesInStream);
            BitBuffer_Seek(BitB, 128); // Room for the MD5
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void FLAC_Write_Vorbis(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 32, 4);
            UTF8 *OVIAVersion = UTF8_Format(UTF8String("OVIA %d.%d.%d"), OVIA_Version_Major, OVIA_Version_Minor, OVIA_Version_Patch);
            BitBuffer_WriteUTF8(BitB, OVIAVersion, StringTerminator_Sized);
            free(OVIAVersion);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void FLAC_Write_Audio(void *Options, BitBuffer *BitB, Audio2DContainer *Audio) {
        // Take the Audio2DContainer, encode the contents, and write it out to the BitBuffer
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
