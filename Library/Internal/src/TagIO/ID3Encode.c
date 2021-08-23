#include "../../../include/Private/TagIO/ID3Tags.h"
#include "../../../../Dependencies/FoundationIO/Library/include/TextIO/LogIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    void ID3Options_SetPreferredVersion(ID3Options *Options, uint8_t MajorVersion, uint8_t MinorVersion) {
        if (Options != NULL && MajorVersion == 2 && (MinorVersion >= 2 && MinorVersion <= 4)) {
            Options->VersionMajor = MajorVersion;
            Options->VersionMinor = MinorVersion;
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (MajorVersion != 2) {
            Log(Severity_USER, PlatformIO_FunctionName, UTF8String("MajorVersion does not equal 2"));
        } else if (MinorVersion < 2 || MinorVersion > 4) {
            Log(Severity_USER, PlatformIO_FunctionName, UTF8String("MinorVersion must be between 2 and 4"));
        }
    }

    void ID3Options_SetPreferredEncoding(ID3Options *Options, ID3Encodings Encoding) {
        if (Options != NULL && Encoding != ID3Encoding_Unspecified) {
            Options->Encoding = Encoding;
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (Encoding == ID3Encoding_Unspecified) {
            Log(Severity_USER, PlatformIO_FunctionName, UTF8String("ID3Encoding_Unspecified is Invalid"));
        }
    }

    static void ID3Options_WriteFlags(ID3Options *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            if (Options->Flags == ID3Flags_Unspecified) {
                BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsFarthest, 8, ID3Flags_Unspecified);
            } else {
                if (Options->VersionMajor == 2) {
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
                } else {
                    Log(Severity_USER, PlatformIO_FunctionName, UTF8String("MajorVersion: %u is invalid, it must be 2"), Options->VersionMajor);
                }
            }
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void ID3Options_WriteHeader(ID3Options *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            BitBuffer_WriteUTF8(BitB, UTF8String("ID3"), StringTerminator_Sized);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsFarthest, 8, Options->VersionMajor);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsFarthest, 8, Options->VersionMinor);
            ID3Options_WriteFlags(Options, BitB);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void ID3Options_WriteFooter(ID3Options *Options, BitBuffer *BitB) {
        if (Options != NULL && BitB != NULL) {
            BitBuffer_WriteUTF8(BitB, UTF8String("3DI"), StringTerminator_Sized);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsFarthest, 8, Options->VersionMajor);
            BitBuffer_WriteBits(BitB, ByteOrder_LSByteIsNearest, BitOrder_LSBitIsFarthest, 8, Options->VersionMinor);
            ID3Options_WriteFlags(Options, BitB);
        } else if (Options == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Options Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("BitBuffer Pointer is NULL"));
        }
    }

    void ID3Options_WriteFrames(ID3Options *Options, BitBuffer *BitB) { // We default to ID3 2.4 UTF-8, But we should have a few options
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
