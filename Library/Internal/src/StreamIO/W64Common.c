#include "../../include/Private/W64Common.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    W64Options *W64Options_Init(void) {
        W64Options *W64 = calloc(1, sizeof(W64Options));
        return W64;
    }
    
    static uint64_t CalculateW64ByteRate(uint64_t NumChannels, uint8_t BitDepth, uint64_t SampleRate) {
        return NumChannels * SampleRate * (BitDepth / 8);
    }
    
    static uint64_t CalculateW64BlockAlign(uint64_t NumChannels, uint8_t BitDepth) {
        return NumChannels * (BitDepth / 8);
    }
    
    void W64Options_Deinit(W64Options *W64) {
        free(W64);
    }
    
#ifdef __cplusplus
}
#endif
