#include "../Common/libModernFLAC_Types.h"

#pragma once

#ifndef libModernFLAC_Parse_H
#define libModernFLAC_Parse_H

#ifdef __cplusplus
extern "C" {
#endif
    
    void FLACParseMetadata(BitBuffer *InputFLAC, DecodeFLAC *Dec);
    
    void FLACParseStreamInfo(BitBuffer *InputFLAC, DecodeFLAC *Dec);
    
    void FLACSkipPadding(BitBuffer *InputFLAC, DecodeFLAC *Dec);
    
    void FLACSkipCustom(BitBuffer *InputFLAC, DecodeFLAC *Dec);
    
    void FLACParseSeekTable(BitBuffer *InputFLAC, DecodeFLAC *Dec);
    
    void FLACParseVorbisComment(BitBuffer *InputFLAC, DecodeFLAC *Dec);
    
    void FLACParseCuesheet(BitBuffer *InputFLAC, DecodeFLAC *Dec);
    
    void FLACParsePicture(BitBuffer *InputFLAC, DecodeFLAC *Dec);
    
    void FLACParsePicture(BitBuffer *InputFLAC, DecodeFLAC *Dec);
    
#ifdef __cplusplus
}
#endif

#endif /* libModernFLAC_Parse_H */
