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
        uint32_t     Width;
        uint32_t     Height;
        uint16_t     BitDepth;
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
        uint32_t     Width;
        uint32_t     Height;
        uint32_t     NumChannels;
        uint32_t     MaxVal;
        uint32_t     BitDepth;
        uint32_t     TupleType;
    } PNMOptions;
    
    typedef struct AIFOptions {
        
    } AIFOptions;
    
    typedef struct WAVOptions {
        
    } WAVOptions;
    
    typedef struct W64Options {
        
    } W64Options;
    
    typedef struct FLACOptions {
        uint8_t         *MD5;
        uint32_t         MinFrameSize;
        uint32_t         MaxFrameSize;
        uint16_t         MinBlockSize;
        uint16_t         MaxBlockSize;
        uint16_t         MaxFilterOrder;
        uint8_t          MaxRICEPartitionOrder;
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
        BMPOptions      *BMPInfo;
        PNMOptions      *PNMInfo;
        PNGOptions      *PNGInfo;
        AIFOptions      *AIFInfo;
        WAVOptions      *WAVInfo;
        W64Options      *W64Info;
        FLACOptions     *FLACInfo;
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
            NumChannels = OVIA_GetNumChannels(Ovia);
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
    
    void IFFSkipPadding(BitBuffer *BitB, uint32_t SubChunkSize) {
        if (IsOdd(SubChunkSize) == true) {
            BitBuffer_Skip(BitB, 8);
        }
    }
    
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
            Ovia->FLACInfo->MinBlockSize = MinBlockSize;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_SetMaxBlockSize(OVIA *Ovia, uint16_t MaxBlockSize) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->MaxBlockSize = MaxBlockSize;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_SetMinFrameSize(OVIA *Ovia, uint16_t MinFrameSize) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->MinFrameSize = MinFrameSize;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_SetMaxFrameSize(OVIA *Ovia, uint16_t MaxFrameSize) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->MaxFrameSize = MaxFrameSize;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_SetMaxFilterOrder(OVIA *Ovia, uint16_t MaxFilterOrder) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->MaxFilterOrder = MaxFilterOrder;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_FLAC_SetMaxRicePartitionOrder(OVIA *Ovia, uint8_t MaxRICEPartitionOrder) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->MaxRICEPartitionOrder = MaxRICEPartitionOrder;
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
            Ovia->FLACInfo->MD5 = MD5;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_PNG_SetIHDR(OVIA *Ovia, uint32_t Height, uint32_t Width, uint8_t BitDepth, uint8_t ColorType const bool Interlace) {
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
        } else if (Enc == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (NumFrames == 0) {
            Log(Log_ERROR, __func__, U8("NumFrames is 0, that isn't possible..."));
        }
    }
    
    void OVIA_PNG_SetFCTL(OVIA *Ovia, const uint32_t FrameNum, const uint32_t FrameWidth, const uint32_t FrameHeight, uint32_t XOffset, uint32_t YOffset, uint16_t FrameDelayNumerator, uint16_t FrameDelayDenominator, uint8_t DisposalType, uint8_t BlendType) {
        
    }
    
    void OVIA_PNG_SetAnimated(OVIA *Ovia, const bool PNGIsAnimated) { // OVIA_PNG_SetAnimated
        if (Ovia != NULL) {
            Ovia->PNGInfo->IsAnimated = PNGIsAnimated;
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
    
    uint8_t OVIA_GetBitDepth(OVIA *Ovia) {
        uint8_t BitDepth = 0;
        if (Ovia != NULL) {
            
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
            BitDepth = (uint8_t) Ovia->BitDepth;
        
        return BitDepth;
    }
    
    uint64_t OVIA_GetNumChannels(OVIA *Ovia) {
        if (Ovia != NULL) {
            
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return OVIA_GetNumChannels(Ovia);
    }
    
    uint64_t OVIA_GetNumSamples(OVIA *Ovia) {
        if (Ovia != NULL) {
            
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        return Ovia->NumChannelAgnosticSamples;
    }
    
    AudioContainer *OVIA_ExtractSamples(OVIA *Ovia, BitBuffer *BitB) {
        AudioContainer *Audio = NULL;
        if (Ovia != NULL) {
            
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        if (Ovia->Format == AIFFormat) {
            Audio = AIFExtractSamples(Ovia, BitB);
        } else if (Ovia->Format == WAVFormat) {
            Audio = WAVExtractSamples(Ovia, BitB);
        } else if (Ovia->Format == W64Format) {
            Audio = W64ExtractSamples(Ovia, BitB);
        }
        return Audio;
    }
    
    ImageContainer *OVIA_ExtractImage(OVIA *Ovia, BitBuffer *BitB) {
        ImageContainer *Image = NULL;
        if (Ovia != NULL) {
            
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        if (Ovia->Format == PNMFormat) {
            Image = PNMExtractImage(Ovia, BitB);
        } else if (Ovia->Format == BMPFormat) {
            Image = BMPExtractImage(Ovia, BitB);
        } else if (Ovia->Format == PNGFormat) {
            Image = PNGExtractImage(Ovia, BitB);
        }
        return Image;
    }
    
    void OVIA_InsertImage(OVIA *Ovia, BitBuffer *BitB, ImageContainer *Image) {
        if (Ovia != NULL) {
            
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
        if (Ovia->Format == PNMFormat) {
            PNMInsertImage(Ovia, BitB, Image);
        } else if (Ovia->Format == BMPFormat) {
            BMPInsertImage(Ovia, BitB, Image);
        } else if (Ovia->Format == PNGFormat) {
            PNGInsertImage(Ovia, BitB, Image);
        }
    }
    
    void OVIA_AppendSamples(OVIA *Ovia, AudioContainer *Audio, BitBuffer *BitB) {
        if (Ovia != NULL && Audio != NULL && BitB != NULL) {
            if (Ovia->Format == AIFFormat) {
                AIFAppendSamples(Ovia, Audio, BitB);
            } else if (Ovia->Format == WAVFormat) {
                WAVAppendSamples(Audio, BitB);
            } else if (Ovia->Format == W64Format) {
                W64AppendSamples(Ovia, Audio, BitB);
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
