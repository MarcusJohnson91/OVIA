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
    CodecIO_Encoder GlobalEncoders[NumEncoders] = {PNGEncoder};
    NumEncoders += 1;
#endif /* OVIA_CodecIO_Encode */
#ifdef    OVIA_CodecIO_Decode
    CodecIO_Decoder GlobalDecoders[NumDecoders] = {PNGDecoder};
    NumDecoders += 1;
#endif /* OVIA_CodecIO_Decode */
#endif /* OVIA_CodecIO_PNG */

#ifdef    OVIA_CodecIO_FLAC
#include "../../include/Private/CodecIO/FLACCodec.h"
#ifdef    OVIA_CodecIO_Encode
    CodecIO_Encoder GlobalEncoders[NumEncoders] = {FLACEncoder};
    NumEncoders += 1;
#endif /* OVIA_CodecIO_Encode */
#ifdef    OVIA_CodecIO_Decode
    CodecIO_Decoder GlobalDecoders[NumDecoders] = {FLACDecoder};
    NumDecoders += 1;
#endif /* OVIA_CodecIO_Decode */
#endif /* OVIA_CodecIO_FLAC */

#ifdef    OVIA_CodecIO_JPEG
#include "../../include/Private/CodecIO/JPEGCodec.h"
#ifdef    OVIA_CodecIO_Encode
    CodecIO_Encoder GlobalEncoders[NumEncoders] = {JPEGEncoder};
    NumEncoders += 1;
#endif /* OVIA_CodecIO_Encode */
#ifdef    OVIA_CodecIO_Decode
    CodecIO_Decoder GlobalDecoders[NumDecoders] = {JPEGDecoder};
    NumDecoders += 1;
#endif /* OVIA_CodecIO_Decode */
#endif /* OVIA_CodecIO_JPEG */

#ifdef    OVIA_CodecIO_LIVC
#include "../../include/Private/CodecIO/LIVCCodec.h"
#ifdef    OVIA_CodecIO_Encode
    CodecIO_Encoder GlobalEncoders[NumEncoders] = {LIVCEncoder};
    NumEncoders += 1;
#endif /* OVIA_CodecIO_Encode */
#ifdef    OVIA_CodecIO_Decode
    CodecIO_Decoder GlobalDecoders[NumDecoders] = {LIVCDecoder};
    NumDecoders += 1;
#endif /* OVIA_CodecIO_Decode */
#endif /* OVIA_CodecIO_LIVC */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
