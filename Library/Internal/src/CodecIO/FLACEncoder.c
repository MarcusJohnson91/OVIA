#include "../../../Dependencies/BitIO/libBitIO/include/BitIO.h"
#include "../../include/libModernFLAC.h"
#include "../../include/Encoder/EncodeFLAC.h"
#include "../../include/FLACTypes.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    struct FLACEncoder {
        bool      EncodeSubset;
        bool      OptimizeFile;
        uint16_t  MaxBlockSize;
        uint8_t   MaxFilterOrder;
        uint8_t   MaxRICEPartitionOrder;
        FLACMeta *Meta;
        FLACData *Data;
        int64_t   RawSamples[FLACMaxSamplesInBlock];
    };
    
    void   FLACWriteStreaminfo(BitOutput *BitO, EncodeFLAC *Enc) {
        WriteBits(BitO, 34, 24, true); // StreamInfoSize
        WriteBits(BitO, FLAC->Meta->StreamInfo->MinimumBlockSize, 16, true);
        WriteBits(BitO, FLAC->Meta->StreamInfo->MaximumBlockSize, 16, true);
        WriteBits(BitO, FLAC->Meta->StreamInfo->MinimumFrameSize, 24, true);
        WriteBits(BitO, FLAC->Meta->StreamInfo->MaximumFrameSize, 24, true);
        WriteBits(BitO, FLAC->Meta->StreamInfo->SampleRate, 20, true);
        WriteBits(BitO, FLAC->Meta->StreamInfo->MinimumFrameSize, 24, true);
        WriteBits(BitO, FLAC->Meta->StreamInfo->Channels - 1, 3, true);
        WriteBits(BitO, FLAC->Meta->StreamInfo->BitDepth - 1, 5, true);
        WriteBits(BitO, FLAC->Meta->StreamInfo->SamplesInStream, 36, true);
        WriteBits(BitO, 0, 128, true); // ROOM for the MD5.
    }
    
    void   FLACWriteVorbisComment(BitOutput *BitO, EncodeFLAC *Enc) {
        
    }
    
    void   FLACWriteMetadata(BitOutput *BitO, EncodeFLAC *Enc) {
        WriteBits(BitO, FLACMagic, 32);
        WriteBits(BitO, 0, 1); // IsLastMetadataBlock
        //WriteBits(BitO, FLACMetadataTypes, 7); // MetadataBlockType
        
    }
    
    EncodeFLAC *InitEncodeFLAC(void) {
        EncodeFLAC *Enc       = calloc(sizeof(EncodeFLAC), 1);
        Enc->Meta              = calloc(sizeof(FLACMeta), 1);
        Enc->Meta->StreamInfo  = calloc(sizeof(FLACStreamInfo), 1);
        Enc->Meta->Seek        = calloc(sizeof(FLACSeekTable), 1);
        Enc->Meta->Vorbis      = calloc(sizeof(FLACVorbisComment), 1);
        Enc->Meta->Cue         = calloc(sizeof(FLACCueSheet), 1);
        Enc->Meta->Pic         = calloc(sizeof(FLACPicture), 1);
        
        Enc->Data              = calloc(sizeof(FLACData), 1);
        Enc->Data->Frame       = calloc(sizeof(FLACFrame), 1);
        Enc->Data->SubFrame    = calloc(sizeof(FLACSubFrame), 1);
        Enc->Data->LPC         = calloc(sizeof(FLACLPC), 1);
        Enc->Data->Rice        = calloc(sizeof(RICEPartition), 1);
        return Enc;
    }
    
    int8_t EncodeFLACFile(PCMFile *PCM, BitOutput *BitO, EncodeFLAC *Enc) {
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
