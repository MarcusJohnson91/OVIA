#include "../../../Dependencies/FoundationIO/Library/include/PlatformIO.h"

#ifdef OVIA_EnableAIF
#include "../../include/Private/StreamIO/AIFStream.h"
#endif /* OVIA_EnableAIF */

#ifdef OVIA_EnableBMP
#include "../../include/Private/StreamIO/BMPStream.h"
#endif /* OVIA_EnableBMP */

#ifdef OVIA_EnablePNM
#include "../../include/Private/StreamIO/PNMStream.h"
#endif /* OVIA_EnablePNM */

#ifdef OVIA_EnableRIFF
#include "../../include/Private/StreamIO/RIFFStream.h"
#endif /* OVIA_EnableRIFF */

#ifdef OVIA_EnableOGG
#include "../../include/Private/StreamIO/OGGStream.h"
#endif /* OVIA_EnableOGG */

#ifdef OVIA_CodecIO_FLACNative
#include "../../include/Private/StreamIO/FLACNativeStream.h"
#endif /* OVIA_CodecIO_FLACNative */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
