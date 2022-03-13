/*!
 @header              OVIATypes.h
 @author              Marcus Johnson
 @copyright           2022+
 @version             1.0.0
 @brief               This header contains code for file extensions, MIME types, and file signatures.
 */

#pragma once

#ifndef OVIA_OVIATypes_h
#define OVIA_OVIATypes_h

#include "../../OVIA.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef struct OVIA_Signature {
        const uint64_t OffsetInBits;
        const uint8_t  SizeInBits;
        const uint8_t *Signature;
    } OVIA_Signature;

    typedef struct OVIA_MagicIDs {
        const uint64_t          NumMagicIDs;
        const OVIA_Signature    MagicIDs[];
    } OVIA_MagicIDs;

    typedef struct OVIA_ExtensionInfo {
        const uint8_t                Size;
        UTF32                       *Extension;
    } OVIA_ExtensionInfo;

    typedef struct OVIA_Extensions {
        const uint8_t            NumExtensions;
        const OVIA_ExtensionInfo Extensions[];
    } OVIA_Extensions;

    typedef struct OVIA_MIMEInfo {
        const uint8_t                Size;
        UTF32                       *MIMEType;
    } OVIA_MIMEInfo;

    typedef struct OVIA_MIMETypes {
        const uint8_t            NumMIMETypes;
        const OVIA_MIMEInfo      MIMETypes[];
    } OVIA_MIMETypes;

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_OVIATypes_h */
