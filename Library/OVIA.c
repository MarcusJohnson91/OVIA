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
        OVIA_Types       Type;
        OVIA_FileFormats Format;
        
    } OVIA;
    
    void IFFSkipPadding(BitBuffer *BitB, uint32_t SubChunkSize) {
        if (IsOdd(SubChunkSize) == Yes) {
            BitBuffer_Skip(BitB, 8);
        }
    }
    
    OVIA *OVIA_Init(void) {
        OVIA *Ovia        = calloc(1, sizeof(OVIA));
        if (Ovia != NULL) {
            
        } else {
            Log(Log_ERROR, __func__, U8("PCMFile Pointer is NULL"));
        }
        return Ovia;
    }
    
    void PCMFile_Identify(PCMFile *PCM, BitBuffer *BitB) {
        uint64_t FileMagic64 = PeekBits(MSByteFirst, LSBitFirst, BitB, 64);
        uint16_t FileMagic16 = FileMagic64 & 0xFFFF;
        uint32_t FileMagic32 = FileMagic64 & 0xFFFFFFFF;
        
        if (FileMagic16 == BMP_BM) {
            PCM->InputFileType = BMPFormat;
        } else if (FileMagic16 == PNM_PBMA || FileMagic16 == PNM_PBMB || FileMagic16 == PNM_PPMA || FileMagic16 == PNM_PPMB || FileMagic16 == PNM_PGMA || FileMagic16 == PNM_PGMB || FileMagic16 == PNM_PAMB) {
            PCM->InputFileType = PNMFormat;
        } else if (FileMagic32 == AIF_FORM) {
            PCM->InputFileType = AIFFormat;
        } else if (FileMagic32 == WAV_RIFF) {
            PCM->InputFileType = WAVFormat;
        } else if (FileMagic32 == 0x72696666) {
            PCM->InputFileType = W64Format;
        } else {
            Log(Log_ERROR, __func__, U8("Unrecognized file magic 0x%llX"), FileMagic64);
        }
    }
    
    void PCMFile_ParseMetadata(PCMFile *PCM, BitBuffer *BitB) {
        if (PCM->InputFileType == AIFFormat) {
            AIFParseMetadata(PCM, BitB);
        } else if (PCM->InputFileType == WAVFormat) {
            WAVParseMetadata(PCM, BitB);
        } else if (PCM->InputFileType == W64Format) {
            W64ParseMetadata(PCM, BitB);
        } else if (PCM->InputFileType == BMPFormat) {
            BMPParseMetadata(PCM, BitB);
        } else if (PCM->InputFileType == PNMFormat) {
            PNMParseMetadata(PCM, BitB);
        }
    }
    
    void PCM_SetOutputFileType(PCMFile *PCM, OVIA_FileFormats OutputFileType) {
        if (PCM == NULL) {
            Log(Log_ERROR, __func__, U8("PCM Pointer is NULL"));
        } else {
            PCM->OutputFileType = OutputFileType;
        }
    }
    
    void PCM_SetOutputPNMType(PCMFile *PCM, PNMTypes PNMType) {
        if (PCM == NULL) {
            Log(Log_ERROR, __func__, U8("PCM Pointer is NULL"));
        } else {
            PCM->Pic->PNMType = PNMType;
        }
    }
    
    void PCM_SetNumOutputSamples(PCMFile *PCM, uint64_t NumChannelIndependentSamples) {
        if (PCM == NULL) {
            Log(Log_ERROR, __func__, U8("PCM Pointer is NULL"));
        } else {
            PCM->NumChannelAgnosticSamples = NumChannelIndependentSamples;
        }
    }
    
    uint8_t PCM_GetBitDepth(PCMFile *PCM) {
        uint8_t BitDepth = 0;
        if (PCM->InputFileType == AIFFormat || PCM->InputFileType == WAVFormat || PCM->InputFileType == W64Format) {
            BitDepth = (uint8_t) PCM->BitDepth;
        } else if (PCM->InputFileType == BMPFormat || PCM->InputFileType == PNMFormat) {
            BitDepth = (uint8_t) PCM->BitDepth;
        }
        return BitDepth;
    }
    
    uint64_t PCM_GetNumChannels(PCMFile *PCM) {
        return PCM->NumChannels;
    }
    
    uint64_t PCM_GetNumSamples(PCMFile *PCM) {
        return PCM->NumChannelAgnosticSamples;
    }
    
    void PCM_ExtractSamples(PCMFile *PCM, BitBuffer *SampleArray, uint64_t NumSamples2Extract, uint32_t **ExtractedSamples) {
        if (PCM->InputFileType == AIFFormat) {
            AIFExtractSamples(PCM, SampleArray, NumSamples2Extract, ExtractedSamples);
        } else if (PCM->InputFileType == WAVFormat) {
            WAVExtractSamples(PCM, SampleArray, NumSamples2Extract, ExtractedSamples);
        } else if (PCM->InputFileType == W64Format) {
            W64ExtractSamples(PCM, SampleArray, NumSamples2Extract, ExtractedSamples);
        }
    }
    
    uint16_t ***PCM_ExtractImage(PCMFile *PCM, BitBuffer *PixelArray) {
        uint16_t ***ExtractedImage = NULL;
        if (PCM->InputFileType == PNMFormat) {
            ExtractedImage = PNMExtractImage(PCM, PixelArray);
        } else if (PCM->InputFileType == BMPFormat) {
            ExtractedImage = BMPExtractImage(PCM, PixelArray);
        }
        return ExtractedImage;
    }
    
    void PCM_InsertImage(PCMFile *PCM, BitBuffer *CreatedImage, uint16_t ***Image2Insert) {
        if (PCM->InputFileType == PNMFormat) {
            PNMInsertImage(PCM, CreatedImage, Image2Insert);
        } else if (PCM->InputFileType == BMPFormat) {
            BMPInsertImage(PCM, CreatedImage, Image2Insert);
        }
    }
    
    void PCM_InsertSamples(PCMFile *PCM, BitBuffer *OutputSamples, uint32_t NumSamples2Write, uint32_t **Samples2Write) {
        if (PCM->OutputFileType == AIFFormat) {
            AIFInsertSamples(PCM, OutputSamples, NumSamples2Write, Samples2Write);
        } else if (PCM->OutputFileType == WAVFormat) {
            WAVInsertSamples(PCM, OutputSamples, NumSamples2Write, Samples2Write);
        } else if (PCM->OutputFileType == W64Format) {
            W64AppendSamples(PCM, OutputSamples, NumSamples2Write, Samples2Write);
        }
    }
    
    void PCM_WriteHeader(PCMFile *PCM, BitBuffer *BitB) {
        if (PCM->OutputFileType == BMPFormat) {
            BMPWriteHeader(PCM, BitB, PCM->NumChannelAgnosticSamples);
        } else if (PCM->OutputFileType == PNMFormat) {
            PNMWriteHeader(PCM, BitB);
        } else if (PCM->OutputFileType == AIFFormat) {
            AIFWriteHeader(PCM, BitB);
        } else if (PCM->OutputFileType == W64Format) {
            W64WriteHeader(PCM, BitB);
        } else if (PCM->OutputFileType == WAVFormat) {
            WAVWriteHeader(PCM, BitB);
        }
    }
    
    void PCMFile_Deinit(PCMFile *PCM) {
        free(PCM->Aud->Meta);
        free(PCM->Aud);
        free(PCM->Pic);
        free(PCM);
    }
    
#ifdef __cplusplus
}
#endif
