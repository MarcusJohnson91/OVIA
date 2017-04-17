#include <stdint.h>

#pragma once

#ifndef LIBMODERNPNG_TABLES_H
#define LIBMODERNPNG_TABLES_H

#ifdef __cplusplus
extern "C" {
#endif
    
    static const uint8_t Adam7Level5[8] = {
        17, 19, 21, 23, 49, 51, 53, 55
    };
    
    static const uint8_t Adam7Level6[16] = {
         2,  4,  6,  8, 18, 20, 22, 24,
        34, 36, 38, 40, 50, 52, 54, 56,
    };
    
    static const uint8_t Adam7Level7[32] = {
         9, 10, 11, 12, 13, 14, 15, 16,
        25, 26, 27, 28, 29, 30, 31, 32,
        41, 42, 43, 44, 45, 46, 47, 48,
        57, 58, 59, 60, 61, 62, 63, 64
    };
    
    enum Adam7Positions {
        Adam7Level1    =  0,
        Adam7Level2    =  5,
        Adam7Level3_1  = 32,
        Adam7Level3_2  = 36,
        Adam7Level4_1  =  3,
        Adam7Level4_2  =  7,
        Adam7Level4_3  = 35,
        Adam7Level4_4  = 39,
        Adam7Level5_1  = 17,
        Adam7Level5_2  = 19,
        Adam7Level5_3  = 21,
        Adam7Level5_4  = 23,
        Adam7Level5_5  = 49,
        Adam7Level5_6  = 51,
        Adam7Level5_7  = 53,
        Adam7Level5_8  = 55,
        Adam7Level6_1  =  2,
        Adam7Level6_2  =  4,
        Adam7Level6_3  =  6,
        Adam7Level6_4  =  8,
        Adam7Level6_5  = 18,
        Adam7Level6_6  = 20,
        Adam7Level6_7  = 22,
        Adam7Level6_8  = 24,
        Adam7Level6_9  = 34,
        Adam7Level6_10 = 36,
        Adam7Level6_11 = 38,
        Adam7Level6_12 = 40,
        Adam7Level6_13 = 50,
        Adam7Level6_14 = 52,
        Adam7Level6_15 = 54,
        Adam7Level6_16 = 56,
        Adam7Level7_1  = 9,
        Adam7Level7_2  = 10,
        Adam7Level7_3  = 11,
        Adam7Level7_4  = 12,
        Adam7Level7_5  = 13,
        Adam7Level7_6  = 14,
        Adam7Level7_7  = 15,
        Adam7Level7_8  = 16,
        Adam7Level7_9  = 25,
        Adam7Level7_10 = 26,
        Adam7Level7_11 = 27,
        Adam7Level7_12 = 28,
        Adam7Level7_13 = 29,
        Adam7Level7_14 = 30,
        Adam7Level7_15 = 31,
        Adam7Level7_16 = 32,
        Adam7Level7_17 = 41,
        Adam7Level7_18 = 42,
        Adam7Level7_19 = 43,
        Adam7Level7_20 = 44,
        Adam7Level7_21 = 45,
        Adam7Level7_22 = 46,
        Adam7Level7_23 = 47,
        Adam7Level7_24 = 48,
        Adam7Level7_25 = 57,
        Adam7Level7_26 = 58,
        Adam7Level7_27 = 59,
        Adam7Level7_28 = 60,
        Adam7Level7_29 = 61,
        Adam7Level7_30 = 62,
        Adam7Level7_31 = 63,
        Adam7Level7_32 = 64,
    };
    
#ifdef __cplusplus
}
#endif

#endif /* LIBMODERNPNG_TABLES_H */
