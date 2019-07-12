#include "../include/Private/OVIACommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    /*
     OVIA Workflows:
     
     Decode: -----------------------------------------
     
     Parse command line arguments with CommandLineIO.
     
     Open specified file
     
     Determine file type
     
     Decode with the approperiate decoder.
     
     return a Container to the main thread.
     
     Encode: ---------------------------------------
     
     Take in a Container (Streaming support may be required)
     
     the main program needs to be able to tell OVIA which codec to use
     
     OVIA calls that encoder with this audio container and breaks it up in such a way that the whole container is able to be written at once without any leftover.
     
     After a sample block is encoed it's added to the stream.
     */
    
    //     --------------------------------------------------------------------------------------------------------------
    
    /*
     OVIA API:
     
     We don't need a centralized type to contain every codec, we're breaking each codec into it's own module.
     
     What we do need is a way to exchange information between the various formats.
     
     ImageContainers and Audio2DContainers work fine for actual content.
     
     but what about metadata?
     
     How do we share GAMMA metadata or ICC color profiles or song tags between formats?
     
     Should ICC profiles be a part of OVIA or ContainerIO?
     */
    
    OVIA_CodecIDs UTF8_Extension2CodecID(UTF8 *Extension) {
        OVIA_CodecIDs CodecID = CodecID_Unknown;
        if (Extension != NULL) {
            UTF8 *Normalized  = UTF8_Normalize(Extension, NormalizationForm_KompatibleCompose);
            UTF8 *CaseFolded  = UTF8_CaseFold(Normalized);
            if (UTF8_Compare(CaseFolded, U8("aif")) || UTF8_Compare(CaseFolded, U8("aiff")) || UTF8_Compare(CaseFolded, U8("aifc")) == Yes) {
                CodecID       = CodecID_AIF;
            } else if (UTF8_Compare(CaseFolded, U8("wav")) == Yes) {
                CodecID       = CodecID_WAV;
            } else if (UTF8_Compare(CaseFolded, U8("w64")) == Yes) {
                CodecID       = CodecID_W64;
            } else if (UTF8_Compare(CaseFolded, U8("bmp")) == Yes) {
                CodecID       = CodecID_BMP;
            } else if (UTF8_Compare(CaseFolded, U8("png")) || UTF8_Compare(CaseFolded, U8("apng")) == Yes) {
                CodecID       = CodecID_PNG;
            } else if (UTF8_Compare(CaseFolded, U8("pam")) == Yes) {
                CodecID       = CodecID_PAM;
            } else if (UTF8_Compare(CaseFolded, U8("pbm")) == Yes) {
                CodecID       = CodecID_PBM_B;
            } else if (UTF8_Compare(CaseFolded, U8("pgm")) == Yes) {
                CodecID       = CodecID_PGM_B;
            } else if (UTF8_Compare(CaseFolded, U8("ppm")) == Yes) {
                CodecID       = CodecID_PPM_B;
            } else {
                Log(Log_DEBUG, __func__, U8("Extension \"%s\" is not known"), Extension);
            }
            free(Normalized);
            free(CaseFolded);
        } else {
            Log(Log_DEBUG, __func__, U8("Extension Pointer is NULL"));
        }
        return CodecID;
    }
    
    OVIA_CodecIDs UTF16_Extension2CodecID(UTF16 *Extension) {
        OVIA_CodecIDs CodecID = CodecID_Unknown;
        if (Extension != NULL) {
            UTF16 *Normalized = UTF16_Normalize(Extension, NormalizationForm_KompatibleCompose);
            UTF16 *CaseFolded = UTF16_CaseFold(Normalized);
            if (UTF16_Compare(CaseFolded, U16("aif")) || UTF16_Compare(CaseFolded, U16("aiff")) || UTF16_Compare(CaseFolded, U16("aifc")) == Yes) {
                CodecID       = CodecID_AIF;
            } else if (UTF16_Compare(CaseFolded, U16("wav")) == Yes) {
                CodecID       = CodecID_WAV;
            } else if (UTF16_Compare(CaseFolded, U16("w64")) == Yes) {
                CodecID       = CodecID_W64;
            } else if (UTF16_Compare(CaseFolded, U16("bmp")) == Yes) {
                CodecID       = CodecID_BMP;
            } else if (UTF16_Compare(CaseFolded, U16("png")) || UTF16_Compare(CaseFolded, U16("apng")) == Yes) {
                CodecID       = CodecID_PNG;
            } else if (UTF16_Compare(CaseFolded, U16("pam")) == Yes) {
                CodecID       = CodecID_PAM;
            } else if (UTF16_Compare(CaseFolded, U16("pbm")) == Yes) {
                CodecID       = CodecID_PBM_B;
            } else if (UTF16_Compare(CaseFolded, U16("pgm")) == Yes) {
                CodecID       = CodecID_PGM_B;
            } else if (UTF16_Compare(CaseFolded, U16("ppm")) == Yes) {
                CodecID       = CodecID_PPM_B;
            } else {
                Log(Log_DEBUG, __func__, U8("Extension \"%S\" is not known"), Extension);
            }
            free(Normalized);
            free(CaseFolded);
        } else {
            Log(Log_DEBUG, __func__, U8("Extension Pointer is NULL"));
        }
        return CodecID;
    }
    
    OVIA_CodecIDs IdentifyFileType(BitBuffer *BitB) {
        OVIA_CodecIDs Format      = CodecID_Unknown;
        uint64_t OriginalPosition = BitBuffer_GetPosition(BitB);
        for (uint64_t Decoder = 0ULL; Decoder < OVIA_NumCodecs; Decoder++) {
            BitBuffer_Seek(BitB, Bytes2Bits(GlobalCodecs.Decoders[Decoder].MagicIDOffset));
            for (uint64_t MagicByte = 0ULL; MagicByte < GlobalCodecs.Decoders[Decoder].MagicIDSize; MagicByte++) {
                uint8_t CurrentByte = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8);
                if (CurrentByte != GlobalCodecs.Decoders[Decoder].MagicID[MagicByte]) {
                    break;
                } else if (CurrentByte == GlobalCodecs.Decoders[Decoder].MagicID[MagicByte] && MagicByte == GlobalCodecs.Decoders[Decoder].MagicIDSize) {
                    // The MagicID matches
                    Format = GlobalCodecs.Decoders[Decoder].DecoderID;
                }
            }
        }
        BitBuffer_SetPosition(BitB, OriginalPosition);
        return Format;
    }
    
#ifdef __cplusplus
}
#endif
