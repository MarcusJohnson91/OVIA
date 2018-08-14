#include "../include/libOVIA.h"

#include "../include/Private/Audio/AIFCommon.h"
#include "../include/Private/Audio/FLACCommon.h"
#include "../include/Private/Audio/W64Common.h"
#include "../include/Private/Audio/WAVCommon.h"
#include "../include/Private/Image/BMPCommon.h"
#include "../include/Private/Image/PNGCommon.h"
#include "../include/Private/Image/PNMCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef struct OVIA {
        uint64_t         NumSamples;
        uint64_t         NumChannels;
        uint64_t         BitDepth;
        uint64_t         SampleRate;
        uint64_t         Width;
        uint64_t         Height;
        uint64_t         FileSize;
        uint64_t         SampleOffset;
        uint64_t         BlockSize;
        uint64_t         NumTags;
        UTF8           **Tags;
        OVIA_TagTypes   *TagTypes;
        
        OVIA_Types       Type;
        OVIA_FileFormats Format;
        bool             Is3D;
    } OVIA;
    
    void OVIA_SetNumChannels(OVIA *Ovia, uint64_t NumChannels) {
        if (Ovia != NULL) {
            Ovia->NumChannels = NumChannels;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint64_t OVIA_GetNumChannels(OVIA *Ovia) {
        uint64_t NumChannels = 0;
        if (Ovia != NULL) {
            NumChannels = Ovia->NumChannels;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return NumChannels;
    }
    
    void OVIA_SetNumSamples(OVIA *Ovia, uint64_t NumSamples) {
        if (Ovia != NULL) {
            Ovia->NumSamples = NumSamples;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint64_t OVIA_GetNumSamples(OVIA *Ovia) {
        uint64_t NumSamples = 0;
        if (Ovia != NULL) {
            NumSamples = Ovia->NumSamples;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return NumSamples;
    }
    
    void OVIA_SetBitDepth(OVIA *Ovia, uint64_t BitDepth) {
        if (Ovia != NULL) {
            Ovia->BitDepth = BitDepth;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint64_t OVIA_GetBitDepth(OVIA *Ovia) {
        uint64_t BitDepth = 0;
        if (Ovia != NULL) {
            BitDepth = Ovia->BitDepth;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return BitDepth;
    }
    
    void OVIA_SetSampleRate(OVIA *Ovia, uint64_t SampleRate) {
        if (Ovia != NULL) {
            Ovia->SampleRate = SampleRate;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint64_t OVIA_GetSampleRate(OVIA *Ovia) {
        uint64_t SampleRate = 0ULL;
        if (Ovia != NULL) {
            SampleRate = Ovia->SampleRate;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return SampleRate;
    }
    
    void OVIA_SetTag(OVIA *Ovia, OVIA_TagTypes TagType, UTF8 *Tag) {
        if (Ovia != NULL && Tag != NULL) {
            Ovia->NumTags += 1;
            if (Ovia->Tags != NULL) {
                Ovia->Tags                    = realloc(Ovia->Tags, Ovia->NumTags * sizeof(UTF8*));
            } else {
                Ovia->Tags                    = calloc(Ovia->NumTags, sizeof(UTF8*));
            }
            if (Ovia->TagTypes != NULL) {
                Ovia->TagTypes                = realloc(Ovia->TagTypes, Ovia->NumTags * sizeof(OVIA_TagTypes));
            } else {
                Ovia->Tags                    = calloc(Ovia->NumTags, sizeof(OVIA_TagTypes));
            }
            Ovia->Tags[Ovia->NumTags - 1]     = Tag;
            Ovia->TagTypes[Ovia->NumTags - 1] = TagType;
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (Tag == NULL) {
            Log(Log_ERROR, __func__, U8("Tag Pointer is NULL"));
        }
    }
    
    uint64_t OVIA_GetTagsIndex(OVIA *Ovia, OVIA_TagTypes TagType) { // 0xFFFFFFFFFFFFFFFF means not found
        uint64_t TagIndex = 0xFFFFFFFFFFFFFFFF;
        if (Ovia != NULL) {
            for (uint64_t Tag = 0ULL; Tag < Ovia->NumTags; Tag++) {
                if (Ovia->TagTypes[Tag] == TagType) {
                    TagIndex = Tag;
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return TagIndex;
    }
    
    UTF8 *OVIA_GetTag(OVIA *Ovia, uint64_t Tag) {
        UTF8 *TagString = NULL;
        if (Ovia != NULL && Tag < Ovia->NumTags) {
            TagString   = Ovia->Tags[Tag];
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return TagString;
    }
    
    void OVIA_SetFileSize(OVIA *Ovia, uint64_t FileSize) {
        if (Ovia != NULL) {
            Ovia->FileSize = FileSize;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint64_t OVIA_GetFileSize(OVIA *Ovia) {
        uint64_t FileSize = 0ULL;
        if (Ovia != NULL) {
            FileSize = Ovia->FileSize;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return FileSize;
    }
    
    void OVIA_SetSampleOffset(OVIA *Ovia, uint64_t SampleOffset) {
        if (Ovia != NULL) {
            Ovia->SampleOffset = SampleOffset;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint64_t OVIA_GetSampleOffset(OVIA *Ovia) {
        uint64_t SampleOffset = 0ULL;
        if (Ovia != NULL) {
            SampleOffset = Ovia->SampleOffset;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return SampleOffset;
    }
    
    void OVIA_SetBlockSize(OVIA *Ovia, uint64_t BlockSize) {
        if (Ovia != NULL) {
            Ovia->BlockSize = BlockSize;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint64_t OVIA_GetBlockSize(OVIA *Ovia) {
        uint64_t BlockSize = 0ULL;
        if (Ovia != NULL) {
            BlockSize = Ovia->BlockSize;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return BlockSize;
    }
    
    /*
     Ok, so we have a PNG file, the user calls OVIA_Identify.
     
     OVIA_Identify sets the OVIA struct saying it's a PNG file.
     
     From then on, all OVIA calls act with the PNG versions.
     
     The user calls OVIA_ReadMetadata.
     
     The user strips the black lines from the image.
     
     the user then writes the file back to the disk.
     
     OVIA just needs to manage the user accessible metadata.
     
     the individual format handlers will handle reading manipulating and writing the data...
     */
    
    void IFFSkipPadding(BitBuffer *BitB, uint32_t SubChunkSize) {
        if (IsOdd(SubChunkSize) == Yes) {
            BitBuffer_Skip(BitB, 8);
        }
    }
    
    OVIA *OVIA_Init(void) {
        OVIA *Ovia        = calloc(1, sizeof(OVIA));
        if (Ovia != NULL) {
            
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return Ovia;
    }
    
    void OVIA_Identify(OVIA *Ovia, BitBuffer *BitB) {
        uint64_t FileMagic64 = PeekBits(MSByteFirst, LSBitFirst, BitB, 64);
        uint32_t FileMagic32 = FileMagic64 & 0xFFFFFFFF;
        uint16_t FileMagic16 = FileMagic64 & 0xFFFF;
        
        if (FileMagic16 == BMP_BM) {
            Ovia->Type          = ImageType;
            Ovia->Format        = BMPFormat;
        } else if (FileMagic16 == PNM_PBMA ||
                   FileMagic16 == PNM_PBMB ||
                   FileMagic16 == PNM_PPMA ||
                   FileMagic16 == PNM_PPMB ||
                   FileMagic16 == PNM_PGMA ||
                   FileMagic16 == PNM_PGMB ||
                   FileMagic16 == PNM_PAMB) {
            Ovia->Type          = ImageType;
            Ovia->Format        = PNMFormat;
        } else if (FileMagic64 == PNGMagic) {
            Ovia->Type          = ImageType;
            Ovia->Format        = PNGFormat;
        } else if (FileMagic32 == AIF_FORM) {
            Ovia->Type          = AudioType;
            Ovia->Format        = AIFFormat;
        } else if (FileMagic32 == 0x52494646) {
            Ovia->Type          = AudioType;
            Ovia->Format        = WAVFormat;
        } else if (FileMagic32 == 0x72696666) {
            Ovia->Type          = AudioType;
            Ovia->Format        = W64Format;
        } else {
            Log(Log_ERROR, __func__, U8("Unrecognized file magic 0x%llu"), FileMagic64);
        }
    }
    
    void OVIA_ParseMetadata(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia->Format == AIFFormat) {
            AIFParseMetadata(Ovia, BitB);
        } else if (Ovia->Format == WAVFormat) {
            WAVParseMetadata(Ovia, BitB);
        } else if (Ovia->Format == W64Format) {
            W64ParseMetadata(Ovia, BitB);
        } else if (Ovia->Format == BMPFormat) {
            BMPParseMetadata(Ovia, BitB);
        } else if (Ovia->Format == PNMFormat) {
            PNMParseMetadata(Ovia, BitB);
        } else if (Ovia->Format == PNGFormat) {
            PNGParseMetadata(Ovia, BitB);
        } else if (Ovia->Format == FLACFormat) {
            FLACParseMetadata(Ovia, BitB);
        }
    }
    
    uint8_t OVIA_GetBitDepth(OVIA *Ovia) {
        uint8_t BitDepth = 0;
        if (Ovia->Format == AIFFormat || Ovia->Format == WAVFormat || Ovia->Format == W64Format) {
            BitDepth = (uint8_t) Ovia->BitDepth;
        } else if (Ovia->Format == BMPFormat || Ovia->Format == PNMFormat) {
            BitDepth = (uint8_t) Ovia->BitDepth;
        }
        return BitDepth;
    }
    
    uint64_t OVIA_GetNumChannels(OVIA *Ovia) {
        return Ovia->NumChannels;
    }
    
    uint64_t OVIA_GetNumSamples(OVIA *Ovia) {
        return Ovia->NumChannelAgnosticSamples;
    }
    
    void OVIA_ExtractSamples(OVIA *Ovia, BitBuffer *SampleArray, uint64_t NumSamples2Extract, uint32_t **ExtractedSamples) {
        if (Ovia->Format == AIFFormat) {
            AIFExtractSamples(Ovia, SampleArray, NumSamples2Extract, ExtractedSamples);
        } else if (Ovia->Format == WAVFormat) {
            WAVExtractSamples(Ovia, SampleArray, NumSamples2Extract, ExtractedSamples);
        } else if (Ovia->Format == W64Format) {
            W64ExtractSamples(Ovia, SampleArray, NumSamples2Extract, ExtractedSamples);
        }
    }
    
    ImageContainer *OVIA_ExtractImage(OVIA *Ovia, BitBuffer *BitB) {
        ImageContainer *Image = ImageContainer_Init(<#Image_Types Type#>, <#uint8_t BitDepth#>, <#uint8_t NumChannels#>, <#uint64_t Width#>, <#uint64_t Height#>)
        if (Ovia->Format == PNMFormat) {
            ExtractedImage = PNMExtractImage(Ovia, BitB);
        } else if (Ovia->Format == BMPFormat) {
            ExtractedImage = BMPExtractImage(Ovia, BitB);
        }
        return ExtractedImage;
    }
    
    void OVIA_InsertImage(OVIA *Ovia, BitBuffer *CreatedImage, uint16_t ***Image2Insert) {
        if (Ovia->Format == PNMFormat) {
            PNMInsertImage(Ovia, CreatedImage, Image2Insert);
        } else if (Ovia->Format == BMPFormat) {
            BMPInsertImage(Ovia, CreatedImage, Image2Insert);
        }
    }
    
    void OVIA_InsertSamples(OVIA *Ovia, BitBuffer *OutputSamples, uint32_t NumSamples2Write, uint32_t **Samples2Write) {
        if (Ovia->Format == AIFFormat) {
            AIFInsertSamples(Ovia, OutputSamples, NumSamples2Write, Samples2Write);
        } else if (Ovia->Format == WAVFormat) {
            WAVInsertSamples(Ovia, OutputSamples, NumSamples2Write, Samples2Write);
        } else if (Ovia->Format == W64Format) {
            W64AppendSamples(Ovia, OutputSamples, NumSamples2Write, Samples2Write);
        }
    }
    
    void OVIA_WriteHeader(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia->Format == BMPFormat) {
            BMPWriteHeader(Ovia, BitB);
        } else if (Ovia->Format == PNMFormat) {
            PNMWriteHeader(Ovia, BitB);
        } else if (Ovia->Format == AIFFormat) {
            AIFWriteHeader(Ovia, BitB);
        } else if (Ovia->Format == W64Format) {
            W64WriteHeader(Ovia, BitB);
        } else if (Ovia->Format == WAVFormat) {
            WAVWriteHeader(Ovia, BitB);
        }
    }
    
    void OVIA_Deinit(OVIA *Ovia) {
        free(Ovia->TagTypes);
        for (uint64_t Tag = 0ULL; Tag < Ovia->NumTags; Tag++) {
            free(Ovia->Tags[Tag]);
        }
        free(Ovia->Tags);
        free(Ovia);
    }
    
#ifdef __cplusplus
}
#endif
