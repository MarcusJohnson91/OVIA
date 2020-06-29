#include "../include/Private/MediaIO.h"                                      /* Included for our declarations */
#include "../../Dependencies/FoundationIO/Library/include/MathIO.h"          /* Included for Absolute, Max/Min */
#include "../../Dependencies/FoundationIO/Library/include/UnicodeIO/LogIO.h" /* Included for error reporting */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    typedef struct AudioChannelMap {
        MediaIO_AudioChannelMask  *Map;
        uint64_t                   NumChannels;
    } AudioChannelMap;
    
    AudioChannelMap *AudioChannelMap_Init(uint64_t NumChannels) {
        AudioChannelMap *ChannelMap = (AudioChannelMap*) calloc(1, sizeof(AudioChannelMap));
        if (ChannelMap != NULL) {
            ChannelMap->NumChannels = NumChannels;
            ChannelMap->Map         = (MediaIO_AudioChannelMask*) calloc(NumChannels, sizeof(MediaIO_AudioChannelMask));
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Couldn't allocate AudioChannelMap"));
        }
        return ChannelMap;
    }
    
    void AudioChannelMap_AddMask(AudioChannelMap *ChannelMap, uint64_t Index, MediaIO_AudioChannelMask Mask) {
        if (ChannelMap != NULL && Index < ChannelMap->NumChannels) {
            ChannelMap->Map[Index] = Mask;
        } else if (ChannelMap == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ChannelMap Pointer is NULL"));
        } else if (Index >= ChannelMap->NumChannels) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Index %llu is greather than there are channels %llu"), Index, ChannelMap->NumChannels);
        }
    }
    
    MediaIO_AudioChannelMask AudioChannelMap_GetMask(AudioChannelMap *ChannelMap, uint64_t Index) {
        MediaIO_AudioChannelMask ChannelMask = AudioMask_Unknown;
        if (ChannelMap != NULL && Index < ChannelMap->NumChannels) {
            ChannelMask = ChannelMap->Map[Index];
        } else if (ChannelMap == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ChannelMap Pointer is NULL"));
        } else if (Index >= ChannelMap->NumChannels) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Index %llu is greather than there are channels %llu"), Index, ChannelMap->NumChannels);
        }
        return ChannelMask;
    }
    
    uint64_t AudioChannelMap_GetLowestUnusedIndex(AudioChannelMap *ChannelMap) {
        uint64_t Index       = 0xFFFFFFFFFFFFFFFF;
        if (ChannelMap != NULL) {
            Index            = ChannelMap->NumChannels;
            uint64_t Channel = 0ULL;
            while (Channel < ChannelMap->NumChannels) {
                if (ChannelMap->Map[Channel] != AudioMask_Unknown) {
                    Index    = Channel;
                    break;
                }
                Channel     += 1;
            }
        } else if (ChannelMap == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ChannelMap Pointer is NULL"));
        }
        return Index;
    }
    
    void AudioChannelMap_Deinit(AudioChannelMap *ChannelMap) {
        if (ChannelMap != NULL) {
            free(ChannelMap->Map);
            free(ChannelMap);
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ChannelMap Pointer is NULL"));
        }
    }
    
    /* Audio2DContainer */
    typedef struct Audio2DContainer {
        void                 **Samples;
        AudioChannelMap       *ChannelMap;
        uint64_t               NumChannels;
        uint64_t               NumSamples;
        uint64_t               SampleRate;
        uint64_t               Offset;
        MediaIO_AudioTypes Type;
    } Audio2DContainer;
    
    Audio2DContainer *Audio2DContainer_Init(MediaIO_AudioTypes Type, AudioChannelMap *ChannelMap, uint64_t SampleRate, uint64_t NumSamples) {
        Audio2DContainer *Audio    = NULL;
        if (NumSamples > 0) {
            Audio                  = (Audio2DContainer*) calloc(1, sizeof(Audio2DContainer));
            if (Audio != NULL) {
                void **Array       = (void**) calloc(ChannelMap->NumChannels * NumSamples, Type / 4);
                if (Array != NULL) {
                    Audio->Samples = Array;
                } else {
                    Audio2DContainer_Deinit(Audio);
                    Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Couldn't allocate the audio array"));
                }
                
                Audio->SampleRate  = SampleRate;
                Audio->NumSamples  = NumSamples;
            } else {
                Audio2DContainer_Deinit(Audio);
                Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Couldn't allocate the Audio2DContainer"));
            }
        } else if (NumSamples == 0) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("NumSamples %llu is invalid"), NumSamples);
        }
        return Audio;
    }
    
    AudioChannelMap *Audio2DContainer_GetChannelMap(Audio2DContainer *Audio) {
        AudioChannelMap *Map = NULL;
        if (Audio != NULL) {
            Map              = Audio->ChannelMap;
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Audio2DContainer Pointer is NULL"));
        }
        return Map;
    }
    
    uint8_t Audio2DContainer_GetBitDepth(Audio2DContainer *Audio) {
        uint8_t BitDepth = 0;
        if (Audio != NULL) {
            if ((Audio->Type & AudioType_Integer8) == AudioType_Integer8) {
                BitDepth = 8;
            } else if ((Audio->Type & AudioType_Integer16) == AudioType_Integer16) {
                BitDepth = 16;
            } else if ((Audio->Type & AudioType_Integer32) == AudioType_Integer32) {
                BitDepth = 32;
            }
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Audio2DContainer Pointer is NULL"));
        }
        return BitDepth;
    }
    
    uint64_t Audio2DContainer_GetNumChannels(Audio2DContainer *Audio) {
        uint64_t NumChannels = 0ULL;
        if (Audio != NULL) {
            NumChannels = Audio->NumChannels;
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Audio2DContainer Pointer is NULL"));
        }
        return NumChannels;
    }
    
    uint64_t Audio2DContainer_GetSampleRate(Audio2DContainer *Audio) {
        uint64_t SampleRate = 0ULL;
        if (Audio != NULL) {
            SampleRate = Audio->SampleRate;
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Audio2DContainer Pointer is NULL"));
        }
        return SampleRate;
    }
    
    uint64_t Audio2DContainer_GetNumSamples(Audio2DContainer *Audio) {
        uint64_t NumSamples = 0ULL;
        if (Audio != NULL) {
            NumSamples = Audio->NumSamples;
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Audio2DContainer Pointer is NULL"));
        }
        return NumSamples;
    }
    
    MediaIO_AudioTypes Audio2DContainer_GetType(Audio2DContainer *Audio) {
        MediaIO_AudioTypes Type = AudioType_Unknown;
        if (Audio != NULL) {
            Type = Audio->Type;
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Audio2DContainer Pointer is NULL"));
        }
        return Type;
    }
    
    void **Audio2DContainer_GetArray(Audio2DContainer *Audio) {
        void **AudioArray = NULL;
        if (Audio != NULL) {
            AudioArray = Audio->Samples;
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Audio2DContainer Pointer is NULL"));
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
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Audio2DContainer Pointer is NULL"));
        }
        return Average;
    }
    
    int64_t Audio2DContainer_GetMax(Audio2DContainer *Audio, uint64_t Channel) {
        int64_t Maximum = INT64_MIN;
        if (Audio != NULL) {
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
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Audio2DContainer Pointer is NULL"));
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
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Audio2DContainer Pointer is NULL"));
        }
        return Minimum;
    }
    
    uint8_t Audio2DContainer_Erase(Audio2DContainer *Audio, uint8_t NewValue) {
        uint8_t Verification = 0xFE;
        if (Audio != NULL) {
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
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Audio2DContainer Pointer is NULL"));
        }
        return Verification;
    }
    
    void Audio2DContainer_Deinit(Audio2DContainer *Audio) {
        if (Audio != NULL) {
            free(Audio->Samples);
            free(Audio->ChannelMap);
            free(Audio);
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Audio2DContainer Pointer is NULL"));
        }
    }
    
    /*
     Audio ChannelMap stuff
     
     Since we're making Audio_ChannelMap a real thing, let's NULL terminate the array and drop the num channels variable from the init functions
     
     MediaIO_AudioChannelMask
     */
    
    typedef struct Audio2DHistogram {
        void                         *Array;
        MediaIO_AudioChannelMask *ChannelMap;
        uint64_t                      NumEntries;
        uint64_t                      NumChannels;
        MediaIO_AudioTypes        Type;
    } Audio2DHistogram;
    
    Audio2DHistogram *Audio2DHistogram_Init(Audio2DContainer *Audio) {
        Audio2DHistogram *Histogram  = NULL;
        if (Audio != NULL) {
            Histogram                = (Audio2DHistogram*) calloc(1, sizeof(Audio2DHistogram));
            if (Histogram != NULL) {
                uint8_t  NumChannels = Audio2DContainer_GetNumChannels(Audio);
                uint64_t NumValues   = Exponentiate(2, Audio2DContainer_GetBitDepth(Audio)) - 1;
                
                if ((Audio->Type & AudioType_Integer8) == AudioType_Integer8) {
                    Histogram->Array = (uint8_t*) calloc(NumValues * NumChannels, sizeof(uint8_t));
                } else if ((Audio->Type & AudioType_Integer16) == AudioType_Integer16) {
                    Histogram->Array = (uint16_t*) calloc(NumValues * NumChannels, sizeof(uint16_t));
                } else if ((Audio->Type & AudioType_Integer32) == AudioType_Integer32) {
                    Histogram->Array = (uint32_t*) calloc(NumValues * NumChannels, sizeof(uint32_t));
                }
                
                if (Histogram->Array != NULL) {
                    Histogram->Type       = Audio->Type;
                    Histogram->NumEntries = NumValues;
                } else {
                    Audio2DHistogram_Deinit(Histogram);
                    Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Couldn't allocate Audio2DHistogram Array"));
                }
            } else {
                Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Couldn't allocate Audio2DHistogram"));
            }
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Audio2DContainer Pointer is NULL"));
        }
        return Histogram;
    }
    
    void *Audio2DHistogram_GetArray(Audio2DHistogram *Histogram) {
        void *Array = NULL;
        if (Histogram != NULL) {
            Array = Histogram->Array;
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Audio2DHistogram Pointer is NULL"));
        }
        return Array;
    }
    
    void Audio2DHistogram_SetArray(Audio2DHistogram *Histogram, void *Array) {
        if (Histogram != NULL) {
            Histogram->Array = (void*) Array;
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Audio2DHistogram Pointer is NULL"));
        }
    }
    
    Audio2DHistogram *Audio2DHistogram_Generate(Audio2DContainer *Audio) {
        Audio2DHistogram *Histogram = NULL;
        if (Audio != NULL) {
            Histogram = Audio2DHistogram_Init(Audio);
            if (Histogram != NULL) {
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
            } else {
                Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Couldn't allocate Audio2DHistogram"));
            }
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Audio2DContainer Pointer is NULL"));
        }
        return Histogram;
    }
    
    void Audio2DHistogram_Sort(Audio2DHistogram *Histogram, MediaIO_SortTypes Sort) {
        if (Histogram != NULL) {
            if (Sort == SortType_Ascending) { // Top to bottom
                if (Histogram->Type == (AudioType_Integer8 | AudioType_Unsigned)) {
                    uint8_t *Audio = (uint8_t*) Audio2DHistogram_GetArray(Histogram);
                    for (uint64_t Element = 1ULL; Element < Histogram->NumEntries; Element++) {
                        Audio[Element - 1] = (uint8_t) Maximum(Audio[Element - 1], Audio[Element]);
                    }
                } else if (Histogram->Type == (AudioType_Integer16 | AudioType_Unsigned)) {
                    uint16_t *Audio = (uint16_t*) Audio2DHistogram_GetArray(Histogram);
                    for (uint64_t Element = 1ULL; Element < Histogram->NumEntries; Element++) {
                        Audio[Element - 1] = (uint16_t) Maximum(Audio[Element - 1], Audio[Element]);
                    }
                } else if (Histogram->Type == (AudioType_Integer32 | AudioType_Unsigned)) {
                    uint32_t *Audio = (uint32_t*) Audio2DHistogram_GetArray(Histogram);
                    for (uint64_t Element = 1ULL; Element < Histogram->NumEntries; Element++) {
                        Audio[Element - 1] = (uint32_t) Maximum(Audio[Element - 1], Audio[Element]);
                    }
                } else if (Histogram->Type == (AudioType_Integer8 | AudioType_Signed)) {
                    int8_t *Audio = (int8_t*) Audio2DHistogram_GetArray(Histogram);
                    for (uint64_t Element = 1ULL; Element < Histogram->NumEntries; Element++) {
                        Audio[Element - 1] = (int8_t) Maximum(Audio[Element - 1], Audio[Element]);
                    }
                } else if (Histogram->Type == (AudioType_Integer16 | AudioType_Signed)) {
                    int16_t *Audio = (int16_t*) Audio2DHistogram_GetArray(Histogram);
                    for (uint64_t Element = 1ULL; Element < Histogram->NumEntries; Element++) {
                        Audio[Element - 1] = (int16_t) Maximum(Audio[Element - 1], Audio[Element]);
                    }
                } else if (Histogram->Type == (AudioType_Integer32 | AudioType_Signed)) {
                    int32_t *Audio = (int32_t*) Audio2DHistogram_GetArray(Histogram);
                    for (uint64_t Element = 1ULL; Element < Histogram->NumEntries; Element++) {
                        Audio[Element - 1] = (int32_t) Maximum(Audio[Element - 1], Audio[Element]);
                    }
                }
            } else if (Sort == SortType_Descending) {
                if (Histogram->Type == (AudioType_Integer8 | AudioType_Unsigned)) {
                    uint8_t *Audio = (uint8_t*) Audio2DHistogram_GetArray(Histogram);
                    for (uint64_t Element = 1ULL; Element < Histogram->NumEntries; Element++) {
                        Audio[Element - 1] = (uint8_t) Minimum(Audio[Element - 1], Audio[Element]);
                    }
                } else if (Histogram->Type == (AudioType_Integer16 | AudioType_Unsigned)) {
                    uint16_t *Audio = (uint16_t*) Audio2DHistogram_GetArray(Histogram);
                    for (uint64_t Element = 1ULL; Element < Histogram->NumEntries; Element++) {
                        Audio[Element - 1] = (uint16_t) Minimum(Audio[Element - 1], Audio[Element]);
                    }
                }
            } else if (Histogram->Type == (AudioType_Integer32 | AudioType_Unsigned)) {
                uint32_t *Audio = (uint32_t*) Audio2DHistogram_GetArray(Histogram);
                for (uint64_t Element = 1ULL; Element < Histogram->NumEntries; Element++) {
                    Audio[Element - 1] = (uint32_t) Minimum(Audio[Element - 1], Audio[Element]);
                }
            } else if (Histogram->Type == (AudioType_Integer8 | AudioType_Signed)) {
                int8_t *Audio = (int8_t*) Audio2DHistogram_GetArray(Histogram);
                for (uint64_t Element = 1ULL; Element < Histogram->NumEntries; Element++) {
                    Audio[Element - 1] = (int8_t) Minimum(Audio[Element - 1], Audio[Element]);
                }
            } else if (Histogram->Type == (AudioType_Integer16 | AudioType_Signed)) {
                int16_t *Audio = (int16_t*) Audio2DHistogram_GetArray(Histogram);
                for (uint64_t Element = 1ULL; Element < Histogram->NumEntries; Element++) {
                    Audio[Element - 1] = (int16_t) Minimum(Audio[Element - 1], Audio[Element]);
                }
            } else if (Histogram->Type == (AudioType_Integer32 | AudioType_Signed)) {
                int32_t *Audio = (int32_t*) Audio2DHistogram_GetArray(Histogram);
                for (uint64_t Element = 1ULL; Element < Histogram->NumEntries; Element++) {
                    Audio[Element - 1] = (int32_t) Minimum(Audio[Element - 1], Audio[Element]);
                }
            }
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageHistogram Pointer is NULL"));
        }
    }
    
    Audio2DContainer *Audio2DContainer_Upsample(Audio2DContainer *Container2Upsample, uint64_t SampleRate, uint8_t BitDepth) {
        Audio2DContainer *Upsammpled = NULL;
        if (Container2Upsample != NULL && SampleRate > 0 && BitDepth > 0) {
            
        } else if (Container2Upsample == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Container Pointer is NULL"));
        } else if (SampleRate == 0) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("SampleRate: 0 is invalid"));
        } else if (BitDepth == 0) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("BitDepth: 0 is invalid"));
        }
        return Upsammpled;
    }
    
    Audio2DContainer *Audio2DContainer_Mix(Audio2DContainer *Container1, Audio2DContainer *Container2) {
        Audio2DContainer *Mixed = NULL;
        if (Container1 != NULL && Container2 != NULL) {
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
        } else if (Container1 == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Container1 Pointer is NULL"));
        } else if (Container2 == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Container2 Pointer is NULL"));
        }
        return Mixed;
    }
    
    uint8_t Audio2DHistogram_Erase(Audio2DHistogram *Histogram, uint8_t NewValue) {
        uint8_t Verification = 0xFE;
        if (Histogram != NULL) {
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
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Audio2DHistogram Pointer is NULL"));
        }
        return Verification;
    }
    
    void Audio2DHistogram_Deinit(Audio2DHistogram *Histogram) {
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
        MediaIO_AudioTypes Type;
    } AudioVector;
    
    AudioVector *AudioVector_Init(void) {
        AudioVector *Vector = (AudioVector*) calloc(1, sizeof(AudioVector));
        return Vector;
    }
    
    void *AudioVector_GetArray(AudioVector *Vector) {
        void *Array = NULL;
        if (Vector != NULL) {
            Array = Vector->Samples;
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("AudioVector Pointer is NULL"));
        }
        return Array;
    }
    
    void AudioVector_SetArray(AudioVector *Vector, void *Array) {
        if (Vector != NULL && Array != NULL) {
            Vector->Samples = (void*) Array;
        } else if (Vector == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("AudioVector Pointer is NULL"));
        } else if (Array == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Array Pointer is NULL"));
        }
    }
    
    uint8_t AudioVector_Erase(AudioVector *Vector, uint8_t NewValue) {
        uint8_t Verification = 0xFE;
        if (Vector != NULL) {
            if ((Vector->Type & AudioType_Integer8) == AudioType_Integer8) {
                uint8_t *Samples = (uint8_t*) Vector->Samples;
                
                for (uint64_t Sample = 0ULL; Sample < Vector->NumSamples; Sample++) {
                    Samples[Sample] = NewValue;
                }
                Verification = (Samples[0] & 0xFF);
            } else if ((Vector->Type & AudioType_Integer16) == AudioType_Integer16) {
                uint16_t *Samples = (uint16_t*) Vector->Samples;
                
                for (uint64_t Sample = 0ULL; Sample < Vector->NumSamples; Sample++) {
                    Samples[Sample] = NewValue;
                }
                Verification = (Samples[0] & 0xFF);
            } else if ((Vector->Type & AudioType_Integer32) == AudioType_Integer32) {
                uint32_t *Samples = (uint32_t*) Vector->Samples;
                
                for (uint64_t Sample = 0ULL; Sample < Vector->NumSamples; Sample++) {
                    Samples[Sample] = NewValue;
                }
                Verification = (Samples[0] & 0xFF);
            }
            
            for (uint64_t Direction = 0; Direction < Vector->NumDirections; Direction++) {
                Vector->Directions[Direction] = 0;
            }
            
            Vector->DirectionOffset = 0;
            Vector->NumDirections   = 0;
            Vector->NumSamples      = 0;
            Vector->SampleRate      = 0;
            Vector->Type            = AudioType_Unknown;
        }
        return Verification;
    }
    
    void AudioVector_Deinit(AudioVector *Vector) {
        if (Vector != NULL) {
            free(Vector->Samples);
            free(Vector->Directions);
            free(Vector);
        }
    }
    
    typedef struct AudioVectorHistogram {
        uint64_t              *Histogram;
        uint64_t               NumSamples;
        MediaIO_AudioTypes Type; // for the BitDepth, there will be 2^BitDepth values in the Histogram array
    } AudioVectorHistogram;
    
    AudioVectorHistogram *AudioVectorHistogram_Init(AudioVector *Vector) {
        AudioVectorHistogram *Histogram = (AudioVectorHistogram*) calloc(1, sizeof(AudioVectorHistogram));
        if (Vector != NULL) {
            Histogram->Type = Vector->Type;
            
            if ((Histogram->Type & AudioType_Integer8) == AudioType_Integer8) {
                Histogram->NumSamples = Minimum(256, Vector->NumSamples);
                Histogram->Histogram  = (uint64_t*) calloc(Histogram->NumSamples, sizeof(uint64_t));
            } else if ((Histogram->Type & AudioType_Integer16) == AudioType_Integer16) {
                Histogram->NumSamples = Minimum(65536, Vector->NumSamples);
                Histogram->Histogram  = (uint64_t*) calloc(Histogram->NumSamples, sizeof(uint64_t));
            } else if ((Histogram->Type & AudioType_Integer32) == AudioType_Integer32) {
                Histogram->NumSamples = Vector->NumSamples;
                Histogram->Histogram  = (uint64_t*) calloc(Histogram->NumSamples, sizeof(uint64_t));
            }
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("AudioVector Pointer is NULL"));
        }
        return Histogram;
    }
    
    void *AudioVectorHistogram_GetArray(AudioVectorHistogram *Histogram) {
        void *Array = NULL;
        if (Histogram != NULL) {
            Array = Histogram->Histogram;
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("AudioVectorHistogram Pointer is NULL"));
        }
        return Array;
    }
    
    void AudioVectorHistogram_SetArray(AudioVectorHistogram *Histogram, void *Array) {
        if (Histogram != NULL && Array != NULL) {
            Histogram->Histogram = (uint64_t*) Array;
        } else if (Histogram == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("AudioVectorHistogram Pointer is NULL"));
        } else if (Array == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Array Pointer is NULL"));
        }
    }
    
    AudioVectorHistogram *AudioVectorHistogram_Generate(AudioVector *Vector) {
        AudioVectorHistogram *Histogram = NULL;
        if (Vector != NULL) {
            Histogram = AudioVectorHistogram_Init(Vector);
            
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
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("AudioVector Pointer is NULL"));
        }
        return Histogram;
    }
    
    void AudioVectorHistogram_Sort(AudioVectorHistogram *Histogram, MediaIO_SortTypes Sort) {
        if (Sort == SortType_Ascending) { // Top to bottom
            if (Histogram->Type == (AudioType_Integer8 | AudioType_Unsigned)) {
                uint8_t *Audio = (uint8_t*) AudioVectorHistogram_GetArray(Histogram);
                for (uint64_t Sample = 1ULL; Sample < Histogram->NumSamples; Sample++) {
                    Audio[Sample - 1] = (uint8_t) Maximum(Audio[Sample - 1], Audio[Sample]);
                }
            } else if (Histogram->Type == (AudioType_Integer16 | AudioType_Unsigned)) {
                uint16_t *Audio = (uint16_t*) AudioVectorHistogram_GetArray(Histogram);
                for (uint64_t Sample = 1ULL; Sample < Histogram->NumSamples; Sample++) {
                    Audio[Sample - 1] = (uint8_t) Maximum(Audio[Sample - 1], Audio[Sample]);
                }
            } else if (Histogram->Type == (AudioType_Integer32 | AudioType_Unsigned)) {
                uint32_t *Audio = (uint32_t*) AudioVectorHistogram_GetArray(Histogram);
                for (uint64_t Sample = 1ULL; Sample < Histogram->NumSamples; Sample++) {
                    Audio[Sample - 1] = (uint8_t) Maximum(Audio[Sample - 1], Audio[Sample]);
                }
            } else if (Histogram->Type == (AudioType_Integer8 | AudioType_Signed)) {
                int8_t *Audio = (int8_t*) AudioVectorHistogram_GetArray(Histogram);
                for (uint64_t Sample = 1ULL; Sample < Histogram->NumSamples; Sample++) {
                    Audio[Sample - 1] = (uint8_t) Maximum(Audio[Sample - 1], Audio[Sample]);
                }
            } else if (Histogram->Type == (AudioType_Integer16 | AudioType_Signed)) {
                int16_t *Audio = (int16_t*) AudioVectorHistogram_GetArray(Histogram);
                for (uint64_t Sample = 1ULL; Sample < Histogram->NumSamples; Sample++) {
                    Audio[Sample - 1] = (uint8_t) Maximum(Audio[Sample - 1], Audio[Sample]);
                }
            } else if (Histogram->Type == (AudioType_Integer32 | AudioType_Signed)) {
                int32_t *Audio = (int32_t*) AudioVectorHistogram_GetArray(Histogram);
                for (uint64_t Sample = 1ULL; Sample < Histogram->NumSamples; Sample++) {
                    Audio[Sample - 1] = (uint8_t) Maximum(Audio[Sample - 1], Audio[Sample]);
                }
            } else if (Sort == SortType_Descending) {
                if (Histogram->Type == (AudioType_Integer8 | AudioType_Unsigned)) {
                    uint8_t *Audio = (uint8_t*) AudioVectorHistogram_GetArray(Histogram);
                    for (uint64_t Sample = 1ULL; Sample < Histogram->NumSamples; Sample++) {
                        Audio[Sample - 1] = (uint8_t) Minimum(Audio[Sample - 1], Audio[Sample]);
                    }
                } else if (Histogram->Type == (AudioType_Integer16 | AudioType_Unsigned)) {
                    uint16_t *Audio = (uint16_t*) AudioVectorHistogram_GetArray(Histogram);
                    for (uint64_t Sample = 1ULL; Sample < Histogram->NumSamples; Sample++) {
                        Audio[Sample - 1] = (uint16_t) Minimum(Audio[Sample - 1], Audio[Sample]);
                    }
                } else if (Histogram->Type == (AudioType_Integer32 | AudioType_Unsigned)) {
                    uint32_t *Audio = (uint32_t*) AudioVectorHistogram_GetArray(Histogram);
                    for (uint64_t Sample = 1ULL; Sample < Histogram->NumSamples; Sample++) {
                        Audio[Sample - 1] = (uint32_t) Minimum(Audio[Sample - 1], Audio[Sample]);
                    }
                } else if (Histogram->Type == (AudioType_Integer8 | AudioType_Signed)) {
                    int8_t *Audio = (int8_t*) AudioVectorHistogram_GetArray(Histogram);
                    for (uint64_t Sample = 1ULL; Sample < Histogram->NumSamples; Sample++) {
                        Audio[Sample - 1] = (int8_t) Minimum(Audio[Sample - 1], Audio[Sample]);
                    }
                } else if (Histogram->Type == (AudioType_Integer16 | AudioType_Signed)) {
                    int16_t *Audio = (int16_t*) AudioVectorHistogram_GetArray(Histogram);
                    for (uint64_t Sample = 1ULL; Sample < Histogram->NumSamples; Sample++) {
                        Audio[Sample - 1] = (int16_t) Minimum(Audio[Sample - 1], Audio[Sample]);
                    }
                } else if (Histogram->Type == (AudioType_Integer32 | AudioType_Signed)) {
                    int32_t *Audio = (int32_t*) AudioVectorHistogram_GetArray(Histogram);
                    for (uint64_t Sample = 1ULL; Sample < Histogram->NumSamples; Sample++) {
                        Audio[Sample - 1] = (int32_t) Minimum(Audio[Sample - 1], Audio[Sample]);
                    }
                }
            }
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageHistogram Pointer is NULL"));
        }
    }
    
    uint8_t AudioVectorHistogram_Erase(AudioVectorHistogram *Histogram, uint8_t NewValue) {
        uint8_t Verification = 0xFE;
        if (Histogram != NULL) {
            for (uint64_t Sample = 0; Sample < Histogram->NumSamples; Sample++) {
                Histogram->Histogram[Sample] = NewValue;
            }
            Verification = (Histogram->Histogram[0] & 0xFF);
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("AudioVectorHistogram Pointer is NULL"));
        }
        return Verification;
    }
    
    void AudioVectorHistogram_Deinit(AudioVectorHistogram *Histogram) {
        if (Histogram != NULL) {
            free(Histogram->Histogram);
            free(Histogram);
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("AudioVectorHistogram Pointer is NULL"));
        }
    }
    
    typedef struct Audio3DContainer {
        AudioVector *Vectors;
        uint64_t     NumVectors;
    } Audio3DContainer;
    
    Audio3DContainer *Audio3DContainer_Init(void) {
        Audio3DContainer *Container = (Audio3DContainer*) calloc(1, sizeof(Audio3DContainer));
        return Container;
    }
    
    void Audio3DContainer_SetVector(Audio3DContainer *Container, AudioVector *Vector, uint64_t Index) {
        if (Container != NULL && Vector != NULL && Index < Container->NumVectors) {
            Container->Vectors[Index] = *Vector;
        } else if (Container == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Container Pointer is NULL"));
        } else if (Vector == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Vector Pointer is NULL"));
        } else if (Index >= Container->NumVectors) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Index %llu is larger than %llu"), Index, Container->NumVectors);
        }
    }
    
    AudioVector *Audio3DContainer_GetVector(Audio3DContainer *Container, uint64_t Index) {
        AudioVector *Vector = NULL;
        if (Container != NULL && Index < Container->NumVectors) {
            Vector          = &Container->Vectors[Index];
        } else if (Container == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Container Pointer is NULL"));
        } else if (Index >= Container->NumVectors) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Index %llu is larger than %llu"), Index, Container->NumVectors);
        }
        return Vector;
    }
    
    uint64_t Audio3DContainer_GetTotalNumSamples(Audio3DContainer *Container) {
        uint64_t NumSamples = 0ULL;
        if (Container != NULL) {
            for (uint64_t Vector = 0ULL; Vector < Container->NumVectors; Vector++) {
                NumSamples += Container->Vectors[Vector].NumSamples;
            }
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Audio3DContainer Pointer is NULL"));
        }
        return NumSamples;
    }
    
    Audio2DContainer *Audio3DContainer_Mix2Audio2DContainer(Audio3DContainer *Audio3D, AudioChannelMap *ChannelMap, MediaIO_AudioTypes Type, uint64_t SampleRate) {
        Audio2DContainer *Audio2D = NULL;
        if (Audio3D != NULL && ChannelMap != NULL) {
            uint64_t NumSamples   = Audio3DContainer_GetTotalNumSamples(Audio3D);
            Audio2D               = Audio2DContainer_Init(Type, ChannelMap, SampleRate, /*Calculate this from all the vectors */ 0);
        } else if (Audio3D == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Audio3DContainer Pointer is NULL"));
        } else if (ChannelMap == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("AudioChannelMap Pointer is NULL"));
        }
        return Audio2D;
    }
    
    uint8_t Audio3DContainer_Erase(Audio3DContainer *Container, uint8_t NewValue) {
        uint8_t Verification = 0xFE;
        if (Container != NULL) {
            for (uint64_t Vector = 0ULL; Vector < Container->NumVectors; Vector++) {
                for (uint64_t Direction = 0ULL;
                     Direction < Container->Vectors[Vector].NumDirections; Direction++) {
                    Container->Vectors[Vector].Directions[Direction] = NewValue;
                }
                if ((Container->Vectors[Vector].Type & AudioType_Integer8) == AudioType_Integer8) {
                    for (uint64_t Sample = 0ULL; Sample < Container->Vectors[Vector].NumSamples; Sample++) {
                        uint8_t *Samples = (uint8_t*) Container->Vectors[Vector].Samples;
                        Samples[Sample] = NewValue;
                    }
                } else if ((Container->Vectors[Vector].Type & AudioType_Integer16) == AudioType_Integer16) {
                    for (uint64_t Sample = 0ULL; Sample < Container->Vectors[Vector].NumSamples; Sample++) {
                        uint16_t *Samples = (uint16_t*) Container->Vectors[Vector].Samples;
                        Samples[Sample] = NewValue;
                    }
                } else if ((Container->Vectors[Vector].Type & AudioType_Integer32) == AudioType_Integer32) {
                    for (uint64_t Sample = 0ULL; Sample < Container->Vectors[Vector].NumSamples; Sample++) {
                        uint32_t *Samples = (uint32_t*) Container->Vectors[Vector].Samples;
                        Samples[Sample] = NewValue;
                    }
                }
            }
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Container Pointer is NULL"));
        }
        return Verification;
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
    /* Audio3DContainer */
    
    /* ImageContainer */
    typedef struct ImageChannelMap {
        MediaIO_ImageChannelMask **Map;
        uint8_t                        NumChannels;
        uint8_t                        NumViews;
    } ImageChannelMap;
    
    ImageChannelMap *ImageChannelMap_Init(uint8_t NumViews, uint8_t NumChannels) {
        ImageChannelMap *ChannelMap = (ImageChannelMap*) calloc(1, sizeof(MediaIO_ImageChannelMask));
        if (ChannelMap != NULL) {
            ChannelMap->NumViews    = NumViews;
            ChannelMap->NumChannels = NumChannels;
            ChannelMap->Map         = (MediaIO_ImageChannelMask**) calloc(NumViews * NumChannels, sizeof(MediaIO_ImageChannelMask**));
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Couldn't allocate ImageChannelMap"));
        }
        return ChannelMap;
    }
    
    uint8_t ImageChannelMap_GetNumViews(ImageChannelMap *ChannelMap) {
        uint8_t NumViews = 0;
        if (ChannelMap != NULL) {
            NumViews     = ChannelMap->NumViews;
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ChannelMap Pointer is NULL"));
        }
        return NumViews;
    }
    
    uint8_t ImageChannelMap_GetNumChannels(ImageChannelMap *ChannelMap) {
        uint8_t NumViews = 0;
        if (ChannelMap != NULL) {
            NumViews     = ChannelMap->NumChannels;
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ChannelMap Pointer is NULL"));
        }
        return NumViews;
    }
    
    uint8_t ImageChannelMap_GetChannelsIndex(ImageChannelMap *ChannelMap, MediaIO_ImageChannelMask Mask) {
        uint8_t Index = 0xFF;
        if (ChannelMap != NULL && Mask != ImageMask_Unknown) {
            Index     = ChannelMap->NumChannels;
            for (uint8_t Channel = 0; Channel < ChannelMap->NumChannels; Channel++) {
                if (ChannelMap->Map[0][Channel] == Mask) {
                    Index = Channel;
                    break;
                }
            }
        } else if (ChannelMap == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageChannelMap Pointer is NULL"));
        } else if (Mask == ImageMask_Unknown) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageMask_Unknown is invalid"));
        }
        return Index;
    }
    
    void ImageChannelMap_AddMask(ImageChannelMap *ChannelMap, uint8_t Index, MediaIO_ImageChannelMask Mask) {
        if (ChannelMap != NULL && Index < ChannelMap->NumChannels) {
            for (uint8_t View = 0; View < ChannelMap->NumViews; View++) {
                ChannelMap->Map[View][Index] = Mask;
            }
        } else if (ChannelMap == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageChannelMap Pointer is NULL"));
        } else if (Index >= ChannelMap->NumChannels) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Index: %hhu is Invalid"), Index);
        }
    }
    
    void ImageChannelMap_Deinit(ImageChannelMap *ChannelMap) {
        if (ChannelMap != NULL) {
            free(ChannelMap->Map);
            free(ChannelMap);
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ChannelMap Pointer is NULL"));
        }
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
        ImageContainer *Image         = NULL;
        if (Width > 0 && Height > 0) {
            Image                     = (ImageContainer*) calloc(1, sizeof(ImageContainer));
            if (Image != NULL) {
                uint8_t NumViews      = ImageChannelMap_GetNumViews(ChannelMap);
                uint8_t NumChannels   = ImageChannelMap_GetNumChannels(ChannelMap);
                void *Array           = (void*) calloc(NumViews * NumChannels * Width * Height, Type);
                if (Array != NULL) {
                    Image->Pixels     = (void****) Array;
                    Image->ChannelMap = ChannelMap;
                } else {
                    ImageContainer_Deinit(Image);
                    Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Couldn't allocate pixel array"));
                }
            } else {
                ImageContainer_Deinit(Image);
                Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Couldn't allocate the ImageContainer"));
            }
        } else if (Width == 0) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Width %llu is invalid"), Width);
        } else if (Height == 0) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Height %llu is invalid"), Height);
        }
        return Image;
    }
    
    uint64_t ImageContainer_GetWidth(ImageContainer *Image) {
        uint64_t Width = 0ULL;
        if (Image != NULL) {
            Width = Image->Width;
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        }
        return Width;
    }
    
    uint64_t ImageContainer_GetHeight(ImageContainer *Image) {
        uint64_t Height = 0ULL;
        if (Image != NULL) {
            Height = Image->Height;
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        }
        return Height;
    }
    
    uint8_t ImageContainer_GetBitDepth(ImageContainer *Image) {
        uint8_t BitDepth = 0ULL;
        if (Image != NULL) {
            if ((Image->Type & ImageType_Integer8) == ImageType_Integer8) {
                BitDepth = 8;
            } else if ((Image->Type & ImageType_Integer16) == ImageType_Integer16) {
                BitDepth = 16;
            }
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        }
        return BitDepth;
    }
    
    ImageChannelMap *ImageContainer_GetChannelMap(ImageContainer *Image) {
        ImageChannelMap *Map = NULL;
        if (Image != NULL) {
            Map = Image->ChannelMap;
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        }
        return Map;
    }
    
    void ImageContainer_SetChannelMap(ImageContainer *Image, ImageChannelMap *ChannelMap) {
        if (Image != NULL && ChannelMap != NULL) {
            free(Image->ChannelMap);
            Image->ChannelMap = ChannelMap;
        } else if (Image == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        } else if (ChannelMap == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ChannelMap Pointer is NULL"));
        }
    }
    
    MediaIO_ImageTypes ImageContainer_GetType(ImageContainer *Image) {
        MediaIO_ImageTypes Type = ImageType_Unknown;
        if (Image != NULL) {
            Type = Image->Type;
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        }
        return Type;
    }
    
    void ****ImageContainer_GetArray(ImageContainer *Image) {
        void ****ImageArray = NULL;
        if (Image != NULL) {
            ImageArray = Image->Pixels;
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        }
        return ImageArray;
    }
    
    void ImageContainer_SetArray(ImageContainer *Image, void ****Array) {
        if (Image != NULL) {
            Image->Pixels = (void****) Array;
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        }
    }
    
    uint64_t ImageContainer_GetAverage(ImageContainer *Image, uint8_t View, uint8_t Channel) {
        uint64_t Average = 0ULL;
        if (Image != NULL) {
            ImageChannelMap *Map         = ImageContainer_GetChannelMap(Image);
            uint8_t          NumViews    = ImageChannelMap_GetNumViews(Map);
            uint8_t          NumChannels = ImageChannelMap_GetNumChannels(Map);
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
                    Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Channel: %hhu is invalid"), Channel);
                }
            } else {
                Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("View: %hhu is invalid"), View);
            }
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        }
        return Average;
    }
    
    uint64_t ImageContainer_GetMax(ImageContainer *Image, uint8_t View, uint8_t Channel) {
        uint64_t Maximum = 0ULL;
        if (Image != NULL) {
            ImageChannelMap *Map         = ImageContainer_GetChannelMap(Image);
            uint8_t          NumViews    = ImageChannelMap_GetNumViews(Map);
            uint8_t          NumChannels = ImageChannelMap_GetNumChannels(Map);
            if (View < NumViews - 1) {
                if (Channel < NumChannels - 1) {
                    if (Image->Type == ImageType_Integer8) {
                        uint8_t ****Array = (uint8_t****) ImageContainer_GetArray(Image);
                        uint8_t Max = Exponentiate(2, ImageContainer_GetBitDepth(Image)) - 1;
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
                        uint16_t ****Array = (uint16_t****) ImageContainer_GetArray(Image);
                        uint16_t Max = Exponentiate(2, ImageContainer_GetBitDepth(Image)) - 1;
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
                    Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("You tried getting the max from a nonexistant channel"));
                }
            } else {
                Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("You tried getting the average from a nonexistant view %hhu"), View);
            }
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        }
        return Maximum;
    }
    
    uint64_t ImageContainer_GetMin(ImageContainer *Image, uint8_t View, uint8_t Channel) {
        uint64_t Minimum = UINT64_MAX;
        if (Image != NULL) {
            ImageChannelMap *Map         = ImageContainer_GetChannelMap(Image);
            uint8_t          NumViews    = ImageChannelMap_GetNumViews(Map);
            uint8_t          NumChannels = ImageChannelMap_GetNumChannels(Map);
            
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
                } else {
                    Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("You tried getting the max from a nonexistant channel"));
                }
            } else {
                Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("You tried getting the average from a nonexistant view %hhu"), View);
            }
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        }
        return Minimum;
    }
    
    void ImageContainer_Flip(ImageContainer *Image, MediaIO_FlipTypes FlipType) {
        if (Image != NULL) {
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
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        }
    }
    
    void ImageContainer_Resize(ImageContainer *Image, int64_t Left, int64_t Right, int64_t Top, int64_t Bottom) {
        if (Image != NULL) {
            ImageChannelMap *Map         = ImageContainer_GetChannelMap(Image);
            uint8_t          NumViews    = ImageChannelMap_GetNumViews(Map);
            uint8_t          NumChannels = ImageChannelMap_GetNumChannels(Map);
            uint64_t         Height      = ImageContainer_GetHeight(Image);
            uint64_t         Width       = ImageContainer_GetWidth(Image);
            MediaIO_ImageTypes      Type        = ImageContainer_GetType(Image);
            
            int64_t NewWidth  = 0;
            int64_t NewHeight = 0;
            
            int64_t LeftOffset   = AbsoluteI(Left);
            int64_t RightOffset  = Width + Right;
            int64_t TopOffset    = AbsoluteI(Top);
            int64_t BottomOffset = Height + Bottom;
            
            void ****New = (void****) calloc(NumViews * NewWidth * NewHeight * NumChannels, Type);
            if (New != NULL) {
                if (Type == ImageType_Integer8) {
                    uint8_t ****Array = (uint8_t****) ImageContainer_GetArray(Image);
                    uint8_t ****NewArray = (uint8_t****) New;
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
                        ImageContainer_SetArray(Image, (void ****) NewArray);
                        free(Array);
                    } else {
                        Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Couldn't allocate an array for the cropped image"));
                    }
                } else if (Type == ImageType_Integer16) {
                    uint16_t ****Array = (uint16_t****) ImageContainer_GetArray(Image);
                    uint16_t ****NewArray = (uint16_t****) New;
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
                        ImageContainer_SetArray(Image, (void****) NewArray);
                        free(Array);
                    } else {
                        Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Couldn't allocate an array for the cropped image"));
                    }
                }
            } else {
                Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Couldn't allocate resized array"));
            }
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        }
    }
    
    ImageContainer *ImageContainer_Compare(ImageContainer *Reference, ImageContainer *Compare) {
        ImageContainer *Difference       = NULL;
        if (Reference != NULL && Compare != NULL) {
            MediaIO_ImageTypes      RefType     = ImageContainer_GetType(Reference);
            MediaIO_ImageTypes      ComType     = ImageContainer_GetType(Compare);
            ImageChannelMap *RefMap      = ImageContainer_GetChannelMap(Reference);
            ImageChannelMap *ComMap      = ImageContainer_GetChannelMap(Compare);
            uint8_t          RefNumChans = ImageChannelMap_GetNumChannels(RefMap);
            uint8_t          ComNumChans = ImageChannelMap_GetNumChannels(ComMap);
            uint8_t          RefNumViews = ImageChannelMap_GetNumViews(RefMap);
            uint8_t          ComNumViews = ImageChannelMap_GetNumViews(ComMap);
            uint64_t         RefHeight   = ImageContainer_GetHeight(Reference);
            uint64_t         ComHeight   = ImageContainer_GetHeight(Compare);
            uint64_t         RefWidth    = ImageContainer_GetWidth(Reference);
            uint64_t         ComWidth    = ImageContainer_GetWidth(Compare);
            if (RefType == ComType && RefNumViews == ComNumViews && RefNumChans == ComNumChans && RefHeight == ComHeight && RefWidth == ComWidth) {
                Difference               = ImageContainer_Init(RefType, RefMap, RefWidth, RefHeight);
                if (Difference != NULL) {
                    if (RefType == ImageType_Integer8 && ComType == ImageType_Integer8) {
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
                    } else if (RefType == ImageType_Integer16 && ComType == ImageType_Integer16) {
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
                } else {
                    Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Couldn't Allocate Difference"));
                }
            } else if (RefType != ComType) {
                Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Reference Type %d does not match Compare Type %d"), RefType, ComType);
            } else if (RefNumViews != ComNumViews) {
                Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Reference NumViews %d does not match Compare NumViews %d"), RefNumViews, ComNumViews);
            } else if (RefNumChans != ComNumChans) {
                Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Reference NumChannels %d does not match Compare NumChannels %d"), RefNumChans, ComNumChans);
            } else if (RefHeight != ComHeight) {
                Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Reference Height %lld does not match Compare Height %lld"), RefHeight, ComHeight);
            } else if (RefWidth != ComWidth) {
                Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Reference Width %lld does not match Compare Width %lld"), RefWidth, ComWidth);
            }
        } else if (Reference == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Reference Pointer is NULL"));
        } else if (Compare == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Compare Pointer is NULL"));
        }
        return Difference;
    }
    
    uint8_t ImageContainer_Erase(ImageContainer *Image, uint8_t NewValue) {
        uint8_t Verification = 0xFE;
        if (Image != NULL) {
            ImageChannelMap *Map         = ImageContainer_GetChannelMap(Image);
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
                Verification = Pixels[0][0][0][0] & 0xFF;
            }
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        }
        return Verification;
    }
    
    void ImageContainer_Deinit(ImageContainer *Image) {
        if (Image != NULL) {
            free(Image->Pixels);
            free(Image->ChannelMap);
            free(Image);
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        }
    }
    
    typedef struct ImageHistogram {
        void                ***Array;
        ImageChannelMap       *ChannelMap;
        uint64_t               Width;
        uint64_t               Height;
        MediaIO_ImageTypes     Type;
    } ImageHistogram;
    
    ImageHistogram *ImageHistogram_Init(ImageContainer *Image) {
        ImageHistogram *Histogram = NULL;
        if (Image != NULL) {
            Histogram                              = (ImageHistogram*) calloc(1, sizeof(ImageHistogram));
            if (Histogram != NULL) {
                ImageChannelMap *Map               = ImageContainer_GetChannelMap(Image);
                uint8_t          NumViews          = ImageChannelMap_GetNumViews(Map);
                uint8_t          NumChannels       = ImageChannelMap_GetNumChannels(Map);
                uint64_t         NumPossibleColors = Exponentiate(2, ImageContainer_GetBitDepth(Image)) - 1;
                
                Histogram->ChannelMap  = Map;
                Histogram->Height      = Image->Height;
                Histogram->Width       = Image->Width;
                
                if (Image->Type == ImageType_Integer8) {
                    uint8_t ***HistogramArray = (uint8_t***) calloc(NumViews * NumChannels * NumPossibleColors, sizeof(uint8_t));
                    if (HistogramArray != NULL) {
                        Histogram->Array = (void ***) HistogramArray;
                        Histogram->Type  = Image->Type;
                    } else {
                        ImageHistogram_Deinit(Histogram);
                        Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Couldn't allocate Histogram array"));
                    }
                } else if (Image->Type == ImageType_Integer16) {
                    uint16_t ***HistogramArray = (uint16_t***) calloc(NumViews * NumChannels * NumPossibleColors, sizeof(uint16_t));
                    if (HistogramArray != NULL) {
                        Histogram->Array = (void ***) HistogramArray;
                        Histogram->Type  = Image->Type;
                    } else {
                        ImageHistogram_Deinit(Histogram);
                        Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Couldn't allocate Histogram array"));
                    }
                }
            } else {
                ImageHistogram_Deinit(Histogram);
                Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Couldn't allocate ImageHistogram"));
            }
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
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
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageHistogram Pointer is NULL"));
        }
        return Array;
    }
    
    void ImageHistogram_SetArray(ImageHistogram *Histogram, void ***Array) {
        if (Histogram != NULL && Array != NULL) {
            Histogram->Array = (void***) Array;
        } else if (Histogram == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageHistogram Pointer is NULL"));
        } else if (Array == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Array Pointer is NULL"));
        }
    }
    
    ImageHistogram *ImageHistogram_Generate(ImageContainer *Image) {
        ImageHistogram *Histogram            = NULL;
        if (Image != NULL) {
            Histogram                        = ImageHistogram_Init(Image);
            if (Histogram != NULL) {
                ImageChannelMap *ChannelMap  = ImageContainer_GetChannelMap(Image);
                uint8_t          NumChannels = ChannelMap->NumChannels;
                uint8_t          NumViews    = ChannelMap->NumViews;
                uint64_t         Width       = ImageContainer_GetWidth(Image);
                uint64_t         Height      = ImageContainer_GetHeight(Image);
                
                if (Histogram->Type == ImageType_Integer8) {
                    uint8_t ****ImageArray = (uint8_t****) Image->Pixels;
                    uint8_t *** HistArray  = (uint8_t***) Histogram->Array;
                    
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
            } else {
                Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Couldn't allocate ImageHistogram"));
            }
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageContainer Pointer is NULL"));
        }
        return Histogram;
    }
    
    void ImageHistogram_Sort(ImageHistogram *Histogram, MediaIO_SortTypes Sort) {
        if (Histogram != NULL) {
            ImageChannelMap *ChannelMap  = ImageHistogram_GetChannelMap(Histogram);
            uint8_t          NumChannels = ChannelMap->NumChannels;
            uint8_t          NumViews    = ChannelMap->NumViews;
            if (Sort == SortType_Ascending) { // Top to bottom
                if (Histogram->Type == ImageType_Integer8) {
                    uint8_t ***Image = (uint8_t***) ImageHistogram_GetArray(Histogram);
                    for (uint64_t View = 0ULL; View < NumViews; View++) {
                        for (uint64_t Width = 0ULL; Width < Histogram->Width; Width++) {
                            for (uint64_t Height = 0ULL; Height < Histogram->Height; Height++) {
                                for (uint64_t Channel = 1ULL; Channel < NumChannels; Channel++) {
                                    uint8_t PreviousChannel = *Image[View][Channel - 1];
                                    uint8_t CurrentChannel = *Image[View][Channel];
                                    *Image[View][Channel] = (uint8_t) Minimum(PreviousChannel, CurrentChannel);
                                }
                            }
                        }
                    }
                } else if (Histogram->Type == ImageType_Integer16) {
                    uint16_t ***Image = (uint16_t***) ImageHistogram_GetArray(Histogram);
                    for (uint64_t View = 0ULL; View < NumViews; View++) {
                        for (uint64_t Width = 0ULL; Width < Histogram->Width; Width++) {
                            for (uint64_t Height = 0ULL; Height < Histogram->Height; Height++) {
                                for (uint64_t Channel = 1ULL; Channel < NumChannels; Channel++) {
                                    uint16_t PreviousChannel = *Image[View][Channel - 1];
                                    uint16_t CurrentChannel = *Image[View][Channel];
                                    *Image[View][Channel] = (uint16_t) Minimum(PreviousChannel, CurrentChannel);
                                }
                            }
                        }
                    }
                }
            } else if (Sort == SortType_Descending) { // Bottom to top
                if (Histogram->Type == ImageType_Integer8) {
                    uint8_t ***Image = (uint8_t***) ImageHistogram_GetArray(Histogram);
                    for (uint64_t View = 0ULL; View < NumViews; View++) {
                        for (uint64_t Width = 0ULL; Width < Histogram->Width; Width++) {
                            for (uint64_t Height = 0ULL; Height < Histogram->Height; Height++) {
                                for (uint64_t Channel = 1ULL; Channel < NumChannels; Channel++) {
                                    uint8_t PreviousChannel = *Image[View][Channel - 1];
                                    uint8_t CurrentChannel = *Image[View][Channel];
                                    *Image[View][Channel] = (uint8_t) Minimum(PreviousChannel, CurrentChannel);
                                }
                            }
                        }
                    }
                } else if (Histogram->Type == ImageType_Integer16) {
                    uint16_t ***Image = (uint16_t***) ImageHistogram_GetArray(Histogram);
                    for (uint64_t View = 0ULL; View < NumViews; View++) {
                        for (uint64_t Width = 0ULL; Width < Histogram->Width; Width++) {
                            for (uint64_t Height = 0ULL; Height < Histogram->Height; Height++) {
                                for (uint64_t Channel = 1ULL; Channel < NumChannels; Channel++) {
                                    uint16_t PreviousChannel = *Image[View][Channel - 1];
                                    uint16_t CurrentChannel = *Image[View][Channel];
                                    *Image[View][Channel] = (uint16_t) Minimum(PreviousChannel, CurrentChannel);
                                }
                            }
                        }
                    }
                }
            }
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageHistogram Pointer is NULL"));
        }
    }
    
    uint8_t ImageHistogram_Erase(ImageHistogram *Histogram, uint8_t NewValue) {
        uint8_t Verification = 0xFE;
        if (Histogram != NULL) {
            ImageChannelMap *ChannelMap  = ImageHistogram_GetChannelMap(Histogram);
            uint8_t          NumChannels = ChannelMap->NumChannels;
            uint8_t          NumViews    = ChannelMap->NumViews;
            
            if (Histogram->Type == ImageType_Integer8) {
                uint8_t ****Pixels = (uint8_t****) ImageHistogram_GetArray(Histogram);
                
                for (uint64_t View = 0ULL; View < NumViews; View++) {
                    for (uint64_t W = 0ULL; W < Histogram->Width; W++) {
                        for (uint64_t H = 0ULL; H < Histogram->Height; H++) {
                            for (uint64_t Channel = 0ULL; Channel < NumChannels; Channel++) {
                                Pixels[View][W][H][Channel] = NewValue;
                            }
                        }
                    }
                }
                Verification = Pixels[0][0][0][0] & 0xFF;
            } else if (Histogram->Type == ImageType_Integer16) {
                uint16_t ****Pixels = (uint16_t****) ImageHistogram_GetArray(Histogram);
                
                for (uint64_t View = 0ULL; View < NumViews; View++) {
                    for (uint64_t W = 0ULL; W < Histogram->Width; W++) {
                        for (uint64_t H = 0ULL; H < Histogram->Height; H++) {
                            for (uint64_t Channel = 0ULL; Channel < NumChannels; Channel++) {
                                Pixels[View][W][H][Channel] = NewValue;
                            }
                        }
                    }
                }
                Verification = Pixels[0][0][0][0] & 0xFF;
            }
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageHistogram Pointer is NULL"));
        }
        return Verification;
    }
    
    void ImageHistogram_Deinit(ImageHistogram *Histogram) {
        if (Histogram != NULL) {
            free(Histogram->Array);
            free(Histogram);
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageHistogram Pointer is NULL"));
        }
    }
    
    ImageChannelMap *ImageHistogram_GetChannelMap(ImageHistogram *Histogram) {
        ImageChannelMap *Map = NULL;
        if (Histogram != NULL) {
            Map              = Histogram->ChannelMap;
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("ImageHistogram Pointer is NULL"));
        }
        return Map;
    }
    /* ImageContainer */
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
