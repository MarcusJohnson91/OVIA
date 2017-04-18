#include "../include/PNGShared.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    uint32_t CalculateSTERPadding(const uint32_t Width) {
        return 7 - ((Width - 1) % 8);
    }
    
#ifdef __cplusplus
}
#endif
