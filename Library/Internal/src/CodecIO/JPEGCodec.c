#include "../../include/Private/JPEGCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void *JPEGOptions_Init(void) {
        void *Options = calloc(1, sizeof(JPEGOptions));
        return Options;
    }
    
    ImageContainer *JPEG_Extract(void *Options, BitBuffer *BitB) {
        ImageContainer *Container = NULL; //ImageContainer_Init(, <#Image_ChannelMask ChannelMask#>, <#uint64_t Width#>, <#uint64_t Height#>);
        return Container;
    }
    
    void JPEGOptions_Deinit(void *Options) {
        JPEGOptions *JPEG = Options;
        free(JPEG);
    }
    
#ifdef __cplusplus
}
#endif

