#include <stdbool.h>
#include <stdint.h>

#pragma once

#ifndef OVIA_libOVIA_H
#define OVIA_libOVIA_H

#ifdef __cplusplus
extern "C" {
#endif
    
    /*
     OVIA flow:
     
     File is opened by other means.
     
     the largest magicid's size is taken as well as the latest magicid offset
     
     BiggestMagicID + BiggestMagicIDOffset is taken, that many bytes are read from the file.
     
     each MagicID as well as it's offset is used to discover the file's type.
     
     if a match is found with one of the codecs in OVIA, we do further processing, otherwise an error code is returned.
     
     Let's assume that the input file is discovered to be a BMP file.
     
     Well then we need to have a function which calls the BMP decoder and returns an ImageContainer with the decoded data.
     
     so what about:
     
     ImageContainer *OVIA_DecodeImage(BitBuffer *BitB);
     
     OVIA_RegisterDecoders
     
     OVIA_RegisterEncoders
     
     How does registration work?
     
     
     */
    
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
    
    typedef struct       Audio2DContainer  Audio2DContainer; // Forward declare ContainerIO's tyoes
    
    typedef struct       AudioVector       AudioVector;
    
    typedef struct       MetadataContainer MetadataContainer;
    
    typedef struct       ImageContainer    ImageContainer; // Forward declare ContainerIO's tyoes
    
    typedef struct       BitBuffer         BitBuffer;      // Forward declare BitIO's types
    
    typedef struct       OVIA              OVIA;
    
    OVIA                *OVIA_Init(void);
    void                 OVIA_Identify(BitBuffer *BitBuffer);
    void                 OVIA_Deinit(OVIA *Ovia);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
    
    Audio2DContainer    *OVIA_ExtractSamples(BitBuffer *BitB, uint64_t MaxNumSamples);
    AudioVector         *OVIA_ExtractVector(BitBuffer *BitB);
    ImageContainer      *OVIA_ExtractFrame(BitBuffer *BitB); // Used for both image and video formats
    MetadataContainer   *OVIA_ExtractMetadata(BitBuffer *BitB);
    void                 OVIA_AppendSamples(Audio2DContainer *Audio, BitBuffer *BitB);
    void                 OVIA_AppendVector(AudioVector *Vector, BitBuffer *BitB);
    void                 OVIA_AppendFrame(ImageContainer *Frame, BitBuffer *BitB);
    void                 OVIA_AppendMetadata(MetadataContainer *Metadata, BitBuffer *BitB);
    
    void                 OVIA_WriteHeader(BitBuffer *BitB);
    
#ifdef __cplusplus
}
#endif

#endif /* OVIA_libOVIA_H */
