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
    
    typedef enum OVIA_TagTypes {
        UnknownTag            = 0,
        TitleTag              = 1,
        AuthorTag             = 2,
        AnnotationTag         = 3,
        ArtistTag             = 4,
    } OVIA_TagTypes;
    
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
    
    typedef struct       AudioContainer AudioContainer;
    
    typedef struct       ImageContainer ImageContainer;
    
    typedef struct       OVIA OVIA;
    
    void                 OVIA_Identify(OVIA *Ovia, BitBuffer *BitB);
    
    void                 OVIA_SetNumChannels(OVIA *Ovia, uint64_t NumChannels);
    
    uint64_t             OVIA_GetNumChannels(OVIA *Ovia);
    
    void                 OVIA_SetNumSamples(OVIA *Ovia, uint64_t NumSamples);
    
    uint64_t             OVIA_GetNumSamples(OVIA *Ovia);
    
    void                 OVIA_SetBitDepth(OVIA *Ovia, uint64_t BitDepth);
    
    uint64_t             OVIA_GetBitDepth(OVIA *Ovia);
    
    void                 OVIA_SetSampleRate(OVIA *Ovia, uint64_t SampleRate);
    
    uint64_t             OVIA_GetSampleRate(OVIA *Ovia);
    
    void                 OVIA_SetTag(OVIA *Ovia, OVIA_TagTypes TagType, UTF8 *Tag);
    
    uint64_t             OVIA_GetTagsIndex(OVIA *Ovia, OVIA_TagTypes TagType);
    
    UTF8                *OVIA_GetTag(OVIA *Ovia, uint64_t Tag);
    
    void                 OVIA_SetFileSize(OVIA *Ovia, uint64_t FileSize);
    
    uint64_t             OVIA_GetFileSize(OVIA *Ovia);
    
    void OVIA_SetSampleOffset(OVIA *Ovia, uint64_t SampleOffset);
    
    uint64_t OVIA_GetSampleOffset(OVIA *Ovia);
    
    void OVIA_SetBlockSize(OVIA *Ovia, uint64_t BlockSize);
    
    uint64_t OVIA_GetBlockSize(OVIA *Ovia);
    
    
    
    
    
    
    
    void                 OVIA_Audio_ReadMetadata(OVIA *Ovia, AudioContainer *Audio, BitBuffer *BitB);
    
    void                 OVIA_Image_ReadMetadata(OVIA *Ovia, ImageContainer *Image, BitBuffer *BitB);
    
    void                 OVIA_ParseMetadata(OVIA *Ovia, BitBuffer *BitB);
    
    bool                 OVIA_IsThereMoreMetadata(OVIA *Ovia);
    
    void                 OVIA_SetNumOutputSamples(OVIA *Ovia, uint64_t NumChannelIndependentSamples);
    
    void                 OVIA_ExtractSamples(OVIA *Ovia, BitBuffer *SampleArray, uint64_t NumSamples2Extract, uint32_t **ExtractedSamples);
    
    void                 OVIA_InsertSamples(OVIA *Ovia, BitBuffer *OutputSamples, uint32_t NumSamples2Write, uint32_t **Samples2Write);
    
    void                 OVIA_InsertPixels(OVIA *Ovia, BitBuffer *OutputPixels, uint32_t NumPixels2Write, uint16_t **Pixels2Write);
    
    void                 OVIA_WriteHeader(OVIA *Ovia, BitBuffer *BitB);
    
    void                 OVIA_Deinit(OVIA *Ovia);
    
    
    void                 OVIA_InsertImage(OVIA *Ovia, BitBuffer *CreatedImage, ImageContainer *Image);
    
    ImageContainer      *OVIA_ExtractImage(OVIA *Ovia, BitBuffer *BitB);
    
#ifdef __cplusplus
}
#endif

#endif /* OVIA_libOVIA_H */
