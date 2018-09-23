#include "../../Dependencies/FoundationIO/libFoundationIO/include/Macros.h"

#include "../include/libOVIA.h"

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
        PNGColorTypes ColorType;
        bool          Progressive;
        uint32_t      CRC;
    } iHDRChunk;
    
    typedef struct PLTEChunk {
        uint8_t  **Palette;
        uint8_t    NumEntries;
        uint32_t   CRC;
    } PLTEChunk;
    
    typedef struct tRNSChunk {
        uint8_t  **Palette;
        uint8_t    NumEntries;
        uint32_t   CRC;
    } tRNSChunk;
    
    typedef struct bkGDChunk {
        uint8_t    BackgroundPaletteEntry;
        uint16_t   Gray;
        uint16_t   Red;
        uint16_t   Green;
        uint16_t   Blue;
        uint32_t   CRC;
    } bkGDChunk;
    
    typedef struct sTERChunk {
        bool       StereoType:1;
        uint32_t   CRC;
    } sTERChunk;
    
    typedef struct cHRMChunk { // sRGB or iCCP overrides cHRM
        uint32_t   WhitePointX; // X = 0, Y = 1
        uint32_t   WhitePointY;
        uint32_t   RedX;
        uint32_t   RedY;
        uint32_t   GreenX;
        uint32_t   GreenY;
        uint32_t   BlueX;
        uint32_t   BlueY;
        uint32_t   CRC;
    } cHRMChunk;
    
    typedef struct gAMAChunk { // sRGB or iCCP overrides gAMA
        uint32_t   Gamma;
        uint32_t   CRC;
    } gAMAChunk;
    
    typedef struct oFFsChunk {
        int32_t    XOffset;
        int32_t    YOffset;
        bool       UnitSpecifier:1;
        uint32_t   CRC;
    } oFFsChunk;
    
    typedef struct iCCPChunk {
        UTF8      *ProfileName;
        uint8_t   *CompressedICCPProfile;
        uint64_t   CompressedICCPProfileSize;
        uint8_t    CompressionType;
        uint32_t   CRC;
    } iCCPChunk;
    
    typedef struct sBITChunk {
        uint8_t    Grayscale;
        uint8_t    Red;
        uint8_t    Green;
        uint8_t    Blue;
        uint8_t    Alpha;
        uint32_t   CRC;
    } sBITChunk;
    
    typedef struct sRGBChunk {
        uint8_t    RenderingIntent;
        uint32_t   CRC;
    } sRGBChunk;
    
    typedef struct pHYsChunk {
        uint32_t   PixelsPerUnitXAxis;
        uint32_t   PixelsPerUnitYAxis;
        uint8_t    UnitSpecifier;
        uint32_t   CRC;
    } pHYsChunk;
    
    typedef struct sPLTChunk {
        char      *Name;
        
    } sPLTChunk;
    
    typedef struct pCALChunk {
        UTF8      *CalibrationName;
        uint8_t   *UnitName;
        int32_t    OriginalZero;
        int32_t    OriginalMax;
        uint8_t    CalibrationNameSize;
        uint8_t    UnitNameSize;
        uint8_t    EquationType;
        uint8_t    NumParams;
        uint32_t   CRC;
    } pCALChunk;
    
    typedef struct sCALChunk {
        float      PixelWidth; // ASCII float
        float      PixelHeight; // ASCII float
        uint8_t    UnitSpecifier;
        uint32_t   CRC;
    } sCALChunk;
    
    typedef struct hISTChunk {
        uint32_t   NumColors;
        uint16_t  *Histogram; // For each PLTE entry, there needs to be 1 array element
        uint32_t   CRC;
    } hISTChunk;
    
    typedef struct TextChunk { // Replaces:  tEXt, iTXt, zTXt
        UTF8         *Keyword;
        UTF8         *Comment;
        PNGTextTypes  TextType;
        uint32_t      CRC;
    } TextChunk;
    
    typedef struct tIMeChunk {
        uint16_t   Year;
        uint8_t    Month:4;
        uint8_t    Day:5;
        uint8_t    Hour:5;
        uint8_t    Minute:6;
        uint8_t    Second:6;
        uint32_t   CRC;
    } tIMeChunk;
    
    typedef struct iDATChunk {
        uint32_t   Size;
        uint32_t   CRC;
    } iDATChunk;
    
    typedef struct acTLChunk {
        uint32_t   NumFrames;
        uint32_t   TimesToLoop;
        uint32_t   CRC;
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
        uint32_t   CRC;
    } fcTLChunk;
    
    typedef struct fdATChunk {
        uint32_t   FrameNum;
        uint32_t   CRC;
    } fdATChunk;
    
    typedef struct iENDChunk {
        uint32_t  Size;    // 0
        uint32_t  ChunkID; // iEND
        uint32_t  CRC;     // 0xAE426082
    } iENDChunk;
    
    typedef struct PNGOptions {
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
        sPLTChunk    *sPLT;
        oFFsChunk    *oFFs; // Extension, before first iDAT
        sCALChunk    *sCAL; // Extension, before first iDAT
        pCALChunk    *pCAL; // Extension, after PLTE, before iDAT
        tIMeChunk    *tIMe; // Order doesn't matter
        TextChunk    *Text; // Order doesn't matter
        iDATChunk    *iDAT;
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
        uint8_t  **ISRC;                // the tracks's ISRC number.
        uint64_t  *TrackOffset;         // samples from the beginning of the FLAC file to the first sample of the track
        uint8_t   *TrackNum;            // which track is this again?
        bool      *IsAudio;             // 1 for audio, 0 for data
        bool      *PreEmphasis;         // 1 for yes 0 for no.
        uint8_t    NumTrackIndexPoints;
    } CueSheetTrack;
    
    typedef struct FLACCueSheet {
        CueSheetTrack *Tracks;
        uint8_t       *NumTrackIndexPoints;
        char          *CatalogID;
        uint64_t       LeadIn;              // in samples
        uint64_t       IndexOffset;         // no idea. must be multiple of CD sector for cd sources
        size_t         CatalogIDSize;
        uint8_t        NumTracks;           // the number of tracks, CD has a max of 100 (including lead out)
        bool           IsCD;                // 1 if it came from a CD; 0 otherwise
        uint8_t        IndexPointNum;
    } FLACCueSheet;
    
    typedef struct FLACSeekTable {
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
        uint8_t SubFrameType:6;
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
        uint8_t       FLACFrameCRC;
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
        uint32_t         NumSeekPoints;
        bool             EncodeSubset;
    } FLACOptions;
    
    typedef struct OVIA {
        uint64_t         FileSize;
        uint64_t         NumChannels;
        uint64_t         BitDepth;
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
        OVIA_TagTypes   *TagTypes;
        
        OVIA_Types       Type;
        OVIA_FileFormats Format;
        bool             Is3D;
    } OVIA;
    
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
    
    uint64_t OVIA_GetBitDepth(OVIA *Ovia) {
        uint64_t BitDepth = 0;
        if (Ovia != NULL) {
            BitDepth      = Ovia->BitDepth;
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
            Ovia->NumTags += 1;
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
    
    /*
     Ok, so we have a PNG file, the user calls OVIA_Identify.
     
     OVIA_Identify sets the OVIA struct saying it's a PNG file.
     
     From then on, all OVIA calls act with the PNG versions.
     
     The user calls OVIA_ReadMetadata.
     
     The user strips the black lines from the image.
     
     the user then writes the file back to the disk.
     
     OVIA just needs to manage the user accessible metadata.
     
     the individual format handlers will handle reading manipulating and writing the data...
     */
    
    OVIA *OVIA_Init(void) {
        OVIA *Ovia        = calloc(1, sizeof(OVIA));
        if (Ovia != NULL) {
            
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return Ovia;
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
        bool EncodeSubset = No;
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
    
    void OVIA_FLAC_CUE_SetCatalogID(OVIA *Ovia, char *CatalogID) {
        if (Ovia != NULL && CatalogID != NULL) {
            Ovia->FLACInfo->CueSheet->CatalogID = CatalogID;
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (CatalogID == NULL) {
            Log(Log_ERROR, __func__, U8("CatalogID Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_SetIHDR(OVIA *Ovia, uint32_t Height, uint32_t Width, uint8_t BitDepth, uint8_t ColorType, const bool Interlace) {
        if (Ovia != NULL && Height > 0 && Width > 0 && (BitDepth > 0 || BitDepth > 16) && (ColorType <= 6 && ColorType != 1 && ColorType != 5) && Interlace >= 0 && Interlace <= 1) {
            OVIA_SetHeight(Ovia, Height);
            OVIA_SetWidth(Ovia, Width);
            OVIA_SetBitDepth(Ovia, BitDepth);
            OVIA_PNG_SetColorType(Ovia, BitDepth);
            OVIA_PNG_SetInterlace(Ovia, Interlace);
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
    
    void OVIA_PNG_SetSTER(OVIA *Ovia, const bool StereoType) {
        if (Ovia != NULL && StereoType >= 0 && StereoType <= 1) {
            Ovia->PNGInfo->sTER->StereoType = StereoType;
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (StereoType < 0 || StereoType > 1) {
            Log(Log_ERROR, __func__, U8("StereoType %d is invalid, valid values range from 0-1"));
        }
    }
    
    void OVIA_PNG_SetACTL(OVIA *Ovia, const uint32_t NumFrames, const uint32_t Times2Loop) {
        if (Ovia != NULL && NumFrames > 0) {
            Ovia->PNGInfo->acTL->NumFrames   = NumFrames;
            Ovia->PNGInfo->acTL->TimesToLoop = Times2Loop;
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (NumFrames == 0) {
            Log(Log_ERROR, __func__, U8("NumFrames is 0, that isn't possible..."));
        }
    }
    
    void OVIA_PNG_SetFCTL(OVIA *Ovia, const uint32_t FrameNum, const uint32_t Width, const uint32_t Height, uint32_t XOffset, uint32_t YOffset, uint16_t DelayNumerator, uint16_t DelayDenominator, uint8_t DisposalType, uint8_t BlendType) {
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
            Ovia->PNGInfo->iCCP->ProfileName = ProfileName;
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
        uint8_t CompressionType = NULL;
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
    
    uint8_t OVIA_PNG_ICCP_GetProfileData(OVIA *Ovia) {
        uint8_t *ProfileData = NULL;
        if (Ovia != NULL) {
            ProfileData      = Ovia->PNGInfo->iCCP->ProfileData;
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
    
    int32_t OVIA_PNG_PHYS_GetPixelsPerUnitX(OVIA *Ovia) {
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
    
    bool OVIA_FLAC_Frame_GetBlockType(OVIA *Ovia) {
        bool BlockType = 0;
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
    
    void OVIA_FLAC_Frame_SetCRC(OVIA *Ovia, uint8_t CRC) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->Frame->FLACFrameCRC = CRC;
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
        } else if (FileMagic64 == PNGMagic) {
            Ovia->Type          = ImageType;
            Ovia->Format        = PNGFormat;
        } else if (FileMagic32 == AIF_FORM) {
            Ovia->Type          = AudioType;
            Ovia->Format        = AIFFormat;
        } else if (FileMagic32 == 0x52494646) {
            Ovia->Type          = AudioType;
            Ovia->Format        = WAVFormat;
        } else if (FileMagic32 == 0x72696666) {
            Ovia->Type          = AudioType;
            Ovia->Format        = W64Format;
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
            FLACParseMetadata(Ovia, BitB);
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
                PNGInsertImage(Image, BitB);
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
