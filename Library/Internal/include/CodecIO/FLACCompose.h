#include "../Common/libModernFLAC_Types.h"

#pragma once

#ifndef libModernFLAC_Compose_H
#define libModernFLAC_Compose_H

#ifdef __cplusplus
extern "C" {
#endif
    
    void   FLACWriteStreaminfo(BitBuffer *OutputFLAC, EncodeFLAC *Enc);
    
    void   FLACWriteVorbisComment(BitBuffer *OutputFLAC, EncodeFLAC *Enc);

#ifdef __cplusplus
}
#endif

#endif /* libModernFLAC_Compose_H */
