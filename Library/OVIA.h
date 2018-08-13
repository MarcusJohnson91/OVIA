#include "../../Dependencies/FoundationIO/libFoundationIO/include/BitIO.h"
#include "../../Dependencies/FoundationIO/libFoundationIO/include/CommandLineIO.h"
#include "../../Dependencies/FoundationIO/libFoundationIO/include/Log.h"
#include "../../Dependencies/FoundationIO/libFoundationIO/include/Math.h"

#pragma once

#ifndef OVIA_libOVIA_H
#define OVIA_libOVIA_H

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef enum OVIA_Types {
        UnknownType           = 0,
        AudioType             = 1,
        ImageType             = 2,
    } OVIA_Types;
    
    typedef enum OVIA_FileFormats {
        UnknownFormat         = 0,
        AIFFormat             = 1,
        WAVFormat             = 2,
        W64Format             = 3,
        FLACFormat            = 4,
        BMPFormat             = 5,
        PNMFormat             = 6,
        PNGFormat             = 7,
    } OVIA_FileFormats;
    
    /*
     OVIA API Design:
     
     ProgramA exists, user calls it with arguments.
     
     Parse the arguments with CommandLineIO.
     
     Create BitInput, BitOutput, and BitBuffer structs for dealing with the file.
     
     it's a media file, the user wants to load the image into memory to manipulate it.
     
     The user asks OVIA to identify the file, to do thi OVIA_Identify needs access to the BitBuffer.
     
     The file is identified as PNG.
     
     The user wants to decode the PNG.
     
     The User needs to get the metadata for the file, and put it into OVIA, and the Image/Audio Container.
     */
    
    typedef struct AudioContainer AudioContainer;
    
    typedef struct ImageContainer ImageContainer;
    
    typedef struct OVIA OVIA;
    
    OVIA                *OVIA_Identify(BitBuffer *BitB);
    
    void                 OVIA_Audio_ReadMetadata(OVIA *Ovia, AudioContainer *Audio, BitBuffer *BitB);
    
    void                 OVIA_Image_ReadMetadata(OVIA *Ovia, ImageContainer *Image, BitBuffer *BitB);
    
    void                 PCMFile_ParseMetadata(PCMFile *PCM, BitBuffer *BitB);
    
    bool                 PCM_IsThereMoreMetadata(PCMFile *PCM);
    
    void                 PCM_SetOutputPNMType(PCMFile *PCM, PNMTypes PNMType);
    
    void                 PCM_SetNumOutputSamples(PCMFile *PCM, uint64_t NumChannelIndependentSamples);
    
    void                 PCM_ExtractSamples(PCMFile *PCM, BitBuffer *SampleArray, uint64_t NumSamples2Extract, uint32_t **ExtractedSamples);
    
    void                 PCM_InsertSamples(PCMFile *PCM, BitBuffer *OutputSamples, uint32_t NumSamples2Write, uint32_t **Samples2Write);
    
    void                 PCM_InsertPixels(PCMFile *PCM, BitBuffer *OutputPixels, uint32_t NumPixels2Write, uint16_t **Pixels2Write);
    
    void                 PCM_SetOutputFileType(PCMFile *PCM, OVIA_FileFormats OutputFileType);
    
    void                 PCM_WriteHeader(PCMFile *PCM, BitBuffer *BitB);
    
    void                 PCMFile_Deinit(PCMFile *PCM);
    
    
    void PCM_InsertImage(PCMFile *PCM, BitBuffer *CreatedImage, uint16_t ***Image2Insert);
    
    uint16_t ***PCM_ExtractImage(PCMFile *PCM, BitBuffer *PixelArray);
    
#ifdef __cplusplus
}
#endif

#endif /* OVIA_libOVIA_H */
