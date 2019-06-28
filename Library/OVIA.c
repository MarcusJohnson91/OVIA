#include "../include/Private/OVIACommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void OVIA_RegisterDecoder(OVIADecoder Decoder) {
        OVIA_CodecIDs DecoderID                                      = Decoder.DecoderID;
        GlobalCodecs.Decoders[DecoderID].MediaType                   = Decoder.MediaType;
        GlobalCodecs.Decoders[DecoderID].Function_Initialize         = Decoder.Function_Initialize;
        GlobalCodecs.Decoders[DecoderID].Function_Parse              = Decoder.Function_Parse;
        GlobalCodecs.Decoders[DecoderID].Function_Decode             = Decoder.Function_Decode;
        GlobalCodecs.Decoders[DecoderID].MagicIDOffset               = Decoder.MagicIDOffset;
        GlobalCodecs.Decoders[DecoderID].MagicIDSize                 = Decoder.MagicIDSize;
        GlobalCodecs.Decoders[DecoderID].MagicID                     = Decoder.MagicID;
    }
    
    void OVIA_RegisterEncoder(OVIAEncoder Encoder) {
        uint64_t EncoderID                                           = Encoder.EncoderID;
        GlobalCodecs.Encoders[EncoderID].MediaType                   = Encoder.MediaType;
        GlobalCodecs.Encoders[EncoderID].Function_Initialize         = Encoder.Function_Initialize;
        GlobalCodecs.Encoders[EncoderID].Function_WriteHeader        = Encoder.Function_WriteHeader;
        GlobalCodecs.Encoders[EncoderID].Function_Encode             = Encoder.Function_Encode;
        GlobalCodecs.Encoders[EncoderID].Function_WriteFooter        = Encoder.Function_WriteFooter;
    }
    
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
    
    uint64_t OVIACodecs_GetCodecIndex(OVIA_CodecIDs CodecID, OVIA_CodecTypes CodecType) {
        uint64_t Index = GlobalCodecs.NumDecoders + GlobalCodecs.NumEncoders + 1;
        if (CodecID != CodecID_Unknown && CodecType != CodecType_Unknown) {
            if (CodecType == CodecType_Decode) {
                for (uint64_t Decoder = 0ULL; Decoder < GlobalCodecs.NumDecoders; Decoder++) {
                    if (GlobalCodecs.Decoders[Decoder].CodecID == CodecID) {
                        Index = Decoder;
                    }
                }
            } else if (CodecType == CodecType_Encode) {
                for (uint64_t Encoder = 0ULL; Encoder < GlobalCodecs.NumEncoders; Encoder++) {
                    if (GlobalCodecs.Encoders[Encoder].CodecID == CodecID) {
                        Index = Encoder;
                    }
                }
            }
            Index      = CodecID - CodecType;
        } else if (CodecID == CodecID_Unknown) {
            Log(Log_ERROR, __func__, U8("CodecID_Unknown is invalid"));
        } else if (CodecType == CodecType_Unknown) {
            Log(Log_ERROR, __func__, U8("CodecType_Unknown is invalid"));
        }
        return Index;
    }
    
    uint64_t OVIACodecs_GetMagicIDSize(OVIACodecs *Codecs, OVIA_CodecIDs CodecID, uint64_t MagicIDIndex) {
        uint64_t MagicIDSize = 0ULL;
        if (Codecs != NULL) {
            for (uint64_t Codec = 0ULL; Codec < Codecs->NumCodecs; Codec++) {
                if (Codecs->Decoders[Codec].CodecID == CodecID) {
                    if (Codecs->Decoders[Codec].NumMagicIDs <= MagicIDIndex) {
                        MagicIDSize = Codecs->Decoders[Codec].MagicIDSize[MagicIDIndex];
                    }
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("OVIACodecs Pointer is NULL"));
        }
        return MagicIDSize;
    }
    
    uint64_t OVIACodecs_GetMagicIDOffset(OVIACodecs Codecs, uint64_t CodecIndex) {
        uint64_t MagicIDOffset = (OVIA_NumCodecs * 2) + 1;
        if (CodecIndex < (OVIA_NumCodecs * 2) + 1) {
            MagicIDOffset      = Codecs.MagicIDOffset[CodecIndex];
        }
        return MagicIDOffset;
    }
    
    uint8_t *OVIACodecs_GetMagicID(OVIACodecs Codecs, uint64_t CodecIndex) {
        uint8_t *MagicIDOffset = NULL;
        if (CodecIndex < (OVIA_NumCodecs * 2) + 1) {
            MagicIDOffset      = Codecs.MagicID[CodecIndex];
        }
        return MagicIDOffset;
    }
    
#ifdef __cplusplus
}
#endif
