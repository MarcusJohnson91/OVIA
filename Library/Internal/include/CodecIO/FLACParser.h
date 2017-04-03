#include "../../../Dependencies/libPCM/Dependencies/BitIO/libBitIO/include/BitIO.h"

#include "../libModernFLAC.h"
#include "../FLACTypes.h"

#pragma once

#ifndef LIBMODERNFLAC_PARSEFLAC_H
#define LIBMODERNFLAC_PARSEFLAC_H

#ifdef __cplusplus
extern "C" {
#endif
    
    void FLACParseMetadata(BitInput *BitI, DecodeFLAC *Dec);
    
    void FLACParseStreamInfo(BitInput *BitI, DecodeFLAC *Dec);
    
    void FLACSkipPadding(BitInput *BitI, DecodeFLAC *Dec);
    
    void FLACSkipCustom(BitInput *BitI, DecodeFLAC *Dec);
    
    void FLACParseSeekTable(BitInput *BitI, DecodeFLAC *Dec);
    
    void FLACParseVorbisComment(BitInput *BitI, DecodeFLAC *Dec);
    
    void FLACParseCuesheet(BitInput *BitI, DecodeFLAC *Dec);
    
    void FLACParsePicture(BitInput *BitI, DecodeFLAC *Dec);
    
    void FLACParsePicture(BitInput *BitI, DecodeFLAC *Dec);
    
#ifdef __cplusplus
}
#endif

#endif /* LIBMODERNFLAC_PARSEFLAC_H */
