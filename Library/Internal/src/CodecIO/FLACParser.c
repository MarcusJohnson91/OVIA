#include "../include/libModernFLAC.h"
#include "../include/ParseFLAC.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void FLACParseMetadata(BitInput *BitI, FLACDecoder *FLAC) {
        FLAC->LastMetadataBlock          = ReadBits(BitI, 1);
        uint8_t  MetadataBlockType       = ReadBits(BitI, 7);  // 6
        FLAC->Meta->MetadataSize         = ReadBits(BitI, 24); // 3391 Does NOT count the 2 fields above.
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
    
    void FLACParseStreamInfo(BitInput *BitI, FLACDecoder *FLAC) {
        FLAC->Meta->StreamInfo->MinimumBlockSize        = ReadBits(BitI, 16); // 4096
        FLAC->Meta->StreamInfo->MaximumBlockSize        = ReadBits(BitI, 16); // 4096
        FLAC->Meta->StreamInfo->MinimumFrameSize        = ReadBits(BitI, 24); // 752
        FLAC->Meta->StreamInfo->MaximumFrameSize        = ReadBits(BitI, 24); // 13,594
        FLAC->Meta->StreamInfo->SampleRate              = ReadBits(BitI, 20); // 44,100
        FLAC->Meta->StreamInfo->Channels                = ReadBits(BitI, 3) + 1; // stereo
        FLAC->Meta->StreamInfo->BitDepth                = ReadBits(BitI, 5) + 1; // 16
        FLAC->Meta->StreamInfo->SamplesInStream         = ReadBits(BitI, 36); // 24,175,621
        for (uint8_t MD5Byte = 0; MD5Byte < 16; MD5Byte++) {
            FLAC->Meta->StreamInfo->MD5[MD5Byte] = ReadBits(BitI, 8); // MD5 = 0xF296D726DAED094F660800131C52CED4
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
            FLAC->Meta->Vorbis->VendorTag[TagByte]  = ReadBits(BitI, 8); // reference libFLAC 1.3.1 20141125
        }
        FLAC->Meta->Vorbis->NumUserTags   = SwapEndian32(ReadBits(BitI, 32)); // 13
        FLAC->Meta->Vorbis->UserTagSize   = calloc(FLAC->Meta->Vorbis->NumUserTags, 1);
        FLAC->Meta->Vorbis->UserTagString = calloc(FLAC->Meta->Vorbis->NumUserTags, 1);
        for (uint32_t Comment = 0; Comment < FLAC->Meta->Vorbis->NumUserTags; Comment++) {
            FLAC->Meta->Vorbis->UserTagSize[Comment] = SwapEndian32(ReadBits(BitI, 32));
            FLAC->Meta->Vorbis->UserTagString[Comment] = calloc(FLAC->Meta->Vorbis->UserTagSize[Comment], 1);
            // 39   // ALBUM=My Beautiful Dark Twisted Fantasy
            // 17   // ARTIST=Kanye West
            // 6    // BPM=87
            // 13   // BPM=00000 BPM
            // 12   // DISCNUMBER=1
            // 9    // genre=Rap
            // 98   //iTunNORM=00001B05 00001A8D 0000D69A 0000E522 00026C39 00026C39 00008000 00008000 0000947A 0000947A
            // 13   // TITLE=Runaway
            // 12   // totaldiscs=1
            // 14   // totaltracks=13
            // 3044 // UNSYNCEDLYRICS=[Kanye West] (Look at You [X14]) (Look at You [X8]) (Ladies and Gentlemen, La-Ladies and Gentlemen) (And I Want to Show You How You All Look Like Beautiful Stars Tonight) (And I Want to Show You How You All Look Like Beautiful Stars Tonight) (And I Want to Show You How You All Look Like Beautiful Stars Tonight) And I Always Find, Yeah, I Always Find Somethin' Wrong You Been Puttin' up Wit' My Shit Just Way Too Long I'm So Gifted at Findin' What I Don't Like the Most So I Think It's Time for Us to Have a Toast Let's Have a Toast for the Douchebags, Let's Have a Toast for the Assholes, Let's Have a Toast for the Scumbags, Every One of Them That I Know Let's Have a Toast to the Jerkoffs That'll Never Take Work off Baby, I Got a Plan Run Away Fast As You Can She Find Pictures in My Email I Sent This Girl a Picture of My, Hey! I Don't Know What It Is with Females But I'm Not Too Good with That, Hey! See, I Could Have Me a Good Girl And Still Be Addicted to Them Hoodrats And I Just Blame Everything on You At Least You Know That's What I'm Good at See, I Always Find And I Always Find Yeah, I Always Find Somethin' Wrong You Been Puttin' up with My Shit Just Way Too Long I'm So Gifted at Findin' What I Don't Like the Most So I Think It's Time for Us to Have a Toast Let's Have a Toast for the Douchebags, Let's Have a Toast for the Assholes, Let's Have a Toast for the Scumbags, Every One of Them That I Know Let's Have a Toast to the Jerkoffs That'll Never Take Work off Baby, I Got a Plan Run Away Fast As You Can R-R-Ru-Ru-Ru-Run Away Run Away from Me, Baby (Look at, Look at, Look at, Look at You) Run Away from Me, Baby (Look at You, Look at You, Look at You) Run Away Run Away from Me, Baby [Pusha T] 24/7, 365, Jenny Stays on My Mind I-I-I-I Did It, All Right, All Right, I Admit It Now Pick Your Best Move, You Could Leave or Live Wit' It Ichabod Crane with That Lamborghini Top off Split and Go Where? Back to Wearin' Knockoffs, Huh? Knock It off, Neiman's, Shop It off Let's Talk Over Mai Tais, Waitress, Top It off (Inaudible) Wanna Fly in Your Freddy Loafers You Can't Blame 'em, They Ain't Never Seen Versace Sofas Every Bag, Every Blouse, Every Bracelet Comes with a Price Tag, Baby, Face It You Should Leave If You Can't Accept the Basics Plenty Bitches in the Baller-Player's Matrix Invisibly Set, the Rolex Is Faceless I'm Just Young, Rich, and Tasteless P! [Kanye] Oh I Haven't Fucked in So Long You Been Puttin' up with My Shit Just Way Too Long I'm So Gifted at Findin' What I Don't Like the Most So I Think It's Time for Us to Have a Toast (Ladies and Gentlemen! ) Yeah, I Always Find Somethin' Wrong You Been Puttin' up with My Shit Just Way Too Long I'm So Gifted at Findin' What I Don't Like the Most So I Think It's Time for Us to Have a Toast Let's Have a Toast for the Douchebags, Let's Have a Toast for the Assholes, Let's Have a Toast for the Scumbags, Every One of Them That I Know Let's Have a Toast to the Jerkoffs That'll Never Take Work off Baby, I Got a Plan Run Away Fast As You Can
            
            // 9  // DATE=2010
            // 13 // TRACKNUMBER=9
            
            char UserTagString[FLAC->Meta->Vorbis->UserTagSize[Comment]];
            for (uint32_t CommentByte = 0; CommentByte < FLAC->Meta->Vorbis->UserTagSize[Comment]; CommentByte++) {
                UserTagString[CommentByte] = ReadBits(BitI, 8);
            }
            FLAC->Meta->Vorbis->UserTagString[Comment] = UserTagString;
        }
        
        for (uint32_t UserTag = 0; UserTag < FLAC->Meta->Vorbis->NumUserTags; UserTag++) {
            snprintf(Description, BitIOStringSize, "User Tag %d of %d: %c\n", UserTag, FLAC->Meta->Vorbis->NumUserTags, FLAC->Meta->Vorbis->UserTagString[UserTag]);
            //printf("%s", Description);
            Log(SYSDebug, "ModernFLAC", "FLACParseVorbisComment", Description);
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
