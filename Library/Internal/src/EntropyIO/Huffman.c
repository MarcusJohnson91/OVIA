#include "../../include/EntropyIO/Huffman.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    /* Create a Min Heap data structure */
    typedef struct Heap {
        uint64_t *Array;
        size_t    NumElements;
        size_t    Capacity;
    } Heap;

    Heap *Heap_Init(size_t Capacity) {
        AssertIO(Capacity > 0);
        Heap *Data = calloc(1, sizeof(Heap));

        AssertIO(Data != NULL);

        Data->Array = calloc(Capacity, sizeof(uint64_t));

        AssertIO(Data->Array != NULL);

        Data->Capacity    = Capacity;
        Data->NumElements = 0;
        return Data;
    }

    void Heap_Insert(Heap *Heap, uint64_t Index, uint64_t Value2Insert) {
        AssertIO(Heap != NULL);
        AssertIO(Index < Heap->Capacity);
        Heap->Array[Index] = Value2Insert;
    }

    /* OLD below */

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

#define ALP_SIZE      256
#define ALP_LAST      (ALP_SIZE - 1)
#define BITS_PER_BYTE 8
#define MAX_LENGTH    32

#define FETCH_BIT(buf, n)        ((uint8_t) (buf[n / BITS_PER_BYTE] << (n % BITS_PER_BYTE)) >> (BITS_PER_BYTE - 1))
#define GET_N_LAST_BITS(code, n) (code & (uint8_t) (Exponentiate(2, n) - 1))

    uint32_t get_file_size(FILE *fp) {
        uint32_t size;

        fseek(fp, 0, SEEK_END);
        size = ftell(fp);
        rewind(fp);

        return size;
    }

    uint8_t *read_file(FILE *fp) {
        uint32_t file_size = get_file_size(fp);

        uint8_t *buf = calloc(sizeof(uint8_t), file_size);
        fread(buf, sizeof(uint8_t), file_size, fp);

        return buf;
    }

    uint32_t *calc_freq(uint8_t *buf, uint32_t size) {
        uint32_t *freq = calloc(sizeof(uint32_t), ALP_SIZE);

        uint32_t i;
        for (i = 0; i < size; i++) {
            freq[buf[i]]++;
        }

        return freq;
    }

    // Order aware sorting
    void sort(uint32_t *freq, uint32_t *symb) {
        uint32_t *pos = malloc(sizeof(uint32_t) * ALP_SIZE);
        uint32_t  i;
        for (i = 0; i < ALP_SIZE; i++) {
            if (symb != NULL) {
                symb[i] = i;
                pos[i]  = i;
            }
        }

        // Insertion sort
        uint32_t j;
        for (i = 0; i < ALP_SIZE; i++) {
            uint32_t value = freq[i];
            j              = i - 1;
            // Find suitable j position
            while ((j >= 0) && (freq[j] > value)) {
                freq[j + 1] = freq[j];
                if (symb != NULL) {
                    pos[j + 1] = pos[j];
                    symb[pos[j + 1]]++;
                }
                j--;
            }
            freq[j + 1] = value;
            if (symb != NULL) {
                pos[j + 1] = i;
                symb[i]    = j + 1;
            }
        }
    }

    void calc_len(uint32_t *freq) {
        // First phase - set parent pointers
        uint32_t leaf = 0; // Leaf index
        uint32_t node = 0; // Node index
        uint32_t root;     // Root node
        for (root = 0; root < ALP_SIZE - 1; root++) {
            // Select first node
            if ((leaf > ALP_SIZE - 1) || ((node < root) && (freq[node] < freq[leaf]))) {
                // Select a node
                freq[root] = freq[node];
                freq[node] = root; // Set pointer to parent
                node++;
            } else {
                // Select a leaf
                freq[root] = freq[leaf];
                leaf++;
            }

            // Add second node
            if ((leaf > ALP_SIZE - 1) || ((node < root) && (freq[node] < freq[leaf]))) {
                // Select a node
                freq[root] += freq[node];
                freq[node] = root; // Set pointer to parent
                node++;
            } else {
                // Select a leaf
                freq[root] += freq[leaf];
                leaf++;
            }
        }

        // Second phase - set node depth
        freq[ALP_SIZE - 2] = 0; // Root depth

        uint32_t i;
        for (i = ALP_SIZE - 3; i >= 0; i--) {
            freq[i] = freq[freq[i]] + 1;
        }

        // Third phase - set leaf depth
        uint32_t depth                   = 0;
        uint32_t nodes_amount            = 0;
        uint32_t nodes_and_leaves_amount = 1;
        uint32_t vacant                  = ALP_SIZE - 1;
        uint32_t node_index              = ALP_SIZE - 2;
        while (nodes_and_leaves_amount > 0) {
            while ((node_index >= 0) && (freq[node_index] == depth)) {
                nodes_amount++;
                node_index--;
            }

            while (nodes_and_leaves_amount > nodes_amount) {
                freq[vacant] = depth;
                vacant--;
                nodes_and_leaves_amount--;
            }

            nodes_and_leaves_amount = 2 * nodes_amount;
            nodes_amount            = 0;
            depth++;
        }
    }

    // Sort symbol by two keys:
    // 1. By code length
    // 2. By alphabetical order
    void sort_len(uint32_t *freq, uint32_t *symb) {
        // Pack length
        uint32_t i;
        for (i = 0; i < ALP_SIZE; i++) {
            freq[symb[i]] *= ALP_SIZE;
            freq[symb[i]] += i;
        }

        sort(freq, NULL);

        // Unpack length
        for (i = 0; i < ALP_SIZE; i++) {
            symb[i] = freq[i] % ALP_SIZE;
            freq[i] = freq[i] / ALP_SIZE;

            if (freq[i] >= MAX_LENGTH) {
                printf("Maximum bit-length reached\n!");
                exit(1);
            }
        }
    }

    void print_binary(uint32_t val, uint32_t len) {
        uint32_t i;
        for (i = (32 - len); i < 32; i++) {
            if (val & (1 << (31 - i))) {
                printf("1");
            } else {
                printf("0");
            }
        }
        printf("\n");
    }

    void create_codebook(uint32_t *len, uint32_t *symb, uint32_t **codebook, uint32_t **codelen) {
        *codebook = calloc(sizeof(uint32_t), ALP_SIZE);
        *codelen  = calloc(sizeof(uint32_t), ALP_SIZE);

        uint32_t  root;
        uint32_t  leaf;
        uint32_t  level;
        uint32_t  i;
        uint32_t *nodes = calloc(sizeof(uint32_t), MAX_LENGTH);
        for (root = 0, i = ALP_LAST, level = len[ALP_LAST]; level > 0; level--) {
            leaf = 0;
            while ((i >= 0) && (len[i] == level)) {
                leaf++;
                i--;
            }

            nodes[level] = root;
            root         = (root + leaf) >> 1;
        }

        uint32_t count;
        for (i = 0, count = 0; i < ALP_SIZE; i++) {
            level                = len[i];
            (*codebook)[symb[i]] = nodes[level] + count;
            (*codelen)[symb[i]]  = len[i];
            printf("symb - %c(%i): ", symb[i], symb[i]);
            print_binary(nodes[level] + count, len[i]);

            if ((i != ALP_SIZE - 1) && len[i] < len[i + 1]) {
                count = 0;
            } else {
                count++;
            }
        }
    }

    /*!
     obuf = OutputBuffer, where the encoded data is written
     */
    void encode_file(uint32_t *codebook, uint32_t *codelen, uint8_t *buf, uint32_t buf_size, uint8_t **obuf, uint32_t *obuf_size, uint32_t *freq, uint32_t *symb, uint32_t *packed_tree) {
        *obuf      = calloc(sizeof(uint8_t), buf_size);
        *obuf_size = 0;

        uint32_t free_bits = BITS_PER_BYTE;
        uint32_t i;
        uint32_t pending_bits;
        for (i = 0; i < buf_size; i++) {
            pending_bits = codelen[buf[i]];

            while (pending_bits != 0) {
                if (pending_bits >= free_bits) {
                    (*obuf)[*obuf_size] <<= free_bits;
                    (*obuf)[(*obuf_size)++] |= GET_N_LAST_BITS(codebook[buf[i]] >> (pending_bits - free_bits), pending_bits);
                    pending_bits -= free_bits;
                    free_bits = BITS_PER_BYTE;
                } else {
                    (*obuf)[*obuf_size] <<= pending_bits;
                    (*obuf)[*obuf_size] |= GET_N_LAST_BITS(codebook[buf[i]], pending_bits);
                    free_bits -= pending_bits;
                    if (i == buf_size - 1) {
                        (*obuf)[(*obuf_size)++] <<= free_bits;
                    }
                    pending_bits = 0;
                }
            }
        }

        FILE *ofp = fopen("encoded", "w+");
        // Pack tree
        fwrite(packed_tree, sizeof(uint32_t), ALP_SIZE, ofp);

        fwrite(*obuf, sizeof(uint8_t), *obuf_size, ofp);
        fclose(ofp);
    }

    void encode(char *filename) {
        // Create buffer
        FILE    *fp = fopen(filename, "r");
        uint8_t *buf;
        buf               = read_file(fp);
        uint32_t buf_size = get_file_size(fp);

        // Sort frequences
        uint32_t *freq = calc_freq(buf, buf_size);
        uint32_t *symb = calloc(sizeof(uint32_t), ALP_SIZE);
        sort(freq, symb);

        // Sort len
        calc_len(freq);
        uint32_t *packed_tree = calloc(sizeof(uint32_t), ALP_SIZE);
        uint32_t  i;
        for (i = 0; i < ALP_SIZE; i++) {
            packed_tree[i] = freq[symb[i]];
        }
        sort_len(freq, symb);

        // Create codebook
        uint32_t *codebook;
        uint32_t *codelen;
        create_codebook(freq, symb, &codebook, &codelen);

        // Encode
        uint8_t *obuf; // Output Buffer, where the encoded data is written
        uint32_t obuf_size;
        encode_file(codebook, codelen, buf, buf_size, &obuf, &obuf_size, freq, symb, packed_tree);

        printf("Source file size in bytes - %i\n", buf_size);
        printf("Encoded size in bytes - %i\n", obuf_size);

        // Free memory
        fclose(fp);
        free(buf);
        free(obuf);
        free(freq);
        free(symb);
        free(codebook);
        free(codelen);
    }

    // nodes - first leaf level
    // offs - internal nodes per level
    void create_decode_tables(uint32_t *len, uint32_t *symb, uint32_t *nodes, uint32_t *offs, uint8_t *buf, uint32_t buf_size) {
        uint32_t root;
        uint32_t leaf;
        uint32_t level;
        uint32_t i;
        for (root = 0, i = ALP_LAST, level = len[ALP_LAST]; level > 0; level--) {
            leaf = 0;
            while ((i >= 0) && (len[i] == level)) {
                leaf++;
                i--;
            }

            nodes[level] = root;
            root         = (root + leaf) >> 1;
            if (leaf) {
                offs[level] = i + 1;
            }
        }
    }

    void decode_file(uint32_t *symb, uint8_t *buf, uint32_t buf_size, uint32_t *nodes, uint32_t *offs) {
        uint8_t *obuf      = calloc(sizeof(uint8_t), buf_size * 5);
        uint32_t obuf_size = 0;
        uint32_t length;
        uint32_t code;
        uint32_t fetched_bits = 0;
        while ((fetched_bits / BITS_PER_BYTE) < (buf_size)) {
            length = 0;
            code   = 0;
            do {
                code <<= 1;
                code |= FETCH_BIT(buf, fetched_bits);
                fetched_bits++;
                length++;
            } while ((code < nodes[length]) && (fetched_bits < (buf_size * BITS_PER_BYTE)));

            if (fetched_bits < (buf_size * BITS_PER_BYTE)) {
                obuf[obuf_size++] = symb[offs[length] + code - nodes[length]];
            }
        }

        FILE *fp = fopen("decoded", "w+");
        fwrite(obuf, sizeof(uint8_t), obuf_size, fp);

        fclose(fp);
        free(obuf);
    }

    void decode(void) {
        // Read file
        FILE     *fp       = fopen("encoded", "r");
        uint32_t  buf_size = get_file_size(fp) - (ALP_SIZE * sizeof(uint32_t));
        uint32_t *freq     = calloc(sizeof(uint32_t), ALP_SIZE);
        fread(freq, sizeof(uint32_t), ALP_SIZE, fp);
        uint8_t *buf = calloc(sizeof(uint32_t), buf_size);
        fread(buf, sizeof(uint8_t), buf_size, fp);

        // Create symb array
        uint32_t *symb = calloc(sizeof(uint32_t), ALP_SIZE);
        uint32_t  i;
        for (i = 0; i < ALP_SIZE; i++) {
            symb[i] = i;
        }

        // Calc decode tables
        sort_len(freq, symb);
        uint32_t *nodes = calloc(sizeof(uint32_t), MAX_LENGTH);
        uint32_t *offs  = calloc(sizeof(uint32_t), MAX_LENGTH);
        create_decode_tables(freq, symb, nodes, offs, buf, buf_size);

        decode_file(symb, buf, buf_size, nodes, offs);
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
