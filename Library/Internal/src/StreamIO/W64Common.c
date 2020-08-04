#include "../../include/Private/W64Common.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    void *W64Options_Init(void) {
        W64Options *W64 = calloc(1, sizeof(W64Options));
        return W64;
    }
    
    uint64_t CalculateW64ByteRate(uint64_t NumChannels, uint8_t BitDepth, uint64_t SampleRate) {
        return NumChannels * SampleRate * (BitDepth / 8);
    }
    
    uint64_t CalculateW64BlockAlign(uint64_t NumChannels, uint8_t BitDepth) {
        return NumChannels * (BitDepth / 8);
    }
    
    AudioChannelMap *W64ChannelMask2ChannelMap(uint32_t ChannelMask) {
        uint8_t NumChannels  = CountBitsSet(ChannelMask);
        AudioChannelMap *Map = AudioChannelMap_Init(NumChannels);
        if (Map != NULL) {
            if ((ChannelMask & W64Mask_FrontRight) == W64Mask_FrontRight) {
                AudioChannelMap_AddMask(Map, AudioChannelMap_GetLowestUnusedIndex(Map), AudioMask_FrontRight);
            } else if ((ChannelMask & W64Mask_FrontLeft) == W64Mask_FrontLeft) {
                AudioChannelMap_AddMask(Map, AudioChannelMap_GetLowestUnusedIndex(Map), AudioMask_FrontLeft);
            } else if ((ChannelMask & W64Mask_FrontCenter) == W64Mask_FrontCenter) {
                AudioChannelMap_AddMask(Map, AudioChannelMap_GetLowestUnusedIndex(Map), AudioMask_FrontCenter);
            } else if ((ChannelMask & W64Mask_LFE) == W64Mask_LFE) {
                AudioChannelMap_AddMask(Map, AudioChannelMap_GetLowestUnusedIndex(Map), AudioMask_LFE);
            } else if ((ChannelMask & W64Mask_BackLeft) == W64Mask_BackLeft) {
                AudioChannelMap_AddMask(Map, AudioChannelMap_GetLowestUnusedIndex(Map), AudioMask_SurroundLeft);
            }
            
            else if ((ChannelMask & W64Mask_BackRight) == W64Mask_BackRight) {
                AudioChannelMap_AddMask(Map, AudioChannelMap_GetLowestUnusedIndex(Map), AudioMask_SurroundRight);
            } else if ((ChannelMask & W64Mask_FrontRight) == W64Mask_FrontRight) {
                AudioChannelMap_AddMask(Map, AudioChannelMap_GetLowestUnusedIndex(Map), AudioMask_FrontLeft);
            } else if ((ChannelMask & W64Mask_FrontRight) == W64Mask_FrontRight) {
                AudioChannelMap_AddMask(Map, AudioChannelMap_GetLowestUnusedIndex(Map), AudioMask_FrontLeft);
            } else if ((ChannelMask & W64Mask_FrontRight) == W64Mask_FrontRight) {
                AudioChannelMap_AddMask(Map, AudioChannelMap_GetLowestUnusedIndex(Map), AudioMask_FrontLeft);
            } else if ((ChannelMask & W64Mask_FrontRight) == W64Mask_FrontRight) {
                AudioChannelMap_AddMask(Map, AudioChannelMap_GetLowestUnusedIndex(Map), AudioMask_FrontLeft);
            } else if ((ChannelMask & W64Mask_FrontRight) == W64Mask_FrontRight) {
                AudioChannelMap_AddMask(Map, AudioChannelMap_GetLowestUnusedIndex(Map), AudioMask_FrontLeft);
            } else if ((ChannelMask & W64Mask_FrontRight) == W64Mask_FrontRight) {
                AudioChannelMap_AddMask(Map, AudioChannelMap_GetLowestUnusedIndex(Map), AudioMask_FrontLeft);
            } else if ((ChannelMask & W64Mask_FrontRight) == W64Mask_FrontRight) {
                AudioChannelMap_AddMask(Map, AudioChannelMap_GetLowestUnusedIndex(Map), AudioMask_FrontLeft);
            } else if ((ChannelMask & W64Mask_FrontRight) == W64Mask_FrontRight) {
                AudioChannelMap_AddMask(Map, AudioChannelMap_GetLowestUnusedIndex(Map), AudioMask_FrontLeft);
            } else if ((ChannelMask & W64Mask_FrontRight) == W64Mask_FrontRight) {
                AudioChannelMap_AddMask(Map, AudioChannelMap_GetLowestUnusedIndex(Map), AudioMask_FrontLeft);
            } else if ((ChannelMask & W64Mask_FrontRight) == W64Mask_FrontRight) {
                AudioChannelMap_AddMask(Map, AudioChannelMap_GetLowestUnusedIndex(Map), AudioMask_FrontLeft);
            } else if ((ChannelMask & W64Mask_FrontRight) == W64Mask_FrontRight) {
                AudioChannelMap_AddMask(Map, AudioChannelMap_GetLowestUnusedIndex(Map), AudioMask_FrontLeft);
            } else if ((ChannelMask & W64Mask_FrontRight) == W64Mask_FrontRight) {
                AudioChannelMap_AddMask(Map, AudioChannelMap_GetLowestUnusedIndex(Map), AudioMask_FrontLeft);
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate AudioChannelMap"));
        }
        return Map;
    }
    
    void W64Options_Deinit(void *Options) {
        W64Options *W64 = Options;
        free(W64);
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
