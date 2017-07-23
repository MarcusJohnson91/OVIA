#include "../../../Dependencies/libPCM/Dependencies/BitIO/libBitIO/include/BitIO.h"

#include "../libModernFLAC.h"

#pragma once

#ifndef LIBMODERNFLAC_COMPOSEFLAC_H
#define LIBMODERNFLAC_COMPOSEFLAC_H

#ifdef __cplusplus
extern "C" {
#endif
    
    void   FLACWriteStreaminfo(BitBuffer *OutputFLAC, EncodeFLAC *Enc);
    
    void   FLACWriteVorbisComment(BitBuffer *OutputFLAC, EncodeFLAC *Enc);

#ifdef __cplusplus
}
#endif

#endif /* LIBMODERNFLAC_COMPOSEFLAC_H */
