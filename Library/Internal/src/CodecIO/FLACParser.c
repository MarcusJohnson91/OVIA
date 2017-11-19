#include "../../include/Private/Decode/libModernFLAC_Parse.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void FLACParseMetadata(BitBuffer *InputFLAC, DecodeFLAC *Dec) {
        if (InputFLAC == NULL) {
            BitIOLog(LOG_ERROR, ModernFLACLibraryName, __func__, "Pointer to BitBuffer is NULL");
        } else if (Dec == NULL) {
            BitIOLog(LOG_ERROR, ModernFLACLibraryName, __func__, "Pointer to DecodeFLAC is NULL");
        } else {
            Dec->LastMetadataBlock          = ReadBits(BitIOMSByte, BitIOLSBit, InputFLAC, 1);  // true
            uint8_t  MetadataBlockType      = ReadBits(BitIOMSByte, BitIOLSBit, InputFLAC, 7);  // 1 aka Padding
            Dec->Meta->MetadataSize         = ReadBits(BitIOMSByte, BitIOLSBit, InputFLAC, 24); // 8192 Does NOT count the 2 fields above.
            
            switch (MetadataBlockType) {
                case StreamInfo:
                    FLACParseStreamInfo(InputFLAC, Dec);
                    break;
                case Padding:
                    FLACSkipPadding(InputFLAC, Dec);
                    break;
                case Custom:
                    FLACSkipCustom(InputFLAC, Dec);
                    break;
                case SeekTable:
                    FLACParseSeekTable(InputFLAC, Dec);
                    break;
                case VorbisComment:
                    FLACParseVorbisComment(InputFLAC, Dec);
                    break;
                case Cuesheet:
                    FLACParseCuesheet(InputFLAC, Dec);
                    break;
                case Picture:
                    FLACParsePicture(InputFLAC, Dec);
                    break;
                default:
                    BitIOLog(LOG_INFORMATION, ModernFLACLibraryName, "FLACParseMetadata", "Invalid Metadata Type: %d\n", MetadataBlockType);
                    break;
            }
        }
    }
    
    void FLACParseStreamInfo(BitBuffer *InputFLAC, DecodeFLAC *Dec) {
        if (InputFLAC == NULL) {
            BitIOLog(LOG_ERROR, ModernFLACLibraryName, __func__, "Pointer to BitBuffer is NULL");
        } else if (Dec == NULL) {
            BitIOLog(LOG_ERROR, ModernFLACLibraryName, __func__, "Pointer to DecodeFLAC is NULL");
        } else {
            Dec->Meta->StreamInfo->MinimumBlockSize        = ReadBits(BitIOMSByte, BitIOLSBit, InputFLAC, 16);    // 4096
            Dec->Meta->StreamInfo->MaximumBlockSize        = ReadBits(BitIOMSByte, BitIOLSBit, InputFLAC, 16);    // 4096
            Dec->Meta->StreamInfo->MinimumFrameSize        = ReadBits(BitIOMSByte, BitIOLSBit, InputFLAC, 24);    // 14
            Dec->Meta->StreamInfo->MaximumFrameSize        = ReadBits(BitIOMSByte, BitIOLSBit, InputFLAC, 24);    // 16
            Dec->Meta->StreamInfo->SampleRate              = ReadBits(BitIOMSByte, BitIOLSBit, InputFLAC, 20);    // 44,100
            Dec->Meta->StreamInfo->Channels                = ReadBits(BitIOMSByte, BitIOLSBit, InputFLAC, 3) + 1; // stereo
            Dec->Meta->StreamInfo->BitDepth                = ReadBits(BitIOMSByte, BitIOLSBit, InputFLAC, 5) + 1; // 15 + 1
            Dec->Meta->StreamInfo->SamplesInStream         = ReadBits(BitIOMSByte, BitIOLSBit, InputFLAC, 36);    // 4607
            for (uint8_t MD5Byte = 0; MD5Byte < 16; MD5Byte++) {
                Dec->Meta->StreamInfo->MD5[MD5Byte] = ReadBits(BitIOMSByte, BitIOLSBit, InputFLAC, 8); // MD5 = 0x162605FCCEAE5EFB19C37DCB8AED7804
            }
        }
    }
    
    void FLACSkipPadding(BitBuffer *InputFLAC, DecodeFLAC *Dec) { // 8192
        if (InputFLAC == NULL) {
            BitIOLog(LOG_ERROR, ModernFLACLibraryName, __func__, "Pointer to BitBuffer is NULL");
        } else if (Dec == NULL) {
            BitIOLog(LOG_ERROR, ModernFLACLibraryName, __func__, "Pointer to DecodeFLAC is NULL");
        } else {
            SkipBits(InputFLAC, Bytes2Bits(Dec->Meta->MetadataSize));
        }
    }
    
    void FLACSkipCustom(BitBuffer *InputFLAC, DecodeFLAC *Dec) { // 134,775
        if (InputFLAC == NULL) {
            BitIOLog(LOG_ERROR, ModernFLACLibraryName, __func__, "Pointer to BitBuffer is NULL");
        } else if (Dec == NULL) {
            BitIOLog(LOG_ERROR, ModernFLACLibraryName, __func__, "Pointer to DecodeFLAC is NULL");
        } else {
            SkipBits(InputFLAC, Bytes2Bits(Dec->Meta->MetadataSize + 1));
        }
    }
    
    void FLACParseSeekTable(BitBuffer *InputFLAC, DecodeFLAC *Dec) { // 18
        if (InputFLAC == NULL) {
            BitIOLog(LOG_ERROR, ModernFLACLibraryName, __func__, "Pointer to BitBuffer is NULL");
        } else if (Dec == NULL) {
            BitIOLog(LOG_ERROR, ModernFLACLibraryName, __func__, "Pointer to DecodeFLAC is NULL");
        } else {
            Dec->Meta->Seek->NumSeekPoints = Dec->Meta->MetadataSize / 18; // 21
            
            for (uint16_t SeekPoint = 0; SeekPoint < Dec->Meta->Seek->NumSeekPoints; SeekPoint++) {
                Dec->Meta->Seek->SampleInTargetFrame[SeekPoint]        = ReadBits(BitIOMSByte, BitIOLSBit, InputFLAC, 64); // 0
                Dec->Meta->Seek->OffsetFrom1stSample[SeekPoint]        = ReadBits(BitIOMSByte, BitIOLSBit, InputFLAC, 64); // 0
                Dec->Meta->Seek->TargetFrameSize[SeekPoint]            = ReadBits(BitIOMSByte, BitIOLSBit, InputFLAC, 16); // 16384
            }
        }
    }
    
    void FLACParseVorbisComment(BitBuffer *InputFLAC, DecodeFLAC *Dec) { // LITTLE ENDIAN
        if (InputFLAC == NULL) {
            BitIOLog(LOG_ERROR, ModernFLACLibraryName, __func__, "Pointer to BitBuffer is NULL");
        } else if (Dec == NULL) {
            BitIOLog(LOG_ERROR, ModernFLACLibraryName, __func__, "Pointer to DecodeFLAC is NULL");
        } else {
            Dec->Meta->Vorbis->VendorTagSize              = ReadBits(BitIOLSByte, BitIOLSBit, InputFLAC, 32); // 32
            Dec->Meta->Vorbis->VendorTag                  = calloc(1, Dec->Meta->Vorbis->VendorTagSize * sizeof(char));
            for (uint32_t TagByte = 0; TagByte < Dec->Meta->Vorbis->VendorTagSize; TagByte++) {
                Dec->Meta->Vorbis->VendorTag[TagByte]     = ReadBits(BitIOLSByte, BitIOLSBit, InputFLAC, 8); // reference libFLAC 1.3.1 20141125
            }
            Dec->Meta->Vorbis->NumUserTags                = ReadBits(BitIOLSByte, BitIOLSBit, InputFLAC, 32); // 13
            Dec->Meta->Vorbis->UserTagSize                = calloc(Dec->Meta->Vorbis->NumUserTags, sizeof(uint8_t));
            Dec->Meta->Vorbis->UserTagString              = calloc(Dec->Meta->Vorbis->NumUserTags, sizeof(char));
            for (uint32_t Comment = 0; Comment < Dec->Meta->Vorbis->NumUserTags; Comment++) {
                Dec->Meta->Vorbis->UserTagSize[Comment]   = ReadBits(BitIOLSByte, BitIOLSBit, InputFLAC, 32);
                Dec->Meta->Vorbis->UserTagString[Comment] = calloc(1, Dec->Meta->Vorbis->UserTagSize[Comment] * sizeof(char));
                
                char UserTagString[Dec->Meta->Vorbis->UserTagSize[Comment]];
                for (uint32_t CommentByte = 0; CommentByte < Dec->Meta->Vorbis->UserTagSize[Comment]; CommentByte++) {
                    UserTagString[CommentByte] = ReadBits(BitIOLSByte, BitIOLSBit, InputFLAC, 8);
                }
                Dec->Meta->Vorbis->UserTagString[Comment] = UserTagString;
            }
            
            for (uint32_t UserTag = 0; UserTag < Dec->Meta->Vorbis->NumUserTags; UserTag++) {
                BitIOLog(LOG_INFORMATION, ModernFLACLibraryName, "FLACParseVorbisComment", "User Tag %d of %d: %c\n", UserTag, Dec->Meta->Vorbis->NumUserTags, Dec->Meta->Vorbis->UserTagString[UserTag]);
            }
        }
    }
    
    void FLACParseCuesheet(BitBuffer *InputFLAC, DecodeFLAC *Dec) {
        if (InputFLAC == NULL) {
            BitIOLog(LOG_ERROR, ModernFLACLibraryName, __func__, "Pointer to BitBuffer is NULL");
        } else if (Dec == NULL) {
            BitIOLog(LOG_ERROR, ModernFLACLibraryName, __func__, "Pointer to DecodeFLAC is NULL");
        } else {
            for (uint8_t CatalogChar = 0; CatalogChar < FLACMedizCatalogNumberSize; CatalogChar++) {
                Dec->Meta->Cue->CatalogID[CatalogChar] = ReadBits(BitIOMSByte, BitIOLSBit, InputFLAC, 8);
            }
            Dec->Meta->Cue->LeadIn = ReadBits(BitIOMSByte, BitIOLSBit, InputFLAC, 64);
            Dec->Meta->Cue->IsCD   = ReadBits(BitIOMSByte, BitIOLSBit, InputFLAC, 1);
            SkipBits(InputFLAC, 2071); // Reserved
            Dec->Meta->Cue->NumTracks = ReadBits(BitIOMSByte, BitIOLSBit, InputFLAC, 8);
            
            for (uint8_t Track = 0; Track < Dec->Meta->Cue->NumTracks; Track++) {
                Dec->Meta->Cue->TrackOffset[Track]  = ReadBits(BitIOMSByte, BitIOLSBit, InputFLAC, 64);
                Dec->Meta->Cue->TrackNum[Track]     = ReadBits(BitIOMSByte, BitIOLSBit, InputFLAC, 64);
                
                for (uint8_t ISRCByte = 0; ISRCByte < FLACISRCSize; ISRCByte++) {
                    Dec->Meta->Cue->ISRC[Track][ISRCByte]  = ReadBits(BitIOMSByte, BitIOLSBit, InputFLAC, 8);
                }
                Dec->Meta->Cue->IsAudio[Track] = ReadBits(BitIOMSByte, BitIOLSBit, InputFLAC, 1);
                Dec->Meta->Cue->PreEmphasis[Track] = ReadBits(BitIOMSByte, BitIOLSBit, InputFLAC, 1);
                SkipBits(InputFLAC, 152); // Reserved, all 0
                Dec->Meta->Cue->NumTrackIndexPoints[Track] = ReadBits(BitIOMSByte, BitIOLSBit, InputFLAC, 8);
            }
            
            Dec->Meta->Cue->IndexOffset    = ReadBits(BitIOMSByte, BitIOLSBit, InputFLAC, 64);
            Dec->Meta->Cue->IndexPointNum  = ReadBits(BitIOMSByte, BitIOLSBit, InputFLAC, 8);
            SkipBits(InputFLAC, 24); // Reserved
        }
    }
    
    void FLACParsePicture(BitBuffer *InputFLAC, DecodeFLAC *Dec) { // 17,151
        if (InputFLAC == NULL) {
            BitIOLog(LOG_ERROR, ModernFLACLibraryName, __func__, "Pointer to BitBuffer is NULL");
        } else if (Dec == NULL) {
            BitIOLog(LOG_ERROR, ModernFLACLibraryName, __func__, "Pointer to DecodeFLAC is NULL");
        } else {
            Dec->Meta->Pic->PicType  = ReadBits(BitIOMSByte, BitIOLSBit, InputFLAC, 32); // 3
            Dec->Meta->Pic->MIMESize = ReadBits(BitIOMSByte, BitIOLSBit, InputFLAC, 32); // 10
            for (uint32_t MIMEByte = 0; MIMEByte < Dec->Meta->Pic->MIMESize; MIMEByte++) {
                Dec->Meta->Pic->MIMEString[MIMEByte] = ReadBits(BitIOMSByte, BitIOLSBit, InputFLAC, 8); // image/jpeg
            }
            Dec->Meta->Pic->PicDescriptionSize = ReadBits(BitIOMSByte, BitIOLSBit, InputFLAC, 32); // 0
            for (uint32_t Char = 0; Char < Dec->Meta->Pic->PicDescriptionSize; Char++) {
                Dec->Meta->Pic->PicDescriptionString[Char] = ReadBits(BitIOMSByte, BitIOLSBit, InputFLAC, 8);
            }
            Dec->Meta->Pic->Width       = ReadBits(BitIOMSByte, BitIOLSBit, InputFLAC, 32); // 1144
            Dec->Meta->Pic->Height      = ReadBits(BitIOMSByte, BitIOLSBit, InputFLAC, 32); // 1144
            Dec->Meta->Pic->BitDepth    = ReadBits(BitIOMSByte, BitIOLSBit, InputFLAC, 32); // 24, PER PIXEL, NOT SUBPIXEL
            Dec->Meta->Pic->ColorsUsed  = ReadBits(BitIOMSByte, BitIOLSBit, InputFLAC, 32); // 0
            Dec->Meta->Pic->PictureSize = ReadBits(BitIOMSByte, BitIOLSBit, InputFLAC, 32); // 17,109
         // Pop in the address of the start of the data, and skip over the data instead of buffering it.
        }
    }
    
#ifdef __cplusplus
}
#endif
