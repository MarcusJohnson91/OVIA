#include "../../Dependencies/BitIO/libBitIO/include/BitIO.h"

#include "../include/ModernAVCTypes.h"
#include "../include/Common.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    int64_t Min(int64_t X, int64_t Y) {
        return X <= Y ? X : Y;
    }
    
    int64_t Max(int64_t X, int64_t Y) {
        return X >= Y ? X : Y;
    }
    
    int64_t InverseRasterScan(int64_t A, int64_t B, int64_t C, int64_t D, int64_t E) {
        int64_t Result = 0;
        if (E == 0) {
            Result = (A % (D / B)) * B;
        } else if (E == 1) {
            Result = (A / (D / B)) * C;
        }
        return Result;
    }
    
    int64_t Clip3(int16_t X, int16_t Y, int16_t Z) {
        int64_t Result = 0;
        if (Z < X) {
            Result = X;
        } else if (Y < Z) {
            Result = Y;
        } else {
            Result = Z;
        }
        return Result;
    }
    
    int64_t Clip1Luma(int16_t X, uint8_t BitDepth) { // Clip1y
        return Clip3(0, (1 << BitDepth) -1, X);
    }
    
    int64_t Clip1Chroma(int16_t X, uint8_t BitDepth) { // Clip1c
        return Clip3(0, (1 << BitDepth) -1, X);
    }
    
    int64_t Median(double X, double Y, double Z) {
        return X + Y + Z - Min(X, Min(Y, Z)) - Max(X, Max(Y, Z));
    }
    
    int8_t Sign(double X) {
        return X >= 0 ? 1 : -1;
    }
    
    bool     AreAllViewsPaired(AVCFile *AVC) {
        bool AllViewsPairedFlag = false;
        for (uint8_t View = 0; View < AVC->SPS->ViewCount; View++) {
            AllViewsPairedFlag = (1 && AVC->SPS->DepthViewPresent[View] && AVC->SPS->TextureViewPresent[View]);
        }
        return AllViewsPairedFlag;
    }
    
    void ParseTransformCoeffs(AVCFile *AVC, uint8_t i16x16DC, uint8_t i16x16AC, uint8_t Level4x4, uint8_t Level8x8, uint8_t StartIndex, uint8_t EndIndex) { // residual_block
        uint8_t Intra16x16DCLevel = i16x16DC, Intra16x16ACLevel = i16x16AC, LumaLevel4x4 = Level4x4, LumaLevel8x8 = Level8x8;
        // Return the first 4 variables
        if (AVC->PPS->EntropyCodingMode == ExpGolomb) {
            // Read ExpGolomb
        } else {
            // Read Arithmetic
        }
        
        if (AVC->SPS->ChromaArrayType == Chroma420 || AVC->SPS->ChromaArrayType == Chroma422) {
            for (uint8_t iCbCr = 0; iCbCr < 1; iCbCr++) {
                ChromaDCLevel[iCbCr] = 4 * NumC8x8;
                for (uint8_t ChromaBlock = 0; ChromaBlock < i4x4; ChromaBlock++) {
                    ChromaACLevel[iCbCr][(i8x8 * 4) + i4x4] = 0;
                }
            }
        } else if (AVC->SPS->ChromaArrayType == Chroma444) {
            
        }
    }
    
    void ResidualLuma(AVCFile *AVC, BitBuffer *BitB, int i16x16DClevel, int i16x16AClevel, int level4x4,
                      int level8x8, int startIdx, int endIdx) { // residual_luma
        if (startIdx == 0 && MbPartPredMode(mb_type, 0) == Intra_16x16) {
            residual_block(i16x16DClevel, 0, 15, 16);
        }
        for (uint8_t i8x8 = 0; i8x8 < 4; i8x8++) {
            if (AVC->MacroBlock->TransformSizeIs8x8 == false || AVC->PPS->EntropyCodingMode == ExpGolomb) {
                for (uint8_t i4x4 = 0; i4x4 < 4; i4x4++) {
                    if (AVC->MacroBlock->BlockPatternLuma & (1 << i8x8)) {
                        if (MbPartPredMode(mb_type, 0) == Intra_16x16) {
                            residual_block(i16x16AClevel[i8x8 * 4 + i4x4], Max(0, startIdx - 1), endIdx - 1, 15);
                        } else {
                            residual_block(level4x4[i8x8 * 4 + i4x4], startIdx, endIdx, 16);
                        }
                    } else if (MbPartPredMode(mb_type, 0) == Intra_16x16) {
                        for (uint8_t i = 0; i < 15; i++) {
                            i16x16AClevel[i8x8 * 4 + i4x4][i] = 0;
                        }
                    } else {
                        for (uint8_t i = 0; i < 16; i++) {
                            level4x4[i8x8 * 4 + i4x4][i] = 0;
                        }
                    }
                }
            }
            if (AVC->PPS->EntropyCodingMode == ExpGolomb && AVC->MacroBlock->TransformSizeIs8x8) {
                for (uint8_t i = 0; i < 16; i++) {
                    level8x8[i8x8][4 * i + i4x4] = level4x4[i8x8 * 4 + i4x4][i];
                }
            }
            else if (AVC->MacroBlock->BlockPatternLuma & (1 << i8x8)) {
                residual_block(level8x8[i8x8], 4 * startIdx, 4 * endIdx + 3, 64);
            } else {
                for (uint8_t i = 0; i < 64; i++) {
                    level8x8[i8x8][i] = 0;
                }
            }
        }
    }
    
    int8_t MacroBlock2SliceGroupMap(AVCFile *AVC, uint8_t CurrentMacroBlock) { // MbToSliceGroupMap
        if ((AVC->PPS->SliceGroups == 1) &&
            (AVC->PPS->SliceGroupMapType == 3) ||
            (AVC->PPS->SliceGroupMapType == 4) ||
            (AVC->PPS->SliceGroupMapType == 5))) {
            if (AVC->PPS->SliceGroupMapType == 3) {
                if (AVC->PPS->SliceGroupChangeDirection == false) {
                    return BoxOutClockwise;
                } else if (AVC->PPS->SliceGroupChangeDirection == true) {
                    return BoxOutCounterClockwise;
                }
            } else if (AVC->PPS->SliceGroupMapType == 4) {
                if (AVC->PPS->SliceGroupChangeDirection == false) {
                    return RasterScan;
                } else if (AVC->PPS->SliceGroupChangeDirection == true) {
                    return ReverseRasterScan;
                }
            } else if (AVC->PPS->SliceGroupMapType == 5) {
                if (AVC->PPS->SliceGroupChangeDirection == false) {
                    return WipeRight;
                } else if (AVC->PPS->SliceGroupChangeDirection == true) {
                    return WipeLeft;
                }
            }
        }
        char Description[BitBOStringSize];
        snprintf(Description, BitBOStringSize, "SliceGroups: %hhu, SliceGroupMapType: %d", AVC->PPS->SliceGroups, AVC->PPS->SliceGroupMapType);
        Log(LOG_ERR, "LibAVC", "MacroBlock2SliceGroupMap", Description);
        return -1; // failure
    }
    
    typedef struct Arthimetic {
        uint16_t *ProbabilityTable;
        size_t    TableSize;
    } Arthimetic;
    
    typedef struct Probability {
        double Low;
        double High;
    } Probability;
    
    /*
     Probabilities FindProbabilityFromSymbol(Probabilities *Probability, double *MaximumTable, double *MinimumTable, size_t TableSize, uint64_t Symbol2Lookup) {
     if (Symbol2Lookup > TableSize) {
     // Not good.
     } else {
     Probability->Maximum = MaximumTable[Symbol2Lookup];
     Probability->Minimum = MinimumTable[Symbol2Lookup];
     }
     return *Probability;
     }
     
     uint64_t ReadArithmetic(BitBnput *Input, uint64_t *MaximumTable, uint64_t *MinimumTable, size_t TableSize, uint64_t Bits2Decode) {
     // Read a bit at a time.
     double High = 1.0, Low = 0.0; // Decimal point is implied before the highest bit.
     return 0;
     }
     
     void WriteArithmetic(BitBuffer *BitB, double *ProbabilityTable[], size_t TableSize, uint64_t Bits2Encode) { // Use the least precision you can get away with to be as efficent as possible.
     uint64_t High = 0xFFFFFFFFFFFFFFFFULL, Low = 0ULL, Range = 0ULL, Probability = 0ULL;
     while ((Bits2Encode >= High) && (Bits2Encode <= Low)) {
     Range = (High - Low) + 1;
     Probability = ProbabilityTable[Range]; // Probability should be an int table ordered on how often a symbol shows up, not it's quantized probability.
     
     }
     }
     
     // Create a function to lookup the symbol from the probabilities
     uint16_t FindSymbolFromProbability(double Probability, uint64_t	*MaximumTable, uint64_t *MinimumTable, size_t TableSize) {
     uint16_t Symbol = 0; // there is a SINGLE probability, not two...
     // If the probability is closer to 1 than 0, start the loop at 1, instead of 0. otherwise, start it at 0. to ensure it takes half the time to traverse it.
     
     bool WhichEnd = round(Probability);
     
     if (WhichEnd == 0) {
     for (uint64_t Index = 0; Index < TableSize; Index++) {
     uint64_t MaxProb   = MaximumTable[Index];
     uint64_t MinProb   = MinimumTable[Index];
     if ((Probability  >= MinProb) && (Probability <= MaxProb)) { // You found the symbol!
     Symbol = Index;
     }
     }
     } else {
     for (uint64_t Index = TableSize; Index > 0; Index--) {
     uint64_t MaxProb   = MaximumTable[Index];
     uint64_t MinProb   = MinimumTable[Index];
     if ((Probability  >= MinProb) && (Probability <= MaxProb)) { // You found the symbol!
     Symbol = Index;
     }
     }
     }
     return Symbol;
     }
     */
    
    void rbsp_slice_trailing_bits(AVCFile *AVC, BitBuffer *BitB) {
        AlignInput(BitB, 1); // rbsp_trailing_bits();
        if (AVC->PPS->EntropyCodingMode == Arithmetic) {
            while (more_rbsp_trailing_data()) {
                AVC->PPS->CABACZeroWord = ReadBits(BitB, 16, true); /* equal to 0x0000 */
            }
        }
    }
    
    uint8_t NumberOfMacroBlockPartitions(uint8_t MacroBlockType) { // NumMbPart
        
    }
    
    uint8_t MacroBlockPartitionPredictionMode(AVCFile *AVC, uint8_t MacroBlockType, uint8_t PartitionNumber) {  // MbPartPredMode
        uint8_t ReturnValue = 0;
        if (MacroBlockType == 0) {
            if (AVC->MacroBlock->TransformSizeIs8x8 == true) {
                RetunValue = Intra_8x8;
            } else {
                ReturnValue = Intra_4x4;
            }
        } else if (MacroBlockType == 1) {
            ReturnValue = I_16x16_0_0_0;
        } else if (MacroBlockType == 2) {
            ReurnValue  = I_16x16_1_0_0;
        } else if (MacroBlockType == 3) {
            ReurnValue  = I_16x16_2_0_0;
        } else if (MacroBlockType == 4) {
            ReurnValue  = I_16x16_3_0_0;
        } else if (MacroBlockType == 5) {
            ReurnValue  = I_16x16_0_1_0;
        } else if (MacroBlockType == 6) {
            ReurnValue  = I_16x16_1_1_0;
        } else if (MacroBlockType == 7) {
            ReurnValue  = I_16x16_2_1_0;
        } else if (MacroBlockType == 8) {
            ReurnValue  = I_16x16_3_1_0;
        } else if (MacroBlockType == 9) {
            ReurnValue  = I_16x16_0_2_0;
        } else if (MacroBlockType == 10) {
            ReurnValue  = I_16x16_1_2_0;
        } else if (MacroBlockType == 11) {
            ReurnValue  = I_16x16_2_2_0;
        } else if (MacroBlockType == 12) {
            ReurnValue  = I_16x16_3_2_0;
        } else if (MacroBlockType == 13) {
            ReurnValue  = I_16x16_0_0_1;
        } else if (MacroBlockType == 14) {
            ReurnValue  = I_16x16_1_0_1;
        } else if (MacroBlockType == 15) {
            ReurnValue  = I_16x16_2_0_1;
        } else if (MacroBlockType == 16) {
            ReurnValue  = I_16x16_3_0_1;
        } else if (MacroBlockType == 17) {
            ReurnValue  = I_16x16_0_1_1;
        } else if (MacroBlockType == 18) {
            ReurnValue  = I_16x16_1_1_1;
        } else if (MacroBlockType == 19) {
            ReurnValue  = I_16x16_2_1_1;
        } else if (MacroBlockType == 20) {
            ReurnValue  = I_16x16_3_1_1;
        } else if (MacroBlockType == 21) {
            ReurnValue  = I_16x16_0_2_1;
        } else if (MacroBlockType == 22) {
            ReurnValue  = I_16x16_1_2_1;
        } else if (MacroBlockType == 23) {
            ReurnValue  = I_16x16_2_2_1;
        } else if (MacroBlockType == 24) {
            ReurnValue  = I_16x16_3_2_1;
        } else if (MacroBlockType == 25) {
            ReurnValue  = I_PCM;
        }
        /*
         if (NotPartitioned == true) {
         return BlockPredictionMode;
         } else if (PartitionNumber >= 0) {
         PartitionMode;
         }
         }
         */
        return ReturnValue;
    }
    
    uint64_t NextMacroBlockAddress(AVCFile *AVC, uint64_t CurrentMacroBlockAddress) { // NextMbAddress
        while (CurrentMacroBlockAddress + 1 < AVC->Slice->PicSizeInMacroBlocks && MacroBlock2SliceGroupMap(AVC, CurrentMacroBlockAddress + 1) != MacroBlock2SliceGroupMap(AVC, CurrentMacroBlockAddress)) {
            i++; nextMbAddress = I
        }
        
        // aka
        for (uint64_t I = CurrentMacroBlockAddress + 1; I < AVC->Slice->PicSizeInMacroBlocks && MbToSliceGroups[I]) {
            MacroBlock2SliceGroupMap(AVC, CurrentMacroBlockAddress);
        }
    }
    
    uint8_t NumberOfSubMacroBlockPartitions() { // NumSubMbPart
        bool IsDirectFlag = 0;
        
        if (mbType[CurrMbAddr] == B_Skip || mbType[CurrMbAddr] == B_Direct_16x16) {
            IsDirectFlag = true;
        } else if (mbType[CurrMbAddr] == B_8x8 && subMbType[CurrMbAddr][mbPartIdx] == B_Direct_8x8) {
            IsDirectFlag = true;
        }
        
        if (isDirectFlag == true && DQID == 0 && nal_unit_type != NAL_AuxiliarySliceExtension) {
            return 4;
        } else if (isDirectFlag == true && DQID > 0 && nal_unit_type == NAL_AuxiliarySliceExtension) {
            return 1;
        } else if (isDirectFlag == 0) {
            return NumSubMbPart(subMbType[CurrMbAddr][mbPartIdx]);
        }
    }
    
    size_t GetSizeOfNALUnit(AVCFile *AVC, BitBuffer *BitB) {
        
    }
    
    bool IsThereMoreDataInThisNAL() { // more_rbsp_data
        if (there is no more data) {
            return false;
        } else {
            // find the rightmost 1 bit, Given the position of this bit, which is the first bit (rbsp_stop_one_bit) of the rbsp_trailing_bits( ) syntax structure
        }
    }
    
    uint8_t CalculateNumberOfTimeStamps(AVCFile *AVC) { // PicOrderCount
        uint8_t NumTimeStamps = 0;
        if ((AVC->Slice->SliceIsInterlaced == false) && (AVC->Slice->TopFieldOrderCount == AVC->Slice->BottomFieldOrderCount)) {
            NumTimeStamps = 1;
        } else if (0 == 1) {
            
        }
        return 0;
    }
    
    AVCFile *InitDecodeAVC(void) {
        AVCFile *AVC              = (AVCFile*)                      calloc(1, sizeof(AVCFile));
        AVC->NAL                  = (NALHeader*)                    calloc(1, sizeof(NALHeader));
        AVC->SPS                  = (SequenceParameterSet*)         calloc(1, sizeof(SequenceParameterSet));
        AVC->PPS                  = (PictureParameterSet*)          calloc(1, sizeof(PictureParameterSet));
        AVC->VUI                  = (VideoUsabilityInformation*)    calloc(1, sizeof(VideoUsabilityInformation));
        AVC->HRD                  = (HypotheticalReferenceDecoder*) calloc(1, sizeof(HypotheticalReferenceDecoder));
        AVC->SEI                  = (SupplementalEnhancementInfo*)  calloc(1, sizeof(SupplementalEnhancementInfo));
        AVC->Slice                = (Slice*)                        calloc(1, sizeof(Slice));
        AVC->SVC                  = (ScalableVideoCoding*)          calloc(1, sizeof(ScalableVideoCoding));
        AVC->DPS                  = (DepthParameterSet*)            calloc(1, sizeof(DepthParameterSet));
        AVC->MacroBlock           = (MacroBlock*)                   calloc(1, sizeof(MacroBlock));
        return AVC;
    }
    
    AVCFile *InitEncodeAVC(void) {
        AVCFile *AVC              = (AVCFile*)                      calloc(1, sizeof(AVCFile));
        AVC->NAL                  = (NALHeader*)                    calloc(1, sizeof(NALHeader));
        AVC->SPS                  = (SequenceParameterSet*)         calloc(1, sizeof(SequenceParameterSet));
        AVC->PPS                  = (PictureParameterSet*)          calloc(1, sizeof(PictureParameterSet));
        AVC->VUI                  = (VideoUsabilityInformation*)    calloc(1, sizeof(VideoUsabilityInformation));
        AVC->HRD                  = (HypotheticalReferenceDecoder*) calloc(1, sizeof(HypotheticalReferenceDecoder));
        AVC->SEI                  = (SupplementalEnhancementInfo*)  calloc(1, sizeof(SupplementalEnhancementInfo));
        AVC->Slice                = (Slice*)                        calloc(1, sizeof(Slice));
        AVC->SVC                  = (ScalableVideoCoding*)          calloc(1, sizeof(ScalableVideoCoding));
        AVC->DPS                  = (DepthParameterSet*)            calloc(1, sizeof(DepthParameterSet));
        AVC->MacroBlock           = (MacroBlock*)                   calloc(1, sizeof(MacroBlock));
        return AVC;
    }
    
#ifdef __cplusplus
}
#endif
