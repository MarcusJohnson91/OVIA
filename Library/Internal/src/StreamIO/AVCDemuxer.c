#include "../../../Dependencies/BitIO/libBitIO/include/BitIO.h"

#include "../../include/ModernAVCTypes.h"
#include "../../include/libModernAVC.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    // In this file we need to process a .avc or .h264 file into slice packets.
    
    // How does MPEG2Stream fit in here? Well, it parses MPEG2*Streams and outputs NALPackets.
    
    // My raw file parser, also needs to output NALPackets
    
#ifdef __cplusplus
}
#endif
