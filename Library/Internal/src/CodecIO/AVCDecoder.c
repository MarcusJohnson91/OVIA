#include "../../include/libModernAVC.h"
#include "../../include/Private/ModernAVCTypes.h"
#include "../../include/Private/Decode/DecodeAVCCommon.h"
#include "../../include/Private/Decode/DecodeAVC.h"
#include "../../include/Private/Decode/DecodeMacroBlock.h"
#include "../../include/Private/Decode/DecodeSlice.h"
#include "../../include/Private/Decode/ParseNAL.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    // Decode NALs, assembling packets as nessicary. this is the multiplexing stage.
    
    
    // Stage 1: Multiplex packets as necessary. (Multiplex stage 1)
    // Stage 2: Extract (re)assembled bitstream NALs
    // Stage 3: Decode NALs into the metadata/data
    // Stage 4: Realign the Slice groups and SPS/PPS into temporal decode order. (multiplex stage 2)
    // Stage 5: Start decoding the Slice groups into ordered macroblocks (Multiplex stage 3)
    
    
    // Within a NAL unit, 0x000000, 0x000001, and 0x000002 shall not occur at byte aligned positions.
    // Not shall 0x000003 occur at byte aligned postions, except: 0x00000300, 0x00000301, 0x00000302, 0x00000303
    // emulation_prevention_three_byte cannot be the third byte of a NAL unit.
    
    
    
    
    
    // MVCD  = Multi-View Coding with Depth, aka 3D.
    // AVC3D =

    // Basically I need to parse the NAL bytestream into the VCL (Video Coding Layer) aka Samples.
    
    // Find AVCMagic, then Find the NAL Size, then Parse the NAL, everything after step 1 needs to be on a loop.
    /*
    size_t FindNALSize(DecodeAVC *Dec, BitBuffer *BitB) {
        size_t   StartBufferPosition, EndOfBufferPosition, NALSize;
        bool     StreamIsByteAligned, NotEOF;
        uint32_t Marker;
        
        StartBufferPosition = GetBitBufferPosition(BitB) + Bits2Bytes(GetBitBufferPosition(BitB), false);
        StreamIsByteAligned = IsBitBufferAligned(BitB, 4);
        NotEOF              = GetBitBufferPosition(BitB) < GetBitBufferSize(BitB) ? false : true;
        Marker              = PeekBits(BitB, 24, true);
        
        if ((NotEOF == true && StreamIsByteAligned == true && ((Marker == 0x000000) || (Marker == 0x000001)))) {
            // Found the end of the NAL, record the size
            // This is gonna thrash the shit outta the buffer tho...
            EndOfBufferPosition = ftell(BitB->File) + Bits2Bytes(BitB->BitsUnavailable, true);
            NALSize = EndOfBufferPosition - StartBufferPosition;
            //UpdateInputBuffer(BitB, -NALSize); // Go back to the start of the NAL in order to read it.
        } else {
            SkipBits(BitB, 24);
        }
        return NALSize;
    }
    
    void ParseAVCFile(DecodeAVC *Dec, BitBuffer *BitB) { // byte_stream_nal_unit
        Log(LOG_INFO, "libModernAVC", "ParseAVCFile", "Parsing AVC File...\n");
        
        // Found a start code.
        if (ReadBits(BitB, 32, true) == AVCMagic && GetBitBufferPosition(BitB) == 0) {
            while (GetBitBufferPosition(BitB) + Bits2Bytes(BitB->BitsUnavailable, true) < BitB->FileSize) {
                ParseAVCHeader(Dec, BitB);
                FindNALSize(Dec, BitB);
                
                ScanNALUnits(Dec, BitB);
            }
        } else {
            SkipBits(BitB, 8);
        }
        Dec->CurrentFilePosition = GetBitBufferPosition(BitB) + Bits2Bytes(BitB->BitsUnavailable, false);
    }
    
    void ParseAVCHeader(DecodeAVC *Dec, BitBuffer *BitB) { // nal_unit
        size_t BytesInNALUnit                 = 1; // nalUnitHeaderBytes
        if (PeekBits(BitB, 1, true) == 0) {
            SkipBits(BitB, 1);
            Dec->NAL->NALRefIDC               = ReadBits(BitB, 2, true); // 0
            Dec->NAL->NALUnitType             = ReadBits(BitB, 5, true); // 6
            
            int     NumBytesInRBSP = 0;
            int nalUnitHeaderBytes = 1;
            
            if ((Dec->NAL->NALUnitType == NAL_PrefixUnit) || (Dec->NAL->NALUnitType == NAL_AuxiliarySliceExtension) ||(Dec->NAL->NALUnitType == NAL_MVCDepthView) ) {
                if (Dec->NAL->NALUnitType == NAL_MVCDepthView) {
                    Dec->NAL->AVC3DExtensionFlag = ReadBits(BitB, 1, true);
                } else {
                    Dec->NAL->SVCExtensionFlag   = ReadBits(BitB, 1, true);
                }
                
                if (Dec->NAL->SVCExtensionFlag == true) {
                    ParseNALSVCExtension(Dec, BitB);
                    BytesInNALUnit += 3;
                } else if (Dec->NAL->AVC3DExtensionFlag == true) {
                    ParseNAL3DAVCExtension(Dec, BitB);
                    BytesInNALUnit += 2;
                } else {
                    ParseNALMVCExtension(Dec, BitB);
                    BytesInNALUnit += 3;
                }
            }
            for (uint64_t NALByte = BytesInNALUnit; NALByte < Dec->NAL->NALUnitSize; NALByte++) {
                if (NALByte + 2 < Dec->NAL->NALUnitSize) {
                    uint32_t NALID = ReadBits(BitB, 24, true);
                    if (NALID == 0x000003) {
                        uint8_t NALID2 = ReadBits(BitB, 8, true);
                        if ((NALID2 == 0x00) || (NALID2 == 0x01) || (NALID2 == 0x02) || (NALID2 == 0x03)) {
                            // these guys are ok
                        } else {
                            // the official way
     
                            
                            // My way
                            SkipBits(BitB, 16);
                            BytesInNALUnit -=2;
                        }
                    }
                }
            } // We've cleared the main header, time to start decoding the NAL Units
        }
    }
    */
    
    /*
     I want to be able to open a file and de/en code H.264, but I ALSO want to use it as a library, but handing in each NAL.
     
     In order to facilitate both of those, I need a DecodeAVCFile function, and a DecodeNAL function, that's fed data by the calling program.
     */
    /*
    void ExtractNALFromByteStream(DecodeAVC *Dec, BitBuffer *BitB, size_t NALSize) {
        // Make sure the stream is byte algined by verifying there are 4 the data = 0x0000001
        // Once you've got that, you've got byte alignment.
        
        uint32_t ByteAlignment = ReadBits(BitB, 32, true);
        if (ByteAlignment == 0x00000001) { // Bytestream is aligned
            uint32_t StreamType = ReadBits(BitB, 24, true);
            if (StreamType == 0x000001) { // Start code prefix
                
            } else if (StreamType == 0x000003) { // emulation_prevention_three_byte
                
            }
            
            if ((ReadBits(BitB, 24, true) == 0x000000) && (IsStreamByteAligned(BitB->BitsUnavailable, 1) == true)) {
                
                
            }
        }
    }
     */
    
    uint64_t ScalingList(DecodeAVC *Dec, BitBuffer *BitB, uint8_t *scalingList, size_t ScalingListSize, bool UseDefaultScalingMatrixFlag) { // scaling_list
        uint8_t LastScale  = 8;
        uint8_t NextScale  = 8;
        uint8_t DeltaScale = 0;
        
        uint8_t ScalingList[ScalingListSize];
        
        for (uint64_t J = 0; J < ScalingListSize; J++) {
            if (NextScale != 0) {
                DeltaScale = ReadExpGolomb(BitB, true);
                NextScale  = (LastScale + DeltaScale + 256) % 256;
                UseDefaultScalingMatrixFlag = (J == 0 && NextScale == 0);
            }
            ScalingList[J] = (NextScale == 0) ? LastScale : NextScale;
            LastScale      = ScalingList[J];
        }
        
        return 0;
    }
    
    uint8_t weightScale4x4(int i, int j) {
        return V4x4[i][j];
    }
    
    uint8_t normAdjust4x4(int m, int i, int j) {
        uint8_t Data = 0;
        // M selects the vertical, i & j select the other
        if ((i % 2 == 0) && (j % 2 == 0)) {
            Data = V4x4[m][0];
        } else if ((i % 2 == 1) && (j % 2 == 0)) {
            Data = V4x4[m][1];
        } else {
            Data = V4x4[m][2];
        }
        return Data;
    }
    
    uint32_t LevelScale4x4(int m, int i, int j) {
        return weightScale4x4(i, j) * normAdjust4x4(m, i, j);
    }
    
    uint8_t weightScale8x8(int i, int j) {
        return V8x8[i][j];
    }
    
    uint8_t normAdjust8x8(int m, int i, int j) { // normAdjust8x8
        uint8_t Data = 0;
        if ((i % 4 == 0) && (j % 4 == 0)) {
            Data = V8x8[m][0];
        } else if ((i % 2 == 1) && (j % 2 == 1)) {
            Data = V8x8[m][1];
        } else if ((i % 4 == 2) && (j % 4 == 2)) {
            Data = V8x8[m][2];
        } else if (((i % 4 == 0) && (j % 2 == 1)) || ((i % 2 == 1) && (j % 4 == 0))) {
            Data = V8x8[m][3];
        } else if (((i % 4 == 0) && (j % 4 == 2)) || ((i % 4 == 2) && (j % 4 == 0))) {
            Data = V8x8[m][4];
        } else {
            Data = V8x8[m][5];
        }
        return Data;
    }
    
    uint8_t levelScale8x8(int m, int i, int j) {
        return weightScale8x8(i, j) * normAdjust8x8(m, i, j);
    }
    
    uint8_t SubMacroBlockType(uint8_t SubMacroBlockType, uint8_t MacroBlockPartitionIndex) { // sub_mb_type
        uint8_t Type = 0;
        if (SubMacroBlockType == B_SubMacroBlock) {
            if (MacroBlockPartitionIndex == 0) {
                Type = B_Direct_8x8;
            } else if (MacroBlockPartitionIndex == 1) {
                Type = B_L0_8x8;
            } else if (MacroBlockPartitionIndex == 2) {
                Type = B_L1_8x8;
            } else if (MacroBlockPartitionIndex == 3) {
                Type = B_Bi_8x8;
            } else if (MacroBlockPartitionIndex == 4) {
                Type = B_L0_8x4;
            } else if (MacroBlockPartitionIndex == 5) {
                Type = B_L0_4x8;
            } else if (MacroBlockPartitionIndex == 6) {
                Type = B_L1_8x4;
            } else if (MacroBlockPartitionIndex == 7) {
                Type = B_L1_4x8;
            } else if (MacroBlockPartitionIndex == 8) {
                Type = B_Bi_8x4;
            } else if (MacroBlockPartitionIndex == 9) {
                Type = B_Bi_4x8;
            } else if (MacroBlockPartitionIndex == 10) {
                Type = B_L0_4x4;
            } else if (MacroBlockPartitionIndex == 11) {
                Type = B_L1_4x4;
            } else if (MacroBlockPartitionIndex == 12) {
                Type = B_Bi_4x4;
            }
        } else if (SubMacroBlockType == P_SubMacroBlock) {
            if (MacroBlockPartitionIndex == 0) {
                Type = P_L0_8x8;
            } else if (MacroBlockPartitionIndex == 1) {
                Type = P_L0_8x4;
            } else if (MacroBlockPartitionIndex == 2) {
                Type = P_L0_4x8;
            } else if (MacroBlockPartitionIndex == 3) {
                Type = P_L0_4x4;
            }
        }
        return Type;
    }
    
    void AcquisitionElement3DV(DecodeAVC *Dec, BitBuffer *BitB, uint8_t numViews, uint8_t DeltaFlag, uint8_t PredDirection, uint8_t precMode, uint8_t expLen, uint8_t OutSign[MVCMaxViews], uint8_t OutExp, uint8_t OutMantissa, uint8_t OutManLen[MVCMaxViews]) { // 3dv_acquisition_element
        
        uint8_t element_equal_flag, numValues, matissa_len_minus1, prec, sign0, exponent0, mantissa0, skip_flag, sign1, exponent_skip_flag, manLen;
        uint8_t mantissaPred, mantissa_diff, exponent1;
        
        if (numViews - DeltaFlag > 1) {
            element_equal_flag = ReadBits(BitB, 1, true);
        }
        if (element_equal_flag == false) {
            numValues = numViews - DeltaFlag;
        } else {
            numValues = 1;
        }
        for (uint8_t Value = 0; Value < numValues; Value++) {
            if ((PredDirection == 2) && (Value = 0)) {
                if (precMode == 0) {
                    matissa_len_minus1 = ReadBits(BitB, 5, true) + 1;
                    OutManLen[index, Value] = manLen = matissa_len_minus1;
                } else {
                    prec = ReadBits(BitB, 5, true);
                }
            }
            if (PredDirection == 2) {
                sign0 = ReadBits(BitB, 1, true);
                OutSign[index, Value] = sign0;
                exponent0 = ReadExpGolomb(BitB, false);
                
                OutExp[index, Value] = exponent0;
                if (precMode == 1) {
                    if (exponent0 == 0) {
                        OutManLen[index, Value] = manLen = Max(0, prec - 30);
                    } else {
                        OutManLen[index, Value] = manLen = Max(0, exponent0 + prec - 31);
                    }
                }
                mantissa0 = ReadBits(BitB, manLen, true);
                OutMantissa[index, Value] = mantissa0;
            } else {
                skip_flag = ReadBits(BitB, 1, true);
                if (skip_flag == 0) {
                    sign1 = ReadBits(BitB, 1, true);
                    OutSign[index, Value] = sign1;
                    exponent_skip_flag = ReadBits(BitB, 1, true);
                    if (exponent_skip_flag == 0) {
                        exponent1 = ReadBits(BitB, expLen, true);
                        OutExp[index, Value] = exponent1;
                    } else {
                        OutExp[index, Value] = OutExp[Dec->DPS->ReferenceDPSID[1], Value];
                    }
                    mantissa_diff = ReadExpGolomb(BitB, true);
                    mantissaPred = ((OutMantissa[Dec->DPS->ReferenceDPSID[1], Value] * Dec->DPS->PredictedWeight0 + OutMantissa[Dec->DPS->ReferenceDPSID[1], Value] * (64 - Dec->DPS->PredictedWeight0) + 32) >> 6);
                    OutMantissa[index, Value] = mantissaPred + mantissa_diff;
                    OutManLen[index, Value] = OutManLen[Dec->DPS->ReferenceDPSID[1], Value];
                } else {
                    OutSign[index, Value] = OutSign[Dec->DPS->ReferenceDPSID[1], Value];
                    OutExp[index, Value] = OutExp[Dec->DPS->ReferenceDPSID[1], Value];
                    OutMantissa[index, Value] = OutMantissa[Dec->DPS->ReferenceDPSID[1], Value];
                    OutManLen[index, Value] = OutManLen[Dec->DPS->ReferenceDPSID[1], Value];
                }
            }
        }
        if (element_equal_flag == 1) {
            for (uint8_t View = 0; View < Dec->SPS->ViewCount; View++) {
                OutSign[index, View] = OutSign[index, 0];
                OutExp[index, View] = OutExp[index, 0];
                OutMantissa[index, View] = OutMantissa[index, 0];
                OutManLen[index, View] = OutManLen[index, 0];
            }
        }
    }
    
    uint64_t DepthRanges(BitBuffer *BitB, uint64_t NumberOfViews, bool PredDirection, uint64_t Index) { // depth_ranges
        bool NearFlag = ReadBits(BitB, 1, true);
        bool FarFlag  = ReadBits(BitB, 1, true);
        
        if (NearFlag == true) {
            AcquisitionElement3DV(NumberOfViews, 0, PredDirection, 7, 0, ZNearSign, ZNearExp, ZNearMantissa, ZNearManLen)
        } else if (FarFlag == true) {
            AcquisitionElement3DV(NumberOfViews, 0, PredDirection, 7, 0, ZFarSign, ZFarExp, ZFarMantissa, ZFarManLen);
        }
        
        return 0;
    }
    
    void vsp_param(DecodeAVC *Dec, BitBuffer *BitB, uint8_t numViews, uint8_t Direction, uint8_t DepthPS) { // vsp_param
        for (uint8_t View = 0; View < numViews; View++) { // Param
            for (uint8_t J = 0; J < View; J++) { // Wat
                int disparity_diff_wji[J][View] = ReadExpGolomb(BitB, false);
                int disparity_diff_oji[J][View] = ReadExpGolomb(BitB, false);
                int disparity_diff_wij[View][J] = ReadExpGolomb(BitB, false);
                int disparity_diff_oij[View][J] = ReadExpGolomb(BitB, false);
                if (Direction == 2) { /* Not 100% sure about the if loop part, but it makes more sense than for alone */
                    Dec->DisparityScale[DepthPS][J][View] = disparity_diff_wji[J][View];
                    Dec->DisparityOffset[DepthPS][J][View] = disparity_ diff_oji[J][View];
                    Dec->DisparityScale[DepthPS][View][J] = disparity_diff_wij[View][J] - disparity_diff_wji[J][View];
                    Dec->DisparityOffset[DepthPS][View][J] = disparity_diff_oij[View][J] - disparity_diff_oji[J][View];
                } else {
                    Dec->DisparityScale[DepthPS][J][View] = disparity_diff_wji[J][View] + (Dec->DisparityScale[Dec->DPS->ReferenceDPSID[0]][J][View] * Dec->DPS->PredictedWeight0 + Dec->DisparityScale[Dec->DPS->ReferenceDPSID[1]][J][View] * (64 - Dec->DPS->PredictedWeight0) + 32) >> 6;
                    Dec->DisparityOffset[DepthPS][J][View] = disparity_diff_oji[J][View] + (Dec->DisparityOffset[Dec->DPS->ReferenceDPSID[0]][J][View] * Dec->DPS->PredictedWeight0 + Dec->DisparityOffset[Dec->DPS->ReferenceDPSID[1]][J][View] * (64 - Dec->DPS->PredictedWeight0) + 32) >> 6;
                    Dec->DisparityScale[DepthPS][View][J] = disparity_diff_wij[View][J] + (Dec->DisparityScale[Dec->DPS->ReferenceDPSID[0]][View][J] * Dec->DPS->PredictedWeight0 + Dec->DisparityScale[Dec->DPS->ReferenceDPSID[1]][View][J] * (64 - Dec->DPS->PredictedWeight0) + 32) >> 6;
                    Dec->DisparityOffset[DepthPS][View][J] = disparity_diff_oij[View][J] + (Dec->DisparityOffset[Dec->DPS->ReferenceDPSID[0]][View][J] * Dec->DPS->PredictedWeight0 + Dec->DisparityOffset[Dec->DPS->ReferenceDPSID[1]][View][J] * (64 - Dec->DPS->PredictedWeight0) + 32) >> 6;
                }
            }
        }
    }
    
    void RefPicListMVCMod(DecodeAVC *Dec, BitBuffer *BitB) { // ref_pic_list_mvc_modification
        if (((Dec->Slice->Type % 5) != 2) && ((Dec->Slice->Type % 5) != 4)) {
            Dec->Slice->RefPicListModFlag[0] = ReadBits(BitB, 1, true);
            if (Dec->Slice->RefPicListModFlag[0] == true) {
                Dec->Slice->ModPicNumsIDC = ReadExpGolomb(BitB, false);
                if ((Dec->Slice->ModPicNumsIDC == 0) || (Dec->Slice->ModPicNumsIDC == 1)) {
                    Dec->Slice->AbsDiffPicNum = ReadExpGolomb(BitB, true) + 1;
                } else if (Dec->Slice->ModPicNumsIDC == 2) {
                    Dec->Slice->LongTermPicNum = ReadExpGolomb(BitB, false);
                } else if ((Dec->Slice->ModPicNumsIDC == 4) || (Dec->Slice->ModPicNumsIDC == 5)) {
                    Dec->Slice->AbsDiffViewIdx = ReadExpGolomb(BitB, false) + 1;
                }
            }
        }
        if ((Dec->Slice->Type % 5) == 1) {
            Dec->Slice->RefPicListModFlag[1] = ReadBits(BitB, 1, true);
            if (Dec->Slice->RefPicListModFlag[1] == true) {
                Dec->Slice->ModPicNumsIDC = ReadExpGolomb(BitB, false);
                if ((Dec->Slice->ModPicNumsIDC == 0) || (Dec->Slice->ModPicNumsIDC == 1)) {
                    Dec->Slice->AbsDiffPicNum = ReadExpGolomb(BitB, true) + 1;
                } else if (Dec->Slice->ModPicNumsIDC == 2) {
                    Dec->Slice->LongTermPicNum = ReadExpGolomb(BitB, false);
                } else if ((Dec->Slice->ModPicNumsIDC == 4) || (Dec->Slice->ModPicNumsIDC == 5)) {
                    Dec->Slice->AbsDiffViewIdx = ReadExpGolomb(BitB, false) + 1;
                }
            }
        }
    }
    
    void RefPicListMod(DecodeAVC *Dec, BitBuffer *BitB) { // ref_pic_list_modification
        if (((Dec->Slice->Type % 5) != 2) && ((Dec->Slice->Type % 5) != 4)) {
            Dec->Slice->RefPicListModFlag[0] = ReadBits(BitB, 1, true);
            if (Dec->Slice->RefPicListModFlag[0] == true) {
                Dec->Slice->ModPicNumsIDC = ReadExpGolomb(BitB, false);
                if ((Dec->Slice->ModPicNumsIDC == 0) || (Dec->Slice->ModPicNumsIDC == 1)) {
                    Dec->Slice->AbsDiffPicNum = ReadExpGolomb(BitB, true) + 1;
                } else if (Dec->Slice->ModPicNumsIDC == 2) {
                    Dec->Slice->LongTermPicNum = ReadExpGolomb(BitB, false);
                } else if ((Dec->Slice->ModPicNumsIDC == 4) || (Dec->Slice->ModPicNumsIDC == 5)) {
                    Dec->Slice->AbsDiffViewIdx = ReadExpGolomb(BitB, false) + 1;
                }
            }
        }
        if ((Dec->Slice->Type % 5) == 1) {
            Dec->Slice->RefPicListModFlag[1] = ReadBits(BitB, 1, true);
            if (Dec->Slice->RefPicListModFlag[1] == true) {
                Dec->Slice->ModPicNumsIDC = ReadExpGolomb(BitB, false);
                if ((Dec->Slice->ModPicNumsIDC == 0) || (Dec->Slice->ModPicNumsIDC == 1)) {
                    Dec->Slice->AbsDiffPicNum = ReadExpGolomb(BitB, true) + 1;
                } else if (Dec->Slice->ModPicNumsIDC == 2) {
                    Dec->Slice->LongTermPicNum = ReadExpGolomb(BitB, false);
                } else if ((Dec->Slice->ModPicNumsIDC == 4) || (Dec->Slice->ModPicNumsIDC == 5)) {
                    Dec->Slice->AbsDiffViewIdx = ReadExpGolomb(BitB, false) + 1;
                }
            }
        }
    }
    
    void pred_weight_table(DecodeAVC *Dec, BitBuffer *BitB) { // pred_weight_table
        Dec->Slice->LumaWeightDenom = ReadExpGolomb(BitB, true);
        if (Dec->SPS->ChromaArrayType != ChromaBW) {
            Dec->Slice->ChromaWeightDenom = ReadExpGolomb(BitB, false);
        }
        for (uint8_t i = 0; i <= Dec->MacroBlock->NumRefIndexActive[0]; i++) {
            Dec->Slice->LumaWeightFlag[0] = ReadBits(BitB, 1, true);
            if (Dec->Slice->LumaWeightFlag[0] == true) {
                Dec->Slice->LumaWeight[0][i]  = ReadExpGolomb(BitB, true);
                Dec->Slice->LumaOffset[0][i]  = ReadExpGolomb(BitB, true);
            }
            if (Dec->SPS->ChromaArrayType != ChromaBW) {
                Dec->Slice->ChromaWeightFlag[0] = ReadBits(BitB, 1, true);
                if (Dec->Slice->ChromaWeightFlag[0] == true) {
                    for (int J = 0; J < 2; J++) {
                        Dec->Slice->ChromaWeight[0][i][J] = ReadExpGolomb(BitB, true);
                        Dec->Slice->ChromaOffset[0][i][J] = ReadExpGolomb(BitB, true);
                    }
                }
            }
        }
        if ((Dec->Slice->Type % 5) == 1) {
            for (uint8_t i = 0; i <= Dec->MacroBlock->NumRefIndexActive[1]; i++) {
                Dec->Slice->LumaWeightFlag[1] = ReadBits(BitB, 1, true);
                if (Dec->Slice->LumaWeightFlag[1] == true) {
                    Dec->Slice->LumaWeight[1][i]  = ReadExpGolomb(BitB, true);
                    Dec->Slice->LumaOffset[1][i]  = ReadExpGolomb(BitB, true);
                }
                if (Dec->SPS->ChromaArrayType != ChromaBW) {
                    Dec->Slice->ChromaWeightFlag[1] = ReadBits(BitB, 1, true);
                    if (Dec->Slice->ChromaWeightFlag[1] == true) {
                        for (uint8_t J = 0; J < 2; J++) {
                            Dec->Slice->ChromaWeight[1][i][J] = ReadExpGolomb(BitB, true);
                            Dec->Slice->ChromaOffset[1][i][J] = ReadExpGolomb(BitB, true);
                        }
                    }
                }
            }
        }
    }
    
    void DecodeRefPicMarking(DecodeAVC *Dec, BitBuffer *BitB) { // dec_ref_pic_marking
        if (Dec->Slice->SliceIsIDR == true) {
            Dec->NAL->EmptyPictureBufferBeforeDisplay = ReadBits(BitB, 1, true);
            Dec->NAL->FrameIsLongTermReference        = ReadBits(BitB, 1, true);
        } else {
            Dec->NAL->AdaptiveRefPicMarkingModeFlag   = ReadBits(BitB, 1, true);
            if (Dec->NAL->AdaptiveRefPicMarkingModeFlag == true) {
                Dec->NAL->MemManControlOp = ReadExpGolomb(BitB, false);
                if ((Dec->NAL->MemManControlOp == 1) || (Dec->NAL->MemManControlOp == 3)) {
                    Dec->NAL->PicNumDiff = ReadExpGolomb(BitB, false) + 1;
                }
                if (Dec->NAL->MemManControlOp == 2) {
                    Dec->Slice->LongTermPicNum  = ReadExpGolomb(BitB, false);
                }
                if ((Dec->NAL->MemManControlOp == 3) || (Dec->NAL->MemManControlOp == 6)) {
                    Dec->NAL->LongTermFrameIndex = ReadExpGolomb(BitB, false);
                }
                if (Dec->NAL->MemManControlOp == 4) {
                    Dec->NAL->MaxLongTermFrameIndex = ReadExpGolomb(BitB, false) - 1;
                }
            }
        }
    }
    
    void Residual(DecodeAVC *Dec, BitBuffer *BitB, uint64_t StartIdx, uint64_t endIdx) {
        if (Dec->PPS->EntropyCodingMode == false) {
            residual_block = ExpGolomb; // residual_block_cavlc
        } else {
            residual_block = Arithmetic; // residual_block_cabac
        }
        ResidualLuma(i16x16DClevel, i16x16AClevel, level4x4, level8x8, startIdx, endIdx);
        Intra16x16DCLevel = i16x16DClevel;
        Intra16x16ACLevel = i16x16AClevel;
        LumaLevel4x4 = level4x4;
        LumaLevel8x8 = level8x8;
        if ((Dec->SPS->ChromaArrayType == Chroma420) || (Dec->SPS->ChromaArrayType == Chroma422)) {
            NumC8x8 = (4 / (SubWidthC * SubHeightC));
            for (uint8_t iCbCr = 0; iCbCr < 2; iCbCr++) {
                if ((Dec->MacroBlock->BlockPatternChroma & 3) && (startIdx == 0)) { /* chroma DC residual present */
                    residual_block(ChromaDCLevel[iCbCr], 0, 4 * NumC8x8 - 1, 4 * NumC8x8);
                } else {
                    for (uint8_t i = 0; i < 4 * NumC8x8; i++) {
                        ChromaDCLevel[iCbCr][i] = 0;
                    }
                }
            }
            for (uint8_t iCbCr = 0; iCbCr < 2; iCbCr++) {
                for (i8x8 = 0; i8x8 < NumC8x8; i8x8++) {
                    for (i4x4 = 0; i4x4 < 4; i4x4++) {
                        if (Dec->MacroBlock->BlockPatternChroma & 2) { /* chroma AC residual present */
                            residual_block(ChromaACLevel[iCbCr][i8x8 * 4 + i4x4], Max(0, startIdx - 1), endIdx - 1, 15);
                        } else {
                            for (int i = 0; i < 15; i++) {
                                ChromaACLevel[iCbCr][i8x8 * 4 + i4x4][i] = 0;
                            }
                        }
                    }
                }
            }
        } else if (Dec->SPS->ChromaArrayType == Chroma444) {
            ResidualLuma(i16x16DClevel, i16x16AClevel, level4x4, level8x8, startIdx, endIdx);
            CbIntra16x16DCLevel = i16x16DClevel;
            CbIntra16x16ACLevel = i16x16AClevel;
            CbLevel4x4          = level4x4;
            CbLevel8x8          = level8x8;
            ResidualLuma(i16x16DClevel, i16x16AClevel, level4x4, level8x8, startIdx, endIdx);
            CrIntra16x16DCLevel = i16x16DClevel;
            CrIntra16x16ACLevel = i16x16AClevel;
            CrLevel4x4          = level4x4;
            CrLevel8x8          = level8x8;
        }
    }
    
    void residual_block_cavlc(DecodeAVC *Dec, BitBuffer *BitB, int coeffLevel, int startIdx, int endIdx, int maxNumCoeff) { // residual_block_cavlc
        int coeffLevel[maxNumCoeff] = {0}, coeff_token, suffixLength, trailing_ones_sign_flag;
        
        coeff_token = ReadExpGolomb(BitB, false);
        if (TotalCoeff(coeff_token) > 0) {
            if ((TotalCoeff(coeff_token) > 10) && (TrailingOnes(coeff_token) < 3)) {
                suffixLength = 1;
            } else {
                suffixLength = 0;
            }
            for (int Coeff = 0; Coeff < TotalCoeff(coeff_token); Coeff++) {
                if (Coeff < TrailingOnes(coeff_token)) {
                    trailing_ones_sign_flag = ReadBits(BitB, 1, true);
                    levelVal[i] = 1 - 2 * trailing_ones_sign_flag;
                } else {
                    level_prefix = ReadExpGolomb(BitB, false);
                    levelCode = (Min(15, level_prefix) << suffixLength);
                    if ((suffixLength > 0) || (level_prefix >= 14)) {
                        level_suffix = ReadBits(BitB, 0, true);
                        levelCode += level_suffix;
                    }
                    if (level_prefix > = 15 && suffixLength == 0) {
                        levelCode += 15;
                    }
                    if (level_prefix > = 16) {
                        levelCode += (1 << (level_prefix - 3)) - 4096;
                    }
                    if (i == TrailingOnes(coeff_token) && TrailingOnes(coeff_token) < 3) {
                        levelCode += 2;
                    }
                    if (levelCode % 2 == 0) {
                        levelVal[i] = (levelCode + 2) >> 1;
                    } else {
                        levelVal[i] = (-levelCode - 1) >> 1;
                    }
                    if(suffixLength == 0) {
                        suffixLength = 1;
                    }
                    if (abs(levelVal[i]) > (3 << (suffixLength - 1)) && suffixLength < 6) {
                        suffixLength++;
                    }
                }
                if (TotalCoeff(coeff_token) < endIdx - startIdx + 1) { // FIXME: this may need to be moved
                    total_zeros = ReadExpGolomb(BitB, false);
                    zerosLeft = total_zeros;
                } else {
                    zerosLeft = 0;
                }
                for (i = 0; i < TotalCoeff(coeff_token) - 1; i++) {
                    if(zerosLeft > 0) {
                        run_before = ReadExpGolomb(BitB, false);
                        runVal[i] = run_before;
                    } else {
                        runVal[i] = 0;
                    }
                    zerosLeft = zerosLeft - runVal[i];
                }
                runVal[TotalCoeff(coeff_token) - 1] = zerosLeft;
                coeffNum = -1;
                for (i = TotalCoeff(coeff_token) - 1; i >= 0; i- -) {
                    coeffNum += runVal[i] + 1;
                    coeffLevel[startIdx + coeffNum] = levelVal[i];
                }
            }
        }
    }
    
    void ScanNALUnits(DecodeAVC *Dec, BitBuffer *BitB) {
        switch (Dec->NAL->NALUnitType) { // nal_unit_type
            case NAL_NonIDRSlice: // 1
                ParseNALSliceNonPartitioned(Dec, BitB); // slice_layer_without_partitioning_rbsp
                break;
            case NAL_SlicePartitionA: // 2
                ParseNALSlicePartitionA(Dec, BitB); // slice_data_partition_a_layer_rbsp
                break;
            case NAL_SlicePartitionB: // 3
                ParseNALSlicePartitionB(Dec, BitB); // slice_data_partition_b_layer_rbsp
                break;
            case NAL_SlicePartitionC: // 4
                ParseNALSlicePartitionC(Dec, BitB); // slice_data_partition_c_layer_rbsp
                break;
            case NAL_IDRSliceNonPartitioned: // 5
                ParseNALSliceNonPartitioned(Dec, BitB); // slice_layer_without_partitioning_rbsp
                break;
            case NAL_SupplementalEnhancementInfo: // 6
                ParseSEIMessage(Dec, BitB); // sei_rbsp
                break;
            case NAL_SequenceParameterSet: // 7
                ParseNALSequenceParameterSet(Dec, BitB); // seq_parameter_set_rbsp
                break;
            case NAL_PictureParameterSet: // 8
                ParseNALPictureParameterSet(Dec, BitB); // pic_parameter_set_rbsp
                break;
            case NAL_AccessUnitDelimiter: // 9
                ParseNALAccessUnitDelimiter(Dec, BitB); // access_unit_delimiter_rbsp()
                break;
            case NAL_EndOfSequence: // 10
                                    //ParseNALEndOfSequence(Dec, BitB);       // End of Sequence aka Rescan end_of_seq_rbsp
            case NAL_EndOfStream: // 11
                                  //ParseNALEndOfStream(Dec, BitB);        // End of Stream NAL.
            case NAL_FillerData: // 12
                ParseNALFillerData(Dec, BitB); // filler_data_rbsp
            case NAL_SequenceParameterSetExtended: // 13
                ParseNALSequenceParameterSetExtended(Dec, BitB);
                break;
            case NAL_PrefixUnit: // 14
                ParseNALPrefixUnit(Dec, BitB); // prefix_nal_unit_rbsp
                break;
            case NAL_SubsetSequenceParameterSet: // 15
                ParseNALSubsetSPS(Dec, BitB);
                break;
            case NAL_DepthParameterSet: // 16
                ParseNALDepthParameterSet(Dec, BitB);
                break;
            case NAL_AuxiliarySliceNonPartitioned: // 19
                ParseNALIDRSliceNonPartitioned(Dec, BitB);
                break;
            case NAL_AuxiliarySliceExtension: // 20
                ParseNALAuxiliarySliceExtension(Dec, BitB);
                // slice_layer_extension_rbsp
                break;
            case NAL_MVCDepthView: // 21
                ParseNALAuxiliarySliceExtension(Dec, BitB);
                // slice_layer_extension_rbsp
                break;
            default:
                Log(LOG_ERR, "libModernAVC", "ScanNALUnits", "NAL ID 0x%X is not supported, seeking to next NAL\n", Dec->NAL->NALUnitType);
                break;
        }
    }
    
    void scaling_list(BitBuffer *BitB, int scalingList, size_t sizeOfScalingList, bool useDefaultScalingMatrixFlag) { // scaling_list
        int last_scale = 8;
        int next_scale = 8;
        for (int Scale = 0; Scale < sizeOfScalingList; Scale++) {
            if (next_scale != 0) {
                delta_scale                 = ReadExpGolomb(BitB, true);
                nextScale                   = (last_scale + delta_scale + 256) % 256;
                useDefaultScalingMatrixFlag = (Scale == 0 && next_scale == 0);
            }
            scalingList[Scale] = (next_scale == 0) ? last_scale : next_scale;
            lastScale = scalingList[Scale];
        }
    }
    
    void RescanSync(BitBuffer *BitB) {
        // search for 0x000001, or 0x000003 for the next NAL.
        
    }
    
    void rbsp_trailing_bits(DecodeAVC *Dec, BitBuffer *BitB) { // rbsp_trailing_bits
        bool rbsp_stop_one_bit = 0;
        rbsp_stop_one_bit = ReadBits(BitB, 1, true);
        AlignBitBuffer(BitB, 1); // while( !byte_aligned( ) )
                             // rbsp_alignment_zero_bit
    }
    
    uint8_t GetCodedBlockPattern(DecodeAVC *Dec, uint8_t CodeNum) { // FIXME: this has to be wrong
        uint8_t Value = 0;
        if (Dec->MacroBlock->BlockPattern == 1 || Dec->MacroBlock->BlockPattern == 2) {
            if (Dec->SPS->ChromaArrayType == 1) {
                // Intra
                Value = CBPCAT12Intra[CodeNum];
            } else if (Dec->SPS->ChromaArrayType == 2) {
                // Inter
                Value = CBPCAT12Inter[CodeNum];
            }
        }
        else if (Dec->MacroBlock->BlockPattern == 0 || Dec->MacroBlock->BlockPattern == 3) {
            if (Dec->SPS->ChromaArrayType == 0) {
                // Intra
                Value = CBPCAT03Intra[CodeNum];
            } else if (Dec->SPS->ChromaArrayType == 3) {
                // Inter
                Value = CBPCAT03Inter[CodeNum];
            }
        }
        return Value;
    }
    
    void MacroBlockPartitionWidth(DecodeAVC *Dec, uint8_t MacroBlockType) { // MbPartWidth
        if (MacroBlockType == P_L0_16x16) {
            return 16;
        } else if (MacroBlockType == P_L0_L0_16x8) {
            return 16;
        } else if (MacroBlockType == P_L0_L0_8x16) {
            return 8;
        } else if (MacroBlockType == P_8x8) {
            return 8;
        } else if (MacroBlockType == P_8x8ref0) {
            return 8;
        } else if (MacroBlockType == P_Skip) {
            return 16;
        }
    }
    
    uint8_t MacroBlockPartitionHeight(DecodeAVC *Dec, uint8_t MacroBlockType) { // MbPartHeight
        if (MacroBlockType == P_L0_16x16) {
            return 16;
        } else if (MacroBlockType == P_L0_L0_16x8) {
            return 8;
        } else if (MacroBlockType == P_L0_L0_8x16) {
            return 16;
        } else if (MacroBlockType == P_8x8) {
            return 8;
        } else if (MacroBlockType == P_8x8ref0) {
            return 8;
        } else if (MacroBlockType == P_Skip) {
            return 16;
        }
    }
    
    void SubMacroBlockPartitionWidth(DecodeAVC *Dec) { // SubMbPartWidth
        
    }
    
    void SubMacroBlockPartitionHeight(DecodeAVC *Dec) { // SubMbPartHeight
        
    }
    
#ifdef __cplusplus
}
#endif
