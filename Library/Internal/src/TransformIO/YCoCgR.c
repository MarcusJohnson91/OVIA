#include "../../include/Private/MediaIO.h"
#include "../../include/Private/TransformIO/TransformIO.h"
#include "../../../Dependencies/FoundationIO/Library/include/TextIO/LogIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    void YCoCgR_Encode(ImageContainer *Image) {
        if (Image != NULL) {
            ImageChannelMap *Map  = ImageContainer_GetChannelMap(Image);
            uint8_t  NumChannels  = ImageChannelMap_GetNumChannels(Map);
            if (NumChannels >= 3 && NumChannels <= 4) {
                uint8_t  RedIndex                             = ImageChannelMap_GetChannelsIndex(Map, ImageMask_Red);
                uint8_t  GreenIndex                           = ImageChannelMap_GetChannelsIndex(Map, ImageMask_Green);
                uint8_t  BlueIndex                            = ImageChannelMap_GetChannelsIndex(Map, ImageMask_Blue);

                uint8_t  NumViews                             = ImageChannelMap_GetNumViews(Map);
                uint8_t  BitDepth                             = ImageType_GetBitDepth(ImageContainer_GetType(Image));
                uint64_t Width                                = ImageContainer_GetWidth(Image);
                uint64_t Height                               = ImageContainer_GetHeight(Image);

                if (BitDepth <= 8) {
                    uint8_t ****Array                         = (uint8_t****) ImageContainer_GetArray(Image);

                    for (uint8_t View = 0; View < NumViews; View++) {
                        for (uint64_t W = 0; W < Width; W++) {
                            for (uint64_t H = 0; H < Height; H++) {
                                uint8_t Red                   = Array[View][W][H][RedIndex];
                                uint8_t Green                 = Array[View][W][H][GreenIndex];
                                uint8_t Blue                  = Array[View][W][H][BlueIndex];

                                uint8_t Co                    = Red   - Blue;
                                uint8_t Temp                  = Blue  + (Co >> 1);
                                uint8_t Cg                    = Green - Temp;
                                uint8_t Y                     = Temp  + (Cg >> 1);

                                Array[View][W][H][RedIndex]   = Co;
                                Array[View][W][H][GreenIndex] = Y;
                                Array[View][W][H][BlueIndex]  = Cg;
                            }
                        }
                    }
                } else if (BitDepth <= 16) {
                    uint16_t ****Array  = (uint16_t****) ImageContainer_GetArray(Image);

                    for (uint8_t View = 0; View < NumViews; View++) {
                        for (uint64_t W = 0; W < Width; W++) {
                            for (uint64_t H = 0; H < Height; H++) {
                                uint16_t Red                  = Array[View][W][H][RedIndex];
                                uint16_t Green                = Array[View][W][H][GreenIndex];
                                uint16_t Blue                 = Array[View][W][H][BlueIndex];

                                uint16_t Co                   = Red   - Blue;
                                uint16_t Temp                 = Blue  + (Co >> 1);
                                uint16_t Cg                   = Green - Temp;
                                uint16_t Y                    = Temp  + (Cg >> 1);

                                Array[View][W][H][RedIndex]   = Co;
                                Array[View][W][H][GreenIndex] = Y;
                                Array[View][W][H][BlueIndex]  = Cg;
                            }
                        }
                    }
                }

                ImageChannelMap *NewMap                       = ImageChannelMap_Init(NumViews, NumChannels);
                ImageChannelMap_AddMask(NewMap, RedIndex, ImageMask_Chroma1);
                ImageChannelMap_AddMask(NewMap, GreenIndex, ImageMask_Luma);
                ImageChannelMap_AddMask(NewMap, BlueIndex, ImageMask_Chroma2);
                ImageContainer_SetChannelMap(Image, NewMap);
            } else {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("NumChannels %hhu must be 3 or 4"), NumChannels);
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        }
    }

    void YCoCgR_Decode(ImageContainer *Image) { // Not Bijective, requires an additional bit in the Chroma channels
        if (Image != NULL) {
            ImageChannelMap *Map  = ImageContainer_GetChannelMap(Image);
            uint8_t  NumChannels  = ImageChannelMap_GetNumChannels(Map);
            if (NumChannels >= 3 && NumChannels <= 4) {
                uint8_t  Chroma1Index                         = ImageChannelMap_GetChannelsIndex(Map, ImageMask_Chroma1);
                uint8_t  LumaIndex                            = ImageChannelMap_GetChannelsIndex(Map, ImageMask_Luma);
                uint8_t  Chroma2Index                         = ImageChannelMap_GetChannelsIndex(Map, ImageMask_Chroma2);

                uint8_t  NumViews                             = ImageChannelMap_GetNumViews(Map);
                uint8_t  BitDepth                             = ImageType_GetBitDepth(ImageContainer_GetType(Image));
                uint64_t Width                                = ImageContainer_GetWidth(Image);
                uint64_t Height                               = ImageContainer_GetHeight(Image);

                if (BitDepth <= 8) {
                    uint8_t ****Array                         = (uint8_t****) ImageContainer_GetArray(Image);

                    for (uint8_t View = 0; View < NumViews; View++) {
                        for (uint64_t W = 0; W < Width; W++) {
                            for (uint64_t H = 0; H < Height; H++) {
                                uint8_t Luma                    = Array[View][W][H][LumaIndex];
                                uint8_t Co                      = Array[View][W][H][Chroma1Index];
                                uint8_t Cg                      = Array[View][W][H][Chroma2Index];

                                uint8_t Temp                    = Luma - (Cg >> 1);
                                uint8_t Green                   = Cg + Temp;
                                uint8_t Blue                    = Temp - (Co >> 1);
                                uint8_t Red                     = Blue + Co;

                                Array[View][W][H][LumaIndex]    = Red;
                                Array[View][W][H][Chroma1Index] = Green;
                                Array[View][W][H][Chroma2Index] = Blue;
                            }
                        }
                    }
                } else if (BitDepth <= 16) {
                    uint16_t ****Array  = (uint16_t****) ImageContainer_GetArray(Image);

                    for (uint8_t View = 0; View < NumViews; View++) {
                        for (uint64_t W = 0; W < Width; W++) {
                            for (uint64_t H = 0; H < Height; H++) {
                                uint16_t Luma                   = Array[View][W][H][LumaIndex];
                                uint16_t Co                     = Array[View][W][H][Chroma1Index];
                                uint16_t Cg                     = Array[View][W][H][Chroma2Index];

                                uint16_t Temp                   = Luma - (Cg >> 1);
                                uint16_t Green                  = Cg + Temp;
                                uint16_t Blue                   = Temp - (Co >> 1);
                                uint16_t Red                    = Blue + Co;

                                Array[View][W][H][LumaIndex]    = Red;
                                Array[View][W][H][Chroma1Index] = Green;
                                Array[View][W][H][Chroma2Index] = Blue;
                            }
                        }
                    }
                }
                ImageChannelMap *NewMap                             = ImageChannelMap_Init(NumViews, NumChannels);
                ImageChannelMap_AddMask(NewMap, Chroma1Index, ImageMask_Red);
                ImageChannelMap_AddMask(NewMap, LumaIndex, ImageMask_Green);
                ImageChannelMap_AddMask(NewMap, Chroma2Index, ImageMask_Blue);
                ImageContainer_SetChannelMap(Image, NewMap);
            } else {
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("NumChannels %u must be 3 or 4"), NumChannels);
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        }
    }

    OVIAColorTransform TransformYCoCgR = {
        .Transform       = ColorTransform_RCT,
        .BijectiveType   = BijectiveType_IsNotBijective,
        .Function_Encode = YCoCgR_Encode,
        .Function_Decode = YCoCgR_Decode,
        .InputChannels   = ImageMask_Red | ImageMask_Green | ImageMask_Blue | ImageMask_Alpha | ImageMask_2D | ImageMask_3D_L | ImageMask_3D_R,
        .OutputChannels  = ImageMask_Luma | ImageMask_Chroma1 | ImageMask_Chroma2 | ImageMask_Alpha | ImageMask_2D | ImageMask_3D_L | ImageMask_3D_R,
    };

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
