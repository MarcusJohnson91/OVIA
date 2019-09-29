#include "../include/ContainerIO.h"    /* Included for our declarations */

#include "../include/Log.h"            /* Included for error reporting */
#include "../include/Math.h"           /* Included for Absolute, Max/Min */
#include "../include/StringIO.h"       /* Included for UTF32 string handling */

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef struct Audio2DContainer {
        void             **Samples;
        Audio_ChannelMask *ChannelMap;
        uint64_t           NumChannels;
        uint64_t           NumSamples;
        uint64_t           SampleRate;
        uint8_t            BitDepth;
        Audio_Types        Type;
    } Audio2DContainer;
    
    Audio2DContainer *Audio2DContainer_Init(Audio_Types Type, uint64_t NumChannels, Audio_ChannelMask *ChannelMap, uint64_t SampleRate, uint64_t NumSamples) {
        Audio2DContainer *Audio     = NULL;
        if (NumSamples > 0) {
            Audio                   = calloc(1, sizeof(Audio2DContainer));
            if (Audio != NULL) {
                void **Array        = calloc(NumChannels * NumSamples, Type / 4);
                if (Array != NULL) {
                    Audio->Samples  = Array;
                } else {
                    Audio2DContainer_Deinit(Audio);
                    Log(Log_DEBUG, __func__, UTF8String("Couldn't allocate the audio array"));
                }
                
                Audio->SampleRate   = SampleRate;
                Audio->NumSamples   = NumSamples;
            } else {
                Audio2DContainer_Deinit(Audio);
                Log(Log_DEBUG, __func__, UTF8String("Couldn't allocate the Audio2DContainer"));
            }
        } else if (NumSamples == 0) {
            Log(Log_DEBUG, __func__, UTF8String("NumSamples %llu is invalid"), NumSamples);
        }
        return Audio;
    }
    
    uint8_t Audio2DContainer_GetBitDepth(Audio2DContainer *Audio) {
        uint8_t BitDepth = 0ULL;
        if (Audio != NULL) {
            BitDepth     = Audio->BitDepth;
        } else {
            Log(Log_DEBUG, __func__, UTF8String("Audio2DContainer Pointer is NULL"));
        }
        return BitDepth;
    }
    
    uint64_t Audio2DContainer_GetNumChannels(Audio2DContainer *Audio) {
        uint64_t NumChannels       = 0ULL;
        if (Audio != NULL) {
            NumChannels            = Audio->NumChannels;
        } else {
            Log(Log_DEBUG, __func__, UTF8String("Audio2DContainer Pointer is NULL"));
        }
        return NumChannels;
    }
    
    uint64_t Audio2DContainer_GetSampleRate(Audio2DContainer *Audio) {
        uint64_t SampleRate = 0ULL;
        if (Audio != NULL) {
            SampleRate      = Audio->SampleRate;
        } else {
            Log(Log_DEBUG, __func__, UTF8String("Audio2DContainer Pointer is NULL"));
        }
        return SampleRate;
    }
    
    uint64_t Audio2DContainer_GetNumSamples(Audio2DContainer *Audio) {
        uint64_t NumSamples = 0ULL;
        if (Audio != NULL) {
            NumSamples      = Audio->NumSamples;
        } else {
            Log(Log_DEBUG, __func__, UTF8String("Audio2DContainer Pointer is NULL"));
        }
        return NumSamples;
    }
    
    uint8_t Audio2DContainer_GetChannelsIndex(Audio2DContainer *Audio, Audio_ChannelMask ChannelMask) {
        uint8_t ChannelMap      = 0;
        if (Audio != NULL) {
            uint8_t NumChannels = Audio2DContainer_GetNumChannels(Audio);
            ChannelMap          = NumChannels;
            for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                if (Audio->ChannelMap[Channel] == ChannelMask) {
                    ChannelMap  = Channel;
                }
            }
        } else {
            Log(Log_DEBUG, __func__, UTF8String("Audio2DContainer Pointer is NULL"));
        }
        return ChannelMap;
    }
    
    Audio_Types Audio2DContainer_GetType(Audio2DContainer *Audio) {
        Audio_Types Type = AudioType_Unknown;
        if (Audio != NULL) {
            Type         = Audio->Type;
        } else {
            Log(Log_DEBUG, __func__, UTF8String("Audio2DContainer Pointer is NULL"));
        }
        return Type;
    }
    
    void **Audio2DContainer_GetArray(Audio2DContainer *Audio) {
        void **AudioArray = NULL;
        if (Audio != NULL) {
            AudioArray    = Audio->Samples;
        } else {
            Log(Log_DEBUG, __func__, UTF8String("Audio2DContainer Pointer is NULL"));
        }
        return AudioArray;
    }
    
    int64_t Audio2DContainer_GetAverage(Audio2DContainer *Audio, uint64_t Channel) {
        int64_t Average = 0LL;
        if (Audio != NULL) {
            if (Audio->Type == (AudioType_Unsigned | AudioType_Integer8)) {
                uint8_t **Array = (uint8_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    Average += Array[Channel][Sample];
                }
            } else if (Audio->Type == (AudioType_Signed | AudioType_Integer8)) {
                int8_t **Array = (int8_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    Average += Array[Channel][Sample];
                }
            } else if (Audio->Type == (AudioType_Unsigned | AudioType_Integer16)) {
                uint16_t **Array = (uint16_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    Average += Array[Channel][Sample];
                }
            } else if (Audio->Type == (AudioType_Signed | AudioType_Integer16)) {
                int16_t **Array = (int16_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    Average += Array[Channel][Sample];
                }
            } else if (Audio->Type == (AudioType_Unsigned | AudioType_Integer32)) {
                uint32_t **Array = (uint32_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    Average += Array[Channel][Sample];
                }
            } else if (Audio->Type == (AudioType_Signed | AudioType_Integer32)) {
                int32_t **Array = (int32_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    Average += Array[Channel][Sample];
                }
            }
            if (Average != 0) {
                Average /= Audio->NumSamples;
            }
        } else {
            Log(Log_DEBUG, __func__, UTF8String("Audio2DContainer Pointer is NULL"));
        }
        return Average;
    }
    
    int64_t Audio2DContainer_GetMax(Audio2DContainer *Audio, uint64_t Channel) {
        int64_t Maximum = INT64_MIN;
        if (Audio != NULL) {
            if (Audio->Type == (AudioType_Unsigned | AudioType_Integer8)) {
                uint8_t **Array = (uint8_t**) Audio2DContainer_GetArray(Audio);
                uint8_t   Max   = Exponentiate(2, Audio->BitDepth);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    if (Array[Channel][Sample] > Maximum) {
                        Maximum = Array[Channel][Sample];
                    }
                    if (Array[Channel][Sample] == Max) {
                        break;
                    }
                }
            } else if (Audio->Type == (AudioType_Signed | AudioType_Integer8)) {
                int8_t **Array = (int8_t**) Audio2DContainer_GetArray(Audio);
                int8_t   Max   = Exponentiate(2, Audio->BitDepth - 1);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    if (Array[Channel][Sample] > Maximum) {
                        Maximum = Array[Channel][Sample];
                    }
                    if (Array[Channel][Sample] == Max) {
                        break;
                    }
                }
            } else if (Audio->Type == (AudioType_Unsigned | AudioType_Integer16)) {
                uint16_t **Array = (uint16_t**) Audio2DContainer_GetArray(Audio);
                uint16_t   Max   = Exponentiate(2, Audio->BitDepth);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    if (Array[Channel][Sample] > Maximum) {
                        Maximum = Array[Channel][Sample];
                    }
                    if (Array[Channel][Sample] == Max) {
                        break;
                    }
                }
            } else if (Audio->Type == (AudioType_Signed | AudioType_Integer16)) {
                int16_t **Array = (int16_t**) Audio2DContainer_GetArray(Audio);
                int16_t   Max   = Exponentiate(2, Audio->BitDepth - 1);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    if (Array[Channel][Sample] > Maximum) {
                        Maximum = Array[Channel][Sample];
                    }
                    if (Array[Channel][Sample] == Max) {
                        break;
                    }
                }
            } else if (Audio->Type == (AudioType_Unsigned | AudioType_Integer32)) {
                uint32_t **Array = (uint32_t**) Audio2DContainer_GetArray(Audio);
                uint32_t   Max   = Exponentiate(2, Audio->BitDepth);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    if (Array[Channel][Sample] > Maximum) {
                        Maximum = Array[Channel][Sample];
                    }
                    if (Array[Channel][Sample] == Max) {
                        break;
                    }
                }
            } else if (Audio->Type == (AudioType_Signed | AudioType_Integer32)) {
                int32_t **Array = (int32_t**) Audio2DContainer_GetArray(Audio);
                int32_t   Max   = Exponentiate(2, Audio->BitDepth - 1);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    if (Array[Channel][Sample] > Maximum) {
                        Maximum = Array[Channel][Sample];
                    }
                    if (Array[Channel][Sample] == Max) {
                        break;
                    }
                }
            }
        } else {
            Log(Log_DEBUG, __func__, UTF8String("Audio2DContainer Pointer is NULL"));
        }
        return Maximum;
    }
    
    int64_t Audio2DContainer_GetMin(Audio2DContainer *Audio, uint64_t Channel) {
        int64_t Minimum = INT64_MAX;
        if (Audio != NULL) {
            if (Audio->Type == (AudioType_Unsigned | AudioType_Integer8)) {
                uint8_t **Array = (uint8_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    if (Array[Channel][Sample] < Minimum) {
                        Minimum = Array[Channel][Sample];
                    }
                    if (Array[Channel][Sample] == 0) {
                        break;
                    }
                }
            } else if (Audio->Type == (AudioType_Signed | AudioType_Integer8)) {
                int8_t **Array = (int8_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    if (Array[Channel][Sample] < Minimum) {
                        Minimum = Array[Channel][Sample];
                    }
                    if (Array[Channel][Sample] == -128) {
                        break;
                    }
                }
            } else if (Audio->Type == (AudioType_Unsigned | AudioType_Integer16)) {
                uint16_t **Array = (uint16_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    if (Array[Channel][Sample] < Minimum) {
                        Minimum = Array[Channel][Sample];
                    }
                    if (Array[Channel][Sample] == 0) {
                        break;
                    }
                }
            } else if (Audio->Type == (AudioType_Signed | AudioType_Integer16)) {
                int16_t **Array = (int16_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    if (Array[Channel][Sample] < Minimum) {
                        Minimum = Array[Channel][Sample];
                    }
                    if (Array[Channel][Sample] == -32768) {
                        break;
                    }
                }
            } else if (Audio->Type == (AudioType_Unsigned | AudioType_Integer32)) {
                uint32_t **Array = (uint32_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    if (Array[Channel][Sample] < Minimum) {
                        Minimum = Array[Channel][Sample];
                    }
                    if (Array[Channel][Sample] == 0) {
                        break;
                    }
                }
            } else if (Audio->Type == (AudioType_Signed | AudioType_Integer32)) {
                int32_t **Array = (int32_t**) Audio2DContainer_GetArray(Audio);
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    if (Array[Channel][Sample] < Minimum) {
                        Minimum = Array[Channel][Sample];
                    }
                    if (Array[Channel][Sample] == -2147483648) {
                        break;
                    }
                }
            }
        } else {
            Log(Log_DEBUG, __func__, UTF8String("Audio2DContainer Pointer is NULL"));
        }
        return Minimum;
    }
    
    void Audio2DContainer_Erase(Audio2DContainer *Audio) {
        if (Audio != NULL) {
            if (Audio->Type == (AudioType_Unsigned | AudioType_Integer8)) {
                uint8_t **Samples = (uint8_t**) Audio2DContainer_GetArray(Audio);
                
                for (uint64_t Channel = 0ULL; Channel < Audio2DContainer_GetNumChannels(Audio); Channel++) {
                    for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                        Samples[Channel][Sample] = 0;
                    }
                }
            } else if (Audio->Type == (AudioType_Signed | AudioType_Integer8)) {
                int8_t **Samples  = (int8_t**) Audio2DContainer_GetArray(Audio);
                
                for (uint64_t Channel = 0ULL; Channel < Audio2DContainer_GetNumChannels(Audio); Channel++) {
                    for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                        Samples[Channel][Sample] = 0;
                    }
                }
            } else if (Audio->Type == (AudioType_Unsigned | AudioType_Integer16)) {
                uint16_t **Samples  = (uint16_t**) Audio2DContainer_GetArray(Audio);
                
                for (uint64_t Channel = 0ULL; Channel < Audio2DContainer_GetNumChannels(Audio); Channel++) {
                    for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                        Samples[Channel][Sample] = 0;
                    }
                }
            } else if (Audio->Type == (AudioType_Signed | AudioType_Integer16)) {
                int16_t **Samples  = (int16_t**) Audio2DContainer_GetArray(Audio);
                
                for (uint64_t Channel = 0ULL; Channel < Audio2DContainer_GetNumChannels(Audio); Channel++) {
                    for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                        Samples[Channel][Sample] = 0;
                    }
                }
            } else if (Audio->Type == (AudioType_Unsigned | AudioType_Integer32)) {
                uint32_t **Samples  = (uint32_t**) Audio2DContainer_GetArray(Audio);
                
                for (uint64_t Channel = 0ULL; Channel < Audio2DContainer_GetNumChannels(Audio); Channel++) {
                    for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                        Samples[Channel][Sample] = 0;
                    }
                }
            } else if (Audio->Type == (AudioType_Signed | AudioType_Integer32)) {
                int32_t **Samples  = (int32_t**) Audio2DContainer_GetArray(Audio);
                
                for (uint64_t Channel = 0ULL; Channel < Audio2DContainer_GetNumChannels(Audio); Channel++) {
                    for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                        Samples[Channel][Sample] = 0;
                    }
                }
            }
        } else {
            Log(Log_DEBUG, __func__, UTF8String("Audio2DContainer Pointer is NULL"));
        }
    }
    
    void Audio2DContainer_Deinit(Audio2DContainer *Audio) {
        if (Audio != NULL) {
            free(Audio->Samples);
            free(Audio->ChannelMap);
            free(Audio);
        } else {
            Log(Log_DEBUG, __func__, UTF8String("Audio2DContainer Pointer is NULL"));
        }
    }
    
    typedef struct AudioVector {
        void       *Samples;
        uint64_t   *Directions;
        uint64_t    SampleRate;
        uint64_t    DirectionOffset;
        uint64_t    NumSamples;
        uint64_t    NumDirections;
        Audio_Types Type;
    } AudioVector;
    
    typedef struct Audio3DContainer {
        AudioVector *Vectors;
        uint64_t     NumVectors;
    } Audio3DContainer;
    
    Audio3DContainer *Audio3DContainer_Init(uint64_t NumVectors) {
        Audio3DContainer *Container       = NULL;
        if (NumVectors > 0) {
            Container                     = calloc(1, sizeof(Audio3DContainer));
            if (Container != NULL) {
                Container->Vectors        = calloc(NumVectors, sizeof(AudioVector));
                if (Container->Vectors != NULL) {
                    Container->NumVectors = NumVectors;
                } else {
                    Log(Log_DEBUG, __func__, UTF8String("Couldn't allocate %lld AudioObjects"), NumVectors);
                }
            } else {
                Log(Log_DEBUG, __func__, UTF8String("Couldn't allocate Audio3DContainer"));
            }
        } else {
            Log(Log_DEBUG, __func__, UTF8String("NumVectors %lld is invalid"), NumVectors);
        }
        return Container;
    }
    
    void Audio3DContainer_SetVector(Audio3DContainer *Container, AudioVector *Vector, uint64_t Index) {
        if (Container != NULL && Vector != NULL && Index < Container->NumVectors) {
            Container->Vectors[Index - 1] = *Vector;
        } else if (Container == NULL) {
            Log(Log_DEBUG, __func__, UTF8String("Container Pointer is NULL"));
        } else if (Vector == NULL) {
            Log(Log_DEBUG, __func__, UTF8String("Vector Pointer is NULL"));
        } else if (Index >= Container->NumVectors) {
            Log(Log_DEBUG, __func__, UTF8String("Index %llu is larger than %llu"), Index, Container->NumVectors);
        }
    }
    
    void Audio3DContainer_Erase(Audio3DContainer *Container) {
        if (Container != NULL) {
            for (uint64_t Vector = 0ULL; Vector < Container->NumVectors; Vector++) {
                for (uint64_t Direction = 0ULL; Direction < Container->Vectors[Vector].NumDirections; Direction++) {
                    Container->Vectors[Vector].Directions[Direction] = 0;
                }
                Container->Vectors[Vector].Samples                   = 0;
            }
        } else {
            Log(Log_DEBUG, __func__, UTF8String("Container Pointer is NULL"));
        }
    }
    
    void Audio3DContainer_Deinit(Audio3DContainer *Container) {
        if (Container != NULL) {
            for (uint64_t Vector = 0ULL; Vector < Container->NumVectors; Vector++) {
                free(Container->Vectors[Vector].Directions);
                free(Container->Vectors[Vector].Samples);
            }
            free(Container->Vectors);
            free(Container);
        }
    }
    
    typedef struct ImageContainer {
        void              ****Pixels;
        Image_ChannelMask    *ChannelMap;
        uint64_t              Width;
        uint64_t              Height;
        Image_ChannelMask     ChannelMask;
        uint8_t               BitDepth;
        Image_Types           Type;
    } ImageContainer;
    
    ImageContainer *ImageContainer_Init(Image_Types Type, Image_ChannelMask ChannelMask, uint64_t Width, uint64_t Height) {
        ImageContainer *Image = NULL;
        if (Width > 0 && Height > 0) {
            Image                             = calloc(1, sizeof(ImageContainer));
            if (Image != NULL) {
                uint8_t           NumViews    = ImageMask_GetNumViews(ChannelMask);
                uint8_t           NumChannels = ImageMask_GetNumChannels(ChannelMask);
                void *Array                   = calloc(NumViews * NumChannels * Width * Height, Type);
                if (Array != NULL) {
                    Image->Pixels             = Array;
                    Image->ChannelMap         = calloc(NumViews * NumChannels, sizeof(Image_ChannelMask));
                    if (Image->ChannelMap != NULL) {
                        Image->Type           = Type;
                        Image->Width          = Width;
                        Image->Height         = Height;
                    } else {
                        ImageContainer_Deinit(Image);
                        Log(Log_DEBUG, __func__, UTF8String("Couldn't allocate channel map"));
                    }
                } else {
                    ImageContainer_Deinit(Image);
                    Log(Log_DEBUG, __func__, UTF8String("Couldn't allocate pixel array"));
                }
            } else {
                ImageContainer_Deinit(Image);
                Log(Log_DEBUG, __func__, UTF8String("Couldn't allocate the ImageContainer"));
            }
        } else if (Width == 0) {
            Log(Log_DEBUG, __func__, UTF8String("Width %llu is invalid"), Width);
        } else if (Height == 0) {
            Log(Log_DEBUG, __func__, UTF8String("Height %llu is invalid"), Height);
        }
        return Image;
    }
    
    void ImageContainer_SetChannelMap(ImageContainer *Image, uint64_t Index, Image_ChannelMask ChannelMask) {
        if (Image != NULL) {
            uint8_t NumViews             = ImageMask_GetNumViews(ChannelMask);
            uint8_t NumChannels          = ImageMask_GetNumChannels(ChannelMask);
            if (NumViews == 1 && NumChannels == 1) {
                Image->ChannelMap[Index] = ChannelMask;
            } else {
                Log(Log_DEBUG, __func__, UTF8String("ChannelMask must contain exactly 1 view and 1 channel"));
            }
        } else if (Image == NULL) {
            Log(Log_DEBUG, __func__, UTF8String("ImageContainer Pointer is NULL"));
        }
    }
    
    uint64_t ImageContainer_GetWidth(ImageContainer *Image) {
        uint64_t Width = 0ULL;
        if (Image != NULL) {
            Width      = Image->Width;
        } else {
            Log(Log_DEBUG, __func__, UTF8String("ImageContainer Pointer is NULL"));
        }
        return Width;
    }
    
    uint64_t ImageContainer_GetHeight(ImageContainer *Image) {
        uint64_t Height = 0ULL;
        if (Image != NULL) {
            Height      = Image->Height;
        } else {
            Log(Log_DEBUG, __func__, UTF8String("ImageContainer Pointer is NULL"));
        }
        return Height;
    }
    
    uint8_t ImageContainer_GetBitDepth(ImageContainer *Image) {
        uint8_t BitDepth = 0ULL;
        if (Image != NULL) {
            BitDepth     = Image->BitDepth;
        } else {
            Log(Log_DEBUG, __func__, UTF8String("ImageContainer Pointer is NULL"));
        }
        return BitDepth;
    }
    
    uint8_t ImageMask_GetNumViews(Image_ChannelMask ChannelMask) {
        uint8_t NumViews            = 0;
        Image_ChannelMask JustViews = ChannelMask & (ImageMask_2D | ImageMask_3D_L | ImageMask_3D_R);
        do {
            if ((JustViews & 1) == 1) {
                NumViews           += 1;
            }
            JustViews             >>= 1;
        } while (JustViews > 0);
        return NumViews;
    }
    
    uint64_t ImageMask_GetNumChannels(Image_ChannelMask ChannelMask) {
        uint64_t NumChannels           = 0ULL;
        Image_ChannelMask JustChannels = ChannelMask - (ImageMask_2D | ImageMask_3D_L | ImageMask_3D_R);
        do {
            if ((JustChannels & 1) == 1) {
                NumChannels           += 1;
            }
            JustChannels             >>= 1;
        } while (JustChannels > 0);
        return NumChannels;
    }
    
    uint64_t ImageContainer_GetNumChannels(ImageContainer *Image) {
        uint64_t NumChannels       = 0ULL;
        if (Image != NULL) {
            Image_ChannelMask Mask = Image->ChannelMask;
            NumChannels            = ImageMask_GetNumChannels(Mask);
        } else {
            Log(Log_DEBUG, __func__, UTF8String("ImageContainer Pointer is NULL"));
        }
        return NumChannels;
    }
    
    uint64_t ImageContainer_GetChannelsIndex(ImageContainer *Image, Image_ChannelMask Mask) {
        uint64_t Index                = 0ULL;
        if (Image != NULL) {
            Image_ChannelMask Channel = Mask - (ImageMask_2D | ImageMask_3D_L | ImageMask_3D_R);
            uint8_t NumViews          = ImageMask_GetNumViews(Mask);
            uint8_t NumChannels       = ImageMask_GetNumChannels(Mask);
            Index                     = NumChannels;
            if (NumViews == 1 && NumChannels == 1) {
                for (uint64_t Channels = 0ULL; Channels < NumChannels; Channels++) {
                    if (Image->ChannelMap[Channels] == Channel) {
                        Index         = Channels;
                    }
                }
            } else if (NumViews != 1) {
                Log(Log_DEBUG, __func__, UTF8String("Mask must contain 1 and only 1 view"));
            } else if (NumChannels != 1) {
                Log(Log_DEBUG, __func__, UTF8String("Mask must contain 1 and only 1 channel"));
            }
        } else {
            Log(Log_DEBUG, __func__, UTF8String("ImageContainer Pointer is NULL"));
        }
        return Index;
    }
    
    Image_ChannelMask ImageContainer_GetChannelMask(ImageContainer *Image) {
        Image_ChannelMask Mask    = 0;
        if (Image != NULL) {
            Mask                  = Image->ChannelMask;
        } else {
            Log(Log_DEBUG, __func__, UTF8String("ImageContainer Pointer is NULL"));
        }
        return Mask;
    }
    
    uint8_t ImageContainer_GetNumViews(ImageContainer *Image) {
        uint8_t NumViews           = 0ULL;
        if (Image != NULL) {
            Image_ChannelMask Mask = Image->ChannelMask;
            NumViews               = ImageMask_GetNumViews(Mask);
        } else {
            Log(Log_DEBUG, __func__, UTF8String("ImageContainer Pointer is NULL"));
        }
        return NumViews;
    }
    
    uint64_t ImageContainer_GetViewsIndex(ImageContainer *Image, Image_ChannelMask Mask) {
        uint64_t Index          = 0ULL;
        if (Image != NULL) {
            uint8_t NumViews    = ImageContainer_GetNumViews(Image);
            uint8_t NumChannels = ImageContainer_GetNumChannels(Image);
            Index               = NumChannels;
            for (uint64_t ViewIndex = 0ULL; ViewIndex < NumViews; ViewIndex++) {
                if (Image->ChannelMap[ViewIndex] == (Mask & 0x7)) {
                    Index = ViewIndex;
                }
            }
        } else {
            Log(Log_DEBUG, __func__, UTF8String("ImageContainer Pointer is NULL"));
        }
        return Index;
    }
    
    Image_Types ImageContainer_GetType(ImageContainer *Image) {
        Image_Types Type = ImageType_Unknown;
        if (Image != NULL) {
            Type         = Image->Type;
        } else {
            Log(Log_DEBUG, __func__, UTF8String("ImageContainer Pointer is NULL"));
        }
        return Type;
    }
    
    void ****ImageContainer_GetArray(ImageContainer *Image) {
        void ****ImageArray = NULL;
        if (Image != NULL) {
            ImageArray   = Image->Pixels;
        } else {
            Log(Log_DEBUG, __func__, UTF8String("ImageContainer Pointer is NULL"));
        }
        return ImageArray;
    }
    
    void ImageContainer_SetArray(ImageContainer *Image, void ****Array) {
        if (Image != NULL) {
            Image->Pixels = Array;
        } else {
            Log(Log_DEBUG, __func__, UTF8String("ImageContainer Pointer is NULL"));
        }
    }
    
    uint64_t ImageContainer_GetAverage(ImageContainer *Image, Image_ChannelMask ChannelMask) {
        uint64_t Average         = 0ULL;
        if (Image != NULL) {
            uint64_t View        = ImageContainer_GetViewsIndex(Image, ChannelMask);
            uint64_t Channel     = ImageContainer_GetChannelsIndex(Image, ChannelMask);
            uint64_t NumViews    = ImageMask_GetNumViews(ChannelMask);
            uint64_t NumChannels = ImageMask_GetNumChannels(ChannelMask);
            if (View < NumViews - 1) {
                if (Channel < NumChannels - 1) {
                    if (Image->Type == ImageType_Integer8) {
                        uint8_t ****Array = (uint8_t****) ImageContainer_GetArray(Image);
                        for (uint64_t Width = 0ULL; Width < Image->Width; Width++) {
                            for (uint64_t Height = 0ULL; Height < Image->Height; Height++) {
                                Average += Array[View][Width][Height][Channel];
                            }
                        }
                    } else if (Image->Type == ImageType_Integer16) {
                        uint16_t ****Array = (uint16_t****) ImageContainer_GetArray(Image);
                        for (uint64_t Width = 0ULL; Width < Image->Width; Width++) {
                            for (uint64_t Height = 0ULL; Height < Image->Height; Height++) {
                                Average += Array[View][Width][Height][Channel];
                            }
                        }
                    }
                    if (Average != 0) {
                        Average /= Image->Width * Image->Height;
                    }
                } else {
                    Log(Log_DEBUG, __func__, UTF8String("You tried getting the average from a nonexistant channel %lld"), Channel);
                }
            } else {
                Log(Log_DEBUG, __func__, UTF8String("You tried getting the average from a nonexistant view %lld"), View);
            }
        } else {
            Log(Log_DEBUG, __func__, UTF8String("ImageContainer Pointer is NULL"));
        }
        return Average;
    }
    
    uint64_t ImageContainer_GetMax(ImageContainer *Image, Image_ChannelMask ChannelMask) {
        uint64_t Maximum         = 0ULL;
        if (Image != NULL) {
            uint64_t Channel     = ImageContainer_GetChannelsIndex(Image, ChannelMask);
            uint64_t View        = ImageContainer_GetViewsIndex(Image, ChannelMask);
            uint64_t NumViews    = ImageMask_GetNumViews(ChannelMask);
            uint64_t NumChannels = ImageMask_GetNumChannels(ChannelMask);
            if (View < NumViews - 1) {
                if (Channel < NumChannels - 1) {
                    if (Image->Type == ImageType_Integer8) {
                        uint8_t ****Array = (uint8_t****) ImageContainer_GetArray(Image);
                        uint8_t      Max  = Exponentiate(2, Image->BitDepth);
                        for (uint64_t Width = 0ULL; Width < Image->Width; Width++) {
                            for (uint64_t Height = 0ULL; Height < Image->Height; Height++) {
                                if (Array[View][Width][Height][Channel] > Maximum) {
                                    Maximum = Array[View][Width][Height][Channel];
                                    if (Array[View][Width][Height][Channel] == Max) {
                                        break;
                                    }
                                }
                            }
                        }
                    } else if (Image->Type == ImageType_Integer16) {
                        uint16_t ****Array = (uint16_t****)  ImageContainer_GetArray(Image);
                        uint16_t     Max   = Exponentiate(2, Image->BitDepth);
                        for (uint64_t Width = 0ULL; Width < Image->Width; Width++) {
                            for (uint64_t Height = 0ULL; Height < Image->Height; Height++) {
                                if (Array[View][Width][Height][Channel] > Maximum) {
                                    Maximum = Array[View][Width][Height][Channel];
                                    if (Array[View][Width][Height][Channel] == Max) {
                                        break;
                                    }
                                }
                            }
                        }
                    }
                } else {
                    Log(Log_DEBUG, __func__, UTF8String("You tried getting the max from a nonexistant channel"));
                }
            } else {
                Log(Log_DEBUG, __func__, UTF8String("You tried getting the average from a nonexistant view %lld"), View);
            }
        } else {
            Log(Log_DEBUG, __func__, UTF8String("ImageContainer Pointer is NULL"));
        }
        return Maximum;
    }
    
    uint64_t ImageContainer_GetMin(ImageContainer *Image, Image_ChannelMask ChannelMask) {
        uint64_t Minimum         = UINT64_MAX;
        if (Image != NULL) {
            uint64_t Channel     = ImageContainer_GetChannelsIndex(Image, ChannelMask);
            uint64_t View        = ImageContainer_GetViewsIndex(Image, ChannelMask);
            uint64_t NumViews    = ImageMask_GetNumViews(ChannelMask);
            uint64_t NumChannels = ImageMask_GetNumChannels(ChannelMask);
            
            if (View < NumViews - 1) {
                if (Channel < NumChannels - 1) {
                    if (Image->Type == ImageType_Integer8) {
                        uint8_t ****Array = (uint8_t****) ImageContainer_GetArray(Image);
                        for (uint64_t Width = 0ULL; Width < Image->Width; Width++) {
                            for (uint64_t Height = 0ULL; Height < Image->Height; Height++) {
                                if (Array[View][Width][Height][Channel] < Minimum) {
                                    Minimum = Array[View][Width][Height][Channel];
                                    if (Array[View][Width][Height][Channel] == 0) {
                                        break;
                                    }
                                }
                            }
                        }
                    } else if (Image->Type == ImageType_Integer16) {
                        uint16_t ****Array = (uint16_t****)  ImageContainer_GetArray(Image);
                        for (uint64_t Width = 0ULL; Width < Image->Width; Width++) {
                            for (uint64_t Height = 0ULL; Height < Image->Height; Height++) {
                                if (Array[View][Width][Height][Channel] < Minimum) {
                                    Minimum = Array[View][Width][Height][Channel];
                                    if (Array[View][Width][Height][Channel] == 0) {
                                        break;
                                    }
                                }
                            }
                        }
                    }
                } else {
                    Log(Log_DEBUG, __func__, UTF8String("You tried getting the max from a nonexistant channel"));
                }
            } else {
                Log(Log_DEBUG, __func__, UTF8String("You tried getting the average from a nonexistant view %lld"), View);
            }
        } else {
            Log(Log_DEBUG, __func__, UTF8String("ImageContainer Pointer is NULL"));
        }
        return Minimum;
    }
    
    void ImageContainer_Flip(ImageContainer *Image, FlipTypes FlipType) {
        if (Image != NULL) {
            uint8_t NumChannels = ImageContainer_GetNumChannels(Image);
            uint8_t NumViews    = ImageContainer_GetNumViews(Image);
            if (FlipType == FlipType_Vertical || FlipType == FlipType_VerticalAndHorizontal) {
                if (Image->Type == ImageType_Integer8) {
                    uint8_t  *Array = (uint8_t*)  ImageContainer_GetArray(Image);
                    for (uint64_t View = 0ULL; View < NumViews; View++) {
                        for (uint64_t Width = 0ULL; Width < Image->Width; Width++) {
                            for (uint64_t TopLine = 0ULL; TopLine < Image->Height; TopLine++) {
                                for (uint64_t BottomLine = Image->Height; BottomLine > 0ULL; BottomLine--) {
                                    for (uint64_t Channel = 0ULL; Channel < NumChannels; Channel++) {
                                        uint8_t TopPixel                        = Array[View * Width * TopLine * Channel];
                                        uint8_t BottomPixel                     = Array[View * Width * BottomLine * Channel];
                                        
                                        Array[View * Width * TopLine * Channel]    = BottomPixel;
                                        Array[View * Width * BottomLine * Channel] = TopPixel;
                                    }
                                }
                            }
                        }
                    }
                } else if (Image->Type == ImageType_Integer16) {
                    uint16_t *Array = (uint16_t*) ImageContainer_GetArray(Image);
                    for (uint64_t View = 0ULL; View < NumViews; View++) {
                        for (uint64_t Width = 0ULL; Width < Image->Width; Width++) {
                            for (uint64_t TopLine = 0ULL; TopLine < Image->Height; TopLine++) {
                                for (uint64_t BottomLine = Image->Height; BottomLine > 0ULL; BottomLine--) {
                                    for (uint64_t Channel = 0ULL; Channel < NumChannels; Channel++) {
                                        uint16_t TopPixel                          = Array[View * Width * TopLine * Channel];
                                        uint16_t BottomPixel                       = Array[View * Width * BottomLine * Channel];
                                        
                                        Array[View * Width * TopLine * Channel]    = BottomPixel;
                                        Array[View * Width * BottomLine * Channel] = TopPixel;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (FlipType == FlipType_Horizontal || FlipType == FlipType_VerticalAndHorizontal) {
                if (Image->Type == ImageType_Integer8) {
                    uint8_t  *Array = (uint8_t*)  ImageContainer_GetArray(Image);
                    for (uint64_t View = 1ULL; View <= NumViews; View++) {
                        for (uint64_t Left = 0ULL; Left < Image->Width; Left++) {
                            for (uint64_t Right = Image->Width; Right > 0ULL; Right++) {
                                for (uint64_t Height = 0ULL; Height < Image->Height; Height++) {
                                    for (uint64_t Channel = 0ULL; Channel < NumChannels; Channel++) {
                                        uint8_t LeftPixel                   = Array[View * Left * Height * Channel];
                                        uint8_t RightPixel                  = Array[View * Right * Height * Channel];
                                        
                                        Array[View * Left * Height * Channel]  = RightPixel;
                                        Array[View * Right * Height * Channel] = LeftPixel;
                                    }
                                }
                            }
                        }
                    }
                } else if (Image->Type == ImageType_Integer16) {
                    uint16_t *Array = (uint16_t*) ImageContainer_GetArray(Image);
                    for (uint64_t View = 0ULL; View < NumViews; View++) {
                        for (uint64_t Left = 0ULL; Left < Image->Width; Left++) {
                            for (uint64_t Right = Image->Width; Right > 0ULL; Right++) {
                                for (uint64_t Height = 0ULL; Height < Image->Height; Height++) {
                                    for (uint64_t Channel = 0ULL; Channel < NumChannels; Channel++) {
                                        uint16_t LeftPixel                   = Array[View * Left * Height * Channel];
                                        uint16_t RightPixel                  = Array[View * Right * Height * Channel];
                                        
                                        Array[View * Left * Height * Channel]  = RightPixel;
                                        Array[View * Right * Height * Channel] = LeftPixel;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } else {
            Log(Log_DEBUG, __func__, UTF8String("ImageContainer Pointer is NULL"));
        }
    }
    
    void ImageContainer_Resize(ImageContainer *Image, int64_t Left, int64_t Right, int64_t Top, int64_t Bottom) {
        if (Image != NULL) {
            uint8_t  NumChannels = ImageContainer_GetNumChannels(Image);
            uint8_t  NumViews    = ImageContainer_GetNumViews(Image);
            uint64_t Height      = ImageContainer_GetHeight(Image);
            uint64_t Width       = ImageContainer_GetWidth(Image);
            Image_Types Type     = ImageContainer_GetType(Image);
            
            int64_t NewWidth     = 0;
            int64_t NewHeight    = 0;
            
            int64_t LeftOffset   = AbsoluteI(Left);
            int64_t RightOffset  = Width + Right;
            int64_t TopOffset    = AbsoluteI(Top);
            int64_t BottomOffset = Height + Bottom;
            
            void *New            = calloc(NumViews * NewWidth * NewHeight * NumChannels, Type);
            if (New != NULL) {
                if (Type == ImageType_Integer8) {
                    uint8_t *Array    = (uint8_t*) ImageContainer_GetArray(Image);
                    uint8_t *NewArray = (uint8_t*) New;
                    if (Array != NULL) {
                        for (uint8_t View = 0; View < NumViews; View++) {
                            for (int64_t W = LeftOffset; W < RightOffset; W++) {
                                for (int64_t H = TopOffset; H < BottomOffset; H++) {
                                    for (uint64_t Channel = 0ULL; Channel < NumChannels; Channel++) {
                                        NewArray[View * (W - LeftOffset) * (H - TopOffset) * Channel] = Array[View * W * H * Channel];
                                    }
                                }
                            }
                        }
                        ImageContainer_SetArray(Image, (void*) NewArray);
                        free(Array);
                    } else {
                        Log(Log_DEBUG, __func__, UTF8String("Couldn't allocate an array for the cropped image"));
                    }
                } else if (Type == ImageType_Integer16) {
                    uint16_t *Array    = (uint16_t*) ImageContainer_GetArray(Image);
                    uint16_t *NewArray = (uint16_t*) New;
                    if (Array != NULL) {
                        for (uint8_t View = 1; View <= NumViews; View++) {
                            for (int64_t W = LeftOffset; W < RightOffset; W++) {
                                for (int64_t H = TopOffset; H < BottomOffset; H++) {
                                    for (uint64_t Channel = 1ULL; Channel <= NumChannels; Channel++) {
                                        NewArray[View * (W - LeftOffset) * (H - TopOffset) * Channel] = Array[View * W * H * Channel];
                                    }
                                }
                            }
                        }
                        ImageContainer_SetArray(Image, (void*) NewArray);
                        free(Array);
                    } else {
                        Log(Log_DEBUG, __func__, UTF8String("Couldn't allocate an array for the cropped image"));
                    }
                }
            } else {
                Log(Log_DEBUG, __func__, UTF8String("Couldn't allocate resized array"));
            }
        } else {
            Log(Log_DEBUG, __func__, UTF8String("ImageContainer Pointer is NULL"));
        }
    }
    
    ImageContainer *ImageContainer_Compare(ImageContainer *Reference, ImageContainer *Compare) {
        ImageContainer *Difference     = NULL;
        if (Reference != NULL && Compare != NULL) {
            Image_Types RefType        = ImageContainer_GetType(Reference);
            Image_Types ComType        = ImageContainer_GetType(Compare);
            uint8_t     RefNumViews    = ImageContainer_GetNumViews(Reference);
            uint8_t     ComNumViews    = ImageContainer_GetNumViews(Compare);
            uint8_t     RefNumChannels = ImageContainer_GetNumChannels(Reference);
            uint8_t     ComNumChannels = ImageContainer_GetNumChannels(Compare);
            uint64_t    RefHeight      = ImageContainer_GetHeight(Reference);
            uint64_t    ComHeight      = ImageContainer_GetHeight(Compare);
            uint64_t    RefWidth       = ImageContainer_GetWidth(Reference);
            uint64_t    ComWidth       = ImageContainer_GetWidth(Compare);
            if (RefType == ComType && RefNumViews == ComNumViews && RefNumChannels == ComNumChannels && RefHeight == ComHeight && RefWidth == ComWidth) {
                Difference          = ImageContainer_Init(RefType, ImageContainer_GetChannelMask(Reference), RefWidth, RefHeight);
                if (Difference != NULL) {
                    if (RefType == ImageType_Integer8 && ComType == ImageType_Integer8) {
                        uint8_t ****RefArray = (uint8_t****) ImageContainer_GetArray(Reference);
                        uint8_t ****ComArray = (uint8_t****) ImageContainer_GetArray(Compare);
                        uint8_t ****DifArray = (uint8_t****) ImageContainer_GetArray(Difference);
                        for (uint8_t View = 0; View < RefNumViews; View++) {
                            for (uint64_t Width = 0ULL; Width < RefWidth; Width++) {
                                for (uint64_t Height = 0ULL; Height < RefHeight; Height++) {
                                    for (uint8_t Channel = 0; Channel < RefNumChannels; Channel++) {
                                        DifArray[View][Width][Height][Channel] = (RefArray[View][Width][Height][Channel] - ComArray[View][Width][Height][Channel]) % 0xFF;
                                    }
                                }
                            }
                        }
                    } else if (RefType == ImageType_Integer16 && ComType == ImageType_Integer16) {
                        uint16_t ****RefArray = (uint16_t****) ImageContainer_GetArray(Reference);
                        uint16_t ****ComArray = (uint16_t****) ImageContainer_GetArray(Compare);
                        uint16_t ****DifArray = (uint16_t****) ImageContainer_GetArray(Difference);
                        for (uint8_t View = 0; View < RefNumViews; View++) {
                            for (uint64_t Width = 0ULL; Width < RefWidth; Width++) {
                                for (uint64_t Height = 0ULL; Height < RefHeight; Height++) {
                                    for (uint8_t Channel = 0; Channel < RefNumChannels; Channel++) {
                                        DifArray[View][Width][Height][Channel] = (RefArray[View][Width][Height][Channel] - ComArray[View][Width][Height][Channel]) % 0xFFFF;
                                    }
                                }
                            }
                        }
                    }
                } else {
                    Log(Log_DEBUG, __func__, UTF8String("Couldn't Allocate Difference"));
                }
            } else if (RefType != ComType) {
                Log(Log_DEBUG, __func__, UTF8String("Reference Type %d does not match Compare Type %d"), RefType, ComType);
            } else if (RefNumViews != ComNumViews) {
                Log(Log_DEBUG, __func__, UTF8String("Reference NumViews %d does not match Compare NumViews %d"), RefNumViews, ComNumViews);
            } else if (RefNumChannels != ComNumChannels) {
                Log(Log_DEBUG, __func__, UTF8String("Reference NumChannels %d does not match Compare NumChannels %d"), RefNumChannels, ComNumChannels);
            } else if (RefHeight != ComHeight) {
                Log(Log_DEBUG, __func__, UTF8String("Reference Height %lld does not match Compare Height %lld"), RefHeight, ComHeight);
            } else if (RefWidth != ComWidth) {
                Log(Log_DEBUG, __func__, UTF8String("Reference Width %lld does not match Compare Width %lld"), RefWidth, ComWidth);
            }
        } else if (Reference == NULL) {
            Log(Log_DEBUG, __func__, UTF8String("Reference Pointer is NULL"));
        } else if (Compare == NULL) {
            Log(Log_DEBUG, __func__, UTF8String("Compare Pointer is NULL"));
        }
        return Difference;
    }
    
    void ImageContainer_Erase(ImageContainer *Image) {
        if (Image != NULL) {
            if (Image->Type == ImageType_Integer8) {
                uint8_t ****Pixels = (uint8_t****) ImageContainer_GetArray(Image);
                
                for (uint64_t View = 0ULL; View < ImageContainer_GetNumViews(Image); View++) {
                    for (uint64_t W = 0ULL; W < Image->Width; W++) {
                        for (uint64_t H = 0ULL; H < Image->Height; H++) {
                            for (uint64_t Channel = 0ULL; Channel < ImageContainer_GetNumChannels(Image); Channel++) {
                                Pixels[View][W][H][Channel] = 0;
                            }
                        }
                    }
                }
            } else if (Image->Type == ImageType_Integer16) {
                uint16_t ****Pixels = (uint16_t****) ImageContainer_GetArray(Image);
                
                for (uint64_t View = 0ULL; View < ImageContainer_GetNumViews(Image); View++) {
                    for (uint64_t W = 0ULL; W < Image->Width; W++) {
                        for (uint64_t H = 0ULL; H < Image->Height; H++) {
                            for (uint64_t Channel = 0ULL; Channel < ImageContainer_GetNumChannels(Image); Channel++) {
                                Pixels[View][W][H][Channel] = 0;
                            }
                        }
                    }
                }
            }
        } else {
            Log(Log_DEBUG, __func__, UTF8String("ImageContainer Pointer is NULL"));
        }
    }
    
    void ImageContainer_Deinit(ImageContainer *Image) {
        if (Image != NULL) {
            free(Image->Pixels);
            free(Image->ChannelMap);
            free(Image);
        } else {
            Log(Log_DEBUG, __func__, UTF8String("ImageContainer Pointer is NULL"));
        }
    }
    
    typedef struct Audio2DHistogram {
        void              *Array;
        uint64_t           NumEntries;
        uint64_t           NumChannels;
        Audio_ChannelMask *ChannelMap;
        Audio_Types        Type;
    } Audio2DHistogram;
    
    Audio2DHistogram *Audio2DHistogram_Init(Audio2DContainer *Audio) {
        Audio2DHistogram *Histogram         = NULL;
        if (Audio != NULL) {
            Histogram                     = calloc(1, sizeof(Audio2DHistogram));
            uint64_t NumChannels          = Audio2DContainer_GetNumChannels(Audio);
            if (Histogram != NULL) {
                uint64_t NumValues        = Exponentiate(2, Audio->BitDepth);
                if (Audio->BitDepth <= 8) {
                    Histogram->Array      = calloc(NumValues * NumChannels, sizeof(uint8_t));
                } else if (Audio->BitDepth <= 16) {
                    Histogram->Array      = calloc(NumValues * NumChannels, sizeof(uint16_t));
                } else if (Audio->BitDepth <= 32) {
                    Histogram->Array      = calloc(NumValues * NumChannels, sizeof(uint32_t));
                }
                
                if (Histogram->Array != NULL) {
                    Histogram->Type       = Audio->Type;
                    Histogram->NumEntries = NumValues;
                } else {
                    Audio2DHistogram_Deinit(Histogram);
                    Log(Log_DEBUG, __func__, UTF8String("Couldn't allocate Audio2DHistogram Array"));
                }
            } else {
                Log(Log_DEBUG, __func__, UTF8String("Couldn't allocate Audio2DHistogram"));
            }
        } else {
            Log(Log_DEBUG, __func__, UTF8String("Audio2DContainer Pointer is NULL"));
        }
        return Histogram;
    }
    
    void *Audio2DHistogram_GetArray(Audio2DHistogram *Histogram) {
        void *Array = NULL;
        if (Histogram != NULL) {
            Array   = Histogram->Array;
        } else {
            Log(Log_DEBUG, __func__, UTF8String("Audio2DHistogram Pointer is NULL"));
        }
        return Array;
    }
    
    void Audio2DHistogram_SetArray(Audio2DHistogram *Histogram, void *Array) {
        if (Histogram != NULL) {
            Histogram->Array = Array;
        } else {
            Log(Log_DEBUG, __func__, UTF8String("Audio2DHistogram Pointer is NULL"));
        }
    }
    
    Audio2DHistogram *Audio2DHistogram_GenerateHistogram(Audio2DContainer *Audio) {
        Audio2DHistogram *Histogram                                = NULL;
        if (Audio != NULL) {
            Histogram                                            = Audio2DHistogram_Init(Audio);
            if (Histogram != NULL) {
                uint8_t  NumChannels                             = Audio2DContainer_GetNumChannels(Audio);
                
                if (Histogram->Type == AudioType_Integer8) {
                    uint8_t **SampleArray                        = (uint8_t**) Audio->Samples;
                    uint8_t *HistArray                           = (uint8_t*)  Histogram->Array;
                    for (uint64_t C = 0ULL; C < NumChannels; C++) {
                        for (uint64_t S = 0ULL; S < Audio->NumSamples; S++) {
                            uint8_t Sample                       = SampleArray[C][S];
                            HistArray[Sample]                   += 1;
                        }
                    }
                } else if (Histogram->Type == AudioType_Integer16) {
                    uint16_t **SampleArray                       = (uint16_t**) Audio->Samples;
                    uint16_t  *HistArray                         = (uint16_t*)  Histogram->Array;
                    
                    for (uint64_t C = 0ULL; C < NumChannels; C++) {
                        for (uint64_t S = 0ULL; S < Audio->NumSamples; S++) {
                            uint16_t Sample                      = SampleArray[C][S];
                            HistArray[Sample]                   += 1;
                        }
                    }
                } else if (Histogram->Type == AudioType_Integer32) {
                    uint32_t **SampleArray                       = (uint32_t**) Audio->Samples;
                    uint32_t  *HistArray                         = (uint32_t*)  Histogram->Array;
                    
                    for (uint64_t C = 0ULL; C < NumChannels; C++) {
                        for (uint64_t S = 0ULL; S < Audio->NumSamples; S++) {
                            uint32_t Sample                      = SampleArray[C][S];
                            HistArray[Sample]                   += 1;
                        }
                    }
                }
            } else {
                Log(Log_DEBUG, __func__, UTF8String("Couldn't allocate Audio2DHistogram"));
            }
        } else {
            Log(Log_DEBUG, __func__, UTF8String("Audio2DContainer Pointer is NULL"));
        }
        return Histogram;
    }
    
    void Audio2DHistogram_Sort(Audio2DHistogram *Histogram, bool SortAscending) {
        if (Histogram != NULL) {
            uint64_t NumCores = FoundationIO_GetNumCPUCores();
            if (SortAscending == Yes) { // Top to bottom
                if (Histogram->Type == (AudioType_Integer8 | AudioType_Unsigned)) {
                    uint8_t  *Audio = Audio2DHistogram_GetArray(Histogram);
                    for (uint64_t Core = 0ULL; Core < NumCores; Core++) {
                        for (uint64_t Element = 1ULL; Element < Histogram->NumEntries / NumCores; Element++) {
                            Audio[Element - 1] = (uint8_t) Maximum(Audio[Element - 1], Audio[Element]);
                        }
                    }
                } else if (Histogram->Type == (AudioType_Integer16 | AudioType_Unsigned)) {
                    uint16_t *Audio = Audio2DHistogram_GetArray(Histogram);
                    for (uint64_t Core = 0ULL; Core < NumCores; Core++) {
                        for (uint64_t Element = 1ULL; Element < Histogram->NumEntries / NumCores; Element++) {
                            Audio[Element - 1] = (uint16_t) Maximum(Audio[Element - 1], Audio[Element]);
                        }
                    }
                } else if (Histogram->Type == (AudioType_Integer32 | AudioType_Unsigned)) {
                    uint32_t *Audio = Audio2DHistogram_GetArray(Histogram);
                    for (uint64_t Core = 0ULL; Core < NumCores; Core++) {
                        for (uint64_t Element = 1ULL; Element < Histogram->NumEntries / NumCores; Element++) {
                            Audio[Element - 1] = (uint32_t) Maximum(Audio[Element - 1], Audio[Element]);
                        }
                    }
                } else if (Histogram->Type == (AudioType_Integer8 | AudioType_Signed)) {
                    int8_t  *Audio = Audio2DHistogram_GetArray(Histogram);
                    for (uint64_t Core = 0ULL; Core < NumCores; Core++) {
                        for (uint64_t Element = 1ULL; Element < Histogram->NumEntries / NumCores; Element++) {
                            Audio[Element - 1] = (int8_t) Maximum(Audio[Element - 1], Audio[Element]);
                        }
                    }
                } else if (Histogram->Type == (AudioType_Integer16 | AudioType_Signed)) {
                    int16_t *Audio = Audio2DHistogram_GetArray(Histogram);
                    for (uint64_t Core = 0ULL; Core < NumCores; Core++) {
                        for (uint64_t Element = 1ULL; Element < Histogram->NumEntries / NumCores; Element++) {
                            Audio[Element - 1] = (int16_t) Maximum(Audio[Element - 1], Audio[Element]);
                        }
                    }
                } else if (Histogram->Type == (AudioType_Integer32 | AudioType_Signed)) {
                    int32_t *Audio = Audio2DHistogram_GetArray(Histogram);
                    for (uint64_t Core = 0ULL; Core < NumCores; Core++) {
                        for (uint64_t Element = 1ULL; Element < Histogram->NumEntries / NumCores; Element++) {
                            Audio[Element - 1] = (int32_t) Maximum(Audio[Element - 1], Audio[Element]);
                        }
                    }
                }
            } else {
                if (Histogram->Type == (AudioType_Integer8 | AudioType_Unsigned)) {
                    uint8_t  *Audio = Audio2DHistogram_GetArray(Histogram);
                    for (uint64_t Core = 0ULL; Core < NumCores; Core++) {
                        for (uint64_t Element = 1ULL; Element < Histogram->NumEntries / NumCores; Element++) {
                            Audio[Element - 1] = (uint8_t) Minimum(Audio[Element - 1], Audio[Element]);
                        }
                    }
                } else if (Histogram->Type == (AudioType_Integer16 | AudioType_Unsigned)) {
                    uint16_t *Audio = Audio2DHistogram_GetArray(Histogram);
                    for (uint64_t Core = 0ULL; Core < NumCores; Core++) {
                        for (uint64_t Element = 1ULL; Element < Histogram->NumEntries / NumCores; Element++) {
                            Audio[Element - 1] = (uint16_t) Minimum(Audio[Element - 1], Audio[Element]);
                        }
                    }
                } else if (Histogram->Type == (AudioType_Integer32 | AudioType_Unsigned)) {
                    uint32_t *Audio = Audio2DHistogram_GetArray(Histogram);
                    for (uint64_t Core = 0ULL; Core < NumCores; Core++) {
                        for (uint64_t Element = 1ULL; Element < Histogram->NumEntries / NumCores; Element++) {
                            Audio[Element - 1] = (uint32_t) Minimum(Audio[Element - 1], Audio[Element]);
                        }
                    }
                } else if (Histogram->Type == (AudioType_Integer8 | AudioType_Signed)) {
                    int8_t  *Audio = Audio2DHistogram_GetArray(Histogram);
                    for (uint64_t Core = 0ULL; Core < NumCores; Core++) {
                        for (uint64_t Element = 1ULL; Element < Histogram->NumEntries / NumCores; Element++) {
                            Audio[Element - 1] = (int8_t) Minimum(Audio[Element - 1], Audio[Element]);
                        }
                    }
                } else if (Histogram->Type == (AudioType_Integer16 | AudioType_Signed)) {
                    int16_t *Audio = Audio2DHistogram_GetArray(Histogram);
                    for (uint64_t Core = 0ULL; Core < NumCores; Core++) {
                        for (uint64_t Element = 1ULL; Element < Histogram->NumEntries / NumCores; Element++) {
                            Audio[Element - 1] = (int16_t) Minimum(Audio[Element - 1], Audio[Element]);
                        }
                    }
                } else if (Histogram->Type == (AudioType_Integer32 | AudioType_Signed)) {
                    int32_t *Audio = Audio2DHistogram_GetArray(Histogram);
                    for (uint64_t Core = 0ULL; Core < NumCores; Core++) {
                        for (uint64_t Element = 1ULL; Element < Histogram->NumEntries / NumCores; Element++) {
                            Audio[Element - 1] = (int32_t) Minimum(Audio[Element - 1], Audio[Element]);
                        }
                    }
                }
            }
        } else {
            Log(Log_DEBUG, __func__, UTF8String("ImageHistogram Pointer is NULL"));
        }
    }
    
    void Audio2DHistogram_Erase(Audio2DHistogram *Histogram) {
        if (Histogram != NULL) {
            if (Histogram->Type == (AudioType_Unsigned | AudioType_Integer8)) {
                uint8_t **Samples = (uint8_t**) Audio2DHistogram_GetArray(Histogram);
                
                for (uint64_t Channel = 0ULL; Channel < Histogram->NumChannels; Channel++) {
                    for (uint64_t Sample = 0ULL; Sample < Histogram->NumEntries; Sample++) {
                        Samples[Channel][Sample] = 0;
                    }
                }
            } else if (Histogram->Type == (AudioType_Signed | AudioType_Integer8)) {
                int8_t **Samples  = (int8_t**) Audio2DHistogram_GetArray(Histogram);
                
                for (uint64_t Channel = 0ULL; Channel < Histogram->NumChannels; Channel++) {
                    for (uint64_t Sample = 0ULL; Sample < Histogram->NumEntries; Sample++) {
                        Samples[Channel][Sample] = 0;
                    }
                }
            } else if (Histogram->Type == (AudioType_Unsigned | AudioType_Integer16)) {
                uint16_t **Samples  = (uint16_t**) Audio2DHistogram_GetArray(Histogram);
                
                for (uint64_t Channel = 0ULL; Channel < Histogram->NumChannels; Channel++) {
                    for (uint64_t Sample = 0ULL; Sample < Histogram->NumEntries; Sample++) {
                        Samples[Channel][Sample] = 0;
                    }
                }
            } else if (Histogram->Type == (AudioType_Signed | AudioType_Integer16)) {
                int16_t **Samples  = (int16_t**) Audio2DHistogram_GetArray(Histogram);
                
                for (uint64_t Channel = 0ULL; Channel < Histogram->NumChannels; Channel++) {
                    for (uint64_t Sample = 0ULL; Sample < Histogram->NumEntries; Sample++) {
                        Samples[Channel][Sample] = 0;
                    }
                }
            } else if (Histogram->Type == (AudioType_Unsigned | AudioType_Integer32)) {
                uint32_t **Samples  = (uint32_t**) Audio2DHistogram_GetArray(Histogram);
                
                for (uint64_t Channel = 0ULL; Channel < Histogram->NumChannels; Channel++) {
                    for (uint64_t Sample = 0ULL; Sample < Histogram->NumEntries; Sample++) {
                        Samples[Channel][Sample] = 0;
                    }
                }
            } else if (Histogram->Type == (AudioType_Signed | AudioType_Integer32)) {
                int32_t **Samples  = (int32_t**) Audio2DHistogram_GetArray(Histogram);
                
                for (uint64_t Channel = 0ULL; Channel < Histogram->NumChannels; Channel++) {
                    for (uint64_t Sample = 0ULL; Sample < Histogram->NumEntries; Sample++) {
                        Samples[Channel][Sample] = 0;
                    }
                }
            }
        } else {
            Log(Log_DEBUG, __func__, UTF8String("Audio2DHistogram Pointer is NULL"));
        }
    }
    
    void Audio2DHistogram_Deinit(Audio2DHistogram *Histogram) {
        free(Histogram->Array);
        free(Histogram);
    }
    
    typedef struct ImageHistogram {
        void              ***Array;
        Image_ChannelMask   *ChannelMap;
        uint64_t             Width;
        uint64_t             Height;
        Image_ChannelMask    ChannelMask;
        uint8_t              BitDepth;
        Image_Types          Type;
    } ImageHistogram;
    
    ImageHistogram *ImageHistogram_Init(ImageContainer *Image) {
        ImageHistogram *Histogram                    = NULL;
        if (Image != NULL) {
            Histogram                                = calloc(1, sizeof(Histogram));
            if (Histogram != NULL) {
                uint8_t  NumViews                    = ImageContainer_GetNumViews(Image);
                uint8_t  NumChannels                 = ImageContainer_GetNumChannels(Image);
                uint64_t NumPossibleColors           = Exponentiate(2, Image->BitDepth);
                
                Histogram->ChannelMap                = Image->ChannelMap;
                Histogram->ChannelMask               = Image->ChannelMask;
                Histogram->Height                    = Image->Height;
                Histogram->Width                     = Image->Width;
                Histogram->BitDepth                  = Image->BitDepth;
                
                if (Image->Type == ImageType_Integer8) {
                    uint8_t ***HistogramArray        = calloc(NumViews * NumChannels * NumPossibleColors, sizeof(uint8_t));
                    if (HistogramArray != NULL) {
                        Histogram->Array             = (void***) HistogramArray;
                        Histogram->Type              = Image->Type;
                    } else {
                        ImageHistogram_Deinit(Histogram);
                        Log(Log_DEBUG, __func__, UTF8String("Couldn't allocate Histogram array"));
                    }
                } else if (Image->Type == ImageType_Integer16) {
                    uint16_t ***HistogramArray       = calloc(NumViews * NumChannels * NumPossibleColors, sizeof(uint16_t));
                    if (HistogramArray != NULL) {
                        Histogram->Array             = (void***) HistogramArray;
                        Histogram->Type              = Image->Type;
                    } else {
                        ImageHistogram_Deinit(Histogram);
                        Log(Log_DEBUG, __func__, UTF8String("Couldn't allocate Histogram array"));
                    }
                }
            } else {
                ImageHistogram_Deinit(Histogram);
                Log(Log_DEBUG, __func__, UTF8String("Couldn't allocate ImageHistogram"));
            }
        } else {
            Log(Log_DEBUG, __func__, UTF8String("ImageContainer Pointer is NULL"));
        }
        return Histogram;
    }
    
    void ***ImageHistogram_GetArray(ImageHistogram *Histogram) {
        void ***Array = NULL;
        if (Histogram != NULL) {
            if (Histogram->Type == ImageType_Integer8) {
                Array = Histogram->Array;
            } else if (Histogram->Type == ImageType_Integer16) {
                Array = Histogram->Array;
            }
        } else {
            Log(Log_DEBUG, __func__, UTF8String("ImageHistogram Pointer is NULL"));
        }
        return Array;
    }
    
    void ImageHistogram_SetArray(ImageHistogram *Histogram, void ***Array) {
        if (Histogram != NULL && Array != NULL) {
            Histogram->Array = Array;
        } else if (Histogram == NULL) {
            Log(Log_DEBUG, __func__, UTF8String("ImageHistogram Pointer is NULL"));
        } else if (Array == NULL) {
            Log(Log_DEBUG, __func__, UTF8String("Array Pointer is NULL"));
        }
    }
    
    ImageHistogram *ImageHistogram_GenerateHistogram(ImageContainer *Image) {
        ImageHistogram *Histogram                               = NULL;
        if (Image != NULL) {
            Histogram                                           = ImageHistogram_Init(Image);
            if (Histogram != NULL) {
                uint8_t  NumViews                               = ImageContainer_GetNumViews(Image);
                uint8_t  NumChannels                            = ImageContainer_GetNumChannels(Image);
                uint64_t Width                                  = ImageContainer_GetWidth(Image);
                uint64_t Height                                 = ImageContainer_GetHeight(Image);
                
                if (Histogram->Type == ImageType_Integer8) {
                    uint8_t ****ImageArray                      = (uint8_t****) Image->Pixels;
                    uint8_t ***HistArray                        = (uint8_t***)  Histogram->Array;
                    
                    for (uint64_t View = 0ULL; View < NumViews; View++) {
                        for (uint64_t W = 0ULL; W < Width; W++) {
                            for (uint64_t H = 0ULL; H < Height; H++) {
                                for (uint64_t C = 0ULL; C < NumChannels; C++) {
                                    uint8_t Sample              = ImageArray[View][W][H][C];
                                    HistArray[View][C][Sample] += 1;
                                }
                            }
                        }
                    }
                } else if (Histogram->Type == ImageType_Integer16) {
                    uint16_t ****ImageArray                     = (uint16_t****) Image->Pixels;
                    uint16_t ***HistArray                       = (uint16_t***)  Histogram->Array;
                    
                    for (uint64_t View = 0ULL; View < NumViews; View++) {
                        for (uint64_t W = 0ULL; W < Width; W++) {
                            for (uint64_t H = 0ULL; H < Height; H++) {
                                for (uint64_t C = 0ULL; C < NumChannels; C++) {
                                    uint16_t Sample             = ImageArray[View][W][H][C];
                                    HistArray[View][C][Sample] += 1;
                                }
                            }
                        }
                    }
                }
            } else {
                Log(Log_DEBUG, __func__, UTF8String("Couldn't allocate ImageHistogram"));
            }
        } else {
            Log(Log_DEBUG, __func__, UTF8String("ImageContainer Pointer is NULL"));
        }
        return Histogram;
    }
    
    void ImageHistogram_Sort(ImageHistogram *Histogram, bool SortAscending) {
        if (Histogram != NULL) {
            if (SortAscending == Yes) { // Top to bottom
                if (Histogram->Type == ImageType_Integer8) {
                    uint8_t ***Image = (uint8_t***) ImageHistogram_GetArray(Histogram);
                    for (uint64_t View = 0ULL; View < ImageMask_GetNumViews(Histogram->ChannelMask); View++) {
                        for (uint64_t Width = 0ULL; Width < Histogram->Width; Width++) {
                            for (uint64_t Height = 0ULL; Height < Histogram->Height; Height++) {
                                for (uint64_t Channel = 1ULL; Channel < ImageMask_GetNumChannels(Histogram->ChannelMask); Channel++) {
                                    uint8_t PreviousChannel = *Image[View][Channel - 1];
                                    uint8_t CurrentChannel  = *Image[View][Channel];
                                    *Image[View][Channel]   = (uint8_t) Minimum(PreviousChannel, CurrentChannel);
                                }
                            }
                        }
                    }
                } else if (Histogram->Type == ImageType_Integer16) {
                    uint16_t ***Image = (uint16_t***) ImageHistogram_GetArray(Histogram);
                    for (uint64_t View = 0ULL; View < ImageMask_GetNumViews(Histogram->ChannelMask); View++) {
                        for (uint64_t Width = 0ULL; Width < Histogram->Width; Width++) {
                            for (uint64_t Height = 0ULL; Height < Histogram->Height; Height++) {
                                for (uint64_t Channel = 1ULL; Channel < ImageMask_GetNumChannels(Histogram->ChannelMask); Channel++) {
                                    uint16_t PreviousChannel = *Image[View][Channel - 1];
                                    uint16_t CurrentChannel  = *Image[View][Channel];
                                    *Image[View][Channel]    = (uint16_t) Minimum(PreviousChannel, CurrentChannel);
                                }
                            }
                        }
                    }
                }
            } else { // Bottom to top
                if (Histogram->Type == ImageType_Integer8) {
                    uint8_t ***Image = (uint8_t***) ImageHistogram_GetArray(Histogram);
                    for (uint64_t View = 0ULL; View < ImageMask_GetNumViews(Histogram->ChannelMask); View++) {
                        for (uint64_t Width = 0ULL; Width < Histogram->Width; Width++) {
                            for (uint64_t Height = 0ULL; Height < Histogram->Height; Height++) {
                                for (uint64_t Channel = 1ULL; Channel < ImageMask_GetNumChannels(Histogram->ChannelMask); Channel++) {
                                    uint8_t PreviousChannel = *Image[View][Channel - 1];
                                    uint8_t CurrentChannel  = *Image[View][Channel];
                                    *Image[View][Channel]   = (uint8_t) Minimum(PreviousChannel, CurrentChannel);
                                }
                            }
                        }
                    }
                } else if (Histogram->Type == ImageType_Integer16) {
                    uint16_t ***Image = (uint16_t***) ImageHistogram_GetArray(Histogram);
                    for (uint64_t View = 0ULL; View < ImageMask_GetNumViews(Histogram->ChannelMask); View++) {
                        for (uint64_t Width = 0ULL; Width < Histogram->Width; Width++) {
                            for (uint64_t Height = 0ULL; Height < Histogram->Height; Height++) {
                                for (uint64_t Channel = 1ULL; Channel < ImageMask_GetNumChannels(Histogram->ChannelMask); Channel++) {
                                    uint16_t PreviousChannel = *Image[View][Channel - 1];
                                    uint16_t CurrentChannel  = *Image[View][Channel];
                                    *Image[View][Channel]    = (uint16_t) Minimum(PreviousChannel, CurrentChannel);
                                }
                            }
                        }
                    }
                }
            }
        } else {
            Log(Log_DEBUG, __func__, UTF8String("ImageHistogram Pointer is NULL"));
        }
    }
    
    void ImageHistogram_Erase(ImageHistogram *Histogram) {
        if (Histogram != NULL) {
            if (Histogram->Type == ImageType_Integer8) {
                uint8_t ****Pixels = (uint8_t****) ImageHistogram_GetArray(Histogram);
                
                for (uint64_t View = 0ULL; View < ImageMask_GetNumViews(Histogram->ChannelMask); View++) {
                    for (uint64_t W = 0ULL; W < Histogram->Width; W++) {
                        for (uint64_t H = 0ULL; H < Histogram->Height; H++) {
                            for (uint64_t Channel = 0ULL; Channel < ImageMask_GetNumChannels(Histogram->ChannelMask); Channel++) {
                                Pixels[View][W][H][Channel] = 0;
                            }
                        }
                    }
                }
            } else if (Histogram->Type == ImageType_Integer16) {
                uint16_t ****Pixels = (uint16_t****) ImageHistogram_GetArray(Histogram);
                
                for (uint64_t View = 0ULL; View < ImageMask_GetNumViews(Histogram->ChannelMask); View++) {
                    for (uint64_t W = 0ULL; W < Histogram->Width; W++) {
                        for (uint64_t H = 0ULL; H < Histogram->Height; H++) {
                            for (uint64_t Channel = 0ULL; Channel < ImageMask_GetNumChannels(Histogram->ChannelMask); Channel++) {
                                Pixels[View][W][H][Channel] = 0;
                            }
                        }
                    }
                }
            }
        } else {
            Log(Log_DEBUG, __func__, UTF8String("ImageHistogram Pointer is NULL"));
        }
    }
    
    void ImageHistogram_Deinit(ImageHistogram *Histogram) {
        if (Histogram != NULL) {
            free(Histogram->Array);
            free(Histogram);
        } else {
            Log(Log_DEBUG, __func__, UTF8String("ImageHistogram Pointer is NULL"));
        }
    }
    
#ifdef __cplusplus
}
#endif
