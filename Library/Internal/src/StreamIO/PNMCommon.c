#include "../../include/Private/PNMCommon.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
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
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
