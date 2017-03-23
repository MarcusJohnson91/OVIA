#include "../libModernFLAC.h"

#pragma once

#ifndef LIBMODERNFLAC_ENCODEFLAC_H
#define LIBMODERNFLAC_ENCODEFLAC_H

#ifdef __cplusplus
extern "C" {
#endif
    
    int8_t EncodeFLACFile(PCMFile *PCM, BitOutput *BitO, EncodeFLAC *Enc);

#ifdef __cplusplus
}
#endif

#endif /* LIBMODERNFLAC_ENCODEFLAC_H */
