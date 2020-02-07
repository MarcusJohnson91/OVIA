#include "../../include/Private/FLACCommon.h"
#include "../../../Dependencies/FoundationIO/Library/include/UnicodeIO/FormatIO.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void   FLACWriteMetadata(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 32, FLACMagic);
            bool IsLastMetadataBlock = false;
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 1, IsLastMetadataBlock);
            uint8_t MetadataBlockType = 1;
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 7, MetadataBlockType);
        } else if (Options == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
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
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Options Pointer is NULL"));
        } else if (Container == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Audio2DContainer Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void FLAC_WriteStreamInfo(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            FLACOptions *FLAC = Options;
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 24, 34); // StreamInfoSize
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 16, FLAC->StreamInfo->MinimumBlockSize);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 16, FLAC->StreamInfo->MaximumBlockSize);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 24, FLAC->StreamInfo->MinimumFrameSize);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 24, FLAC->StreamInfo->MaximumFrameSize);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 20, FLAC->StreamInfo->CodedSampleRate);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst,  3, FLAC->StreamInfo->CodedSampleRate - 1);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst,  5, FLAC->StreamInfo->CodedBitDepth - 1);
            BitBuffer_WriteBits(BitB, MSByteFirst, LSBitFirst, 36, FLAC->StreamInfo->SamplesInStream);
            BitBuffer_Seek(BitB, 128); // Room for the MD5
        } else if (Options == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    void FLAC_WriteVorbis(void *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            BitBuffer_WriteBits(BitB, MSByteFirst, MSBitFirst, 32, 4);
            UTF8 *OVIAVersion = UTF8_Format(UTF8String("OVIA"));
            BitBuffer_WriteUTF8(BitB, OVIAVersion, WriteType_Sized);
        } else if (Options == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
    static void RegisterEncoder_FLAC(OVIA *Ovia) {
        Ovia->NumEncoders                                    += 1;
        uint64_t EncoderIndex                                 = Ovia->NumEncoders;
        Ovia->Encoders                                        = realloc(Ovia->Encoders, sizeof(OVIAEncoder) * Ovia->NumEncoders);
        
        Ovia->Encoders[EncoderIndex].EncoderID                = CodecID_FLAC;
        Ovia->Encoders[EncoderIndex].MediaType                = MediaType_Audio2D;
        Ovia->Encoders[EncoderIndex].Function_Initialize[0]   = FLACOptions_Init;
        Ovia->Encoders[EncoderIndex].Function_WriteHeader[0]  = FLAC_WriteStreamInfo; // Make a new function that wrap up all this stuff called WriteHeader
        Ovia->Encoders[EncoderIndex].Function_Encode[0]       = FLAC_Encode;
        Ovia->Encoders[EncoderIndex].Function_WriteFooter[0]  = NULL;
        Ovia->Encoders[EncoderIndex].Function_Deinitialize[0] = FLACOptions_Deinit;
    }
    
    static OVIACodecRegistry Register_FLACEncoder = {
        .Function_RegisterEncoder[CodecID_FLAC - 1]   = RegisterEncoder_FLAC,
    };
    
#ifdef __cplusplus
}
#endif
