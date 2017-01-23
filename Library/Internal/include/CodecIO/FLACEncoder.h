#include "libModernFLAC.h"
#include "/usr/local/Packages/libPCM/include/libPCM.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef struct FLACEncoder {
        bool      EncodeSubset;
        bool      OptimizeFile;
        uint16_t  MaxBlockSize;
        uint8_t   MaxFilterOrder;
        uint8_t   MaxRICEPartitionOrder;
        FLACMeta *Meta;
        FLACData *Data;
        int64_t   RawSamples[FLACMaxSamplesInBlock];
    } FLACEncoder;
    
    void   InitFLACEncoder(FLACEncoder *FLAC);
    
    int8_t EncodeFLAC(PCMFile *PCM, BitOutput *BitO, FLACEncoder *FLAC);

#ifdef __cplusplus
}
#endif
