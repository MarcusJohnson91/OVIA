#include "../include/EncodeFLAC.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void   FLACWriteStreaminfo(BitOutput *BitO, FLACEncoder *FLAC) {
        WriteBits(BitO, 34, 24); // StreamInfoSize
        WriteBits(BitO, FLAC->Meta->StreamInfo->MinimumBlockSize, 16);
        WriteBits(BitO, FLAC->Meta->StreamInfo->MaximumBlockSize, 16);
        WriteBits(BitO, FLAC->Meta->StreamInfo->MinimumFrameSize, 24);
        WriteBits(BitO, FLAC->Meta->StreamInfo->MaximumFrameSize, 24);
        WriteBits(BitO, FLAC->Meta->StreamInfo->SampleRate, 20);
        WriteBits(BitO, FLAC->Meta->StreamInfo->MinimumFrameSize, 24);
        WriteBits(BitO, FLAC->Meta->StreamInfo->Channels - 1, 3);
        WriteBits(BitO, FLAC->Meta->StreamInfo->BitDepth - 1, 5);
        WriteBits(BitO, FLAC->Meta->StreamInfo->SamplesInStream, 36);
        WriteBits(BitO, 0, 128); // ROOM for the MD5.
    }
    
    void   FLACWriteVorbisComment(BitOutput *BitO, FLACEncoder *FLAC) {
        
    }
    
    void   FLACWriteMetadata(BitOutput *BitO, FLACEncoder *FLAC) {
        WriteBits(BitO, FLACMagic, 32);
        WriteBits(BitO, 0, 1); // IsLastMetadataBlock
        //WriteBits(BitO, FLACMetadataTypes, 7); // MetadataBlockType
        
    }
    
    void   InitFLACEncoder(FLACEncoder *FLAC) {
        FLAC->Meta             = calloc(sizeof(FLACMeta), 1);
        FLAC->Meta->StreamInfo = calloc(sizeof(FLACStreamInfo), 1);
        FLAC->Meta->Seek       = calloc(sizeof(FLACSeekTable), 1);
        FLAC->Meta->Vorbis     = calloc(sizeof(FLACVorbisComment), 1);
        FLAC->Meta->Cue        = calloc(sizeof(FLACCueSheet), 1);
        FLAC->Meta->Pic        = calloc(sizeof(FLACPicture), 1);
        
        FLAC->Data             = calloc(sizeof(FLACData), 1);
        FLAC->Data->Frame      = calloc(sizeof(FLACFrame), 1);
        FLAC->Data->SubFrame   = calloc(sizeof(FLACSubFrame), 1);
        FLAC->Data->LPC        = calloc(sizeof(FLACLPC), 1);
        FLAC->Data->Rice       = calloc(sizeof(RICEPartition), 1);
    }
    
    int8_t EncodeFLAC(PCMFile *PCM, BitOutput *BitO, FLACEncoder *FLAC) {
        if (FLAC->EncodeSubset == true && FLAC->Data->Frame->SampleRate <= 48000) {
            FLAC->MaxBlockSize          =  4608;
            FLAC->MaxFilterOrder        =    12;
            FLAC->MaxRICEPartitionOrder =     8;
        } else {
            FLAC->MaxBlockSize          = 16384;
            FLAC->MaxFilterOrder        =    32;
            FLAC->MaxRICEPartitionOrder =    15;
        }
        
        if (FLAC->OptimizeFile == true) {
            // Optimize this mufucka

        }
        
        return 0;
    }
    
#ifdef __cplusplus
}
#endif
