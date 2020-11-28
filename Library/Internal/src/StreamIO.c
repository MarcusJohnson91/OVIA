#include "../../../Dependencies/FoundationIO/Library/include/PlatformIO.h"

#ifdef OVIA_EnableAIF
#include "../../include/Private/StreamIO/AIFStream.h"
#endif /* OVIA_EnableAIF */

#ifdef OVIA_EnableBMP
#include "../../include/Private/StreamIO/AIFStream.h"
#endif /* OVIA_EnableBMP */

#ifdef OVIA_EnablePNM
#include "../../include/Private/StreamIO/AIFStream.h"
#endif /* OVIA_EnablePNM */

#ifdef OVIA_EnableRIFF
#include "../../include/Private/StreamIO/AIFStream.h"
#endif /* OVIA_EnableRIFF */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

#ifdef OVIA_EnableEncoders

#endif /* OVIA_EnableEncoders */

#ifdef OVIA_EnableDecoders

#endif /* OVIA_EnableDecoders */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
