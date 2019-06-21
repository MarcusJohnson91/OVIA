#include "../../include/Private/Audio/WAVCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void OVIA_W64_SetSpeakerMask(OVIA *Ovia, uint64_t SpeakerMask) {
        if (Ovia != NULL) {
            Ovia->W64Info->SpeakerMask = SpeakerMask;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_W64_SetCompressionType(OVIA *Ovia, uint16_t CompressionType) {
        if (Ovia != NULL) {
            Ovia->W64Info->CompressionFormat = CompressionType;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_W64_SetBlockAlignment(OVIA *Ovia, uint16_t BlockAlignment) {
        if (Ovia != NULL) {
            Ovia->W64Info->BlockAlignment = BlockAlignment;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint16_t OVIA_W64_GetBlockAlignment(OVIA *Ovia) {
        uint16_t BlockAlignment = 0;
        if (Ovia != NULL) {
            BlockAlignment = Ovia->W64Info->BlockAlignment;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return BlockAlignment;
    }
    
#ifdef __cplusplus
}
#endif
