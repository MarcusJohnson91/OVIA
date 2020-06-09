#include "../../include/Private/ID3Common.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    void ID3v2_Read(ID3 *Id3, BitBuffer *BitB) {
        if (Id3 != NULL && BitB != NULL) { // We assume the "ID3" marker has already been consumed
            // Read the Version variable
        } else if (Id3 == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ID3 Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
