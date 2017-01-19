#include "../include/libModernFLAC.h"
#include "../include/ParseFLAC.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    int8_t FLACParseMetadata(BitInput *BitI, FLACDecoder *FLAC) {
        bool     IsLastMetadataBlock     = ReadBits(BitI, 1); // true
        uint8_t  MetadataBlockType       = ReadBits(BitI, 7); // 1
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
        return IsLastMetadataBlock;
    }
    
    void FLACParseStreamInfo(BitInput *BitI, FLACDecoder *FLAC) {
        FLAC->Meta->StreamInfo->MinimumBlockSize        = ReadBits(BitI, 16); // 0x4000, 16384
        FLAC->Meta->StreamInfo->MaximumBlockSize        = ReadBits(BitI, 16); // 0x4000, 18384
        FLAC->Meta->StreamInfo->MinimumFrameSize        = ReadBits(BitI, 24); // 12
        FLAC->Meta->StreamInfo->MaximumFrameSize        = ReadBits(BitI, 24); // 12
        FLAC->Meta->StreamInfo->SampleRate              = ReadBits(BitI, 20); // 48,000
        FLAC->Meta->StreamInfo->Channels                = ReadBits(BitI, 3) + 1; // 1
        FLAC->Meta->StreamInfo->BitDepth                = ReadBits(BitI, 5) + 1; // 24
        FLAC->Meta->StreamInfo->SamplesInStream         = ReadBits(BitI, 36); // 16384
        for (uint8_t MD5Byte = 0; MD5Byte < 16; MD5Byte++) {
            FLAC->Meta->StreamInfo->MD5[MD5Byte] = ReadBits(BitI, 8); // MD5 = 0xA36961002FA9736BE71CA42AD657C8D5
        }
    }
    
    void FLACSkipPadding(BitInput *BitI, FLACDecoder *FLAC) { // 8192
        SkipBits(BitI, Bytes2Bits(FLAC->Meta->MetadataSize));
    }
    
    void FLACSkipCustom(BitInput *BitI, FLACDecoder *FLAC) { // 134,775
        SkipBits(BitI, Bytes2Bits(FLAC->Meta->MetadataSize + 1));
    }
    
    void FLACParseSeekTable(BitInput *BitI, FLACDecoder *FLAC) { // 18
        FLAC->Meta->Seek->NumSeekPoints = FLAC->Meta->MetadataSize / 18; // 21
        
        for (uint16_t SeekPoint = 0; SeekPoint < FLAC->Meta->Seek->NumSeekPoints; SeekPoint++) {
            FLAC->Meta->Seek->SampleInTargetFrame[SeekPoint]        = ReadBits(BitI, 64); // 0
            FLAC->Meta->Seek->OffsetFrom1stSample[SeekPoint]        = ReadBits(BitI, 64); // 0
            FLAC->Meta->Seek->TargetFrameSize[SeekPoint]            = ReadBits(BitI, 16); // 16384
        }
    }
    
    void FLACParseVorbisComment(BitInput *BitI, FLACDecoder *FLAC) { // LITTLE ENDIAN
        char Description[BitIOStringSize] = {0};
        FLAC->Meta->Vorbis->VendorTagSize = SwapEndian32(ReadBits(BitI, 32)); // 32
        FLAC->Meta->Vorbis->VendorTag     = calloc(FLAC->Meta->Vorbis->VendorTagSize, 1);
        for (uint32_t TagByte = 0; TagByte < FLAC->Meta->Vorbis->VendorTagSize; TagByte++) {
            FLAC->Meta->Vorbis->VendorTag[TagByte]  = ReadBits(BitI, 8); // reference libFLAC 1.3.2 20170101
        }
        
        FLAC->Meta->Vorbis->NumUserTags   = SwapEndian32(ReadBits(BitI, 32)); // 0
        FLAC->Meta->Vorbis->UserTagString = calloc(FLAC->Meta->Vorbis->NumUserTags, 1);
        FLAC->Meta->Vorbis->UserTagSize   = calloc(FLAC->Meta->Vorbis->NumUserTags, 1);
        for (uint32_t Comment = 0; Comment < FLAC->Meta->Vorbis->NumUserTags; Comment++) {
            FLAC->Meta->Vorbis->UserTagSize[Comment] = SwapEndian32(ReadBits(BitI, 32));
            FLAC->Meta->Vorbis->UserTagString[Comment] = calloc(FLAC->Meta->Vorbis->UserTagSize[Comment], 1);
            char UserTagString[FLAC->Meta->Vorbis->UserTagSize[Comment]];
            for (uint32_t CommentByte = 0; CommentByte < FLAC->Meta->Vorbis->UserTagSize[Comment]; CommentByte++) {
                UserTagString[CommentByte] = ReadBits(BitI, 8);
            }
            FLAC->Meta->Vorbis->UserTagString[Comment] = UserTagString;
        }
        
        for (uint32_t UserTag = 0; UserTag < FLAC->Meta->Vorbis->NumUserTags; UserTag++) {
            snprintf(Description, BitIOStringSize, "User Tag %d of %d: %s\n", UserTag, FLAC->Meta->Vorbis->NumUserTags, FLAC->Meta->Vorbis->UserTagString[UserTag]);
            printf(Description);
            Log(SYSInformation, "ModernFLAC", "FLACParseVorbisComment", Description);
        }
    }
    
    void FLACParseCuesheet(BitInput *BitI, FLACDecoder *FLAC) {
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
    
    void FLACParsePicture(BitInput *BitI, FLACDecoder *FLAC) { // 17,151
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
    
#ifdef __cplusplus
}
#endif
