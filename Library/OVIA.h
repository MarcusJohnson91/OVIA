/*!
 @header              OVIA.h
 @author              Marcus Johnson
 @copyright           2018+
 @SPDX-License-Identifier: Apache-2.0
 @version             0.3.7
 @brief               This header is for OVIA's public API.
 */

#pragma once

#ifndef OVIA_H
#define OVIA_H

#define OVIA_Version_Major 0
#define OVIA_Version_Minor 3
#define OVIA_Version_Patch 7

#include "../Dependencies/FoundationIO/Library/include/TextIO/TextIOTypes.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    /*
     OVIA Interface: User Perspective
     Read the command line arguments
     Open input and output streams
     identify the input file
     identify the requested action (encoding/muxing) to which codec/container.
     look through the registered decoders and encoders to find if we can fulfill the request.
     if we can, start doing it.
     
     OVIA Interface: External Developer Perspective: What is an External dev trying to do? Use OVIA to acomplish a task.
     Identify the input data and decide if we can reencode or remux it.
     Also, substream identification and extraction.
     (2 types of stream identification, sync code based which is just a few large constants, and actual IDs which are predefined integer constants, the is basically a list of values)
     
     OVIA Interface: Internal Developer Perspective:
     How do I add suport for MY codec, it's weird.
     Well, you use the various components of OVIA's internal skeleton to build your codec, you register your decoder/encoder as well as the frameing parameters for bot decoding, encoding, as well as muxing.
     and then you write the pieces you need with whats available, theres MD5, CRC32, Adler32, functions as well as generic entropy coders available for use.
     and if none of the generic stuff works you can just write your own with the same performance as the built in ones.
     
     So we need Audio and Image Containers, a Tag container, a Subtitle/Caption Container, as well as Entropy codecs, and most importantly I need a good representation of how the to abstract file/network sources, as well as how to abstract the actual data contained within.
     */
    
    /*
     The main problem with a Packet based architecture is that it doesn't fit into the stream architecture I'm trying to create.
     
     But what if we reverse it?
     
     Files get converted into frames, and network packets get accumulated into a frame it's self?
     
     How would that fit in with Container format encapsulation as well as codec substreams?
     
     Well, lets say theres a video codec with substreams, H.264 SVC video for example with 2 substreams, one 1080p and the other 480p.
     
     Then the H.264 video is interlaced with DTS-HD MA audio in 3 languages, English, Swedish, and German.
     
     and theres 3 subtitle formats as well, for the same languages.
     
     and all of this content is encoded in a MPEG Media Transport stream.
     
     We need to be able to parse the MMT header to find what is contained in the Payload as well as where the Payload is.
     
     and we'd need to be able to extract the payload into a buffer of fixed or growable? size.
     
     
     */
    
    /*
     Subtitles are a type of metadata or tag, they're just text linked to certain start and stop times.
     How does "bitstream filtering" (aka muxing/frame extraction/concatenation) fit in?
     Encoding and decoding both have two points of entry, the encoded form and the Container form.
     
     
     */
    
    /*
     OVIA needs the ability to open files, and to handle streams already in memory.
     
     So the memory version needs to take a pointer and size to a 1D array of bytes.
     
     the file version simply needs to create a file pointer, allocate memory for the frame, and copy the data in from the file to that memory address.
     
     then OVIA_Identify needs to just take a pointer and size the the memory and read the magic numbers from there.
     
     OVIA_ReadFile
     
     There's just one problem, and that is we need to support individual files like BMP and streaming files like APNG or AVC.
     
     So, the obvious solution is to treat this like a String, split out a file size function, and take that as a parameter.
     
     Ok but how do we handle formats like AVC which are completely variable with no landing marks at all?
     */
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
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
     
     Well then we need to have a function which calls the BMP decoder and returns an ImageCanvas with the decoded data.
     
     so what about:
     
     ImageCanvas *OVIA_DecodeImage(BitBuffer *BitB);
     
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
    
    
    
    typedef enum OVIA_CodecIDs : uint8_t {
        CodecID_Unspecified   = 0,
        CodecID_PCMAudio      = 1,
        CodecID_PCMImage      = 2,
        CodecID_FLAC          = 3,
        CodecID_BMP           = 4,
        CodecID_PNG           = 5,
        CodecID_PNM           = 6,
        CodecID_JPEG          = 7,
        CodecID_LastCodec     = CodecID_JPEG,
        OVIA_NumCodecs        = CodecID_LastCodec,
    } OVIA_CodecIDs;
    
    typedef enum OVIA_ContainerIDs : uint8_t {
        ContainerID_Unspecified = 0,
        ContainerID_RIFF        = 1,
        ContainerID_AIF         = 2,
        ContainerID_FLACNative  = 3,
        ContainerID_TIFF        = 4,
        ContainerIO_Matroska    = 5,
    } OVIA_ContainerIDs;
    
    typedef enum OVIA_ColorTransforms : uint8_t {
        ColorTransform_Unspecified = 0,
        ColorTransform_RCT         = 1, // JPEG-2000 Reversible Color Transform
        ColorTransform_YCoCgR      = 2, // AVC, HEVC Lossless Transform
        OVIA_NumTransforms         = ColorTransform_YCoCgR,
    } OVIA_ColorTransforms;
    
    typedef enum OVIA_MediaTypes : uint8_t {
        MediaType_Unspecified = 0,
        MediaType_Container   = 1,
        MediaType_Audio2D     = 2,
        MediaType_Audio3D     = 3,
        MediaType_Image       = 4,
        MediaType_Video       = 5,
        MediaType_Caption     = 6,
    } OVIA_MediaTypes;

    /* Forward declarations from FoundationIO */
    typedef struct       BitBuffer         BitBuffer;
    /* Forward declarations from FoundationIO */

    /* Forward declarations from MediaIO */
    typedef struct       AudioScape2D  AudioScape2D; // Forward declare ContainerIO's tyoes

    typedef struct       AudioScape3D  AudioScape3D;

    typedef struct       AudioVector       AudioVector;

    typedef struct       ImageCanvas    ImageCanvas; // Forward declare MediaIO's tyoes

    typedef struct       MetadataContainer MetadataContainer;
    /* Forward declarations from MediaIO */
    
    typedef struct       OVIA              OVIA;
    
    OVIA                *OVIA_Init(void);
    OVIA_CodecIDs        OVIA_IdentifyFileType(OVIA *Ovia, BitBuffer *BitB);
    AudioScape2D    *OVIA_ExtractSamples(BitBuffer *BitB, uint64_t MaxNumSamples);
    AudioVector         *OVIA_ExtractVector(BitBuffer *BitB);
    ImageCanvas      *OVIA_ExtractFrame(BitBuffer *BitB); // Used for both image and video formats
    MetadataContainer   *OVIA_ExtractMetadata(BitBuffer *BitB);
    void                 OVIA_AppendSamples(AudioScape2D *Audio, BitBuffer *BitB);
    void                 OVIA_AppendVector(AudioVector *Vector, BitBuffer *BitB);
    void                 OVIA_AppendFrame(ImageCanvas *Frame, BitBuffer *BitB);
    void                 OVIA_AppendMetadata(MetadataContainer *Metadata, BitBuffer *BitB);
    void                 OVIA_WriteHeader(BitBuffer *BitB);
    void                 OVIA_Deinit(OVIA *Ovia);

    /*
     Sooo....

     A file is passed to OVIA, the signature is read, demuxers and extension stuff get involved in figuring out what to do with this file...

     we need to be able to extract frames from video/images, as well as extract samples from audio.

     audio codecs are limited to frames of X sample clusters that must be extract at once.

     audio streams the number of extractable samples at once...
     */

    /* TODO: NEW DESIGN FOR OVIA
     So, we need to discover at runtime which codecs, streams, etc are available via reading the CodecIO, SteamIO, etc sections of our binary just like TestIO does.

     Our Identify Stream/Codec functions are called which loop over the MagicIDs checking signatures.

     The corect decoder/demuxer are called to start processing the data via the registered function pointers.

     We then identify the requested output format, if one is specified by it's extension on the command line.

     the corect encoder/muxer is called with the AudioScape2D/ImageCanvas as it's source.
     */
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_H */
