#include "../../include/libModernFLAC.h"
#include "../../include/FLACTypes.h"

#pragma once

#ifndef LIBMODERNFLAC_DECODEFLAC_H
#define LIBMODERNFLAC_DECODEFLAC_H

#ifdef __cplusplus
extern "C" {
#endif
    
    
    // When decoding FLAC, give the caller the option to skip extracting the images, to help embedded devices reduce memory requirements.
    
    // I want 2 ways to handle this, the first is to just decode the whole file as quick as possible, and the second is for real time applications, which decodes all metadata, and a single frame.
    
    // for the 2nd use case, I need to make a few functions, one that reads all metadata, and another that reads a frame of audio at a time.
    // The fast as possible method will just loop through all the blocks
    
    
    
    
    
    
    // Well, in the library I need to just run through one frame of audio at a time.
    // So, let's set it all up so all the user metadata is in one struct, and all the audio data is in another.
    
    void    FLACReadFrame(BitInput *BitI, DecodeFLAC *Dec);
    
    void    FLACReadSubFrame(BitInput *BitI, DecodeFLAC *Dec, uint8_t Channel);
    
    void    FLACDecodeSubFrameVerbatim(BitInput *BitI, DecodeFLAC *Dec);
    
    void    FLACDecodeSubFrameConstant(BitInput *BitI, DecodeFLAC *Dec);
    
    void    FLACDecodeSubFrameFixed(BitInput *BitI, DecodeFLAC *Dec);
    
    void    FLACDecodeSubFrameLPC(BitInput *BitI, DecodeFLAC *Dec, uint8_t Channel);
    
    void    DecodeFLACesidual(BitInput *BitI, DecodeFLAC *Dec);
    
    void    DecodeFLACice1Partition(BitInput *BitI, DecodeFLAC *Dec);
    
    void    DecodeFLACice2Partition(BitInput *BitI, DecodeFLAC *Dec);
    
    void    FLACBitDepth(DecodeFLAC *Dec);
    
    void    FLACSampleRate(BitInput *BitI, DecodeFLAC *Dec);
    
    uint8_t GetBlockSizeInSamples(uint8_t BlockSize);
    
    void    FLACReadStream(BitInput *BitI, DecodeFLAC *Dec);
    
    void    FLACParseMetadata(BitInput *BitI, DecodeFLAC *Dec);
    
    void    FLACParseStreamInfo(BitInput *BitI, DecodeFLAC *Dec);
    
    void    FLACSkipPadding(BitInput *BitI, DecodeFLAC *Dec);
    
    void    FLACSkipCustom(BitInput *BitI, DecodeFLAC *Dec);
    
    void    FLACParseSeekTable(BitInput *BitI, DecodeFLAC *Dec);
    
    void    FLACParseVorbisComment(BitInput *BitI, DecodeFLAC *Dec);
    
    void    FLACParseCuesheet(BitInput *BitI, DecodeFLAC *Dec);
    
    void    FLACParsePicture(BitInput *BitI, DecodeFLAC *Dec);
    
#ifdef __cplusplus
}
#endif

#endif /* LIBMODERNFLAC_DECODEFLAC_H */
