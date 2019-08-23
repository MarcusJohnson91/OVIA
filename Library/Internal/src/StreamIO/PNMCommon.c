#include "../../include/Private/PNMCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void *PNMOptions_Init(void) {
        void *Options = calloc(1, sizeof(PNMOptions));
        return Options;
    }
    
    void PNMOptions_Deinit(void *Options) {
        PNMOptions *PNM = Options;
        free(PNM);
    }
    
#ifdef __cplusplus
}
#endif
