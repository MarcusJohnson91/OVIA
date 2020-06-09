#include "../../include/Private/AIFCommon.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    void *AIFOptions_Init(void) {
        AIFOptions *AIF = calloc(1, sizeof(AIFOptions));
        return AIF;
    }
    
    void AIFSkipPadding(BitBuffer *BitB, uint32_t SubChunkSize) {
        if (IsOdd(SubChunkSize) == true) {
            BitBuffer_Seek(BitB, 8);
        }
    }
    
    void AIFOptions_Deinit(void *Options) {
        AIFOptions *AIF = Options;
        free(AIF->Tags);
        free(AIF->TagTypes);
        free(AIF);
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
