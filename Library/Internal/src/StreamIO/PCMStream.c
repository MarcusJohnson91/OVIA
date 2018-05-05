#include <stdlib.h>

#include "../../Dependencies/FoundationIO/libFoundationIO/include/BitIO.h"
#include "../../Dependencies/FoundationIO/libFoundationIO/include/Log.h"
#include "../../Dependencies/FoundationIO/libFoundationIO/include/Math.h"

#include "../include/libPCM.h"
#include "../include/Private/libPCMTypes.h"
#include "../include/Private/Audio/WAVCommon.h"
#include "../include/Private/Audio/W64Common.h"
#include "../include/Private/Audio/AIFCommon.h"
#include "../include/Private/Image/BMPCommon.h"
#include "../include/Private/Image/PXMCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    struct Samples {
        void   **SampleArray;
        uint8_t  BitDepth;
        uint8_t  NumChannels;
    };
    
    Samples *InitSamplesBuffer(PCMFile *PCM, uint64_t NumSamples) {
        Samples *Data = calloc(1, sizeof(Samples));
        if (Data == NULL) {
            Log(Log_ERROR, __func__, U8("Not enough memory to init Samples"));
        } else {
            Data->BitDepth    = PCM->BitDepth;
            Data->NumChannels = PCM->NumChannels;
            Data->SampleArray = calloc(NumSamples * Data->NumChannels, Bits2Bytes(Data->BitDepth, Yes));
        }
        return Data;
    }
    
    void IFFSkipPadding(BitBuffer *BitB, uint32_t SubChunkSize) {
        if (IsOdd(SubChunkSize) == Yes) {
            BitBuffer_Skip(BitB, 8);
        }
    }
    
    PCMFile *PCMFile_Init(void) {
        PCMFile *PCM       = calloc(1, sizeof(PCMFile));
        if (PCM == NULL) {
            Log(Log_ERROR, __func__, U8("PCMFile Pointer is NULL"));
        } else {
            PCM->Aud       = calloc(1, sizeof(AUDHeader));
            PCM->Aud->Meta = calloc(1, sizeof(AUDMetadata));
            PCM->Pic       = calloc(1, sizeof(PICHeader));
        }
        return PCM;
    }
    
    void PCMFile_Identify(PCMFile *PCM, BitBuffer *BitB) {
        uint64_t FileMagic64 = PeekBits(MSByteFirst, LSBitFirst, BitB, 64);
        uint16_t FileMagic16 = FileMagic64 & 0xFFFF;
        uint32_t FileMagic32 = FileMagic64 & 0xFFFFFFFF;
        
        if (FileMagic16 == BMP_BM) {
            PCM->InputFileType = BMPFormat;
        } else if (FileMagic16 == PXM_PBMA || FileMagic16 == PXM_PBMB || FileMagic16 == PXM_PPMA || FileMagic16 == PXM_PPMB || FileMagic16 == PXM_PGMA || FileMagic16 == PXM_PGMB || FileMagic16 == PXM_PAMB) {
            PCM->InputFileType = PXMFormat;
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
        } else if (PCM->InputFileType == PXMFormat) {
            PXMParseMetadata(PCM, BitB);
        }
    }
    
    void PCM_SetOutputFileType(PCMFile *PCM, libPCMFileFormats OutputFileType) {
        if (PCM == NULL) {
            Log(Log_ERROR, __func__, U8("PCM Pointer is NULL"));
        } else {
            PCM->OutputFileType = OutputFileType;
        }
    }
    
    void PCM_SetOutputPXMType(PCMFile *PCM, PXMTypes PXMType) {
        if (PCM == NULL) {
            Log(Log_ERROR, __func__, U8("PCM Pointer is NULL"));
        } else {
            PCM->Pic->PXMType = PXMType;
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
        } else if (PCM->InputFileType == BMPFormat || PCM->InputFileType == PXMFormat) {
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
        if (PCM->InputFileType == PXMFormat) {
            ExtractedImage = PXMExtractImage(PCM, PixelArray);
        } else if (PCM->InputFileType == BMPFormat) {
            ExtractedImage = BMPExtractImage(PCM, PixelArray);
        }
        return ExtractedImage;
    }
    
    void PCM_InsertImage(PCMFile *PCM, BitBuffer *CreatedImage, uint16_t ***Image2Insert) {
        if (PCM->InputFileType == PXMFormat) {
            PXMInsertImage(PCM, CreatedImage, Image2Insert);
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
            W64InsertSamples(PCM, OutputSamples, NumSamples2Write, Samples2Write);
        }
    }
    
    void PCM_WriteHeader(PCMFile *PCM, BitBuffer *BitB) {
        if (PCM->OutputFileType == BMPFormat) {
            BMPWriteHeader(PCM, BitB, PCM->NumChannelAgnosticSamples);
        } else if (PCM->OutputFileType == PXMFormat) {
            PXMWriteHeader(PCM, BitB);
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
