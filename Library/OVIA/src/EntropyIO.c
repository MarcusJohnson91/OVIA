#include "../include/EntropyIO.h" /* Included for our declarations */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    /*!
     @var Symbol     Symbol is the actual Huffman code to search for.
     @var Value      Value  is the decoded value to replace the Symbol with.
     @var BitLength  BitLength is which level this Node is associated with.
     */
    typedef struct HuffmanNode {
        uint16_t Symbol;
        uint16_t Value;
        uint8_t  BitLength;
    } HuffmanNode;

    typedef struct HuffmanTable {
        HuffmanNode *Nodes;
        uint16_t     NumNodes;
    } HuffmanTable;

    HuffmanTable *HuffmanTable_Init(uint16_t NumNodes) {
        HuffmanTable *Table = calloc(1, sizeof(HuffmanTable));
        Table->Nodes        = calloc(NumNodes, sizeof(HuffmanNode));
        Table->NumNodes     = NumNodes;
        return Table;
    }

    static uint8_t HuffmanTable_GetBitLength(uint64_t Node) {
        uint8_t BitLength = 0;
        BitLength         = (Exponentiate(2, Node + 1) - (Node + 1));
        return BitLength;
    }

    static uint8_t HuffmanTable_GetNumLevels(uint64_t NumFrequencies) {
        uint8_t NumLevels = 0;
        bool IsPowerOf2 = NumFrequencies % 2;
        if (IsPowerOf2 == Yes) {
            NumLevels = Logarithm(2, NumFrequencies);
        } else {
            NumLevels = HuffmanTable_GetBitLength(NumFrequencies);
        }
        return NumLevels;
    }

    /*!
     @remark                 Frequencies MUST be sorted in Descending order.
     @param FrequencySize    Frequency Size is the size of each element in bytes, uint8_t = 1, uint16_t = 2, etc
     */
    void HuffmanTable_GenerateFromFrequencies(HuffmanTable *Table, uint64_t NumFrequencies, uint16_t *Frequencies, uint8_t FrequencySize) {
        if (Table != NULL && Frequencies != NULL && NumFrequencies == Table->NumNodes) {
            for (uint64_t Frequency = 0ULL; Frequency < NumFrequencies; Frequency++) {
                Table->Nodes[Frequency].BitLength        = HuffmanTable_GetBitLength(Frequency); // Work on this function
                if (Frequency > 0) {
                    Table->Nodes[Frequency].Symbol       = Table->Nodes[Frequency - 1].Symbol + 1;
                    Table->Nodes[Frequency].Value        = Table->Nodes[Frequency - 1].Value  + 1;
                    if (Table->Nodes[Frequency - 1].BitLength != Table->Nodes[Frequency].BitLength) {
                        Table->Nodes[Frequency].Symbol   = Table->Nodes[Frequency - 1].Symbol + 1;
                        Table->Nodes[Frequency].Symbol <<= Table->Nodes[Frequency].BitLength - Logarithm(2, Table->Nodes[Frequency].Symbol);
                    }
                }
            }

            /*
             "Jonsson" 2 O's, 2 S's, 2 N's 1 J.
             Frequencies: {o, n, s, J} // Listed in order of apperance
             FrequenciesS: {n, o, s, J} // Sorted
             Node[0] = {.BitLength = 1, .Symbol = 0b1,   .Value = 'n'},
             Node[1] = {.BitLength = 2, .Symbol = 0b10,  .Value = 'o'},
             Node[2] = {.BitLength = 2, .Symbol = 0b11,  .Value = 's'},
             Node[3] = {.BitLength = 3, .Symbol = 0b100, .Value = 'J'},
             */

            // "Jonsson" encoded with Canonical Huffman is "0b1001011111101" I did something wrong here, but it's 13 bits instead of 49 ASCII or 56 UTF-8 bits, a savings of 73.4% or 76.7%

            // 1 bit codes are only allowed in specific circuminstances

            // Jonsson actually takes up 2 bits for each symbol; or 14 bits instead of 13.




            /* C.1 */
            /*
            uint16_t Index           = 0;
            for (uint16_t i = 0; i < NumCodes; i++) {
                for (uint16_t j = 0; j < BitLengthCounts[i]; j++) {
                    HUFFSIZE[Index]  = i + 1;
                    Index           += 1;
                }
            }
            uint16_t LastK           = Index;
            */
            /* C.1 */

            /* C.2 */
            /*
            uint16_t k    = 0;
            uint16_t si   = HUFFSIZE[0];
            uint16_t code = 0;
            for (uint16_t i = 0; i < LastK; i++) {
                while (HUFFSIZE[k] == si) {
                    huffcode[k] = code;
                    code++;
                    k++;
                }
                code <<= 1;
                si++;
            }
            */
            /* C.2 */

            /* C.3 */
            /*
            for (uint16_t k = 0; k < LastK; k++) {
                L          = HUFFVAK[k];
                EHUFCO(L)  = HUFFCODE(k);
                EHUFSI(L)  = HUFFSIZE(k);
                k         += 1;
            }
             */
            /* C.3 */

            /* F.15 */
            /*
            uint16_t bitcount = 0;
            for (uint8_t i = 0; i < 16; i++) {
                if (BitLengthCounts[i] != 0) {
                    valptr[i] = bitcount;
                }
                for (uint8_t j = 0; j < bits[i]; j++) {
                    if (huffcode[j + NumCodes] < mincode[i] || mincode[i] == -1) {
                        mincode[i] = huffcode[j + NumCodes];
                    }

                    if (huffcode[j + bitcount] > maxcode[i]) {
                        maxcode[i] = huffcode[j + NumCodes];
                    }
                }
                if (mincode[i] != -1) {
                    valptr[i] = (short)(valptr[i] - mincode[i]);
                }
                bitcount += BitLengthCounts[i];
            }
             */
            /* F.15 */

            /* F.12 */
            // Parameters: diff, t
            /*
            uint16_t Vt = 1 << t - 1;
            if (diff < Vt) {
                Vt     = (-1 << t) + 1;
                diff   = diff + Vt;
            }
            return diff;
             */
            /* F.12 */

            /* F.16 */
            // Parameters: JPEGBinaryReader JPEGStream
            /*
                int i = 0;
                short code = (short)JPEGStream.BitBuffer_ReadBits(1);
                while (code > maxcode[i])
                {
                    i++;
                    code <<= 1;
                    code |= (short)JPEGStream.BitBuffer_ReadBits(1);
                }
                int val = HUFFVAL[code + (valptr[i])];
            if (val < 0) {
                    val = 256 + val;
            }
                return val;
             */
            /* F.16 */
        } else if (Table == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Table Pointer is NULL"));
        } else if (Frequencies == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Frequencies Pointer is NULL"));
        } else if (NumFrequencies != Table->NumNodes) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("NumFrequencies does not match the number of nodes in Table"));
        }
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
