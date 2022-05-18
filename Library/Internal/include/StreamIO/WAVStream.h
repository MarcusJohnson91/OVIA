/*!
 @header              WAVStream.h
 @author              Marcus Johnson
 @copyright           2017+
 @version             1.0.0
 @brief               This header contains code for reading and writing lossless WAV audio files
 */

#pragma once

#ifndef OVIA_StreamIO_WAVStream_H
#define OVIA_StreamIO_WAVStream_H

#include "../../../OVIA/include/StreamIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef enum WAVSpeakerMask {
        WAVMask_FrontRight         = 0x1,
        WAVMask_FrontLeft          = 0x2,
        WAVMask_FrontCenter        = 0x4,
        WAVMask_LFE                = 0x8,
        WAVMask_BackLeft           = 0x10,
        WAVMask_BackRight          = 0x20,
        WAVMask_FrontLeftCenter    = 0x40,
        WAVMask_FrontRightCenter   = 0x80,
        WAVMask_BackCenter         = 0x100,
        WAVMask_SideLeft           = 0x200,
        WAVMask_SideRight          = 0x400,
        WAVMask_TopCenter          = 0x800,
        WAVMask_TopFrontLeft       = 0x1000,
        WAVMask_TopFrontCenter     = 0x2000,
        WAVMask_TopFrontRight      = 0x4000,
        WAVMask_TopBackLeft        = 0x8000,
        WAVMask_TopBackCenter      = 0x10000,
        WAVMask_TopBackRight       = 0x20000,
    } WAVSpeakerMask;

    typedef enum WAVCompressionFormats {
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
    } WAVCompressionFormats;

    typedef enum WAVChunkIDs { // odd sized chunks have a trailing 0 byte, and strings are null padded, this is for WAV, and W64.
        WAV_RIFF                      = 0x52494646,
        WAV_BW64                      = 0x42573634, // Literally the same as RIFF, just diff chunk id
        WAV_DS64                      = 0x64733634,
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
        WAV_JUNK                      = 0,
    } WAVChunkIDs;

    typedef struct WAVTags {
        UTF8 *Title;
        UTF8 *Artist;
        UTF8 *Album;
        UTF8 *ReleaseDate;
        UTF8 *Genre;
        UTF8 *CreationSoftware;
    } WAVTags;

    typedef struct BEXTChunk {
        UTF8 *Description;
        UTF8 *Originator;
        UTF8 *OriginatorRef;
        UTF8 *OriginatorDate;
    } BEXTChunk;
    
    typedef struct WAVOptions {
        WAVTags   *Info;
        BEXTChunk *BEXT;
        uint64_t   NumSamplesWritten;
        uint32_t   SampleRate;
        uint32_t   ByteRate;
        uint32_t   BlockAlign;
        uint32_t   SpeakerMask;
        uint32_t   ChannelMask;
        uint16_t   CompressionFormat;
        uint16_t   BlockAlignment;
        uint16_t   NumChannels;
        uint16_t   BitDepth;
        uint16_t   ValidBitsPerSample;
    } WAVOptions;

    WAVOptions *WAVOptions_Init(void);

    void WAVSkipPadding(BitBuffer *BitB, uint32_t SubChunkSize);

    void WAVOptions_Deinit(WAVOptions *Options);

#ifdef OVIA_StreamIO_WAV
    extern const OVIA_MagicIDs WAVSignature;

    const OVIA_MagicIDs WAVSignature = {
        .NumMagicIDs          = 1,
        .MagicIDs             = {
            [0]               = {
                .OffsetInBits = 0,
                .SizeInBits   = 32,
                .Signature    = (uint8_t[4]) {0x57, 0x41, 0x56, 0x45},
            },
        },
    };

    extern const OVIA_Extensions WAVExtensions;

    const OVIA_Extensions WAVExtensions = {
        .NumExtensions     = 2,
        .Extensions        = {
            OVIA_RegisterExtension(0, UTF32String("wave"))
            OVIA_RegisterExtension(1, UTF32String("wav"))
        }
    };

    extern const OVIA_MIMETypes WAVMIMETypes;

    const OVIA_MIMETypes WAVMIMETypes = {
        .NumMIMETypes     = 2,
        .MIMETypes        = {
            OVIA_RegisterMIMEType(0, UTF32String("audio/wav"))
            OVIA_RegisterMIMEType(1, UTF32String("audio/x-wav"))
        }
    };

#if defined(OVIA_StreamIO_Encode)
    extern const OVIA_Stream WAVMuxer;

    const OVIA_Stream WAVMuxer = {
        .MediaType             = MediaType_Audio2D,
        .MagicID               = &WAVSignature,
        .Function_Initialize   = WAVOptions_Init,
        .Function_Parse        = WAVParseMetadata,
        .Function_Decode       = WAVExtractSamples,
        .Function_Deinitialize = WAVOptions_Deinit,
    };
#endif /* OVIA_StreamIO_Encode */

#if defined(OVIA_StreamIO_Decode)
    extern const StreamIO_Demuxer WAVDemuxer;

    const StreamIO_Demuxer WAVDemuxer = {
        .MediaType             = MediaType_Audio2D,
        .MagicID               = &WAVSignature,
        .Function_Initialize   = WAVOptions_Init,
        .Function_Parse        = WAVParseMetadata,
        .Function_Decode       = WAVExtractSamples,
        .Function_Deinitialize = WAVOptions_Deinit,
    };
#endif /* OVIA_StreamIO_Decode */

#endif /* OVIA_StreamIO_WAV */
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_StreamIO_WAVStream_H */
