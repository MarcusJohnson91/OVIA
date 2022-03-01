#include "../../include/CodecIO/FLACCodec.h"

#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/TextIO/LogIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    void *FLACOptions_Init(void) {
        FLACOptions *FLAC     = calloc(1, sizeof(FLACOptions));
        if (FLAC != NULL) {
            FLAC->StreamInfo  = calloc(1, sizeof(StreamInfo));
            FLAC->CueSheet    = calloc(1, sizeof(FLACCueSheet));
            FLAC->LPC         = calloc(1, sizeof(FLACLPC));
            
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate FLACOptions"));
        }
        return FLAC;
    }
    
    uint8_t FLAC_GetNumChannels(void *Options) {
        uint8_t NumChannels = 0;
        if (Options != NULL) {
            FLACOptions *FLAC = Options;
            if (FLAC->Frame->ChannelLayout <= 7) {
                NumChannels = FLAC->Frame->ChannelLayout + 1;
            } else if (FLAC->Frame->ChannelLayout <= 11) {
                NumChannels = 2;
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        }
        return NumChannels;
    }
    
    uint16_t FLAC_GetBlockSizeInSamples(void *Options) {
        uint16_t SamplesInBlock = 0;
        FLACOptions *FLAC = Options;
        if (FLAC->StreamInfo->CodedSampleRate == 1) {
            SamplesInBlock = 192;
        } else if (FLAC->StreamInfo->CodedSampleRate >= 2 && FLAC->StreamInfo->CodedSampleRate <= 5) {
            SamplesInBlock = 576 * Exponentiate(2, FLAC->Frame->CodedBlockSize - 2);
        } else if (FLAC->StreamInfo->CodedSampleRate == 6) {
            // get 8 bit block from the end of the header
        } else if (FLAC->StreamInfo->CodedSampleRate == 7) {
            // get 16 bit block from the end of the header
        } else if (FLAC->StreamInfo->CodedSampleRate >= 8 && FLAC->StreamInfo->CodedSampleRate <= 15) {
            SamplesInBlock = 256 * Exponentiate(2, FLAC->Frame->CodedBlockSize - 8);
        } else {
            // Reserved
        }
        return SamplesInBlock;
    }
    
    void FLACOptions_Deinit(void *Options) {
        FLACOptions *FLAC = Options;
        free(FLAC->StreamInfo);
        free(FLAC->CueSheet);
        free(FLAC);
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
