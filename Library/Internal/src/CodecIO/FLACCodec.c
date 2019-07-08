#include "../../include/Private/FLACCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    uint32_t Adker32(BitBuffer *BitB, uint64_t Start, uint64_t NumBytes) {
        uint32_t Output = 0;
        if (BitB != NULL && Start * 8 < BitBuffer_GetSize(BitB) && (Start + NumBytes) * 8 <= BitBuffer_GetSize(BitB)) {
            uint16_t A = 1;
            uint16_t B = 0;
            
            for (uint64_t Byte = Start; Byte < NumBytes - 1; Byte++) {
                uint8_t Value = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8);
                A = (A + Value) % 65521;
                B = (B + A)     % 65521;
            }
            
            Output = (B << 16) | A;
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (Start * 8 < BitBuffer_GetSize(BitB)) {
            Log(Log_DEBUG, __func__, U8("Start: %lld is larger than the BitBuffer %lld"), Start * 8, BitBuffer_GetSize(BitB));
        } else if ((Start + NumBytes) * 8 <= BitBuffer_GetSize(BitB)) {
            Log(Log_DEBUG, __func__, U8("End: %lld is larger than the BitBuffer %lld"), (Start + NumBytes) * 8, BitBuffer_GetSize(BitB));
        }
        return Output;
    }
    
    uint32_t CRC32(BitBuffer *BitB, uint64_t Start, uint64_t NumBytes) {
        uint32_t Output = -1;
        if (BitB != NULL && Start * 8 < BitBuffer_GetSize(BitB) && (Start + NumBytes) * 8 <= BitBuffer_GetSize(BitB)) {
            for (uint64_t Byte = Start; Byte < NumBytes - 1; Byte++) {
                uint32_t Polynomial = 0x82608EDB;
                uint8_t  Data       = BitBuffer_ReadBits(BitB, MSByteFirst, LSBitFirst, 8);
                
                Output               ^= Data;
                for (uint8_t Bit = 0; Bit < 8; Bit++) {
                    if (Output & 1) {
                        Output = (Output >> 1) ^ Polynomial;
                    } else {
                        Output >>= 1;
                    }
                }
            }
        } else if (BitB == NULL) {
            Log(Log_DEBUG, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (Start * 8 < BitBuffer_GetSize(BitB)) {
            Log(Log_DEBUG, __func__, U8("Start: %lld is larger than the BitBuffer %lld"), Start * 8, BitBuffer_GetSize(BitB));
        } else if ((Start + NumBytes) * 8 <= BitBuffer_GetSize(BitB)) {
            Log(Log_DEBUG, __func__, U8("End: %lld is larger than the BitBuffer %lld"), (Start + NumBytes) * 8, BitBuffer_GetSize(BitB));
        }
        return ~Output;
    }
    
    HuffmanTree *HuffmanBuildTree(uint64_t NumSymbols, const uint16_t *CodeLengths) {
        HuffmanTree *Tree   = calloc(1, sizeof(HuffmanTree));
        if (Tree != NULL) {
            Tree->Symbols    = calloc(NumSymbols, sizeof(uint16_t));
            Tree->Frequency  = calloc(NumSymbols, sizeof(uint16_t));
            uint64_t *Offset = calloc(NumSymbols, sizeof(uint64_t));
            
            if (Tree->Symbols != NULL && Tree->Frequency != NULL) {
                for (uint64_t Value = 0ULL; Value < NumSymbols - 1; Value++) {
                    Tree->Frequency[CodeLengths[Value]] += 1;
                }
                
                if (Tree->Frequency[0] == NumSymbols) {
                    Log(Log_DEBUG, __func__, U8("All frequencies are zero, that doesn't make sense..."));
                }
                
                for (uint64_t Length = 1ULL; Length < MaxBitsPerSymbol; Length++) {
                    Offset[Length + 1]   = Offset[Length] + Tree->Frequency[Length];
                }
                
                for (uint64_t Symbol = 0ULL; Symbol < NumSymbols; Symbol++) {
                    if (CodeLengths[Symbol] != 0) {
                        uint16_t Index       = Offset[CodeLengths[Symbol]] + 1;
                        Tree->Symbols[Index] = Symbol;
                    }
                }
            } else if (Tree->Frequency == NULL) {
                Log(Log_DEBUG, __func__, U8("Could not allocate Huffman Symbols"));
            } else if (Tree->Frequency == NULL) {
                Log(Log_DEBUG, __func__, U8("Could not allocate Huffman Frequency"));
            }
        } else {
            Log(Log_DEBUG, __func__, U8("Could not allocate Huffman Tree"));
        }
        return Tree;
    }
    
    void FLAC_SetMinBlockSize(uint16_t MinBlockSize) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->StreamInfo->MinimumBlockSize = MinBlockSize;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void FLAC_SetMaxBlockSize(uint16_t MaxBlockSize) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->StreamInfo->MaximumBlockSize = MaxBlockSize;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void FLAC_SetMinFrameSize(uint16_t MinFrameSize) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->StreamInfo->MinimumFrameSize = MinFrameSize;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void FLAC_SetMaxFrameSize(uint16_t MaxFrameSize) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->StreamInfo->MaximumFrameSize = MaxFrameSize;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void FLAC_SetMaxFilterOrder(uint16_t MaxFilterOrder) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->Frame->Sub->LPCFilterOrder = MaxFilterOrder;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void FLAC_SetMaxRicePartitionOrder(uint8_t MaxRICEPartitionOrder) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->Frame->PartitionOrder = MaxRICEPartitionOrder;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void FLAC_SetEncodeSubset(bool EncodeSubset) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->EncodeSubset = EncodeSubset;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    bool FLAC_GetEncodeSubset(OVIA *Ovia) {
        bool EncodeSubset = false;
        if (Ovia != NULL) {
            EncodeSubset = Ovia->FLACInfo->EncodeSubset;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
        return EncodeSubset;
    }
    
    void FLAC_SetMD5(uint8_t *MD5) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->StreamInfo->MD5 = MD5;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint8_t *FLAC_GetMD5(OVIA *Ovia) {
        uint8_t *MD5 = NULL;
        if (Ovia != NULL) {
            MD5 = Ovia->FLACInfo->StreamInfo->MD5;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
        return MD5;
    }
    
    void FLAC_CUE_SetCatalogID(char *CatalogID) {
        if (Ovia != NULL && CatalogID != NULL) {
            Ovia->FLACInfo->CueSheet->CatalogID = CatalogID;
        } else if (Ovia == NULL) {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        } else if (CatalogID == NULL) {
            Log(Log_DEBUG, __func__, U8("CatalogID Pointer is NULL"));
        }
    }
    
    void FLAC_LPC_SetLPCOrder(uint8_t LPCOrder) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->LPC->LPCOrder = LPCOrder;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint8_t FLAC_LPC_GetLPCPrecision(OVIA *Ovia) {
        uint8_t LPCPrecision = 0;
        if (Ovia != NULL) {
            LPCPrecision      = Ovia->FLACInfo->LPC->LPCPrecision;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
        return LPCPrecision;
    }
    
    void FLAC_LPC_SetLPCPrecision(uint8_t LPCPrecision) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->LPC->LPCPrecision = LPCPrecision;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint8_t FLAC_LPC_GetLPCShift(OVIA *Ovia) {
        uint8_t LPCShift = 0;
        if (Ovia != NULL) {
            LPCShift     = Ovia->FLACInfo->LPC->LPCShift;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
        return LPCShift;
    }
    
    void FLAC_LPC_SetLPCShift(uint8_t LPCShift) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->LPC->LPCShift = LPCShift;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint8_t FLAC_LPC_GetNumLPCCoeffs(OVIA *Ovia) {
        uint8_t NumLPCCoeffs = 0;
        if (Ovia != NULL) {
            NumLPCCoeffs     = Ovia->FLACInfo->LPC->NumLPCCoeffs;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
        return NumLPCCoeffs;
    }
    
    void FLAC_LPC_SetNumLPCCoeffs(uint8_t NumLPCCoeffs) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->LPC->NumLPCCoeffs = NumLPCCoeffs;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint8_t FLAC_LPC_GetLPCCoeff(uint8_t CoeffNum) {
        uint8_t Coeff = 0;
        if (Ovia != NULL) {
            Coeff     = Ovia->FLACInfo->LPC->LPCCoeff[Coeff];
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
        return Coeff;
    }
    
    void FLAC_LPC_SetLPCCoeff(uint8_t CoeffNum, uint8_t Coeff) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->LPC->LPCCoeff[CoeffNum] = Coeff;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint8_t FLAC_LPC_GetLPCOrder(OVIA *Ovia) {
        uint8_t LPCOrder = 0;
        if (Ovia != NULL) {
            LPCOrder      = Ovia->FLACInfo->LPC->LPCOrder;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
        return LPCOrder;
    }
    
    uint8_t FLAC_LPC_GetRICEPartitionType(OVIA *Ovia) {
        uint8_t RICEPartitionType = 0;
        if (Ovia != NULL) {
            RICEPartitionType      = Ovia->FLACInfo->LPC->RicePartitionType;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
        return RICEPartitionType;
    }
    
    void FLAC_LPC_SetRICEPartitionType(uint8_t RICEPartitionType) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->LPC->RicePartitionType = RICEPartitionType;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint8_t FLAC_LPC_GetRICEPartitionOrder(OVIA *Ovia) {
        uint8_t RICEPartitionOrder = 0;
        if (Ovia != NULL) {
            RICEPartitionOrder     = Ovia->FLACInfo->LPC->PartitionOrder;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
        return RICEPartitionOrder;
    }
    
    void FLAC_LPC_SetRICEParameter(uint8_t Partition, uint8_t Parameter) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->Rice->RICEParameter[Partition] = Parameter;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    uint8_t FLAC_LPC_GetRICEParameter(uint8_t Partition) {
        uint8_t Parameter = 0;
        if (Ovia != NULL) {
            Parameter     = Ovia->FLACInfo->Rice->RICEParameter[Partition];
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
        return Parameter;
    }
    
    void FLAC_LPC_SetRICEPartitionOrder(uint8_t RICEPartitionOrder) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->LPC->PartitionOrder = RICEPartitionOrder;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void FLAC_Seek_SetSeekPoint(uint32_t SeekPoint, uint64_t Sample, uint64_t Offset, uint16_t FrameSize) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->SeekPoints->NumSeekPoints += 1;
            Ovia->FLACInfo->SeekPoints->SampleInTargetFrame[SeekPoint] = Sample;
            Ovia->FLACInfo->SeekPoints->OffsetFrom1stSample[SeekPoint] = Offset;
            Ovia->FLACInfo->SeekPoints->TargetFrameSize[SeekPoint]     = FrameSize;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void FLAC_CUE_SetLeadIn(uint64_t LeadIn) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->CueSheet->LeadIn = LeadIn;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void FLAC_CUE_SetIsCD(bool IsCD) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->CueSheet->IsCD = IsCD;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    bool FLAC_CUE_GetIsCD(OVIA *Ovia) {
        bool IsCD = false;
        if (Ovia != NULL) {
            IsCD = Ovia->FLACInfo->CueSheet->IsCD;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
        return IsCD;
    }
    
    void FLAC_CUE_SetNumTracks(uint8_t NumTracks) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->CueSheet->NumTracks = NumTracks;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void FLAC_CUE_Track_SetOffset(uint8_t Track, uint64_t TrackOffset) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->CueSheet->Tracks[Track].TrackOffset[Track] = TrackOffset;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void FLAC_Frame_SetBlockType(bool BlockTypeIsFixed) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->Frame->BlockType = BlockTypeIsFixed;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void FLAC_Frame_SetCodedBlockSize(uint8_t CodedBlockSize) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->Frame->CodedBlockSize = CodedBlockSize;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void FLAC_Frame_SetCodedSampleRate(uint8_t CodedSampleRate) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->Frame->CodedSampleRate = CodedSampleRate;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void FLAC_Frame_SetSampleRate(uint32_t SampleRate) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->Frame->SampleRate = SampleRate;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void FLAC_Frame_SetChannelLayout(uint8_t ChannelLayout) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->Frame->ChannelLayout = ChannelLayout;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void FLAC_Frame_SetCodedBitDepth(uint8_t CodedBitDepth) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->Frame->CodedBitDepth = CodedBitDepth;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void FLAC_Frame_SetFrameNumber(uint64_t FrameNumber) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->Frame->FrameNumber = FrameNumber;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void FLAC_Frame_SetSampleNumber(uint64_t SampleNumber) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->Frame->SampleNumber = SampleNumber;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void FLAC_Frame_SetBlockSize(uint16_t BlockSize) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->Frame->BlockSize = BlockSize;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    bool FLAC_Frame_GetBlockType(OVIA *Ovia) {
        bool BlockType = false;
        if (Ovia != NULL) {
            BlockType = Ovia->FLACInfo->Frame->BlockType;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
        return BlockType;
    }
    
    uint8_t FLAC_Frame_GetCodedBlockSize(OVIA *Ovia) {
        uint8_t CodedBlockSize = 0;
        if (Ovia != NULL) {
            CodedBlockSize = Ovia->FLACInfo->Frame->CodedBlockSize;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
        return CodedBlockSize;
    }
    
    uint8_t FLAC_Frame_GetCodedSampleRate(OVIA *Ovia) {
        uint8_t CodedSampleRate = 0;
        if (Ovia != NULL) {
            CodedSampleRate = Ovia->FLACInfo->Frame->CodedSampleRate;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
        return CodedSampleRate;
    }
    
    uint8_t FLAC_Frame_GetChannelLayout(OVIA *Ovia) {
        uint8_t ChannelLayout = 0;
        if (Ovia != NULL) {
            ChannelLayout = Ovia->FLACInfo->Frame->ChannelLayout;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
        return ChannelLayout;
    }
    
    uint8_t FLAC_Frame_GetCodedBitDepth(OVIA *Ovia) {
        uint8_t CodedBitDepth = 0;
        if (Ovia != NULL) {
            CodedBitDepth = Ovia->FLACInfo->Frame->CodedBitDepth;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
        return CodedBitDepth;
    }
    
    uint64_t FLAC_Frame_GetFrameNumber(OVIA *Ovia) {
        uint64_t FrameNumber = 0;
        if (Ovia != NULL) {
            FrameNumber = Ovia->FLACInfo->Frame->FrameNumber;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
        return FrameNumber;
    }
    
    uint64_t FLAC_Frame_GetSampleNumber(OVIA *Ovia) {
        uint64_t SampleNumber = 0;
        if (Ovia != NULL) {
            SampleNumber = Ovia->FLACInfo->Frame->SampleNumber;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
        return SampleNumber;
    }
    
    void FLAC_SubFrame_SetType(uint8_t SubframeType) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->Frame->Sub->SubFrameType   = SubframeType;
            if (SubframeType > 0) {
                Ovia->FLACInfo->Frame->Sub->LPCFilterOrder = (SubframeType & 0x1F) - 1;
            }
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
    void FLAC_SubFrame_SetWastedBits(bool WastedBitsFlag, uint8_t NumWastedBits) {
        if (Ovia != NULL) {
            Ovia->FLACInfo->Frame->Sub->WastedBitsFlag = WastedBitsFlag;
            Ovia->FLACInfo->Frame->Sub->WastedBits     = NumWastedBits;
        } else {
            Log(Log_DEBUG, __func__, U8("OVIA Pointer is NULL"));
        }
    }
    
#ifdef __cplusplus
}
#endif
