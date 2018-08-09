#include "../../../include/Private/Audio/FLACCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void DeinitFLACEncoder(EncodeFLAC *Enc) {
        free(Enc->Meta->StreamInfo);
        free(Enc->Meta->Seek);
        free(Enc->Meta->Vorbis);
        free(Enc->Meta->Cue);
        free(Enc->Meta->Pic);
        free(Enc->Meta);
        free(Enc->Data->Frame);
        free(Enc->Data->SubFrame);
        free(Enc->Data->LPC);
        free(Enc->Data->Rice);
        free(Enc->Data);
        free(Enc);
    }
    
    void WriteBase64(BitBuffer *OutputFLAC, uint8_t *Buffer, uint64_t BufferSize) {
        
    }
    
    void   FLACWriteMetadata(BitBuffer *OutputFLAC, EncodeFLAC *Enc) {
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputFLAC, 32, FLACMagic);
        bool IsLastMetadataBlock = No;
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputFLAC, 1, IsLastMetadataBlock);
        uint8_t MetadataBlockType = 1;
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputFLAC, 7, MetadataBlockType);
    }
    
    int8_t EncodeFLACFile(PCMFile *PCM, BitBuffer *OutputFLAC, EncodeFLAC *Enc) {
        if (Enc->EncodeSubset == true && Enc->Data->Frame->SampleRate <= 48000) {
            Enc->MaxBlockSize          =  4608;
            Enc->MaxFilterOrder        =    12;
            Enc->MaxRICEPartitionOrder =     8;
        } else {
            Enc->MaxBlockSize          = 16384;
            Enc->MaxFilterOrder        =    32;
            Enc->MaxRICEPartitionOrder =    15;
        }
        
        if (Enc->OptimizeFile == true) {
            // Optimize this mufucka

        }
        
        return EXIT_SUCCESS;
    }
    
    void   FLACWriteStreaminfo(BitBuffer *OutputFLAC, EncodeFLAC *Enc) {
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputFLAC, 24, 34); // StreamInfoSize
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputFLAC, 16, Enc->Meta->StreamInfo->MinimumBlockSize);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputFLAC, 16, Enc->Meta->StreamInfo->MaximumBlockSize);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputFLAC, 24, Enc->Meta->StreamInfo->MinimumFrameSize);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputFLAC, 24, Enc->Meta->StreamInfo->MaximumFrameSize);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputFLAC, 20, Enc->Meta->StreamInfo->SampleRate);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputFLAC, 24, Enc->Meta->StreamInfo->MinimumFrameSize);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputFLAC,  3, Enc->Meta->StreamInfo->Channels - 1);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputFLAC,  5, Enc->Meta->StreamInfo->BitDepth - 1);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputFLAC, 36, Enc->Meta->StreamInfo->SamplesInStream);
        WriteGUUID(BitIOBinaryGUID, OutputFLAC, 0x00000000000000000000000000000000); // ROOM for the MD5.
    }
    
    void   FLACWriteVorbisComment(BitBuffer *OutputFLAC, EncodeFLAC *Enc) {
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputFLAC, 32, Enc->Meta->Vorbis->VendorTagSize);
        for (uint32_t TagByte = 0; TagByte < Enc->Meta->Vorbis->VendorTagSize; TagByte++) {
            WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputFLAC, 8, Enc->Meta->Vorbis->VendorTag[TagByte]);
        }
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputFLAC, 32, Enc->Meta->Vorbis->NumUserTags);
        for (uint32_t UserTag = 0; UserTag < Enc->Meta->Vorbis->NumUserTags; UserTag++) {
            WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputFLAC, 8, Enc->Meta->Vorbis->UserTagSize);
        }
    }
    
#ifdef __cplusplus
}
#endif
