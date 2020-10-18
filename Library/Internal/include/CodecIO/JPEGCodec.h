/*!
 @header              JPEGCodec.h
 @author              Marcus Johnson
 @copyright           2020+
 @version             1.0.0
 @brief               This header contains code for JPEG (encoding and decoding).
 */

#include "CodecIO.h"
#include "../EntropyIO/Arithmetic.h"
#include "../EntropyIO/CanonicalHuffman.h"

#pragma once

#ifndef OVIA_JPEGCodec_H
#define OVIA_JPEGCodec_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    /* !!!ONLY LOSSLESS JPEG CODECS WILL EVER BE SUPPORTED!!! */

    typedef enum JPEG_Markers {
        JPEGMarker_StartOfImage           = 0xFFD8, // SOI, Magic
        JPEGMarker_StartOfFrameLossless1  = 0xFFC3, // SOF3, Start of Frame, Lossless, non-differential, Huffman
        JPEGMarker_StartOfFrameLossless2  = 0xFFC7, // SOF7, Start of Frame, Lossless, Differential, Huffman
        JPEGMarker_StartOfFrameLossless3  = 0xFFCB, // SOF11, Start of Frame, Lossless, non-differential, Arithmetic
        JPEGMarker_StartOfFrameLossless4  = 0xFFCF, // SOF15, Start of Frame, Differential, Arithmetic
        JPEGMarker_DefineHuffmanTable     = 0xFFC4, // DHT
        JPEGMarker_DefineArthimeticTable  = 0xFFCC, // DAC
        JPEGMarker_StartOfScan            = 0xFFDA, // SOS
        JPEGMarker_NumberOfLines          = 0xFFDC, // DNL
        JPEGMarker_JFIF                   = 0xFFE0, // APP0
        JPEGMarker_EXIF                   = 0xFFE1, // APP1
        JPEGMarker_ICCProfile             = 0xFFE2, // APP2
        JPEGMarker_StereoImage            = 0xFFE3, // APP3
        JPEGMarker_Comment                = 0xFFFE, // COM, NULL terminated
        JPEGMarker_Extension7             = 0xFFF7, // JPG7/SOF48
        JPEGMarker_RestartInterval        = 0xFFDD, // DRI
        JPEGMarker_Restart0               = 0xFFD0, // RST0
        JPEGMarker_Restart1               = 0xFFD1, // RST1
        JPEGMarker_Restart2               = 0xFFD2, // RST2
        JPEGMarker_Restart3               = 0xFFD3, // RST3
        JPEGMarker_Restart4               = 0xFFD4, // RST4
        JPEGMarker_Restart5               = 0xFFD5, // RST5
        JPEGMarker_Restart6               = 0xFFD6, // RST6
        JPEGMarker_Restart7               = 0xFFD7, // RST7
        JPEGMarker_EndOfImage             = 0xFFD9, // EOI
    } JPEG_Markers;

    typedef enum JPEG_Predictors {
        JPEGPredictor_Unknown = 0,
        JPEGPredictor_1       = 1, // Predicted = Previous Sample: to the Left
        JPEGPredictor_2       = 2, // Predicted = Previous Sample: Above
        JPEGPredictor_3       = 3, // Predicted = Previous Sample: Diagonal, Above and Left
        JPEGPredictor_4       = 4, // Predicted = Previous Sample: (_1 + _2) - _3
        JPEGPredictor_5       = 5, // Predicted =  _1 + ((_2 – _3)/2)>>A)
        JPEGPredictor_6       = 6, // Predicted =  _2 + ((_1 – _3)/2)>>A)
        JPEGPredictor_7       = 7, // Predicted =  (_1+_2) / 2
    } JPEG_Predictors;

    typedef struct JPEG_ChannelParameters {
        uint8_t ChannelID;
        uint8_t HorizontalSF;  // Sampling Factor
        uint8_t VerticalSF;    // Sampling Factor
    } JPEG_ChannelParameters;

    typedef enum JPEG_EntropyCoders {
        EntropyCoder_Unknown    = 0,
        EntropyCoder_Huffman    = 1,
        EntropyCoder_Arithmetic = 2,
    } JPEG_EntropyCoders;

    typedef enum JPEG_CodingMode {
        CodingMode_Unknown          = 0,
        CodingMode_Hierarchical     = 1,
        CodingMode_NonHierarchical  = 2,
    } JPEG_CodingMode;

    typedef struct HuffmanValue {
        uint16_t HuffCode; // the actual BitString
        uint8_t  BitLength;
        uint8_t  Symbol;  // What the BitSting represents
    } HuffmanValue;

    typedef struct JPEGHuffman {
        HuffmanValue **Values; // [TableID][Value]
        uint16_t      *NumValues; // {TableSize0 = X, TableSize1 = Y}
    } JPEGHuffman;

    /*
    typedef struct JPEGHuffman {

        uint16_t Codes[2][256];   // the Huffman strings to replace
        uint8_t  Symbols[2][256]; // encoded value
        uint16_t TableSize;
        uint8_t  MinBits; // 2
        uint8_t  MaxBits; // 9
    } JPEGHuffman;
     */

    typedef struct JPEGOptions {
        JPEG_ChannelParameters *Channels;
        JPEGHuffman            *Huffman;
        // Huffman and Arthimetic states
        CharSet8               *Comment;
        uint16_t                Width;
        uint16_t                Height;
        uint16_t                RestartInterval; // in Minimum Coding Units
        uint16_t                CommentSize;
        uint16_t                HeaderSize;
        uint8_t                 BitDepth;
        uint8_t                 NumChannels;
        JPEG_EntropyCoders      EntropyCoder;
        JPEG_CodingMode         CodingMode;
    } JPEGOptions;

    /*

     Marker Fomat:

     Marker        = 16 bits
     LengthInBytes = 16 bits (includes the 2 byte length field

     SOF = frame header
     SOS = scan header

     Markers are padded with 0xFF

     Entropy compressed markers must be stuffed with 0x00 after every 0xFF byte

     Huffman bytes are padded with 1 bits to finish the byte


     */

    // Default Huffman Tables:

    void           *JPEGOptions_Init(void);

    void            JPEG_Parse(void *Options, BitBuffer *BitB);

    ImageContainer *JPEG_Extract(void *Options, BitBuffer *BitB);

    void            JPEGOptions_Deinit(void *Options);

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_JPEGCodec_H */
