#include "../../include/Private/BMPCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    /*
     BitInput contains the size of the file... OVIA itself has no need for that information...
     */
    
    void OVIA_BMP_SetCompressionType(uint32_t CompressionType) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->CompressionType = CompressionType;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_BMP_SetNumBytesUsedBySamples(uint32_t NumBytesUsedBySamples) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->NumBytesUsedBySamples = NumBytesUsedBySamples;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_BMP_SetWidthInMeters(uint32_t WidthInMeters) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->WidthPixelsPerMeter = WidthInMeters;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_BMP_SetHeightInMeters(uint32_t HeightInMeters) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->HeightPixelsPerMeter = HeightInMeters;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_BMP_SetColorsIndexed(uint32_t ColorsIndexed) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->ColorsIndexed = ColorsIndexed;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_BMP_SetIndexColorsUsed(uint32_t IndexColorsUsed) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->IndexedColorsUsed = IndexColorsUsed;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_BMP_SetColorSpaceType(uint32_t ColorSpaceType) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->ColorSpaceType = ColorSpaceType;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_BMP_SetXCoordinate(uint32_t XCoordinate) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->XCoordinate = XCoordinate;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_BMP_SetYCoordinate(uint32_t YCoordinate) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->YCoordinate = YCoordinate;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_BMP_SetZCoordinate(uint32_t ZCoordinate) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->ZCoordinate = ZCoordinate;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_BMP_SetRGamma(uint32_t RGamma) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->RGamma = RGamma;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_BMP_SetGGamma(uint32_t GGamma) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->GGamma = GGamma;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_BMP_SetBGamma(uint32_t BGamma) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->BGamma = BGamma;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_BMP_SetICC(uint32_t ICCIntent, uint32_t ICCSize, uint8_t *ICCPayload) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->ICCIntent  = ICCIntent;
            Ovia->BMPInfo->ICCSize    = ICCSize;
            Ovia->BMPInfo->ICCPayload = ICCPayload;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_BMP_SetRMask(uint32_t RedMask) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->RMask = RedMask;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_BMP_SetGMask(uint32_t GreenMask) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->GMask = GreenMask;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_BMP_SetBMask(uint32_t BlueMask) {
        if (Ovia != NULL) {
            Ovia->BMPInfo->BMask = BlueMask;
        } else {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void OVIA_BMP_SetAMask(uint32_t AlphaMask) {
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
    
#ifdef __cplusplus
}
#endif
