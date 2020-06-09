/*!
 @header              JPEGCommon.h
 @author              Marcus Johnson
 @copyright           2020+
 @version             1.0.0
 @brief               This header contains code for reading and writing lossless JPEG image files
 */

#include "OVIACommon.h"

#pragma once

#ifndef OVIA_JPEGCommon_H
#define OVIA_JPEGCommon_H

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    /* !!!ONLY LOSSLESS JPEG CODECS WILL EVER BE SUPPORTED!!! */
    
    typedef enum JPEGCommon {
        NumBitCounts = 16,
    } JPEGCommon;
    
    typedef enum JPEG_Markers {
        // Size-less markers: StartOfImage, EndOfImage, Restart0..7
        Marker_StartOfImage           = 0xFFD8, // SOI, Magic
        Marker_StartOfFrameLossless1  = 0xFFC3, // SOF3, Start of Frame, Lossless, non-differential, Huffman
        Marker_StartOfFrameLossless2  = 0xFFC7, // SOF7, Start of Frame, Lossless, Differential, Huffman
        Marker_StartOfFrameLossless3  = 0xFFCB, // SOF11, Start of Frame, Lossless, non-differential, Arithmetic
        Marker_StartOfFrameLossless4  = 0xFFCF, // SOF15, Start of Frame, Differential, Arithmetic
        Marker_DefineHuffmanTable     = 0xFFC4, // DHT
        Marker_DefineArthimeticTable  = 0xFFCC, // DAC
        Marker_StartOfScan            = 0xFFDA, // SOS
        Marker_NumberOfLines          = 0xFFDC, // DNL
        Marker_JFIF                   = 0xFFE0, // APP0
        Marker_EXIF                   = 0xFFE1, // APP1
        Marker_ICCProfile             = 0xFFE2, // APP2
        Marker_StereoImage            = 0xFFE3, // APP3
        Marker_Comment                = 0xFFFE, // COM
        Marker_Extension7             = 0xFFF7, // JPG7/SOF48
        Marker_RestartInterval        = 0xFFDD, // DRI
        Marker_Restart0               = 0xFFD0, // RST0
        Marker_Restart1               = 0xFFD1, // RST1
        Marker_Restart2               = 0xFFD2, // RST2
        Marker_Restart3               = 0xFFD3, // RST3
        Marker_Restart4               = 0xFFD4, // RST4
        Marker_Restart5               = 0xFFD5, // RST5
        Marker_Restart6               = 0xFFD6, // RST6
        Marker_Restart7               = 0xFFD7, // RST7
        Marker_EndOfImage             = 0xFFD9, // EOI
    } JPEG_Markers;
    
    typedef enum JPEG_Predictors {
        Predictor_Unknown = 0,
        Predictor_0       = 1, // No Prediction
        Predictor_1       = 2, // PredictedX = RA
        Predictor_2       = 3, // PredictedX = RB
        Predictor_3       = 4, // PredictedX = RC
        Predictor_4       = 5, // PredictedX = RA + RB - RC
        Predictor_5       = 6, // PredictedX = Ra + ((Rb – Rc)/2)>>A)
        Predictor_6       = 7, // PredictedX = Rb + ((Ra – Rc)/2)>>A)
        Predictor_7       = 8, // PredictedX = (Ra+Rb) / 2
    } JPEG_Predictors;
    
    typedef struct ComponentParameters {
        uint8_t ComponentID;
        uint8_t Horizontal;  // Sampling Factor
        uint8_t Vertical;    // Sampling Factor
    } ComponentParameters;
    
    typedef struct HuffmanTable {
        uint16_t **CodeValue; // Codes are the Huffman value in the stream, Values are their meanings
        uint16_t   NumCodes;
        uint8_t  *NumBits;      // Values[1..3] = 4, 5, 6
        uint8_t   TableID;      // 0 = Luma, 1 = Chroma
        uint16_t  EndOfBlockCode;
    } HuffmanTable;
    
    typedef struct HuffmanKey {
        uint16_t Symbol; // The actual Huffman code written
        uint8_t  Value;  // The number the Huffman symbol represents
    } HuffmanKey;
    
    typedef struct HuffmanTable2 {
        //uint8_t      NumBitLengthCounts; // Always 16
        uint8_t      TableClass;
        uint8_t      TableID;
        HuffmanKey **Keys; // Keys[0][X], 0 = BitLength1, X = Key Number
        /*
         
         Table[BitLength] = [BitLength]{}
         
         BitLength[2] = 3 Keys
         
         How do we store an array of keys tho, NULL terminated array of Arrays?
         
         
         
         */
    } HuffmanTable2;
    
    typedef struct HuffmanTable3 {
        // Just the Code and the Value
        uint8_t  *BitLengthCounts;
        uint16_t *Codes;
        uint8_t  *Values;
        uint8_t   TableID;
    } HuffmanTable3;
    
    typedef struct ArithmeticTable {
        uint16_t CodeLength;
        uint8_t  TableClass;
        uint8_t  TableDestination;
        uint8_t  CodeValue;
    } ArithmeticTable;
    
    typedef enum JPEG_CompressionModes {
        CompressionMode_Unknown  = 0,
        CompressionMode_Lossy    = 1, // Unsupported
        CompressionMode_Lossless = 2,
    } JPEG_CompressionModes;
    
    typedef enum JPEG_EntropyCoders {
        EntropyCoder_Unknown    = 0,
        EntropyCoder_Huffman    = 1,
        EntropyCoder_Arithmetic = 2,
    } JPEG_EntropyCoders;
    
    typedef struct JPEGOptions {
        ComponentParameters  *Components;
        HuffmanTable3       **Huffman;
        //HuffmanKey         **Keys;
        //HuffmanTable       **Huffman;
        ArithmeticTable      *Arithmetic;
        uint16_t              Width;
        uint16_t              Height;
        uint16_t              RestartInterval; // in Minimum Coding Units
        uint16_t              SamplesPerLine;
        uint8_t               HuffmanTableCount;
        uint8_t               BitDepth;
        uint8_t               NumChannels;
        uint8_t               Predictor;
        uint8_t               SE; // Not sure
        uint8_t               Ah; // Not sure
        uint8_t               Al; // Not sure
        uint8_t               ComponentSelector;
        uint8_t               PointTransform;
        JPEG_EntropyCoders    EntropyCoder;
        JPEG_CompressionModes Mode;
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
    
    void            JPEG_ReadSegments(void *Options, BitBuffer *BitB);
    
    void           *JPEG_Extract(void *Options, BitBuffer *BitB);
    
    void            JPEG_BuildTable(JPEGOptions *JPEG, uint8_t TableID, uint8_t *BitLengthCounts, uint8_t NumValues, uint8_t *Values);
    
    void            JPEGWriteHeader(void *Options, BitBuffer *BitB);
    
    void            JPEGWriteImage(void *Options, void *Container, BitBuffer *BitB);
    
    void            JPEGWriteFooter(void *Options, BitBuffer *BitB);
    
    void            JPEGOptions_Deinit(void *Options);
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_JPEGCommon_H */
