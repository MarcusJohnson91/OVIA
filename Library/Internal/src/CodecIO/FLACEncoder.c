#include "../../include/libModernFLAC.h"
#include "../../include/Private/ModernFLACTypes.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void WriteBase64(BitBuffer *OutputFLAC, uint8_t *Buffer, uint64_t BufferSize) {
        
    }
    
    void   FLACWriteMetadata(BitBuffer *OutputFLAC, EncodeFLAC *Enc) {
        WriteBits(OutputFLAC, FLACMagic, 32, true);
        WriteBits(OutputFLAC, false, 1, true); // IsLastMetadataBlock
        //WriteBits(OutputFLAC, FLACMetadataTypes, 7); // MetadataBlockType
        
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
