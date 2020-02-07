#include "../../include/Private/W64Common.h"

#ifdef __cplusplus
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
            Log(Log_DEBUG, FoundationIOFunctionName, UTF8String("Couldn't allocate AudioChannelMap"));
        }
        return Map;
    }
    /*
    Audio_ChannelMask W64ChannelMap2AudioMask(uint64_t ChannelMask) {
        Audio_ChannelMask AudioMask = 0;
        if (ChannelMask & 0x1) {
            AudioMask += AudioMask_FrontLeft;
        } else if (ChannelMask & 0x2) {
            AudioMask += AudioMask_FrontRight;
        } else if (ChannelMask & 0x4) {
            AudioMask += AudioMask_FrontCenter;
        } else if (ChannelMask & 0x8) {
            AudioMask += AudioMask_LFE;
        } else if (ChannelMask & 0x10) {
            AudioMask += AudioMask_RearLeft;
        } else if (ChannelMask & 0x20) {
            AudioMask += AudioMask_RearRight;
        } else if (ChannelMask & 0x40) {
            AudioMask += AudioMask_FrontCenterLeft;
        } else if (ChannelMask & 0x80) {
            AudioMask += AudioMask_FrontCenterRight;
        } else if (ChannelMask & 0x100) {
            AudioMask += AudioMask_RearCenter;
        } else if (ChannelMask & 0x200) {
            AudioMask += AudioMask_SurroundLeft;
        } else if (ChannelMask & 0x400) {
            AudioMask += AudioMask_SurroundRight;
        } else if (ChannelMask & 0x800) {
            AudioMask += AudioMask_TopCenter;
        } else if (ChannelMask & 0x1000) {
            AudioMask += AudioMask_TopFrontLeft;
        } else if (ChannelMask & 0x2000) {
            AudioMask += AudioMask_TopFrontCenter;
        } else if (ChannelMask & 0x4000) {
            AudioMask += AudioMask_TopFrontRight;
        } else if (ChannelMask & 0x8000) {
            AudioMask += AudioMask_TopRearLeft;
        } else if (ChannelMask & 0x10000) {
            AudioMask += AudioMask_TopRearCenter;
        } else if (ChannelMask & 0x20000) {
            AudioMask += AudioMask_TopRearRight;
        }
        return AudioMask;
    }
    */
    void W64Options_Deinit(void *Options) {
        W64Options *W64 = Options;
        free(W64);
    }
    
#ifdef __cplusplus
}
#endif
