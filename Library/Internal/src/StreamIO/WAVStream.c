#include "../../include/StreamIO/WAVStream.h"

#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h" /* Included for IsOdd */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    WAVOptions *WAVOptions_Init(void) {
        WAVOptions *Options = calloc(1, sizeof(WAVOptions));
        return Options;
    }
    
    void WAVSkipPadding(BitBuffer *BitB, uint32_t SubChunkSize) {
        if (IsOdd(SubChunkSize) == true) {
            BitBuffer_Seek(BitB, 8);
        }
    }
    
    void WAVOptions_Deinit(WAVOptions *Options) {
        free(Options->Info->Album);
        free(Options->Info->Artist);
        free(Options->Info->CreationSoftware);
        free(Options->Info->Genre);
        free(Options->Info->ReleaseDate);
        free(Options->Info->Title);
        free(Options->BEXT->Description);
        free(Options->BEXT->Originator);
        free(Options->BEXT->OriginatorDate);
        free(Options->BEXT->Description);
        free(Options->BEXT->OriginatorRef);
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
