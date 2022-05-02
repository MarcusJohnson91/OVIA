#include "../../include/StreamIO/PNMStream.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    PNMOptions *PNMOptions_Init(void) {
        PNMOptions *Options = calloc(1, sizeof(PNMOptions));
        return Options;
    }
    
    void PNMOptions_Deinit(PNMOptions *Options) {
        free(Options);
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
