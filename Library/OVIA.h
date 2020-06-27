#include "../../Dependencies/FoundationIO/Library/include/PlatformIO.h"
#include "../../Dependencies/FoundationIO/Library/include/UnicodeIO/UnicodeIOTypes.h"

#pragma once

#ifndef OVIA_OVIA_H
#define OVIA_OVIA_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    /*
     OVIA Refactor:
     
     I want OVIA to be better modularized and simplified by making it more table based.
     
     So codecs and streams (mux/demuxer) all deal with MagicIDs, chunk ids, with byte order/bit order as well as alignment issues; they're very similar in that regard so that stuff can be easily dealth with using the same code over and over and it should be too.
     
     Also should I move FoundationIO's ContainerIO to OVIA, and should I rename ContainerIO to MediaIO?
     
     ContainerIO really has nothing to do with FoundationIO.
     
     It's not used by anything else in FoundationIO, and it doesn't really rely on anything in FoundationIO except for basic PlatformIO and MathIO.
     
     Oh and Logging, all of which is used with OVIA so it really doesn't effect anything here.
     
     The only reason ContainerIO was part of FoundationIO was so it could be common to the seperate projects that made up OVIA; libPCM, ModernFLAC, and ModernPNG.
     
     As for renaming ContainerIO, what does ContainerIO really provide? ContainerIO abstracts the representation of various PCM multimmedia types and functions to operate on those types.
     
     Maybe ContainerIO should be a first class citizen of OVIA?
     
     What about MediaIO as a new name for ContainerIO? it makes sense but the theme is too FoundationIO-esque.
     
     as for the new compoments of OVIA, Codecs and Streams, we could go for CodecIO and StreamIO, but again i think it's too FoundationIO-esque.
     
     so the CodecIO and StreamIO and MediaIO could be the headers for the internal library, with the OVIA header exposing the public interface and those 3 exposing the private interface.
     
     But what would we name the actual stream and codec implementations?
     
     and if I do copy over ContainerIO I should just do a basic move, no history rewrite.
     
     Oh and a header, MetaIO.h which would be for Metadata conversion as well, for like tagging.
     
     */
    
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
        CodecID_PCMAudio      = 1,
        CodecID_PCMImage      = 2,
        CodecID_FLAC          = 4,
        CodecID_BMP           = 5,
        CodecID_PNG           = 6,
        CodecID_PNM           = 7,
        CodecID_JPEG          = 8,
        CodecID_LastCodec     = CodecID_JPEG,
        OVIA_NumCodecs        = CodecID_LastCodec,
    } OVIA_CodecIDs;
    
    typedef enum OVIA_ContainerIDs {
        ContainerID_Unknown = 0,
        ContainerID_RIFF    = 1,
        ContainerID_AIF     = 2,
    } OVIA_ContainerIDs;
    
    typedef enum OVIA_ColorTransforms {
        ColorTransform_Unknown = 0,
        ColorTransform_RCT     = 1, // JPEG-2000 Reversible Color Transform
        ColorTransform_YCoCgR  = 2, // AVC, HEVC Lossless Transform
        OVIA_NumTransforms     = ColorTransform_YCoCgR,
    } OVIA_ColorTransforms;
    
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
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_OVIA_H */
