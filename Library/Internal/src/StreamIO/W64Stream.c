#include "../../include/Private/StreamIO/RIFFStream.h"
#include "../../../Dependencies/FoundationIO/Library/include/TextIO/LogIO.h"
#include "../../../Dependencies/FoundationIO/Library/include/MathIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    void *W64Options_Init(void) {
        void *Options = calloc(1, sizeof(W64Options));
        return Options;
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
    
    void W64Options_Deinit(void *Options) {
        W64Options *W64 = Options;
        free(W64);
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
