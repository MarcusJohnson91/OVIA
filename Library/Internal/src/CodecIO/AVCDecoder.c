#include "../../include/Private/Common/libModernAVC_Types.h"
#include "../../include/Private/Common/libModernAVC_Common.h"
#include "../../include/Private/Decode/libModernAVC_Decode.h"
#include "../../include/Private/Decode/libModernAVC_ParseMacroBlock.h"
#include "../../include/Private/Decode/libModernAVC_ParseSlice.h"
#include "../../include/Private/Decode/libModernAVC_ParseNAL.h"

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
     BitIOLog(LOG_INFO, "libModernAVC", "ParseAVCFile", "Parsing AVC File...\n");
     
     // Found a start code.
     if (ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32) == AVCMagic && GetBitBufferPosition(BitB) == 0) {
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
     Dec->NAL->NALRefIDC               = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 2); // 0
     Dec->NAL->NALUnitType             = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 5); // 6
     
     int     NumBytesInRBSP = 0;
     int nalUnitHeaderBytes = 1;
     
     if ((Dec->NAL->NALUnitType == NAL_PrefixUnit) || (Dec->NAL->NALUnitType == NAL_AuxiliarySliceExtension) ||(Dec->NAL->NALUnitType == NAL_MVCDepthView) ) {
     if (Dec->NAL->NALUnitType == NAL_MVCDepthView) {
     Dec->NAL->AVC3DExtensionFlag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
     } else {
     Dec->NAL->SVCExtensionFlag   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
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
     uint32_t NALID = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 24);
     if (NALID == 0x000003) {
     uint8_t NALID2 = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
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
     
     uint32_t ByteAlignment = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32);
     if (ByteAlignment == 0x00000001) { // Bytestream is aligned
     uint32_t StreamType = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 24);
     if (StreamType == 0x000001) { // Start code prefix
     
     } else if (StreamType == 0x000003) { // emulation_prevention_three_byte
     
     }
     
     if ((ReadBits(BitIOMSByte, BitIOLSBit, BitB, 24) == 0x000000) && (IsStreamByteAligned(BitB->BitsUnavailable, 1) == true)) {
     
     
     }
     }
     }
     */
    
    DecodeAVC *InitAVCDecoder(void) {
        DecodeAVC *Dec                = (DecodeAVC*) calloc(1, sizeof(DecodeAVC));
        
        if (Dec == NULL) {
            BitIOLog(LOG_ERROR, "libModernAVC", "InitAVCDecoder", "Pointer to DecodeAVC is NULL");
        } else {
            Dec->NAL                  = (NALHeader*)                    calloc(1, sizeof(NALHeader));
            Dec->SPS                  = (SequenceParameterSet*)         calloc(1, sizeof(SequenceParameterSet));
            Dec->PPS                  = (PictureParameterSet*)          calloc(1, sizeof(PictureParameterSet));
            Dec->VUI                  = (VideoUsabilityInformation*)    calloc(1, sizeof(VideoUsabilityInformation));
            Dec->HRD                  = (HypotheticalReferenceDecoder*) calloc(1, sizeof(HypotheticalReferenceDecoder));
            Dec->SEI                  = (SupplementalEnhancementInfo*)  calloc(1, sizeof(SupplementalEnhancementInfo));
            Dec->Slice                = (Slice*)                        calloc(1, sizeof(Slice));
            Dec->SVC                  = (ScalableVideoCoding*)          calloc(1, sizeof(ScalableVideoCoding));
            Dec->DPS                  = (DepthParameterSet*)            calloc(1, sizeof(DepthParameterSet));
            Dec->MacroBlock           = (MacroBlock*)                   calloc(1, sizeof(MacroBlock));
        }
        return Dec;
    }
    
    bool     AreAllViewsPaired(DecodeAVC *Dec) {
        bool AllViewsPairedFlag = false;
        for (uint8_t View = 0; View < Dec->SPS->ViewCount; View++) {
            AllViewsPairedFlag = (1 && Dec->SPS->DepthViewPresent[View] && Dec->SPS->TextureViewPresent[View]);
        }
        return AllViewsPairedFlag;
    }
    
    void ParseTransformCoeffs(DecodeAVC *Dec, uint8_t i16x16DC, uint8_t i16x16AC, uint8_t Level4x4, uint8_t Level8x8, uint8_t StartIndex, uint8_t EndIndex) { // residual_block
        uint8_t Intra16x16DCLevel = i16x16DC, Intra16x16ACLevel = i16x16AC, LumaLevel4x4 = Level4x4, LumaLevel8x8 = Level8x8;
        // Return the first 4 variables
        if (Dec->PPS->EntropyCodingMode == ExpGolomb) {
            // Read ExpGolomb
        } else {
            // Read Arithmetic
        }
        
        if (Dec->SPS->ChromaArrayType == Chroma420 || Dec->SPS->ChromaArrayType == Chroma422) {
            for (uint8_t iCbCr = 0; iCbCr < 1; iCbCr++) {
                ChromaDCLevel[iCbCr] = 4 * NumC8x8;
                for (uint8_t ChromaBlock = 0; ChromaBlock < i4x4; ChromaBlock++) {
                    ChromaACLevel[iCbCr][(i8x8 * 4) + i4x4] = 0;
                }
            }
        } else if (Dec->SPS->ChromaArrayType == Chroma444) {
            
        }
    }
    
    void ResidualLuma(DecodeAVC *Dec, BitBuffer *BitB, int i16x16DClevel, int i16x16AClevel, int level4x4,
                      int level8x8, int startIdx, int endIdx) { // residual_luma
        if (startIdx == 0 && MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) == Intra_16x16) {
            residual_block(i16x16DClevel, 0, 15, 16);
        }
        for (uint8_t i8x8 = 0; i8x8 < 4; i8x8++) {
            if (Dec->MacroBlock->TransformSizeIs8x8 == false || Dec->PPS->EntropyCodingMode == ExpGolomb) {
                for (uint8_t i4x4 = 0; i4x4 < 4; i4x4++) {
                    if (Dec->MacroBlock->BlockPatternLuma & (1 << i8x8)) {
                        if (MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) == Intra_16x16) {
                            residual_block(i16x16AClevel[i8x8 * 4 + i4x4], Max(0, startIdx - 1), endIdx - 1, 15);
                        } else {
                            residual_block(level4x4[i8x8 * 4 + i4x4], startIdx, endIdx, 16);
                        }
                    } else if (MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) == Intra_16x16) {
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
            
            if (Dec->PPS->EntropyCodingMode == ExpGolomb && Dec->MacroBlock->TransformSizeIs8x8) {
                for (uint8_t i = 0; i < 16; i++) {
                    level8x8[i8x8][4 * i + i4x4] = level4x4[i8x8 * 4 + i4x4][i];
                }
            }
            else if (Dec->MacroBlock->BlockPatternLuma & (1 << i8x8)) {
                residual_block(level8x8[i8x8], 4 * startIdx, 4 * endIdx + 3, 64);
            } else {
                for (uint8_t i = 0; i < 64; i++) {
                    level8x8[i8x8][i] = 0;
                }
            }
        }
    }
    
    int8_t MacroBlock2SliceGroupMap(DecodeAVC *Dec, uint8_t CurrentMacroBlock) { // MbToSliceGroupMap
        if (Dec->PPS->SliceGroups == 1 && (Dec->PPS->SliceGroupMapType == 3 || Dec->PPS->SliceGroupMapType == 4 || Dec->PPS->SliceGroupMapType == 5)) {
            if (Dec->PPS->SliceGroupMapType == 3) {
                if (Dec->PPS->SliceGroupChangeDirection == false) {
                    return BoxOutClockwise;
                } else if (Dec->PPS->SliceGroupChangeDirection == true) {
                    return BoxOutCounterClockwise;
                }
            } else if (Dec->PPS->SliceGroupMapType == 4) {
                if (Dec->PPS->SliceGroupChangeDirection == false) {
                    return RasterScan;
                } else if (Dec->PPS->SliceGroupChangeDirection == true) {
                    return ReverseRasterScan;
                }
            } else if (Dec->PPS->SliceGroupMapType == 5) {
                if (Dec->PPS->SliceGroupChangeDirection == false) {
                    return WipeRight;
                } else if (Dec->PPS->SliceGroupChangeDirection == true) {
                    return WipeLeft;
                }
            }
        }
        BitIOLog(LOG_ERROR, "libModernAVC", "MacroBlock2SliceGroupMap", "SliceGroups: %hhu, SliceGroupMapType: %d", Dec->PPS->SliceGroups, Dec->PPS->SliceGroupMapType);
        return -1; // failure
    }
    
    void rbsp_slice_trailing_bits(DecodeAVC *Dec, BitBuffer *BitB) {
        AlignBitBuffer(BitB, 1); // rbsp_trailing_bits();
        if (Dec->PPS->EntropyCodingMode == Arithmetic) {
            while (more_rbsp_trailing_data()) {
                uint64_t CABACZeroWord = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16); /* equal to 0x0000 */
            }
        }
    }
    
    uint8_t MacroBlockPartitionPredictionMode(DecodeAVC *Dec, uint8_t MacroBlockType, uint8_t PartitionNumber) {  // MbPartPredMode
        uint8_t ReturnValue = 0;
        if (MacroBlockType == 0) {
            if (Dec->MacroBlock->TransformSizeIs8x8 == true) {
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
    
    uint64_t NextMacroBlockAddress(DecodeAVC *Dec, uint64_t CurrentMacroBlockAddress) { // NextMbAddress
        while (CurrentMacroBlockAddress + 1 < Dec->Slice->PicSizeInMacroBlocks && MacroBlock2SliceGroupMap(Dec, CurrentMacroBlockAddress + 1) != MacroBlock2SliceGroupMap(Dec, CurrentMacroBlockAddress)) {
            i++; nextMbAddress = I
        }
        
        // aka
        for (uint64_t I = CurrentMacroBlockAddress + 1; I < Dec->Slice->PicSizeInMacroBlocks && MbToSliceGroups[I]) {
            MacroBlock2SliceGroupMap(Dec, CurrentMacroBlockAddress);
        }
    }
    
    uint8_t CalculateNumberOfTimeStamps(DecodeAVC *Dec) { // PicOrderCount
        uint8_t NumTimeStamps = 0;
        if ((Dec->Slice->SliceIsInterlaced == false) && (Dec->Slice->TopFieldOrderCount == Dec->Slice->BottomFieldOrderCount)) {
            NumTimeStamps = 1;
        } else if (0 == 1) {
            
        }
        return 0;
    }
    
    bool InCropWindow(DecodeAVC *Dec, uint64_t MacroBlockAddress) { // mbAddr
        uint64_t mbX     = ((MacroBlockAddress / (1 + Dec->Slice->MbaffFrameFlag)) % Dec->Slice->PicWidthInMacroBlocks);
        uint64_t mbY0    = 0;
        uint64_t mbY1    = 0;
        uint8_t  scalMbH  = 0; // Max = 32
        bool     Status   = 0;
        
        Dec->Slice->ScaledRefLayerPicHeightInSamplesL = Dec->Slice->PicHeightInMacroBlocks * 16 - (Dec->Slice->ScaledRefLayerTopOffset + Dec->Slice->ScaledRefLayerBottomOffset) / (1 + Dec->Slice->SliceIsInterlaced);
        
        if (Dec->Slice->MbaffFrameFlag == false) {
            mbY0 = (MacroBlockAddress / Dec->Slice->PicWidthInMacroBlocks);
            mbY1 = (MacroBlockAddress / Dec->Slice->PicWidthInMacroBlocks);
        } else if (Dec->Slice->MbaffFrameFlag == true) {
            mbY0 = (2 * ((MacroBlockAddress / Dec->Slice->PicWidthInMacroBlocks) / 2));
            mbY1 = mbY0 + 1;
        }
        scalMbH = (16 * (1 + Dec->Slice->SliceIsInterlaced));
        
        if (Dec->NAL->NoInterLayerPredictionFlag == true && (mbX >= ((Dec->Slice->ScaledRefLayerLeftOffset + 15) / 16) && mbX < ((Dec->Slice->ScaledRefLayerLeftOffset + ScaledRefLayerPicWidthInSamplesL) / 16)) && mbY0 >= ((Dec->Slice->ScaledRefLayerTopOffset + scalMbH - 1) / scalMbH) && mbY1 < ((Dec->Slice->ScaledRefLayerTopOffset + Dec->Slice->ScaledRefLayerPicHeightInSamplesL) / scalMbH)) {
            Status = true;
        } else {
            Status = false;
        }
        
        return Status;
    }
    
    uint64_t ScalingList(DecodeAVC *Dec, BitBuffer *BitB, uint8_t *scalingList, size_t ScalingListSize, bool UseDefaultScalingMatrixFlag) { // scaling_list
        uint8_t LastScale  = 8;
        uint8_t NextScale  = 8;
        uint8_t DeltaScale = 0;
        
        uint8_t ScalingList[ScalingListSize];
        
        for (uint64_t J = 0; J < ScalingListSize; J++) {
            if (NextScale != 0) {
                DeltaScale = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, true);
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
            element_equal_flag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
        }
        if (element_equal_flag == false) {
            numValues = numViews - DeltaFlag;
        } else {
            numValues = 1;
        }
        for (uint8_t Value = 0; Value < numValues; Value++) {
            if ((PredDirection == 2) && (Value = 0)) {
                if (precMode == 0) {
                    matissa_len_minus1 = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 5) + 1;
                    OutManLen[index, Value] = manLen = matissa_len_minus1;
                } else {
                    prec = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 5);
                }
            }
            if (PredDirection == 2) {
                sign0 = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                OutSign[index, Value] = sign0;
                exponent0 = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                
                OutExp[index, Value] = exponent0;
                if (precMode == 1) {
                    if (exponent0 == 0) {
                        OutManLen[index, Value] = manLen = Max(0, prec - 30);
                    } else {
                        OutManLen[index, Value] = manLen = Max(0, exponent0 + prec - 31);
                    }
                }
                mantissa0 = ReadBits(BitIOMSByte, BitIOLSBit, BitB, manLen);
                OutMantissa[index, Value] = mantissa0;
            } else {
                skip_flag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                if (skip_flag == 0) {
                    sign1 = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                    OutSign[index, Value] = sign1;
                    exponent_skip_flag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                    if (exponent_skip_flag == 0) {
                        exponent1 = ReadBits(BitIOMSByte, BitIOLSBit, BitB, expLen);
                        OutExp[index, Value] = exponent1;
                    } else {
                        OutExp[index, Value] = OutExp[Dec->DPS->ReferenceDPSID[1], Value];
                    }
                    mantissa_diff = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, true);
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
        bool NearFlag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
        bool FarFlag  = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
        
        if (NearFlag == true) {
            AcquisitionElement3DV(NumberOfViews, 0, PredDirection, 7, 0, ZNearSign, ZNearExp, ZNearMantissa, ZNearManLen)
        } else if (FarFlag == true) {
            AcquisitionElement3DV(NumberOfViews, 0, PredDirection, 7, 0, ZFarSign, ZFarExp, ZFarMantissa, ZFarManLen);
        }
        
        return 0;
    }
    
    void vsp_param(DecodeAVC *Dec, BitBuffer *BitB, uint8_t numViews, uint8_t Direction, uint8_t DepthPS) { // vsp_param
        uint64_t disparity_diff_wji[numViews][numViews];
        uint64_t disparity_diff_oji[numViews][numViews];
        uint64_t disparity_diff_wij[numViews][numViews];
        uint64_t disparity_diff_oij[numViews][numViews];
        
        for (uint8_t View = 0; View < numViews; View++) { // Param
            for (uint8_t J = 0; J < View; J++) { // Wat
                disparity_diff_wji[J][View] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                disparity_diff_oji[J][View] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                disparity_diff_wij[View][J] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                disparity_diff_oij[View][J] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                if (Direction == 2) { /* Not 100% sure about the if loop part, but it makes more sense than for alone */
                    Dec->DisparityScale[DepthPS][J][View] = disparity_diff_wji[J][View];
                    Dec->DisparityOffset[DepthPS][J][View] = disparity_diff_oji[J][View];
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
            Dec->Slice->RefPicListModFlag[0] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->Slice->RefPicListModFlag[0] == true) {
                Dec->Slice->ModPicNumsIDC = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                if ((Dec->Slice->ModPicNumsIDC == 0) || (Dec->Slice->ModPicNumsIDC == 1)) {
                    Dec->Slice->AbsDiffPicNum = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, true) + 1;
                } else if (Dec->Slice->ModPicNumsIDC == 2) {
                    Dec->Slice->LongTermPicNum = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                } else if ((Dec->Slice->ModPicNumsIDC == 4) || (Dec->Slice->ModPicNumsIDC == 5)) {
                    Dec->Slice->AbsDiffViewIdx = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false) + 1;
                }
            }
        }
        if ((Dec->Slice->Type % 5) == 1) {
            Dec->Slice->RefPicListModFlag[1] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->Slice->RefPicListModFlag[1] == true) {
                Dec->Slice->ModPicNumsIDC = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                if ((Dec->Slice->ModPicNumsIDC == 0) || (Dec->Slice->ModPicNumsIDC == 1)) {
                    Dec->Slice->AbsDiffPicNum = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, true) + 1;
                } else if (Dec->Slice->ModPicNumsIDC == 2) {
                    Dec->Slice->LongTermPicNum = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                } else if ((Dec->Slice->ModPicNumsIDC == 4) || (Dec->Slice->ModPicNumsIDC == 5)) {
                    Dec->Slice->AbsDiffViewIdx = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false) + 1;
                }
            }
        }
    }
    
    void RefPicListMod(DecodeAVC *Dec, BitBuffer *BitB) { // ref_pic_list_modification
        if (((Dec->Slice->Type % 5) != 2) && ((Dec->Slice->Type % 5) != 4)) {
            Dec->Slice->RefPicListModFlag[0] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->Slice->RefPicListModFlag[0] == true) {
                Dec->Slice->ModPicNumsIDC = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                if ((Dec->Slice->ModPicNumsIDC == 0) || (Dec->Slice->ModPicNumsIDC == 1)) {
                    Dec->Slice->AbsDiffPicNum = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, true) + 1;
                } else if (Dec->Slice->ModPicNumsIDC == 2) {
                    Dec->Slice->LongTermPicNum = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                } else if ((Dec->Slice->ModPicNumsIDC == 4) || (Dec->Slice->ModPicNumsIDC == 5)) {
                    Dec->Slice->AbsDiffViewIdx = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false) + 1;
                }
            }
        }
        if ((Dec->Slice->Type % 5) == 1) {
            Dec->Slice->RefPicListModFlag[1] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->Slice->RefPicListModFlag[1] == true) {
                Dec->Slice->ModPicNumsIDC = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                if ((Dec->Slice->ModPicNumsIDC == 0) || (Dec->Slice->ModPicNumsIDC == 1)) {
                    Dec->Slice->AbsDiffPicNum = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, true) + 1;
                } else if (Dec->Slice->ModPicNumsIDC == 2) {
                    Dec->Slice->LongTermPicNum = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                } else if ((Dec->Slice->ModPicNumsIDC == 4) || (Dec->Slice->ModPicNumsIDC == 5)) {
                    Dec->Slice->AbsDiffViewIdx = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false) + 1;
                }
            }
        }
    }
    
    void pred_weight_table(DecodeAVC *Dec, BitBuffer *BitB) { // pred_weight_table
        Dec->Slice->LumaWeightDenom = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, true);
        if (Dec->SPS->ChromaArrayType != ChromaBW) {
            Dec->Slice->ChromaWeightDenom = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
        }
        for (uint8_t i = 0; i <= Dec->MacroBlock->NumRefIndexActiveLevel0; i++) {
            Dec->Slice->LumaWeightFlag[0] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->Slice->LumaWeightFlag[0] == true) {
                Dec->Slice->LumaWeight[0][i]  = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, true);
                Dec->Slice->LumaOffset[0][i]  = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, true);
            }
            if (Dec->SPS->ChromaArrayType != ChromaBW) {
                Dec->Slice->ChromaWeightFlag[0] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                if (Dec->Slice->ChromaWeightFlag[0] == true) {
                    for (int J = 0; J < 2; J++) {
                        Dec->Slice->ChromaWeight[0][i][J] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, true);
                        Dec->Slice->ChromaOffset[0][i][J] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, true);
                    }
                }
            }
        }
        if ((Dec->Slice->Type % 5) == 1) {
            for (uint8_t i = 0; i <= Dec->MacroBlock->NumRefIndexActiveLevel1; i++) {
                Dec->Slice->LumaWeightFlag[1] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                if (Dec->Slice->LumaWeightFlag[1] == true) {
                    Dec->Slice->LumaWeight[1][i]  = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, true);
                    Dec->Slice->LumaOffset[1][i]  = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, true);
                }
                if (Dec->SPS->ChromaArrayType != ChromaBW) {
                    Dec->Slice->ChromaWeightFlag[1] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                    if (Dec->Slice->ChromaWeightFlag[1] == true) {
                        for (uint8_t J = 0; J < 2; J++) {
                            Dec->Slice->ChromaWeight[1][i][J] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, true);
                            Dec->Slice->ChromaOffset[1][i][J] = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, true);
                        }
                    }
                }
            }
        }
    }
    
    void DecodeRefPicMarking(DecodeAVC *Dec, BitBuffer *BitB) { // dec_ref_pic_marking
        if (Dec->Slice->SliceIsIDR == true) {
            Dec->NAL->EmptyPictureBufferBeforeDisplay = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            Dec->NAL->FrameIsLongTermReference        = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
        } else {
            Dec->NAL->AdaptiveRefPicMarkingModeFlag   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
            if (Dec->NAL->AdaptiveRefPicMarkingModeFlag == true) {
                Dec->NAL->MemManControlOp = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                if ((Dec->NAL->MemManControlOp == 1) || (Dec->NAL->MemManControlOp == 3)) {
                    Dec->NAL->PicNumDiff = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false) + 1;
                }
                if (Dec->NAL->MemManControlOp == 2) {
                    Dec->Slice->LongTermPicNum  = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                }
                if ((Dec->NAL->MemManControlOp == 3) || (Dec->NAL->MemManControlOp == 6)) {
                    Dec->NAL->LongTermFrameIndex = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                }
                if (Dec->NAL->MemManControlOp == 4) {
                    Dec->NAL->MaxLongTermFrameIndex = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false) - 1;
                }
            }
        }
    }
    
    void Residual(DecodeAVC *Dec, BitBuffer *BitB, uint64_t StartIdx, uint64_t endIdx) {
        if (Dec->PPS->EntropyCodingMode == false) {
            residual_block = ExpGolomb; // ParseResidualTransformCoefficentExpGolomb
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
    
    void ParseResidualTransformCoefficentExpGolomb(DecodeAVC *Dec, BitBuffer *BitB, uint64_t CoefficentLevel, uint64_t StartIndex, uint64_t EndIndex, uint64_t MaxCoefficents) { // residual_block_cavlc
                                                                                                                                                                                 // int coeffLevel, int startIdx, int endIdx, int maxNumCoeff
        
        uint64_t coeff_token = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
        uint8_t  suffixLength;
        uint8_t  trailing_ones_sign_flag;
        uint64_t levelVal[MaxCoefficents];
        uint64_t level_prefix;
        uint64_t level_suffix;
        uint64_t levelCode;
        uint64_t zerosLeft;
        uint64_t total_zeros;
        uint64_t run_before;
        uint64_t runVal[MaxCoefficents];
        uint64_t coeffNum;
        uint64_t coeffLevel[MaxCoefficents];
        
        if (TotalCoeff(coeff_token) > 0) {
            if ((TotalCoeff(coeff_token) > 10) && (TrailingOnes(coeff_token) < 3)) {
                suffixLength = 1;
            } else {
                suffixLength = 0;
            }
            for (uint64_t Coefficent = 0; Coefficent < TotalCoeff(coeff_token); Coefficent++) {
                if (Coefficent < TrailingOnes(coeff_token)) {
                    trailing_ones_sign_flag = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
                    levelVal[Coefficent] = 1 - 2 * trailing_ones_sign_flag;
                } else {
                    level_prefix = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    levelCode = (Min(15, level_prefix) << suffixLength);
                    if ((suffixLength > 0) || (level_prefix >= 14)) {
                        level_suffix = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 0);
                        levelCode += level_suffix;
                    }
                    if (level_prefix >= 15 && suffixLength == 0) {
                        levelCode += 15;
                    }
                    if (level_prefix >= 16) {
                        levelCode += (1 << (level_prefix - 3)) - 4096;
                    }
                    if (Coefficent == TrailingOnes(coeff_token) && TrailingOnes(coeff_token) < 3) {
                        levelCode += 2;
                    }
                    if (levelCode % 2 == 0) {
                        levelVal[Coefficent] = (levelCode + 2) >> 1;
                    } else {
                        levelVal[Coefficent] = (-levelCode - 1) >> 1;
                    }
                    if(suffixLength == 0) {
                        suffixLength = 1;
                    }
                    if (abs(levelVal[Coefficent]) > (3 << (suffixLength - 1)) && suffixLength < 6) {
                        suffixLength++;
                    }
                }
                if (TotalCoeff(coeff_token) < EndIndex - StartIndex + 1) { // FIXME: this may need to be moved
                    total_zeros = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                    zerosLeft = total_zeros;
                } else {
                    zerosLeft = 0;
                }
                for (uint64_t i = 0; i < TotalCoeff(coeff_token) - 1; i++) {
                    if (zerosLeft > 0) {
                        run_before = ReadExpGolomb(BitIOMSByte, BitIOLSBit, BitB, false);
                        runVal[i] = run_before;
                    } else {
                        runVal[i] = 0;
                    }
                    zerosLeft = zerosLeft - runVal[i];
                }
                runVal[TotalCoeff(coeff_token) - 1] = zerosLeft;
                coeffNum = -1;
                for (uint64_t i = TotalCoeff(coeff_token) - 1; i >= 0; i--) {
                    coeffNum += runVal[i] + 1;
                    coeffLevel[StartIndex + coeffNum] = levelVal[i];
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
                BitIOLog(LOG_ERROR, "libModernAVC", "ScanNALUnits", "NAL ID 0x%X is not supported, seeking to next NAL\n", Dec->NAL->NALUnitType);
                break;
        }
    }
    
    void RescanSync(BitBuffer *BitB) {
        // search for 0x000001, or 0x000003 for the next NAL.
        
    }
    
    void rbsp_trailing_bits(DecodeAVC *Dec, BitBuffer *BitB) { // rbsp_trailing_bits
        bool rbsp_stop_one_bit = 0;
        rbsp_stop_one_bit = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 1);
        AlignBitBuffer(BitB, 1); // while( !byte_aligned( ) )
                                 // rbsp_alignment_zero_bit
    }
    
    uint8_t GetCodedBlockPattern(DecodeAVC *Dec, uint8_t CodeNum) {
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
    
    uint8_t MacroBlockPartitionWidth(DecodeAVC *Dec, uint8_t MacroBlockType) { // MbPartWidth
        uint8_t MacroBlockPartitionWidth = 0;
        if (MacroBlockType == P_L0_16x16) {
            MacroBlockPartitionWidth = 16;
        } else if (MacroBlockType == P_L0_L0_16x8) {
            MacroBlockPartitionWidth = 16;
        } else if (MacroBlockType == P_L0_L0_8x16) {
            MacroBlockPartitionWidth = 8;
        } else if (MacroBlockType == P_8x8) {
            MacroBlockPartitionWidth = 8;
        } else if (MacroBlockType == P_8x8ref0) {
            MacroBlockPartitionWidth = 8;
        } else if (MacroBlockType == P_Skip) {
            MacroBlockPartitionWidth = 16;
        }
        return MacroBlockPartitionWidth;
    }
    
    uint8_t SubMacroBlockPartitionWidth(DecodeAVC *Dec) { // SubMbPartWidth
        
    }
    
    uint8_t MacroBlockPartitionHeight(DecodeAVC *Dec, uint8_t MacroBlockType) { // MbPartHeight
        uint8_t MacroBlockPartitionHeight = 0;
        if (MacroBlockType == P_L0_16x16) {
            MacroBlockPartitionHeight = 16;
        } else if (MacroBlockType == P_L0_L0_16x8) {
            MacroBlockPartitionHeight = 8;
        } else if (MacroBlockType == P_L0_L0_8x16) {
            MacroBlockPartitionHeight = 16;
        } else if (MacroBlockType == P_8x8) {
            MacroBlockPartitionHeight = 8;
        } else if (MacroBlockType == P_8x8ref0) {
            MacroBlockPartitionHeight = 8;
        } else if (MacroBlockType == P_Skip) {
            MacroBlockPartitionHeight = 16;
        }
        return MacroBlockPartitionHeight;
    }
    
    void SubMacroBlockPartitionHeight(DecodeAVC *Dec) { // SubMbPartHeight
        
    }
    
#ifdef __cplusplus
}
#endif
