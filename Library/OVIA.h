#include <stdbool.h>
#include <stdint.h>

/* Forward declare StringIO's types */
#ifndef               UTF8
typedef               unsigned char                        UTF8;
#endif /* UTF8 */
/* Forward declare StringIO's types */

#pragma once

#ifndef OVIA_libOVIA_H
#define OVIA_libOVIA_H

#ifdef __cplusplus
extern "C" {
#endif
    
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
    
    typedef struct       AudioContainer AudioContainer; // Forward declare ContainerIO's tyoes
    
    typedef struct       ImageContainer ImageContainer; // Forward declare ContainerIO's tyoes
    
    typedef struct       BitBuffer      BitBuffer;      // Forward declare BitIO's types
    
    typedef struct       OVIA OVIA;
    
    OVIA                *OVIA_Init(void);
    
    AudioContainer      *OVIA_ExtractSamples(OVIA *Ovia, BitBuffer *BitB);
    AudioContainer      *OVIA_GetAudioContainerPointer(OVIA *Ovia);
    bool                 OVIA_IsThereMoreMetadata(OVIA *Ovia);
    ImageContainer      *OVIA_ExtractImage(OVIA *Ovia, BitBuffer *BitB);
    int64_t              OVIA_GetHeight(OVIA *Ovia);
    int64_t              OVIA_GetWidth(OVIA *Ovia);
    uint8_t              OVIA_GetBitDepth(OVIA *Ovia);
    uint64_t             OVIA_GetBlockSize(OVIA *Ovia);
    uint64_t             OVIA_GetFileSize(OVIA *Ovia);
    uint64_t             OVIA_GetNumChannels(OVIA *Ovia);
    uint64_t             OVIA_GetNumSamples(OVIA *Ovia);
    uint64_t             OVIA_GetNumTags(OVIA *Ovia);
    uint64_t             OVIA_GetSampleOffset(OVIA *Ovia);
    uint64_t             OVIA_GetSampleRate(OVIA *Ovia);
    uint64_t             OVIA_GetTagsIndex(OVIA *Ovia, OVIA_TagTypes TagType);
    uint64_t             OVIA_GetTagSize(OVIA *Ovia, uint64_t Tag);
    UTF8                *OVIA_GetTag(OVIA *Ovia, uint64_t Tag);
    void                 OVIA_AppendSamples(OVIA *Ovia, AudioContainer *Audio, BitBuffer *BitB);
    void                 OVIA_Audio_ReadMetadata(OVIA *Ovia, AudioContainer *Audio, BitBuffer *BitB);
    void                 OVIA_Identify(OVIA *Ovia, BitBuffer *BitB);
    void                 OVIA_Image_ReadMetadata(OVIA *Ovia, ImageContainer *Image, BitBuffer *BitB);
    void                 OVIA_InsertFrame(OVIA *Ovia, ImageContainer *Image, BitBuffer *BitB);
    void                 OVIA_InsertImage(OVIA *Ovia, ImageContainer *Image, BitBuffer *BitB);
    void                 OVIA_ParseMetadata(OVIA *Ovia, BitBuffer *BitB);
    void                 OVIA_SetAudioContainerPointer(OVIA *Ovia, AudioContainer *Audio);
    void                 OVIA_SetBitDepth(OVIA *Ovia, uint64_t BitDepth);
    void                 OVIA_SetBlockSize(OVIA *Ovia, uint64_t BlockSize);
    void                 OVIA_SetFileSize(OVIA *Ovia, uint64_t FileSize);
    void                 OVIA_SetHeight(OVIA *Ovia, int64_t Height);
    void                 OVIA_SetNumChannels(OVIA *Ovia, uint64_t NumChannels);
    void                 OVIA_SetNumSamples(OVIA *Ovia, uint64_t NumSamples);
    void                 OVIA_SetNumTags(OVIA *Ovia, uint64_t NumTags);
    void                 OVIA_SetSampleOffset(OVIA *Ovia, uint64_t SampleOffset);
    void                 OVIA_SetSampleRate(OVIA *Ovia, uint64_t SampleRate);
    void                 OVIA_SetTag(OVIA *Ovia, OVIA_TagTypes TagType, UTF8 *Tag);
    void                 OVIA_SetWidth(OVIA *Ovia, int64_t Width);
    void                 OVIA_WriteHeader(OVIA *Ovia, BitBuffer *BitB);
    
    void                 OVIA_SetFormat(OVIA *Ovia, OVIA_FileFormats Format);
    void                 OVIA_Deinit(OVIA *Ovia);
    
#ifdef __cplusplus
}
#endif

#endif /* OVIA_libOVIA_H */
