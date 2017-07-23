#include "../../include/libModernFLAC.h"
#include "../../include/FLACTypes.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void WriteBase64(BitOutput *BitO, uint8_t *Buffer, uint64_t BufferSize) {
        
    }
    
    void   FLACWriteMetadata(BitOutput *BitO, EncodeFLAC *Enc) {
        WriteBits(BitO, FLACMagic, 32, true);
        WriteBits(BitO, false, 1, true); // IsLastMetadataBlock
        //WriteBits(BitO, FLACMetadataTypes, 7); // MetadataBlockType
        
    }
    
    int8_t EncodeFLACFile(PCMFile *PCM, BitOutput *BitO, EncodeFLAC *Enc) {
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
