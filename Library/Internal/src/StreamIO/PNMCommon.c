#include "../../include/Private/PNMCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void *PNMOptions_Init(void) {
        PNMOptions *PNM = calloc(1, sizeof(PNMOptions));
        return PNM;
    }
    
    void PNMOptions_Deinit(PNMOptions *PNM) {
        free(PNM);
    }
    
#ifdef __cplusplus
}
#endif
