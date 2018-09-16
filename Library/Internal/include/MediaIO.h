#include <stdint.h>
#include <stdbool.h>

#pragma  once

#ifndef  FoundationIO_ContainerIO_H
#define  FoundationIO_ContainerIO_H

#ifdef __cplusplus
extern "C" {
#endif
    
    /*!
     @header                ContainerIO.h
     @author                Marcus Johnson
     @copyright             2018+
     @version               1.0.0
     @brief                 This header contains code for containers (type generic arrays)
     */
    
    typedef enum Audio_ChannelMask {
        Audio_UnknownChannel        = 0,
        LeftChannel                 = 1,
        CenterChannel               = 2,
        RightChannel                = 4,
        LFEChannel                  = 8,
        SurroundLeftChannel         = 16,
        SurroundRightChannel        = 32,
        RearLeftChannel             = 64,
        RearRightChannel            = 128,
    } Audio_ChannelMask;
    
    typedef enum Audio_Types {
        AudioContainer_UnknownType  = 0,
        AudioContainer_UInteger8    = 1,
        AudioContainer_SInteger8    = 2,
        AudioContainer_UInteger16   = 3,
        AudioContainer_SInteger16   = 4,
        AudioContainer_UInteger32   = 5,
        AudioContainer_SInteger32   = 6,
    } Audio_Types;
    
    typedef struct          AudioContainer AudioContainer;
    
    /*!
     @abstract                             "Initalizes an empty AudioContainer".
     @param                 Type           "A type from AudioContinerTypes".
     @param                 BitDepth       "The number of actual bits to store a sample, e.g. 24 bit audio has a bit depth of 24, not 32".
     @param                 NumChannels    "The number of channels in the audio".
     @param                 SampleRate     "The number of samples in one second of audio".
     @param                 NumSamples     "NumSamples is the number of channel independent samples, e.g. X samples is BitDepth * NumChnnels * X".
     */
    AudioContainer         *AudioContainer_Init(Audio_Types Type, uint64_t BitDepth, uint64_t NumChannels, uint64_t SampleRate, uint64_t NumSamples);
    
    /*!
     @abstract                             "Sets the channel type for each channel index".
     @param                 Audio          "A pointer to the instance of an AudioContainer in question".
     @param                 Index          "Which index does the channel mask apply to"?
     @param                 ChannelMask    "What is the channel mask"?
     */
    void                    AudioContainer_SetChannelMask(AudioContainer *Audio, uint64_t Index, Audio_ChannelMask ChannelMask);
    
    /*!
     @abstract                             "Returns the number of audio channels".
     @param                 Audio          "A pointer to the instance of an AudioContainer in question".
     */
    uint64_t                AudioContainer_GetNumChannels(AudioContainer *Audio);
    
    /*!
     @abstract                             "Returns the number of channel-agnostic samples in one second".
     @param                 Audio          "A pointer to the instance of an AudioContainer in question".
     */
    uint64_t                AudioContainer_GetSampleRate(AudioContainer *Audio);
    
    /*!
     @abstract                             "Returns the number of bits required to represent a audio sample".
     @param                 Audio          "A pointer to the instance of an AudioContainer in question".
     */
    uint64_t                AudioContainer_GetBitDepth(AudioContainer *Audio);
    
    /*!
     @abstract                             "Returns the number of channel-agnostic audio samples stored in the container".
     @param                 Audio          "A pointer to the instance of an AudioContainer in question".
     */
    uint64_t          AudioContainer_GetNumSamples(AudioContainer *Audio);
    uint64_t                AudioContainer_GetNumSamples(AudioContainer *Audio);
    
    /*!
     @abstract                       "Sets the channel type for each channel index".
     @param           Audio          "A pointer to the instance of an AudioContainer".
     @param           Index          "Which index does the channel mask apply to"?
     @abstract                             "Gets the index that channel X is in".
     @remark                               "If the channel is not present in the stream, we return Audio_UnknownChannel".
     @param                 Audio          "A pointer to the instance of an AudioContainer in question".
     @param                 ChannelMask    "The actual channel you're trying to get".
     */
    uint64_t                AudioContainer_GetChannelsIndex(AudioContainer *Audio, Audio_ChannelMask ChannelMask);
    
    /*!
     @abstract                             "Gets the type of the array contained by the AudioContainer".
     @param                 Audio          "A pointer to the instance of an AudioContainer in question".
     */
    Audio_Types             AudioContainer_GetType(AudioContainer *Audio);
    
    /*!
     @abstract                             "Gets a pointer to the array of samples".
     @remark                               "You need to cast the pointer to the correct type you got from AudioContainer_GetType".
     @param                 Audio          "A pointer to the instance of an AudioContainer in question".
     */
    void                  **AudioContainer_GetArray(AudioContainer *Audio);
    
    /*!
     @abstract                             "Returns the average of the samples in the buffer".
     @param                 Audio          "A pointer to the instance of an AudioContainer in question".
     @param                 Channel        "Which index should we get average"?
     */
    int64_t                 AudioContainer_GetAverage(AudioContainer *Audio, uint64_t Channel);
    
    /*!
     @abstract                             "Returns the highest valued sample in the buffer".
     @param                 Audio          "A pointer to the instance of an AudioContainer in question".
     @param                 Channel        "Which index should we get the highest value from"?
     */
    int64_t                 AudioContainer_GetMax(AudioContainer *Audio, uint64_t Channel);
    
    /*!
     @abstract                             "Returns the lowest valued sample in the buffer".
     @param                 Audio          "A pointer to the instance of an AudioContainer in question".
     @param                 Channel        "Which index should we get the lowest value from"?
     */
    int64_t                 AudioContainer_GetMin(AudioContainer *Audio, uint64_t Channel);
    
    /*!
     @abstract                             "Deinitializes an AudioContainer, and any samples stored within it".
     @param                 Audio          "A pointer to the instance of an AudioContainer in question".
     */
    void                    AudioContainer_Deinit(AudioContainer *Audio);
    
    typedef enum Image_Views {
        UnknownView                 = 0,
        View2D                      = 1,
        LeftEyeView                 = 2,
        RightEyeView                = 4,
    } Image_Views;
    
    typedef enum Image_ChannelMask {
        Image_UnknownChannel        = 0x0,
        Grayscale2D                 = 0x1,
        Red2D                       = 0x2,
        Green2D                     = 0x4,
        Green_2_2D                  = 0x8,
        Blue2D                      = 0x10,
        Alpha2D                     = 0x20,
        Luma2D                      = 0x40,
        Chroma_1_2D                 = 0x80,
        Chroma_2_2D                 = 0x100,
        Chroma_3_2D                 = 0x200,
        Grayscale3D_L               = 0x400,
        Red3D_L                     = 0x800,
        Green3D_L                   = 0x1000,
        Blue3D_L                    = 0x2000,
        Green3D_2_L                 = 0x4000,
        Alpha3D_L                   = 0x8000,
        Luma3D_L                    = 0x10000,
        Chroma_1_3D_L               = 0x20000,
        Chroma_2_3D_L               = 0x40000,
        Chroma_3_3D_L               = 0x80000,
        Grayscale3D_R               = 0x100000,
        Red3D_R                     = 0x200000,
        Green3D_R                   = 0x400000,
        Blue3D_R                    = 0x800000,
        Green3D_2_R                 = 0x1000000,
        Alpha3D_R                   = 0x2000000,
        Luma3D_R                    = 0x4000000,
        Chroma_1_3D_R               = 0x8000000,
        Chroma_2_3D_R               = 0x10000000,
        Chroma_3_3D_R               = 0x20000000,
    } Image_ChannelMask;
    
    typedef enum Image_Types {
        ImageContainer_UnknownType  = 0,
        ImageContainer_2DUInteger8  = 1,
        ImageContainer_2DSInteger8  = 2,
        ImageContainer_2DUInteger16 = 3,
        ImageContainer_2DSInteger16 = 4,
        ImageContainer_3DUInteger8  = 5,
        ImageContainer_3DSInteger8  = 6,
        ImageContainer_3DUInteger16 = 7,
        ImageContainer_3DSInteger16 = 8,
    } Image_Types;
    
    typedef struct          ImageContainer ImageContainer;
    
    /*!
     @abstract                             "Initalizes an empty ImageContainer".
     @param                 Type           "The type of array to create".
     @param                 BitDepth       "The number of actual bits to store a sample, e.g. a 14 bit image has a bit depth of 14, not 16".
     @remark                               "All channels in an image must have the same bit depth, padding will be added if necessary".
     @param                 NumChannels    "The number of channels in the image".
     @param                 Width          "The number of pixels making up one row".
     @param                 Height         "The number of pixels making up one column".
     */
    ImageContainer         *ImageContainer_Init(Image_Types Type, uint64_t BitDepth, uint64_t NumChannels, uint64_t Width, uint64_t Height);
    
    /*!
     @abstract                             "Sets the channel type for each channel index".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     @param                 Index          "Which index does the channel mask apply to"?
     @param                 ChannelMask    "What is the channel mask"?
     */
    void                    ImageContainer_SetChannelMask(ImageContainer *Image, uint64_t Index, Image_ChannelMask ChannelMask);
    
    /*!
     @abstract                             "Returns the number of pixels in one row of this image".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     */
    uint64_t                ImageContainer_GetWidth(ImageContainer *Image);
    
    /*!
     @abstract                             "Returns the number of pixels in one column of this image".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     */
    uint64_t                ImageContainer_GetHeight(ImageContainer *Image);
    
    /*!
     @abstract                             "Returns the number of bits needed to represent the image in this container".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     */
    uint64_t                ImageContainer_GetBitDepth(ImageContainer *Image);
    
    /*!
     @abstract                             "Returns the number of channels contained in this image".
     @remark                               "If there are multiple views, it only returns the number of channels per view".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     */
    uint64_t                ImageContainer_GetNumChannels(ImageContainer *Image);
    
    /*!
     @abstract                             "Gets the channel index for the specified mask".
     @remark                               "If the mask is not present in the ImageContainer, we return the number of channels, which is outside the valid range".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     @param                 ChannelMask    "Which channel are you trying to access"?
     */
    uint64_t                ImageContainer_GetChannelsIndex(ImageContainer *Image, Image_ChannelMask ChannelMask);
    
    /*!
     @abstract                             "Gets the number of views in an ImageContainer".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     */
    uint64_t                ImageContainer_GetNumViews(ImageContainer *Image);
    
    /*!
     @abstract                             "Gets the views index for the specified mask".
     @remark                               "If the mask is not present in the ImageContainer, we return the number of views, which is outside the valid range".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     @param                 View           "Which view are you trying to access"?
     */
    uint64_t                ImageContainer_GetViewsIndex(ImageContainer *Image, Image_Views View);
    
    /*!
     @abstract                             "Gets the type of the array contained by the ImageContainer".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     */
    Image_Types             ImageContainer_GetType(ImageContainer *Image);
    
    /*!
     @abstract                             "Gets a pointer to the array of pixels".
     @remark                               "You need to cast the pointer to the correct type you got from ImageContainer_GetType".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     */
    void                ****ImageContainer_GetArray(ImageContainer *Image);
    
    /*!
     @abstract                             "Sets a pointer to the array of pixels".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     @param                 Array          "A pointer to the new array".
     */
    void                    ImageContainer_SetArray(ImageContainer *Image, void ****Array);
    
    /*!
     @abstract                             "Returns the average value (rounded) of the pixels in this image".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     @param                 ViewMask       "Which view are you trying to get the average from"?
     @param                 ChannelMask    "Which channel should we get the average from"?
     */
    int64_t                 ImageContainer_GetAverage(ImageContainer *Image, Image_Views ViewMask, Image_ChannelMask ChannelMask);
    
    /*!
     @abstract                             "Returns the highest value pixel in this image".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     @param                 ViewMask       "Which view are you trying to get the maximum from"?
     @param                 ChannelMask    "Which channel should we get the maximum from"?
     */
    int64_t                 ImageContainer_GetMax(ImageContainer *Image, Image_Views ViewMask, Image_ChannelMask ChannelMask);
    
    /*!
     @abstract                             "Returns the lowest value pixel in this image".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     @param                 ViewMask       "Which view are you trying to get the minimum from"?
     @param                 ChannelMask    "Which channel should we get the minimum from"?
     */
    int64_t                 ImageContainer_GetMin(ImageContainer *Image, Image_Views ViewMask, Image_ChannelMask ChannelMask);
    
    /*!
     @abstract                             "Rotates an Image, either Vertically, Horizontally, or both".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     @param                 VerticalFlip   "Top becomes bottom and vice versa".
     @param                 HorizontalFlip "left becomes right and vice versa".
     */
    void                    ImageContainer_Flip(ImageContainer *Image, bool VerticalFlip, bool HorizontalFlip);
    
    /*!
     @abstract                             "Crops an Image".
     @remark                               "Allocates a new array and copies over just the content requested".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     @param                 Top            "The offset from the top of the image".
     @param                 Bottom         "The offset from the bottom of the image".
     @param                 Left           "The offset from the left of the image".
     @param                 Right          "The offset from the right of the image".
     */
    void                    ImageContainer_Crop(ImageContainer *Image, uint64_t Top, uint64_t Bottom, uint64_t Left, uint64_t Right);
    
    /*!
     @abstract                             "Deinitializes the ImageContainer pointed to".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     */
    void                    ImageContainer_Deinit(ImageContainer *Image);
    
    typedef struct          ImageHistogram ImageHistogram;
    
    /*!
     @abstract                             "Creates a ImageHistogram".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     @return                               "Returns the newly created Histogram".
     */
    ImageHistogram         *ImageHistogram_Init(ImageContainer *Image);
    
    /*!
     @abstract                             "Gets a pointer to the histogram data".
     @param                 Histogram      "A pointer to the instance of an ImageHistogram in question".
     @return                               "Returns a pointer to the histogram data".
     */
    void                 ***ImageHistogram_GetArray(ImageHistogram *Histogram);
    
    /*!
     @abstract                             "Sets a pointer to the histogram data".
     @param                 Histogram      "A pointer to the instance of an ImageHistogram in question".
     @param                 Array          "A pointer to the histogram data".
     */
    void                    ImageHistogram_SetArray(ImageHistogram *Histogram, void ***Array);
    
    /*!
     @abstract                             "Generates a histogram from an image".
     @param                 Image          "A pointer to the instance of an ImageContainer in question".
     @return                               "Returns the newly created Histogram".
     */
    ImageHistogram         *ImageHistogram_GenerateHistogram(ImageContainer *Image);
    
#ifdef __cplusplus
}
#endif

#endif   /* FoundationIO_CommandLineIO_H */
