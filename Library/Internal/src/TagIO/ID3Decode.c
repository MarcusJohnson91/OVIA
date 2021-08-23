#include "../../../include/Private/TagIO/ID3Tags.h"
#include "../../../../Dependencies/FoundationIO/Library/include/TextIO/LogIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    static uint32_t ID3Options_GetHeaderSize(BitBuffer *BitB) {
        uint32_t HeaderSize = 0UL;
        for (uint8_t SizeByte = 0; SizeByte < 4; SizeByte++) {
            BitBuffer_Seek(BitB, 1);
            HeaderSize <<= 7;
            HeaderSize  |= BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsFarthest, 7);
        }
        return HeaderSize;
    }

    void ID3Options_ReadHeader(ID3Options *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            Options->VersionMajor = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsFarthest, 8);
            Options->VersionMinor = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsFarthest, 8);
            bool UnsyncIsUsed     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsFarthest, 1);
            if (UnsyncIsUsed == Yes) {
                Options->Flags   |= ID3Flags_UnsyncIsUsed;
            }
            if (Options->VersionMajor == 2) {
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
            } else {
                Log(Severity_USER, PlatformIO_FunctionName, UTF8String("MajorVersion: %u is invalid, it must be 2"), Options->VersionMajor);
            }
            Options->TagSize          = ID3Options_GetHeaderSize(BitB);
            if ((Options->Flags & ID3Flags_IsExtended) == ID3Flags_IsExtended) {
                Options->ExtendedSize = ID3Options_GetHeaderSize(BitB);
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void ID3Options_ReadFooter(ID3Options *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            if (Options->VersionMajor == 2 && Options->VersionMinor == 2) { // ID3v2.2 aka first version, 3 bytes per frame id and frame size
                // Big 'ole switch for individual frames
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void ID3Options_ReadFrames(ID3Options *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            if (Options->VersionMajor == 2 && Options->VersionMinor == 2) { // ID3v2.2 aka first version, 3 bytes per frame id and frame size
                // Big 'ole switch for individual frames
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
