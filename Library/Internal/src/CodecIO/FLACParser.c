#include "../../include/Decoder/ParseFLAC.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void FLACParseMetadata(BitInput *BitI, DecodeFLAC *Dec) {
        Dec->LastMetadataBlock          = ReadBits(BitI, 1, true);
        uint8_t  MetadataBlockType       = ReadBits(BitI, 7, true);  // 6
        Dec->Meta->MetadataSize         = ReadBits(BitI, 24, true); // 3391 Does NOT count the 2 fields above.
        char Description[BitIOStringSize];
        
        switch (MetadataBlockType) {
            case StreamInfo:
                FLACParseStreamInfo(BitI, Dec);
                break;
            case Padding:
                FLACSkipPadding(BitI, Dec);
                break;
            case Custom:
                FLACSkipCustom(BitI, Dec);
                break;
            case SeekTable:
                FLACParseSeekTable(BitI, Dec);
                break;
            case VorbisComment:
                FLACParseVorbisComment(BitI, Dec);
                break;
            case Cuesheet:
                FLACParseCuesheet(BitI, Dec);
                break;
            case Picture:
                FLACParsePicture(BitI, Dec);
                break;
            default:
                snprintf(Description, BitIOStringSize, "Invalid Metadata Type: %d\n", MetadataBlockType);
                Log(LOG_WARNING, "ModernFLAC", "FLACParseMetadata", Description);
                break;
        }
    }
    
    void FLACParseStreamInfo(BitInput *BitI, DecodeFLAC *Dec) {
        Dec->Meta->StreamInfo->MinimumBlockSize        = ReadBits(BitI, 16, true); // 4096
        Dec->Meta->StreamInfo->MaximumBlockSize        = ReadBits(BitI, 16, true); // 4096
        Dec->Meta->StreamInfo->MinimumFrameSize        = ReadBits(BitI, 24, true); // 752
        Dec->Meta->StreamInfo->MaximumFrameSize        = ReadBits(BitI, 24, true); // 13,594
        Dec->Meta->StreamInfo->SampleRate              = ReadBits(BitI, 20, true); // 44,100
        Dec->Meta->StreamInfo->Channels                = ReadBits(BitI, 3, true) + 1; // stereo
        Dec->Meta->StreamInfo->BitDepth                = ReadBits(BitI, 5, true) + 1; // 16
        Dec->Meta->StreamInfo->SamplesInStream         = ReadBits(BitI, 36, true); // 24,175,621
        for (uint8_t MD5Byte = 0; MD5Byte < 16; MD5Byte++) {
            Dec->Meta->StreamInfo->MD5[MD5Byte] = ReadBits(BitI, 8, true); // MD5 = 0xF296D726DAED094F660800131C52CED4
        }
    }
    
    void FLACSkipPadding(BitInput *BitI, DecodeFLAC *Dec) { // 8192
        SkipBits(BitI, Bytes2Bits(Dec->Meta->MetadataSize));
    }
    
    void FLACSkipCustom(BitInput *BitI, DecodeFLAC *Dec) { // 134,775
        SkipBits(BitI, Bytes2Bits(Dec->Meta->MetadataSize + 1));
    }
    
    void FLACParseSeekTable(BitInput *BitI, DecodeFLAC *Dec) { // 18
        Dec->Meta->Seek->NumSeekPoints = Dec->Meta->MetadataSize / 18; // 21
        
        for (uint16_t SeekPoint = 0; SeekPoint < Dec->Meta->Seek->NumSeekPoints; SeekPoint++) {
            Dec->Meta->Seek->SampleInTargetFrame[SeekPoint]        = ReadBits(BitI, 64, true); // 0
            Dec->Meta->Seek->OffsetFrom1stSample[SeekPoint]        = ReadBits(BitI, 64, true); // 0
            Dec->Meta->Seek->TargetFrameSize[SeekPoint]            = ReadBits(BitI, 16, true); // 16384
        }
    }
    
    void FLACParseVorbisComment(BitInput *BitI, DecodeFLAC *Dec) { // LITTLE ENDIAN
        char *Description;
        Dec->Meta->Vorbis->VendorTagSize = SwapEndian32(ReadBits(BitI, 32, true)); // 32
        Dec->Meta->Vorbis->VendorTag     = calloc(1, Dec->Meta->Vorbis->VendorTagSize);
        for (uint32_t TagByte = 0; TagByte < Dec->Meta->Vorbis->VendorTagSize; TagByte++) {
            Dec->Meta->Vorbis->VendorTag[TagByte]  = ReadBits(BitI, 8, true); // reference libFLAC 1.3.1 20141125
        }
        Dec->Meta->Vorbis->NumUserTags   = SwapEndian32(ReadBits(BitI, 32, true)); // 13
        Dec->Meta->Vorbis->UserTagSize   = calloc(1, Dec->Meta->Vorbis->NumUserTags);
        Dec->Meta->Vorbis->UserTagString = calloc(1, Dec->Meta->Vorbis->NumUserTags);
        for (uint32_t Comment = 0; Comment < Dec->Meta->Vorbis->NumUserTags; Comment++) {
            Dec->Meta->Vorbis->UserTagSize[Comment] = SwapEndian32(ReadBits(BitI, 32, true));
            Dec->Meta->Vorbis->UserTagString[Comment] = calloc(1, Dec->Meta->Vorbis->UserTagSize[Comment]);
            
            char UserTagString[Dec->Meta->Vorbis->UserTagSize[Comment]];
            for (uint32_t CommentByte = 0; CommentByte < Dec->Meta->Vorbis->UserTagSize[Comment]; CommentByte++) {
                UserTagString[CommentByte] = ReadBits(BitI, 8, true);
            }
            Dec->Meta->Vorbis->UserTagString[Comment] = UserTagString;
        }
        
        for (uint32_t UserTag = 0; UserTag < Dec->Meta->Vorbis->NumUserTags; UserTag++) {
            snprintf(Description, BitIOStringSize, "User Tag %d of %d: %c\n", UserTag, Dec->Meta->Vorbis->NumUserTags, Dec->Meta->Vorbis->UserTagString[UserTag]);
            //printf("%s", Description);
            Log(LOG_DEBUG, "ModernFLAC", "FLACParseVorbisComment", Description);
        }
    }
    
    void FLACParseCuesheet(BitInput *BitI, DecodeFLAC *Dec) {
        for (uint8_t CatalogChar = 0; CatalogChar < FLACMedizCatalogNumberSize; CatalogChar++) {
            Dec->Meta->Cue->CatalogID[CatalogChar] = ReadBits(BitI, 8, true);
        }
        Dec->Meta->Cue->LeadIn = ReadBits(BitI, 64, true);
        Dec->Meta->Cue->IsCD   = ReadBits(BitI, 1, true);
        SkipBits(BitI, 2071); // Reserved
        Dec->Meta->Cue->NumTracks = ReadBits(BitI, 8, true);
        
        for (uint8_t Track = 0; Track < Dec->Meta->Cue->NumTracks; Track++) {
            Dec->Meta->Cue->TrackOffset[Track]  = ReadBits(BitI, 64, true);
            Dec->Meta->Cue->TrackNum[Track]     = ReadBits(BitI, 64, true);
            
            for (uint8_t ISRCByte = 0; ISRCByte < FLACISRCSize; ISRCByte++) {
                Dec->Meta->Cue->ISRC[Track][ISRCByte]  = ReadBits(BitI, 8, true);
            }
            Dec->Meta->Cue->IsAudio[Track] = ReadBits(BitI, 1, true);
            Dec->Meta->Cue->PreEmphasis[Track] = ReadBits(BitI, 1, true);
            SkipBits(BitI, 152); // Reserved, all 0
            Dec->Meta->Cue->NumTrackIndexPoints[Track] = ReadBits(BitI, 8, true);
        }
        
        Dec->Meta->Cue->IndexOffset    = ReadBits(BitI, 64, true);
        Dec->Meta->Cue->IndexPointNum  = ReadBits(BitI, 8, true);
        SkipBits(BitI, 24); // Reserved
    }
    
    void FLACParsePicture(BitInput *BitI, DecodeFLAC *Dec) { // 17,151
        Dec->Meta->Pic->PicType  = ReadBits(BitI, 32, true); // 3
        Dec->Meta->Pic->MIMESize = ReadBits(BitI, 32, true); // 10
        for (uint32_t MIMEByte = 0; MIMEByte < Dec->Meta->Pic->MIMESize; MIMEByte++) {
            Dec->Meta->Pic->MIMEString[MIMEByte] = ReadBits(BitI, 8, true); // image/jpeg
        }
        Dec->Meta->Pic->PicDescriptionSize = ReadBits(BitI, 32, true); // 0
        for (uint32_t Char = 0; Char < Dec->Meta->Pic->PicDescriptionSize; Char++) {
            Dec->Meta->Pic->PicDescriptionString[Char] = ReadBits(BitI, 8, true);
        }
        Dec->Meta->Pic->Width       = ReadBits(BitI, 32, true); // 1144
        Dec->Meta->Pic->Height      = ReadBits(BitI, 32, true); // 1144
        Dec->Meta->Pic->BitDepth    = ReadBits(BitI, 32, true); // 24, PER PIXEL, NOT SUBPIXEL
        Dec->Meta->Pic->ColorsUsed  = ReadBits(BitI, 32, true); // 0
        Dec->Meta->Pic->PictureSize = ReadBits(BitI, 32, true); // 17,109
                                                                // Pop in the address of the start of the data, and skip over the data instead of buffering it.
    }
    
#ifdef __cplusplus
}
#endif
