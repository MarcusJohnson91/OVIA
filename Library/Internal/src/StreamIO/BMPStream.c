#include "../../include/Private/StreamIO/BMPStream.h"
#include "../../../Dependencies/FoundationIO/Library/include/TextIO/LogIO.h"
#include "../../../Dependencies/FoundationIO/Library/include/MathIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    void *BMPOptions_Init(void) {
        BMPOptions *BMP = calloc(1, sizeof(BMPOptions));
        return BMP;
    }

    static uint64_t BMPGetRowSize(const uint16_t BitsPerPixel, const uint32_t ImageWidth) {
        return FloorF((BitsPerPixel * ImageWidth + 31) / 32) * 4; // Floor((4 * 1024 + 31) / 32) * 4
    }

    static uint64_t BMPGetPixelArraySize(const uint64_t RowSize, const int32_t ImageHeight) { // 5568, 3712, there's 54 extra bytes in the PixelArray...
        return RowSize * AbsoluteI(ImageHeight);
    }

    void BMPOptions_Deinit(void *Options) {
        BMPOptions *BMP = Options;
        free(BMP->ICCPayload);
        free(BMP);
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
