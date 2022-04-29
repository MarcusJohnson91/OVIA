/*!
 @header              TransformIO.h
 @author              Marcus Johnson
 @copyright           2020+
 @version             1.0.0
 @brief               This header contains code for reusable reversible image transforms.
 */

#pragma once

#ifndef OVIA_TransformIO_H
#define OVIA_TransformIO_H

#include "OVIATypes.h"
#include "MediaIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef enum Transform_BijectiveType {
        BijectiveType_Unknown        = 0,
        BijectiveType_IsBijective    = 1,
        BijectiveType_IsNotBijective = 2,
    } Transform_BijectiveType;

    typedef struct OVIAColorTransform { // Ignore Alpha channels
        const OVIA_Function_Transform   Function_Encode;
        const OVIA_Function_Transform   Function_Decode;
        const MediaIO_ImageMask         InputChannels;
        const MediaIO_ImageMask         OutputChannels;
        const OVIA_ColorTransforms      Transform;
        const Transform_BijectiveType   BijectiveType;
    } OVIAColorTransform;

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_TransformIO_H */
