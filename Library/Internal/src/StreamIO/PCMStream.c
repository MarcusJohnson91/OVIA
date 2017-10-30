#include <stdio.h>
#include "../include/libPCM.h"
#include "../include/Private/libPCMTypes.h"
#include "../include/Private/Audio/WAVCommon.h"
#include "../include/Private/Audio/W64Common.h"
#include "../include/Private/Audio/AIFCommon.h"

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
        } else if (FileMagic16 == PortableBitMap1 || FileMagic16 == PortableBitMap2 || FileMagic16 == PortablePixMap1 || FileMagic16 == PortablePixMap2 || FileMagic16 == PortableGrayMap1 || FileMagic16 == PortableGrayMap2 || FileMagic16 == PortableAnyMap) {
            PCM->PCMFileType = PXMFormat;
        } else if (FileMagic32 == AIF_FORM) {
            PCM->PCMFileType = AIFFormat;
        } else if (FileMagic32 == WAV_RIFF) {
            PCM->PCMFileType = WAVFormat;
        } else if (FileMagic32 == 0x72696666) {
            PCM->PCMFileType = W64Format;
        } else {
            Log(LOG_ERROR, "libPCM", "IdentifyPCMFile", "Unrecognized file magic 0x%X", FileMagic64);
        }
    }
    
    void ParsePCMMetadata(PCMFile *PCM, BitBuffer *BitB) {
        if (PCM->PCMFileType == AIFFormat) {
            ParseAIFMetadata(PCM, BitB);
        } else if (PCM->PCMFileType == BMPFormat) {
            
        } else if (PCM->PCMFileType == PXMFormat) {
            
        } else if (PCM->PCMFileType == WAVFormat) {
            
        } else if (PCM->PCMFileType == W64Format) {
            
        }
    }
    
    void ExtractPCMSamples(PCMFile *PCM, BitBuffer *BitB, uint64_t Samples2Extract) {
        
    }
    
    
    
    
    
    
    
    
    
    
    
    // Well, we need to identify the file type, set the approperiate values (is it audio or video?), then extract the correct amount of data depending on the type.
    
    
    
    
    // So, We need to accept a BitInput pointer, and start reading the input file to discover it's file type, then call the dedicated format metadata parser to get the info we need and verify it's raw PCM, and then line us up with the PCM samples, and wait for calls to ExtractSamples
    
    // I want to just hand a file pointer here, and tell it to extract X samples (regardless of channel count)
    // So we need to parse the various chunks in the file, extract info of value into a format specific struct
    // then the ExtractSamples function needs to extract the samples and pass back the channel count, sample rate, and bit depth.
    // and the ExtractSamples function needs to handle all of that for all formats
    // So we need a struct to contain a bool that says if the metadata has been parsed and what sample we're on.
    // Actually which sample we're on is already being tracked by BitIO
    
    /*!
     @param NumSamples2Extract is the number of channel agnostic samples to read from the input file
     */
    uint32_t **ExtractSamples(PCMFile *PCM, BitBuffer *BitB, uint64_t NumSamples2Extract) {
        uint32_t **ExtractedSamples = NULL;
        if (PCM == NULL) {
            Log(LOG_ERROR, "libPCM", "ExtractSamples", "Pointer to PCMFile is NULL");
        } else if (BitB == NULL) {
            Log(LOG_ERROR, "libPCM", "ExtractSamples", "Pointer to BitBuffer is NULL");
        } else if (NumSamples2Extract == 0) {
            Log(LOG_ERROR, "libPCM", "ExtractSamples", "Requested too few samples %d", NumSamples2Extract);
        } else {
            // just read the requested samples
            if (PCM->PCMFileType == WAVFormat) {
                ExtractedSamples = WAVExtractSamples(PCM, BitB, NumSamples2Extract);
            } else if (PCM->PCMFileType == W64Format) {
                
            } else if (PCM->PCMFileType == AIFFormat) {
                
            } else if (PCM->PCMFileType == PXMFormat) {
                
            } else if (PCM->PCMFileType == BMPFormat) {
                
            } else {
                Log(LOG_ERROR, "libPCM", "ExtractSamples", "Unknown file format, Magic: 0x%X", PCM->PCMFileType);
            }
        }
        return ExtractedSamples;
    }
    
    void ParseW64File(PCMFile *PCM, BitBuffer *BitB) {
        uint8_t *ChunkUUID = ReadGUUID(BitIOUUIDString, BitB);
        uint64_t ChunkSize = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 64);
        if (CompareGUUIDs(ChunkUUID, W64_FMT_GUIDString, BitIOBinaryUUID) == Yes) {
            ParseW64FMTChunk(PCM, BitB);
        } else if (CompareGUUIDs(ChunkUUID, W64_BEXT_GUIDString, BitIOBinaryUUID) == Yes) {
            ParseW64BEXTChunk(PCM, BitB);
        }
    }
    
    void ParseAIFFChunk(PCMFile *PCM, BitBuffer *BitB) {
        
    }
    
    void ParseAIFCChunk(PCMFile *PCM, BitBuffer *BitB) {
        
    }
    
    void ParseAIFFile(PCMFile *PCM, BitBuffer *BitB) {
        // if NumFrames = 0, SNSD may not exist.
        uint32_t AIFSize = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
        uint32_t ChunkID = ReadBits(BitIOLSByte, BitIOLSBit, BitB, 32);
        switch (ChunkID) { // If the number of sound data bytes is odd, appened a padding sample.
            case AIF_AIFF:
                
                break;
            case AIF_AIFC:
                
            default:
                break;
        }
    }
    
    void ExtractAudioSamples(PCMFile *PCM, BitBuffer *ExtractedSamples, uint64_t Samples2Extract) {
        
    }
    
    void ExtractImageLines(PCMFile *PCM, BitBuffer *ExtractedLines, uint64_t Lines2Extract) {
        
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
