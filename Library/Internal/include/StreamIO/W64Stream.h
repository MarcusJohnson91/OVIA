/*!
 @header              W64Common.h
 @author              Marcus Johnson
 @copyright           2017+
 @version             1.0.0
 @brief               This header contains code for reading and writing lossless W64 audio files
 */

#include "OVIACommon.h"
#include "StreamIO.h"

#pragma once

#ifndef OVIA_W64Common_H
#define OVIA_W64Common_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    typedef struct W64Options {
        uint32_t SpeakerMask;
        uint32_t SampleRate;
        uint32_t ByteRate;
        uint32_t BlockAlign;
        uint16_t CompressionFormat;
        uint16_t BlockAlignment;
        uint16_t NumChannels;
        uint16_t BitDepth;
        uint16_t ValidBitsPerSample;
    } W64Options;
    
    void    *W64Options_Init(void);
    
    uint64_t CalculateW64ByteRate(uint64_t NumChannels, uint8_t BitDepth, uint64_t SampleRate);
    
    uint64_t CalculateW64BlockAlign(uint64_t NumChannels, uint8_t BitDepth);
    
    void     W64Options_Deinit(void *Options);
    
    static const uint8_t W64_RIFF_GUIDString[] = {
        0x72,0x69,0x66,0x66,0x2D,0x2E,0x91,0x2D,0xCF,0x11,0x2D,0xA5,0xD6,0x2D,0x28,0xDB,0x04,0xC1,0x00,0x00,0x00
    };
    
    static const uint8_t W64_WAVE_GUIDString[] = {
        0x77,0x61,0x76,0x65,0x2D,0xF3,0xAC,0x2D,0xD3,0x11,0x2D,0x8C,0xD1,0x2D,0x00,0xC0,0x4F,0x8E,0xDB,0x8A,0x00
    };
    
    static const uint8_t W64_FMT_GUIDString[] = {
        0x66,0x6D,0x74,0x20,0x2D,0xF3,0xAC,0x2D,0xD3,0x11,0x2D,0x8C,0xD1,0x2D,0x00,0xC0,0x4F,0x8E,0xDB,0x8A,0x00
    };
    
    static const uint8_t W64_DATA_GUIDString[] = {
        0x64,0x61,0x74,0x61,0x2D,0xF3,0xAC,0x2D,0xD3,0x11,0x2D,0x8C,0xD1,0x2D,0x00,0xC0,0x4F,0x8E,0xDB,0x8A,0x00
    };
    
    static const uint8_t W64_LEVL_GUIDString[] = { // aka Peak Envelope Chunk
        0x6A,0x75,0x6E,0x6B,0x2D,0xF3,0xAC,0x2D,0xD3,0x11,0x2D,0x8C,0xD1,0x2D,0x00,0xC0,0x4F,0x8E,0xDB,0x8A,0x00
    };
    
    static const uint8_t W64_BEXT_GUIDString[] = {
        0x62,0x65,0x78,0x74,0x2D,0xf3,0xAC,0x2D,0xD3,0xAA,0x2D,0xD1,0x8C,0x2D,0x00,0xC0,0x4F,0x8E,0xDB,0x8A,0x00
    };
    
    static const uint8_t W64_FACT_GUIDString[] = {
        0x66,0x61,0x63,0x74,0x2D,0xF3,0xAC,0x2D,0xD3,0x11,0x2D,0x8C,0xD1,0x2D,0x00,0xC0,0x4F,0x8E,0xDB,0x8A,0x00
    };
    
    static const uint8_t W64_JUNK_GUIDString[] = {
        0x6B,0x6E,0x75,0x6A,0x2D,0xAC,0xF3,0x2D,0x11,0xD3,0x2D,0x8C,0xD1,0x2D,0x00,0xC0,0x4F,0x8E,0xDB,0x8A,0x00
    };
    
    static const uint8_t W64_MRKR_GUIDString[] = { // MARKER
        0xAB,0xF7,0x62,0x56,0x2D,0x39,0x45,0x2D,0x11,0xD2,0x2D,0x86,0xC7,0x2D,0x00,0xC0,0x4F,0x8E,0xDB,0x8A,0x00
    };
    
    static const uint8_t W64_SUMM_GUIDString[] = { // SUMMARY LIST
        0x92,0x5F,0x94,0xBC,0x2D,0x52,0x5A,0x2D,0x11,0xD2,0x2D,0x86,0xDC,0x2D,0x00,0xC0,0x4F,0x8E,0xDB,0x8A,0x00
    };
    
    static const uint8_t W64_LIST_GUIDString[] = {
        0x74,0x73,0x69,0x6C,0x2D,0x91,0x2F,0x2D,0x11,0xCF,0x2D,0xA5,0xD6,0x2D,0x28,0xDB,0x04,0xC1,0x00,0x00,0x00
    };
    
    typedef enum W64SpeakerMask {
        W64Mask_FrontRight         = 0x1,
        W64Mask_FrontLeft          = 0x2,
        W64Mask_FrontCenter        = 0x4,
        W64Mask_LFE                = 0x8,
        W64Mask_BackLeft           = 0x10,
        W64Mask_BackRight          = 0x20,
        W64Mask_FrontLeftCenter    = 0x40,
        W64Mask_FrontRightCenter   = 0x80,
        W64Mask_BackCenter         = 0x100,
        W64Mask_SideLeft           = 0x200,
        W64Mask_SideRight          = 0x400,
        W64Mask_TopCenter          = 0x800,
        W64Mask_TopFrontLeft       = 0x1000,
        W64Mask_TopFrontCenter     = 0x2000,
        W64Mask_TopFrontRight      = 0x4000,
        W64Mask_TopBackLeft        = 0x8000,
        W64Mask_TopBackCenter      = 0x10000,
        W64Mask_TopBackRight       = 0x20000,
    } W64SpeakerMask;
    
    typedef enum W64CompressionFormats {
        Codec_Unknown                 = 0x0000,
        Codec_PCM                     = 0x0001,
        Codec_ADPCM                   = 0x0002,
        Codec_Float                   = 0x0003,
        Codec_VSELP                   = 0x0004,
        Codec_IBM_CVSD                = 0x0005,
        Codec_ALAW                    = 0x0006,
        Codec_MULAW                   = 0x0007,
        Codec_DTS                     = 0x0008,
        Codec_OKI_ADPCM               = 0x0009,
        Codec_DVI_ADPCM               = 0x0010,
        Codec_IMA_ADPCM               = 0x0011,
        Codec_MEDIASPACE_ADPCM        = 0x0012,
        Codec_SIERRA_ADPCM            = 0x0013,
        Codec_G723_ADPCM              = 0x0014,
        Codec_DIGISTD                 = 0x0015,
        Codec_DIGIFIX                 = 0x0016,
        Codec_DIALOGIC_OKI_ADPCM      = 0x0017,
        Codec_MEDIAVISION_ADPCM       = 0x0018,
        Codec_CU_COOvia               = 0x0019,
        Codec_YAMAHA_ADPCM            = 0x0020,
        Codec_SONARC                  = 0x0021,
        Codec_DSPGROUP_TRUESPEECH     = 0x0022,
        Codec_ECHOSC1                 = 0x0023,
        Codec_AUDIOFILE_AF36          = 0x0024,
        Codec_APTX                    = 0x0025,
        Codec_AUDIOFILE_AF10          = 0x0026,
        Codec_PROSODY_1612            = 0x0027,
        Codec_LRC                     = 0x0028,
        Codec_DOLBY_AC2               = 0x0030,
        Codec_GSM610                  = 0x0031,
        Codec_MSNAUDIO                = 0x0032,
        Codec_ANTEX_ADPCME            = 0x0033,
        Codec_CONTROL_RES_VQLPC       = 0x0034,
        Codec_DIGIREAL                = 0x0035,
        Codec_DIGIADPCM               = 0x0036,
        Codec_CONTROL_RES_CR10        = 0x0037,
        Codec_NMS_VBXADPCM            = 0x0038,
        Codec_CS_IMAADPCM             = 0x0039,
        Codec_ECHOSC3                 = 0x003A,
        Codec_ROCKWELL_ADPCM          = 0x003B,
        Codec_ROCKWELL_DIGITALK       = 0x003C,
        Codec_XEBEC                   = 0x003D,
        Codec_G721_ADPCM              = 0x0040,
        Codec_G728_CELP               = 0x0041,
        Codec_MSG723                  = 0x0042,
        Codec_MPEG                    = 0x0050,
        Codec_RT24                    = 0x0052,
        Codec_PAC                     = 0x0053,
        Codec_MPEGLAYER3              = 0x0055,
        Codec_LUCENT_G723             = 0x0059,
        Codec_CIRRUS                  = 0x0060,
        Codec_ESPCM                   = 0x0061,
        Codec_VOXWARE                 = 0x0062,
        Codec_CANOPUS_ATRAC           = 0x0063,
        Codec_G726_ADPCM              = 0x0064,
        Codec_G722_ADPCM              = 0x0065,
        Codec_DSAT_DISPLAY            = 0x0067,
        Codec_VOXWARE_BYTE_ALIGNED    = 0x0069,
        Codec_VOXWARE_AC8             = 0x0070,
        Codec_VOXWARE_AC10            = 0x0071,
        Codec_VOXWARE_AC16            = 0x0072,
        Codec_VOXWARE_AC20            = 0x0073,
        Codec_VOXWARE_RT24            = 0x0074,
        Codec_VOXWARE_RT29            = 0x0075,
        Codec_VOXWARE_RT29HW          = 0x0076,
        Codec_VOXWARE_VR12            = 0x0077,
        Codec_VOXWARE_VR18            = 0x0078,
        Codec_VOXWARE_TQ40            = 0x0079,
        Codec_SOFTSOUND               = 0x0080,
        Codec_VOXWARE_TQ60            = 0x0081,
        Codec_MSRT24                  = 0x0082,
        Codec_G729A                   = 0x0083,
        Codec_MVI_MVI2                = 0x0084,
        Codec_DF_G726                 = 0x0085,
        Codec_DF_GSM610               = 0x0086,
        Codec_ISIAUDIO                = 0x0088,
        Codec_ONLIVE                  = 0x0089,
        Codec_SBC24                   = 0x0091,
        Codec_DOLBY_AC3_SPDIF         = 0x0092,
        Codec_MEDIASONIC_G723         = 0x0093,
        Codec_PROSODY_8KBPS           = 0x0094,
        Codec_ZYXEL_ADPCM             = 0x0097,
        Codec_PHILIPS_LPCBB           = 0x0098,
        Codec_PACKED                  = 0x0099,
        Codec_MALDEN_PHONYTALK        = 0x00A0,
        Codec_RHETOREX_ADPCM          = 0x0100,
        Codec_IRAT                    = 0x0101,
        Codec_VIVO_G723               = 0x0111,
        Codec_VIVO_SIREN              = 0x0112,
        Codec_DIGITAL_G723            = 0x0123,
        Codec_SANYO_LD_ADPCM          = 0x0125,
        Codec_SIPROLAB_ACEPLNET       = 0x0130,
        Codec_SIPROLAB_ACELP4800      = 0x0131,
        Codec_SIPROLAB_ACELP8V3       = 0x0132,
        Codec_SIPROLAB_G729           = 0x0133,
        Codec_SIPROLAB_G729A          = 0x0134,
        Codec_SIPROLAB_KELVIN         = 0x0135,
        Codec_G726ADPCM               = 0x0140,
        Codec_QUALCOMM_PUREVOICE      = 0x0150,
        Codec_QUALCOMM_HALFRATE       = 0x0151,
        Codec_TUBGSM                  = 0x0155,
        Codec_MSAUDIO1                = 0x0160,
        Codec_CREATIVE_ADPCM          = 0x0200,
        Codec_CREATIVE_FASTSPEECH8    = 0x0202,
        Codec_CREATIVE_FASTSPEECH10   = 0x0203,
        Codec_UHER_ADPCM              = 0x0210,
        Codec_QUARTEROviaK            = 0x0220,
        Codec_ILINK_VC                = 0x0230,
        Codec_RAW_SPORT               = 0x0240,
        Codec_IPI_HSX                 = 0x0250,
        Codec_IPI_RPELP               = 0x0251,
        Codec_CS2                     = 0x0260,
        Codec_SONY_SCX                = 0x0270,
        Codec_FM_TOWNS_SND            = 0x0300,
        Codec_BTV_DIGITAL             = 0x0400,
        Codec_QDESIGN_MUSIC           = 0x0450,
        Codec_VME_VMPCM               = 0x0680,
        Codec_TPC                     = 0x0681,
        Codec_OLIGSM                  = 0x1000,
        Codec_OLIADPCM                = 0x1001,
        Codec_OLICELP                 = 0x1002,
        Codec_OLISBC                  = 0x1003,
        Codec_OLIOPR                  = 0x1004,
        Codec_LH_COOvia               = 0x1100,
        Codec_NORRIS                  = 0x1400,
        Codec_SOUNDSPACE_MUSICOMPRESS = 0x1500,
        Codec_DVM                     = 0x2000,
    } W64CompressionFormats;

    static const OVIA_Extensions W64Extensions = {
        .NumExtensions = 1,
        .Extensions    = {
            [0] = UTF32String("w64"),
        },
    };

    static const OVIA_MagicIDs W64MagicIDs = {
        .NumMagicIDs         = 1,
        .MagicIDOffsetInBits = 192,
        .MagicIDSizeInBits   = 128,
        .MagicIDNumber = {
            [0] = (uint8_t[16]){0x77, 0x61, 0x76, 0x65, 0xF3, 0xAC, 0xD3, 0x11, 0x8C, 0xD1, 0x00, 0xC0, 0x4F, 0x8E, 0xDB, 0x8A},
        },
    };

    
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_W64Common_H */
