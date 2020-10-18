#include "../../include/Private/StreamIO/PNMStream.h"
#include "../../../Dependencies/FoundationIO/Library/include/TextIO/LogIO.h"
#include "../../../Dependencies/FoundationIO/Library/include/MathIO.h"

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
