#include "../../include/CodecIO/JPEGCodec.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h" /* Included for Assertions */
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    void *JPEGOptions_Init(void) {
        JPEGOptions *Options = calloc(1, sizeof(JPEGOptions));
        AssertIO(Options != NULL);
        Options->Huffman     = calloc(1, sizeof(JPEGHuffman));
        return Options;
    }

    static MediaIO_ImageTypes JPEG_GetImageType(JPEGOptions *JPEG) {
        AssertIO(JPEG != NULL);
        MediaIO_ImageTypes Type = ImageType_Unknown;
        if (JPEG->BitDepth > 8) {
            Type          = ImageType_Integer8;
        } else {
            Type          = ImageType_Integer16;
        }
        return Type;
    }

    static MediaIO_ImageMask JPEG_GetChannelMask(JPEGOptions *JPEG) {
        AssertIO(JPEG != NULL);
        MediaIO_ImageMask Mask = ImageMask_Unknown;
        if (JPEG->NumChannels == 3) {
            Mask               = ImageMask_2D | ImageMask_Luma | ImageMask_Chroma1 | ImageMask_Chroma2;
        } else if (JPEG->NumChannels == 1) { // Todo: Actually find the channels encoded instead of assuming this basic shit
            Mask               = ImageMask_2D | ImageMask_Luma;
        }
        return Mask;
    }

    void JPEG_Extract(JPEGOptions *Options, BitBuffer *BitB, void *Container) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        AssertIO(Container != NULL);
        JPEGOptions *JPEG            = Options;
        ImageContainer *Image        = Container;
        uint16_t Symbol           = -1;
        /*

         Ok so what do I need to do?

         Loop until we get an End of Block Huffman code, the end of the file, a restart marker, or that's pretty much it actually.

         */
        while (Symbol != JPEG->Huffman->EndOfBlockSymbol && (Symbol < JPEGMarker_Restart0 || Symbol > JPEGMarker_Restart7)) {
            // What do I do now? Read a Unary code until we find a stop bit effictively
            // Yes, read until you find a zerom it's effictively a RICE code.
            Symbol               = BitBuffer_ReadUnary(BitB, ByteOrder_Left2Right, BitOrder_Left2Right, UnaryType_Whole, UnaryTerminator_Zero);
            // Count the number of bits set in Symbol, go to that index in the Huffman code table and loop over all possible values there, until you find a matching code, then read that number of bits
            uint8_t SymbolSize   = CountBitsSet(Symbol);
            uint8_t NumBits2Read = 0;
            for (uint8_t NumSymbols = 0; NumSymbols < JPEG->Huffman->Values[NumSymbols]->Symbol; NumSymbols++) {
                /*
                 if (SymbolSize == JPEG->Huffman->Values[NumBits2Read]) {
                 NumBits2Read = JPEG->Huffman->Values[NumBits2Read];
                 }
                 */
            }
            Symbol               = (int16_t) BitBuffer_ReadBits(BitB, ByteOrder_Left2Right, BitOrder_Left2Right, SymbolSize);
        }
    }

    void JPEGOptions_Deinit(JPEGOptions *Options) {
        AssertIO(Options != NULL);
        free(Options);
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
