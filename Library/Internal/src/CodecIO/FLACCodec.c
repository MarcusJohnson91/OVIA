#include "../../include/Private/Common/libModernFLAC_Common.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    inline int64_t OnesCompliment2TwosCompliment(const int64_t OnesCompliment) {
        return ~OnesCompliment + 1;
    }
    
    inline int64_t TwosCompliment2OnesCompliment(const int64_t TwosCompliment) {
        return TwosCompliment ^ 0xFFFFFFFFFFFFFFFF;
    }
    
#ifdef __cplusplus
}
#endif
