#include "../../include/CodecIO/JPEGCodec.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h" /* Included for Assertions */
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    void JPEG_WriteSegment_StartOfFrame(JPEGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        
        if (Options->EntropyCoder == EntropyCoder_Arithmetic) {
            BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 16, JPEGMarker_StartOfFrameLossless3);
        } else if (Options->EntropyCoder == EntropyCoder_Huffman) {
            BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 16, JPEGMarker_StartOfFrameLossless1);
        }
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 16, Options->BitDepth);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 16, Options->Height);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 16, Options->Width);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 16, Options->NumChannels);
        
        for (uint8_t Channel = 0; Channel < Options->NumChannels; Channel++) {
            BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 8, Options->Channels[Channel].ChannelID);
            BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 4, Options->Channels[Channel].HorizontalSF);
            BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 4, Options->Channels[Channel].VerticalSF);
            BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 8, Options->Huffman->Values[Options->Huffman->TableID]); // 0
        }
    }
    
    void JPEG_WriteSegment_DefineHuffmanTable(JPEGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 4, Options->Huffman->TableID);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 4, Options->Huffman->TableID);
        
        for (uint8_t Count = 0; Count < 16; Count++) {
            BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 8, Options->Huffman->Values[Count]->BitLength);
            for (uint8_t Code = 0; Code < Options->Huffman->Values[Count]; Code++) {
                if (Options->Huffman->Values[Code] > 0) {
                    BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 8, Options->Huffman->Values[Code]);
                }
            }
        }
    }
    
    void JPEG_WriteSegment_DefineArithmeticTable(JPEGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 4,  Options->Arithmetic->TableType);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 4,  Options->Arithmetic->TableID);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 8,  Options->Arithmetic->CodeValue);
    }
    
    void JPEG_WriteSegment_DefineRestartInterval(JPEGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 16, Options->RestartInterval);
    }
    
    void JPEG_WriteSegment_DefineNumberOfLines(JPEGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 16, Options->Height);
    }
    
    void JPEG_WriteSegment_Comment(JPEGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        if (Options->CommentSize >= 3 && Options->Comment != NULL) {
            BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsLeft, 16, JPEGMarker_Comment);
            BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsLeft, 16, Options->CommentSize);
            for (uint16_t Byte = 0; Byte < Options->CommentSize - 2; Byte++) {
                BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsLeft, 16, Options->Comment[Byte]);
            }
        }
    }
    
    void JPEG_WriteSegment_StartOfScan(JPEGOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsLeft, 16, JPEGMarker_StartOfScan);
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsLeft, 16, (Options->NumChannels * 2) + 2);
        
        BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsLeft, 16, Options->NumChannels);
        for (uint8_t Channel = 0; Channel < Options->NumChannels; Channel++) {
            BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsLeft, 8, Options->Channels[Channel].ChannelID);
            BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsLeft, 4, Options->Channels[Channel].HorizontalSF);
            BitBuffer_WriteBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsLeft, 4, Options->Channels[Channel].VerticalSF);
        }
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
