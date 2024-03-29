#include "../../include/StreamIO/BMPStream.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    BMPOptions *BMPOptions_Init(void) {
        BMPOptions *BMP = calloc(1, sizeof(BMPOptions));
        return BMP;
    }
    
    uint64_t BMPGetRowSize(const uint16_t BitsPerPixel, const uint32_t ImageWidth) {
        return FloorF((BitsPerPixel * ImageWidth + 31) / 32) * 4; // Floor((4 * 1024 + 31) / 32) * 4
    }
    
    uint64_t BMPGetPixelArraySize(const uint64_t RowSize, const int32_t ImageHeight) { // 5568, 3712, there's 54 extra bytes in the PixelArray...
        return RowSize * AbsoluteI(ImageHeight);
    }
    
    void BMPOptions_Deinit(BMPOptions *Options) {
        free(Options->ICCPayload);
        free(Options);
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
