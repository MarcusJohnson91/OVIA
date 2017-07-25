#include "../../include/libModernPNG.h"
#include "../../include/ModernPNGTypes.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void IsAnimatedPNG(EncodePNG *Enc, const bool IsAnimatedPNG) {
        if (Enc == NULL) {
            Log(LOG_ERR, "libModernPNG", "ComposePNGHeight", "Pointer to EncodePNG is NULL\n");
        } else {
            Enc->IsAnimatedPNG = IsAnimatedPNG;
        }
    }
    
    void ComposeAPNGNumFrames(EncodePNG *Enc, const uint32_t NumFrames) {
        if (Enc == NULL) {
            Log(LOG_ERR, "libModernPNG", "ComposeAPNGNumFrames", "Pointer to EncodePNG is NULL\n");
        } else {
            Enc->acTL->NumFrames = NumFrames;
        }
    }
    
    void ComposeAPNGTimes2Loop(EncodePNG *Enc, const uint32_t NumTimes2Loop) {
        if (Enc == NULL) {
            Log(LOG_ERR, "libModernPNG", "ComposeAPNGTimes2Loop", "Pointer to EncodePNG is NULL\n");
        } else {
            Enc->acTL->TimesToLoop = NumTimes2Loop;
        }
    }
    
    void ComposeAPNGFrameDelay(EncodePNG *Enc, const uint32_t FrameDelayNumerator, const uint32_t FrameDelayDenominator) {
        if (Enc == NULL) {
            Log(LOG_ERR, "libModernPNG", "ComposeAPNGFrameDelay", "Pointer to EncodePNG is NULL\n");
        } else {
            Enc->fcTL->FrameDelayNumerator   = FrameDelayNumerator;
            Enc->fcTL->FrameDelayDenominator = FrameDelayDenominator;
        }
    }
    
    void ComposePNGWidth(EncodePNG *Enc, const uint32_t ImageWidth) {
        if (Enc == NULL) {
            Log(LOG_ERR, "libModernPNG", "ComposePNGWidth", "Pointer to EncodePNG is NULL\n");
        } else {
            Enc->iHDR->Width       = ImageWidth;
            if (Enc->IsAnimatedPNG) {
                Enc->fcTL->Width   = ImageWidth;
            }
        }
    }
    
    void ComposePNGHeight(EncodePNG *Enc, const uint32_t ImageHeight) {
        if (Enc == NULL) {
            Log(LOG_ERR, "libModernPNG", "ComposePNGHeight", "Pointer to EncodePNG is NULL\n");
        } else {
            Enc->iHDR->Height      = ImageHeight;
            if (Enc->IsAnimatedPNG) {
                Enc->fcTL->Height  = ImageHeight;
            }
        }
    }
    
#ifdef __cplusplus
}
#endif
