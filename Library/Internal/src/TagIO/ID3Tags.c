#include "../../include/Private/TagIO/ID3Tags.h"
#include "../../../Dependencies/FoundationIO/Library/include/TextIO/LogIO.h"
#include "../../../Dependencies/FoundationIO/Library/include/BufferIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    void *ID3Options_Init(void) {
        ID3Options *Options   = calloc(1, sizeof(ID3Options));
        Options->VersionMajor = 2;
        Options->VersionMinor = 4;
        Options->Encoding     = ID3Encoding_UTF8;
        return Options;
    }

    void ID3Options_Deinit(ID3Options *Options) {
        free(Options);
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
