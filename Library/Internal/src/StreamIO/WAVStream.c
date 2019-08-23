#include "../../include/Private/WAVCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void *WAVOptions_Init(void) {
        void *Options = calloc(1, sizeof(WAVOptions));
        return Options;
    }
    
    void WAVSkipPadding(BitBuffer *BitB, uint32_t SubChunkSize) {
        if (IsOdd(SubChunkSize) == true) {
            BitBuffer_Seek(BitB, 8);
        }
    }
    
    void WAVOptions_Deinit(void *Options) {
        WAVOptions *WAV = Options;
        free(WAV);
    }
    
#ifdef __cplusplus
}
#endif
