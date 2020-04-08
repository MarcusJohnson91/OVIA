#include "../../include/Private/FLACCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void *FLACOptions_Init(void) {
        FLACOptions *FLAC     = calloc(1, sizeof(FLACOptions));
        if (FLAC != NULL) {
            FLAC->StreamInfo  = calloc(1, sizeof(StreamInfo));
            FLAC->CueSheet    = calloc(1, sizeof(FLACCueSheet));
        } else {
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Couldn't allocate FLACOptions"));
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
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Options Pointer is NULL"));
        }
        return NumChannels;
    }
    
    uint16_t FLAC_GetBlockSizeInSamples(void *Options, uint8_t CodedBlockSize, uint16_t EndOfHeaderBlockSize) {
        uint16_t SamplesInBlock = 0;
        FLACOptions *FLAC = Options;
        if (CodedBlockSize == 1) {
            SamplesInBlock = 192;
        } else if (CodedBlockSize >= 2 && CodedBlockSize <= 5) {
            SamplesInBlock = 576 * Exponentiate(2, CodedBlockSize - 2);
        } else if (CodedBlockSize == 6) {
            SamplesInBlock = EndOfHeaderBlockSize;
        } else if (CodedBlockSize == 7) {
            SamplesInBlock = EndOfHeaderBlockSize;
        } else if (CodedBlockSize >= 8 && CodedBlockSize <= 15) {
            SamplesInBlock = 256 * Exponentiate(2, CodedBlockSize - 8);
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
    
#ifdef __cplusplus
}
#endif
