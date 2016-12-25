#include "../include/EncodeFLAC.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void FLACEncodeLPC(FLACFile *FLAC, int64_t *RAWSample[FLAC->Frame->BlockSize], uint8_t PredictorCoefficents, int8_t Shift) {
        int32_t PreviousSample = 0, CurrentSample = 0, EncodedSample = 0;
        for (uint16_t Sample = 0; Sample < FLAC->Frame->BlockSize; Sample++) {
            CurrentSample    =  RAWSample[Sample];
            EncodedSample    = (RAWSample[Sample] - PreviousSample) * PredictorCoefficents;
            PreviousSample   =  RAWSample[Sample];
        }
    }
    
#ifdef __cplusplus
}
#endif
