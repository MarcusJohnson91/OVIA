#include "../../include/Private/Audio/FLACCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
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
    
#ifdef __cplusplus
}
#endif
