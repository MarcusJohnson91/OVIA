#include "../../include/StreamIO/AIFStream.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    OVIADecoder AIFCDecoder = {
        .DecoderID             = StreamIO_AIF,
        .MediaType             = MediaType_Audio2D,
        .MagicIDOffset         = 0,
        .MagicIDSize           = 4,
        .MagicID               = (uint8_t) {0x41, 0x49, 0x46, 0x43},
        .Function_Initialize   = AIFOptions_Init,
        .Function_Parse        = AIFParseMetadata,
        .Function_Decode       = AIFExtractSamples,
        .Function_Deinitialize = AIFOptions_Deinit,
    };
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
