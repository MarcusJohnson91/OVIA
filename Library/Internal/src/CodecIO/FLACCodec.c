#include "../include/libModernFLAC.h"
#include "../include/Private/ModernFLACTypes.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void CloseFLACEncoder(EncodeFLAC *Enc) {
        free(Enc->Meta->StreamInfo);
        free(Enc->Meta->Seek);
        free(Enc->Meta->Vorbis);
        free(Enc->Meta->Cue);
        free(Enc->Meta->Pic);
        free(Enc->Meta);
        free(Enc->Data->Frame);
        free(Enc->Data->SubFrame);
        free(Enc->Data->LPC);
        free(Enc->Data->Rice);
        free(Enc->Data);
        free(Enc);
    }
    
    void CloseFLACDecoder(DecodeFLAC *Dec) {
        free(Dec->Meta->StreamInfo);
        free(Dec->Meta->Seek);
        free(Dec->Meta->Vorbis);
        free(Dec->Meta->Cue);
        free(Dec->Meta->Pic);
        free(Dec->Meta);
        free(Dec->Data->Frame);
        free(Dec->Data->SubFrame);
        free(Dec->Data->LPC);
        free(Dec->Data->Rice);
        free(Dec->Data);
        free(Dec);
    }
    
#ifdef __cplusplus
}
#endif
