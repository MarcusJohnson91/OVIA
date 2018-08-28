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
    
    uint64_t             OVIA_GetFileSize(OVIA *Ovia);
    
    uint64_t             OVIA_GetSampleOffset(OVIA *Ovia);
    
    uint64_t             OVIA_GetBlockSize(OVIA *Ovia);
    
    int64_t              OVIA_GetWidth(OVIA *Ovia);
    
    int64_t              OVIA_GetHeight(OVIA *Ovia);
    
    void                 OVIA_SetNumChannels(OVIA *Ovia, uint64_t NumChannels);
    
    void                 OVIA_SetNumSamples(OVIA *Ovia, uint64_t NumSamples);
    
    void                 OVIA_SetBitDepth(OVIA *Ovia, uint64_t BitDepth);
    
    void                 OVIA_SetSampleRate(OVIA *Ovia, uint64_t SampleRate);
    
    void                 OVIA_SetTag(OVIA *Ovia, OVIA_TagTypes TagType, UTF8 *Tag);
    
    void                 OVIA_SetFileSize(OVIA *Ovia, uint64_t FileSize);
    
    void                 OVIA_SetSampleOffset(OVIA *Ovia, uint64_t SampleOffset);
    
    void                 OVIA_SetBlockSize(OVIA *Ovia, uint64_t BlockSize);
    
    void                 OVIA_SetWidth(OVIA *Ovia, int64_t Width);
    
    void                 OVIA_SetHeight(OVIA *Ovia, int64_t Height);
    
    void                 OVIA_FLAC_SetMinBlockSize(OVIA *Ovia, uint16_t MinBlockSize);
    
    void                 OVIA_FLAC_SetMaxBlockSize(OVIA *Ovia, uint16_t MaxBlockSize);
    
    void                 OVIA_FLAC_SetMinFrameSize(OVIA *Ovia, uint16_t MinFrameSize);
    
    void                 OVIA_FLAC_SetMaxFrameSize(OVIA *Ovia, uint16_t MaxFrameSize);
    
    void                 OVIA_FLAC_SetMD5(OVIA *Ovia, uint8_t *MD5);
    
    
    /* Format Specific Options */
    void OVIA_PNG_SetIHDR(ImageContainer *Image, const bool Progressive);
    
    
    
    
    
    
    
    
    
    
    void                 OVIA_Audio_ReadMetadata(OVIA *Ovia, AudioContainer *Audio, BitBuffer *BitB);
    
    void                 OVIA_Image_ReadMetadata(OVIA *Ovia, ImageContainer *Image, BitBuffer *BitB);
    
    void                 OVIA_ParseMetadata(OVIA *Ovia, BitBuffer *BitB);
    
    bool                 OVIA_IsThereMoreMetadata(OVIA *Ovia);
    
    void                 OVIA_SetNumOutputSamples(OVIA *Ovia, uint64_t NumChannelIndependentSamples);
    
    AudioContainer      *OVIA_ExtractSamples(OVIA *Ovia, BitBuffer *BitB);
    
    void                 OVIA_AppendSamples(OVIA *Ovia, BitBuffer *BitB, AudioContainer *Audio);
    
    void                 OVIA_InsertFrame(OVIA *Ovia, BitBuffer *BitB, ImageContainer *Image);
    
    void                 OVIA_WriteHeader(OVIA *Ovia, BitBuffer *BitB);
    
    void                 OVIA_Deinit(OVIA *Ovia);
    
    
    void                 OVIA_InsertImage(OVIA *Ovia, BitBuffer *BitB, ImageContainer *Image);
    
    ImageContainer      *OVIA_ExtractImage(OVIA *Ovia, BitBuffer *BitB);
    
#ifdef __cplusplus
}
#endif

#endif /* OVIA_libOVIA_H */
