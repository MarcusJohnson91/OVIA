#include "../include/EncodeFLAC.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void   InitFLACEncoder(FLACEncoder *FLAC) {
        FLAC->Meta             = calloc(sizeof(FLACMeta), 1);
        FLAC->Meta->StreamInfo = calloc(sizeof(FLACStreamInfo), 1);
        FLAC->Meta->Seek       = calloc(sizeof(FLACSeekTable), 1);
        FLAC->Meta->Vorbis     = calloc(sizeof(FLACVorbisComment), 1);
        FLAC->Meta->Cue        = calloc(sizeof(FLACCueSheet), 1);
        FLAC->Meta->Pic        = calloc(sizeof(FLACPicture), 1);
        
        FLAC->Data             = calloc(sizeof(FLACData), 1);
        FLAC->Data->Frame      = calloc(sizeof(FLACFrame), 1);
        FLAC->Data->SubFrame   = calloc(sizeof(FLACSubFrame), 1);
        FLAC->Data->LPC        = calloc(sizeof(FLACLPC), 1);
        FLAC->Data->Rice       = calloc(sizeof(RICEPartition), 1);
    }
    
    int8_t EncodeFLAC(BitInput *BitI, BitOutput *BitO, FLACEncoder *FLAC) {
        if (FLAC->EncodeSubset == true && FLAC->Data->Frame->SampleRate <= 48000) {
            FLAC->MaxBlockSize          =  4608;
            FLAC->MaxFilterOrder        =    12;
            FLAC->MaxRICEPartitionOrder =     8;
        } else {
            FLAC->MaxBlockSize          = 16384;
            FLAC->MaxFilterOrder        =    32;
            FLAC->MaxRICEPartitionOrder =    15;
        }
        
        if (FLAC->OptimizeFile == true) {
            // Optimize this mufucka
#pragma omp parallel
            for (uint8_t Coeff = 0; Coeff < 32; Coeff++) {
                
            }
        }
        
        return 0;
    }
    
#ifdef __cplusplus
}
#endif
