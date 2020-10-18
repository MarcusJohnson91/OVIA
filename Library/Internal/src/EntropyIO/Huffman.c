#include "../../include/Private/EntropyIO/EntropyIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif



    /*
     Ok, so we take in a histogram saying the symbol probabilities.

     We then need to assign Codes (bitstrings) to each value.

     So, we need to know the number of symbols we need to encode, we get that by looping over the histogram counting every value greater than 0.

     which means we need a generic histogram function, one that takes a void array and has a parameter for the number of elements in the array, and the element size.

     Which brings up another issue: MediaIO's histograms operate on type, Huffman only encodes bytes though, so you need to do some weird wrap thing there.

     So, yeah what else do we need to create an optimal huffman tree?

     Really just the histogram, and what our histograms need is to have the ability to be mismatched.

     so you can create a histogram of type uint8_t and it can operate on uint16_t or uint32_t arrays by accessing the individual bytes and basically just casting it.

     How do we operate on multidimensional histograms btw?

     well the image histogram can be taken in multi channel mode, and then we can have a function that just folds that down to 1 dimension and resorts it.

     
     */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
