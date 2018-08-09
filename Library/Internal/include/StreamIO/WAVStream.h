#include "../../libOVIA.h"

#include "../../../../Dependencies/FoundationIO/libFoundationIO/include/BitIO.h"
#include "../../../../Dependencies/FoundationIO/libFoundationIO/include/Log.h"
#include "../../../../Dependencies/FoundationIO/libFoundationIO/include/Math.h"
#include "../../../../Dependencies/FoundationIO/libFoundationIO/include/ContainerIO.h"

#pragma once

#ifndef OVIA_W64Common_H
#define OVIA_W64Common_H

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef enum WAVCompressionFormats {
        UnknownCompressionFormat      = 0x0000,
        Lossless_PCM                  = 0x0001,
        Lossy_ADPCM                   = 0x0002,
        Lossy_IEEE_FLOAT              = 0x0003,
        Lossy_VSELP                   = 0x0004,
        Lossy_IBM_CVSD                = 0x0005,
        Lossy_ALAW                    = 0x0006,
        Lossy_MULAW                   = 0x0007,
        Lossy_DTS                     = 0x0008,
        Lossy_OKI_ADPCM               = 0x0010,
        Lossy_DVI_ADPCM               = 0x0011,
        Lossy_IMA_ADPCM               = 0x0011,
        Lossy_MEDIASPACE_ADPCM        = 0x0012,
        Lossy_SIERRA_ADPCM            = 0x0013,
        Lossy_G723_ADPCM              = 0x0014,
        Lossy_DIGISTD                 = 0x0015,
        Lossy_DIGIFIX                 = 0x0016,
        Lossy_DIALOGIC_OKI_ADPCM      = 0x0017,
        Lossy_MEDIAVISION_ADPCM       = 0x0018,
        Lossy_CU_CODEC                = 0x0019,
        Lossy_YAMAHA_ADPCM            = 0x0020,
        Lossy_SONARC                  = 0x0021,
        Lossy_DSPGROUP_TRUESPEECH     = 0x0022,
        Lossy_ECHOSC1                 = 0x0023,
        Lossy_AUDIOFILE_AF36          = 0x0024,
        Lossy_APTX                    = 0x0025,
        Lossy_AUDIOFILE_AF10          = 0x0026,
        Lossy_PROSODY_1612            = 0x0027,
        Lossy_LRC                     = 0x0028,
        Lossy_DOLBY_AC2               = 0x0030,
        Lossy_GSM610                  = 0x0031,
        Lossy_MSNAUDIO                = 0x0032,
        Lossy_ANTEX_ADPCME            = 0x0033,
        Lossy_CONTROL_RES_VQLPC       = 0x0034,
        Lossy_DIGIREAL                = 0x0035,
        Lossy_DIGIADPCM               = 0x0036,
        Lossy_CONTROL_RES_CR10        = 0x0037,
        Lossy_NMS_VBXADPCM            = 0x0038,
        Lossy_CS_IMAADPCM             = 0x0039,
        Lossy_ECHOSC3                 = 0x003A,
        Lossy_ROCKWELL_ADPCM          = 0x003B,
        Lossy_ROCKWELL_DIGITALK       = 0x003C,
        Lossy_XEBEC                   = 0x003D,
        Lossy_G721_ADPCM              = 0x0040,
        Lossy_G728_CELP               = 0x0041,
        Lossy_MSG723                  = 0x0042,
        Lossy_MPEG                    = 0x0050,
        Lossy_RT24                    = 0x0052,
        Lossy_PAC                     = 0x0053,
        Lossy_MPEGLAYER3              = 0x0055,
        Lossy_LUCENT_G723             = 0x0059,
        Lossy_CIRRUS                  = 0x0060,
        Lossy_ESPCM                   = 0x0061,
        Lossy_VOXWARE                 = 0x0062,
        Lossy_CANOPUS_ATRAC           = 0x0063,
        Lossy_G726_ADPCM              = 0x0064,
        Lossy_G722_ADPCM              = 0x0065,
        Lossy_DSAT_DISPLAY            = 0x0067,
        Lossy_VOXWARE_BYTE_ALIGNED    = 0x0069,
        Lossy_VOXWARE_AC8             = 0x0070,
        Lossy_VOXWARE_AC10            = 0x0071,
        Lossy_VOXWARE_AC16            = 0x0072,
        Lossy_VOXWARE_AC20            = 0x0073,
        Lossy_VOXWARE_RT24            = 0x0074,
        Lossy_VOXWARE_RT29            = 0x0075,
        Lossy_VOXWARE_RT29HW          = 0x0076,
        Lossy_VOXWARE_VR12            = 0x0077,
        Lossy_VOXWARE_VR18            = 0x0078,
        Lossy_VOXWARE_TQ40            = 0x0079,
        Lossy_SOFTSOUND               = 0x0080,
        Lossy_VOXWARE_TQ60            = 0x0081,
        Lossy_MSRT24                  = 0x0082,
        Lossy_G729A                   = 0x0083,
        Lossy_MVI_MVI2                = 0x0084,
        Lossy_DF_G726                 = 0x0085,
        Lossy_DF_GSM610               = 0x0086,
        Lossy_ISIAUDIO                = 0x0088,
        Lossy_ONLIVE                  = 0x0089,
        Lossy_SBC24                   = 0x0091,
        Lossy_DOLBY_AC3_SPDIF         = 0x0092,
        Lossy_MEDIASONIC_G723         = 0x0093,
        Lossy_PROSODY_8KBPS           = 0x0094,
        Lossy_ZYXEL_ADPCM             = 0x0097,
        Lossy_PHILIPS_LPCBB           = 0x0098,
        Lossy_PACKED                  = 0x0099,
        Lossy_MALDEN_PHONYTALK        = 0x00A0,
        Lossy_RHETOREX_ADPCM          = 0x0100,
        Lossy_IRAT                    = 0x0101,
        Lossy_VIVO_G723               = 0x0111,
        Lossy_VIVO_SIREN              = 0x0112,
        Lossy_DIGITAL_G723            = 0x0123,
        Lossy_SANYO_LD_ADPCM          = 0x0125,
        Lossy_SIPROLAB_ACEPLNET       = 0x0130,
        Lossy_SIPROLAB_ACELP4800      = 0x0131,
        Lossy_SIPROLAB_ACELP8V3       = 0x0132,
        Lossy_SIPROLAB_G729           = 0x0133,
        Lossy_SIPROLAB_G729A          = 0x0134,
        Lossy_SIPROLAB_KELVIN         = 0x0135,
        Lossy_G726ADPCM               = 0x0140,
        Lossy_QUALCOMM_PUREVOICE      = 0x0150,
        Lossy_QUALCOMM_HALFRATE       = 0x0151,
        Lossy_TUBGSM                  = 0x0155,
        Lossy_MSAUDIO1                = 0x0160,
        Lossy_CREATIVE_ADPCM          = 0x0200,
        Lossy_CREATIVE_FASTSPEECH8    = 0x0202,
        Lossy_CREATIVE_FASTSPEECH10   = 0x0203,
        Lossy_UHER_ADPCM              = 0x0210,
        Lossy_QUARTERDECK             = 0x0220,
        Lossy_ILINK_VC                = 0x0230,
        Lossy_RAW_SPORT               = 0x0240,
        Lossy_IPI_HSX                 = 0x0250,
        Lossy_IPI_RPELP               = 0x0251,
        Lossy_CS2                     = 0x0260,
        Lossy_SONY_SCX                = 0x0270,
        Lossy_FM_TOWNS_SND            = 0x0300,
        Lossy_BTV_DIGITAL             = 0x0400,
        Lossy_QDESIGN_MUSIC           = 0x0450,
        Lossy_VME_VMPCM               = 0x0680,
        Lossy_TPC                     = 0x0681,
        Lossy_OLIGSM                  = 0x1000,
        Lossy_OLIADPCM                = 0x1001,
        Lossy_OLICELP                 = 0x1002,
        Lossy_OLISBC                  = 0x1003,
        Lossy_OLIOPR                  = 0x1004,
        Lossy_LH_CODEC                = 0x1100,
        Lossy_NORRIS                  = 0x1400,
        Lossy_SOUNDSPACE_MUSICOMPRESS = 0x1500,
        Lossy_DVM                     = 0x2000,
    } WAVCompressionFormats;
    
    enum WAVChunkIDs { // odd sized chunks have a trailing 0 byte, and strings are null padded, this is for WAV, and W64.
        WAV_RIFF                      = 0x52494646,
        WAV_WAVE                      = 0x57415645,
        WAV_FMT                       = 0x666D7420,
        WAV_LIST                      = 0x4C495354,
        WAV_INFO                      = 0x494E464F,
        WAV_IART                      = 0x49415254, // Artist
        WAV_ICRD                      = 0x49435244, // Release data
        WAV_IGNR                      = 0x49474E52, // Genre
        WAV_INAM                      = 0x494E414D, // Song name aka title
        WAV_IPRD                      = 0x49505244, // Album name
        WAV_IPRT                      = 0x49505254, // Track number
        WAV_ISFT                      = 0x49534654, // Software that created it?
        WAV_CODE                      = 0x434f4445, // Encoder
        WAV_ICMT                      = 0x49434d54, // Comment
        WAV_ICOP                      = 0x49434f50, // Copyright
        WAV_YEAR                      = 0x59454152, // Year
        WAV_TRCK                      = 0x5452434b, // Track
        WAV_DATA                      = 0x64617461,
    };
    
    void WAVParseMetadata(PCMFile *PCM, BitBuffer *BitB);
    
    void WAVExtractSamples(PCMFile *PCM, BitBuffer *BitB, uint64_t NumSamples2Extract, uint32_t **ExtractedSamples);
    
    void WAVInsertSamples(PCMFile *PCM, BitBuffer *OutputSamples, uint32_t NumSamples2Write, uint32_t **Samples2Write);
    
    void WAVWriteHeader(PCMFile *PCM, BitBuffer *BitB);
    
#ifdef __cplusplus
}
#endif

#endif /* OVIA_WAVCommon_H */
