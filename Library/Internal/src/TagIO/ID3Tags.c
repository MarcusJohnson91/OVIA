#include "../../include/Private/ID3Common.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void ID3v2_Read(ID3 *Id3, BitBuffer *BitB) {
        if (Id3 != NULL && BitB != NULL) { // We assume the "ID3" marker has already been consumed
            // Read the Version variable
        } else if (Id3 == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("ID3 Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
#ifdef __cplusplus
}
#endif
