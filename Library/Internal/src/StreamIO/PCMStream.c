#include "../../Dependencies/BitIO/libBitIO/include/BitIO.h"

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
    
    PCMFile *PCMFile_Init(void) {
        PCMFile *PCM       = calloc(1, sizeof(PCMFile));
        if (PCM == NULL) {
            BitIOLog(LOG_ERROR, libPCMLibraryName, __func__, "PCMFile Pointer is NULL");
        } else {
            PCM->AUD       = calloc(1, sizeof(AUDHeader));
            PCM->AUD->Meta = calloc(1, sizeof(AUDMetadata));
            PCM->PIC       = calloc(1, sizeof(PICHeader));
        }
        return PCM;
    }
    
    void PCMFile_Identify(PCMFile *PCM, BitBuffer *BitB) {
        uint64_t FileMagic64 = PeekBits(BitIOMSByte, BitIOLSBit, BitB, 64);
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
            BitIOLog(LOG_ERROR, libPCMLibraryName, __func__, "Unrecognized file magic 0x%X", FileMagic64);
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
    
    uint32_t **PCM_ExtractSamples(PCMFile *PCM, BitBuffer *SampleArray, uint64_t NumSamples2Extract) {
        uint32_t **ExtractedSamples = NULL;
        if (PCM->InputFileType == AIFFormat) {
            ExtractedSamples        = AIFExtractSamples(PCM, SampleArray, NumSamples2Extract);
        } else if (PCM->InputFileType == WAVFormat) {
            ExtractedSamples        = WAVExtractSamples(PCM, SampleArray, NumSamples2Extract);
        } else if (PCM->InputFileType == W64Format) {
            ExtractedSamples        = W64ExtractSamples(PCM, SampleArray, NumSamples2Extract);
        }
        return ExtractedSamples;
    }
    
    uint16_t **PCM_ExtractPixels(PCMFile *PCM, BitBuffer *PixelArray, uint64_t NumPixels2Extract) {
        uint16_t **ExtractedPixels = NULL;
        if (PCM->InputFileType == PXMFormat) {
            ExtractedPixels        = PXMExtractPixels(PCM, PixelArray, NumPixels2Extract);
        } else if (PCM->InputFileType == BMPFormat) {
            ExtractedPixels        = BMPExtractPixels(PCM, PixelArray, NumPixels2Extract);
        }
        return ExtractedPixels;
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
    
    void PCM_InsertPixels(PCMFile *PCM, BitBuffer *OutputPixels, uint32_t NumPixels2Write, uint16_t **Pixels2Write) {
        if (PCM->OutputFileType == PXMFormat) {
            PXMInsertPixels(PCM, OutputPixels, NumPixels2Write, Pixels2Write);
        } else if (PCM->OutputFileType == BMPFormat) {
            BMPInsertPixels(PCM, OutputPixels, NumPixels2Write, Pixels2Write);
        }
    }
    
    void PCMSetOutputFileType(PCMFile *PCM, libPCMFileFormats OutputFileType) {
        if (PCM == NULL) {
            BitIOLog(LOG_ERROR, libPCMLibraryName, __func__, "PCM Pointer is NULL");
        } else {
            PCM->OutputFileType = OutputFileType;
        }
    }
    
    void PCMSetOutputPXMType(PCMFile *PCM, PXMTypes PXMType) {
        if (PCM == NULL) {
            BitIOLog(LOG_ERROR, libPCMLibraryName, __func__, "PCM Pointer is NULL");
        } else {
            PCM->PIC->PXMType = PXMType;
        }
    }
    
    void PCMSetNumOutputSamples(PCMFile *PCM, uint64_t NumChannelIndependentSamples) {
        if (PCM == NULL) {
            BitIOLog(LOG_ERROR, libPCMLibraryName, __func__, "PCM Pointer is NULL");
        } else {
            PCM->NumChannelAgnosticSamples = NumChannelIndependentSamples;
        }
    }
    
    uint8_t PCMGetBitDepth(PCMFile *PCM) {
        uint8_t BitDepth = 0;
        if (PCM->InputFileType == AIFFormat || PCM->InputFileType == WAVFormat || PCM->InputFileType == W64Format) {
         	BitDepth = (uint8_t) PCM->BitDepth;
        } else if (PCM->InputFileType == BMPFormat || PCM->InputFileType == PXMFormat) {
         	BitDepth = (uint8_t) PCM->BitDepth;
        }
        return BitDepth;
    }
    
    uint64_t PCMGetNumChannels(PCMFile *PCM) {
        return PCM->NumChannels;
    }
    
    uint64_t PCMGetNumSamples(PCMFile *PCM) {
        return PCM->NumChannelAgnosticSamples;
    }
    
    void PCMFileDeinit(PCMFile *PCM) {
        free(PCM->AUD->Meta);
        free(PCM->AUD);
        free(PCM->PIC);
        free(PCM);
    }
    
#ifdef __cplusplus
}
#endif
