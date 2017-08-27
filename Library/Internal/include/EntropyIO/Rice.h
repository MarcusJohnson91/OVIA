#include "../../libModernFLAC.h"

#pragma once

#ifndef LIBMODERNFLAC_ModernFLACEntropyCoders_H
#define LIBMODERNFLAC_ModernFLACEntropyCoders_H

#ifdef __cplusplus
extern "C" {
#endif
    
    /*!
     @abstract Generates MD5 hash from Buffer data.
     @param PadData if set to one, we will pad each block, if set to 0 we won't.
     */
    char *GenerateMD5(uint8_t *Buffer, size_t BufferSize, bool PadData, char *MD5String);
    
    bool VerifyMD5(uint8_t *Buffer, size_t BufferSize, bool PadData, char *MD5String, char *PrecomputedMD5);
    
#ifdef __cplusplus
}
#endif

#endif /* LIBMODERNFLAC_ModernFLACEntropyCoders_H */
