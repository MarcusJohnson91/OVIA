#include "../../include/CodecIO.h"

#ifdef    OVIA_EnableImage
#ifdef    OVIA_CodecIO_PNG
#include "../../include/Private/CodecIO/PNGCodec.h"
#endif /* OVIA_CodecIO_PNG */

#ifdef    OVIA_CodecIO_JPEG
#include "../../include/Private/CodecIO/JPEGCodec.h"
#endif /* OVIA_CodecIO_JPEG */

#ifdef    OVIA_EnableBMP
#include "../../include/Private/CodecIO/BMPCodec.h"
#endif /* OVIA_EnableBMP */

#ifdef    OVIA_EnablePNM
#include "../../include/Private/CodecIO/PNMCodec.h"
#endif /* OVIA_EnablePNM */

#ifdef    OVIA_EnableLIVC
#include "../../include/Private/CodecIO/LIVCCodec.h"
#endif /* OVIA_EnableLIVC */
#endif /* OVIA_EnableImage */

#ifdef    OVIA_EnableAudio
#ifdef    OVIA_CodecIO_FLAC
#include "../../include/Private/CodecIO/FLACCodec.h"
#endif /* OVIA_CodecIO_FLAC */

#ifdef    OVIA_EnablePCM
#include "../../include/Private/CodecIO/PCMCodec.h"
#endif /* OVIA_EnablePCM */
#endif /* OVIA_EnableAudio */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    uint64_t        NumEncoders;
    CodecIO_Encoder GlobalEncoders[];

#ifdef OVIA_CodecIO_PNG
    GlobalEncoders    = {
        [__COUNTER__] = PNGEncoder,
    }
    NumEncoders      += 1;
#endif /* OVIA_CodecIO_PNG */

#ifdef OVIA_CodecIO_JPEG
    GlobalEncoders    = {
        [__COUNTER__] = JPEGEncoder,
    }
    NumEncoders      += 1;
#endif /* OVIA_CodecIO_JPEG */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif