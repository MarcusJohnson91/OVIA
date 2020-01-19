#include "../../include/Private/ID3Common.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void ID3v2_Read(ID3v2 *ID3, BitBuffer *BitB) {
        if (ID3 != NULL && BitB != NULL) { // We assume the "ID3" marker has already been consumed
            // Read the Version variable
        } else if (ID3 == NULL) {
            Log(Log_DEBUG, __func__, UTF8String("ID3 Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
#ifdef __cplusplus
}
#endif
