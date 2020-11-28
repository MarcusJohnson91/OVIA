#include "../../../Dependencies/FoundationIO/Library/include/PlatformIO.h"

#ifdef    OVIA_EnableImage

#ifdef    OVIA_EnableJPEG
#include "../../include/Private/CodecIO/JPEGCodec.h"
#endif /* OVIA_EnableJPEG */

#ifdef    OVIA_EnablePNG
#include "../../include/Private/CodecIO/PNGCodec.h"
#endif /* OVIA_EnableJPEG */

#ifdef    OVIA_EnableBMP
#include "../../include/Private/CodecIO/PCMCodec.h"
#endif /* OVIA_EnableJPEG */

#endif /* OVIA_EnableImage */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
