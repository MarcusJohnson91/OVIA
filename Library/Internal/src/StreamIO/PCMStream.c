#include <stdio.h>
#include "../include/libPCM.h"
#include "../include/PCMTypes.h"
#include "../include/WAVCommon.h"
#include "../include/W64Common.h"
#include "../include/AIFCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    PCMFile *InitPCMFile(void) {
        PCMFile *PCM = calloc(sizeof(PCMFile), 1);
        PCM->Meta    = calloc(sizeof(PCMMetadata), 1);
        return PCM;
    }
    
    
    // So, We need to accept a BitInput pointer, and start reading the input file to discover it's file type, then call the dedicated format metadata parser to get the info we need and verify it's raw PCM, and then line us up with the PCM samples, and wait for calls to ExtractSamples
    
    void ClosePCMFile(PCMFile *PCM) {
        free(PCM->Meta);
        free(PCM);
    }
    
    // I want to just hand a file pointer here, and tell it to extract X samples (regardless of channel count)
    // So we need to parse the various chunks in the file, extract info of value into a format specific struct
    // then the ExtractSamples function needs to extract the samples and pass back the channel count, sample rate, and bit depth.
    // and the ExtractSamples function needs to handle all of that for all formats
    // So we need a struct to contain a bool that says if the metadata has been parsed and what sample we're on.
    // Actually which sample we're on is already being tracked by BitIO
    
    /*!
     @param NumSamples2Extract is the number of channel agnostic samples to read from the input file
     */
    void ExtractSamples(BitInput *BitI, PCMFile *PCM, uint64_t NumSamples2Extract) {
        WAVHeader *WAV = calloc(sizeof(WAVHeader), 1);
        W64Header *W64 = calloc(sizeof(W64Header), 1);
        AIFHeader *AIF = calloc(sizeof(AIFHeader), 1);
        if (PCM->MetadataHasBeenParsed == false) {
            uint32_t Magic = ReadBits(BitI, 32, true);
            switch (Magic) {
                case WAVMagic:
                    PCM->FileType = WAV_File;
                    IdentifyPCMFile(BitI, PCM);
                    break;
                case W64Magic:
                    PCM->FileType = W64_File;
                    IdentifyPCMFile(BitI, PCM);
                    break;
                case AIFMagic:
                    PCM->FileType = AIFF_File;
                    IdentifyPCMFile(BitI, PCM);
                    break;
                    
                default:
                    break;
            }
        } else {
            // just read the requested samples
            if (PCM->FileType == WAV_File) {
                WAVExtractSamples(BitI, PCM, NumSamples2Extract);
            }
        }
    }
    
    void ParseWAVFile(BitInput *BitI, PCMFile *PCM) {
        char ErrorDescription[BitIOStringSize];
        
        uint32_t ChunkID   = ReadBits(BitI, 32, true);
        uint32_t ChunkSize = ReadBits(BitI, 32, true);
        
        switch (ChunkID) {
            case WAV_LIST:
                ParseWavLISTChunk(BitI, PCM, ChunkSize);
                break;
            case WAV_FMT:
                ParseWavFMTChunk(BitI, PCM, ChunkSize);
                break;
            case WAV_WAVE:
                SkipBits(BitI, 32);
                break;
            case WAV_DATA:
                ParseWavDATAChunk(BitI, PCM, ChunkSize);
                break;
                
            default:
                snprintf(ErrorDescription, BitIOStringSize, "Invalid ChunkID: 0x%X", ChunkID);
                Log(LOG_ERR, "libPCM", "ParseWAVFile", ErrorDescription);
                break;
        }
    }
    
    void ParseW64File(BitInput *BitI, W64Header *W64) {
        uint32_t ChunkID   = ReadBits(BitI, 32, true);
        SkipBits(BitI, 96); // The rest of the GUID.
        uint64_t ChunkSize = ReadBits(BitI, 64, true);
        switch (ChunkID) {
            case W64_FMT:
                ParseW64FMTChunk(BitI, W64);
                break;
            case W64_BEXT:
                ParseW64BEXTChunk(BitI, W64);
                break;
                
            default:
                break;
        }
    }
    
    void ParseAIFFChunk(BitInput *BitI, AIFHeader *AIF) {
        
    }
    
    void ParseAIFCChunk(BitInput *BitI, AIFHeader *AIF) {
        
    }
    
    void ParseAIFFile(BitInput *BitI, AIFHeader *AIF) {
        // if NumFrames = 0, SNSD may not exist.
        uint32_t AIFSize = ReadBits(BitI, 32, true);
        uint32_t ChunkID = ReadBits(BitI, 32, true);
        switch (ChunkID) { // If the number of sound data bytes is odd, appened a padding sample.
            case AIF_AIFF:
                
                break;
            case AIF_AIFC:
            
            default:
                break;
        }
    }
    
    void IdentifyPCMFile(BitInput *BitI, PCMFile *PCM) {
        uint8_t  FileType = 0;
        
        uint32_t InputMagic = ReadBits(BitI, 32, true);
        if (InputMagic == WAV_RIFF) {
            PCM->FileType = WAV_File;
            WAVHeader *WAV = calloc(sizeof(WAVHeader), 1);
            PCM->WAV = WAV;
            ParseWAVFile(BitI, PCM);
        } else if (InputMagic == W64_RIFF) {
            PCM->FileType = W64_File;
            SkipBits(BitI, 96); // Rest of the W64 RIFF GUID
            SkipBits(BitI, 64); // RIFF ChunkSize
            W64Header *W64 = calloc(sizeof(W64Header), 1);
            ParseW64File(BitI, W64);
        } else if (InputMagic == AIF_FORM) {
            PCM->FileType = AIFF_File;
            AIFHeader *AIF = calloc(sizeof(AIFHeader), 1);
            ParseAIFFile(BitI, AIF);
        }
    }
    
#ifdef __cplusplus
}
#endif
