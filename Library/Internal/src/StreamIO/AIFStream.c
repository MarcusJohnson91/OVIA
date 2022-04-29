#include "../../include/StreamIO/AIFStream.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
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
    
    void AIFOptions_Deinit(AIFOptions *Options) {
        free(Options->Tags);
        free(Options->TagTypes);
        free(Options);
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
