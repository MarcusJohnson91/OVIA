#include "../include/CodecIO.h" /* Included for our declarations */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

#ifdef    OVIA_CodecIO_PNG
#include "../../Internal/include/CodecIO/PNGCodec.h"
#ifdef    OVIA_CodecIO_Encode
    CodecIO_RegisterEncoder(PNGEncoder)
#endif /* OVIA_CodecIO_Encode */
#ifdef    OVIA_CodecIO_Decode
    CodecIO_RegisterDecoder(PNGDecoder)
#endif /* OVIA_CodecIO_Decode */
#endif /* OVIA_CodecIO_PNG */
    
#ifdef    OVIA_CodecIO_JPEG
#include "../../Internal/include/CodecIO/JPEGCodec.h"
#ifdef    OVIA_CodecIO_Encode
    CodecIO_RegisterEncoder(JPEGEncoder);
#endif /* OVIA_CodecIO_Encode */
#ifdef    OVIA_CodecIO_Decode
    CodecIO_RegisterDecoder(JPEGDecoder);
#endif /* OVIA_CodecIO_Decode */
#endif /* OVIA_CodecIO_JPEG */
    
#ifdef    OVIA_CodecIO_LIVC
#include "../../External/LIVC/include/CodecIO/LIVCCodec.h"
#ifdef    OVIA_CodecIO_Encode
    CodecIO_RegisterEncoder(LIVCEncoder);
#endif /* OVIA_CodecIO_Encode */
#ifdef    OVIA_CodecIO_Decode
    CodecIO_RegisterDecoder(LIVCDecoder);
#endif /* OVIA_CodecIO_Decode */
#endif /* OVIA_CodecIO_LIVC */

#ifdef    OVIA_CodecIO_FLAC
#include "../../Internal/include/CodecIO/FLACCodec.h"
#ifdef    OVIA_CodecIO_Encode
    CodecIO_RegisterEncoder(FLACEncoder)
#endif /* OVIA_CodecIO_Encode */
#ifdef    OVIA_CodecIO_Decode
    CodecIO_RegisterDecoder(FLACDecoder);
#endif /* OVIA_CodecIO_Decode */
#endif /* OVIA_CodecIO_FLAC */
    
    uint64_t NumEncoders = CodecIO_Internal_NumEncoders;
    uint64_t NumDecoders = CodecIO_Internal_NumDecoders;
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
