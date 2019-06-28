#include "../../include/Private/WAVCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    WAVOptions *WAVOptions_Init(void) {
        WAVOptions *WAV = calloc(1, sizeof(WAVOptions));
        return WAV;
    }
    
    void WAVOptions_Deinit(WAVOptions *WAV) {
        free(WAV);
    }
    
#ifdef __cplusplus
}
#endif
