#include "../../include/Private/W64Common.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    W64Options *W64Options_Init(void) {
        W64Options *W64 = calloc(1, sizeof(W64Options));
        return W64;
    }
    
    uint64_t CalculateW64ByteRate(uint64_t NumChannels, uint8_t BitDepth, uint64_t SampleRate) {
        return NumChannels * SampleRate * (BitDepth / 8);
    }
    
    uint64_t CalculateW64BlockAlign(uint64_t NumChannels, uint8_t BitDepth) {
        return NumChannels * (BitDepth / 8);
    }
    
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
    
    void W64Options_Deinit(W64Options *W64) {
        free(W64);
    }
    
#ifdef __cplusplus
}
#endif
