#include "../include/Private/InternalOVIA.h"

#include <assert.h>

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
