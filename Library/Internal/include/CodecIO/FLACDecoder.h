#include "libModernFLAC.h"
#pragma once

#ifdef __cplusplus
extern "C" {
#endif
    
    
    // When decoding FLAC, give the caller the option to skip extracting the images, to help embedded devices reduce memory requirements.
    
    // I want 2 ways to handle this, the first is to just decode the whole file as quick as possible, and the second is for real time applications, which decodes all metadata, and a single frame.
    
    // for the 2nd use case, I need to make a few functions, one that reads all metadata, and another that reads a frame of audio at a time.
    // The fast as possible method will just loop through all the blocks
    
    
    
    
    
    
    // Well, in the library I need to just run through one frame of audio at a time.
    // So, let's set it all up so all the user metadata is in one struct, and all the audio data is in another.
    
    void    InitDecodeFLACFile(FLACFile *FLAC);
    
    void    FLACDecodeSubFrameLPC(BitInput *BitI, FLACFile *FLAC, uint8_t Channel);
    
    void    FLACDecodeSubFrameConstant(BitInput *BitI, FLACFile *FLAC);
    
    void    FLACReadSubFrame(BitInput *BitI, FLACFile *FLAC, uint8_t Channel);
    
    void    FLACDecodeRice1Partition(BitInput *BitI, FLACFile *FLAC);
    
    void    FLACDecodeRice2Partition(BitInput *BitI, FLACFile *FLAC);
    
    void    FLACReadFrame(BitInput *BitI, FLACFile *FLAC);
    
    uint8_t GetBlockSizeInSamples(uint8_t BlockSize);
    
    void    FLACBitDepth(FLACFile *FLAC);
    
    void    FLACSampleRate(BitInput *BitI, FLACFile *FLAC);
    
    
#ifdef __cplusplus
}
#endif
