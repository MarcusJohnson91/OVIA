#include "../../include/Private/FLACCommon.h"
#include "../../../Dependencies/FoundationIO/Library/include/UnicodeIO/FormatIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    void   FLACWriteMetadata(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            BitBuffer_WriteBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 32, FLACMagic);
            bool IsLastMetadataBlock = false;
            BitBuffer_WriteBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 1, IsLastMetadataBlock);
            uint8_t MetadataBlockType = 1;
            BitBuffer_WriteBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 7, MetadataBlockType);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void FLAC_Encode(void *Options, void *Container, BitBuffer *BitB) {
        if (Options != NULL && Container != NULL && BitB != NULL) {
            FLACOptions *FLAC       = Options;
            Audio2DContainer *Audio = Container;
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
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (Container == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Audio2DContainer Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void FLAC_WriteStreamInfo(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            FLACOptions *FLAC = Options;
            BitBuffer_WriteBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 24, 34); // StreamInfoSize
            BitBuffer_WriteBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 16, FLAC->StreamInfo->MinimumBlockSize);
            BitBuffer_WriteBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 16, FLAC->StreamInfo->MaximumBlockSize);
            BitBuffer_WriteBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 24, FLAC->StreamInfo->MinimumFrameSize);
            BitBuffer_WriteBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 24, FLAC->StreamInfo->MaximumFrameSize);
            BitBuffer_WriteBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 20, FLAC->StreamInfo->SampleRate);
            BitBuffer_WriteBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit,  3, FLAC->StreamInfo->CodedChannels - 1);
            BitBuffer_WriteBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit,  5, FLAC->StreamInfo->BitDepth - 1);
            BitBuffer_WriteBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_LSBit, 36, FLAC->StreamInfo->SamplesInStream);
            BitBuffer_Seek(BitB, 128); // Room for the MD5
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void FLAC_WriteVorbis(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            BitBuffer_WriteBits(BitB, BitIO_ByteOrder_MSByte, BitIO_BitOrder_MSBit, 32, 4);
            UTF8 *OVIAVersion = UTF8_Format(UTF8String("OVIA"));
            BitBuffer_WriteUTF8(BitB, OVIAVersion, StringTerminator_Sized);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
#define NumFLACExtensions 1
    
    static const UTF32 *FLACExtensions[NumFLACExtensions] = {
        [0] = UTF32String("flac"),
    };
    
    static const OVIAEncoder FLACEncoder = {
        .EncoderID             = CodecID_FLAC,
        .MediaType             = MediaType_Audio2D,
        .NumExtensions         = NumFLACExtensions,
        .Extensions            = FLACExtensions,
        .Function_Initialize   = FLACOptions_Init,
        .Function_WriteHeader  = FLAC_WriteStreamInfo,
        .Function_Encode       = FLAC_Encode,
        .Function_Deinitialize = FLACOptions_Deinit,
    };
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
