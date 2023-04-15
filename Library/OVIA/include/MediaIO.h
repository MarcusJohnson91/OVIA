/*!
 @header              MediaIO.h
 @author              Marcus Johnson
 @copyright           2018+
 @version             1.1.0
 @brief               This header contains code for media (type generic arrays)
 */

#pragma once

#ifndef OVIA_MediaIO_H
#define OVIA_MediaIO_H

#include "../../OVIA.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    /*!
     @enum         MediaIO_AudioMask
     @abstract                                      Defines the PlatformIOTypes values, OR-able.
     @constant     AudioMask_Unspecified            Invalid AudioMask, exists solely to tell when it hasn't been set.
     @constant     AudioMask_FrontLeft              The channel's location is the front left.
     @constant     AudioMask_FrontRight             The channel's location is the front right.
     @constant     AudioMask_FrontCenter            The channel's location is the front center.
     @constant     AudioMask_LFE                    The channel's contains the Low Frequency Effects for subwoofers.
     @constant     AudioMask_SurroundLeft           The channel's location is the side left.
     @constant     AudioMask_SurroundRight          The channel's location is the side right.
     @constant     AudioMask_SurroundCenter         The channel's location is the side center.
     @constant     AudioMask_RearLeft               The channel's location is the rear left.
     @constant     AudioMask_RearRight              The channel's location is the rear right.
     @constant     AudioMask_RearCenter             The channel's location is the rear center.
     @constant     AudioMask_FrontCenterLeft        The channel's location is the front center left.
     @constant     AudioMask_FrontCenterRight       The channel's location is the front center right.
     @constant     AudioMask_TopCenter              The channel's location is the top center.
     @constant     AudioMask_TopFrontLeft           The channel's location is the top front left.
     @constant     AudioMask_TopFrontRight          The channel's location is the top front right.
     @constant     AudioMask_TopFrontCenter         The channel's location is the top front center.
     @constant     AudioMask_TopRearLeft            The channel's location is the top rear left.
     @constant     AudioMask_TopRearRight           The channel's location is the top rear right.
     @constant     AudioMask_TopRearCenter          The channel's location is the top rear center.
     @constant     AudioMask_StereoLeft             RF64 Extension, Stereo Downmix, Left.
     @constant     AudioMask_StereoRight            RF64 Extension, Stereo Downmix, Right.
     */
    typedef enum MediaIO_AudioMask : uint32_t {
                   AudioMask_Unspecified            = 0,
                   AudioMask_FrontLeft              = 1,
                   AudioMask_FrontRight             = 2,
                   AudioMask_FrontCenter            = 4,
                   AudioMask_LFE                    = 8,
                   AudioMask_SurroundLeft           = 16,
                   AudioMask_SurroundRight          = 32,
                   AudioMask_SurroundCenter         = 64,
                   AudioMask_RearLeft               = 128,
                   AudioMask_RearRight              = 256,
                   AudioMask_RearCenter             = 512,
                   AudioMask_FrontCenterLeft        = 1024,
                   AudioMask_FrontCenterRight       = 2048,
                   AudioMask_TopCenter              = 4096,
                   AudioMask_TopFrontLeft           = 8192,
                   AudioMask_TopFrontRight          = 16384,
                   AudioMask_TopFrontCenter         = 32768,
                   AudioMask_TopRearLeft            = 65536,
                   AudioMask_TopRearRight           = 131072,
                   AudioMask_TopRearCenter          = 262144,
                   AudioMask_StereoLeft             = 524288,
                   AudioMask_StereoRight            = 1048576,
    } MediaIO_AudioMask;
    
    /*!
     @enum         MediaIO_ImageMask
     @abstract                                      Defines the ChannelMask values.
     @constant     ImageMask_Unspecified            Invalid ImageMask, exists solely to tell when it hasn't been set.
     @constant     ImageMask_2D                     The image has two dimensions.
     @constant     ImageMask_3D_L                   The image has three dimensions, this one is the left.
     @constant     ImageMask_3D_R                   The image has three dimensions, this one is the right.
     @constant     ImageMask_Luma                   The channel is for Luminance, also used for single channel/gray images.
     @constant     ImageMask_Chroma1                The channel is for Chrominance.
     @constant     ImageMask_Chroma2                The channel is for Chrominance.
     @constant     ImageMask_Red                    The channel is for Red.
     @constant     ImageMask_Green                  The channel is for Green.
     @constant     ImageMask_Blue                   The channel is for Blue.
     @constant     ImageMask_Cyan                   The channel is for Cyan.
     @constant     ImageMask_Magenta                The channel is for Magenta.
     @constant     ImageMask_Yellow                 The channel is for Yellow.
     @constant     ImageMask_Black                  The channel is for Black.
     @constant     ImageMask_White                  The channel is for White.
     @constant     ImageMask_Chroma3                The channel is for Chrominance.
     @constant     ImageMask_Alpha                  The channel is for Alpha aka transparency information.
     @constant     ImageMask_Green2                 The channel is for Green2.
     @constant     ImageMask_Emerald                The channel is for Emerald.
     @constant     ImageMask_Infrared               The channel is for Infrared, sometimes used for dust removal.
     @constant     ImageMask_Ultraviolet            The channel is for Ultraviolet.
     */
    typedef enum MediaIO_ImageMask : uint32_t {
                   ImageMask_Unspecified            = 0,
                   ImageMask_2D                     = 1,
                   ImageMask_3D_L                   = 2,
                   ImageMask_3D_R                   = 4,
                   ImageMask_Luma                   = 8,
                   ImageMask_Chroma1                = 16,
                   ImageMask_Chroma2                = 32,
                   ImageMask_Red                    = 64,
                   ImageMask_Green                  = 128,
                   ImageMask_Blue                   = 256,
                   ImageMask_Cyan                   = 512,
                   ImageMask_Magenta                = 1024,
                   ImageMask_Yellow                 = 2048,
                   ImageMask_Black                  = 4096,
                   ImageMask_White                  = 8192,
                   ImageMask_Chroma3                = 16384,
                   ImageMask_Alpha                  = 32768,
                   ImageMask_Green2                 = 65536,
                   ImageMask_Emerald                = 131072,
                   ImageMask_Infrared               = 262144,
                   ImageMask_Ultraviolet            = 524288,
    } MediaIO_ImageMask;
    
    /*!
     @enum         MediaIO_FlipTypes
     @abstract                                      Defines the type of flipping.
     @constant     FlipType_Unspecified             Invalid ImageType, exists solely to tell when it hasn't been set.
     @constant     FlipType_Vertical                Flip the imge vertically, up and down.
     @constant     FlipType_Horizontal              Flip the image horizontally, side to side.
     @constant     FlipType_VerticalAndHorizontal   Flip the image in both ways.
     */
    typedef enum MediaIO_FlipTypes : uint8_t {
                   FlipType_Unspecified             = 0,
                   FlipType_Vertical                = 1,
                   FlipType_Horizontal              = 2,
                   FlipType_VerticalAndHorizontal   = 3,
    } MediaIO_FlipTypes;
    
    /*!
     @enum         MediaIO_SortTypes
     @abstract                                      Defines the type of sorting.
     @constant     SortType_Unspecified             Invalid SortType, exists solely to tell when it hasn't been set.
     @constant     SortType_Ascending               Index 0 contains the most common value.
     @constant     SortType_Descending              Index 0 contains the least common value.
     */
    typedef enum MediaIO_SortTypes : uint8_t {
                   SortType_Unspecified             = 0,
                   SortType_Ascending               = 1,
                   SortType_Descending              = 2,
    } MediaIO_SortTypes;
    
    typedef struct          AudioScape2D        AudioScape2D;
    
    typedef struct          AudioChannelMap         AudioChannelMap;
    
    typedef struct          Audio2DHistogram        Audio2DHistogram;
    
    typedef struct          AudioScape3D        AudioScape3D;
    
    typedef struct          AudioVector             AudioVector;
    
    typedef struct          AudioVectorHistogram    AudioVectorHistogram;
    
    typedef struct          ImageCanvas             ImageCanvas;
    
    typedef struct          ImageChannelMap         ImageChannelMap;
    
    typedef struct          ImageHistogram          ImageHistogram;
    
    /*!
     @abstract                                      Gets the BitDepth of the Audio Samples.
     @param                 AudioType               The AudioType.
     */
    uint8_t                 AudioType_GetBitDepth(PlatformIOTypes AudioType);
    
    /*!
     @abstract                                      Gets the BitDepth of the Image Samples.
     @param                 ImageType               The ImageType.
     */
    uint8_t                 ImageType_GetBitDepth(PlatformIOTypes ImageType);
    
    /*!
     @abstract                                      Creates an empty AudioScape2D.
     @param                 Type                    A type from PlatformIOTypes.
     @param                 ChannelMap              Array of ChannelMasks, one array entry for each channel.
     @param                 SampleRate              The number of samples in one second of audio.
     @param                 NumSamples              NumSamples is the number of channel independent samples, e.g. X samples is BitDepth * NumChannels * X.
     */
    AudioScape2D       *AudioScape2D_Init(PlatformIOTypes Type, AudioChannelMap *ChannelMap, uint64_t SampleRate, uint64_t NumSamples);
    
    /*!
     @abstract                                      Returns the number of channel-agnostic samples in one second.
     @param                 Audio                   A pointer to the instance of an AudioScape2D in question.
     */
    uint64_t                AudioScape2D_GetSampleRate(AudioScape2D *Audio);
    
    /*!
     @abstract                                      Returns the number of bits required to represent a audio sample.
     @param                 Audio                   A pointer to the instance of an AudioScape2D in question.
     */
    uint8_t                 AudioScape2D_GetBitDepth(AudioScape2D *Audio);
    
    /*!
     @abstract                                      Returns the number of channel-agnostic audio samples stored in the container.
     @param                 Audio                   A pointer to the instance of an AudioScape2D in question.
     */
    uint64_t                AudioScape2D_GetNumSamples(AudioScape2D *Audio);
    
    /*!
     @abstract                                      Gets the type of the array contained by the AudioScape2D.
     @param                 Audio                   A pointer to the instance of an AudioScape2D in question.
     */
    PlatformIOTypes      AudioScape2D_GetType(AudioScape2D *Audio);
    
    /*!
     @abstract                                      Gets a pointer to the array of samples.
     @remark                                        You need to cast the pointer to the correct type you got from AudioScape2D_GetType.
     @param                 Audio                   A pointer to the instance of an AudioScape2D in question.
     */
    void                  **AudioScape2D_GetArray(AudioScape2D *Audio);
    
    /*!
     @abstract                                      Returns the average of the samples in the buffer.
     @param                 Audio                   A pointer to the instance of an AudioScape2D in question.
     @param                 Channel                 Which index should we get average?
     */
    int64_t                 AudioScape2D_GetAverage(AudioScape2D *Audio, uint64_t Channel);
    
    /*!
     @abstract                                      Returns the highest valued sample in the buffer.
     @param                 Audio                   A pointer to the instance of an AudioScape2D in question.
     @param                 Channel                 Which index should we get the highest value from?
     */
    int64_t                 AudioScape2D_GetMax(AudioScape2D *Audio, uint64_t Channel);
    
    /*!
     @abstract                                      Returns the lowest valued sample in the buffer.
     @param                 Audio                   A pointer to the instance of an AudioScape2D in question.
     @param                 Channel                 Which index should we get the lowest value from?
     */
    int64_t                 AudioScape2D_GetMin(AudioScape2D *Audio, uint64_t Channel);
    
    /*!
     @abstract                                      Securely erases an AudioScape2D.
     @param                 Audio                   A pointer to the instance of an AudioScape2D in question.
     @param                 NewValue                The value to set each codeunit to while erasing.
     @return                                        Returns the value of the first element of String, or 0xFE if it was unsucessful
     */
    uint8_t                 AudioScape2D_Erase(AudioScape2D *Audio, uint8_t NewValue);
    
    /*!
     @abstract                                      Deletes an AudioScape2D, and any samples stored within it.
     @param                 Audio                   A pointer to the instance of an AudioScape2D in question.
     */
    void                    AudioScape2D_Deinit(AudioScape2D *Audio);
    
    /*!
     @abstract                                      Creates a Audio2DHistogram.
     @param                 Audio                   A pointer to the instance of an AudioScape2D in question.
     @return                                        Returns the newly created Histogram.
     */
    Audio2DHistogram       *Audio2DHistogram_Init(AudioScape2D *Audio);
    
    /*!
     @abstract                                      Gets a pointer to the histogram data.
     @param                 Histogram               A pointer to the instance of an Audio2DHistogram in question.
     @return                                        Returns a pointer to the histogram data.
     */
    void                  **Audio2DHistogram_GetArray(Audio2DHistogram *Histogram);
    
    /*!
     @abstract                                      Sets a pointer to the histogram data.
     @param                 Histogram               A pointer to the instance of an Audio2DHistogram in question.
     @param                 Array                   A pointer to the histogram data.
     */
    void                    Audio2DHistogram_SetArray(Audio2DHistogram *Histogram, void **Array);
    
    /*!
     @abstract                                      Generates a histogram from an AudioScape2D.
     @param                 Audio                   A pointer to the instance of an AudioScape2D in question.
     @return                                        Returns the newly created Histogram.
     */
    Audio2DHistogram       *Audio2DHistogram_Generate(AudioScape2D *Audio);
    
    /*!
     @abstract                                      Sorts a histogram.
     @param                 Histogram               A pointer to the Audio2DHistogram in question.
     @param                 Sort                    The kind of sorting to do.
     */
    void                    Audio2DHistogram_Sort(Audio2DHistogram *Histogram, MediaIO_SortTypes Sort);
    
    /*!
     @abstract                                      Securely erases an AudioScape2D.
     @param                 Histogram               A pointer to the instance of an AudioScape2D in question.
     @param                 NewValue                The value to set each codeunit to while erasing.
     @return                                        Returns the value of the first element of String, or 0xFE if it was unsucessful
     */
    uint8_t                 Audio2DHistogram_Erase(Audio2DHistogram *Histogram, uint8_t NewValue);
    
    /*!
     @abstract                                      Frees a Audio2DHistogram.
     @param                 Histogram               A pointer to the Audio2DHistogram to deinitialize.
     */
    void                    Audio2DHistogram_Deinit(Audio2DHistogram *Histogram);
    
    /*!
     @abstract                                      Creates a AudioChannelMap.
     @param                 NumChannels             The number of channels.
     @return                                        Returns the newly created ChannelMap.
     */
    AudioChannelMap        *AudioChannelMap_Init(uint64_t NumChannels);
    
    /*!
     @abstract                                      Gets a pointer to the ChannelMap.
     @param                 Audio                   AudioScape2D Pointer.
     @return                                        Returns a pointer to the ChannelMap.
     */
    AudioChannelMap        *AudioScape2D_GetChannelMap(AudioScape2D *Audio);
    
    /*!
     @abstract                                      Returns the number of audio channels.
     @param                 Audio                   AudioScape2D Pointer.
     */
    uint64_t                AudioScape2D_GetNumChannels(AudioScape2D *Audio);
    
    /*!
     @abstract                                      Adds a MediaIO_AudioMask at the specified Index to the ChannelMap.
     @param                 ChannelMap              The number of channels.
     @param                 Index                   The index in the ChannelMap to add the mask.
     @param                 Mask                    The ChannelMask for the index.
     */
    void                    AudioChannelMap_AddMask(AudioChannelMap *ChannelMap, uint64_t Index, MediaIO_AudioMask Mask);
    
    /*!
     @abstract                                      Returns the ChannelMask for Index.
     @param                 ChannelMap              AudioScape2D Pointer.
     @param                 Index                   The channel index to get the mask for.
     */
    MediaIO_AudioMask       AudioChannelMap_GetMask(AudioChannelMap *ChannelMap, uint64_t Index);
    
    /*!
     @abstract                                       Finds the lowest index in the ChannelMap that is unused.
     @param                 ChannelMap               Pointer to the ChannelMap.
     */
    uint64_t                AudioChannelMap_GetLowestUnusedIndex(AudioChannelMap *ChannelMap);
    
    /*!
     @abstract                                      Deletes a ChannelMap.
     @param                 ChannelMap              The AudioChannelMap to delete.
     */
    void                    AudioChannelMap_Deinit(AudioChannelMap *ChannelMap);
    
    /*!
     @abstract                                      Creates an empty AudioScape3D.
     @param                 NumVectors             The number of vectors to create.
     @return                                        Returns a pointer to an AudioScape3D.
     */
    AudioScape3D       *AudioScape3D_Init(size_t NumVectors);
    
    /*!
     @abstract                                      Adds a AudioVector to an AudioScape3D.
     @param                 Container               The AudioScape3D to add the AudioVector to.
     @param                 Vector                  The AudioVector to add.
     @param                 Index                   The order of the AudioVector.
     */
    void                    AudioScape3D_SetVector(AudioScape3D *Container, AudioVector *Vector, uint64_t Index);
    
    /*!
     @abstract                                      Gets a pointer to an AudioVector.
     @param                 Container               The AudioScape3D to get the AudioVector from.
     @param                 Index                   The AudioVector to get.
     */
    AudioVector            *AudioScape3D_GetVector(AudioScape3D *Container, uint64_t Index);
    
    /*!
     @abstract                                      Gets the total number of samples in the AudioScape3D.
     @param                 Container               The AudioScape3D to get the total number of samples from.
     */
    uint64_t                AudioScape3D_GetTotalNumSamples(AudioScape3D *Container);
    
    /*!
     @abstract                                      Downmixes 3D audio to 2D audio.
     @param                 Audio3D                 The AudioScape3D to get the total number of samples from.
     @param                 ChannelMap              The ChannelMap to create the downmix with.
     @param                 Type                    The type of the audio, should this be gleaned from the 3D audio?
     */
    AudioScape2D       *AudioScape3D_Mix2Audio2DContainer(AudioScape3D *Audio3D, AudioChannelMap *ChannelMap, PlatformIOTypes Type, uint64_t SampleRate);
    
    
    /*!
     @abstract                                      Securely erases an AudioScape3D.
     @param                 Container               A pointer to the instance of the AudioScape3D in question.
     @param                 NewValue                The value to set each codeunit to while erasing.
     @return                                        Returns the value of the first element of String, or 0xFE if it was unsucessful.
     */
    uint8_t                 AudioScape3D_Erase(AudioScape3D *Container, uint8_t NewValue);
    
    /*!
     @abstract                                      Frees a AudioScape3D.
     @param                 Container               A pointer to the AudioScape3D to deinitialize.
     */
    void                    AudioScape3D_Deinit(AudioScape3D *Container);
    
    /*!
     @abstract                                      Creates an empty AudioVector.
     */
    AudioVector            *AudioVector_Init(void);
    
    /*!
     @abstract                                      Gets a Pointer to an AudioVector.
     @param                 Vector                  A pointer to the AudioScape3D to deinitialize.
     */
    void                   *AudioVector_GetArray(AudioVector *Vector);
    
    /*!
     @abstract                                      Sets a Pointer to the samples for an AudioVector.
     @param                 Vector                  A pointer to the AudioVector to set.
     @param                 Array                   A pointer to the sample data.
     */
    void                    AudioVector_SetArray(AudioVector *Vector, void *Array);
    
    /*!
     @abstract                                      Zeros an AudioVector.
     @param                 Vector                  The AudioVector to zero.
     @param                 NewValue                The value to set each codeunit to while erasing.
     @return                                        Returns the value of the first element of String, or 0xFE if it was unsucessful.
     */
    uint8_t                 AudioVector_Erase(AudioVector *Vector, uint8_t NewValue);
    
    /*!
     @abstract                                      Destroys an AudioVector.
     @param                 Vector                  The AudioVector to destroy.
     */
    void                    AudioVector_Deinit(AudioVector *Vector);
    
    /*!
     @abstract                                      Creates an empty AudioVectorHistogram.
     @param                 Vector                  The AudioVector to create the histogram from.
     */
    AudioVectorHistogram   *AudioVectorHistogram_Init(AudioVector *Vector);
    
    /*!
     @abstract                                      Gets a pointer to the histogram data.
     @param                 Histogram               A pointer to the instance of an AudioVectorHistogram in question.
     @return                                        Returns a pointer to the histogram data.
     */
    void                   *AudioVectorHistogram_GetArray(AudioVectorHistogram *Histogram);
    
    /*!
     @abstract                                      Sets a pointer to the histogram data.
     @param                 Histogram               A pointer to the instance of an AudioVectorHistogram in question.
     @param                 Array                   A pointer to the histogram data.
     */
    void                    AudioVectorHistogram_SetArray(AudioVectorHistogram *Histogram, void *Array);
    
    /*!
     @abstract                                      Generates a histogram from an AudioVector.
     @param                 Vector                  A pointer to the instance of an AudioVector in question.
     @return                                        Returns the newly created Histogram.
     */
    AudioVectorHistogram   *AudioVectorHistogram_Generate(AudioVector *Vector);
    
    /*!
     @abstract                                      Sorts a histogram.
     @param                 Histogram               A pointer to the AudioVectorHistogram in question.
     @param                 Sort                    The kind of sorting to do.
     */
    void                    AudioVectorHistogram_Sort(AudioVectorHistogram *Histogram, MediaIO_SortTypes Sort);
    
    /*!
     @abstract                                      Zeros an AudioVectorHistogram.
     @param                 Histogram               The AudioVector to zero.
     @param                 NewValue                The value to set each codeunit to while erasing.
     */
    void                    AudioVectorHistogram_Erase(AudioVectorHistogram *Histogram, uint8_t NewValue);
    
    /*!
     @abstract                                      Destroys an AudioVectorHistogram.
     @param                 Histogram               The AudioVectorHistogram to destroy.
     */
    void                    AudioVectorHistogram_Deinit(AudioVectorHistogram *Histogram);
    
    /*!
     @abstract                                      Creates an empty ImageChannelMap.
     @param                 NumViews                The number of views in the Image.
     @param                 NumChannels             The number of channels in the Image.
     */
    ImageChannelMap        *ImageChannelMap_Init(uint8_t NumViews, uint8_t NumChannels);
    
    /*!
     @abstract                                      Gets the number of views in the ChannelMap.
     @param                 ChannelMap              The ChannelMap to read.
     */
    uint8_t                 ImageChannelMap_GetNumViews(ImageChannelMap *ChannelMap);
    
    /*!
     @abstract                                      Gets the number of channels in the ChannelMap.
     @param                 ChannelMap              The ChannelMap to read.
     */
    uint8_t                 ImageChannelMap_GetNumChannels(ImageChannelMap *ChannelMap);
    
    /*!
     @abstract                                      Gets the index of the Mask.
     @remark                                        If the Mask isn't present, NumChannels is returned which is an invalid index.
     @param                 ChannelMap              The ChannelMap to read.
     @param                 Mask                    The color you want to find the index of.
     @return                                        Returns the Index.
     */
    uint8_t                 ImageChannelMap_GetChannelsIndex(ImageChannelMap *ChannelMap, MediaIO_ImageMask Mask);
    
    /*!
     @abstract                                      Sets a ChannelMap.
     @param                 ChannelMap              The ChannelMap to read.
     @param                 Index                   The index of the channel.
     @param                 Mask                    The ChannelMask for the Index
     */
    void                    ImageChannelMap_AddMask(ImageChannelMap *ChannelMap, uint8_t Index, MediaIO_ImageMask Mask);
    
    /*!
     @abstract                                      Destroys an AudioVectorHistogram.
     @param                 ChannelMap              The ChannelMap to destroy.
     */
    void                    ImageChannelMap_Deinit(ImageChannelMap *ChannelMap);
    
    /*!
     @abstract                                      Creates an empty ImageCanvas.
     @remark                                        All channels in an image must have the same bit depth, padding will be added if necessary.
     @param                 Type                    The type of array to create.
     @param                 ChannelMap              The channel map.
     @param                 Width                   The number of pixels making up one row.
     @param                 Height                  The number of pixels making up one column.
     */
    ImageCanvas         *ImageCanvas_Init(PlatformIOTypes Type, ImageChannelMap *ChannelMap, uint64_t Width, uint64_t Height);
    
    /*!
     @abstract                                      Returns the number of pixels in one row of this image.
     @param                 Image                   A pointer to the instance of an ImageCanvas in question.
     */
    uint64_t                ImageCanvas_GetWidth(ImageCanvas *Image);
    
    /*!
     @abstract                                      Returns the number of pixels in one column of this image.
     @param                 Image                   A pointer to the instance of an ImageCanvas in question.
     */
    uint64_t                ImageCanvas_GetHeight(ImageCanvas *Image);
    
    /*!
     @abstract                                      Gets the channel mask.
     @param                 Image                   A pointer to the instance of an ImageCanvas in question.
     */
    ImageChannelMap        *ImageCanvas_GetChannelMap(ImageCanvas *Image);
    
    /*!
     @abstract                                      Sets the channel mask.
     @param                 Image                   A pointer to the instance of an ImageCanvas in question.
     @param                 ChannelMap              The new channel map to set to the Image.
     */
    void                    ImageCanvas_SetChannelMap(ImageCanvas *Image, ImageChannelMap *ChannelMap);
    
    /*!
     @abstract                                      Gets the type of the array contained by the ImageCanvas.
     @param                 Image                   A pointer to the instance of an ImageCanvas in question.
     */
    PlatformIOTypes      ImageCanvas_GetType(ImageCanvas *Image);

    /*!
     @abstract                                      Convienence function that just calls ImageCanvas_GetType.
     @param                 Image                   A pointer to the instance of an ImageCanvas in question.
     @return                                        Returns the BitDepth of the Container as an integer.
     */
    uint8_t                 ImageCanvas_GetBitDepth(ImageCanvas *Image);

    /*!
     @abstract                                      Convienence function that just calls ImageChannelMap_GetNumViews.
     @param                 Image                   A pointer to the instance of an ImageCanvas in question.
     @return                                        Returns the NumViews of the Container as an integer.
     */
    uint8_t                 ImageCanvas_GetNumViews(ImageCanvas *Image);
    
    /*!
     @abstract                                      Gets a pointer to the array of pixels.
     @remark                                        You need to cast the pointer to the correct type you got from ImageType_GetType.
     @param                 Image                   A pointer to the instance of an ImageCanvas in question.
     */
    void                ****ImageCanvas_GetArray(ImageCanvas *Image);
    
    /*!
     @abstract                                      Sets a pointer to the array of pixels.
     @param                 Image                   A pointer to the instance of an ImageCanvas in question.
     @param                 Array                   A pointer to the new array.
     */
    void                    ImageCanvas_SetArray(ImageCanvas *Image, void ****Array);
    
    /*!
     @abstract                                      Returns the average value (rounded) of the pixels in this image.
     @param                 Image                   A pointer to the instance of an ImageCanvas in question.
     @param                 View                    Which view should we get the minimum from?
     @param                 Channel                 Which channel should we get the minimum from?
     */
    uint64_t                ImageCanvas_GetAverage(ImageCanvas *Image, uint8_t View, uint8_t Channel);
    
    /*!
     @abstract                                      Returns the highest value pixel in this image.
     @param                 Image                   A pointer to the instance of an ImageCanvas in question.
     @param                 View                    Which view should we get the minimum from?
     @param                 Channel                 Which channel should we get the minimum from?
     */
    uint64_t                ImageCanvas_GetMax(ImageCanvas *Image, uint8_t View, uint8_t Channel);
    
    /*!
     @abstract                                      Returns the lowest value pixel in this image.
     @param                 Image                   A pointer to the instance of an ImageCanvas in question.
     @param                 View                    Which view should we get the minimum from?
     @param                 Channel                 Which channel should we get the minimum from?
     */
    uint64_t                ImageCanvas_GetMin(ImageCanvas *Image, uint8_t View, uint8_t Channel);
    
    /*!
     @abstract                                      Rotates an Image, either Vertically, Horizontally, or both.
     @param                 Image                   A pointer to the instance of an ImageCanvas in question.
     @param                 FlipType                The type of flipping to use.
     */
    void                    ImageCanvas_Flip(ImageCanvas *Image, MediaIO_FlipTypes FlipType);
    
    /*!
     @abstract                                      Resizes an Image.
     @remark                                        0 means keep the same, -1 means remove one, 1 means add one.
     @param                 Image                   A pointer to the instance of an ImageCanvas in question.
     @param                 Top                     The offset from the top of the image.
     @param                 Bottom                  The offset from the bottom of the image.
     @param                 Left                    The offset from the left of the image.
     @param                 Right                   The offset from the right of the image.
     */
    void                    ImageCanvas_Resize(ImageCanvas *Image, int64_t Left, int64_t Right, int64_t Top, int64_t Bottom);
    
    /*!
     @abstract                                      Compares two Images.
     @param                 Reference               A pointer to the reference ImageCanvas.
     @param                 Compare                 A pointer to the ImageCanvas to compare to the Reference.
     @return                                        Returns an ImageCanvas containing (Reference - Compare) % 2^BitDepth.
     */
    ImageCanvas         *ImageCanvas_Compare(ImageCanvas *Reference, ImageCanvas *Compare);
    
    /*!
     @abstract                                      Securely erases an ImageCanvas.
     @param                 Image                   A pointer to the instance of an ImageCanvas in question.
     @param                 NewValue                The value to set each codeunit to while erasing.
     @return                                        Returns the value of the first element of String, or 0xFE if it was unsucessful.
     */
    uint8_t                 ImageCanvas_Erase(ImageCanvas *Image, uint8_t NewValue);
    
    /*!
     @abstract                                      Deletes the ImageCanvas pointed to.
     @param                 Image                   A pointer to the instance of an ImageCanvas in question.
     */
    void                    ImageCanvas_Deinit(ImageCanvas *Image);
    
    /*!
     @abstract                                      Creates a ImageHistogram.
     @param                 Image                   A pointer to the instance of an ImageCanvas in question.
     @return                                        Returns the newly created Histogram.
     */
    ImageHistogram         *ImageHistogram_Init(ImageCanvas *Image);
    
    /*!
     @abstract                                      Gets a pointer to the histogram data.
     @param                 Histogram               A pointer to the instance of an ImageHistogram in question.
     @return                                        Returns a pointer to the histogram data.
     */
    uint64_t             ***ImageHistogram_GetArray(ImageHistogram *Histogram);
    
    /*!
     @abstract                                      Sets a pointer to the histogram data.
     @param                 Histogram               A pointer to the instance of an ImageHistogram in question.
     @param                 Array                   A pointer to the histogram data.
     */
    void                    ImageHistogram_SetArray(ImageHistogram *Histogram, uint64_t ***Array);
    
    /*!
     @abstract                                      Generates a histogram from an ImageCanvas.
     @param                 Image                   A pointer to the instance of an ImageCanvas in question.
     @return                                        Returns the newly created Histogram.
     */
    ImageHistogram         *ImageHistogram_Generate(ImageCanvas *Image);
    
    /*!
     @abstract                                      Sorts a histogram.
     @param                 Histogram               A pointer to the ImageHistogram in question.
     @param                 Sort                    The kind of sorting to do.
     */
    void                    ImageHistogram_Sort(ImageHistogram *Histogram, MediaIO_SortTypes Sort);
    
    /*!
     @abstract                                      Securely erases an ImageHistogram.
     @param                 Histogram               A pointer to the instance of an ImageCanvas in question.
     @param                 NewValue                The value to set each codeunit to while erasing.
     @return                                        Returns the value of the first element of String, or 0xFE if it was unsucessful.
     */
    uint8_t                 ImageHistogram_Erase(ImageHistogram *Histogram, uint8_t NewValue);
    
    /*!
     @abstract                                      frees a ImageHistogram.
     @param                 Histogram               A pointer to the ImageHistogram to deinitialize.
     */
    void                    ImageHistogram_Deinit(ImageHistogram *Histogram);
    
    /*!
     @abstract                                      Gets a pointer to an ImageChannelMap from an ImageHistogram.
     @param                 Histogram               A pointer to the ImageHistogram to deinitialize.
     @return                                        Returns a pointer to the ImageChannelMap.
     */
    ImageChannelMap        *ImageHistogram_GetChannelMap(ImageHistogram *Histogram);
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_MediaIO_H */
