#include "../../../include/Private/TransformIO/TransformIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/TextIO/LogIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    void RCT_Encode(ImageContainer *Image) {
        if (Image != NULL) {
            ImageChannelMap *Map                              = ImageContainer_GetChannelMap(Image);
            uint8_t NumChannels                               = ImageChannelMap_GetNumChannels(Map);
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
                                
                                // RGB = 00BBFF, RCT = 459D44, YCoCgR =

                                /*
                                4 channel RCT: Possibly use Mid-Side coding on the Green channels?
                                Cr = Red - (Green1 + Green2)
                                Y  = Floor(Red + (Green1 + Green2) + Blue) / 4
                                Cb = Blue - (Green1 + Green2)

                                Mid-Side RCT:
                                GreenM = Green1 + Green2
                                GreenS = Green1 - Green2

                                Cr     = Red - GreenS
                                Y      = Floor((Red + GreenM + Blue) / 4)
                                Cb     = Blue - GreenS

                                That seems pretty damn simple.

                                Lets do a test and see how it performs:

                                R = C0, G1 = 20, G2 = 21, B = A2

                                GreenM = 0x41
                                GreenS = 0xFF

                                Cr     = 0xC1
                                Y      = 0x68
                                Cb     = 0xA3

                                Decode:

                                Green? = Y - Floor((Cb + Cr) / 4) = 0x68 - Floor(() / 4) = 0x59

                                 Ok, so the issue is that you can' cram 4 channels into 3.

                                 So, let's create a new transform that is similar to RCT

                                 Why not have two luma channels? because they contain the most information.

                                 why
                                */
                                
                                uint8_t Cr                    = Red - Green;
                                uint8_t Y                     = FloorD((Red + (Green * 2) + Blue) / 4);
                                uint8_t Cb                    = Blue - Green;
                                
                                Array[View][W][H][RedIndex]   = Cr;
                                Array[View][W][H][GreenIndex] = Y;
                                Array[View][W][H][BlueIndex]  = Cb;
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
                                
                                uint16_t Cr                   = Red - Green;
                                uint16_t Y                    = (Red + (Green * 2) + Blue) / 4;
                                uint16_t Cb                   = Blue - Green;
                                
                                Array[View][W][H][RedIndex]   = Cr;
                                Array[View][W][H][GreenIndex] = Y;
                                Array[View][W][H][BlueIndex]  = Cb;
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
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("NumChannels %u must be 3 or 4"), NumChannels);
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        }
    }
    
    
    void RCT_Decode(ImageContainer *Image) {
        if (Image != NULL) {
            ImageChannelMap *Map  = ImageContainer_GetChannelMap(Image);
            uint8_t  NumChannels  = ImageChannelMap_GetNumChannels(Map);
            if (NumChannels >= 3 && NumChannels <= 4) {
                ImageChannelMap *Map                          = ImageContainer_GetChannelMap(Image);
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
                                uint8_t Luma                    = Array[View][W][H][LumaIndex];    // 69
                                uint8_t Cr                      = Array[View][W][H][Chroma1Index]; // 157
                                uint8_t Cb                      = Array[View][W][H][Chroma2Index]; // 68
                                
                                // RGB = 00BBFF, RCT = 459D44, YCoCgR = 15D|01|BC


                                
                                uint8_t Green                   = Luma - FloorD((Cb + Cr) / 4); // 69 - 56 = 13
                                uint8_t Red                     = Cr   + Green;
                                uint8_t Blue                    = Cb   + Green;
                                
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
                                uint16_t Cr                     = Array[View][W][H][Chroma1Index];
                                uint16_t Cb                     = Array[View][W][H][Chroma2Index];
                                
                                uint16_t Green                  = Luma - FloorD((Cb + Cr) / 4);
                                uint16_t Red                    = Cr   + Green;
                                uint16_t Blue                   = Cb   + Green;
                                
                                Array[View][W][H][LumaIndex]    = Red;
                                Array[View][W][H][Chroma1Index] = Green;
                                Array[View][W][H][Chroma2Index] = Blue;
                            }
                        }
                    }
                }
                
                ImageChannelMap *NewMap                       = ImageChannelMap_Init(NumViews, NumChannels);
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

    OVIAColorTransform TransformRCT = {
        .Transform       = ColorTransform_RCT,
        .BijectiveType   = BijectiveType_IsBijective,
        .Function_Encode = RCT_Encode,
        .Function_Decode = RCT_Decode,
        .InputChannels   = ImageMask_Red | ImageMask_Green | ImageMask_Blue | ImageMask_Alpha | ImageMask_2D | ImageMask_3D_L | ImageMask_3D_R,
        .OutputChannels  = ImageMask_Luma | ImageMask_Chroma1 | ImageMask_Chroma2 | ImageMask_Alpha | ImageMask_2D | ImageMask_3D_L | ImageMask_3D_R,
    };
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
