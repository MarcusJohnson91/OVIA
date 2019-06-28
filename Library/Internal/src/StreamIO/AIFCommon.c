#include "../../include/Private/AIFCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    AIFOptions *AIFOptions_Init(void) {
        AIFOptions *AIF = calloc(1, sizeof(AIFOptions));
        return AIF;
    }
    
    void AIFSkipPadding(BitBuffer *BitB, uint32_t SubChunkSize) {
        if (IsOdd(SubChunkSize) == true) {
            BitBuffer_Seek(BitB, 8);
        }
    }
    
    void AIFOptions_Deinit(AIFOptions *AIF) {
        free(AIF);
    }
    
#ifdef __cplusplus
}
#endif
