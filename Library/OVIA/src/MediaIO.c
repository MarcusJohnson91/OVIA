#include "../include/MediaIO.h"                                                /* Included for our declarations */

#include "../../../Dependencies/FoundationIO/Library/include/AssertIO.h"       /* Included for Assertions */
#include "../../../Dependencies/FoundationIO/Library/include/MathIO.h"         /* Included for Exponentiate */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    uint8_t AudioType_GetBitDepth(MediaIO_AudioTypes AudioType) {
        AssertIO(AudioType != AudioType_Unknown);
        return ((AudioType & (AudioType_Integer8 | AudioType_Integer16 | AudioType_Integer32)) >> 2) * 8;
    }

    uint8_t ImageType_GetBitDepth(MediaIO_ImageTypes ImageType) {
        AssertIO(ImageType != ImageType_Unknown);
        return (ImageType & (ImageType_Integer8 | ImageType_Integer16 | ImageType_Integer32)) * 8;
    }

    typedef struct FlatHistogram {
        uint64_t Histogram[256];
    } FlatHistogram;

    FlatHistogram *FlatHistogram_Init(void) {
        FlatHistogram *Flat = calloc(1, sizeof(FlatHistogram));
        AssertIO(Flat != NULL);
        return Flat;
    }

    uint64_t *FlatHistogram_GetArray(FlatHistogram *Flat) {
        AssertIO(Flat != NULL);
        return Flat->Histogram;
    }

    static int Sort_Compare_Ascending(const void *A, const void *B) {
        return *(const int*)A - *(const int*)B;
    }

    static int Sort_Compare_Descending(const void *A, const void *B) {
        return *(const int*)B - *(const int*)A;
    }

    void FlatHistogram_Sort(FlatHistogram *Flat) {
        AssertIO(Flat != NULL);
        qsort(Flat->Histogram, 256, sizeof(uint64_t), Sort_Compare_Descending);
    }

    void FlatHistogram_Deinit(FlatHistogram *Flat) {
        AssertIO(Flat != NULL);
        free(Flat);
    }
    
    typedef struct AudioChannelMap {
        MediaIO_AudioMask  *Map;
        uint64_t            NumChannels;
    } AudioChannelMap;
    
    AudioChannelMap *AudioChannelMap_Init(uint64_t NumChannels) {
        AssertIO(NumChannels > 0);
        AudioChannelMap *ChannelMap = calloc(1, sizeof(AudioChannelMap));
        AssertIO(ChannelMap != NULL);
        ChannelMap->NumChannels = NumChannels;
        ChannelMap->Map         = calloc(NumChannels, sizeof(MediaIO_AudioMask));
        return ChannelMap;
    }
    
    void AudioChannelMap_AddMask(AudioChannelMap *ChannelMap, uint64_t Index, MediaIO_AudioMask Mask) {
        AssertIO(ChannelMap != NULL);
        AssertIO(Index < ChannelMap->NumChannels);
        ChannelMap->Map[Index] = Mask;
    }
    
    MediaIO_AudioMask AudioChannelMap_GetMask(AudioChannelMap *ChannelMap, uint64_t Index) {
        AssertIO(ChannelMap != NULL);
        AssertIO(Index < ChannelMap->NumChannels);
        MediaIO_AudioMask ChannelMask = ChannelMap->Map[Index];
        return ChannelMask;
    }
    
    uint64_t AudioChannelMap_GetLowestUnusedIndex(AudioChannelMap *ChannelMap) {
        AssertIO(ChannelMap != NULL);
        uint64_t Index       = 0xFFFFFFFFFFFFFFFF;
        Index            = ChannelMap->NumChannels;
        uint64_t Channel = 0ULL;
        while (Channel < ChannelMap->NumChannels) {
            if (ChannelMap->Map[Channel] != AudioMask_Unknown) {
                Index    = Channel;
                break;
            }
            Channel     += 1;
        }
        return Index;
    }
    
    void AudioChannelMap_Deinit(AudioChannelMap *ChannelMap) {
        AssertIO(ChannelMap != NULL);
        free(ChannelMap->Map);
        free(ChannelMap);
    }
    
    /* Audio2DContainer */
    typedef struct Audio2DContainer {
        void              **Samples;
        AudioChannelMap    *ChannelMap;
        uint64_t            NumChannels;
        uint64_t            NumSamples;
        uint64_t            SampleRate;
        uint64_t            Offset;
        MediaIO_AudioTypes  Type;
    } Audio2DContainer;
    
    Audio2DContainer *Audio2DContainer_Init(MediaIO_AudioTypes Type, AudioChannelMap *ChannelMap, uint64_t SampleRate, uint64_t NumSamples) {
        AssertIO(Type != AudioMask_Unknown);
        AssertIO(ChannelMap != NULL);
        AssertIO(SampleRate > 0);
        AssertIO(NumSamples > 0);
        Audio2DContainer *Audio = (Audio2DContainer*) calloc(1, sizeof(Audio2DContainer));
        void **Array       = (void**) calloc(ChannelMap->NumChannels * NumSamples, Type / 4);
        AssertIO(Array != NULL);

        Audio->Samples = Array;
        Audio->SampleRate  = SampleRate;
        Audio->NumSamples  = NumSamples;
        return Audio;
    }
    
    AudioChannelMap *Audio2DContainer_GetChannelMap(Audio2DContainer *Audio) {
        AssertIO(Audio != NULL);
        AudioChannelMap *Map = Audio->ChannelMap;
        return Map;
    }
    
    uint8_t Audio2DContainer_GetBitDepth(Audio2DContainer *Audio) {
        AssertIO(Audio != NULL);
        uint8_t BitDepth = 0;
        if ((Audio->Type & AudioType_Integer8) == AudioType_Integer8) { // Convert to PlatformIO_Is
            BitDepth = 8;
        } else if ((Audio->Type & AudioType_Integer16) == AudioType_Integer16) {
            BitDepth = 16;
        } else if ((Audio->Type & AudioType_Integer32) == AudioType_Integer32) {
            BitDepth = 32;
        }
        return BitDepth;
    }
    
    uint64_t Audio2DContainer_GetNumChannels(Audio2DContainer *Audio) {
        AssertIO(Audio != NULL);
        uint64_t NumChannels = Audio->NumChannels;
        return NumChannels;
    }
    
    uint64_t Audio2DContainer_GetSampleRate(Audio2DContainer *Audio) {
        AssertIO(Audio != NULL);
        uint64_t SampleRate = Audio->SampleRate;
        return SampleRate;
    }
    
    uint64_t Audio2DContainer_GetNumSamples(Audio2DContainer *Audio) {
        AssertIO(Audio != NULL);
        uint64_t NumSamples = Audio->NumSamples;
        return NumSamples;
    }
    
    MediaIO_AudioTypes Audio2DContainer_GetType(Audio2DContainer *Audio) {
        AssertIO(Audio != NULL);
        MediaIO_AudioTypes Type = Audio->Type;
        return Type;
    }
    
    void **Audio2DContainer_GetArray(Audio2DContainer *Audio) {
        AssertIO(Audio != NULL);
        void **AudioArray = Audio->Samples;
        return AudioArray;
    }
    
    int64_t Audio2DContainer_GetAverage(Audio2DContainer *Audio, uint64_t Channel) {
        AssertIO(Audio != NULL);
        AssertIO(Channel < Audio->NumChannels);
        int64_t Average = 0LL;

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
                int16_t **Array = (int16_t **) Audio2DContainer_GetArray(Audio);
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
        return Average;
    }
    
    int64_t Audio2DContainer_GetMax(Audio2DContainer *Audio, uint64_t Channel) {
        AssertIO(Audio != NULL);
        AssertIO(Channel < Audio->NumChannels);
        int64_t Maximum = INT64_MIN;
        if (Audio->Type == (AudioType_Unsigned | AudioType_Integer8)) {
            uint8_t **Array = (uint8_t**) Audio2DContainer_GetArray(Audio);
            uint8_t   Max   = (uint8_t) Exponentiate(2, Audio2DContainer_GetBitDepth(Audio)) - 1;
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
            int8_t   Max   = (int8_t) Exponentiate(2, Audio2DContainer_GetBitDepth(Audio) - 1) - 1;
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
            uint16_t   Max   = (uint16_t) Exponentiate(2, Audio2DContainer_GetBitDepth(Audio)) - 1;
            for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                if (Array[Channel][Sample] > Maximum) {
                    Maximum = Array[Channel][Sample];
                }
                if (Array[Channel][Sample] == Max) {
                    break;
                }
            }
        } else if (Audio->Type == (AudioType_Signed | AudioType_Integer16)) {
            int16_t **Array = (int16_t **) Audio2DContainer_GetArray(Audio);
            int16_t   Max   = (int16_t) Exponentiate(2, Audio2DContainer_GetBitDepth(Audio) - 1) - 1;
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
            uint32_t   Max   = (uint32_t) Exponentiate(2, Audio2DContainer_GetBitDepth(Audio)) - 1;
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
            int32_t   Max   = (int32_t) Exponentiate(2, Audio2DContainer_GetBitDepth(Audio) - 1) - 1;
            for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                if (Array[Channel][Sample] > Maximum) {
                    Maximum = Array[Channel][Sample];
                }
                if (Array[Channel][Sample] == Max) {
                    break;
                }
            }
        }
        return Maximum;
    }
    
    int64_t Audio2DContainer_GetMin(Audio2DContainer *Audio, uint64_t Channel) {
        AssertIO(Audio != NULL);
        AssertIO(Channel < Audio->NumChannels);
        int64_t Minimum = INT64_MAX;
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
            int16_t **Array = (int16_t **) Audio2DContainer_GetArray(Audio);
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
                if (Array[Channel][Sample] == 0x80000000) {
                    break;
                }
            }
        }
        return Minimum;
    }
    
    uint8_t Audio2DContainer_Erase(Audio2DContainer *Audio, uint8_t NewValue) {
        AssertIO(Audio != NULL);
        uint8_t Verification = 0xFE;
        if (Audio->Type == (AudioType_Unsigned | AudioType_Integer8)) {
            uint8_t **Samples = (uint8_t**) Audio2DContainer_GetArray(Audio);

            for (uint64_t Channel = 0ULL; Channel < Audio2DContainer_GetNumChannels(Audio); Channel++) {
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    Samples[Channel][Sample] = NewValue;
                }
            }
            Verification = (Samples[0][0] & 0xFF);
        } else if (Audio->Type == (AudioType_Signed | AudioType_Integer8)) {
            int8_t **Samples = (int8_t**) Audio2DContainer_GetArray(Audio);

            for (uint64_t Channel = 0ULL; Channel < Audio2DContainer_GetNumChannels(Audio); Channel++) {
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    Samples[Channel][Sample] = NewValue;
                }
            }
            Verification = (Samples[0][0] & 0xFF);
        } else if (Audio->Type == (AudioType_Unsigned | AudioType_Integer16)) {
            uint16_t **Samples = (uint16_t**) Audio2DContainer_GetArray(Audio);

            for (uint64_t Channel = 0ULL; Channel < Audio2DContainer_GetNumChannels(Audio); Channel++) {
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    Samples[Channel][Sample] = NewValue;
                }
            }
            Verification = (Samples[0][0] & 0xFF);
        } else if (Audio->Type == (AudioType_Signed | AudioType_Integer16)) {
            int16_t **Samples = (int16_t **) Audio2DContainer_GetArray(Audio);

            for (uint64_t Channel = 0ULL; Channel < Audio2DContainer_GetNumChannels(Audio); Channel++) {
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    Samples[Channel][Sample] = NewValue;
                }
            }
            Verification = (Samples[0][0] & 0xFF);
        } else if (Audio->Type == (AudioType_Unsigned | AudioType_Integer32)) {
            uint32_t **Samples = (uint32_t**) Audio2DContainer_GetArray(Audio);

            for (uint64_t Channel = 0ULL; Channel < Audio2DContainer_GetNumChannels(Audio); Channel++) {
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    Samples[Channel][Sample] = NewValue;
                }
            }
            Verification = (Samples[0][0] & 0xFF);
        } else if (Audio->Type == (AudioType_Signed | AudioType_Integer32)) {
            int32_t **Samples = (int32_t**) Audio2DContainer_GetArray(Audio);

            for (uint64_t Channel = 0ULL; Channel < Audio2DContainer_GetNumChannels(Audio); Channel++) {
                for (uint64_t Sample = 0ULL; Sample < Audio->NumSamples; Sample++) {
                    Samples[Channel][Sample] = NewValue;
                }
            }
            Verification = (Samples[0][0] & 0xFF);
        }
        return Verification;
    }
    
    void Audio2DContainer_Deinit(Audio2DContainer *Audio) {
        AssertIO(Audio != NULL);
        free(Audio->Samples);
        free(Audio->ChannelMap);
        free(Audio);
    }
    
    /*
     Audio ChannelMap stuff
     
     Since we're making Audio_ChannelMap a real thing, let's NULL terminate the array and drop the num channels variable from the init functions
     
     MediaIO_AudioMask
     */
    
    typedef struct Audio2DHistogram {
        void                     **Array; // Channel, Sample
        MediaIO_AudioMask         *ChannelMap;
        uint64_t                   NumEntries;
        uint8_t                    NumChannels;
        MediaIO_AudioTypes         Type;
    } Audio2DHistogram;
    
    Audio2DHistogram *Audio2DHistogram_Init(Audio2DContainer *Audio) {
        AssertIO(Audio != NULL);
        Audio2DHistogram *Histogram = calloc(1, sizeof(Audio2DHistogram));
        AssertIO(Histogram != NULL);
        uint8_t  NumChannels      = Audio2DContainer_GetNumChannels(Audio);
        uint64_t NumValues        = Exponentiate(2, Audio2DContainer_GetBitDepth(Audio));
        Histogram->Array          = calloc(NumChannels, sizeof(uint64_t*));
        AssertIO(Histogram->Array != NULL);
        for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
            Histogram->Array[Channel] = calloc(NumValues, sizeof(uint64_t));
        }

        Histogram->Type       = Audio->Type;
        Histogram->NumEntries = NumValues;
        return Histogram;
    }
    
    void **Audio2DHistogram_GetArray(Audio2DHistogram *Histogram) {
        AssertIO(Histogram != NULL);
        return Histogram->Array;
    }
    
    void Audio2DHistogram_SetArray(Audio2DHistogram *Histogram, void **Array) {
        AssertIO(Histogram != NULL);
        AssertIO(Array != NULL);
        Histogram->Array = Array;
    }

    FlatHistogram *Audio2DHistogram_Flatten(Audio2DHistogram *Histogram) {
        AssertIO(Histogram != NULL);
        FlatHistogram *Flat                     = NULL;
        if ((Histogram->Type & AudioType_Integer8) == AudioType_Integer8) {
            uint8_t **Array                 = (uint8_t**) Histogram->Array;
            for (uint8_t Channel = 0; Channel < Histogram->NumChannels; Channel++) {
                for (uint64_t Element = 0; Element < Histogram->NumEntries; Element++) {
                    uint8_t Value           = Array[Channel][Element];
                    Flat->Histogram[Value] += 1;
                }
            }
        } else if ((Histogram->Type & AudioType_Integer16) == AudioType_Integer16) {
            uint16_t **Array                 = (uint16_t**) Histogram->Array;
            for (uint8_t Channel = 0; Channel < Histogram->NumChannels; Channel++) {
                for (uint64_t Element = 0; Element < Histogram->NumEntries; Element++) {
                    uint8_t Value1           = (Array[Channel][Element] & 0xFF00) >> 8;
                    uint8_t Value2           =  Array[Channel][Element] & 0xFF;
                    Flat->Histogram[Value1] += 1;
                    Flat->Histogram[Value2] += 1;
                }
            }
        } else if ((Histogram->Type & AudioType_Integer32) == AudioType_Integer32) {
            uint32_t **Array                 = (uint32_t**) Histogram->Array;
            for (uint8_t Channel = 0; Channel < Histogram->NumChannels; Channel++) {
                for (uint64_t Element = 0; Element < Histogram->NumEntries; Element++) {
                    uint8_t Value1           = (Array[Channel][Element] & 0xFF000000) >> 24;
                    uint8_t Value2           = (Array[Channel][Element] & 0xFF0000) >> 16;
                    uint8_t Value3           = (Array[Channel][Element] & 0xFF00) >> 8;
                    uint8_t Value4           =  Array[Channel][Element] & 0xFF;
                    Flat->Histogram[Value1] += 1;
                    Flat->Histogram[Value2] += 1;
                    Flat->Histogram[Value3] += 1;
                    Flat->Histogram[Value4] += 1;
                }
            }
        }
        return Flat;
    }
    
    Audio2DHistogram *Audio2DHistogram_Generate(Audio2DContainer *Audio) {
        AssertIO(Audio != NULL);
        Audio2DHistogram *Histogram = Audio2DHistogram_Init(Audio);
        AssertIO(Histogram != NULL);
        uint8_t NumChannels = Audio2DContainer_GetNumChannels(Audio);

        if (Histogram->Type == AudioType_Integer8) {
            uint8_t **SampleArray = (uint8_t**) Audio->Samples;
            uint8_t  *HistArray   = (uint8_t*) Histogram->Array;
            for (uint64_t C = 0ULL; C < NumChannels; C++) {
                for (uint64_t S = 0ULL; S < Audio->NumSamples; S++) {
                    uint8_t Sample = SampleArray[C][S];
                    HistArray[Sample] += 1;
                }
            }
        } else if (Histogram->Type == AudioType_Integer16) {
            uint16_t **SampleArray = (uint16_t**) Audio->Samples;
            uint16_t  *HistArray   = (uint16_t*) Histogram->Array;

            for (uint64_t C = 0ULL; C < NumChannels; C++) {
                for (uint64_t S = 0ULL; S < Audio->NumSamples; S++) {
                    uint16_t Sample = SampleArray[C][S];
                    HistArray[Sample] += 1;
                }
            }
        } else if (Histogram->Type == AudioType_Integer32) {
            uint32_t **SampleArray = (uint32_t**) Audio->Samples;
            uint32_t * HistArray   = (uint32_t*) Histogram->Array;

            for (uint64_t C = 0ULL; C < NumChannels; C++) {
                for (uint64_t S = 0ULL; S < Audio->NumSamples; S++) {
                    uint32_t Sample = SampleArray[C][S];
                    HistArray[Sample] += 1;
                }
            }
        }
        return Histogram;
    }
    
    void Audio2DHistogram_Sort(Audio2DHistogram *Histogram, MediaIO_SortTypes Sort) {
        AssertIO(Histogram != NULL);
        AssertIO(Sort != SortType_Unknown);
        if (Sort == SortType_Descending) {
            qsort(Histogram->Array, Histogram->NumEntries, sizeof(uint64_t), Sort_Compare_Descending);
        } else if (Sort == SortType_Ascending) {
            qsort(Histogram->Array, Histogram->NumEntries, sizeof(uint64_t), Sort_Compare_Ascending);
        }
    }
    
    Audio2DContainer *Audio2DContainer_Upsample(Audio2DContainer *Container2Upsample, uint64_t SampleRate, uint8_t BitDepth) {
        AssertIO(Container2Upsample != NULL);
        AssertIO(SampleRate > 0);
        AssertIO(BitDepth > 0);
        Audio2DContainer *Upsammpled = NULL;
        return Upsammpled;
    }
    
    Audio2DContainer *Audio2DContainer_Mix(Audio2DContainer *Container1, Audio2DContainer *Container2) {
        AssertIO(Container1 != NULL);
        AssertIO(Container2 != NULL);
        Audio2DContainer *Mixed = NULL;
        // Get the sample rate bitdepth numchannels etc for each container
        uint64_t Container1_NumChannels = Audio2DContainer_GetNumChannels(Container1);
        uint64_t Container1_SampleRate  = Audio2DContainer_GetSampleRate(Container1);
        uint8_t  Container1_BitDepth    = Audio2DContainer_GetBitDepth(Container1);
        uint64_t Container2_NumChannels = Audio2DContainer_GetNumChannels(Container2);
        uint64_t Container2_SampleRate  = Audio2DContainer_GetSampleRate(Container2);
        uint8_t  Container2_BitDepth    = Audio2DContainer_GetBitDepth(Container2);

        uint64_t New_NumChannels        = Maximum(Container1_NumChannels, Container2_NumChannels);
        uint64_t New_SampleRate         = Maximum(Container1_SampleRate, Container2_SampleRate);
        uint8_t  New_BitDepth           = Maximum(Container1_BitDepth, Container2_BitDepth);

        /*

         Now we need to take the highest value from either container and double it and create our new container with those variables

         then we need to upsample

         */
        return Mixed;
    }
    
    uint8_t Audio2DHistogram_Erase(Audio2DHistogram *Histogram, uint8_t NewValue) {
        AssertIO(Histogram != NULL);
        uint8_t Verification = 0xFE;
        if (Histogram->Type == (AudioType_Unsigned | AudioType_Integer8)) {
            uint8_t **Samples = (uint8_t**) Audio2DHistogram_GetArray(Histogram);

            for (uint64_t Channel = 0ULL; Channel < Histogram->NumChannels; Channel++) {
                for (uint64_t Sample = 0ULL; Sample < Histogram->NumEntries; Sample++) {
                    Samples[Channel][Sample] = NewValue;
                }
            }
            Verification = (Samples[0][0] & 0xFF);
        } else if (Histogram->Type == (AudioType_Signed | AudioType_Integer8)) {
            int8_t **Samples = (int8_t**) Audio2DHistogram_GetArray(Histogram);

            for (uint64_t Channel = 0ULL; Channel < Histogram->NumChannels; Channel++) {
                for (uint64_t Sample = 0ULL; Sample < Histogram->NumEntries; Sample++) {
                    Samples[Channel][Sample] = NewValue;
                }
            }
            Verification = (Samples[0][0] & 0xFF);
        } else if (Histogram->Type == (AudioType_Unsigned | AudioType_Integer16)) {
            uint16_t **Samples = (uint16_t**) Audio2DHistogram_GetArray(Histogram);

            for (uint64_t Channel = 0ULL; Channel < Histogram->NumChannels; Channel++) {
                for (uint64_t Sample = 0ULL; Sample < Histogram->NumEntries; Sample++) {
                    Samples[Channel][Sample] = NewValue;
                }
            }
            Verification = (Samples[0][0] & 0xFF);
        } else if (Histogram->Type == (AudioType_Signed | AudioType_Integer16)) {
            int16_t **Samples = (int16_t **) Audio2DHistogram_GetArray(Histogram);

            for (uint64_t Channel = 0ULL; Channel < Histogram->NumChannels; Channel++) {
                for (uint64_t Sample = 0ULL; Sample < Histogram->NumEntries; Sample++) {
                    Samples[Channel][Sample] = NewValue;
                }
            }
            Verification = (Samples[0][0] & 0xFF);
        } else if (Histogram->Type == (AudioType_Unsigned | AudioType_Integer32)) {
            uint32_t **Samples = (uint32_t**) Audio2DHistogram_GetArray(Histogram);

            for (uint64_t Channel = 0ULL; Channel < Histogram->NumChannels; Channel++) {
                for (uint64_t Sample = 0ULL; Sample < Histogram->NumEntries; Sample++) {
                    Samples[Channel][Sample] = NewValue;
                }
            }
            Verification = (Samples[0][0] & 0xFF);
        } else if (Histogram->Type == (AudioType_Signed | AudioType_Integer32)) {
            int32_t **Samples = (int32_t**) Audio2DHistogram_GetArray(Histogram);

            for (uint64_t Channel = 0ULL; Channel < Histogram->NumChannels; Channel++) {
                for (uint64_t Sample = 0ULL; Sample < Histogram->NumEntries; Sample++) {
                    Samples[Channel][Sample] = NewValue;
                }
            }
            Verification = (Samples[0][0] & 0xFF);
        }
        return Verification;
    }
    
    void Audio2DHistogram_Deinit(Audio2DHistogram *Histogram) {
        AssertIO(Histogram != NULL);
        free(Histogram->Array);
        free(Histogram);
    }
    /* Audio2DContainer */
    
    /* Audio3DContainer */
    typedef struct AudioVector {
        void                  *Samples;
        uint64_t              *Directions;
        uint64_t               SampleRate;
        uint64_t               NumSamples;
        uint64_t               NumDirections;
        uint64_t               DirectionOffset;
        MediaIO_AudioTypes     Type;
    } AudioVector;
    
    AudioVector *AudioVector_Init(void) {
        AudioVector *Vector = (AudioVector*) calloc(1, sizeof(AudioVector));
        AssertIO(Vector != NULL);
        return Vector;
    }
    
    void *AudioVector_GetArray(AudioVector *Vector) {
        AssertIO(Vector != NULL);
        return Vector->Samples;
    }
    
    void AudioVector_SetArray(AudioVector *Vector, void *Array) {
        AssertIO(Vector != NULL);
        AssertIO(Array != NULL);
        Vector->Samples = Array;
    }
    
    uint8_t AudioVector_Erase(AudioVector *Vector, uint8_t NewValue) {
        AssertIO(Vector != NULL);
        if ((Vector->Type & AudioType_Integer8) == AudioType_Integer8) {
            uint8_t *Samples = (uint8_t*) Vector->Samples;

            for (uint64_t Sample = 0ULL; Sample < Vector->NumSamples; Sample++) {
                Samples[Sample] = NewValue;
            }
        } else if ((Vector->Type & AudioType_Integer16) == AudioType_Integer16) {
            uint16_t *Samples = (uint16_t*) Vector->Samples;

            for (uint64_t Sample = 0ULL; Sample < Vector->NumSamples; Sample++) {
                Samples[Sample] = NewValue;
            }
        } else if ((Vector->Type & AudioType_Integer32) == AudioType_Integer32) {
            uint32_t *Samples = (uint32_t*) Vector->Samples;

            for (uint64_t Sample = 0ULL; Sample < Vector->NumSamples; Sample++) {
                Samples[Sample] = NewValue;
            }
        }

        for (uint64_t Direction = 0; Direction < Vector->NumDirections; Direction++) {
            Vector->Directions[Direction] = 0;
        }

        Vector->DirectionOffset = 0;
        Vector->NumDirections   = 0;
        Vector->NumSamples      = 0;
        Vector->SampleRate      = 0;
        Vector->Type            = AudioType_Unknown;
        return NewValue;
    }
    
    void AudioVector_Deinit(AudioVector *Vector) {
        AssertIO(Vector != NULL);
        free(Vector->Samples);
        free(Vector->Directions);
        free(Vector);
    }
    
    typedef struct AudioVectorHistogram {
        uint64_t          *Histogram;
        uint64_t           NumSamples;
        MediaIO_AudioTypes Type; // for the BitDepth, there will be 2^BitDepth values in the Histogram array
    } AudioVectorHistogram;
    
    AudioVectorHistogram *AudioVectorHistogram_Init(AudioVector *Vector) {
        AssertIO(Vector != NULL);
        AudioVectorHistogram *Histogram = calloc(1, sizeof(AudioVectorHistogram));
        AssertIO(Histogram != NULL);
        Histogram->Type             = Vector->Type;
        uint8_t  BitDepth           = AudioType_GetBitDepth(Vector->Type);
        uint64_t NumValues          = Exponentiate(2, BitDepth);

        Histogram->Histogram        = calloc(NumValues, BitDepth / 8);
        AssertIO(Histogram->Histogram != NULL);

        if ((Histogram->Type & AudioType_Integer8) == AudioType_Integer8) {
            Histogram->NumSamples   = Minimum(256, Vector->NumSamples);
            Histogram->Histogram    = (uint64_t*) calloc(Histogram->NumSamples, sizeof(uint64_t));
        } else if ((Histogram->Type & AudioType_Integer16) == AudioType_Integer16) {
            Histogram->NumSamples   = Minimum(65536, Vector->NumSamples);
            Histogram->Histogram    = (uint64_t*) calloc(Histogram->NumSamples, sizeof(uint64_t));
        } else if ((Histogram->Type & AudioType_Integer32) == AudioType_Integer32) {
            Histogram->NumSamples   = Vector->NumSamples;
            Histogram->Histogram    = (uint64_t*) calloc(Histogram->NumSamples, sizeof(uint64_t));
        }
        return Histogram;
    }
    
    void *AudioVectorHistogram_GetArray(AudioVectorHistogram *Histogram) {
        AssertIO(Histogram != NULL);
        return Histogram->Histogram;
    }
    
    void AudioVectorHistogram_SetArray(AudioVectorHistogram *Histogram, void *Array) {
        AssertIO(Histogram != NULL);
        AssertIO(Array != NULL);
        Histogram->Histogram = (uint64_t*) Array;
    }
    
    AudioVectorHistogram *AudioVectorHistogram_Generate(AudioVector *Vector) {
        AssertIO(Vector != NULL);
        AudioVectorHistogram *Histogram = AudioVectorHistogram_Init(Vector);
        AssertIO(Histogram != NULL);
        if ((Vector->Type & AudioType_Integer8) == AudioType_Integer8) {
            uint8_t *Samples = (uint8_t*) AudioVector_GetArray(Vector);

            for (uint64_t Sample = 0; Sample < Vector->NumSamples; Sample++) {
                uint64_t Value = (uint64_t) Samples[Sample];
                Histogram->Histogram[Value] += 1;
            }
        } else if ((Vector->Type & AudioType_Integer16) == AudioType_Integer16) {
            uint16_t *Samples = (uint16_t*) AudioVector_GetArray(Vector);

            for (uint64_t Sample = 0; Sample < Vector->NumSamples; Sample++) {
                uint64_t Value = (uint64_t) Samples[Sample];
                Histogram->Histogram[Value] += 1;
            }
        } else if ((Vector->Type & AudioType_Integer32) == AudioType_Integer32) {
            uint32_t *Samples = (uint32_t*) AudioVector_GetArray(Vector);

            for (uint64_t Sample = 0; Sample < Vector->NumSamples; Sample++) {
                uint64_t Value = (uint64_t) Samples[Sample];
                Histogram->Histogram[Value] += 1;
            }
        }
        return Histogram;
    }
    
    void AudioVectorHistogram_Sort(AudioVectorHistogram *Histogram, MediaIO_SortTypes Sort) {
        AssertIO(Histogram != NULL);
        AssertIO(Sort != SortType_Unknown);
        if (Sort == SortType_Descending) {
            qsort(Histogram->Histogram, Histogram->NumSamples, sizeof(uint64_t), Sort_Compare_Descending);
        } else if (Sort == SortType_Ascending) { // Bottom to Top
            qsort(Histogram->Histogram, Histogram->NumSamples, sizeof(uint64_t), Sort_Compare_Ascending);
        }
    }
    
    void AudioVectorHistogram_Erase(AudioVectorHistogram *Histogram, uint8_t NewValue) {
        AssertIO(Histogram != NULL);
        for (uint64_t Sample = 0; Sample < Histogram->NumSamples; Sample++) {
            Histogram->Histogram[Sample] = NewValue;
        }
    }
    
    void AudioVectorHistogram_Deinit(AudioVectorHistogram *Histogram) {
        AssertIO(Histogram != NULL);
        free(Histogram->Histogram);
        free(Histogram);
    }
    
    typedef struct Audio3DContainer {
        AudioVector **Vectors;
        uint64_t      NumVectors;
    } Audio3DContainer;
    
    Audio3DContainer *Audio3DContainer_Init(size_t NumVectors) {
        Audio3DContainer *Container = calloc(1, sizeof(Audio3DContainer));
        AssertIO(Container != NULL);
        Container->Vectors          = calloc(NumVectors, sizeof(AudioVector));
        AssertIO(Container->Vectors != NULL);
        Container->NumVectors       = NumVectors;
        return Container;
    }
    
    void Audio3DContainer_SetVector(Audio3DContainer *Container, AudioVector *Vector, uint64_t Index) {
        AssertIO(Container != NULL);
        AssertIO(Vector != NULL);
        AssertIO(Index < Container->NumVectors);
        Container->Vectors[Index] = Vector;
    }
    
    AudioVector *Audio3DContainer_GetVector(Audio3DContainer *Container, uint64_t Index) {
        AssertIO(Container != NULL);
        AssertIO(Index < Container->NumVectors);
        return Container->Vectors[Index];
    }
    
    uint64_t Audio3DContainer_GetTotalNumSamples(Audio3DContainer *Container) {
        AssertIO(Container != NULL);
        uint64_t NumSamples = 0ULL;
        for (uint64_t Vector = 0ULL; Vector < Container->NumVectors; Vector++) {
            NumSamples += Container->Vectors[Vector]->NumSamples;
        }
        return NumSamples;
    }
    
    Audio2DContainer *Audio3DContainer_Mix2Audio2DContainer(Audio3DContainer *Audio3D, AudioChannelMap *ChannelMap, MediaIO_AudioTypes Type, uint64_t SampleRate) {
        AssertIO(Audio3D != NULL);
        return Audio2DContainer_Init(Type, ChannelMap, SampleRate, Audio3DContainer_GetTotalNumSamples(Audio3D));
    }
    
    uint8_t Audio3DContainer_Erase(Audio3DContainer *Container, uint8_t NewValue) {
        AssertIO(Container != NULL);
        uint8_t Verification = 0xFE;
        for (uint64_t Vector = 0ULL; Vector < Container->NumVectors; Vector++) {
            for (uint64_t Direction = 0ULL;
                 Direction < Container->Vectors[Vector]->NumDirections; Direction++) {
                Container->Vectors[Vector]->Directions[Direction] = NewValue;
            }
            if ((Container->Vectors[Vector]->Type & AudioType_Integer8) == AudioType_Integer8) {
                for (uint64_t Sample = 0ULL; Sample < Container->Vectors[Vector]->NumSamples; Sample++) {
                    uint8_t *Samples = (uint8_t*) Container->Vectors[Vector]->Samples;
                    Samples[Sample] = NewValue;
                }
            } else if ((Container->Vectors[Vector]->Type & AudioType_Integer16) == AudioType_Integer16) {
                for (uint64_t Sample = 0ULL; Sample < Container->Vectors[Vector]->NumSamples; Sample++) {
                    uint16_t *Samples = (uint16_t*) Container->Vectors[Vector]->Samples;
                    Samples[Sample] = NewValue;
                }
            } else if ((Container->Vectors[Vector]->Type & AudioType_Integer32) == AudioType_Integer32) {
                for (uint64_t Sample = 0ULL; Sample < Container->Vectors[Vector]->NumSamples; Sample++) {
                    uint32_t *Samples = (uint32_t*) Container->Vectors[Vector]->Samples;
                    Samples[Sample] = NewValue;
                }
            }
        }
        return Verification;
    }
    
    void Audio3DContainer_Deinit(Audio3DContainer *Container) {
        AssertIO(Container != NULL);
        for (uint64_t Vector = 0ULL; Vector < Container->NumVectors; Vector++) {
            free(Container->Vectors[Vector]->Directions);
            free(Container->Vectors[Vector]->Samples);
        }
        free(Container->Vectors);
        free(Container);
    }
    /* Audio3DContainer */
    
    /* ImageContainer */
    typedef struct ImageChannelMap {
        MediaIO_ImageMask **Map;
        uint8_t             NumChannels;
        uint8_t             NumViews;
    } ImageChannelMap;
    
    ImageChannelMap *ImageChannelMap_Init(uint8_t NumViews, uint8_t NumChannels) {
        AssertIO(NumViews > 0);
        AssertIO(NumChannels > 0);
        ImageChannelMap *ChannelMap = calloc(1, sizeof(MediaIO_ImageMask));
        ChannelMap->NumViews    = NumViews;
        ChannelMap->NumChannels = NumChannels;
        ChannelMap->Map         = calloc(NumViews * NumChannels, sizeof(MediaIO_ImageMask**));
        return ChannelMap;
    }
    
    uint8_t ImageChannelMap_GetNumViews(ImageChannelMap *ChannelMap) {
        AssertIO(ChannelMap != NULL);
        return ChannelMap->NumViews;
    }
    
    uint8_t ImageChannelMap_GetNumChannels(ImageChannelMap *ChannelMap) {
        AssertIO(ChannelMap != NULL);
        return ChannelMap->NumChannels;
    }
    
    uint8_t ImageChannelMap_GetChannelsIndex(ImageChannelMap *ChannelMap, MediaIO_ImageMask Mask) {
        AssertIO(ChannelMap != NULL);
        AssertIO(Mask != ImageMask_Unknown);
        uint8_t Index = 0xFF;
        Index     = ChannelMap->NumChannels;
        for (uint8_t Channel = 0; Channel < ChannelMap->NumChannels; Channel++) {
            if (ChannelMap->Map[0][Channel] == Mask) {
                Index = Channel;
                break;
            }
        }
        return Index;
    }
    
    void ImageChannelMap_AddMask(ImageChannelMap *ChannelMap, uint8_t Index, MediaIO_ImageMask Mask) {
        AssertIO(ChannelMap != NULL);
        AssertIO(Index < ChannelMap->NumChannels);
        for (uint8_t View = 0; View < ChannelMap->NumViews; View++) {
            ChannelMap->Map[View][Index] = Mask;
        }
    }
    
    void ImageChannelMap_Deinit(ImageChannelMap *ChannelMap) {
        AssertIO(ChannelMap != NULL);
        free(ChannelMap->Map);
        free(ChannelMap);
    }
    
    typedef struct ImageContainer {
        // We need to add support for Planar formats as well.
        void               ****Pixels;
        ImageChannelMap       *ChannelMap;
        uint64_t               Width;
        uint64_t               Height;
        MediaIO_ImageTypes     Type;
    } ImageContainer;
    
    ImageContainer *ImageContainer_Init(MediaIO_ImageTypes Type, ImageChannelMap *ChannelMap, uint64_t Width, uint64_t Height) {
        AssertIO(Type != ImageType_Unknown);
        AssertIO(ChannelMap != NULL);
        AssertIO(Width > 0);
        AssertIO(Height > 0);
        ImageContainer *Image = calloc(1, sizeof(ImageContainer));
        AssertIO(Image != NULL);
        uint8_t NumViews      = ImageChannelMap_GetNumViews(ChannelMap);
        uint8_t NumChannels   = ImageChannelMap_GetNumChannels(ChannelMap);
        void *Array           = calloc(NumViews * NumChannels * Width * Height, Type);
        AssertIO(Array != NULL);
        Image->Pixels     = (void****) Array;
        Image->ChannelMap = ChannelMap;
        return Image;
    }
    
    uint64_t ImageContainer_GetWidth(ImageContainer *Image) {
        AssertIO(Image != NULL);
        return Image->Width;
    }
    
    uint64_t ImageContainer_GetHeight(ImageContainer *Image) {
        AssertIO(Image != NULL);
        return Image->Height;
    }
    
    ImageChannelMap *ImageContainer_GetChannelMap(ImageContainer *Image) {
        AssertIO(Image != NULL);
        return Image->ChannelMap;
    }
    
    void ImageContainer_SetChannelMap(ImageContainer *Image, ImageChannelMap *ChannelMap) {
        AssertIO(Image != NULL);
        AssertIO(ChannelMap != NULL);
        ImageChannelMap_Deinit(Image->ChannelMap);
        Image->ChannelMap = ChannelMap;
    }
    
    MediaIO_ImageTypes ImageContainer_GetType(ImageContainer *Image) {
        AssertIO(Image != NULL);
        return Image->Type;
    }

    uint8_t ImageContainer_GetBitDepth(ImageContainer *Image) {
        AssertIO(Image != NULL);
        return ImageType_GetBitDepth(Image->Type);
    }

    uint8_t ImageContainer_GetNumViews(ImageContainer *Image) {
        AssertIO(Image != NULL);
        return ImageChannelMap_GetNumViews(Image->ChannelMap);
    }
    
    void ****ImageContainer_GetArray(ImageContainer *Image) {
        AssertIO(Image != NULL);
        return Image->Pixels;
    }
    
    void ImageContainer_SetArray(ImageContainer *Image, void ****Array) {
        AssertIO(Image != NULL);
        AssertIO(Array != NULL);
        Image->Pixels = (void****) Array;
    }
    
    uint64_t ImageContainer_GetAverage(ImageContainer *Image, uint8_t View, uint8_t Channel) {
        AssertIO(Image != NULL);
        uint64_t Average = 0ULL;
        ImageChannelMap *Map         = ImageContainer_GetChannelMap(Image);
        AssertIO(Map != NULL);
        uint8_t          NumViews    = ImageChannelMap_GetNumViews(Map);
        AssertIO(View < NumViews);
        uint8_t          NumChannels = ImageChannelMap_GetNumChannels(Map);
        AssertIO(Channel < NumChannels);
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
        return Average;
    }
    
    uint64_t ImageContainer_GetMax(ImageContainer *Image, uint8_t View, uint8_t Channel) {
        AssertIO(Image != NULL);
        uint64_t Maximum = 0ULL;
        ImageChannelMap   *Map         = ImageContainer_GetChannelMap(Image);
        AssertIO(Map != NULL);
        MediaIO_ImageTypes Type        = ImageContainer_GetType(Image);
        uint8_t            NumViews    = ImageChannelMap_GetNumViews(Map);
        AssertIO(View < NumViews);
        uint8_t            NumChannels = ImageChannelMap_GetNumChannels(Map);
        AssertIO(Channel < NumChannels);
        if (Image->Type == ImageType_Integer8) {
            uint8_t ****Array  = (uint8_t****) ImageContainer_GetArray(Image);
            uint8_t Max        = Exponentiate(2, ImageType_GetBitDepth(Type));
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
            uint16_t ****Array  = (uint16_t****) ImageContainer_GetArray(Image);
            uint16_t Max        = Exponentiate(2, ImageType_GetBitDepth(Type));
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
        return Maximum;
    }
    
    uint64_t ImageContainer_GetMin(ImageContainer *Image, uint8_t View, uint8_t Channel) {
        AssertIO(Image != NULL);
        uint64_t Minimum = UINT64_MAX;
        ImageChannelMap *Map         = ImageContainer_GetChannelMap(Image);
        AssertIO(Map != NULL);
        uint8_t          NumViews    = ImageChannelMap_GetNumViews(Map);
        AssertIO(View < NumViews);
        uint8_t          NumChannels = ImageChannelMap_GetNumChannels(Map);
        AssertIO(Channel < NumChannels);
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
            uint16_t ****Array = (uint16_t****) ImageContainer_GetArray(Image);
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
        return Minimum;
    }
    
    void ImageContainer_Flip(ImageContainer *Image, MediaIO_FlipTypes FlipType) {
        AssertIO(Image != NULL);
        AssertIO(FlipType != FlipType_Unknown);
        ImageChannelMap *Map         = ImageContainer_GetChannelMap(Image);
        uint8_t          NumViews    = ImageChannelMap_GetNumViews(Map);
        uint8_t          NumChannels = ImageChannelMap_GetNumChannels(Map);
        if (FlipType == FlipType_Vertical || FlipType == FlipType_VerticalAndHorizontal) {
            if (Image->Type == ImageType_Integer8) {
                uint8_t *Array = (uint8_t*) ImageContainer_GetArray(Image);
                for (uint64_t View = 0ULL; View < NumViews; View++) {
                    for (uint64_t Width = 0ULL; Width < Image->Width; Width++) {
                        for (uint64_t TopLine = 0ULL; TopLine < Image->Height; TopLine++) {
                            for (uint64_t BottomLine = Image->Height; BottomLine > 0ULL; BottomLine--) {
                                for (uint64_t Channel = 0ULL; Channel < NumChannels; Channel++) {
                                    uint8_t TopPixel = Array[View * Width * TopLine * Channel];
                                    uint8_t BottomPixel = Array[View * Width * BottomLine * Channel];

                                    Array[View * Width * TopLine * Channel] = BottomPixel;
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
                                    uint16_t TopPixel = Array[View * Width * TopLine * Channel];
                                    uint16_t BottomPixel = Array[View * Width * BottomLine * Channel];

                                    Array[View * Width * TopLine * Channel] = BottomPixel;
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
                uint8_t *Array = (uint8_t*) ImageContainer_GetArray(Image);
                for (uint64_t View = 1ULL; View <= NumViews; View++) {
                    for (uint64_t Left = 0ULL; Left < Image->Width; Left++) {
                        for (uint64_t Right = Image->Width; Right > 0ULL; Right++) {
                            for (uint64_t Height = 0ULL; Height < Image->Height; Height++) {
                                for (uint64_t Channel = 0ULL; Channel < NumChannels; Channel++) {
                                    uint8_t LeftPixel = Array[View * Left * Height * Channel];
                                    uint8_t RightPixel = Array[View * Right * Height * Channel];

                                    Array[View * Left * Height * Channel] = RightPixel;
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
                                    uint16_t LeftPixel = Array[View * Left * Height * Channel];
                                    uint16_t RightPixel = Array[View * Right * Height * Channel];

                                    Array[View * Left * Height * Channel] = RightPixel;
                                    Array[View * Right * Height * Channel] = LeftPixel;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    void ImageContainer_Resize(ImageContainer *Image, int64_t Left, int64_t Right, int64_t Top, int64_t Bottom) {
        AssertIO(Image != NULL);
        ImageChannelMap *Map           = ImageContainer_GetChannelMap(Image);
        AssertIO(Map != NULL);
        uint8_t            NumViews    = ImageChannelMap_GetNumViews(Map);
        uint8_t            NumChannels = ImageChannelMap_GetNumChannels(Map);
        uint64_t           Height      = ImageContainer_GetHeight(Image);
        uint64_t           Width       = ImageContainer_GetWidth(Image);
        MediaIO_ImageTypes Type        = ImageContainer_GetType(Image);

        int64_t NewWidth  = 0;
        int64_t NewHeight = 0;

        int64_t LeftOffset   = AbsoluteI(Left);
        int64_t RightOffset  = Width + Right;
        int64_t TopOffset    = AbsoluteI(Top);
        int64_t BottomOffset = Height + Bottom;

        void ****New = (void****) calloc(NumViews * NewWidth * NewHeight * NumChannels, Type);
        AssertIO(New != NULL);
        if (Type == ImageType_Integer8) {
            uint8_t ****Array    = (uint8_t****) ImageContainer_GetArray(Image);
            uint8_t ****NewArray = (uint8_t****) New;
            AssertIO(Array != NULL);
            AssertIO(NewArray != NULL);
            for (uint8_t View = 0; View < NumViews; View++) {
                for (int64_t W = LeftOffset; W < RightOffset; W++) {
                    for (int64_t H = TopOffset; H < BottomOffset; H++) {
                        for (uint64_t Channel = 0ULL; Channel < NumChannels; Channel++) {
                            NewArray[View * (W - LeftOffset) * (H - TopOffset) * Channel] = Array[View * W * H * Channel];
                        }
                    }
                }
            }
            ImageContainer_SetArray(Image, (void ****) NewArray);
            free(Array);
        } else if (Type == ImageType_Integer16) {
            uint16_t ****Array = (uint16_t****) ImageContainer_GetArray(Image);
            uint16_t ****NewArray = (uint16_t****) New;
            AssertIO(Array != NULL);
            AssertIO(NewArray != NULL);
            for (uint8_t View = 1; View <= NumViews; View++) {
                for (int64_t W = LeftOffset; W < RightOffset; W++) {
                    for (int64_t H = TopOffset; H < BottomOffset; H++) {
                        for (uint64_t Channel = 1ULL; Channel <= NumChannels; Channel++) {
                            NewArray[View * (W - LeftOffset) * (H - TopOffset) * Channel] = Array[View * W * H * Channel];
                        }
                    }
                }
            }
            ImageContainer_SetArray(Image, (void****) NewArray);
            free(Array);
        }
    }
    
    ImageContainer *ImageContainer_Compare(ImageContainer *Reference, ImageContainer *Compare) {
        AssertIO(Reference != NULL);
        AssertIO(Compare != NULL);
        MediaIO_ImageTypes  RefType     = ImageContainer_GetType(Reference);
        ImageChannelMap    *RefMap      = ImageContainer_GetChannelMap(Reference);
        AssertIO(RefMap != NULL);
        uint8_t             RefNumChans = ImageChannelMap_GetNumChannels(RefMap);
        uint8_t             RefNumViews = ImageChannelMap_GetNumViews(RefMap);
        uint64_t            RefHeight   = ImageContainer_GetHeight(Reference);
        uint64_t            RefWidth    = ImageContainer_GetWidth(Reference);

        MediaIO_ImageTypes  ComType     = ImageContainer_GetType(Compare);
        ImageChannelMap    *ComMap      = ImageContainer_GetChannelMap(Compare);
        AssertIO(ComMap != NULL);
        uint8_t             ComNumChans = ImageChannelMap_GetNumChannels(ComMap);
        uint8_t             ComNumViews = ImageChannelMap_GetNumViews(ComMap);
        uint64_t            ComHeight   = ImageContainer_GetHeight(Compare);
        uint64_t            ComWidth    = ImageContainer_GetWidth(Compare);

        AssertIO(RefType == ComType);
        AssertIO(RefNumChans == ComNumChans);
        AssertIO(RefNumViews == ComNumViews);
        AssertIO(RefHeight == ComHeight);
        AssertIO(RefWidth == ComWidth);

        ImageContainer *Difference      = ImageContainer_Init(RefType, RefMap, RefWidth, RefHeight);
        AssertIO(Difference != NULL);
        if (RefType == ImageType_Integer8) {
            uint8_t ****RefArray = (uint8_t****) ImageContainer_GetArray(Reference);
            uint8_t ****ComArray = (uint8_t****) ImageContainer_GetArray(Compare);
            uint8_t ****DifArray = (uint8_t****) ImageContainer_GetArray(Difference);
            for (uint8_t View = 0; View < RefNumViews; View++) {
                for (uint64_t Width = 0ULL; Width < RefWidth; Width++) {
                    for (uint64_t Height = 0ULL; Height < RefHeight; Height++) {
                        for (uint8_t Channel = 0; Channel < RefNumChans; Channel++) {
                            DifArray[View][Width][Height][Channel] = (RefArray[View][Width][Height][Channel] - ComArray[View][Width][Height][Channel]) % 0xFF;
                        }
                    }
                }
            }
        } else if (RefType == ImageType_Integer16) {
            uint16_t ****RefArray = (uint16_t****) ImageContainer_GetArray(Reference);
            uint16_t ****ComArray = (uint16_t****) ImageContainer_GetArray(Compare);
            uint16_t ****DifArray = (uint16_t****) ImageContainer_GetArray(Difference);
            for (uint8_t View = 0; View < RefNumViews; View++) {
                for (uint64_t Width = 0ULL; Width < RefWidth; Width++) {
                    for (uint64_t Height = 0ULL; Height < RefHeight; Height++) {
                        for (uint8_t Channel = 0; Channel < RefNumChans; Channel++) {
                            DifArray[View][Width][Height][Channel] = (RefArray[View][Width][Height][Channel] - ComArray[View][Width][Height][Channel]) % 0xFFFF;
                        }
                    }
                }
            }
        }
        return Difference;
    }
    
    uint8_t ImageContainer_Erase(ImageContainer *Image, uint8_t NewValue) {
        AssertIO(Image != NULL);
        uint8_t Verification         = 0xFE;
        ImageChannelMap *Map         = ImageContainer_GetChannelMap(Image);
        AssertIO(Map != NULL);
        uint8_t          NumChannels = ImageChannelMap_GetNumChannels(Map);
        uint8_t          NumViews    = ImageChannelMap_GetNumViews(Map);

        if (Image->Type == ImageType_Integer8) {
            uint8_t ****Pixels = (uint8_t****) ImageContainer_GetArray(Image);

            for (uint64_t View = 0ULL; View < NumViews; View++) {
                for (uint64_t W = 0ULL; W < Image->Width; W++) {
                    for (uint64_t H = 0ULL; H < Image->Height; H++) {
                        for (uint64_t Channel = 0ULL; Channel < NumChannels; Channel++) {
                            Pixels[View][W][H][Channel] = NewValue;
                        }
                    }
                }
            }
            Verification = Pixels[0][0][0][0] & 0xFF;
        } else if (Image->Type == ImageType_Integer16) {
            uint16_t ****Pixels = (uint16_t****) ImageContainer_GetArray(Image);

            for (uint64_t View = 0ULL; View < NumViews; View++) {
                for (uint64_t W = 0ULL; W < Image->Width; W++) {
                    for (uint64_t H = 0ULL; H < Image->Height; H++) {
                        for (uint64_t Channel = 0ULL; Channel < NumChannels; Channel++) {
                            Pixels[View][W][H][Channel] = NewValue;
                        }
                    }
                }
            }
            Verification = Pixels[0][0][0][0] & 0xFFFF;
        }
        return Verification;
    }
    
    void ImageContainer_Deinit(ImageContainer *Image) {
        AssertIO(Image != NULL);
        free(Image->Pixels);
        free(Image->ChannelMap);
        free(Image);
    }
    
    typedef struct ImageHistogram {
        uint64_t           ***Array; // View, Channel, Sample
        ImageChannelMap      *ChannelMap;
        uint64_t              NumSamples; // aka 2^BitDepth
        MediaIO_ImageTypes    Type;
    } ImageHistogram;
    
    ImageHistogram *ImageHistogram_Init(ImageContainer *Image) {
        AssertIO(Image != NULL);
        ImageHistogram *Histogram = calloc(1, sizeof(ImageHistogram));
        AssertIO(Histogram != NULL);
        ImageChannelMap *Map                  = ImageContainer_GetChannelMap(Image);
        AssertIO(Map != NULL);
        uint8_t          NumViews             = ImageChannelMap_GetNumViews(Map);
        uint8_t          NumChannels          = ImageChannelMap_GetNumChannels(Map);
        uint8_t          BitDepth             = ImageType_GetBitDepth(Image->Type);
        uint64_t         NumPossibleColors    = Exponentiate(2, BitDepth);

        Histogram->ChannelMap                 = Map;
        Histogram->NumSamples                 = NumPossibleColors;

        uint64_t ***HistogramArray            = calloc(NumViews, sizeof(void*));
        AssertIO(HistogramArray != NULL);
        for (uint8_t View = 0; View < NumViews; View++) {
            HistogramArray[View]              = calloc(NumChannels, sizeof(void*));
            AssertIO(HistogramArray[View] != NULL);
            for (uint8_t Channel = 0; Channel < NumChannels; Channel++) {
                HistogramArray[View][Channel] = calloc(NumPossibleColors, BitDepth / 8);
                AssertIO(HistogramArray[View][Channel] != NULL);
            }
        }

        Histogram->Array                  = HistogramArray;
        Histogram->Type                   = Image->Type;
        return Histogram;
    }
    
    uint64_t ***ImageHistogram_GetArray(ImageHistogram *Histogram) {
        AssertIO(Histogram != NULL);
        return Histogram->Array;
    }
    
    void ImageHistogram_SetArray(ImageHistogram *Histogram, uint64_t ***Array) {
        AssertIO(Histogram != NULL);
        AssertIO(Array != NULL);
        Histogram->Array = Array;
    }

    FlatHistogram *ImageHistogram_Flatten(ImageHistogram *Histogram) {
        AssertIO(Histogram != NULL);
        FlatHistogram *Flat                         = FlatHistogram_Init();
        if ((Histogram->Type & ImageType_Integer8) == ImageType_Integer8) {
            uint8_t ***Array                    = (uint8_t***) Histogram->Array;
            for (uint8_t View = 0; View < ImageChannelMap_GetNumViews(Histogram->ChannelMap); View++) {
                for (uint8_t Channel = 0; Channel < ImageChannelMap_GetNumChannels(Histogram->ChannelMap); Channel++) {
                    for (uint64_t Sample = 0; Sample < Histogram->NumSamples; Sample++) {
                        uint8_t Value           = Array[View][Channel][Sample];
                        Flat->Histogram[Value] += 1;
                    }
                }
            }
        } else if ((Histogram->Type & ImageType_Integer16) == ImageType_Integer16) {
            uint16_t ***Array                    = (uint16_t***) Histogram->Array;
            for (uint8_t View = 0; View < ImageChannelMap_GetNumViews(Histogram->ChannelMap); View++) {
                for (uint8_t Channel = 0; Channel < ImageChannelMap_GetNumChannels(Histogram->ChannelMap); Channel++) {
                    for (uint64_t Sample = 0; Sample < Histogram->NumSamples; Sample++) {
                        uint8_t Value1           = (Array[View][Channel][Sample] & 0xFF00) >> 8;
                        uint8_t Value2           =  Array[View][Channel][Sample] & 0xFF;
                        Flat->Histogram[Value1] += 1;
                        Flat->Histogram[Value2] += 1;
                    }
                }
            }
        } else if ((Histogram->Type & ImageType_Integer32) == ImageType_Integer32) {
            uint32_t ***Array                        = (uint32_t***) Histogram->Array;
            for (uint8_t View = 0; View < ImageChannelMap_GetNumViews(Histogram->ChannelMap); View++) {
                for (uint8_t Channel = 0; Channel < ImageChannelMap_GetNumChannels(Histogram->ChannelMap); Channel++) {
                    for (uint64_t Sample = 0; Sample < Histogram->NumSamples; Sample++) {
                        uint8_t Value1           = (Array[View][Channel][Sample] & 0xFF000000) >> 24;
                        uint8_t Value2           = (Array[View][Channel][Sample] & 0xFF0000) >> 16;
                        uint8_t Value3           = (Array[View][Channel][Sample] & 0xFF00) >> 8;
                        uint8_t Value4           =  Array[View][Channel][Sample] & 0xFF;
                        Flat->Histogram[Value1] += 1;
                        Flat->Histogram[Value2] += 1;
                        Flat->Histogram[Value3] += 1;
                        Flat->Histogram[Value4] += 1;
                    }
                }
            }
        }
        return Flat;
    }
    
    ImageHistogram *ImageHistogram_Generate(ImageContainer *Image) {
        AssertIO(Image != NULL);
        ImageHistogram *Histogram    = ImageHistogram_Init(Image);
        AssertIO(Histogram != NULL);
        ImageChannelMap *ChannelMap  = ImageContainer_GetChannelMap(Image);
        AssertIO(ChannelMap != NULL);
        uint8_t          NumChannels = ChannelMap->NumChannels;
        uint8_t          NumViews    = ChannelMap->NumViews;
        uint64_t         Width       = ImageContainer_GetWidth(Image);
        uint64_t         Height      = ImageContainer_GetHeight(Image);

        if (Histogram->Type == ImageType_Integer8) {
            uint8_t ****ImageArray = (uint8_t****) Image->Pixels;
            uint8_t  ***HistArray  = (uint8_t***) Histogram->Array;

            for (uint64_t View = 0ULL; View < NumViews; View++) {
                for (uint64_t W = 0ULL; W < Width; W++) {
                    for (uint64_t H = 0ULL; H < Height; H++) {
                        for (uint64_t C = 0ULL; C < NumChannels; C++) {
                            uint8_t Sample = ImageArray[View][W][H][C];
                            HistArray[View][C][Sample] += 1;
                        }
                    }
                }
            }
        } else if (Histogram->Type == ImageType_Integer16) {
            uint16_t ****ImageArray = (uint16_t****) Image->Pixels;
            uint16_t *** HistArray  = (uint16_t***) Histogram->Array;

            for (uint64_t View = 0ULL; View < NumViews; View++) {
                for (uint64_t W = 0ULL; W < Width; W++) {
                    for (uint64_t H = 0ULL; H < Height; H++) {
                        for (uint64_t C = 0ULL; C < NumChannels; C++) {
                            uint16_t Sample = ImageArray[View][W][H][C];
                            HistArray[View][C][Sample] += 1;
                        }
                    }
                }
            }
        }
        return Histogram;
    }
    
    void ImageHistogram_Sort(ImageHistogram *Histogram, MediaIO_SortTypes Sort) {
        AssertIO(Histogram != NULL);
        ImageChannelMap *ChannelMap  = ImageHistogram_GetChannelMap(Histogram);
        if (Sort == SortType_Descending) {
            qsort(Histogram->Array, Histogram->NumSamples, sizeof(uint64_t), Sort_Compare_Descending);
        } else if (Sort == SortType_Ascending) {
            qsort(Histogram->Array, Histogram->NumSamples, sizeof(uint64_t), Sort_Compare_Ascending);
        }
    }
    
    uint8_t ImageHistogram_Erase(ImageHistogram *Histogram, uint8_t NewValue) {
        AssertIO(Histogram != NULL);
        uint8_t Verification = 0xFE;
        ImageChannelMap *ChannelMap  = ImageHistogram_GetChannelMap(Histogram);
        AssertIO(ChannelMap != NULL);
        uint8_t          NumChannels = ChannelMap->NumChannels;
        uint8_t          NumViews    = ChannelMap->NumViews;

        if (Histogram->Type == ImageType_Integer8) {
            uint8_t ***Pixels        = (uint8_t***) ImageHistogram_GetArray(Histogram);

            for (uint64_t View = 0ULL; View < NumViews; View++) {
                for (uint64_t Channel = 0ULL; Channel < NumChannels; Channel++) {
                    for (uint64_t Sample = 0ULL; Sample < Histogram->NumSamples; Sample++) {
                        Pixels[View][Channel][Sample] = NewValue;
                    }
                }
            }
            Verification = Pixels[0][0][0] & 0xFF;
        } else if (Histogram->Type == ImageType_Integer16) {
            uint16_t ***Pixels = (uint16_t***) ImageHistogram_GetArray(Histogram);

            for (uint64_t View = 0ULL; View < NumViews; View++) {
                for (uint64_t Channel = 0ULL; Channel < NumChannels; Channel++) {
                    for (uint64_t Sample = 0ULL; Sample < Histogram->NumSamples; Sample++) {
                        Pixels[View][Channel][Sample] = NewValue;
                    }
                }
            }
            Verification = Pixels[0][0][0] & 0xFFFF;
        }
        return Verification;
    }
    
    void ImageHistogram_Deinit(ImageHistogram *Histogram) {
        AssertIO(Histogram != NULL);
        free(Histogram->Array);
        free(Histogram);
    }
    
    ImageChannelMap *ImageHistogram_GetChannelMap(ImageHistogram *Histogram) {
        AssertIO(Histogram != NULL);
        ImageChannelMap *Map = Histogram->ChannelMap;
        return Map;
    }
    /* ImageContainer */
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
