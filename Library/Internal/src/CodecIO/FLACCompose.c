#include "../../include/Private/Encode/libModernFLAC_Compose.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void   FLACWriteStreaminfo(BitBuffer *OutputFLAC, EncodeFLAC *Enc) {
        WriteBits(BitIOMSByte, BitIOLSBit, OutputFLAC, 24, 34); // StreamInfoSize
        WriteBits(BitIOMSByte, BitIOLSBit, OutputFLAC, 16, Enc->Meta->StreamInfo->MinimumBlockSize);
       	WriteBits(BitIOMSByte, BitIOLSBit, OutputFLAC, 16, Enc->Meta->StreamInfo->MaximumBlockSize);
        WriteBits(BitIOMSByte, BitIOLSBit, OutputFLAC, 24, Enc->Meta->StreamInfo->MinimumFrameSize);
       	WriteBits(BitIOMSByte, BitIOLSBit, OutputFLAC, 24, Enc->Meta->StreamInfo->MaximumFrameSize);
        WriteBits(BitIOMSByte, BitIOLSBit, OutputFLAC, 20, Enc->Meta->StreamInfo->SampleRate);
        WriteBits(BitIOMSByte, BitIOLSBit, OutputFLAC, 24, Enc->Meta->StreamInfo->MinimumFrameSize);
        WriteBits(BitIOMSByte, BitIOLSBit, OutputFLAC,  3, Enc->Meta->StreamInfo->Channels - 1);
        WriteBits(BitIOMSByte, BitIOLSBit, OutputFLAC,  5, Enc->Meta->StreamInfo->BitDepth - 1);
        WriteBits(BitIOMSByte, BitIOLSBit, OutputFLAC, 36, Enc->Meta->StreamInfo->SamplesInStream);
        WriteGUUID(BitIOBinaryGUID, OutputFLAC, 0x00000000000000000000000000000000); // ROOM for the MD5.
    }
    
    void   FLACWriteVorbisComment(BitBuffer *OutputFLAC, EncodeFLAC *Enc) {
        WriteBits(BitIOMSByte, BitIOLSBit, OutputFLAC, 32, Enc->Meta->Vorbis->VendorTagSize);
        for (uint32_t TagByte = 0; TagByte < Enc->Meta->Vorbis->VendorTagSize; TagByte++) {
            WriteBits(BitIOMSByte, BitIOLSBit, OutputFLAC, 8, Enc->Meta->Vorbis->VendorTag[TagByte]);
        }
        WriteBits(BitIOMSByte, BitIOLSBit, OutputFLAC, 32, Enc->Meta->Vorbis->NumUserTags);
        for (uint32_t UserTag = 0; UserTag < Enc->Meta->Vorbis->NumUserTags; UserTag++) {
            WriteBits(BitIOMSByte, BitIOLSBit, OutputFLAC, 8, Enc->Meta->Vorbis->UserTagSize);
        }
    }
    
#ifdef __cplusplus
}
#endif
