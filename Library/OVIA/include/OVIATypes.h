/*!
 @header              OVIATypes.h
 @author              Marcus Johnson
 @copyright           2022+
 @SPDX-License-Identifier: Apache-2.0
 @version             1.0.0
 @brief               This header contains code for file extensions, MIME types, and file signatures.
 */

#pragma once

#ifndef OVIA_OVIATypes_H
#define OVIA_OVIATypes_H

#include "../../OVIA.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef void* (*OVIA_Options);

    typedef void (*MediaIO_Container);

    typedef OVIA_Options (*OVIA_Function_Init)(void);

    typedef void  (*OVIA_Function_Parse)(OVIA_Options Options, BitBuffer *BitB);

    typedef void  (*OVIA_Function_Coder)(OVIA_Options Options, BitBuffer *BitB, MediaIO_Container Container);

    typedef void  (*OVIA_Function_Transform)(MediaIO_Container *Container);

    typedef void  (*OVIA_Function_Deinit)(OVIA_Options Options);

    typedef struct OVIA_Signature {
        const uint64_t OffsetInBits;
        const uint8_t  SizeInBits;
        const uint8_t *Signature;
    } OVIA_Signature;

#define OVIA_RegisterSignature(OffsetNumBits, SizeNumBits, SignatureWithBraces)  \
[__COUNTER__] = { \
.OffsetInBits = OffsetNumBits, \
.SizeInBits = SizeNumBits \
.Signature = SignatureWithBraces \
},

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

#define OVIA_RegisterExtension(ExtensionString)  \
    [__COUNTER__] = { \
        .Size = PlatformIO_GetStringSizeInCodeUnits(ExtensionString) \
        .Extension = UTF32String(ExtensionString) \
    },


    typedef struct OVIA_MIMEInfo {
        const uint8_t                Size;
        UTF32                       *MIMEType;
    } OVIA_MIMEInfo;

    typedef struct OVIA_MIMETypes {
        const uint8_t            NumMIMETypes;
        const OVIA_MIMEInfo      MIMETypes[];
    } OVIA_MIMETypes;

#define OVIA_RegisterMIMEType(MIMEString)  \
    [__COUNTER__] = { \
        .Size = PlatformIO_GetStringSizeInCodeUnits(MIMEString) \
        .Extension = UTF32String(MIMEString) \
    },

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_OVIATypes_H */
