#include "../../../Dependencies/libPCM/Dependencies/BitIO/libBitIO/include/BitIO.h"
#include "../../../Dependencies/libPCM/libPCM/include/libPCM.h"
#include "../../include/libModernFLAC.h"
#include "../../include/Encoder/EncodeFLAC.h"
#include "../../include/FLACTypes.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void   FLACWriteStreaminfo(BitOutput *BitO, EncodeFLAC *Enc) {
        WriteBits(BitO, 34, 24, true); // StreamInfoSize
        WriteBits(BitO, Enc->Meta->StreamInfo->MinimumBlockSize, 16, true);
        WriteBits(BitO, Enc->Meta->StreamInfo->MaximumBlockSize, 16, true);
        WriteBits(BitO, Enc->Meta->StreamInfo->MinimumFrameSize, 24, true);
        WriteBits(BitO, Enc->Meta->StreamInfo->MaximumFrameSize, 24, true);
        WriteBits(BitO, Enc->Meta->StreamInfo->SampleRate, 20, true);
        WriteBits(BitO, Enc->Meta->StreamInfo->MinimumFrameSize, 24, true);
        WriteBits(BitO, Enc->Meta->StreamInfo->Channels - 1, 3, true);
        WriteBits(BitO, Enc->Meta->StreamInfo->BitDepth - 1, 5, true);
        WriteBits(BitO, Enc->Meta->StreamInfo->SamplesInStream, 36, true);
        WriteBits(BitO, 0, 128, true); // ROOM for the MD5.
    }
    
    void   FLACWriteVorbisComment(BitOutput *BitO, EncodeFLAC *Enc) {
        
    }
    
    void   FLACWriteMetadata(BitOutput *BitO, EncodeFLAC *Enc) {
        WriteBits(BitO, FLACMagic, 32, true);
        WriteBits(BitO, false, 1, true); // IsLastMetadataBlock
        //WriteBits(BitO, FLACMetadataTypes, 7); // MetadataBlockType
        
    }
    
    EncodeFLAC *InitEncodeFLAC(void) {
        EncodeFLAC *Enc        = calloc(1, sizeof(EncodeFLAC));
        Enc->Meta              = calloc(1, sizeof(FLACMeta));
        Enc->Meta->StreamInfo  = calloc(1, sizeof(FLACStreamInfo));
        Enc->Meta->Seek        = calloc(1, sizeof(FLACSeekTable));
        Enc->Meta->Vorbis      = calloc(1, sizeof(FLACVorbisComment));
        Enc->Meta->Cue         = calloc(1, sizeof(FLACCueSheet));
        Enc->Meta->Pic         = calloc(1, sizeof(FLACPicture));
        
        Enc->Data              = calloc(1, sizeof(FLACData));
        Enc->Data->Frame       = calloc(1, sizeof(FLACFrame));
        Enc->Data->SubFrame    = calloc(1, sizeof(FLACSubFrame));
        Enc->Data->LPC         = calloc(1, sizeof(FLACLPC));
        Enc->Data->Rice        = calloc(1, sizeof(RICEPartition));
        return Enc;
    }
    
    int8_t EncodeFLACFile(PCMFile *PCM, BitOutput *BitO, EncodeFLAC *Enc) {
        if (Enc->EncodeSubset == true && Enc->Data->Frame->SampleRate <= 48000) {
            Enc->MaxBlockSize          =  4608;
            Enc->MaxFilterOrder        =    12;
            Enc->MaxRICEPartitionOrder =     8;
        } else {
            Enc->MaxBlockSize          = 16384;
            Enc->MaxFilterOrder        =    32;
            Enc->MaxRICEPartitionOrder =    15;
        }
        
        if (Enc->OptimizeFile == true) {
            // Optimize this mufucka

        }
        
        return 0;
    }
    
#ifdef __cplusplus
}
#endif
