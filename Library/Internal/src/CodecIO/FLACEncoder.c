#include "../include/EncodeFLAC.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void FLACEncodeLPC(FLACFile *FLAC, int64_t *RAWAudio[FLACMaxSamplesInBlock], uint8_t PredictorCoefficents, int8_t Shift) {
        int64_t EncodedSample = 0;
        for (uint16_t Sample = 0; Sample < FLAC->Data->Frame->BlockSize; Sample++) {
            EncodedSample    = (RAWAudio[Sample] - RAWAudio[Sample - 1]) * PredictorCoefficents;
        }
    }
    
#ifdef __cplusplus
}
#endif
