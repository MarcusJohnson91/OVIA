#include "../../include/CodecIO/FLACCodec.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h" /* Included for Assertions */
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    FLACOptions *FLACOptions_Init(void) {
        FLACOptions *Options     = calloc(1, sizeof(FLACOptions));
        AssertIO(Options != NULL);
        Options->StreamInfo  = calloc(1, sizeof(StreamInfo));
        Options->CueSheet    = calloc(1, sizeof(FLACCueSheet));
        Options->LPC         = calloc(1, sizeof(FLACLPC));
        return Options;
    }
    
    uint8_t FLAC_GetNumChannels(FLACOptions *Options) {
        AssertIO(Options != NULL);
        uint8_t NumChannels = 0;
        if (Options->Frame->ChannelLayout <= 7) {
            NumChannels = Options->Frame->ChannelLayout + 1;
        } else if (Options->Frame->ChannelLayout <= 11) {
            NumChannels = 2;
        }
        return NumChannels;
    }
    
    uint16_t FLAC_GetBlockSizeInSamples(FLACOptions *Options) {
        AssertIO(Options != NULL);
        uint16_t SamplesInBlock = 0;
        if (Options->StreamInfo->CodedSampleRate == 1) {
            SamplesInBlock = 192;
        } else if (Options->StreamInfo->CodedSampleRate >= 2 && Options->StreamInfo->CodedSampleRate <= 5) {
            SamplesInBlock = 576 * Exponentiate(2, Options->Frame->CodedBlockSize - 2);
        } else if (Options->StreamInfo->CodedSampleRate == 6) {
            // get 8 bit block from the end of the header
        } else if (Options->StreamInfo->CodedSampleRate == 7) {
            // get 16 bit block from the end of the header
        } else if (Options->StreamInfo->CodedSampleRate >= 8 && Options->StreamInfo->CodedSampleRate <= 15) {
            SamplesInBlock = 256 * Exponentiate(2, Options->Frame->CodedBlockSize - 8);
        } else {
            // Reserved
        }
        return SamplesInBlock;
    }
    
    void FLACOptions_Deinit(FLACOptions *Options) {
        AssertIO(Options != NULL);
        free(Options->StreamInfo);
        free(Options->CueSheet);
        free(Options);
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
