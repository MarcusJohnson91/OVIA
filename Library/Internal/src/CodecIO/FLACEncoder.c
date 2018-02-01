#include "../../include/Private/Encode/libModernFLAC_Encode.h"

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
    
#ifdef __cplusplus
}
#endif
