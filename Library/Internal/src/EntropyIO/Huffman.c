#include "../../../include/Private/EntropyIO/EntropyIO.h"

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

    HuffmanTree *HuffmanTree_Init(uint8_t BitLengths[16], uint8_t *Values) {
        HuffmanTree *Tree = calloc(1, sizeof(HuffmanTree));
        if (Tree != NULL) {
            // What do we do?
            // Create HUFFSIZE and HUFFCODE tables from BitLengths and Values
            // HUFFSIZE=CodeLengths
            // HUFFCODE=actual codes

            /*
             0000010501010101010100000000000000000102030405060708090A0B0100030101010101010101010000000000000102030405060708090A0B100002010303020403050504040000017D01020300041105122131410613516107227114328191A1082342B1C11552D1F02433627282090A161718191A25262728292A3435363738393A434445464748494A535455565758595A636465666768696A737475767778797A838485868788898A92939495969798999AA2A3A4A5A6A7A8A9AAB2B3B4B5B6B7B8B9BAC2C3C4C5C6C7C8C9CAD2D3D4D5D6D7D8D9DAE1E2E3E4E5E6E7E8E9EAF1F2F3F4F5F6F7F8F9FA1100020102040403040705040400010277000102031104052131061241510761711322328108144291A1B1C109233352F0156272D10A162434E125F11718191A262728292A35363738393A434445464748494A535455565758595A636465666768696A737475767778797A82838485868788898A92939495969798999AA2A3A4A5A6A7A8A9AAB2B3B4B5B6B7B8B9BAC2C3C4C5C6C7C8C9CAD2D3D4D5D6D7D8D9DAE2E3E4E5E6E7E8E9EAF2F3F4F5F6F7F8F9FA
             */

            /*
             Sorting:
             Symbols are first sorted by length, the sorted alphabetically.

             First symbol is 0, the the number of zeros being determined by bitlength

             Subsequent codes within a bitlength are
             */

            /*
             C.1:

             for (i = 0; i < ????; i++) {
                for (j = 0; j < ????; j++) {
                    for (k = 1; k < ????; k++) {
                        if (k > BitLengths[i]) {
                            i += 1;
                            j  = 1;
                            if (i > 16) {
                                HUFFSIZE[k] = 0;
                                LASTK       = k;
                            }
                        } else {
                            HUFFSIZE[k] = i + 1;
                            k += 1;
                            j += 1;
                        }
                    }
                }
             }
             */

            /*
             C.2: SLL CODE X = ShiftLeft X bits


             */
        }
        return Tree;
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
