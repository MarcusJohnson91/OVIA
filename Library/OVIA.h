#include <stdbool.h>
#include <stdint.h>

#pragma once

#ifndef OVIA_libOVIA_H
#define OVIA_libOVIA_H

#ifdef __cplusplus
extern "C" {
#endif
    
#ifndef   FoundationIO_StringType8
#define   FoundationIO_StringType8 (1)
#ifdef    UTF8
#undef    UTF8
#endif /* UTF8 */
    typedef   unsigned char  UTF8;
#endif /* FoundationIO_StringType8 */
    
#ifndef   FoundationIO_StringType16
#define   FoundationIO_StringType16 (2)
#ifdef    UTF16
#undef    UTF16
#endif /* UTF16 */
    typedef  uint_least16_t  UTF16;
#endif /* FoundationIO_StringType16 */
    
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
    
    typedef enum OVIA_CodecIDs {
        CodecID_Unknown       = 0,
        CodecID_AIF           = 1,
        CodecID_AIFC          = 2,
        CodecID_WAV           = 3,
        CodecID_W64           = 4,
        CodecID_FLAC          = 5,
        CodecID_BMP           = 6,
        CodecID_PAM           = 7,
        CodecID_PNG           = 8,
        CodecID_PBM_B         = 9,
        CodecID_PBM_A         = 10,
        CodecID_PGM_B         = 11,
        CodecID_PGM_A         = 12,
        CodecID_PPM_B         = 13,
        CodecID_PPM_A         = 14,
        OVIA_NumPNM           = 6,
        OVIA_NumCodecs        = CodecID_PPM_A - OVIA_NumPNM
    } OVIA_CodecIDs;
    
    typedef struct       Audio2DContainer  Audio2DContainer; // Forward declare ContainerIO's tyoes
    
    typedef struct       Audio3DContainer  Audio3DContainer;
    
    typedef struct       AudioVector       AudioVector;
    
    typedef struct       ImageContainer    ImageContainer; // Forward declare ContainerIO's tyoes
    
    typedef struct       MetadataContainer MetadataContainer;
    
    typedef struct       BitBuffer         BitBuffer;      // Forward declare BitIO's types
    
    typedef struct       OVIA              OVIA;
    
    OVIA                *OVIA_Init(void);
    OVIA_CodecIDs        OVIA_IdentifyFileType(OVIA *Ovia, BitBuffer *BitB);
    OVIA_CodecIDs        UTF8_Extension2CodecID(UTF8 *Extension);
    OVIA_CodecIDs        UTF16_Extension2CodecID(UTF16 *Extension);
    Audio2DContainer    *OVIA_ExtractSamples(BitBuffer *BitB, uint64_t MaxNumSamples);
    AudioVector         *OVIA_ExtractVector(BitBuffer *BitB);
    ImageContainer      *OVIA_ExtractFrame(BitBuffer *BitB); // Used for both image and video formats
    MetadataContainer   *OVIA_ExtractMetadata(BitBuffer *BitB);
    void                 OVIA_AppendSamples(Audio2DContainer *Audio, BitBuffer *BitB);
    void                 OVIA_AppendVector(AudioVector *Vector, BitBuffer *BitB);
    void                 OVIA_AppendFrame(ImageContainer *Frame, BitBuffer *BitB);
    void                 OVIA_AppendMetadata(MetadataContainer *Metadata, BitBuffer *BitB);
    void                 OVIA_WriteHeader(BitBuffer *BitB);
    void                 OVIA_Deinit(OVIA *Ovia);
    
#ifdef __cplusplus
}
#endif

#endif /* OVIA_libOVIA_H */
