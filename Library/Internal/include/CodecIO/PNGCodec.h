#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include "../../Dependencies/libPCM/Dependencies/FoundationIO/libFoundationIO/include/BitIO.h"
#include "../../Dependencies/libPCM/Dependencies/FoundationIO/libFoundationIO/include/StringIO.h"

#pragma once

#ifndef libModernPNG_H
#define libModernPNG_H

#ifdef __cplusplus
extern "C" {
#endif
    
#define PNGMagic 0x89504E470D0A1A0A
    
    enum libModernPNGChunkMarkers {
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
    };
    
    typedef enum PNGColorTypes {
        PNG_Grayscale      = 0,
        PNG_RGB            = 2,
        PNG_PalettedRGB    = 3,
        PNG_GrayAlpha      = 4,
        PNG_RGBA           = 6,
    } PNGColorTypes;
    
    typedef enum libModernPNGFilterTypes {
        NotFiltered   = 0,
        SubFilter     = 1,
        UpFilter      = 2,
        AverageFilter = 3,
        PaethFilter   = 4,
    } libModernPNGFilterTypes;
    
    enum libModernPNGInterlaceType {
        PNGNotInterlaced   = 0,
        PNGInterlacedAdam7 = 1,
    };
    
    static const char ModernPNGNumber2Month[12][3] = {
        U8("Jan"), U8("Feb"), U8("Mar"), U8("Apr"), U8("May"), U8("Jun"),
        U8("Jul"), U8("Aug"), U8("Sep"), U8("Oct"), U8("Nov"), U8("Dec"),
    };
    
    static const uint8_t ModernPNGChannelsPerColorType[7] = {
        1, 0, 3, 3, 4, 0, 4
    };
    
    uint32_t    CalculateSTERPadding(const uint32_t Width);
    
    typedef struct PNGDecoder DecodePNG;
    
    typedef struct PNGEncoder EncodePNG;
    
    /*!
     @abstract                  "Initializes the DecodePNG structure to start decoding this specific PNG file"
     @return                    "It takes no parameters, and returns a pointer to the PNGDecoder (typedef'd as DecodePNG) structure"
     */
    DecodePNG  *DecodePNG_Init(void);
    
    /*!
     @abstract                  "Initializes the EncodePNG structure to start encoding this specific PNG file"
     @return                    "It takes no parameters, and returns a pointer to the PNGEncoder (typedef'd as EncodePNG) structure"
     */
    EncodePNG  *EncodePNG_Init(void);
    
    /*!
     @abstract                  "Uninitializes the PNGDecoder (typedef'd as DecodePNG) structure after you're done decoding this specific PNG file"
     */
    void        DecodePNG_Deinit(DecodePNG *Dec);
    
    /*!
     @abstract                  "Uninitializes the PNGEncoder (typedef'd as EncodePNG) structure after you're done decoding this specific PNG file"
     */
    void        EncodePNG_Deinit(EncodePNG *Enc);
    
    void        DecodePNG_Text_Init(DecodePNG *Dec, uint8_t KeywordSize, uint32_t CommentSize);
    
    void        EncodePNG_Text_Init(EncodePNG *Enc, uint8_t KeywordSize, uint32_t CommentSize);
    
    /*!
     @abstract                  "Encodes a PNG from RawImage2Encode to a BitBuffer"
     @param     Enc             "Pointer to EncodePNG struct containing all the metadata about the image to be encoded"
     @param     RawImage2Encode "Pointer to raw array containing the image, supports 2D array containing stereoscopic frames"
     @param     InterlacePNG    "Should this PNG file be interlaced using the Adam7 algorithm for progressive download?"
     @param     OptimizePNG     "Should this PNG file be optimized by trying all filters? (Huffman optimization is enabled by default)"
     */
    BitBuffer  *EncodePNGImage(EncodePNG *Enc, void ****RawImage2Encode, bool InterlacePNG, bool OptimizePNG);
    
    /*!
     @abstract                  "Decodes a PNG from a bitbuffer to an array"
     @param     Dec             "Pointer to DecodePNG struct containing all the metadata about the image to be decoded"
     @param     PNGFile         "Pointer to raw array containing the image, supports 2D array containing stereoscopic frames"
     */
    uint16_t ***DecodePNGImage(DecodePNG *Dec, BitBuffer *PNGFile);
    
    void        PNGSetTextChunk(EncodePNG *Enc, UTF8 *KeywordString, UTF8 *CommentString);
    
    uint32_t    PNGGetNumTextChunks(DecodePNG *Dec);
    
    /*!
     @abstract                  "Extracts the Keyword and Comment strings from the Instance of the text chunk".
     @param     Dec             "DecodePNG Pointer to extract the text chunk from".
     @param     Instance        "Which instance of the text chunk should we extract"?
     @param     Keyword         "Pointer the Keyword string is returned through".
     @param     Comment         "Pointer the Comment string is returned through".
     */
    void        PNGGetTextChunk(DecodePNG *Dec, uint32_t Instance, UTF8 *Keyword, UTF8 *Comment);
    
    uint32_t    GetPNGWidth(DecodePNG *Dec);
    
    uint32_t    GetPNGHeight(DecodePNG *Dec);
    
    uint8_t     GetPNGBitDepth(DecodePNG *Dec);
    
    uint8_t     GetPNGColorType(DecodePNG *Dec);
    
    bool        GetPNGInterlaceStatus(DecodePNG *Dec);
    
    bool        IsPNGStereoscopic(DecodePNG *Dec);
    
    uint32_t    GetPNGWhitepointX(DecodePNG *Dec);
    
    uint32_t    GetPNGWhitepointY(DecodePNG *Dec);
    
    uint32_t    GetPNGGamma(DecodePNG *Dec);
    
    const char *GetPNGColorProfileName(DecodePNG *Dec);
    
    uint8_t    *GetColorProfile(DecodePNG *Dec);

#ifdef __cplusplus
}
#endif

#endif /* libModernPNG_H */
