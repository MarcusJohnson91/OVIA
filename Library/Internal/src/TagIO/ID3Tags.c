#include "../../include/TagIO/ID3Tags.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/TextIO/StringIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    ID3Options *ID3Options_Init(void) {
        ID3Options *Options   = calloc(1, sizeof(ID3Options));
        Options->VersionMajor = 2;
        Options->VersionMinor = 4;
        Options->Encoding     = ID3Encoding_UTF8;
        return Options;
    }
    
    static uint32_t ID3Options_GetHeaderSize(BitBuffer *BitB) {
        AssertIO(BitB != NULL);
        uint32_t HeaderSize = 0UL;
        for (uint8_t SizeByte = 0; SizeByte < 4; SizeByte++) {
            BitBuffer_Seek(BitB, 1);
            HeaderSize <<= 7;
            HeaderSize  |= BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsFarthest, 7);
        }
        return HeaderSize;
    }
    
    void ID3Options_SetPreferredVersion(ID3Options *Options, uint8_t MajorVersion, uint8_t MinorVersion) {
        AssertIO(Options != NULL);
        AssertIO(MajorVersion == 2);
        AssertIO(MinorVersion >= 2 && MinorVersion <= 4);
        Options->VersionMajor = MajorVersion;
        Options->VersionMinor = MinorVersion;
    }
    
    void ID3Options_SetPreferredEncoding(ID3Options *Options, ID3Encodings Encoding) {
        AssertIO(Options != NULL);
        AssertIO(Encoding != ID3Encoding_Unspecified);
        Options->Encoding = Encoding;
    }
    
    static void ID3Options_WriteFlags(ID3Options *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        if (Options->Flags == ID3Flags_Unspecified) {
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsFarthest, 8, ID3Flags_Unspecified);
        } else {
            AssertIO(Options->VersionMajor == 2);
            if (Options->VersionMinor == 2) {
                bool IsCompressed     = (Options->Flags & ID3Flags_Compressed) == ID3Flags_Compressed;
                if (IsCompressed == Yes) {
                    BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsFarthest, 8, ID3Flags_Compressed);
                }
            } else if ((Options->VersionMinor >= 3 && Options->VersionMinor <= 4)) { // ID3v2.3-2.4
                bool IsExtended       = (Options->Flags & ID3Flags_IsExtended) == ID3Flags_IsExtended;
                if (IsExtended == Yes) {
                    BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsFarthest, 8, ID3Flags_IsExtended);
                }
                bool IsExperimental   = (Options->Flags & ID3Flags_IsExperimental) == ID3Flags_IsExperimental;
                if (IsExperimental == Yes) {
                    BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsFarthest, 8, ID3Flags_IsExperimental);
                }
            } else if (Options->VersionMinor == 4) { // ID3v2.4
                bool HasFooter        = (Options->Flags & ID3Flags_HasFooter) == ID3Flags_HasFooter;
                if (HasFooter == Yes) {
                    BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsFarthest, 8, ID3Flags_HasFooter);
                }
            }
        }
    }
    
    void ID3Options_WriteHeader(ID3Options *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        BitBuffer_WriteUTF8(BitB, UTF8String("ID3"), StringTerminator_Sized);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsFarthest, 8, Options->VersionMajor);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsFarthest, 8, Options->VersionMinor);
        ID3Options_WriteFlags(Options, BitB);
    }
    
    void ID3Options_WriteFooter(ID3Options *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        BitBuffer_WriteUTF8(BitB, UTF8String("3DI"), StringTerminator_Sized);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsFarthest, 8, Options->VersionMajor);
        BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsFarthest, 8, Options->VersionMinor);
        ID3Options_WriteFlags(Options, BitB);
    }
    
    void ID3Options_WriteFrames(ID3Options *Options, BitBuffer *BitB) { // We default to ID3 2.4 UTF-8, But we should have a few options
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        if (Options->VersionMajor == 2 && Options->VersionMinor == 2) { // ID3v2.2 aka first version, 3 bytes per frame id and frame size
                                                                        // Big 'ole switch for individual frames
        }
    }
    
    void ID3Options_ReadHeader(ID3Options *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->VersionMajor = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsFarthest, 8);
        Options->VersionMinor = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsFarthest, 8);
        bool UnsyncIsUsed     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsFarthest, 1);
        if (UnsyncIsUsed == Yes) {
            Options->Flags   |= ID3Flags_UnsyncIsUsed;
        }
        AssertIO(Options->VersionMajor == 2);
        if (Options->VersionMinor == 2) {
            bool IsCompressed     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsFarthest, 1);
            if (IsCompressed == Yes) {
                Options->Flags   |= ID3Flags_Compressed;
            }
        } else if ((Options->VersionMinor >= 3 && Options->VersionMinor <= 4)) { // ID3v2.3-2.4
            bool IsExtended       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsFarthest, 1);
            if (IsExtended == Yes) {
                Options->Flags   |= ID3Flags_IsExtended;
            }
            bool IsExperimental   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsFarthest, 1);
            if (IsExperimental == Yes) {
                Options->Flags   |= ID3Flags_IsExperimental;
            }
        } else if (Options->VersionMinor == 4) { // ID3v2.4
            bool HasFooter        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsFarthest, 1);
            if (HasFooter == Yes) {
                Options->Flags   |= ID3Flags_HasFooter;
            }
        }
        Options->TagSize          = ID3Options_GetHeaderSize(BitB);
        if ((Options->Flags & ID3Flags_IsExtended) == ID3Flags_IsExtended) {
            Options->ExtendedSize = ID3Options_GetHeaderSize(BitB);
        }
    }
    
    void ID3Options_ReadFooter(ID3Options *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        if (Options->VersionMajor == 2 && Options->VersionMinor == 2) { // ID3v2.2 aka first version, 3 bytes per frame id and frame size
                                                                        // Big 'ole switch for individual frames
        }
    }
    
    void ID3Options_ReadFrames(ID3Options *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        if (Options->VersionMajor == 2 && Options->VersionMinor == 2) { // ID3v2.2 aka first version, 3 bytes per frame id and frame size
                                                                        // Big 'ole switch for individual frames
        }
    }
    
    void ID3Options_Deinit(ID3Options *Options) {
        AssertIO(Options != NULL);
        free(Options);
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
