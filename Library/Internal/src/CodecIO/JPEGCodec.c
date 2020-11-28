#include "../../include/Private/CodecIO/JPEGCodec.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    void *JPEGOptions_Init(void) {
        JPEGOptions *Options = calloc(1, sizeof(JPEGOptions));
        if (Options != NULL) {
            Options->Huffman     = calloc(1, sizeof(JPEGHuffman));
        }
        return Options;
    }

    static MediaIO_ImageTypes JPEG_GetImageType(JPEGOptions *JPEG) {
        MediaIO_ImageTypes Type = ImageType_Unknown;
        if (JPEG->BitDepth > 8) {
            Type          = ImageType_Integer8;
        } else {
            Type          = ImageType_Integer16;
        }
        return Type;
    }

    static Image_ChannelMask JPEG_GetChannelMask(JPEGOptions *JPEG) {
        Image_ChannelMask Mask = ImageMask_Unknown;
        if (JPEG->NumChannels == 3) {
            Mask               = ImageMask_2D | ImageMask_Luma | ImageMask_Chroma1 | ImageMask_Chroma2;
        } else if (JPEG->NumChannels == 1) { // Todo: Actually find the channels encoded instead of assuming this basic shit
            Mask               = ImageMask_2D | ImageMask_Luma;
        }
        return Mask;
    }

    void JPEG_Extract(void *Options, BitBuffer *BitB, void *Container) {
        JPEGOptions *JPEG            = Options;
        ImageContainer *Image        = Container;
        if (Container != NULL) {
            int16_t Symbol           = -1;
            /*

             Ok so what do I need to do?

             Loop until we get an End of Block Huffman code, the end of the file, a restart marker, or that's pretty much it actually.

             */
            while (Symbol != JPEG->Huffman->EndOfBlockCode && (Symbol < JPEGMarker_Restart0 || Symbol > JPEGMarker_Restart7)) {
                // What do I do now? Read a Unary code until we find a stop bit effictively
                // Yes, read until you find a zerom it's effictively a RICE code.
                Symbol               = BitBuffer_ReadUnary(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, UnaryType_Whole, UnaryTerminator_Zero);
                // Count the number of bits set in Symbol, go to that index in the Huffman code table and loop over all possible values there, until you find a matching code, then read that number of bits
                uint8_t SymbolSize   = CountBitsSet(Symbol);
                uint8_t NumBits2Read = 0;
                for (uint8_t NumSymbols = 0; NumSymbols < JPEG->Huffman->BitLengths[NumSymbols]; NumSymbols++) {
                    /*
                    if (SymbolSize == JPEG->Huffman->Values[NumBits2Read]) {
                        NumBits2Read = JPEG->Huffman->Values[NumBits2Read];
                    }
                     */
                }
                Symbol               = (int16_t) BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, SymbolSize);
            }
        } else {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Couldn't allocate ImageContainer"));
        }
    }

    void JPEGOptions_Deinit(void *Options) {
        JPEGOptions *JPEG = Options;
        free(JPEG);
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
