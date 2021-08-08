#include "../../include/CodecIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    /*
    CodecIO_Encoder GlobalEncoders[];
    CodecIO_Decoder GlobalDecoders[];
    */
#define EncoderID __COUNTER__
#define DecoderID __COUNTER__
    
#ifdef    OVIA_CodecIO_PNG
#include "../../include/Private/CodecIO/PNGCodec.h"
#ifdef    OVIA_CodecIO_Encode
    CodecIO_Encoder GlobalEncoders[EncoderID] = {PNGEncoder};
#endif /* OVIA_CodecIO_Encode */
#ifdef    OVIA_CodecIO_Decode
    CodecIO_Decoder GlobalDecoders[DecoderID] = {PNGDecoder};
#endif /* OVIA_CodecIO_Decode */
#endif /* OVIA_CodecIO_PNG */
    
#ifdef    OVIA_CodecIO_FLAC
#include "../../include/Private/CodecIO/FLACCodec.h"
#ifdef    OVIA_CodecIO_Encode
    CodecIO_Encoder GlobalEncoders[EncoderID] = {FLACEncoder};
#endif /* OVIA_CodecIO_Encode */
#ifdef    OVIA_CodecIO_Decode
    CodecIO_Decoder GlobalDecoders[DecoderID] = {FLACDecoder};
#endif /* OVIA_CodecIO_Decode */
#endif /* OVIA_CodecIO_FLAC */
    
#ifdef    OVIA_CodecIO_JPEG
#include "../../include/Private/CodecIO/JPEGCodec.h"
#ifdef    OVIA_CodecIO_Encode
    CodecIO_Encoder GlobalEncoders[EncoderID] = {JPEGEncoder};
#endif /* OVIA_CodecIO_Encode */
#ifdef    OVIA_CodecIO_Decode
    CodecIO_Decoder GlobalDecoders[DecoderID] = {JPEGDecoder};
#endif /* OVIA_CodecIO_Decode */
#endif /* OVIA_CodecIO_JPEG */
    
#ifdef    OVIA_CodecIO_LIVC
#include "../../include/Private/CodecIO/LIVCCodec.h"
#ifdef    OVIA_CodecIO_Encode
    CodecIO_Encoder GlobalEncoders[EncoderID] = {LIVCEncoder};
#endif /* OVIA_CodecIO_Encode */
#ifdef    OVIA_CodecIO_Decode
    CodecIO_Decoder GlobalDecoders[DecoderID] = {LIVCDecoder};
#endif /* OVIA_CodecIO_Decode */
#endif /* OVIA_CodecIO_LIVC */
    
    uint64_t NumEncoders = EncoderID;
    uint64_t NumDecoders = DecoderID;
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
