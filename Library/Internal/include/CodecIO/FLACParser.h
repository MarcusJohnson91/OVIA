#include "../include/libModernFLAC.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void FLACParseStreamInfo(BitInput *BitI, FLACFile *FLAC);
    
    void FLACSkipPadding(BitInput *BitI, FLACFile *FLAC);
    
    void FLACSkipCustom(BitInput *BitI, FLACFile *FLAC);
    
    void FLACParseSeekTable(BitInput *BitI, FLACFile *FLAC);
    
    void FLACParseVorbisComment(BitInput *BitI, FLACFile *FLAC);
    
    void FLACParseCuesheet(BitInput *BitI, FLACFile *FLAC);
    
    void FLACParsePicture(BitInput *BitI, FLACFile *FLAC);
    
    void FLACParseMetadata(BitInput *BitI, FLACFile *FLAC);
    
#ifdef __cplusplus
}
#endif
