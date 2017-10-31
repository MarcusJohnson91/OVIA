#include <stdio.h>
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
    
    PCMFile *PCMFileInit(void) {
        PCMFile *PCM = calloc(1, sizeof(PCMFile));
        return PCM;
    }
    
    void IdentifyPCMFile(PCMFile *PCM, BitBuffer *BitB) {
        uint64_t FileMagic64 = PeekBits(BitIOMSByte, BitIOLSBit, BitB, 64);
        uint16_t FileMagic16 = FileMagic64 & 0xFFFF;
        uint32_t FileMagic32 = FileMagic64 & 0xFFFFFFFF;
        
        if (FileMagic16 == BMP_BM) {
            PCM->PCMFileType = BMPFormat;
        } else if (FileMagic16 == PortableBitMapASCII || FileMagic16 == PortableBitMapBinary || FileMagic16 == PortablePixMapASCII || FileMagic16 == PortablePixMapBinary || FileMagic16 == PortableGrayMapASCII || FileMagic16 == PortableGrayMapBinary || FileMagic16 == PortableAnyMap) {
            PCM->PCMFileType = PXMFormat;
        } else if (FileMagic32 == AIF_FORM) {
            PCM->PCMFileType = AIFFormat;
        } else if (FileMagic32 == WAV_RIFF) {
            PCM->PCMFileType = WAVFormat;
        } else if (FileMagic32 == 0x72696666) {
            PCM->PCMFileType = W64Format;
        } else {
            BitIOLog(LOG_ERROR, "libPCM", "IdentifyPCMFile", "Unrecognized file magic 0x%X", FileMagic64);
        }
    }
    
    void ParsePCMMetadata(PCMFile *PCM, BitBuffer *BitB) {
        if (PCM->PCMFileType == AIFFormat) {
            AIFParseMetadata(PCM, BitB);
        } else if (PCM->PCMFileType == WAVFormat) {
            WAVParseMetadata(PCM, BitB);
        } else if (PCM->PCMFileType == W64Format) {
            W64ParseMetadata(PCM, BitB);
        } else if (PCM->PCMFileType == BMPFormat) {
            BMPParseMetadata(PCM, BitB);
        } else if (PCM->PCMFileType == PXMFormat) {
            PXMParseMetadata(PCM, BitB);
        }
    }
    
    uint32_t **ExtractSamples(PCMFile *PCM, BitBuffer *SampleArray, uint64_t NumSamples2Extract) {
        uint32_t **ExtractedSamples = NULL;
        if (PCM->PCMFileType == AIFFormat) {
            ExtractedSamples        = AIFExtractSamples(PCM, SampleArray, NumSamples2Extract);
        } else if (PCM->PCMFileType == WAVFormat) {
            ExtractedSamples        = WAVExtractSamples(PCM, SampleArray, NumSamples2Extract);
        } else if (PCM->PCMFileType == W64Format) {
            ExtractedSamples        = W64ExtractSamples(PCM, SampleArray, NumSamples2Extract);
        }
        return ExtractedSamples;
    }
    
    uint16_t **ExtractPixels(PCMFile *PCM, BitBuffer *PixelArray, uint64_t NumPixels2Extract) {
        uint16_t **ExtractedPixels = NULL;
        if (PCM->PCMFileType == PXMFormat) {
            ExtractedPixels        = PXMExtractPixels(PCM, PixelArray, NumPixels2Extract);
        } else if (PCM->PCMFileType == BMPFormat) {
            ExtractedPixels        = BMPExtractPixels(PCM, PixelArray, NumPixels2Extract);
        }
        return ExtractedPixels;
    }
    
    bool IsThereMoreMetadata(PCMFile *PCM) {
        bool Truth = No;
        if (PCM->DataLeft > 1) {
            Truth = Yes;
        }
        return Truth;
    }
    
    void PCMFileDeinit(PCMFile *PCM) {
        free(PCM->AUD->Meta);
        free(PCM->AUD);
        free(PCM->BMP);
        free(PCM->PXM);
        free(PCM);
    }
    
#ifdef __cplusplus
}
#endif
