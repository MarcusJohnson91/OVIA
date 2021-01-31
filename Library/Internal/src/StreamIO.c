#include "../../include/StreamIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

#ifdef    OVIA_StreamIO_AIF
#include "../../include/Private/StreamIO/AIFStream.h"
#endif /* OVIA_StreamIO_AIF */

#ifdef    OVIA_StreamIO_BMP
#include "../../include/Private/StreamIO/BMPStream.h"
#endif /* OVIA_StreamIO_BMP */

#ifdef    OVIA_StreamIO_PNM
#include "../../include/Private/StreamIO/PNMStream.h"
#endif /* OVIA_StreamIO_PNM */

#ifdef    OVIA_StreamIO_RIFF
#include "../../include/Private/StreamIO/RIFFStream.h"
#endif /* OVIA_StreamIO_RIFF */

#ifdef    OVIA_StreamIO_OGG
#include "../../include/Private/StreamIO/OGGStream.h"
#endif /* OVIA_StreamIO_OGG */

#ifdef    OVIA_StreamIO_FLAC
#include "../../include/Private/StreamIO/FLACNativeStream.h"
#endif /* OVIA_StreamIO_FLAC */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
