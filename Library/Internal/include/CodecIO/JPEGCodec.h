#include "OVIACommon.h"

#pragma once

#ifndef OVIA_JPEGCommon_H
#define OVIA_JPEGCommon_H

#ifdef __cplusplus
extern "C" {
#endif
    
    /* !!!ONLY LOSSLESS JPEG CODECS WILL EVER BE SUPPORTED!!! */
    
    typedef enum JPEG_Markers {
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
        Predictor_1       = 1, // PredictedX = RA
        Predictor_2       = 2, // PredictedX = RB
        Predictor_3       = 3, // PredictedX = RC
        Predictor_4       = 4, // PredictedX = RA + RB - RC
        Predictor_5       = 5, // PredictedX = Ra + ((Rb – Rc)/2)>>A)
        Predictor_6       = 6, // PredictedX = Rb + ((Ra – Rc)/2)>>A)
        Predictor_7       = 7, // PredictedX = (Ra+Rb) / 2
    } JPEG_Predictors;
    
    typedef struct ComponentParameters {
        uint8_t ComponentID;
        uint8_t Horizontal;  // Sampling Factor
        uint8_t Vertical;    // Sampling Factor
    } ComponentParameters;
    
    typedef struct HuffmanTable {
        uint8_t  *NumBits; // Values[1..3] = 4, 5, 6
        uint8_t   BitLengths[16];
        uint8_t   TableClass;
        uint8_t   TableID;
        uint16_t  EndOfBlockCode;
    } HuffmanTable;
    
    typedef struct ArithmeticTable {
        uint16_t CodeLength;
        uint8_t  TableClass;
        uint8_t  TableDestination;
        uint8_t  CodeValue;
    } ArithmeticTable;
    
    typedef struct HuffmanNode {
        
    } HuffmanNode;
    
    typedef struct HuffmanTree {
        HuffmanNode *Left;  // 0
        HuffmanNode *Right; // 1
    } HuffmanTree;
    
    typedef enum JPEG_EntropyCoders {
        EntropyCoder_Unknown    = 0,
        EntropyCoder_Huffman    = 1,
        EntropyCoder_Arithmetic = 2,
    } JPEG_EntropyCoders;
    
    typedef struct JPEGOptions {
        ComponentParameters *Components;
        HuffmanTable        *Huffman;
        ArithmeticTable     *Arithmetic;
        uint16_t             Width;
        uint16_t             Height;
        uint16_t             RestartInterval; // in Minimum Coding Units
        uint8_t              BitDepth;
        uint8_t              NumChannels;
        JPEG_EntropyCoders   EntropyCoder; //
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
    
#ifdef __cplusplus
}
#endif

#endif /* OVIA_JPEGCommon_H */
