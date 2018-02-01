#include "../../include/Private/Encode/libModernFLAC_Compose.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void   FLACWriteStreaminfo(BitBuffer *OutputFLAC, EncodeFLAC *Enc) {
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputFLAC, 24, 34); // StreamInfoSize
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputFLAC, 16, Enc->Meta->StreamInfo->MinimumBlockSize);
       	WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputFLAC, 16, Enc->Meta->StreamInfo->MaximumBlockSize);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputFLAC, 24, Enc->Meta->StreamInfo->MinimumFrameSize);
       	WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputFLAC, 24, Enc->Meta->StreamInfo->MaximumFrameSize);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputFLAC, 20, Enc->Meta->StreamInfo->SampleRate);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputFLAC, 24, Enc->Meta->StreamInfo->MinimumFrameSize);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputFLAC,  3, Enc->Meta->StreamInfo->Channels - 1);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputFLAC,  5, Enc->Meta->StreamInfo->BitDepth - 1);
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputFLAC, 36, Enc->Meta->StreamInfo->SamplesInStream);
        WriteGUUID(BitIOBinaryGUID, OutputFLAC, 0x00000000000000000000000000000000); // ROOM for the MD5.
    }
    
    void   FLACWriteVorbisComment(BitBuffer *OutputFLAC, EncodeFLAC *Enc) {
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputFLAC, 32, Enc->Meta->Vorbis->VendorTagSize);
        for (uint32_t TagByte = 0; TagByte < Enc->Meta->Vorbis->VendorTagSize; TagByte++) {
            WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputFLAC, 8, Enc->Meta->Vorbis->VendorTag[TagByte]);
        }
        WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputFLAC, 32, Enc->Meta->Vorbis->NumUserTags);
        for (uint32_t UserTag = 0; UserTag < Enc->Meta->Vorbis->NumUserTags; UserTag++) {
            WriteBits(BitIOMSByteFirst, BitIOLSBitFirst, OutputFLAC, 8, Enc->Meta->Vorbis->UserTagSize);
        }
    }
    
#ifdef __cplusplus
}
#endif
