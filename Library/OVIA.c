#include "OVIA.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
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
     
     ImageCanvass and AudioScape2Ds work fine for actual content.
     
     but what about metadata?
     
     How do we share GAMMA metadata or ICC color profiles or song tags between formats?
     
     Should ICC profiles be a part of OVIA or ContainerIO?
     */
    
    
    /*
    OVIA_CodecIDs OVIA_IdentifyFileType(OVIA *Ovia, BitBuffer *BitB) {
        OVIA_CodecIDs Format      = CodecID_Unspecified;
        uint64_t OriginalPosition = BitBuffer_GetPosition(BitB);
        
        // We need to generatte the header containing the enabled components
        
        for (uint64_t Decoder = 0ULL; Decoder < Ovia->NumDecoders; Decoder++) {
            for (uint64_t MagicID = 0ULL; MagicID < Ovia->Decoders[Decoder].NumMagicIDs; MagicID++) {
                BitBuffer_Seek(BitB, Ovia->Decoders[Decoder].MagicIDOffsetInBits[MagicID]);
                for (uint64_t MagicIDByte = 0ULL; MagicIDByte < Ovia->Decoders[Decoder].MagicIDSizeInBits[Decoder]; MagicIDByte++) {
                    uint8_t ExtractedByte = BitBuffer_ReadBits(BitB, ByteOrder_Right2Left, BitOrder_Right2Left, 8); // Should we put a bit order field in the magic id thing?
                    if (ExtractedByte != Ovia->Decoders[Decoder].MagicID[MagicIDByte]) {
                        break;
                    } else if (MagicIDByte + 1 == Ovia->Decoders[Decoder].MagicIDSizeInBits[Decoder] && ExtractedByte == Ovia->Decoders[Decoder].MagicID[MagicIDByte]) {
                        Format = Ovia->Decoders[Decoder].DecoderID;
                    }
                }
                BitBuffer_Seek(BitB, OriginalPosition);
            }
        }
        BitBuffer_SetPosition(BitB, OriginalPosition);
        return Format;
    }
    */
    /*
     We need to create a general purpose function for each decoder that registers each codec.
     
     void OVIARegisterDecoder_AIF(OVIADecoder *Decoder) {
     Decoder.MagicID = AIFMagicID;
     ... ETC
     }
     
     Then we need OVIA to provide a basic function that will register the  codec by calling a parameter function pointer
     void (*Function_RegisterDecoder)(OVIACodecs, OVIADecoder*);
     
     void OVIARegister_Decoder(void (*Function_RegisterDecoder)(OVIADecoder* Decoder)) {
     Function_RegisterDecoder(Decoder);
     }
     
     I need a function that will call a function
     */
    /*
    void OVIA_Deinit(OVIA *Ovia) {
        if (Ovia != NULL) {
            free(Ovia->Encoders);
            free(Ovia->Decoders);
        }
    }
    
    OVIA *OVIA_Init(void) {
        OVIA *Ovia            = calloc(1, sizeof(OVIA));
        if (Ovia != NULL) {
            Ovia->Encoders    = calloc(OVIA_NumCodecs, sizeof(OVIAEncoder));
            Ovia->Decoders    = calloc(OVIA_NumCodecs, sizeof(OVIADecoder));
            Ovia->NumDecoders = OVIA_NumCodecs;
            Ovia->NumEncoders = OVIA_NumCodecs;
        } else {
            OVIA_Deinit(Ovia);
        }
        return Ovia;
    }
     */
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
