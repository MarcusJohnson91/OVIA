#include <stdbool.h>
#include <stdint.h>

#pragma once

#ifndef LIBMODERNPNG_SHARED_H
#define LIBMODERNPNG_SHARED_H

#ifdef __cplusplus
extern "C" {
#endif
    
    uint32_t CalculateSTERPadding(const uint32_t Width);
    
#ifdef __cplusplus
}
#endif

#endif /* LIBMODERNPNG_SHARED_H */
