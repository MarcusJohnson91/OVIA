#include "../../include/Private/Audio/WAVCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void OVIA_WAV_SetSpeakerMask(OVIA *Ovia, uint32_t SpeakerMask) {
        if (Ovia != NULL) {
            Ovia->WAVInfo->SpeakerMask = SpeakerMask;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_WAV_SetCompressionType(OVIA *Ovia, uint16_t CompressionType) {
        if (Ovia != NULL) {
            Ovia->WAVInfo->CompressionFormat = CompressionType;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_WAV_SetBlockAlignment(OVIA *Ovia, uint16_t BlockAlignment) {
        if (Ovia != NULL) {
            Ovia->WAVInfo->BlockAlignment = BlockAlignment;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint16_t OVIA_WAV_GetBlockAlignment(OVIA *Ovia) {
        uint16_t BlockAlignment = 0;
        if (Ovia != NULL) {
            BlockAlignment = Ovia->WAVInfo->BlockAlignment;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return BlockAlignment;
    }
    
#ifdef __cplusplus
}
#endif
