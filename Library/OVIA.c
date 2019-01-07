#include "../include/libOVIA.h"
#include "../include/Private/InternalOVIA.h"

#include "../include/Private/Audio/AIFCommon.h"
#include "../include/Private/Audio/FLACCommon.h"
#include "../include/Private/Audio/W64Common.h"
#include "../include/Private/Audio/WAVCommon.h"
#include "../include/Private/Image/BMPCommon.h"
#include "../include/Private/Image/PNGCommon.h"
#include "../include/Private/Image/PNMCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef enum PNGTextTypes {
        UnknownTextType = 0,
        tEXt            = 1,
        iTXt            = 2,
        zTXt            = 3,
    } PNGTextTypes;
    
    typedef struct iHDRChunk {
        uint32_t      Width;
        uint32_t      Height;
        uint8_t       BitDepth;
        uint8_t       Compression;
        uint8_t       FilterMethod;
        OVIA_PNG_ColorTypes ColorType;
        bool          Progressive;
        bool          Interlaced;
    } iHDRChunk;
    
    typedef struct PLTEChunk {
        uint8_t  **Palette;
        uint8_t    NumEntries;
    } PLTEChunk;
    
    typedef struct tRNSChunk {
        uint8_t  **Palette;
        uint8_t    NumEntries;
    } tRNSChunk;
    
    typedef struct bkGDChunk {
        uint8_t    BackgroundPaletteEntry;
        uint16_t   Gray;
        uint16_t   Red;
        uint16_t   Green;
        uint16_t   Blue;
    } bkGDChunk;
    
    typedef struct sTERChunk {
        bool       StereoType:1;
    } sTERChunk;
    
    typedef struct cHRMChunk { // sRGB or iCCP overrides cHRM
        uint32_t   WhitePointX;
        uint32_t   WhitePointY;
        uint32_t   RedX;
        uint32_t   RedY;
        uint32_t   GreenX;
        uint32_t   GreenY;
        uint32_t   BlueX;
        uint32_t   BlueY;
    } cHRMChunk;
    
    typedef struct gAMAChunk { // sRGB or iCCP overrides gAMA
        uint32_t   Gamma;
    } gAMAChunk;
    
    typedef struct oFFsChunk {
        int32_t    XOffset;
        int32_t    YOffset;
        bool       UnitSpecifier:1;
    } oFFsChunk;
    
    typedef struct iCCPChunk {
        UTF8      *ProfileName;
        uint8_t   *CompressedICCPProfile;
        uint64_t   CompressedICCPProfileSize;
        uint8_t    CompressionType;
    } iCCPChunk;
    
    typedef struct sBITChunk {
        uint8_t    Grayscale;
        uint8_t    Red;
        uint8_t    Green;
        uint8_t    Blue;
        uint8_t    Alpha;
    } sBITChunk;
    
    typedef struct sRGBChunk {
        uint8_t    RenderingIntent;
    } sRGBChunk;
    
    typedef struct pHYsChunk {
        uint32_t   PixelsPerUnitXAxis;
        uint32_t   PixelsPerUnitYAxis;
        uint8_t    UnitSpecifier;
    } pHYsChunk;
    
    typedef struct sPLTChunk {
        uint16_t  *Red;
        uint16_t  *Green;
        uint16_t  *Blue;
        uint16_t  *Alpha;
        uint16_t  *RelativeFrequency;
        char      *Name;
        uint8_t    SampleDepth;
    } sPLTChunk;
    
    typedef struct pCALChunk {
        UTF8      *CalibrationName;
        UTF8      *UnitName;
        uint8_t    NumParams;
        int32_t    OriginalZero;
        int32_t    OriginalMax;
        uint8_t    EquationType;
    } pCALChunk;
    
    typedef struct sCALChunk {
        double     PixelWidth; // ASCII float
        double     PixelHeight; // ASCII float
        uint8_t    UnitSpecifier;
    } sCALChunk;
    
    typedef struct hISTChunk {
        uint32_t   NumColors;
        uint16_t  *Histogram; // For each PLTE entry, there needs to be 1 array element
    } hISTChunk;
    
    typedef struct TextChunk { // Replaces:  tEXt, iTXt, zTXt
        UTF8         *Keyword;
        UTF8         *Comment;
        PNGTextTypes  TextType;
    } TextChunk;
    
    typedef struct tIMeChunk {
        uint16_t   Year;
        uint8_t    Month:4;
        uint8_t    Day:5;
        uint8_t    Hour:5;
        uint8_t    Minute:6;
        uint8_t    Second:6;
    } tIMeChunk;
    
    typedef struct DATChunk {
        uint8_t     *ImageArray;
        HuffmanTable *LengthLiteralTree;
        HuffmanTable *DistanceTree;
        uint64_t     ImageSize;
        uint64_t     ImageOffset;
        uint32_t     DictID;
        uint32_t     Size;
        uint16_t     LengthLiteralTreeSize;
        uint8_t      DistanceTreeSize;
        uint8_t      CMF;
        uint8_t      FLG;
    } DATChunk;
    
    typedef struct acTLChunk {
        uint32_t   NumFrames;
        uint32_t   TimesToLoop;
    } acTLChunk;
    
    typedef struct fcTLChunk {
        uint32_t   FrameNum;
        uint32_t   Width;
        uint32_t   Height;
        uint32_t   XOffset;
        uint32_t   YOffset;
        uint16_t   FrameDelayNumerator;
        uint16_t   FrameDelayDenominator;
        uint8_t    DisposeMethod;
        bool       BlendMethod:1;
    } fcTLChunk;
    
    typedef struct fdATChunk {
        uint32_t   FrameNum;
    } fdATChunk;
    
    typedef struct iENDChunk {
        uint32_t  Size;
    } iENDChunk;
    
    typedef struct PNGOptions {
        sPLTChunk   **sPLT; // May be multiple
        iHDRChunk    *iHDR;
        cHRMChunk    *cHRM;
        gAMAChunk    *gAMA;
        iCCPChunk    *iCCP;
        sBITChunk    *sBIT;
        sRGBChunk    *sRGB;
        PLTEChunk    *PLTE;
        bkGDChunk    *bkGD;
        hISTChunk    *hIST;
        tRNSChunk    *tRNS;
        pHYsChunk    *pHYs;
        oFFsChunk    *oFFs; // Extension, before first iDAT
        sCALChunk    *sCAL; // Extension, before first iDAT
        pCALChunk    *pCAL; // Extension, after PLTE, before iDAT
        tIMeChunk    *tIMe; // Order doesn't matter
        TextChunk    *Text; // Order doesn't matter
        DATChunk     *DAT;
        /* 3D */
        sTERChunk    *sTER;
        /* 3D */
        /* APNG */
        acTLChunk    *acTL;
        fcTLChunk    *fcTL;
        fdATChunk    *fdAT;
        /* APNG */
        bool          IsAnimated;
    } PNGOptions;
    
    typedef struct BMPOptions {
        uint32_t     DIBSize;
        uint32_t     FileSize;
        uint32_t     Offset;
        uint32_t     CompressionType;
        uint32_t     NumBytesUsedBySamples;
        uint32_t     WidthPixelsPerMeter;
        uint32_t     HeightPixelsPerMeter;
        uint32_t     ColorsIndexed;
        uint32_t     IndexedColorsUsed;
        uint32_t     ColorSpaceType;
        uint32_t     XCoordinate;
        uint32_t     YCoordinate;
        uint32_t     ZCoordinate;
        uint32_t     RGamma;
        uint32_t     GGamma;
        uint32_t     BGamma;
        uint32_t     ICCIntent;
        uint32_t     ICCSize;
        uint8_t     *ICCPayload;
        uint32_t     RMask;
        uint32_t     GMask;
        uint32_t     BMask;
        uint32_t     AMask;
    } BMPOptions;
    
    typedef struct PNMOptions {
        PNMTupleTypes TupleType;
        PNMTypes      Type;
    } PNMOptions;
    
    typedef struct AIFOptions {
        
    } AIFOptions;
    
    typedef struct WAVOptions {
        uint16_t CompressionFormat;
        uint16_t BlockAlignment;
    } WAVOptions;
    
    typedef struct W64Options {
        uint16_t CompressionFormat;
        uint16_t BlockAlignment;
    } W64Options;
    
    typedef struct FLACStreamInfo {
        uint64_t  SamplesInStream:36; // 36 bits, all the samples in the stream; channel agnostic
        uint32_t MinimumFrameSize:24;
        uint32_t MaximumFrameSize:24;
        uint16_t MinimumBlockSize:16; // in samples; must be at least 16
        uint16_t MaximumBlockSize:16; // If these match than it's fixed blocksize format
        uint8_t      CodedSampleRate; // 4 bits
        uint8_t      CodedBitDepth:6; // 5 bits, 4-32 bits per sample
        uint8_t      CodedChannels:4; // 3 bits, add 1 to get the real value
        uint8_t                 *MD5; // MD5
    } FLACStreamInfo;
    
    typedef struct CueSheetTrack {
        UTF8      *ISRC;                // the tracks's ISRC number as a string.
        uint64_t  *TrackOffset;         // samples from the beginning of the FLAC file to the first sample of the track
        uint8_t   *TrackNum;            // which track is this again?
        bool      *IsAudio;             // 1 for audio, 0 for data
        bool      *PreEmphasis;         // 1 for yes 0 for no.
    } CueSheetTrack;
    
    typedef struct FLACCueSheet {
        CueSheetTrack *Tracks;
        uint8_t        NumTrackIndexPoints;
        char          *CatalogID;
        uint64_t       LeadIn;              // in samples
        uint64_t       IndexOffset;         // no idea. must be multiple of CD sector for cd sources
        size_t         CatalogIDSize;
        uint8_t        NumTracks;           // the number of tracks, CD has a max of 100 (including lead out)
        bool           IsCD;                // 1 if it came from a CD; 0 otherwise
        uint8_t        IndexPointNum;
    } FLACCueSheet;
    
    typedef struct FLACSeekTable {
        uint32_t      NumSeekPoints;
        uint64_t     *SampleInTargetFrame; // FIXME: Sample in the whole file, or sample in a specific frame?
        uint64_t     *OffsetFrom1stSample; // in bytes
        uint16_t     *TargetFrameSize;
    } FLACSeekTable;
    
    typedef struct FLACPicture {
        uint32_t     *PictureStart; // Pointer to the start of the picture
        uint8_t      *MIMEString;
        uint8_t      *PicDescriptionString;
        uint32_t      PicDescriptionSize;
        uint32_t      Width;
        uint32_t      Height;
        uint32_t      MIMESize; // size of the MIME string in bytes
        uint32_t      BitDepth;
        uint32_t      ColorsUsed; // 0 for not paletted
        uint32_t      PictureSize;
        FLACPicTypes  PicType;
    } FLACPicture;
    
    typedef struct FLACSubFrame {
        uint8_t SubFrameType;
        uint8_t LPCFilterOrder;
        uint8_t WastedBits:6; // Uses unary coding
        bool    WastedBitsFlag:1;
    } FLACSubFrame;
    
    typedef struct FLACFrame {
        FLACSubFrame *Sub;
        uint64_t      SampleNumber:36;
        uint32_t      SampleRate;
        uint32_t      FrameNumber:31;
        uint16_t      BlockSize; // SamplesInBlock
        uint8_t       BitDepth;
        uint8_t       PartitionOrder;
        uint8_t       CurrentPartition;
        uint8_t       SamplesInPartition;
        uint8_t       CodedBlockSize;
        uint8_t       CodedSampleRate:5;
        uint8_t       ChannelLayout:4;
        uint8_t       CodedBitDepth:4;
        bool          BlockType:1;
    } FLACFrame;
    
    typedef struct RICEPartition {
        uint8_t      *RICEParameter;
        uint8_t       EscapeBitDepth:5;
        uint8_t       NumRICEPartitions:4;
    } RICEPartition;
    
    typedef struct FLACLPC {
        int8_t       *LPCCoeff;
        uint8_t       LPCOrder;
        uint8_t       NumLPCCoeffs;
        uint8_t       LPCShift:5;
        uint8_t       LPCPrecision:4;
        uint8_t       PartitionOrder:4;
        uint8_t       RicePartitionType:2;
    } FLACLPC;
    
    typedef struct FLACOptions {
        FLACPicture    **Pictures;
        FLACStreamInfo  *StreamInfo;
        FLACCueSheet    *CueSheet;
        FLACSeekTable   *SeekPoints;
        FLACFrame       *Frame;
        FLACLPC         *LPC;
        RICEPartition   *Rice;
        uint32_t         NumPictures;
        bool             EncodeSubset;
    } FLACOptions;
    
    typedef struct OVIA {
        AudioContainer  *Audio;
        ImageContainer  *Image;
        uint64_t         FileSize;
        uint64_t         NumChannels;
        int64_t          Width;
        int64_t          Height;
        uint64_t         NumSamples;
        uint64_t         SampleRate;
        uint64_t         SampleOffset;
        uint64_t         BlockSize;
        AIFOptions      *AIFInfo;
        BMPOptions      *BMPInfo;
        FLACOptions     *FLACInfo;
        PNGOptions      *PNGInfo;
        PNMOptions      *PNMInfo;
        W64Options      *W64Info;
        WAVOptions      *WAVInfo;
        uint64_t         NumTags;
        UTF8           **Tags;
        uint8_t          BitDepth;
        OVIA_TagTypes   *TagTypes;
        
        OVIA_Types       Type;
        OVIA_FileFormats Format;
        bool             Is3D;
    } OVIA;
    
    OVIA *OVIA_Init(void) {
        OVIA *Ovia        = calloc(1, sizeof(OVIA));
        if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return Ovia;
    }
    
    void OVIA_SetNumChannels(OVIA *Ovia, uint64_t NumChannels) {
        if (Ovia != NULL) {
            Ovia->NumChannels = NumChannels;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint64_t OVIA_GetNumChannels(OVIA *Ovia) {
        uint64_t NumChannels = 0;
        if (Ovia != NULL) {
            NumChannels = Ovia->NumChannels;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return NumChannels;
    }
    
    void OVIA_SetNumSamples(OVIA *Ovia, uint64_t NumSamples) {
        if (Ovia != NULL) {
            Ovia->NumSamples = NumSamples;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint64_t OVIA_GetNumSamples(OVIA *Ovia) {
        uint64_t NumSamples = 0;
        if (Ovia != NULL) {
            NumSamples = Ovia->NumSamples;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return NumSamples;
    }
    
    void OVIA_SetBitDepth(OVIA *Ovia, uint64_t BitDepth) {
        if (Ovia != NULL) {
            Ovia->BitDepth = BitDepth;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint8_t OVIA_GetBitDepth(OVIA *Ovia) {
        uint8_t BitDepth = 0;
        if (Ovia != NULL) {
            BitDepth     = Ovia->BitDepth;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return BitDepth;
    }
    
    void OVIA_SetSampleRate(OVIA *Ovia, uint64_t SampleRate) {
        if (Ovia != NULL) {
            Ovia->SampleRate = SampleRate;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint64_t OVIA_GetSampleRate(OVIA *Ovia) {
        uint64_t SampleRate = 0ULL;
        if (Ovia != NULL) {
            SampleRate      = Ovia->SampleRate;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return SampleRate;
    }
    
    void OVIA_SetTag(OVIA *Ovia, OVIA_TagTypes TagType, UTF8 *Tag) {
        if (Ovia != NULL && Tag != NULL) {
            Ovia->NumTags                    += 1;
            if (Ovia->Tags != NULL) {
                Ovia->Tags                    = realloc(Ovia->Tags, Ovia->NumTags * sizeof(UTF8*));
            } else {
                Ovia->Tags                    = calloc(Ovia->NumTags, sizeof(UTF8*));
            }
            if (Ovia->TagTypes != NULL) {
                Ovia->TagTypes                = realloc(Ovia->TagTypes, Ovia->NumTags * sizeof(OVIA_TagTypes));
            } else {
                Ovia->Tags                    = calloc(Ovia->NumTags, sizeof(OVIA_TagTypes));
            }
            Ovia->Tags[Ovia->NumTags - 1]     = Tag;
            Ovia->TagTypes[Ovia->NumTags - 1] = TagType;
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (Tag == NULL) {
            Log(Log_ERROR, __func__, U8("Tag Pointer is NULL"));
        }
    }
    
    uint64_t OVIA_GetTagsIndex(OVIA *Ovia, OVIA_TagTypes TagType) { // 0xFFFFFFFFFFFFFFFF means not found
        uint64_t TagIndex = 0xFFFFFFFFFFFFFFFF;
        if (Ovia != NULL) {
            for (uint64_t Tag = 0ULL; Tag < Ovia->NumTags; Tag++) {
                if (Ovia->TagTypes[Tag] == TagType) {
                    TagIndex = Tag;
                }
            }
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return TagIndex;
    }
    
    UTF8 *OVIA_GetTag(OVIA *Ovia, uint64_t Tag) {
        UTF8 *TagString = NULL;
        if (Ovia != NULL && Tag < Ovia->NumTags) {
            TagString   = Ovia->Tags[Tag];
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return TagString;
    }
    
    uint64_t OVIA_GetNumTags(OVIA *Ovia) {
        uint64_t NumTags = 0ULL;
        if (Ovia != NULL) {
            NumTags      = Ovia->NumTags;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return NumTags;
    }
    
    void OVIA_SetNumTags(OVIA *Ovia, uint64_t NumTags) {
        if (Ovia != NULL) {
            Ovia->NumTags = NumTags;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint64_t OVIA_GetTagSize(OVIA *Ovia, uint64_t Tag) {
        uint64_t TagSize = 0ULL;
        if (Ovia != NULL) {
            if (Tag < Ovia->NumTags) {
                TagSize  = UTF8_GetStringSizeInCodeUnits(Ovia->Tags[Tag]);
            }
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return TagSize;
    }
    
    void OVIA_SetFileSize(OVIA *Ovia, uint64_t FileSize) {
        if (Ovia != NULL) {
            Ovia->FileSize = FileSize;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint64_t OVIA_GetFileSize(OVIA *Ovia) {
        uint64_t FileSize = 0ULL;
        if (Ovia != NULL) {
            FileSize = Ovia->FileSize;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return FileSize;
    }
    
    void OVIA_SetSampleOffset(OVIA *Ovia, uint64_t SampleOffset) {
        if (Ovia != NULL) {
            Ovia->SampleOffset = SampleOffset;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint64_t OVIA_GetSampleOffset(OVIA *Ovia) {
        uint64_t SampleOffset = 0ULL;
        if (Ovia != NULL) {
            SampleOffset = Ovia->SampleOffset;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return SampleOffset;
    }
    
    void OVIA_SetBlockSize(OVIA *Ovia, uint64_t BlockSize) {
        if (Ovia != NULL) {
            Ovia->BlockSize = BlockSize;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint64_t OVIA_GetBlockSize(OVIA *Ovia) {
        uint64_t BlockSize = 0ULL;
        if (Ovia != NULL) {
            BlockSize = Ovia->BlockSize;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return BlockSize;
    }
    
    int64_t OVIA_GetWidth(OVIA *Ovia) {
        int64_t Width = 0ULL;
        if (Ovia != NULL) {
            Width = Ovia->Width;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return Width;
    }
    
    void OVIA_SetWidth(OVIA *Ovia, int64_t Width) {
        if (Ovia != NULL) {
            Ovia->Width = Width;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    int64_t OVIA_GetHeight(OVIA *Ovia) {
        int64_t Height = 0ULL;
        if (Ovia != NULL) {
            Height = Ovia->Height;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return Height;
    }
    
    void OVIA_SetHeight(OVIA *Ovia, int64_t Height) {
        if (Ovia != NULL) {
            Ovia->Height = Height;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_SetMinBlockSize(OVIA *Ovia, uint16_t MinBlockSize) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->StreamInfo->MinimumBlockSize = MinBlockSize;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_SetMaxBlockSize(OVIA *Ovia, uint16_t MaxBlockSize) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->StreamInfo->MaximumBlockSize = MaxBlockSize;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_SetMinFrameSize(OVIA *Ovia, uint16_t MinFrameSize) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->StreamInfo->MinimumFrameSize = MinFrameSize;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_SetMaxFrameSize(OVIA *Ovia, uint16_t MaxFrameSize) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->StreamInfo->MaximumFrameSize = MaxFrameSize;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_SetMaxFilterOrder(OVIA *Ovia, uint16_t MaxFilterOrder) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->Frame->Sub->LPCFilterOrder = MaxFilterOrder;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_SetMaxRicePartitionOrder(OVIA *Ovia, uint8_t MaxRICEPartitionOrder) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->Frame->PartitionOrder = MaxRICEPartitionOrder;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_SetEncodeSubset(OVIA *Ovia, bool EncodeSubset) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->EncodeSubset = EncodeSubset;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    bool OVIA_FLAC_GetEncodeSubset(OVIA *Ovia) {
        bool EncodeSubset = false;
        if (Ovia != NULL) {
            EncodeSubset = Ovia->FLACInfo->EncodeSubset;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return EncodeSubset;
    }
    
    void OVIA_FLAC_SetMD5(OVIA *Ovia, uint8_t *MD5) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->StreamInfo->MD5 = MD5;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint8_t *OVIA_FLAC_GetMD5(OVIA *Ovia) {
        uint8_t *MD5 = NULL;
        if (Ovia != NULL) {
            MD5 = Ovia->FLACInfo->StreamInfo->MD5;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return MD5;
    }
    
    void OVIA_FLAC_CUE_SetCatalogID(OVIA *Ovia, char *CatalogID) {
        if (Ovia != NULL && CatalogID != NULL) {
            Ovia->FLACInfo->CueSheet->CatalogID = CatalogID;
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (CatalogID == NULL) {
            Log(Log_ERROR, __func__, U8("CatalogID Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_LPC_SetLPCOrder(OVIA *Ovia, uint8_t LPCOrder) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->LPC->LPCOrder = LPCOrder;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint8_t OVIA_FLAC_LPC_GetLPCPrecision(OVIA *Ovia) {
        uint8_t LPCPrecision = 0;
        if (Ovia != NULL) {
            LPCPrecision      = Ovia->FLACInfo->LPC->LPCPrecision;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return LPCPrecision;
    }
    
    void OVIA_FLAC_LPC_SetLPCPrecision(OVIA *Ovia, uint8_t LPCPrecision) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->LPC->LPCPrecision = LPCPrecision;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint8_t OVIA_FLAC_LPC_GetLPCShift(OVIA *Ovia) {
        uint8_t LPCShift = 0;
        if (Ovia != NULL) {
            LPCShift     = Ovia->FLACInfo->LPC->LPCShift;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return LPCShift;
    }
    
    void OVIA_FLAC_LPC_SetLPCShift(OVIA *Ovia, uint8_t LPCShift) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->LPC->LPCShift = LPCShift;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint8_t OVIA_FLAC_LPC_GetNumLPCCoeffs(OVIA *Ovia) {
        uint8_t NumLPCCoeffs = 0;
        if (Ovia != NULL) {
            NumLPCCoeffs     = Ovia->FLACInfo->LPC->NumLPCCoeffs;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return NumLPCCoeffs;
    }
    
    void OVIA_FLAC_LPC_SetNumLPCCoeffs(OVIA *Ovia, uint8_t NumLPCCoeffs) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->LPC->NumLPCCoeffs = NumLPCCoeffs;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint8_t OVIA_FLAC_LPC_GetLPCCoeff(OVIA *Ovia, uint8_t CoeffNum) {
        uint8_t Coeff = 0;
        if (Ovia != NULL) {
            Coeff     = Ovia->FLACInfo->LPC->LPCCoeff[Coeff];
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return Coeff;
    }
    
    void OVIA_FLAC_LPC_SetLPCCoeff(OVIA *Ovia, uint8_t CoeffNum, uint8_t Coeff) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->LPC->LPCCoeff[CoeffNum] = Coeff;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint8_t OVIA_FLAC_LPC_GetLPCOrder(OVIA *Ovia) {
        uint8_t LPCOrder = 0;
        if (Ovia != NULL) {
            LPCOrder      = Ovia->FLACInfo->LPC->LPCOrder;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return LPCOrder;
    }
    
    uint8_t OVIA_FLAC_LPC_GetRICEPartitionType(OVIA *Ovia) {
        uint8_t RICEPartitionType = 0;
        if (Ovia != NULL) {
            RICEPartitionType      = Ovia->FLACInfo->LPC->RicePartitionType;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return RICEPartitionType;
    }
    
    void OVIA_FLAC_LPC_SetRICEPartitionType(OVIA *Ovia, uint8_t RICEPartitionType) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->LPC->RicePartitionType = RICEPartitionType;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint8_t OVIA_FLAC_LPC_GetRICEPartitionOrder(OVIA *Ovia) {
        uint8_t RICEPartitionOrder = 0;
        if (Ovia != NULL) {
            RICEPartitionOrder     = Ovia->FLACInfo->LPC->PartitionOrder;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return RICEPartitionOrder;
    }
    
    void OVIA_FLAC_LPC_SetRICEParameter(OVIA *Ovia, uint8_t Partition, uint8_t Parameter) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->Rice->RICEParameter[Partition] = Parameter;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint8_t OVIA_FLAC_LPC_GetRICEParameter(OVIA *Ovia, uint8_t Partition) {
        uint8_t Parameter = 0;
        if (Ovia != NULL) {
            Parameter     = Ovia->FLACInfo->Rice->RICEParameter[Partition];
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return Parameter;
    }
    
    void OVIA_FLAC_LPC_SetRICEPartitionOrder(OVIA *Ovia, uint8_t RICEPartitionOrder) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->LPC->PartitionOrder = RICEPartitionOrder;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_Seek_SetSeekPoint(OVIA *Ovia, uint32_t SeekPoint, uint64_t Sample, uint64_t Offset, uint16_t FrameSize) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->SeekPoints->NumSeekPoints += 1;
            Ovia->FLACInfo->SeekPoints->SampleInTargetFrame[SeekPoint] = Sample;
            Ovia->FLACInfo->SeekPoints->OffsetFrom1stSample[SeekPoint] = Offset;
            Ovia->FLACInfo->SeekPoints->TargetFrameSize[SeekPoint]     = FrameSize;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_CUE_SetLeadIn(OVIA *Ovia, uint64_t LeadIn) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->CueSheet->LeadIn = LeadIn;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_CUE_SetIsCD(OVIA *Ovia, bool IsCD) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->CueSheet->IsCD = IsCD;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    bool OVIA_FLAC_CUE_GetIsCD(OVIA *Ovia) {
        bool IsCD = false;
        if (Ovia != NULL) {
            IsCD = Ovia->FLACInfo->CueSheet->IsCD;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return IsCD;
    }
    
    void OVIA_FLAC_CUE_SetNumTracks(OVIA *Ovia, uint8_t NumTracks) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->CueSheet->NumTracks = NumTracks;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_CUE_Track_SetOffset(OVIA *Ovia, uint8_t Track, uint64_t TrackOffset) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->CueSheet->Tracks[Track].TrackOffset[Track] = TrackOffset;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_IHDR_SetIHDR(OVIA *Ovia, uint32_t Height, uint32_t Width, uint8_t BitDepth, uint8_t ColorType, const bool Interlace) {
        if (Ovia != NULL && Height > 0 && Width > 0 && (BitDepth > 0 || BitDepth > 16) && (ColorType <= 6 && ColorType != 1 && ColorType != 5) && Interlace >= 0 && Interlace <= 1) {
            Ovia->PNGInfo->iHDR->Width      = Width;
            Ovia->PNGInfo->iHDR->Height     = Height;
            Ovia->PNGInfo->iHDR->BitDepth   = BitDepth;
            Ovia->PNGInfo->iHDR->ColorType  = ColorType;
            Ovia->PNGInfo->iHDR->Interlaced = Interlace;
            Ovia->Width                     = Width;
            Ovia->Height                    = Height;
            Ovia->BitDepth                  = BitDepth;
            
            
            
            
            OVIA_SetHeight(Ovia, Height);
            OVIA_SetWidth(Ovia, Width);
            OVIA_SetBitDepth(Ovia, BitDepth);
            OVIA_PNG_iHDR_SetColorType(Ovia, ColorType);
            OVIA_PNG_iHDR_SetInterlace(Ovia, Interlace);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (Height == 0) {
            Log(Log_ERROR, __func__, U8("Height is 0, which is invalid"));
        } else if (Width == 0) {
            Log(Log_ERROR, __func__, U8("Width is 0, which is invalid"));
        } else if (BitDepth == 0 || BitDepth > 16) {
            Log(Log_ERROR, __func__, U8("BitDepth %d is invalid, valid values range from 1-16"), BitDepth);
        } else if (ColorType > 6 || ColorType == 1 || ColorType == 5) {
            Log(Log_ERROR, __func__, U8("ColorType %d is invalid, valid values range from 0-6, excluding 1 and 5"), ColorType);
        } else if (Interlace > 1) {
            Log(Log_ERROR, __func__, U8("Interlace %d is invalid, valid values range from 0-1"), Interlace);
        }
    }
    
    void OVIA_PNG_ACTL_SetACTL(OVIA *Ovia, const uint32_t NumFrames, const uint32_t Times2Loop) {
        if (Ovia != NULL && NumFrames > 0) {
            Ovia->PNGInfo->acTL->NumFrames   = NumFrames;
            Ovia->PNGInfo->acTL->TimesToLoop = Times2Loop;
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (NumFrames == 0) {
            Log(Log_ERROR, __func__, U8("NumFrames is 0, that isn't possible..."));
        }
    }
    
    void OVIA_PNG_FCTL_SetFCTL(OVIA *Ovia, const uint32_t FrameNum, const uint32_t Width, const uint32_t Height, uint32_t XOffset, uint32_t YOffset, uint16_t DelayNumerator, uint16_t DelayDenominator, uint8_t DisposalType, uint8_t BlendType) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->fcTL->FrameNum              = FrameNum;
            Ovia->PNGInfo->fcTL->Width                 = Width;
            Ovia->PNGInfo->fcTL->Height                = Height;
            Ovia->PNGInfo->fcTL->XOffset               = XOffset;
            Ovia->PNGInfo->fcTL->YOffset               = YOffset;
            Ovia->PNGInfo->fcTL->FrameDelayNumerator   = DelayNumerator;
            Ovia->PNGInfo->fcTL->FrameDelayDenominator = DelayDenominator;
            Ovia->PNGInfo->fcTL->DisposeMethod         = DisposalType;
            Ovia->PNGInfo->fcTL->BlendMethod           = BlendType;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_SetAnimated(OVIA *Ovia, const bool PNGIsAnimated) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->IsAnimated = PNGIsAnimated;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_PNM_SetPNMType(OVIA *Ovia, PNMTypes PNMType) {
        if (Ovia != NULL) {
            Ovia->PNMInfo->Type = PNMType;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_PNM_SetTupleType(OVIA *Ovia, PNMTupleTypes TupleType) {
        if (Ovia != NULL) {
            Ovia->PNMInfo->TupleType = TupleType;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    PNMTypes OVIA_PNM_GetPNMType(OVIA *Ovia) {
        PNMTypes Type = UnknownPNM;
        if (Ovia != NULL) {
            Type      = Ovia->PNMInfo->Type;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return Type;
    }
    
    PNMTupleTypes OVIA_PNM_GetTupleType(OVIA *Ovia) {
        PNMTupleTypes Type = PNM_TUPLE_Unknown;
        if (Ovia != NULL) {
            Type      = Ovia->PNMInfo->TupleType;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return Type;
    }
    
    uint32_t OVIA_PNG_ACTL_GetNumFrames(OVIA *Ovia) {
        uint32_t NumFrames = 0;
        if (Ovia != NULL) {
            NumFrames = Ovia->PNGInfo->acTL->NumFrames;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return NumFrames;
    }
    
    uint32_t OVIA_PNG_ACTL_GetTimes2Loop(OVIA *Ovia) {
        uint32_t Times2Loop = 0;
        if (Ovia != NULL) {
            Times2Loop = Ovia->PNGInfo->acTL->TimesToLoop;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return Times2Loop;
    }
    
    uint32_t OVIA_PNG_FCTL_GetFrameNum(OVIA *Ovia) {
        uint32_t FrameNum = 0;
        if (Ovia != NULL) {
            FrameNum = Ovia->PNGInfo->fcTL->FrameNum;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return FrameNum;
    }
    
    uint32_t OVIA_PNG_FCTL_GetWidth(OVIA *Ovia) {
        uint32_t Width = 0;
        if (Ovia != NULL) {
            Width = Ovia->PNGInfo->fcTL->Width;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return Width;
    }
    
    uint32_t OVIA_PNG_FCTL_GetHeight(OVIA *Ovia) {
        uint32_t Height = 0;
        if (Ovia != NULL) {
            Height = Ovia->PNGInfo->fcTL->Height;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return Height;
    }
    
    uint32_t OVIA_PNG_FCTL_GetXOffset(OVIA *Ovia) {
        uint32_t XOffset = 0;
        if (Ovia != NULL) {
            XOffset = Ovia->PNGInfo->fcTL->XOffset;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return XOffset;
    }
    
    uint32_t OVIA_PNG_FCTL_GetYOffset(OVIA *Ovia) {
        uint32_t YOffset = 0;
        if (Ovia != NULL) {
            YOffset = Ovia->PNGInfo->fcTL->YOffset;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return YOffset;
    }
    
    uint16_t OVIA_PNG_FCTL_GetFrameDelayNumerator(OVIA *Ovia) {
        uint16_t FrameDelayNumerator = 0;
        if (Ovia != NULL) {
            FrameDelayNumerator = Ovia->PNGInfo->fcTL->FrameDelayNumerator;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return FrameDelayNumerator;
    }
    
    uint16_t OVIA_PNG_FCTL_GetFrameDelayDenominator(OVIA *Ovia) {
        uint16_t FrameDelayDenominator = 0;
        if (Ovia != NULL) {
            FrameDelayDenominator = Ovia->PNGInfo->fcTL->FrameDelayDenominator;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return FrameDelayDenominator;
    }
    
    uint8_t OVIA_PNG_FCTL_GetDisposeMethod(OVIA *Ovia) {
        uint8_t DisposeMethod = 0;
        if (Ovia != NULL) {
            DisposeMethod = Ovia->PNGInfo->fcTL->DisposeMethod;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return DisposeMethod;
    }
    
    uint8_t OVIA_PNG_FCTL_GetBlendMethod(OVIA *Ovia) {
        uint8_t BlendMethod = 0;
        if (Ovia != NULL) {
            BlendMethod = Ovia->PNGInfo->fcTL->BlendMethod;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return BlendMethod;
    }
    
    void OVIA_PNG_FCTL_SetFrameNum(OVIA *Ovia, uint32_t FrameNum) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->fcTL->FrameNum = FrameNum;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_FCTL_SetWidth(OVIA *Ovia, uint32_t Width) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->fcTL->Width = Width;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_FCTL_SetHeight(OVIA *Ovia, uint32_t Height) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->fcTL->Height = Height;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_FCTL_SetXOffset(OVIA *Ovia, uint32_t XOffset) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->fcTL->XOffset = XOffset;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_FCTL_SetYOffset(OVIA *Ovia, uint32_t YOffset) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->fcTL->YOffset = YOffset;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_FCTL_SetFrameDelayNumerator(OVIA *Ovia, uint16_t FrameDelayNumerator) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->fcTL->FrameDelayNumerator = FrameDelayNumerator;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_FCTL_SetFrameDelayDenominator(OVIA *Ovia, uint16_t FrameDelayDenominator) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->fcTL->FrameDelayDenominator = FrameDelayDenominator;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_FCTL_SetDisposeMethod(OVIA *Ovia, uint8_t DisposeMethod) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->fcTL->DisposeMethod = DisposeMethod;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_FCTL_SetBlendMethod(OVIA *Ovia, uint8_t BlendMethod) { // Last Set
        if (Ovia != NULL) {
            Ovia->PNGInfo->fcTL->BlendMethod = BlendMethod;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint8_t OVIA_PNG_STER_GetSterType(OVIA *Ovia) {
        uint8_t STERType = 0;
        if (Ovia != NULL) {
            STERType = Ovia->PNGInfo->sTER->StereoType;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return STERType;
    }
    
    void OVIA_PNG_STER_SetSterType(OVIA *Ovia, uint8_t sTERType) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->sTER->StereoType = sTERType;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_IHDR_SetColorType(OVIA *Ovia, uint8_t ColorType) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->iHDR->ColorType = ColorType;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint8_t OVIA_PNG_iHDR_GetColorType(OVIA *Ovia) {
        uint8_t ColorType = 0;
        if (Ovia != NULL) {
            ColorType = Ovia->PNGInfo->iHDR->ColorType;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return ColorType;
    }
    
    uint8_t OVIA_PNG_BKGD_GetBackgroundPaletteEntry(OVIA *Ovia) {
        uint8_t PaletteEntry = 0;
        if (Ovia != NULL) {
            PaletteEntry = Ovia->PNGInfo->bkGD->BackgroundPaletteEntry;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return PaletteEntry;
    }
    
    void OVIA_PNG_BKGD_SetBackgroundPaletteEntry(OVIA *Ovia, uint8_t PaletteEntry) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->bkGD->BackgroundPaletteEntry = PaletteEntry;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_CHRM_SetWhitePoint(OVIA *Ovia, uint32_t WhitePointX, uint32_t WhitePointY) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->cHRM->WhitePointX = WhitePointX;
            Ovia->PNGInfo->cHRM->WhitePointY = WhitePointY;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_CHRM_SetRed(OVIA *Ovia, uint32_t RedX, uint32_t RedY) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->cHRM->RedX = RedX;
            Ovia->PNGInfo->cHRM->RedY = RedY;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_CHRM_SetGreen(OVIA *Ovia, uint32_t GreenX, uint32_t GreenY) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->cHRM->GreenX = GreenX;
            Ovia->PNGInfo->cHRM->GreenY = GreenY;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_CHRM_SetBlue(OVIA *Ovia, uint32_t BlueX, uint32_t BlueY) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->cHRM->BlueX = BlueX;
            Ovia->PNGInfo->cHRM->BlueY = BlueY;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint32_t OVIA_PNG_CHRM_GetWhitePointX(OVIA *Ovia) {
        uint32_t WhitePointX = 0;
        if (Ovia != NULL) {
            WhitePointX = Ovia->PNGInfo->cHRM->WhitePointX;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return WhitePointX;
    }
    
    uint32_t OVIA_PNG_CHRM_GetWhitePointY(OVIA *Ovia) {
        uint32_t WhitePointY = 0;
        if (Ovia != NULL) {
            WhitePointY = Ovia->PNGInfo->cHRM->WhitePointY;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return WhitePointY;
    }
    
    uint32_t OVIA_PNG_CHRM_GetRedX(OVIA *Ovia) {
        uint32_t RedX = 0;
        if (Ovia != NULL) {
            RedX = Ovia->PNGInfo->cHRM->RedX;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return RedX;
    }
    
    uint32_t OVIA_PNG_CHRM_GetRedY(OVIA *Ovia) {
        uint32_t RedY = 0;
        if (Ovia != NULL) {
            RedY = Ovia->PNGInfo->cHRM->RedY;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return RedY;
    }
    
    uint32_t OVIA_PNG_CHRM_GetGreenX(OVIA *Ovia) {
        uint32_t GreenX = 0;
        if (Ovia != NULL) {
            GreenX = Ovia->PNGInfo->cHRM->GreenX;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return GreenX;
    }
    
    uint32_t OVIA_PNG_CHRM_GetGreenY(OVIA *Ovia) {
        uint32_t GreenY = 0;
        if (Ovia != NULL) {
            GreenY = Ovia->PNGInfo->cHRM->GreenY;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return GreenY;
    }
    
    uint32_t OVIA_PNG_CHRM_GetBlueX(OVIA *Ovia) {
        uint32_t BlueX = 0;
        if (Ovia != NULL) {
            BlueX = Ovia->PNGInfo->cHRM->BlueX;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return BlueX;
    }
    
    uint32_t OVIA_PNG_CHRM_GetBlueY(OVIA *Ovia) {
        uint32_t BlueY = 0;
        if (Ovia != NULL) {
            BlueY = Ovia->PNGInfo->cHRM->BlueY;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return BlueY;
    }
    
    void OVIA_PNG_OFFS_SetXOffset(OVIA *Ovia, int32_t XOffset) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->oFFs->XOffset = XOffset;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_OFFS_SetYOffset(OVIA *Ovia, int32_t YOffset) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->oFFs->YOffset = YOffset;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_OFFS_SetSpecifier(OVIA *Ovia, bool Specifier) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->oFFs->UnitSpecifier = Specifier;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    int32_t OVIA_PNG_OFFS_GetXOffset(OVIA *Ovia) {
        int32_t XOffset = 0;
        if (Ovia != NULL) {
            XOffset = Ovia->PNGInfo->oFFs->XOffset;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return XOffset;
    }
    
    int32_t OVIA_PNG_OFFS_GetYOffset(OVIA *Ovia) {
        int32_t YOffset = 0;
        if (Ovia != NULL) {
            YOffset = Ovia->PNGInfo->oFFs->YOffset;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return YOffset;
    }
    
    bool OVIA_PNG_OFFS_GetSpecifier(OVIA *Ovia) {
        bool Specifier = 0;
        if (Ovia != NULL) {
            Specifier = Ovia->PNGInfo->oFFs->UnitSpecifier;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return Specifier;
    }
    
    void OVIA_PNG_ICCP_SetProfileName(OVIA *Ovia, UTF8 *ProfileName) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->iCCP->ProfileName = UTF8_Clone(ProfileName);
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_ICCP_SetCompressionType(OVIA *Ovia, uint8_t CompressionType) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->iCCP->CompressionType = CompressionType;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_ICCP_SetProfileData(OVIA *Ovia, uint8_t *ProfileData) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->iCCP->CompressedICCPProfile = ProfileData;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_ICCP_SetProfileDataSize(OVIA *Ovia, uint64_t ProfileSize) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->iCCP->CompressedICCPProfileSize = ProfileSize;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    UTF8 *OVIA_PNG_ICCP_GetProfileName(OVIA *Ovia) {
        UTF8 *ProfileName = NULL;
        if (Ovia != NULL) {
            ProfileName   = Ovia->PNGInfo->iCCP->ProfileName;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return ProfileName;
    }
    
    uint8_t OVIA_PNG_ICCP_GetCompressionType(OVIA *Ovia) {
        uint8_t CompressionType = 0;
        if (Ovia != NULL) {
            CompressionType   = Ovia->PNGInfo->iCCP->CompressionType;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return CompressionType;
    }
    
    uint64_t OVIA_PNG_ICCP_GetProfileDataSize(OVIA *Ovia) {
        uint64_t ProfileSize = 0ULL;
        if (Ovia != NULL) {
            ProfileSize      = Ovia->PNGInfo->iCCP->CompressedICCPProfileSize;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return ProfileSize;
    }
    
    uint8_t *OVIA_PNG_ICCP_GetProfileData(OVIA *Ovia) {
        uint8_t *ProfileData = NULL;
        if (Ovia != NULL) {
            ProfileData      = Ovia->PNGInfo->iCCP->CompressedICCPProfile;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return ProfileData;
    }
    
    void OVIA_PNG_SBIT_SetGray(OVIA *Ovia, uint8_t GrayBitDepth) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->sBIT->Grayscale = GrayBitDepth;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_SBIT_SetRed(OVIA *Ovia, uint8_t RedBitDepth) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->sBIT->Red = RedBitDepth;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_SBIT_SetGreen(OVIA *Ovia, uint8_t GreenBitDepth) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->sBIT->Green = GreenBitDepth;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_SBIT_SetBlue(OVIA *Ovia, uint8_t BlueBitDepth) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->sBIT->Blue = BlueBitDepth;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_SBIT_SetAlpha(OVIA *Ovia, uint8_t AlphaBitDepth) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->sBIT->Alpha = AlphaBitDepth;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint8_t OVIA_PNG_SBIT_GetGray(OVIA *Ovia) {
        uint8_t GrayBitDepth = 0;
        if (Ovia != NULL) {
            GrayBitDepth     = Ovia->PNGInfo->sBIT->Grayscale;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return GrayBitDepth;
    }
    
    uint8_t OVIA_PNG_SBIT_GetRed(OVIA *Ovia) {
        uint8_t RedBitDepth = 0;
        if (Ovia != NULL) {
            RedBitDepth     = Ovia->PNGInfo->sBIT->Red;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return RedBitDepth;
    }
    
    uint8_t OVIA_PNG_SBIT_GetGreen(OVIA *Ovia) {
        uint8_t GreenBitDepth = 0;
        if (Ovia != NULL) {
            GreenBitDepth     = Ovia->PNGInfo->sBIT->Green;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return GreenBitDepth;
    }
    
    uint8_t OVIA_PNG_SBIT_GetBlue(OVIA *Ovia) {
        uint8_t BlueBitDepth = 0;
        if (Ovia != NULL) {
            BlueBitDepth     = Ovia->PNGInfo->sBIT->Blue;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return BlueBitDepth;
    }
    
    uint8_t OVIA_PNG_SBIT_GetAlpha(OVIA *Ovia) {
        uint8_t AlphaBitDepth = 0;
        if (Ovia != NULL) {
            AlphaBitDepth     = Ovia->PNGInfo->sBIT->Alpha;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return AlphaBitDepth;
    }
    
    void OVIA_PNG_SRGB_SetRenderingIntent(OVIA *Ovia, uint8_t RenderingIntent) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->sRGB->RenderingIntent = RenderingIntent;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint8_t OVIA_PNG_SRGB_GetRenderingIntent(OVIA *Ovia) {
        uint8_t RenderingIntent = 0;
        if (Ovia != NULL) {
            RenderingIntent     = Ovia->PNGInfo->sRGB->RenderingIntent;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return RenderingIntent;
    }
    
    void OVIA_PNG_PCAL_SetCalibrationName(OVIA *Ovia, UTF8 *CalibrationName) {
        if (Ovia != NULL && CalibrationName != NULL) {
            Ovia->PNGInfo->pCAL->CalibrationName = UTF8_Clone(CalibrationName);
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (CalibrationName == NULL) {
            Log(Log_ERROR, __func__, U8("CalibrationName Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_PHYS_SetPixelsPerUnitX(OVIA *Ovia, int32_t PixelsPerUnitX) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->pHYs->PixelsPerUnitXAxis = PixelsPerUnitX;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_PHYS_SetPixelsPerUnitY(OVIA *Ovia, int32_t PixelsPerUnitY) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->pHYs->PixelsPerUnitYAxis = PixelsPerUnitY;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_PHYS_SetUnitSpecifier(OVIA *Ovia, bool UnitSpecifier) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->pHYs->UnitSpecifier = UnitSpecifier;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    int32_t OVIA_PNG_PHYS_GetPixelsPerUnitX(OVIA *Ovia) {
        int32_t PixelsPerUnitX = 0;
        if (Ovia != NULL) {
            PixelsPerUnitX     = Ovia->PNGInfo->pHYs->PixelsPerUnitXAxis;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return PixelsPerUnitX;
    }
    
    int32_t OVIA_PNG_PHYS_GetPixelsPerUnitY(OVIA *Ovia) {
        int32_t PixelsPerUnitY = 0;
        if (Ovia != NULL) {
            PixelsPerUnitY     = Ovia->PNGInfo->pHYs->PixelsPerUnitYAxis;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return PixelsPerUnitY;
    }
    
    bool OVIA_PNG_PHYS_GetUnitSpecifier(OVIA *Ovia) {
        bool UnitSpecifier = 0;
        if (Ovia != NULL) {
            UnitSpecifier  = Ovia->PNGInfo->pHYs->UnitSpecifier;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return UnitSpecifier;
    }
    
    void OVIA_PNG_PLTE_Init(OVIA *Ovia, uint64_t NumEntries) {
        if (Ovia != NULL) {
            uint8_t NumChannels             = OVIA_GetNumChannels(Ovia);
            uint8_t BitDepth                = OVIA_GetBitDepth(Ovia);
            Ovia->PNGInfo->PLTE->NumEntries = NumEntries;
            Ovia->PNGInfo->PLTE->Palette    = calloc(NumEntries * NumChannels, BitDepth * NumChannels);
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_PLTE_SetPalette(OVIA *Ovia, uint64_t Entry, uint16_t Red, uint16_t Green, uint16_t Blue) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->PLTE->Palette[Entry][0] = Red;
            Ovia->PNGInfo->PLTE->Palette[Entry][1] = Green;
            Ovia->PNGInfo->PLTE->Palette[Entry][2] = Blue;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint8_t OVIA_PNG_PLTE_GetPaletteEntryRed(OVIA *Ovia, uint64_t Entry) {
        uint8_t Red = 0;
        if (Ovia != NULL) {
            Red = Ovia->PNGInfo->PLTE->Palette[Entry][0];
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return Red;
    }
    
    uint8_t OVIA_PNG_PLTE_GetPaletteEntryGreen(OVIA *Ovia, uint64_t Entry) {
        uint8_t Green = 0;
        if (Ovia != NULL) {
            Green = Ovia->PNGInfo->PLTE->Palette[Entry][1];
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return Green;
    }
    
    uint8_t OVIA_PNG_PLTE_GetPaletteEntryBlue(OVIA *Ovia, uint64_t Entry) {
        uint8_t Blue = 0;
        if (Ovia != NULL) {
            Blue = Ovia->PNGInfo->PLTE->Palette[Entry][2];
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return Blue;
    }
    
    void OVIA_PNG_TRNS_Init(OVIA *Ovia, uint64_t NumEntries) {
        if (Ovia != NULL) {
            uint8_t NumChannels             = OVIA_GetNumChannels(Ovia);
            uint8_t BitDepth                = OVIA_GetBitDepth(Ovia);
            Ovia->PNGInfo->tRNS->NumEntries = NumEntries;
            Ovia->PNGInfo->tRNS->Palette    = calloc(NumEntries, BitDepth);
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_TRNS_SetPalette(OVIA *Ovia, uint64_t Entry, uint8_t Alpha) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->tRNS->Palette[Entry] = Alpha;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_HIST_SetNumEntries(OVIA *Ovia, uint32_t NumEntries) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->hIST->Histogram  = calloc(1, NumEntries);
            if (Ovia->PNGInfo->hIST->Histogram != NULL) {
                Ovia->PNGInfo->hIST->NumColors  = NumEntries;
            } else {
                free(Ovia->PNGInfo->hIST->Histogram);
                Log(Log_ERROR, __func__, U8("Couldn't allocate PNG HIST Chunk"));
            }
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_TIME_SetTime(OVIA *Ovia, uint16_t Year, uint8_t Month, uint8_t Day, uint8_t Hour, uint8_t Minute, uint8_t Second) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->tIMe->Year   = Year;
            Ovia->PNGInfo->tIMe->Month  = Month;
            Ovia->PNGInfo->tIMe->Day    = Day;
            Ovia->PNGInfo->tIMe->Hour   = Hour;
            Ovia->PNGInfo->tIMe->Minute = Minute;
            Ovia->PNGInfo->tIMe->Second = Second;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint16_t OVIA_PNG_TIME_GetYear(OVIA *Ovia) {
        uint16_t Year = 0;
        if (Ovia != NULL) {
            Year = Ovia->PNGInfo->tIMe->Year;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return Year;
    }
    
    uint8_t OVIA_PNG_TIME_GetMonth(OVIA *Ovia) {
        uint8_t Month = 0;
        if (Ovia != NULL) {
            Month = Ovia->PNGInfo->tIMe->Month;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return Month;
    }
    
    uint8_t OVIA_PNG_TIME_GetDay(OVIA *Ovia) {
        uint8_t Day = 0;
        if (Ovia != NULL) {
            Day = Ovia->PNGInfo->tIMe->Day;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return Day;
    }
    
    uint8_t OVIA_PNG_TIME_GetHour(OVIA *Ovia) {
        uint8_t Hour = 0;
        if (Ovia != NULL) {
            Hour = Ovia->PNGInfo->tIMe->Hour;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return Hour;
    }
    
    uint8_t OVIA_PNG_TIME_GetMinute(OVIA *Ovia) {
        uint8_t Minute = 0;
        if (Ovia != NULL) {
            Minute = Ovia->PNGInfo->tIMe->Minute;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return Minute;
    }
    
    uint8_t OVIA_PNG_TIME_GetSecond(OVIA *Ovia) {
        uint8_t Second = 0;
        if (Ovia != NULL) {
            Second = Ovia->PNGInfo->tIMe->Second;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return Second;
    }
    
    void OVIA_PNG_DAT_SetCMF(OVIA *Ovia, uint8_t CMF) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->DAT->CMF =  CMF;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_DAT_SetFLG(OVIA *Ovia, uint8_t FLG) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->DAT->FLG =  FLG;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint8_t OVIA_PNG_DAT_GetCompressionMethod(OVIA *Ovia) {
        uint8_t CompressionMethod = 0;
        if (Ovia != NULL) {
            CompressionMethod = Ovia->PNGInfo->DAT->CMF & 0xF;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return CompressionMethod;
    }
    
    uint8_t OVIA_PNG_DAT_GetCompressionInfo(OVIA *Ovia) {
        uint8_t CompressionInfo = 0;
        if (Ovia != NULL) {
            CompressionInfo = Ovia->PNGInfo->DAT->CMF & 0xF0 >> 4;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return CompressionInfo;
    }
    
    void OVIA_PNG_DAT_SetFCHECK(OVIA *Ovia, uint8_t FCHECK) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->DAT->FLG  = FCHECK & 0xF8 >> 3;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint8_t OVIA_PNG_DAT_GetFCHECK(OVIA *Ovia) {
        uint8_t FCHECK = 0;
        if (Ovia != NULL) {
            FCHECK = Ovia->PNGInfo->DAT->FLG & 0xF8 >> 3;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return FCHECK;
    }
    
    uint8_t OVIA_PNG_DAT_GetFDICT(OVIA *Ovia) {
        uint8_t FDICT = 0;
        if (Ovia != NULL) {
            FDICT = Ovia->PNGInfo->DAT->FLG & 0x4 >> 2;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return FDICT;
    }
    
    uint8_t OVIA_PNG_DAT_GetFLEVEL(OVIA *Ovia) {
        uint8_t FLEVEL = 0;
        if (Ovia != NULL) {
            FLEVEL = Ovia->PNGInfo->DAT->FLG & 0x3;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return FLEVEL;
    }
    
    void OVIA_PNG_DAT_SetDictID(OVIA *Ovia, uint32_t DictID) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->DAT->DictID = DictID;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint32_t OVIA_PNG_DAT_GetDictID(OVIA *Ovia) {
        uint32_t DictID = 0;
        if (Ovia != NULL) {
            DictID = Ovia->PNGInfo->DAT->DictID;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return DictID;
    }
    
    void OVIA_PNG_DAT_SetArray(OVIA *Ovia, uint8_t *Array) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->DAT->ImageArray = Array;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint8_t *OVIA_PNG_DAT_GetArray(OVIA *Ovia) {
        uint8_t *Array = NULL;
        if (Ovia != NULL) {
            Array = Ovia->PNGInfo->DAT->ImageArray;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return Array;
    }
    
    void OVIA_PNG_DAT_SetArraySize(OVIA *Ovia, uint64_t ArraySize) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->DAT->ImageSize = ArraySize;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint64_t OVIA_PNG_DAT_GetArraySize(OVIA *Ovia) {
        uint64_t ArraySize = 0ULL;
        if (Ovia != NULL) {
            ArraySize = Ovia->PNGInfo->DAT->ImageSize;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return ArraySize;
    }
    
    void OVIA_PNG_DAT_SetArrayOffset(OVIA *Ovia, uint64_t ArrayOffset) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->DAT->ImageOffset = ArrayOffset;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint64_t OVIA_PNG_DAT_GetArrayOffset(OVIA *Ovia) {
        uint64_t ArrayOffset = 0ULL;
        if (Ovia != NULL) {
            ArrayOffset = Ovia->PNGInfo->DAT->ImageOffset;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return ArrayOffset;
    }
    
    void OVIA_PNG_DAT_SetLengthLiteralHuffmanTable(OVIA *Ovia, HuffmanTable *LengthLiteralTree) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->DAT->LengthLiteralTree = LengthLiteralTree;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    HuffmanTable *OVIA_PNG_DAT_GetLengthLiteralHuffmanTable(OVIA *Ovia) {
        HuffmanTable *LengthLiteralTree = NULL;
        if (Ovia != NULL) {
            LengthLiteralTree = Ovia->PNGInfo->DAT->LengthLiteralTree;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return LengthLiteralTree;
    }
    
    void OVIA_PNG_DAT_SetDistanceHuffmanTable(OVIA *Ovia, HuffmanTable *DistanceTree) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->DAT->DistanceTree = DistanceTree;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    HuffmanTable *OVIA_PNG_DAT_GetDistanceHuffmanTable(OVIA *Ovia) {
        HuffmanTable *DistanceTree = NULL;
        if (Ovia != NULL) {
            DistanceTree = Ovia->PNGInfo->DAT->DistanceTree;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return DistanceTree;
    }
    
    void OVIA_PNG_GAMA_SetGamma(OVIA *Ovia, uint32_t Gamma) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->gAMA->Gamma = Gamma;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_SCAL_SetSCAL(OVIA *Ovia, uint8_t UnitSpecifier, double Width, double Height) {
        if (Ovia != NULL) {
            Ovia->PNGInfo->sCAL->UnitSpecifier = UnitSpecifier;
            Ovia->PNGInfo->sCAL->PixelWidth    = Width;
            Ovia->PNGInfo->sCAL->PixelHeight   = Height;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_WAV_SetCompressionType(OVIA *Ovia, uint16_t CompressionType) {
        if (Ovia != NULL) {
            Ovia->WAVInfo->CompressionFormat = CompressionType;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_W64_SetCompressionType(OVIA *Ovia, uint16_t CompressionType) {
        if (Ovia != NULL) {
            Ovia->W64Info->CompressionFormat = CompressionType;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_WAV_SetBlockAlignment(OVIA *Ovia, uint16_t BlockAlignment) {
        if (Ovia != NULL) {
            Ovia->WAVInfo->BlockAlignment = BlockAlignment;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_W64_SetBlockAlignment(OVIA *Ovia, uint16_t BlockAlignment) {
        if (Ovia != NULL) {
            Ovia->W64Info->BlockAlignment = BlockAlignment;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint16_t OVIA_WAV_GetBlockAlignment(OVIA *Ovia) {
        uint16_t BlockAlignment = 0;
        if (Ovia != NULL) {
            BlockAlignment = Ovia->WAVInfo->BlockAlignment;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return BlockAlignment;
    }
    
    uint16_t OVIA_W64_GetBlockAlignment(OVIA *Ovia) {
        uint16_t BlockAlignment = 0;
        if (Ovia != NULL) {
            BlockAlignment = Ovia->W64Info->BlockAlignment;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return BlockAlignment;
    }
    
    void OVIA_BMP_SetDIBSize(OVIA *Ovia, uint32_t DIBSize) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->DIBSize = DIBSize;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_BMP_SetFileSize(OVIA *Ovia, uint32_t FileSize) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->FileSize = FileSize;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_BMP_SetOffset(OVIA *Ovia, uint32_t Offset) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->Offset = Offset;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_BMP_SetCompressionType(OVIA *Ovia, uint32_t CompressionType) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->CompressionType = CompressionType;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_BMP_SetNumBytesUsedBySamples(OVIA *Ovia, uint32_t NumBytesUsedBySamples) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->NumBytesUsedBySamples = NumBytesUsedBySamples;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_BMP_SetWidthInMeters(OVIA *Ovia, uint32_t WidthInMeters) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->WidthPixelsPerMeter = WidthInMeters;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_BMP_SetHeightInMeters(OVIA *Ovia, uint32_t HeightInMeters) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->HeightPixelsPerMeter = HeightInMeters;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_BMP_SetColorsIndexed(OVIA *Ovia, uint32_t ColorsIndexed) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->ColorsIndexed = ColorsIndexed;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_BMP_SetIndexColorsUsed(OVIA *Ovia, uint32_t IndexColorsUsed) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->IndexedColorsUsed = IndexColorsUsed;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_BMP_SetColorSpaceType(OVIA *Ovia, uint32_t ColorSpaceType) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->ColorSpaceType = ColorSpaceType;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_BMP_SetXCoordinate(OVIA *Ovia, uint32_t XCoordinate) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->XCoordinate = XCoordinate;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_BMP_SetYCoordinate(OVIA *Ovia, uint32_t YCoordinate) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->YCoordinate = YCoordinate;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_BMP_SetZCoordinate(OVIA *Ovia, uint32_t ZCoordinate) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->ZCoordinate = ZCoordinate;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_BMP_SetRGamma(OVIA *Ovia, uint32_t RGamma) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->RGamma = RGamma;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_BMP_SetGGamma(OVIA *Ovia, uint32_t GGamma) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->GGamma = GGamma;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_BMP_SetBGamma(OVIA *Ovia, uint32_t BGamma) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->BGamma = BGamma;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_BMP_SetICC(OVIA *Ovia, uint32_t ICCIntent, uint32_t ICCSize, uint8_t *ICCPayload) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->ICCIntent  = ICCIntent;
            Ovia->BMPInfo->ICCSize    = ICCSize;
            Ovia->BMPInfo->ICCPayload = ICCPayload;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_BMP_SetRMask(OVIA *Ovia, uint32_t RedMask) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->RMask = RedMask;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_BMP_SetGMask(OVIA *Ovia, uint32_t GreenMask) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->GMask = GreenMask;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_BMP_SetBMask(OVIA *Ovia, uint32_t BlueMask) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->BMask = BlueMask;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_BMP_SetAMask(OVIA *Ovia, uint32_t AlphaMask) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->AMask = AlphaMask;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint32_t OVIA_BMP_GetDIBSize(OVIA *Ovia) {
        uint32_t DIBSize = 0;
        if (Ovia != NULL) {
            DIBSize = Ovia->BMPInfo->DIBSize;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return DIBSize;
    }
    
    uint32_t OVIA_BMP_GetFileSize(OVIA *Ovia) {
        uint32_t FileSize = 0;
        if (Ovia != NULL) {
            FileSize = Ovia->BMPInfo->FileSize;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return FileSize;
    }
    
    uint32_t OVIA_BMP_GetOffset(OVIA *Ovia) {
        uint32_t Offset = 0;
        if (Ovia != NULL) {
            Offset = Ovia->BMPInfo->Offset;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return Offset;
    }
    
    uint32_t OVIA_BMP_GetCompressionType(OVIA *Ovia) {
        uint32_t CompressionType = 0;
        if (Ovia != NULL) {
            CompressionType = Ovia->BMPInfo->CompressionType;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return CompressionType;
    }
    
    uint32_t OVIA_BMP_GetNumBytesUsedBySamples(OVIA *Ovia) {
        uint32_t NumBytesUsedBySamples = 0;
        if (Ovia != NULL) {
            NumBytesUsedBySamples = Ovia->BMPInfo->NumBytesUsedBySamples;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return NumBytesUsedBySamples;
    }
    
    uint32_t OVIA_BMP_GetWidthInMeters(OVIA *Ovia) {
        uint32_t WidthInMeters = 0;
        if (Ovia != NULL) {
            WidthInMeters = Ovia->BMPInfo->WidthPixelsPerMeter;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return WidthInMeters;
    }
    
    uint32_t OVIA_BMP_GetHeightInMeters(OVIA *Ovia) {
        uint32_t HeightInMeters = 0;
        if (Ovia != NULL) {
            HeightInMeters = Ovia->BMPInfo->HeightPixelsPerMeter;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return HeightInMeters;
    }
    
    uint32_t OVIA_BMP_GetColorsIndexed(OVIA *Ovia) {
        uint32_t ColorsIndexed = 0;
        if (Ovia != NULL) {
            ColorsIndexed = Ovia->BMPInfo->ColorsIndexed;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return ColorsIndexed;
    }
    
    uint32_t OVIA_BMP_GetIndexColorsUsed(OVIA *Ovia) {
        uint32_t IndexColorsUsed = 0;
        if (Ovia != NULL) {
            IndexColorsUsed = Ovia->BMPInfo->IndexedColorsUsed;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return IndexColorsUsed;
    }
    
    uint32_t OVIA_BMP_GetColorSpaceType(OVIA *Ovia) {
        uint32_t ColorSpaceType = 0;
        if (Ovia != NULL) {
            ColorSpaceType = Ovia->BMPInfo->ColorSpaceType;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return ColorSpaceType;
    }
    
    uint32_t OVIA_BMP_GetXCoordinate(OVIA *Ovia) {
        uint32_t XCoordinate = 0;
        if (Ovia != NULL) {
            XCoordinate = Ovia->BMPInfo->YCoordinate;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return XCoordinate;
    }
    
    uint32_t OVIA_BMP_GetYCoordinate(OVIA *Ovia) {
        uint32_t YCoordinate = 0;
        if (Ovia != NULL) {
            YCoordinate = Ovia->BMPInfo->YCoordinate;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return YCoordinate;
    }
    
    uint32_t OVIA_BMP_GetZCoordinate(OVIA *Ovia) {
        uint32_t ZCoordinate = 0;
        if (Ovia != NULL) {
            ZCoordinate = Ovia->BMPInfo->ZCoordinate;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return ZCoordinate;
    }
    
    uint32_t OVIA_BMP_GetRGamma(OVIA *Ovia) {
        uint32_t RedGamma = 0;
        if (Ovia != NULL) {
            RedGamma = Ovia->BMPInfo->RGamma;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return RedGamma;
    }
    
    uint32_t OVIA_BMP_GetGGamma(OVIA *Ovia) {
        uint32_t GreenGamma = 0;
        if (Ovia != NULL) {
            GreenGamma = Ovia->BMPInfo->GGamma;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return GreenGamma;
    }
    
    uint32_t OVIA_BMP_GetBGamma(OVIA *Ovia) {
        uint32_t BlueGamma = 0;
        if (Ovia != NULL) {
            BlueGamma = Ovia->BMPInfo->BGamma;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return BlueGamma;
    }
    
    uint32_t OVIA_BMP_GetICCIntent(OVIA *Ovia) {
        uint32_t ICCIntent = 0;
        if (Ovia != NULL) {
            ICCIntent = Ovia->BMPInfo->ICCIntent;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return ICCIntent;
    }
    
    uint32_t OVIA_BMP_GetICCSize(OVIA *Ovia) {
        uint32_t ICCSize = 0;
        if (Ovia != NULL) {
            ICCSize = Ovia->BMPInfo->ICCSize;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return ICCSize;
    }
    
    uint8_t *OVIA_BMP_GetICCPayload(OVIA *Ovia) {
        uint8_t *ICCPayload = NULL;
        if (Ovia != NULL) {
            ICCPayload = Ovia->BMPInfo->ICCPayload;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return ICCPayload;
    }
    
    uint32_t OVIA_BMP_GetRMask(OVIA *Ovia) {
        uint32_t RedMask = 0;
        if (Ovia != NULL) {
            RedMask = Ovia->BMPInfo->RMask;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return RedMask;
    }
    
    uint32_t OVIA_BMP_GetGMask(OVIA *Ovia) {
        uint32_t GreenMask = 0;
        if (Ovia != NULL) {
            GreenMask = Ovia->BMPInfo->GMask;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return GreenMask;
    }
    
    uint32_t OVIA_BMP_GetBMask(OVIA *Ovia) {
        uint32_t BlueMask = 0;
        if (Ovia != NULL) {
            BlueMask = Ovia->BMPInfo->BMask;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return BlueMask;
    }
    
    uint32_t OVIA_BMP_GetAMask(OVIA *Ovia) {
        uint32_t AlphaMask = 0;
        if (Ovia != NULL) {
            AlphaMask = Ovia->BMPInfo->AMask;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return AlphaMask;
    }
    
    void OVIA_FLAC_Frame_SetBlockType(OVIA *Ovia, bool BlockTypeIsFixed) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->Frame->BlockType = BlockTypeIsFixed;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_Frame_SetCodedBlockSize(OVIA *Ovia, uint8_t CodedBlockSize) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->Frame->CodedBlockSize = CodedBlockSize;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_Frame_SetCodedSampleRate(OVIA *Ovia, uint8_t CodedSampleRate) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->Frame->CodedSampleRate = CodedSampleRate;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_Frame_SetSampleRate(OVIA *Ovia, uint32_t SampleRate) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->Frame->SampleRate = SampleRate;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_Frame_SetChannelLayout(OVIA *Ovia, uint8_t ChannelLayout) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->Frame->ChannelLayout = ChannelLayout;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_Frame_SetCodedBitDepth(OVIA *Ovia, uint8_t CodedBitDepth) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->Frame->CodedBitDepth = CodedBitDepth;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_Frame_SetFrameNumber(OVIA *Ovia, uint64_t FrameNumber) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->Frame->FrameNumber = FrameNumber;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_Frame_SetSampleNumber(OVIA *Ovia, uint64_t SampleNumber) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->Frame->SampleNumber = SampleNumber;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_Frame_SetBlockSize(OVIA *Ovia, uint16_t BlockSize) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->Frame->BlockSize = BlockSize;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    bool OVIA_FLAC_Frame_GetBlockType(OVIA *Ovia) {
        bool BlockType = false;
        if (Ovia != NULL) {
            BlockType = Ovia->FLACInfo->Frame->BlockType;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return BlockType;
    }
    
    uint8_t OVIA_FLAC_Frame_GetCodedBlockSize(OVIA *Ovia) {
        uint8_t CodedBlockSize = 0;
        if (Ovia != NULL) {
            CodedBlockSize = Ovia->FLACInfo->Frame->CodedBlockSize;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return CodedBlockSize;
    }
    
    uint8_t OVIA_FLAC_Frame_GetCodedSampleRate(OVIA *Ovia) {
        uint8_t CodedSampleRate = 0;
        if (Ovia != NULL) {
            CodedSampleRate = Ovia->FLACInfo->Frame->CodedSampleRate;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return CodedSampleRate;
    }
    
    uint8_t OVIA_FLAC_Frame_GetChannelLayout(OVIA *Ovia) {
        uint8_t ChannelLayout = 0;
        if (Ovia != NULL) {
            ChannelLayout = Ovia->FLACInfo->Frame->ChannelLayout;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return ChannelLayout;
    }
    
    uint8_t OVIA_FLAC_Frame_GetCodedBitDepth(OVIA *Ovia) {
        uint8_t CodedBitDepth = 0;
        if (Ovia != NULL) {
            CodedBitDepth = Ovia->FLACInfo->Frame->CodedBitDepth;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return CodedBitDepth;
    }
    
    uint64_t OVIA_FLAC_Frame_GetFrameNumber(OVIA *Ovia) {
        uint64_t FrameNumber = 0;
        if (Ovia != NULL) {
            FrameNumber = Ovia->FLACInfo->Frame->FrameNumber;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return FrameNumber;
    }
    
    uint64_t OVIA_FLAC_Frame_GetSampleNumber(OVIA *Ovia) {
        uint64_t SampleNumber = 0;
        if (Ovia != NULL) {
            SampleNumber = Ovia->FLACInfo->Frame->SampleNumber;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return SampleNumber;
    }
    
    void OVIA_FLAC_SubFrame_SetType(OVIA *Ovia, uint8_t SubframeType) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->Frame->Sub->SubFrameType   = SubframeType;
            if (SubframeType > 0) {
                Ovia->FLACInfo->Frame->Sub->LPCFilterOrder = (SubframeType & 0x1F) - 1;
            }
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_SubFrame_SetWastedBits(OVIA *Ovia, bool WastedBitsFlag, uint8_t NumWastedBits) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->Frame->Sub->WastedBitsFlag = WastedBitsFlag;
            Ovia->FLACInfo->Frame->Sub->WastedBits     = NumWastedBits;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    AudioContainer *OVIA_GetAudioContainerPointer(OVIA *Ovia) {
        AudioContainer *Audio = NULL;
        if (Ovia != NULL) {
            Audio = Ovia->Audio;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return Audio;
    }
    
    void OVIA_SetAudioContainerPointer(OVIA *Ovia, AudioContainer *Audio) {
        if (Ovia != NULL && Audio != NULL) {
            Ovia->Audio = Audio;
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (Audio == NULL) {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_CUE_SetTrack(OVIA *Ovia, uint64_t Offset, bool IsAudio, bool PreEmphasis, UTF8 *ISRC) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->CueSheet->NumTracks += 1;
            
            uint8_t CurrentTrack = Ovia->FLACInfo->CueSheet->NumTracks - 1;
            
            Ovia->FLACInfo->CueSheet->Tracks[CurrentTrack].TrackOffset = Offset;
            Ovia->FLACInfo->CueSheet->Tracks[CurrentTrack].IsAudio     = IsAudio;
            Ovia->FLACInfo->CueSheet->Tracks[CurrentTrack].PreEmphasis = PreEmphasis;
            Ovia->FLACInfo->CueSheet->Tracks[CurrentTrack].ISRC        = UTF8_Clone(ISRC);
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_Identify(OVIA *Ovia, BitBuffer *BitB) {
        uint64_t FileMagic64 = BitBuffer_PeekBits(MSByteFirst, LSBitFirst, BitB, 64);
        uint32_t FileMagic32 = FileMagic64 & 0xFFFFFFFF;
        uint16_t FileMagic16 = FileMagic64 & 0xFFFF;
        
        if (FileMagic16 == BMP_BM) {
            Ovia->Type          = ImageType;
            Ovia->Format        = BMPFormat;
        } else if (FileMagic16 == PNM_PBMA ||
                   FileMagic16 == PNM_PBMB ||
                   FileMagic16 == PNM_PPMA ||
                   FileMagic16 == PNM_PPMB ||
                   FileMagic16 == PNM_PGMA ||
                   FileMagic16 == PNM_PGMB ||
                   FileMagic16 == PNM_PAMB) {
            Ovia->Type          = ImageType;
            Ovia->Format        = PNMFormat;
        } else if (FileMagic32 == AIF_FORM) {
            Ovia->Type          = AudioType;
            Ovia->Format        = AIFFormat;
        } else if (FileMagic32 == 0x52494646) {
            Ovia->Type          = AudioType;
            Ovia->Format        = WAVFormat;
        } else if (FileMagic32 == 0x72696666) {
            Ovia->Type          = AudioType;
            Ovia->Format        = W64Format;
        } else if (FileMagic64 == PNGMagic) {
            Ovia->Type          = ImageType;
            Ovia->Format        = PNGFormat;
        } else {
            Log(Log_ERROR, __func__, U8("Unrecognized file magic 0x%llu"), FileMagic64);
        }
    }
    
    void OVIA_ParseMetadata(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia->Format == AIFFormat) {
            AIFParseMetadata(Ovia, BitB);
        } else if (Ovia->Format == WAVFormat) {
            WAVParseMetadata(Ovia, BitB);
        } else if (Ovia->Format == W64Format) {
            W64ParseMetadata(Ovia, BitB);
        } else if (Ovia->Format == BMPFormat) {
            BMPParseMetadata(Ovia, BitB);
        } else if (Ovia->Format == PNMFormat) {
            PNMParseMetadata(Ovia, BitB);
        } else if (Ovia->Format == PNGFormat) {
            PNGParseMetadata(Ovia, BitB);
        } else if (Ovia->Format == FLACFormat) {
            OVIA_FLAC_Parse_Blocks(Ovia, BitB);
        }
    }
    
    AudioContainer *OVIA_ExtractSamples(OVIA *Ovia, BitBuffer *BitB) {
        AudioContainer *Audio = NULL;
        if (Ovia != NULL) {
            if (Ovia->Format == AIFFormat) {
                Audio = AIFExtractSamples(Ovia, BitB);
            } else if (Ovia->Format == WAVFormat) {
                Audio = WAVExtractSamples(Ovia, BitB);
            } else if (Ovia->Format == W64Format) {
                Audio = W64ExtractSamples(Ovia, BitB);
            }
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return Audio;
    }
    
    ImageContainer *OVIA_ExtractImage(OVIA *Ovia, BitBuffer *BitB) {
        ImageContainer *Image = NULL;
        if (Ovia != NULL) {
            if (Ovia->Format == PNMFormat) {
                Image = PNMExtractImage(Ovia, BitB);
            } else if (Ovia->Format == BMPFormat) {
                Image = BMPExtractImage(Ovia, BitB);
            } else if (Ovia->Format == PNGFormat) {
                Image = PNGExtractImage(Ovia, BitB);
            }
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return Image;
    }
    
    void OVIA_InsertImage(OVIA *Ovia, ImageContainer *Image, BitBuffer *BitB) {
        if (Ovia != NULL) {
            if (Ovia->Format == PNMFormat) {
                PNMInsertImage(Image, BitB);
            } else if (Ovia->Format == BMPFormat) {
                BMPInsertImage(Image, BitB);
            } else if (Ovia->Format == PNGFormat) {
                OVIA_PNG_Image_Insert(Image, BitB, false, true);
            }
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_AppendSamples(OVIA *Ovia, AudioContainer *Audio, BitBuffer *BitB) {
        if (Ovia != NULL && Audio != NULL && BitB != NULL) {
            if (Ovia->Format == AIFFormat) {
                AIFAppendSamples(Ovia, Audio, BitB);
            } else if (Ovia->Format == WAVFormat) {
                WAVAppendSamples(Audio, BitB);
            } else if (Ovia->Format == W64Format) {
                W64AppendSamples(Audio, BitB);
            } else if (Ovia->Format == FLACFormat) {
                FLACAppendSamples(Ovia, Audio, BitB);
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (Audio == NULL) {
            Log(Log_ERROR, __func__, U8("AudioContainer Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void OVIA_WriteHeader(OVIA *Ovia, BitBuffer *BitB) {
        if (Ovia != NULL && BitB != NULL) {
            if (Ovia->Format == BMPFormat) {
                BMPWriteHeader(Ovia, BitB);
            } else if (Ovia->Format == PNMFormat) {
                PNMWriteHeader(Ovia, BitB);
            } else if (Ovia->Format == PNGFormat) {
                PNGWriteHeader(Ovia, BitB);
            } else if (Ovia->Format == AIFFormat) {
                AIFWriteHeader(Ovia, BitB);
            } else if (Ovia->Format == W64Format) {
                W64WriteHeader(Ovia, BitB);
            } else if (Ovia->Format == WAVFormat) {
                WAVWriteHeader(Ovia, BitB);
            } else if (Ovia->Format == FLACFormat) {
                FLACWriteHeader(Ovia, BitB);
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        }
    }
    
    void OVIA_SetFormat(OVIA *Ovia, OVIA_FileFormats Format) {
        if (Ovia != NULL && Format != UnknownFormat) {
            if (Format == AIFFormat || Format == WAVFormat ||Format == W64Format || Format == FLACFormat) {
                Ovia->Type   = AudioType;
                Ovia->Format = Format;
            } else if (Format == BMPFormat || Format == PNMFormat || Format == PNGFormat) {
                Ovia->Type   = ImageType;
                Ovia->Format = Format;
            }
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_Deinit(OVIA *Ovia) {
        if (Ovia != NULL) {
            free(Ovia->TagTypes);
            for (uint64_t Tag = 0ULL; Tag < Ovia->NumTags; Tag++) {
                free(Ovia->Tags[Tag]);
            }
            free(Ovia->Tags);
            free(Ovia);
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
#ifdef __cplusplus
}
#endif
