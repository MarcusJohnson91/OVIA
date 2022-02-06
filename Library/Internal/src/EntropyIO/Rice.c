#include "../../include/EntropyIO/Rice.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    /*
     Rice Decoder: Read unary 1's until you hit a zero, that's the main number, then read 1's until you hit a zero that's the remainder
     */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
