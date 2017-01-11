#include "../include/libModernFLAC.h"
#include "../include/ParseFLAC.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void FLACParseStreamInfo(BitInput *BitI, FLACFile *FLAC) {
        FLAC->Meta->StreamInfo->MinimumBlockSize        = ReadBits(BitI, 16);
        FLAC->Meta->StreamInfo->MaximumBlockSize        = ReadBits(BitI, 16);
        FLAC->Meta->StreamInfo->MinimumFrameSize        = ReadBits(BitI, 24);
        FLAC->Meta->StreamInfo->MaximumFrameSize        = ReadBits(BitI, 24);
        FLAC->Meta->StreamInfo->SampleRate              = ReadBits(BitI, 20);
        FLAC->Meta->StreamInfo->Channels                = ReadBits(BitI, 3) + 1;
        FLAC->Meta->StreamInfo->BitDepth                = ReadBits(BitI, 5) + 1;
        FLAC->Meta->StreamInfo->SamplesInStream         = ReadBits(BitI, 36);
        for (uint8_t MD5Byte = 0; MD5Byte < 16; MD5Byte++) {
            FLAC->Meta->StreamInfo->MD5[MD5Byte] = ReadBits(BitI, 8); // MD5 = 0x58B4285B9CFD75081C9E6A3B4C9E0D28
        }
    }
    
    void FLACSkipPadding(BitInput *BitI, FLACFile *FLAC) {
        SkipBits(BitI, Bytes2Bits(FLAC->Meta->MetadataSize));
    }
    
    void FLACSkipCustom(BitInput *BitI, FLACFile *FLAC) { // 134,775
        SkipBits(BitI, Bytes2Bits(FLAC->Meta->MetadataSize + 1));
    }
    
    void FLACParseSeekTable(BitInput *BitI, FLACFile *FLAC) { // 378
        FLAC->Meta->Seek->NumSeekPoints = FLAC->Meta->MetadataSize / 18; // 21
        
        for (uint16_t SeekPoint = 0; SeekPoint < FLAC->Meta->Seek->NumSeekPoints; SeekPoint++) {
            FLAC->Meta->Seek->SampleInTargetFrame[SeekPoint]        = ReadBits(BitI, 64);
            FLAC->Meta->Seek->OffsetFrom1stSample[SeekPoint]        = ReadBits(BitI, 64);
            FLAC->Meta->Seek->TargetFrameSize[SeekPoint]            = ReadBits(BitI, 16);
        }
    }
    
    void FLACParseVorbisComment(BitInput *BitI, FLACFile *FLAC) { // LITTLE ENDIAN
        uint32_t UserCommentSize[FLACMaxVorbisComments] = {0};
        
        uint32_t VendorTagSize = SwapEndian32(ReadBits(BitI, 32));
        for (uint32_t TagByte = 0; TagByte < VendorTagSize; TagByte++) {
            FLAC->Meta->Vorbis->VendorString[TagByte]  = ReadBits(BitI, 8); // reference libFLAC 1.3.1 20141125
        }
        
        uint32_t NumUserComments = SwapEndian32(ReadBits(BitI, 32)); // 28
        for (uint32_t UserComment = 0; UserComment < NumUserComments; UserComment++) {
            UserCommentSize[UserComment] = SwapEndian32(ReadBits(BitI, 32)); // 28
            for (uint32_t TagByte = 0; TagByte < UserCommentSize[UserComment]; TagByte++) {
                FLAC->Meta->Vorbis->UserCommentString[UserComment][TagByte] = ReadBits(BitI, 8);
            }
        }
    }
    
    void FLACParseCuesheet(BitInput *BitI, FLACFile *FLAC) {
        for (uint8_t CatalogChar = 0; CatalogChar < FLACMedizCatalogNumberSize; CatalogChar++) {
            FLAC->Meta->Cue->CatalogID[CatalogChar] = ReadBits(BitI, 8);
        }
        FLAC->Meta->Cue->LeadIn = ReadBits(BitI, 64);
        FLAC->Meta->Cue->IsCD   = ReadBits(BitI, 1);
        SkipBits(BitI, 2071); // Reserved
        FLAC->Meta->Cue->NumTracks = ReadBits(BitI, 8);
        
        for (uint8_t Track = 0; Track < FLAC->Meta->Cue->NumTracks; Track++) {
            FLAC->Meta->Cue->TrackOffset[Track]  = ReadBits(BitI, 64);
            FLAC->Meta->Cue->TrackNum[Track]     = ReadBits(BitI, 64);
            
            for (uint8_t ISRCByte = 0; ISRCByte < FLACISRCSize; ISRCByte++) {
                FLAC->Meta->Cue->ISRC[Track][ISRCByte]  = ReadBits(BitI, 8);
            }
            FLAC->Meta->Cue->IsAudio[Track] = ReadBits(BitI, 1);
            FLAC->Meta->Cue->PreEmphasis[Track] = ReadBits(BitI, 1);
            SkipBits(BitI, 152); // Reserved, all 0
            FLAC->Meta->Cue->NumTrackIndexPoints[Track] = ReadBits(BitI, 8);
        }
        
        FLAC->Meta->Cue->IndexOffset    = ReadBits(BitI, 64);
        FLAC->Meta->Cue->IndexPointNum  = ReadBits(BitI, 8);
        SkipBits(BitI, 24); // Reserved
    }
    
    void FLACParsePicture(BitInput *BitI, FLACFile *FLAC) { // 17,151
        FLAC->Meta->Pic->PicType  = ReadBits(BitI, 32); // 3
        FLAC->Meta->Pic->MIMESize = ReadBits(BitI, 32); // 10
        for (uint32_t MIMEByte = 0; MIMEByte < FLAC->Meta->Pic->MIMESize; MIMEByte++) {
            FLAC->Meta->Pic->MIMEString[MIMEByte] = ReadBits(BitI, 8); // image/jpeg
        }
        FLAC->Meta->Pic->PicDescriptionSize = ReadBits(BitI, 32); // 0
        for (uint32_t Char = 0; Char < FLAC->Meta->Pic->PicDescriptionSize; Char++) {
            FLAC->Meta->Pic->PicDescriptionString[Char] = ReadBits(BitI, 8);
        }
        FLAC->Meta->Pic->Width       = ReadBits(BitI, 32); // 1144
        FLAC->Meta->Pic->Height      = ReadBits(BitI, 32); // 1144
        FLAC->Meta->Pic->BitDepth    = ReadBits(BitI, 32); // 24, PER PIXEL, NOT SUBPIXEL
        FLAC->Meta->Pic->ColorsUsed  = ReadBits(BitI, 32); // 0
        FLAC->Meta->Pic->PictureSize = ReadBits(BitI, 32); // 17,109
                                                           // Pop in the address of the start of the data, and skip over the data instead of buffering it.
    }
    
    void FLACParseMetadata(BitInput *BitI, FLACFile *FLAC) {
        bool     IsLastMetadataBlock     = ReadBits(BitI, 1);
        uint8_t  MetadataBlockType       = ReadBits(BitI, 7);
        FLAC->Meta->MetadataSize         = ReadBits(BitI, 24); // Does NOT count the 2 fields above.
        char Description[BitIOStringSize];
        
        switch (MetadataBlockType) {
            case StreamInfo:
                FLACParseStreamInfo(BitI, FLAC);
                break;
            case Padding:
                FLACSkipPadding(BitI, FLAC);
                break;
            case Custom:
                FLACSkipCustom(BitI, FLAC);
                break;
            case SeekTable:
                FLACParseSeekTable(BitI, FLAC);
                break;
            case VorbisComment:
                FLACParseVorbisComment(BitI, FLAC);
                break;
            case Cuesheet:
                FLACParseCuesheet(BitI, FLAC);
                break;
            case Picture:
                FLACParsePicture(BitI, FLAC);
                break;
            default:
                snprintf(Description, BitIOStringSize, "Invalid Metadata Type: %d\n", MetadataBlockType);
                Log(SYSWarning, "ModernFLAC", "FLACParseMetadata", Description);
                break;
        }
    }
    
    
#ifdef __cplusplus
}
#endif
