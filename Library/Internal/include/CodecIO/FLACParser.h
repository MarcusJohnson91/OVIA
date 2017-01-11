//#include "../include/libModernFLAC.h"
#include "../include/DecodeFLAC.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void FLACParseStreamInfo(BitInput *BitI, FLACDecoder *FLAC);
    
    void FLACSkipPadding(BitInput *BitI, FLACDecoder *FLAC);
    
    void FLACSkipCustom(BitInput *BitI, FLACDecoder *FLAC);
    
    void FLACParseSeekTable(BitInput *BitI, FLACDecoder *FLAC);
    
    void FLACParseVorbisComment(BitInput *BitI, FLACDecoder *FLAC);
    
    void FLACParseCuesheet(BitInput *BitI, FLACDecoder *FLAC);
    
    void FLACParsePicture(BitInput *BitI, FLACDecoder *FLAC);
    
    void FLACParseMetadata(BitInput *BitI, FLACDecoder *FLAC);
    
#ifdef __cplusplus
}
#endif
