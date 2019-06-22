#include "../include/OVIA.h"

#include "../../Dependencies/FoundationIO/libFoundationIO/include/Log.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    /*
     OVIA API:
     
     We don't need a centralized type to contain every codec, we're breaking each codec into it's own module.
     
     What we do need is a way to exchange information between the various formats.
     
     ImageContainers and Audio2DContainers work fine for actual content.
     
     but what about metadata?
     
     How do we share GAMMA metadata or ICC color profiles or song tags between formats?
     
     Should ICC profiles be a part of OVIA or ContainerIO?
     */
    
    /*
     typedef struct OVIACodecs {
     uint64_t          NumMagicIDs[OVIA_NumCodecs];
     uint8_t          *MagicID[OVIA_NumCodecs];
     uint64_t         *MagicIDSize[OVIA_NumCodecs];
     uint64_t         *MagicIDOffset[OVIA_NumCodecs];
     OVIA_MediaTypes   MediaTypes[OVIA_NumCodecs];
     OVIA_CodecTypes   CodecTypes[OVIA_NumCodecs];
     void            (*InitFunction[OVIA_NumCodecs])(void);
     void            (*ParseFunction[OVIA_NumCodecs])(OVIA*, BitBuffer*);
     Audio2DContainer *(*DecodeAudio2D[OVIA_NumCodecs])(OVIA*, BitBuffer*);
     void            (*EncodeAudio2D[OVIA_NumCodecs])(Audio2DContainer*, BitBuffer*);
     ImageContainer *(*DecodeImage[OVIA_NumCodecs])(OVIA*, BitBuffer*);
     void            (*EncodeImage[OVIA_NumCodecs])(ImageContainer*, BitBuffer*);
     } OVIACodecs;
     */
    
    typedef struct OVIADecoder {
        uint64_t            NumMagicIDs;
        uint64_t           *MagicIDSize;   // array containing the size of each magic id
        uint64_t           *MagicIDOffset; // array containing the offset in bytes for each magic id
        uint8_t           **MagicIDs;      // array of pointers to arrays of bytes one for each MagicID
        OVIA_MediaTypes     MediaType;
        void             *(*Function_Init)(void); // Void pointer to each codecs init function
        void              (*Function_Parse)(BitBuffer*);
        Audio2DContainer *(*Function_ReadAudio2D)(BitBuffer*);
        Audio3DContainer *(*Function_ReadAudio3D)(BitBuffer*);
        ImageContainer   *(*Function_ReadImage)(BitBuffer*);
    } OVIADecoder;
    
    typedef struct OVIAEncoder {
        OVIA_MediaTypes     MediaType;
        void             *(*Function_Init)(void); // Void pointer to each codecs init function
        void              (*Function_WriteHeader)(BitBuffer*);
        Audio2DContainer *(*Function_WriteAudio2D)(BitBuffer*);
        AudioVector      *(*Function_WriteAudio3D)(BitBuffer*);
        ImageContainer   *(*Function_WriteImage)(BitBuffer*);
    } OVIAEncoder;
    
    typedef struct OVIACodecs {
        uint64_t     NumCodecs;
        OVIAEncoder *Encoders;
        OVIADecoder *Decoders;
    } OVIACodecs;
    
    extern OVIACodecs *OVIACodecList;
    
    uint64_t OVIACodecs_GetCodecIndex(OVIA_CodecIDs CodecID, OVIA_CodecTypes CodecType) {
        uint64_t Index = (OVIA_NumCodecs * 2) + 1;
        if (CodecID != CodecID_Unknown && CodecType != CodecType_Unknown) {
            Index      = CodecID - CodecType;
        } else if (CodecID == CodecID_Unknown) {
            Log(Log_ERROR, __func__, U8("CodecID_Unknown is invalid"));
        } else if (CodecType == CodecType_Unknown) {
            Log(Log_ERROR, __func__, U8("CodecType_Unknown is invalid"));
        }
        return Index;
    }
    
    uint64_t OVIACodecs_GetMagicIDSize(OVIACodecs Codecs, uint64_t CodecIndex) {
        uint64_t MagicIDSize = (OVIA_NumCodecs * 2) + 1;
        if (CodecIndex < (OVIA_NumCodecs * 2) + 1) {
            MagicIDSize      = Codecs.MagicIDSize[CodecIndex];
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
