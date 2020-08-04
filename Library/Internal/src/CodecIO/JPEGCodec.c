#include "../../include/Private/JPEGCommon.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    void *JPEGOptions_Init(void) {
        void *Options          = calloc(1, sizeof(JPEGOptions));
        JPEGOptions *JPEG      = Options;
        JPEG->Huffman          = calloc(2, sizeof(HuffmanTable));
        return Options;
    }
    
    /*
     * A leaf node in a code tree. It has a symbol value.
     */
    /*
    struct Leaf final : public Node {
        
    uint32_t symbol;
        
        
    public: explicit Leaf(std::uint32_t sym);
        
    };
    */
    
    
    /*
     * An internal node in a code tree. It has two nodes as children.
     */
    /*
    struct InternalNode final : public Node {
        
    public: std::unique_ptr<Node> leftChild;  // Not null
        
    public: std::unique_ptr<Node> rightChild;  // Not null
        
        
    public: explicit InternalNode(std::unique_ptr<Node> &&left, std::unique_ptr<Node> &&right);
        
    };
    */
    
    
    /*
     * A binary tree that represents a mapping between symbols and binary strings.
     * The data structure is immutable. There are two main uses of a code tree:
     * - Read the root field and walk through the tree to extract the desired information.
     * - Call getCode() to get the binary code for a particular encodable symbol.
     * The path to a leaf node determines the leaf's symbol's code. Starting from the root, going
     * to the left child represents a 0, and going to the right child represents a 1. Constraints:
     * - The root must be an internal node, and the tree is finite.
     * - No symbol value is found in more than one leaf.
     * - Not every possible symbol value needs to be in the tree.
     * Illustrated example:
     *   Huffman codes:
     *     0: Symbol A
     *     10: Symbol B
     *     110: Symbol C
     *     111: Symbol D
     *
     *   Code tree:
     *       .
     *      / \
     *     A   .
     *        / \
     *       B   .
     *          / \
     *         C   D
     */
    //struct CodeTree final {
        
        /*---- Fields ----*/
        
    //public: InternalNode root;
        
        
        // Stores the code for each symbol, or null if the symbol has no code.
        // For example, if symbol 5 has code 10011, then codes.get(5) is the list [1,0,0,1,1].
    //private: std::vector<std::vector<char> > codes;
        
        
        /*---- Constructor ----*/
     /*
        // Constructs a code tree from the given tree of nodes and given symbol limit.
        // Each symbol in the tree must have value strictly less than the symbol limit.
    public: explicit CodeTree(InternalNode &&rt, std::uint32_t symbolLimit);
        
     */
        /*---- Methods ----*/
     /*
        // Recursive helper function for the constructor
    private: void buildCodeList(const Node *node, std::vector<char> &prefix);
        
        
        // Returns the Huffman code for the given symbol, which is a list of 0s and 1s.
    public: const std::vector<char> &getCode(std::uint32_t symbol) const;
        
    };
    */
    
    
    
    
    
    
    
    

    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    static MediaIO_ImageTypes JPEG_GetImageType(JPEGOptions *JPEG) {
        MediaIO_ImageTypes Type = ImageType_Unknown;
        if (JPEG->BitDepth <= 8) {
            Type          = ImageType_Integer8;
        } else {
            Type          = ImageType_Integer16;
        }
        return Type;
    }
    
    static MediaIO_ImageChannelMask JPEG_GetChannelMask(JPEGOptions *JPEG) {
        MediaIO_ImageChannelMask Mask = ImageMask_Unknown;
        if (JPEG->NumChannels == 3) {
            Mask               = ImageMask_2D | ImageMask_Red | ImageMask_Green | ImageMask_Blue;
        } else if (JPEG->NumChannels == 1) { // Todo: Actually find the channels encoded instead of assuming this basic shit
            Mask               = ImageMask_2D | ImageMask_Luma;
        }
        return Mask;
    }
    
    /*
     Prediction:
     
     We need to loop over the ImageContainer in order to access the lines and pixels and shit
     
     but I also want to centralize all Prediction algorithms in one place
     
     
     */
    
    void JPEG_Predictor(void *Options, ImageContainer *Image) {
        
    }
    
    /*
     
     HuffmanTable[Index] = {HuffmanCode, Value}
     
     All I need to do is figure out how to generate the HuffmanCode from BITS
     
     Lᵢ                  = Bits[i]
     Lᵢ(t) = Table[t][i] = Bits[i]
     Vᵢ,ⱼ =  Value where i is the bit length, and j is the index of the bitlength
     */

    // How many Nodes are there? BitLengthCounts
    
    void *JPEG_Extract(void *Options, BitBuffer *BitB) {
        JPEGOptions *JPEG            = Options;
        ImageContainer *Container    = ImageContainer_Init(JPEG_GetImageType(JPEG), JPEG_GetChannelMask(JPEG), JPEG->Width, JPEG->Height);
        if (Container != NULL) {
            int16_t Symbol           = -1;
            /*
             
             Ok so what do I need to do?
            
             Loop until we get an End of Block Huffman code, the end of the file, a restart marker, or that's pretty much it actually.
             
             */
            /*
            while (Symbol != JPEG->Huffman->EndOfBlockCode && (Symbol < Marker_Restart0 || Symbol > Marker_Restart7)) {
                // What do I do now? Read a Unary code until we find a stop bit effictively
                // Yes, read until you find a zerom it's effictively a RICE code.
                Symbol               = BitBuffer_ReadUnary(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, WholeUnary, 0);
                // Count the number of bits set in Symbol, go to that index in the Huffman code table and loop over all possible values there, until you find a matching code, then read that number of bits
                uint8_t SymbolSize   = CountBitsSet(Symbol);
                uint8_t NumBits2Read = 0;
                for (uint8_t NumSymbols = 0; NumSymbols < JPEG->Huffman->BitLengths[NumSymbols]; NumSymbols++) {
                    if (SymbolSize == JPEG->Huffman->NumBits[NumBits2Read]) {
                        NumBits2Read = JPEG->Huffman->NumBits[NumBits2Read];
                    }
                }
                Symbol               = (int16_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, SymbolSize);
            }
             */
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate ImageContainer"));
        }
        return Container;
    }
    
    void JPEGOptions_Deinit(void *Options) {
        JPEGOptions *JPEG = Options;
        free(JPEG->Arithmetic);
        free(JPEG->Components);
        free(JPEG->Huffman);
        free(JPEG);
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
