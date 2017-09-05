#include "../../libModernFLAC.h"

#pragma once

#ifndef LIBMODERNFLAC_ModernFLACCommon_H
#define LIBMODERNFLAC_ModernFLACCommon_H

#ifdef __cplusplus
extern "C" {
#endif
    
    inline int64_t OnesCompliment2TwosCompliment(const int64_t OnesCompliment);
    
    inline int64_t TwosCompliment2OnesCompliment(const int64_t TwosCompliment);
    
#ifdef __cplusplus
}
#endif

#endif /* LIBMODERNFLAC_ModernFLACCommon_H */
