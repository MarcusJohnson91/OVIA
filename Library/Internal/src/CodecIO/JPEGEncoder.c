#include "../../include/CodecIO/JPEGCodec.h"

#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/TextIO/LogIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    void JPEG_WriteSegment_StartOfFrame(void *Options, BitBuffer *BitB) {
        JPEGOptions *JPEG = Options;
        if (JPEG != NULL && BitB != NULL) {

        } else if (JPEG == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
        if (JPEG->EntropyCoder == EntropyCoder_Arithmetic) {
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16, JPEGMarker_StartOfFrameLossless3);
        } else if (JPEG->EntropyCoder == EntropyCoder_Huffman) {
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16, JPEGMarker_StartOfFrameLossless1);
        }
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16, JPEG->BitDepth);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16, JPEG->Height);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16, JPEG->Width);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16, JPEG->NumChannels);

        for (uint8_t Channel = 0; Channel < JPEG->NumChannels; Channel++) {
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8, JPEG->Channels[Channel].ChannelID);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 4, JPEG->Channels[Channel].HorizontalSF);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 4, JPEG->Channels[Channel].VerticalSF);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8, JPEG->Huffman->Values[JPEG->Huffman->TableID]); // 0
        }
    }

    void JPEG_WriteSegment_DefineHuffmanTable(void *Options, BitBuffer *BitB) {
        JPEGOptions *JPEG = Options;
        if (JPEG != NULL && BitB != NULL) {

        } else if (JPEG == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 4, JPEG->Huffman->TableID);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 4, JPEG->Huffman->TableID);

        for (uint8_t Count = 0; Count < 16; Count++) {
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8, JPEG->Huffman->Values[Count]->BitLength);
            for (uint8_t Code = 0; Code < JPEG->Huffman->Values[Count]; Code++) {
                if (JPEG->Huffman->Values[Code] > 0) {
                    BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8, JPEG->Huffman->Values[Code]);
                }
            }
        }
    }

    void JPEG_WriteSegment_DefineArithmeticTable(void *Options, BitBuffer *BitB) {
        JPEGOptions *JPEG = Options;
        if (JPEG != NULL && BitB != NULL) {

        } else if (JPEG == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 4,  JPEG->Arithmetic->TableType);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 4,  JPEG->Arithmetic->TableID);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8,  JPEG->Arithmetic->CodeValue);
    }

    void JPEG_WriteSegment_DefineRestartInterval(void *Options, BitBuffer *BitB) {
        JPEGOptions *JPEG = Options;
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16, JPEG->RestartInterval);
    }

    void JPEG_WriteSegment_DefineNumberOfLines(void *Options, BitBuffer *BitB) {
        JPEGOptions *JPEG = Options;
        if (JPEG != NULL && BitB != NULL) {

        } else if (JPEG == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 16, JPEG->Height);
    }

    void JPEG_WriteSegment_Comment(void *Options, BitBuffer *BitB) {
        JPEGOptions *JPEG = Options;
        if (JPEG != NULL && BitB != NULL) {

        } else if (JPEG == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
        if (JPEG->CommentSize >= 3 && JPEG->Comment != NULL) {
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 16, JPEGMarker_Comment);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 16, JPEG->CommentSize);
            for (uint16_t Byte = 0; Byte < JPEG->CommentSize - 2; Byte++) {
                BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 16, JPEG->Comment[Byte]);
            }
        }
    }

    void JPEG_WriteSegment_StartOfScan(void *Options, BitBuffer *BitB) {
        JPEGOptions *JPEG = Options;
        if (JPEG != NULL && BitB != NULL) {

        } else if (JPEG == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 16, JPEGMarker_StartOfScan);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 16, (JPEG->NumChannels * 2) + 2);

        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 16, JPEG->NumChannels);
        for (uint8_t Channel = 0; Channel < JPEG->NumChannels; Channel++) {
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 8, JPEG->Channels[Channel].ChannelID);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 4, JPEG->Channels[Channel].HorizontalSF);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, 4, JPEG->Channels[Channel].VerticalSF);
        }
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
