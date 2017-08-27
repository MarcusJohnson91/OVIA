#include "../Common/ModernFLACTypes.h"

#pragma once

#ifndef LIBMODERNFLAC_ComposeModernFLAC_H
#define LIBMODERNFLAC_ComposeModernFLAC_H

#ifdef __cplusplus
extern "C" {
#endif
    
    void   FLACWriteStreaminfo(BitBuffer *OutputFLAC, EncodeFLAC *Enc);
    
    void   FLACWriteVorbisComment(BitBuffer *OutputFLAC, EncodeFLAC *Enc);

#ifdef __cplusplus
}
#endif

#endif /* LIBMODERNFLAC_ComposeModernFLAC_H */
