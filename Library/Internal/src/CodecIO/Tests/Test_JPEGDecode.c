#include "../../Dependencies/FoundationIO/Library/include/TestIO.h"
#include "../../Dependencies/FoundationIO/Library/include/FileIO.h"
#include "../../Library/include/Private/CodecIO/CodecIO.h"
#include "../../Dependencies/FoundationIO/Library/include/TextIO/CommandLineIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    int main(const int argc, const char *argv[]) {
        FILE     *JPG       = FileIO_OpenUTF8(UTF8String("/Users/Marcus/Desktop/Tinder.jpg"), FileMode_Read | FileMode_Binary);
        uint64_t  JPGSize   = FileIO_GetSize(JPG);
        uint8_t  *JPGMemory = calloc(JPGSize, sizeof(uint8_t));
        if (JPGMemory != NULL) {
            
        }
        return 0;
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
