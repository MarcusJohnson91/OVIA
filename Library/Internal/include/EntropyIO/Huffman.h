//
//  Huffman.h
//  OVIA
//
//  Created by Marcus Johnson on 9/6/20.
//  Copyright © 2020 Marcus Johnson. All rights reserved.
//

#pragma once

#include "EntropyIO.h"

#ifndef OVIA_Huffman_h
#define OVIA_Huffman_h

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    /*
     We need to Standardize our Terminology.

     Huffman is composed of two numbers, one representing the encoded bitstring that is prefix free that represents the other number,
     the other number is the actual value that the bitstring number represents.
     */

    /*
     Optimal Canonical Huffman Code Length

     Ok, so we have X values to encode in our base-2 Huffman code.

     What is the shortest representation possible for X?

     let's say we need to encode 11 numbers.

     in normal binary you could represent 11 values in 4 bits, with 4 unused symbols.

     but in Huffman, you can not have ANY code be a prefix of another code.

     so the 1 bit codes 1 and 0 can not be used.
     what about 2 bit codes? 00, 01, 10, 11? i don't think that would work either but maybe.
     3 bit codes: 000, 001, 010, 011, 100, 101, 110, 111
     4 bit codes; 0000, 0001, 0010, 0011, 0100, 0101, 0110, 0111, 1000, 1001, 1010, 1011, 1100, 1101, 1110, 1111

     The easy way would be to just write it in ceil(sqrt(NumValues)) bits, but I think we can do better.

     What if we reserved half of each bitstring level for different codes?

     {000} = Odd Values
     {111} = Even values


     0          1
     0          0

     {0, 1}
     {00, 01, 10, 11}
     {000, 001, 010, 011, 100, 101, 110, 111}

     How many codes can we use without being a prefix of eachother?

     {111, 110, 101, 100, }

     ----

     Encode "Bad"
     A=1, B=1, D=1
     Codes = {A=0, B=10, D=11} # Can't do that, A=1 D could be AA or D, no way to tell which it is.
     A=0 might work tho, beause nothing is coded as just 1 or 100

     So we just discovered another implicit rule: A symbol can only be used if both sides of it are unused.

     Encoded string = {10011}

     All 1 bit prefixed codes are excluded as a prefix for longer codes.

     so {1, 11, 111, 1111, 11111, 111111, 1111111, 11111111, 111111111, 1111111111, 11111111111, 111111111111, 1111111111111, 1111111111111, 111111111111111, 1111111111111111} are all offlimits.

     
     */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif

#endif /* OVIA_Huffman_h */
