#include "../../include/CodecIO/AVCCodec.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h" /* Included for Assertions */
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    AVCOptions *AVCOptions_Init(void) {
        AVCOptions *Options           = calloc(1, sizeof(AVCOptions));
        AssertIO(Options != NULL);
        Options->NAL                  = calloc(1, sizeof(NALHeader));
        Options->SPS                  = calloc(1, sizeof(SequenceParameterSet));
        Options->PPS                  = calloc(1, sizeof(PictureParameterSet));
        Options->VUI                  = calloc(1, sizeof(VideoUsabilityInformation));
        Options->HRD                  = calloc(1, sizeof(HypotheticalReferenceDecoder));
        Options->SEI                  = calloc(1, sizeof(SupplementalEnhancementInfo));
        Options->Slice                = calloc(1, sizeof(Slice));
        Options->SVC                  = calloc(1, sizeof(ScalableVideoCoding));
        Options->DPS                  = calloc(1, sizeof(DepthParameterSet));
        Options->MacroBlock           = calloc(1, sizeof(MacroBlock));
        return Options;
    }

    static uint64_t ReadExpGolomb(AVCOptions *Options, BitBuffer *BitB, BufferIO_ByteOrders ByteOrder, BufferIO_BitOrders BitOrder, AVCGolombTypes Type) { // ue/me/te/se or when entropy_coding_mode_flag == 0

        // te = truncaed exp-golomb, special process
        // Read the number of 0 bits, counting them, stopping when the stop bit of 1 is seen, including the stop bit in the count
        size_t NumZeroBits = 0;
        while (BitBuffer_ReadBits(BitB, ByteOrder, BitOrder, 1) == 0) {
            NumZeroBits += 1;
        }
        uint64_t Unary = Exponentiate(2, NumZeroBits) - 1; // 4^2 = 15, 4 - 1 = 3
        uint64_t Binary = BitBuffer_ReadBits(BitB, ByteOrder, BitOrder, NumZeroBits); // 0b0001
        uint64_t CodeNum = Unary + Binary; // 16 + 1 = 17
        if (Type == Golomb_Unsigned) {
            return CodeNum; // 0; 3
        } else if (Type == Golomb_Signed) {
            int64_t Sign = (CodeNum & 1) - 1;
            return ((CodeNum >> 1) ^ Sign) + 1;
        } else if (Type == Golomb_Mapped) {
            return 0;
            if (Options->MacroBlock->BlockPattern == Intra_4x4 || Options->MacroBlock->BlockPattern == Intra_8x8) {

            } else if (Options->MacroBlock->BlockPattern == Inter_4x4 || Options->MacroBlock->BlockPattern == Inter_4x4) {

            }

            if (Options->SPS->ChromaArrayType == 0 || Options->SPS->ChromaArrayType == 3) {

            } else if (Options->SPS->ChromaArrayType == 1 || Options->SPS->ChromaArrayType == 2) {
                // Intra_4x4, Intra_8x8, Inter
            }
        }

        /*
         1 = 0
         01 = read one bit
         001 = read two bits
         */
        return 0;
    }
    
    bool AreAllViewsPaired(AVCOptions *Options) {
        AssertIO(Options != NULL);
        bool AllViewsPairedFlag = false;
        for (uint8_t View = 0; View < Options->SPS->ViewCount; View++) {
            AllViewsPairedFlag = (1 && Options->SPS->DepthViewPresent[View] && Options->SPS->TextureViewPresent[View]);
        }
        return AllViewsPairedFlag;
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
        return X + Y + Z - Minimum(X, Minimum(Y, Z)) - Maximum(X, Maximum(Y, Z));
    }
    
    int8_t Sign(double X) {
        return X >= 0 ? 1 : -1;
    }
    
    void WriteArithmetic(BitBuffer *BitB, double *ProbabilityTable[], size_t TableSize, uint64_t Bits2Encode) { // Use the least precision you can get away with to be as efficent as possible.
        AssertIO(BitB != NULL);
        uint64_t High = 0xFFFFFFFFFFFFFFFFULL, Low = 0ULL, Range = 0ULL, Probability = 0ULL;
        while ((Bits2Encode >= High) && (Bits2Encode <= Low)) {
            Range = (High - Low) + 1;
            Probability = ProbabilityTable[Range]; // Probability should be an int table ordered on how often a symbol shows up, not it's quantized probability.
            
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
        return 0;
    }
    
    bool IsThereMoreDataInThisNAL() { // more_rbsp_data
        /*
         if (there is no more data) {
         return false;
         } else {
         // find the rightmost 1 bit, Given the position of this bit, which is the first bit (rbsp_stop_one_bit) of the rbsp_trailing_bits( ) syntax structure
         }
         */
        return false;
    }
    
    uint64_t ReadArithmetic(BitBuffer *BitB, uint64_t *MaximumTable, uint64_t *MinimumTable, size_t TableSize, uint64_t Bits2Decode) {
        // Read a bit at a time.
        double High = 1.0, Low = 0.0; // Decimal point is implied before the highest bit.
        return 0;
    }
    
    size_t GetSizeOfNALUnit(AVCOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        return 0;
    }

    uint8_t CalculateNumberOfTimeStamps(AVCOptions *Options) { // PicOrderCount
        AssertIO(Options != NULL);
        uint8_t NumTimeStamps = 0;
        if ((Options->Slice->SliceIsInterlaced == false) && (Options->Slice->TopFieldOrderCount == Options->Slice->BottomFieldOrderCount)) {
            NumTimeStamps = 1;
        } else if (0 == 1) {

        }
        return 0;
    }

    uint8_t MacroBlockPartitionPredictionMode(AVCOptions *Options, uint8_t MacroBlockType, uint8_t PartitionNumber) {  // MbPartPredMode
        uint8_t ReturnValue = 0;
        AssertIO(Options != NULL);
        if (MacroBlockType == 0) {
            if (Options->MacroBlock->TransformSizeIs8x8 == true) {
                ReturnValue = Intra_8x8;
            } else {
                ReturnValue = Intra_4x4;
            }
        } else if (MacroBlockType == 1) {
            ReturnValue = I_16x16_0_0_0;
        } else if (MacroBlockType == 2) {
            ReturnValue  = I_16x16_1_0_0;
        } else if (MacroBlockType == 3) {
            ReturnValue  = I_16x16_2_0_0;
        } else if (MacroBlockType == 4) {
            ReturnValue  = I_16x16_3_0_0;
        } else if (MacroBlockType == 5) {
            ReturnValue  = I_16x16_0_1_0;
        } else if (MacroBlockType == 6) {
            ReturnValue  = I_16x16_1_1_0;
        } else if (MacroBlockType == 7) {
            ReturnValue  = I_16x16_2_1_0;
        } else if (MacroBlockType == 8) {
            ReturnValue  = I_16x16_3_1_0;
        } else if (MacroBlockType == 9) {
            ReturnValue  = I_16x16_0_2_0;
        } else if (MacroBlockType == 10) {
            ReturnValue  = I_16x16_1_2_0;
        } else if (MacroBlockType == 11) {
            ReturnValue  = I_16x16_2_2_0;
        } else if (MacroBlockType == 12) {
            ReturnValue  = I_16x16_3_2_0;
        } else if (MacroBlockType == 13) {
            ReturnValue  = I_16x16_0_0_1;
        } else if (MacroBlockType == 14) {
            ReturnValue  = I_16x16_1_0_1;
        } else if (MacroBlockType == 15) {
            ReturnValue  = I_16x16_2_0_1;
        } else if (MacroBlockType == 16) {
            ReturnValue  = I_16x16_3_0_1;
        } else if (MacroBlockType == 17) {
            ReturnValue  = I_16x16_0_1_1;
        } else if (MacroBlockType == 18) {
            ReturnValue  = I_16x16_1_1_1;
        } else if (MacroBlockType == 19) {
            ReturnValue  = I_16x16_2_1_1;
        } else if (MacroBlockType == 20) {
            ReturnValue  = I_16x16_3_1_1;
        } else if (MacroBlockType == 21) {
            ReturnValue  = I_16x16_0_2_1;
        } else if (MacroBlockType == 22) {
            ReturnValue  = I_16x16_1_2_1;
        } else if (MacroBlockType == 23) {
            ReturnValue  = I_16x16_2_2_1;
        } else if (MacroBlockType == 24) {
            ReturnValue  = I_16x16_3_2_1;
        } else if (MacroBlockType == 25) {
            ReturnValue  = I_PCM;
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
    
    void AVCOptions_Deinit(AVCOptions *Options) {
        free(Options->NAL);
        free(Options->SPS);
        free(Options->PPS);
        free(Options->VUI);
        free(Options->HRD);
        free(Options->SEI);
        free(Options->Slice);
        free(Options->SVC);
        free(Options->DPS);
        free(Options->MacroBlock);
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
