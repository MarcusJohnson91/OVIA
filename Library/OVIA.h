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
    
    typedef enum OVIA_TagTypes {
        UnknownTag            = 0,
        TitleTag              = 1,
        AuthorTag             = 2,
        AnnotationTag         = 3,
        ArtistTag             = 4,
        AlbumTag              = 5,
        GenreTag              = 6,
        ReleaseTag            = 7,
        CreatingSoftware      = 8,
    } OVIA_TagTypes;
    
    /*
     OVIA API Design:
     
     ProgramA exists, user calls it with arguments.
     
     Parse the arguments with CommandLineIO.
     
     Create BitInput, BitOutput, and BitBuffer structs for dealing with the file.
     
     it's a media file, the user wants to load the image into memory to manipulate it.
     
     The user asks OVIA to identify the file, to do thi OVIA_Identify needs access to the BitBuffer.
     
     The file is identified as PNG.
     
     The user wants to OVIA the PNG.
     
     The User needs to get the metadata for the file, and put it into OVIA, and the Image/Audio Container.
     */
    
    typedef struct       AudioContainer AudioContainer;
    
    typedef struct       ImageContainer ImageContainer;
    
    typedef struct       OVIA OVIA;
    
    void                 OVIA_Identify(OVIA *Ovia, BitBuffer *BitB);
    
    uint64_t             OVIA_GetNumChannels(OVIA *Ovia);
    
    uint64_t             OVIA_GetNumSamples(OVIA *Ovia);
    
    uint64_t             OVIA_GetBitDepth(OVIA *Ovia);
    
    uint64_t             OVIA_GetSampleRate(OVIA *Ovia);
    
    uint64_t             OVIA_GetTagsIndex(OVIA *Ovia, OVIA_TagTypes TagType);
    
    UTF8                *OVIA_GetTag(OVIA *Ovia, uint64_t Tag);
    
    uint64_t             OVIA_GetNumTags(OVIA *Ovia);
    
    void                 OVIA_SetNumTags(OVIA *Ovia, uint64_t NumTags);
    
    uint64_t             OVIA_GetTagSize(OVIA *Ovia, uint64_t Tag);
    
    uint64_t             OVIA_GetFileSize(OVIA *Ovia);
    
    uint64_t             OVIA_GetSampleOffset(OVIA *Ovia);
    
    uint64_t             OVIA_GetBlockSize(OVIA *Ovia);
    
    int64_t              OVIA_GetWidth(OVIA *Ovia);
    
    int64_t              OVIA_GetHeight(OVIA *Ovia);
    
    void                 OVIA_Audio_ReadMetadata(OVIA *Ovia, AudioContainer *Audio, BitBuffer *BitB);
    
    void                 OVIA_Image_ReadMetadata(OVIA *Ovia, ImageContainer *Image, BitBuffer *BitB);
    
    void                 OVIA_ParseMetadata(OVIA *Ovia, BitBuffer *BitB);
    
    bool                 OVIA_IsThereMoreMetadata(OVIA *Ovia);
    
    AudioContainer      *OVIA_ExtractSamples(OVIA *Ovia, BitBuffer *BitB);
    
    void                 OVIA_AppendSamples(OVIA *Ovia, AudioContainer *Audio, BitBuffer *BitB);
    
    void                 OVIA_InsertFrame(OVIA *Ovia, ImageContainer *Image, BitBuffer *BitB);
    
    void                 OVIA_WriteHeader(OVIA *Ovia, BitBuffer *BitB);
    
    void                 OVIA_InsertImage(OVIA *Ovia, ImageContainer *Image, BitBuffer *BitB);
    
    ImageContainer      *OVIA_ExtractImage(OVIA *Ovia, BitBuffer *BitB);
    
    void                 OVIA_Deinit(OVIA *Ovia);
    
#ifdef __cplusplus
}
#endif

#endif /* OVIA_libOVIA_H */
