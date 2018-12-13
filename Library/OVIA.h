#include "../../Dependencies/FoundationIO/libFoundationIO/include/BitIO.h"
#include "../../Dependencies/FoundationIO/libFoundationIO/include/CommandLineIO.h"
#include "../../Dependencies/FoundationIO/libFoundationIO/include/Log.h"
#include "../../Dependencies/FoundationIO/libFoundationIO/include/Math.h"
#include "../../Dependencies/FoundationIO/libFoundationIO/include/StringIO.h"

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
    
#define PNGMagic 0x89504E470D0A1A0A
    
    typedef enum OVIA_PNG_ChunkMarkers {
        acTLMarker         = 0x6163544C,
        bKGDMarker         = 0x626B4744,
        cHRMMarker         = 0x6348524D,
        fcTLMarker         = 0x6663544C,
        fDATMarker         = 0x66444154,
        gAMAMarker         = 0x67414D41,
        hISTMarker         = 0x68495354,
        iCCPMarker         = 0x69434350,
        IDATMarker         = 0x49444154,
        iHDRMarker         = 0x49484452,
        iTXtMarker         = 0x69545874,
        oFFsMarker         = 0x6F464673,
        pCALMarker         = 0x7043414C,
        pHYsMarker         = 0x70485973,
        PLTEMarker         = 0x504C5445,
        sBITMarker         = 0x73424954,
        sCALMarker         = 0x7343414c,
        sRGBMarker         = 0x73524742,
        sTERMarker         = 0x73544552,
        tEXtMarker         = 0x74455874,
        zTXtMarker         = 0x7A545874,
        tIMEMarker         = 0x74494d45,
        tRNSMarker         = 0x74524e53,
        sPLTMarker         = 0x73504c54,
    } OVIA_PNG_ChunkMarkers;
    
    typedef enum OVIA_PNG_ColorTypes {
        PNG_Grayscale      = 0,
        PNG_RGB            = 2,
        PNG_PalettedRGB    = 3,
        PNG_GrayAlpha      = 4,
        PNG_RGBA           = 6,
    } OVIA_PNG_ColorTypes;
    
    typedef enum OVIA_PNG_Filter_Types {
        NotFiltered   = 0,
        SubFilter     = 1,
        UpFilter      = 2,
        AverageFilter = 3,
        PaethFilter   = 4,
    } OVIA_PNG_Filter_Types;
    
    enum OVIA_PNG_Interlace_Types {
        PNGNotInterlaced   = 0,
        PNGInterlacedAdam7 = 1,
    };
    
    static const UTF8 OVIA_PNG_MonthMap[12][4] = {
        U8("Jan"), U8("Feb"), U8("Mar"), U8("Apr"), U8("May"), U8("Jun"),
        U8("Jul"), U8("Aug"), U8("Sep"), U8("Oct"), U8("Nov"), U8("Dec"),
    };
    
    static const uint8_t OVIA_PNG_NumChannelsPerColorType[7] = {
        1, 0, 3, 3, 4, 0, 4
    };
    
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
    
    OVIA                *OVIA_Init(void);
    
    AudioContainer      *OVIA_ExtractSamples(OVIA *Ovia, BitBuffer *BitB);
    AudioContainer      *OVIA_GetAudioContainerPointer(OVIA *Ovia);
    bool                 OVIA_IsThereMoreMetadata(OVIA *Ovia);
    ImageContainer      *OVIA_ExtractImage(OVIA *Ovia, BitBuffer *BitB);
    int64_t              OVIA_GetHeight(OVIA *Ovia);
    int64_t              OVIA_GetWidth(OVIA *Ovia);
    uint64_t             OVIA_GetBitDepth(OVIA *Ovia);
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
    
    void                 OVIA_Deinit(OVIA *Ovia);
    
#ifdef __cplusplus
}
#endif

#endif /* OVIA_libOVIA_H */
