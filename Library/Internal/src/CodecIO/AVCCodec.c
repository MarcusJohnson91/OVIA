#include "../../include/Private/Common/libModernAVC_Common.h"

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
    
    void WriteArithmetic(BitBuffer *BitB, double *ProbabilityTable[], size_t TableSize, uint64_t Bits2Encode) { // Use the least precision you can get away with to be as efficent as possible.
        if (BitB == NULL) {
            Log(LOG_ERR, "libModernAVC", "ResidualLuma", "Pointer to BitBuffer is NULL");
        } else {
            uint64_t High = 0xFFFFFFFFFFFFFFFFULL, Low = 0ULL, Range = 0ULL, Probability = 0ULL;
            while ((Bits2Encode >= High) && (Bits2Encode <= Low)) {
                Range = (High - Low) + 1;
                Probability = ProbabilityTable[Range]; // Probability should be an int table ordered on how often a symbol shows up, not it's quantized probability.
                
            }
        }
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
    
    uint8_t NumberOfMacroBlockPartitions(uint8_t MacroBlockType) { // NumMbPart
        return 0;
    }
    
    uint8_t GetNumSubMacroBlockPartitions(bool IsDirectFlag, uint8_t *MacroBlockType, uint8_t **SubMacroBlockType, uintptr_t CurrentMacroBlockAddress, uint8_t MacroBlockPartitionIndex) { // NumSubMbPart
        if (MacroBlockType[CurrentMacroBlockAddress] == B_Skip || MacroBlockType[CurrentMacroBlockAddress] == B_Direct_16x16) {
            IsDirectFlag = true;
        } else if (MacroBlockType[CurrentMacroBlockAddress] == B_8x8 && SubMacroBlockType[CurrentMacroBlockAddress][MacroBlockPartitionIndex] == B_Direct_8x8) {
            IsDirectFlag = true;
        }
        
        if (IsDirectFlag == true && DQID == 0 && nal_unit_type != NAL_AuxiliarySliceExtension) {
            return 4;
        } else if (IsDirectFlag == true && DQID > 0 && nal_unit_type == NAL_AuxiliarySliceExtension) {
            return 1;
        } else if (IsDirectFlag == false) {
            return NumSubMbPart(subMbType[CurrentMacroBlockAddress][MacroBlockPartitionIndex]);
        }
    }
    
    bool IsThereMoreDataInThisNAL() { // more_rbsp_data
        if (Enc == NULL) {
            Log(LOG_ERR, "libModernAVC", "IsThereMoreDataInThisNAL", "Pointer to EncodeAVC is NULL");
        } else {
            if (there is no more data) {
                return false;
            } else {
                // find the rightmost 1 bit, Given the position of this bit, which is the first bit (rbsp_stop_one_bit) of the rbsp_trailing_bits( ) syntax structure
            }
        }
    }
    
    typedef struct Arthimetic {
        uint16_t *ProbabilityTable;
        size_t    TableSize;
    } Arthimetic;
    
    typedef struct Probability {
        double Low;
        double High;
    } Probability;
    
    uint64_t ReadArithmetic(BitBuffer *BitB, uint64_t *MaximumTable, uint64_t *MinimumTable, size_t TableSize, uint64_t Bits2Decode) {
        // Read a bit at a time.
        double High = 1.0, Low = 0.0; // Decimal point is implied before the highest bit.
        return 0;
    }
    
    size_t GetSizeOfNALUnit(DecodeAVC *Dec, BitBuffer *BitB) {
        
    }
    
    bool IsThereMoreDataInThisNAL() { // more_rbsp_data, TODO: This function needs to be finished
        if (there is no more data) {
            return false;
        } else {
            // find the rightmost 1 bit, Given the position of this bit, which is the first bit (rbsp_stop_one_bit) of the rbsp_trailing_bits( ) syntax structure
        }
    }
    
#ifdef __cplusplus
}
#endif
