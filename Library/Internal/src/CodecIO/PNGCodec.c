#include "../../include/Private/Audio/FLACCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
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
    
#ifdef __cplusplus
}
#endif
