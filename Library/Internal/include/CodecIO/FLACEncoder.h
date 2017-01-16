#include "libModernFLAC.h"
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
    
    int8_t EncodeFLAC(BitInput *BitI, BitOutput *BitO, FLACEncoder *FLAC, bool CreateSubsetStream);

#ifdef __cplusplus
}
#endif
