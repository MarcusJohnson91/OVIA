/*!
 @header              TransformIO.h
 @author              Marcus Johnson
 @copyright           2020+
 @version             1.0.0
 @brief               This header contains code for reusable reversible image transforms.
 */

#include "MediaIO.h"

#pragma once

#ifndef OVIA_TransformIO_H
#define OVIA_TransformIO_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    typedef enum Transform_BijectiveType {
        BijectiveType_Unknown        = 0,
        BijectiveType_IsBijective    = 1,
        BijectiveType_IsNotBijective = 2,
    } Transform_BijectiveType;

    typedef void (*ColorTransformFunction)(ImageContainer *);

    typedef struct OVIAColorTransform { // Ignore Alpha channels
        const ColorTransformFunction    Function_Encode;
        const ColorTransformFunction    Function_Decode;
        const MediaIO_ImageChannelMask  InputChannels;
        const MediaIO_ImageChannelMask  OutputChannels;
        const OVIA_ColorTransforms      Transform;
        const Transform_BijectiveType   BijectiveType;
    } OVIAColorTransform;

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_TransformIO_H */
