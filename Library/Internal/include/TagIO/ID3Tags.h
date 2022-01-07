/*!
 @header              ID3Tags.h
 @author              Marcus Johnson
 @copyright           2020+
 @version             1.0.0
 @brief               This header contains code for reading ID3v2 Tags.
 */

#pragma once

#ifndef OVIA_TagIO_ID3Tags_H
#define OVIA_TagIO_ID3Tags_H

#include "TagIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef enum ID3Flags {
        ID3Flags_Unspecified       = 0,
        ID3Flags_UnsyncIsUsed      = 1,
        ID3Flags_Compressed        = 2,
        ID3Flags_IsExtended        = 4,
        ID3Flags_IsExperimental    = 8,
        ID3Flags_HasFooter         = 16,
    } ID3Flags;

    typedef enum ID3Encodings {
        ID3Encoding_Unspecified    = 0,
        ID3Encoding_UTF8           = 1,
        ID3Encoding_UTF16          = 2,
        ID3Encoding_ISO_8859_1     = 3,
    } ID3Encodings;

    typedef enum ID3FrameMarkersOld { // ID3 version 2.2
        FrameMarkerOld_Unspecified = 0,
        FrameMarkerOld_CNT         = 1, // Play count
        FrameMarkerOld_COM         = 2, // Comment
        FrameMarkerOld_CRA         = 3, // Audio Encryption
        FrameMarkerOld_CRM         = 4, // Encrypted meta frame
        FrameMarkerOld_ETC         = 5, // Event Timing Codes
        FrameMarkerOld_EQU         = 6, // Equalization
        FrameMarkerOld_GEO         = 7, // General Encapsulated Object
        FrameMarkerOld_IPL         = 8, // Involved People List
        FrameMarkerOld_LNK         = 9, // Linked information
        FrameMarkerOld_MCI         = 10, // Music CD Identifier
        FrameMarkerOld_MLL         = 11, // MPEG Location Lookup
        FrameMarkerOld_PIC         = 12, // Attached Picture, generally cover art
        FrameMarkerOld_POP         = 13, // Popularimeter, popularity
        FrameMarkerOld_REV         = 14, // Reverb
        FrameMarkerOld_RVA         = 15, // Relative Volume Adjustment
        FrameMarkerOld_SLT         = 16, // Synchronized Lyrics
        FrameMarkerOld_STC         = 17, // Synchronized Tempo Codes
        FrameMarkerOld_TAL         = 18, // Album/Movie/Show Title
        FrameMarkerOld_TBP         = 19, // BPM beats per minute
        FrameMarkerOld_TCP         = 20, // Composer
        FrameMarkerOld_TCO         = 21, // Content Type
        FrameMarkerOld_TCR         = 22, // Copyright message
        FrameMarkerOld_TDA         = 23, // Date
        FrameMarkerOld_TDY         = 24, // Playlist Delay
        FrameMarkerOld_TEN         = 25, // Encoded by (person? software?)
        FrameMarkerOld_TFT         = 26, // File Type
        FrameMarkerOld_TIM         = 27, // Time (Date? Duration?)
        FrameMarkerOld_TKE         = 28, // Initial Key
        FrameMarkerOld_TLA         = 29, // Language(s)
        FrameMarkerOld_TLE         = 30, // Length
        FrameMarkerOld_TMT         = 31, // Media Type
        FrameMarkerOld_TOA         = 32, // Original Artist/Performer
        FrameMarkerOld_TOF         = 33, // Original Filename
        FrameMarkerOld_TOL         = 34, // Original lyracist/text writer
        FrameMarkerOld_TOR         = 35, // Original Release Year
        FrameMarkerOld_TOT         = 36, // Original Album/Movie/Show Title
        FrameMarkerOld_TP1         = 37, // Lead Artist/Performer/Soloist/Group
        FrameMarkerOld_TP2         = 38, // Band/Orchestra/Accompynment
        FrameMarkerOld_TP3         = 39, // Conductor/Performer Refinement
        FrameMarkerOld_TP4         = 40, // Intrepreted/Remixed/Modified by
        FrameMarkerOld_TPA         = 41, // Part of a Set
        FrameMarkerOld_TPB         = 42, // Publisher
        FrameMarkerOld_TRC         = 43, // ISRC
        FrameMarkerOld_TRD         = 44, // Recording Date(s)
        FrameMarkerOld_TRK         = 45, // Track number
        FrameMarkerOld_TSI         = 46, // SIZE
        FrameMarkerOld_TSS         = 47, // Software/Hardware and settings used during encoding
        FrameMarkerOld_TT1         = 48, // Content Group Description
        FrameMarkerOld_TT2         = 49, // Title/Songname/Content description
        FrameMarkerOld_TT3         = 50, // Subtitle/Description refinement
        FrameMarkerOld_TXT         = 51, // Lyrics/Text writer
        FrameMarkerOld_TXX         = 52, // User defined text frame
        FrameMarkerOld_TYE         = 53, // Year
        FrameMarkerOld_UFI         = 54, // Unique File Identifier
        FrameMarkerOld_ULT         = 55, // Unsynchronized Lyrics/Transcript
        FrameMarkerOld_WAF         = 56, // Official File Webpage
        FrameMarkerOld_WAR         = 57, // Official Artist Webpage
        FrameMarkerOld_WAS         = 58, // Official Audio Source Webpage
        FrameMarkerOld_WCM         = 59, // Commercial Information
        FrameMarkerOld_WCP         = 60, // Copyright Information
        FrameMarkerOld_WPB         = 61, // Publishers webpage
        FrameMarkerOld_WXX         = 62, // User defined URL
    } ID3FrameMarkers2_2;

    typedef enum ID3FrameMarkersNew { // ID3 version 2.3 and 2.4
        FrameMarkerNew_Unspecified = 0,
    } ID3FrameMarkersNew;

    typedef struct ID3Options {
        uint32_t     TagSize;
        uint32_t     ExtendedSize;
        uint8_t      VersionMajor;
        uint8_t      VersionMinor;
        ID3Flags     Flags;
        ID3Encodings Encoding;
    } ID3Options;

    void *ID3Options_Init(void);

    void ID3Options_SetPreferredVersion(ID3Options *Options, uint8_t MajorVersion, uint8_t MinorVersion);

    void ID3Options_SetPreferredEncoding(ID3Options *Options, ID3Encodings Encoding);

    void ID3Options_ReadHeader(ID3Options *Options, BitBuffer *BitB);

    void ID3Options_ReadFrames(ID3Options *Options, BitBuffer *BitB);

    void ID3Options_ReadFooter(ID3Options *Options, BitBuffer *BitB);

    void ID3Options_WriteHeader(ID3Options *Options, BitBuffer *BitB);

    void ID3Options_WriteFrames(ID3Options *Options, BitBuffer *BitB);

    void ID3Options_WriteFooter(ID3Options *Options, BitBuffer *BitB);

    void ID3Options_Deinit(ID3Options *Options);

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_TagIO_ID3Tags_H */
