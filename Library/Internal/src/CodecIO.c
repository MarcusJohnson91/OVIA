#include "../../include/CodecIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    uint64_t        NumEncoders;
    uint64_t        NumDecoders;
#define NumEncoders2 0
    CodecIO_Encoder GlobalEncoders[];
    CodecIO_Decoder GlobalDecoders[];

#ifdef    OVIA_CodecIO_PNG
#include "../../include/Private/CodecIO/PNGCodec.h"
#ifdef    OVIA_CodecIO_Encode
    CodecIO_Encoder GlobalEncoders[NumEncoders2] = {PNGEncoder};
    NumEncoders += 1;
#endif /* OVIA_CodecIO_Encode */
#ifdef    OVIA_CodecIO_Decode
    CodecIO_Encoder GlobalEncoders[NumEncoders2] = {PNGDecoder};
    NumEncoders += 1;
#endif /* OVIA_CodecIO_Decode */
#endif /* OVIA_CodecIO_PNG */

#ifdef    OVIA_CodecIO_FLAC
#include "../../include/Private/CodecIO/FLACCodec.h"
#ifdef    OVIA_CodecIO_Encode
    CodecIO_Encoder GlobalEncoders[NumEncoders2] = {FLACEncoder};
    NumEncoders += 1;
#endif /* OVIA_CodecIO_Encode */
#ifdef    OVIA_CodecIO_Decode
    CodecIO_Encoder GlobalEncoders[NumEncoders2] = {FLACDecoder};
    NumEncoders += 1;
#endif /* OVIA_CodecIO_Decode */
#endif /* OVIA_CodecIO_FLAC */

#ifdef    OVIA_CodecIO_JPEG
#include "../../include/Private/CodecIO/JPEGCodec.h"
#ifdef    OVIA_CodecIO_Encode
    CodecIO_Encoder GlobalEncoders[NumEncoders2] = {JPEGEncoder};
    NumEncoders += 1;
#endif /* OVIA_CodecIO_Encode */
#ifdef    OVIA_CodecIO_Decode
    CodecIO_Encoder GlobalEncoders[NumEncoders2] = {JPEGDecoder};
    NumEncoders += 1;
#endif /* OVIA_CodecIO_Decode */
#endif /* OVIA_CodecIO_JPEG */

#ifdef    OVIA_CodecIO_LIVC
#include "../../include/Private/CodecIO/LIVCCodec.h"
#ifdef    OVIA_CodecIO_Encode
    CodecIO_Encoder GlobalEncoders[NumEncoders2] = {LIVCEncoder};
    NumEncoders += 1;
#endif /* OVIA_CodecIO_Encode */
#ifdef    OVIA_CodecIO_Decode
    CodecIO_Encoder GlobalEncoders[NumEncoders2] = {LIVCDecoder};
    NumEncoders += 1;
#endif /* OVIA_CodecIO_Decode */
#endif /* OVIA_CodecIO_LIVC */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
