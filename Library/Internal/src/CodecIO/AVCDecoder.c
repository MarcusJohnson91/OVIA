#include "../../include/CodecIO/AVCCodec.h"

#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/TextIO/LogIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
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
     Marker              = BitBuffer_PeekBits(BitB, 24, true);
     
     if ((NotEOF == true && StreamIsByteAligned == true && ((Marker == 0x000000) || (Marker == 0x000001)))) {
     // Found the end of the NAL, record the size
     // This is gonna thrash the shit outta the buffer tho...
     EndOfBufferPosition = ftell(BitB->File) + Bits2Bytes(BitB->BitsUnavailable, true);
     NALSize = EndOfBufferPosition - StartBufferPosition;
     //UpdateInputBuffer(BitB, -NALSize); // Go back to the start of the NAL in order to read it.
     } else {
     BitBuffer_Seek(BitB, 24);
     }
     return NALSize;
     }
     
     void ParseAVCFile(DecodeAVC *Dec, BitBuffer *BitB) { // byte_stream_nal_unit
     Log(Severity_USER, "ParseAVCFile", "Parsing AVC File...\n");
     
     // Found a start code.
     if (BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 32) == AVCMagic && GetBitBufferPosition(BitB) == 0) {
     while (GetBitBufferPosition(BitB) + Bits2Bytes(BitB->BitsUnavailable, true) < BitB->FileSize) {
     ParseAVCHeader(Dec, BitB);
     FindNALSize(Dec, BitB);
     
     ScanNALUnits(Dec, BitB);
     }
     } else {
     BitBuffer_Seek(BitB, 8);
     }
     Dec->CurrentFilePosition = GetBitBufferPosition(BitB) + Bits2Bytes(BitB->BitsUnavailable, false);
     }
     
     void ParseAVCHeader(DecodeAVC *Dec, BitBuffer *BitB) { // nal_unit
     size_t BytesInNALUnit                 = 1; // nalUnitHeaderBytes
     if (BitBuffer_PeekBits(BitB, 1, true) == 0) {
     BitBuffer_Seek(BitB, 1);
     Dec->NAL->NALRefIDC               = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 2); // 0
     Dec->NAL->NALUnitType             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 5); // 6
     
     int     NumBytesInRBSP = 0;
     int nalUnitHeaderBytes = 1;
     
     if ((Dec->NAL->NALUnitType == NAL_PrefixUnit) || (Dec->NAL->NALUnitType == NAL_AuxiliarySliceExtension) ||(Dec->NAL->NALUnitType == NAL_MVCDepthView) ) {
     if (Dec->NAL->NALUnitType == NAL_MVCDepthView) {
     Dec->NAL->AVC3DExtensionFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
     } else {
     Dec->NAL->SVCExtensionFlag   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
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
     uint32_t NALID = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 24);
     if (NALID == 0x000003) {
     uint8_t NALID2 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 8);
     if ((NALID2 == 0x00) || (NALID2 == 0x01) || (NALID2 == 0x02) || (NALID2 == 0x03)) {
     // these guys are ok
     } else {
     // the official way
     
     
     // My way
     BitBuffer_Seek(BitB, 16);
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
     
     uint32_t ByteAlignment = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 32);
     if (ByteAlignment == 0x00000001) { // Bytestream is aligned
     uint32_t StreamType = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 24);
     if (StreamType == 0x000001) { // Start code prefix
     
     } else if (StreamType == 0x000003) { // emulation_prevention_three_byte
     
     }
     
     if ((BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 24) == 0x000000) && (IsStreamByteAligned(BitB->BitsUnavailable, 1) == true)) {
     
     
     }
     }
     }
     */
    
    DecodeAVC *InitAVCDecoder(void) {
        DecodeAVC *Dec                = (DecodeAVC*) calloc(1, sizeof(DecodeAVC));
        
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
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
        Log(Severity_DEBUG, PlatformIO_FunctionName, u8"SliceGroups: %hhu, SliceGroupMapType: %d", Dec->PPS->SliceGroups, Dec->PPS->SliceGroupMapType);
        return -1; // failure
    }
    
    void rbsp_slice_trailing_bits(DecodeAVC *Dec, BitBuffer *BitB) {
        BitBuffer_Align(BitB, 1); // rbsp_trailing_bits();
        if (Dec->PPS->EntropyCodingMode == Arithmetic) {
            while (more_rbsp_trailing_data()) {
                uint64_t CABACZeroWord = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16); /* equal to 0x0000 */
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
                DeltaScale = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
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
            element_equal_flag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
        }
        if (element_equal_flag == false) {
            numValues = numViews - DeltaFlag;
        } else {
            numValues = 1;
        }
        for (uint8_t Value = 0; Value < numValues; Value++) {
            if ((PredDirection == 2) && (Value = 0)) {
                if (precMode == 0) {
                    matissa_len_minus1 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 5) + 1;
                    OutManLen[index, Value] = manLen = matissa_len_minus1;
                } else {
                    prec = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 5);
                }
            }
            if (PredDirection == 2) {
                sign0 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                OutSign[index, Value] = sign0;
                exponent0 = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                
                OutExp[index, Value] = exponent0;
                if (precMode == 1) {
                    if (exponent0 == 0) {
                        OutManLen[index, Value] = manLen = Max(0, prec - 30);
                    } else {
                        OutManLen[index, Value] = manLen = Max(0, exponent0 + prec - 31);
                    }
                }
                mantissa0 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, manLen);
                OutMantissa[index, Value] = mantissa0;
            } else {
                skip_flag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (skip_flag == 0) {
                    sign1 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                    OutSign[index, Value] = sign1;
                    exponent_skip_flag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                    if (exponent_skip_flag == 0) {
                        exponent1 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, expLen);
                        OutExp[index, Value] = exponent1;
                    } else {
                        OutExp[index, Value] = OutExp[Dec->DPS->ReferenceDPSID[1], Value];
                    }
                    mantissa_diff = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
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
        bool NearFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
        bool FarFlag  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
        
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
                disparity_diff_wji[J][View] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                disparity_diff_oji[J][View] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                disparity_diff_wij[View][J] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                disparity_diff_oij[View][J] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
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
            Dec->Slice->RefPicListModFlag[0] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->Slice->RefPicListModFlag[0] == true) {
                Dec->Slice->ModPicNumsIDC = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                if ((Dec->Slice->ModPicNumsIDC == 0) || (Dec->Slice->ModPicNumsIDC == 1)) {
                    Dec->Slice->AbsDiffPicNum = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true) + 1;
                } else if (Dec->Slice->ModPicNumsIDC == 2) {
                    Dec->Slice->LongTermPicNum = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                } else if ((Dec->Slice->ModPicNumsIDC == 4) || (Dec->Slice->ModPicNumsIDC == 5)) {
                    Dec->Slice->AbsDiffViewIdx = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                }
            }
        }
        if ((Dec->Slice->Type % 5) == 1) {
            Dec->Slice->RefPicListModFlag[1] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->Slice->RefPicListModFlag[1] == true) {
                Dec->Slice->ModPicNumsIDC = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                if ((Dec->Slice->ModPicNumsIDC == 0) || (Dec->Slice->ModPicNumsIDC == 1)) {
                    Dec->Slice->AbsDiffPicNum = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true) + 1;
                } else if (Dec->Slice->ModPicNumsIDC == 2) {
                    Dec->Slice->LongTermPicNum = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                } else if ((Dec->Slice->ModPicNumsIDC == 4) || (Dec->Slice->ModPicNumsIDC == 5)) {
                    Dec->Slice->AbsDiffViewIdx = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                }
            }
        }
    }
    
    void RefPicListMod(DecodeAVC *Dec, BitBuffer *BitB) { // ref_pic_list_modification
        if (((Dec->Slice->Type % 5) != 2) && ((Dec->Slice->Type % 5) != 4)) {
            Dec->Slice->RefPicListModFlag[0] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->Slice->RefPicListModFlag[0] == true) {
                Dec->Slice->ModPicNumsIDC = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                if ((Dec->Slice->ModPicNumsIDC == 0) || (Dec->Slice->ModPicNumsIDC == 1)) {
                    Dec->Slice->AbsDiffPicNum = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true) + 1;
                } else if (Dec->Slice->ModPicNumsIDC == 2) {
                    Dec->Slice->LongTermPicNum = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                } else if ((Dec->Slice->ModPicNumsIDC == 4) || (Dec->Slice->ModPicNumsIDC == 5)) {
                    Dec->Slice->AbsDiffViewIdx = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                }
            }
        }
        if ((Dec->Slice->Type % 5) == 1) {
            Dec->Slice->RefPicListModFlag[1] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->Slice->RefPicListModFlag[1] == true) {
                Dec->Slice->ModPicNumsIDC = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                if ((Dec->Slice->ModPicNumsIDC == 0) || (Dec->Slice->ModPicNumsIDC == 1)) {
                    Dec->Slice->AbsDiffPicNum = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true) + 1;
                } else if (Dec->Slice->ModPicNumsIDC == 2) {
                    Dec->Slice->LongTermPicNum = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                } else if ((Dec->Slice->ModPicNumsIDC == 4) || (Dec->Slice->ModPicNumsIDC == 5)) {
                    Dec->Slice->AbsDiffViewIdx = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                }
            }
        }
    }
    
    void pred_weight_table(DecodeAVC *Dec, BitBuffer *BitB) { // pred_weight_table
        Dec->Slice->LumaWeightDenom = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
        if (Dec->SPS->ChromaArrayType != ChromaBW) {
            Dec->Slice->ChromaWeightDenom = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
        }
        for (uint8_t i = 0; i <= Dec->MacroBlock->NumRefIndexActiveLevel0; i++) {
            Dec->Slice->LumaWeightFlag[0] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->Slice->LumaWeightFlag[0] == true) {
                Dec->Slice->LumaWeight[0][i]  = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                Dec->Slice->LumaOffset[0][i]  = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
            }
            if (Dec->SPS->ChromaArrayType != ChromaBW) {
                Dec->Slice->ChromaWeightFlag[0] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (Dec->Slice->ChromaWeightFlag[0] == true) {
                    for (int J = 0; J < 2; J++) {
                        Dec->Slice->ChromaWeight[0][i][J] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                        Dec->Slice->ChromaOffset[0][i][J] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                    }
                }
            }
        }
        if ((Dec->Slice->Type % 5) == 1) {
            for (uint8_t i = 0; i <= Dec->MacroBlock->NumRefIndexActiveLevel1; i++) {
                Dec->Slice->LumaWeightFlag[1] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (Dec->Slice->LumaWeightFlag[1] == true) {
                    Dec->Slice->LumaWeight[1][i]  = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                    Dec->Slice->LumaOffset[1][i]  = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                }
                if (Dec->SPS->ChromaArrayType != ChromaBW) {
                    Dec->Slice->ChromaWeightFlag[1] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                    if (Dec->Slice->ChromaWeightFlag[1] == true) {
                        for (uint8_t J = 0; J < 2; J++) {
                            Dec->Slice->ChromaWeight[1][i][J] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                            Dec->Slice->ChromaOffset[1][i][J] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                        }
                    }
                }
            }
        }
    }
    
    void DecodeRefPicMarking(DecodeAVC *Dec, BitBuffer *BitB) { // dec_ref_pic_marking
        if (Dec->Slice->SliceIsIDR == true) {
            Dec->NAL->EmptyPictureBufferBeforeDisplay = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            Dec->NAL->FrameIsLongTermReference        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
        } else {
            Dec->NAL->AdaptiveRefPicMarkingModeFlag   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->NAL->AdaptiveRefPicMarkingModeFlag == true) {
                Dec->NAL->MemManControlOp = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                if ((Dec->NAL->MemManControlOp == 1) || (Dec->NAL->MemManControlOp == 3)) {
                    Dec->NAL->PicNumDiff = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                }
                if (Dec->NAL->MemManControlOp == 2) {
                    Dec->Slice->LongTermPicNum  = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                }
                if ((Dec->NAL->MemManControlOp == 3) || (Dec->NAL->MemManControlOp == 6)) {
                    Dec->NAL->LongTermFrameIndex = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                }
                if (Dec->NAL->MemManControlOp == 4) {
                    Dec->NAL->MaxLongTermFrameIndex = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) - 1;
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
        
        uint64_t coeff_token = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
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
                    trailing_ones_sign_flag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                    levelVal[Coefficent] = 1 - 2 * trailing_ones_sign_flag;
                } else {
                    level_prefix = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    levelCode = (Min(15, level_prefix) << suffixLength);
                    if ((suffixLength > 0) || (level_prefix >= 14)) {
                        level_suffix = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 0);
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
                    total_zeros = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    zerosLeft = total_zeros;
                } else {
                    zerosLeft = 0;
                }
                for (uint64_t i = 0; i < TotalCoeff(coeff_token) - 1; i++) {
                    if (zerosLeft > 0) {
                        run_before = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
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
                Log(Severity_DEBUG, PlatformIO_FunctionName, u8"NAL ID 0x%X is not supported, seeking to next NAL\n", Dec->NAL->NALUnitType);
                break;
        }
    }
    
    void RescanSync(BitBuffer *BitB) {
        // search for 0x000001, or 0x000003 for the next NAL.
        
    }
    
    void rbsp_trailing_bits(DecodeAVC *Dec, BitBuffer *BitB) { // rbsp_trailing_bits
        bool rbsp_stop_one_bit = 0;
        rbsp_stop_one_bit = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
        BitBuffer_Align(BitB, 1); // while( !byte_aligned( ) )
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
    
    BitBuffer *ExtractNAL2Packet(DecodeAVC *Dec, BitBuffer *AVCStream) { // NAL here means any NAL, packet here means NAL with any substituting required performed.
        /*
         Within a NAL, we need to check the size of the NAL, then scan through the input stream to make sure there are no bytes set to 0x000003, 0x000004, or 0x000005?
         */

        /*
         NAL rules:

         Can not end with a zero byte
         */
        return NULL;
    }

    /* Sequence Parameter Set */
    void ParseSequenceParameterSetData(DecodeAVC *Dec, BitBuffer *BitB) { // seq_parameter_set_data
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 8);
            Dec->SPS->ProfileIDC                                                        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 8); // 100
            Dec->SPS->ConstraintFlag0                                                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1); // 0
            Dec->SPS->ConstraintFlag1                                                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1); // 0
            Dec->SPS->ConstraintFlag2                                                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1); // 0
            Dec->SPS->ConstraintFlag3                                                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1); // 0
            Dec->SPS->ConstraintFlag4                                                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1); // 0
            Dec->SPS->ConstraintFlag5                                                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1); // 0
            BitBuffer_Seek(BitB, 2); // Zero bits.
            Dec->SPS->LevelIDC[0]                                                       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 8); // 51
            Dec->SPS->SeqParamSetID                                                     = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false); // 1

            if ((Dec->SPS->ProfileIDC == 44)  ||
                (Dec->SPS->ProfileIDC == 83)  ||
                (Dec->SPS->ProfileIDC == 86)  ||
                (Dec->SPS->ProfileIDC == 100) ||
                (Dec->SPS->ProfileIDC == 110) ||
                (Dec->SPS->ProfileIDC == 118) ||
                (Dec->SPS->ProfileIDC == 122) ||
                (Dec->SPS->ProfileIDC == 128) ||
                (Dec->SPS->ProfileIDC == 134) ||
                (Dec->SPS->ProfileIDC == 138) ||
                (Dec->SPS->ProfileIDC == 139) ||
                (Dec->SPS->ProfileIDC == 244)) {
                ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                Dec->SPS->ChromaFormatIDC                                               = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false); // 1 aka 420
                if (Dec->SPS->ChromaFormatIDC == Chroma444) {
                    Dec->SPS->SeperateColorPlane                                        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                }
                Dec->SPS->LumaBitDepthMinus8                                            = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false); // 8
                Dec->SPS->ChromaBitDepthMinus8                                          = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                Dec->SPS->QPPrimeBypassFlag                                             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1); // qpprime_y_zero_transform_bypass_flag
                Dec->SPS->ScalingMatrixFlag                                             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);

                if (Dec->SPS->ScalingMatrixFlag == true) {
                    for (uint8_t i = 0; i < ((Dec->SPS->ChromaFormatIDC != Chroma444) ? 8 : 12); i++) {
                        Dec->SPS->ScalingListFlag[i]                                    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                        if (Dec->SPS->ScalingListFlag[i] == true) {
                            if (i < 6) {
                                ScalingList(Dec, BitB, ScalingList4x4[i], 16, Dec->PPS->DefaultScalingMatrix4x4[i]);
                            } else {
                                ScalingList(Dec, BitB, ScalingList8x8[i - 6], 64, Dec->PPS->DefaultScalingMatrix8x8[i - 6]);
                            }
                        }
                    }
                }
            }
            Dec->SPS->MaxFrameNumMinus4                                                 = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 4; // 3
            Dec->SPS->PicOrderCount                                                     = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            if (Dec->SPS->PicOrderCount == 0) {
                Dec->SPS->MaxPicOrder                                                   = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 4;
            } else {
                Dec->SPS->DeltaPicOrder                                                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                Dec->SPS->OffsetNonRefPic                                               = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                Dec->SPS->OffsetTop2Bottom                                              = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                Dec->SPS->RefFramesInPicOrder                                           = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                for (uint8_t i = 0; i < Dec->SPS->RefFramesInPicOrder; i++) {
                    Dec->SPS->RefFrameOffset[i]                                         = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                }

            }
            Dec->SPS->MaxRefFrames                                                      = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            Dec->SPS->GapsInFrameNumAllowed                                             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            Dec->SPS->PicWidthInMacroBlocksMinus1                                       = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            Dec->SPS->PicHeightInMapUnitsMinus1                                         = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            Dec->SPS->OnlyMacroBlocksInFrame                                            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->SPS->OnlyMacroBlocksInFrame == false) {
                Dec->SPS->AdaptiveFrameFlag                                             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            }
            Dec->SPS->Inference8x8                                                      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            Dec->SPS->FrameCroppingFlag                                                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->SPS->FrameCroppingFlag == true) {
                Dec->SPS->FrameCropLeft                                                 = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                Dec->SPS->FrameCropRight                                                = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                Dec->SPS->FrameCropTop                                                  = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                Dec->SPS->FrameCropBottom                                               = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            }
            Dec->SPS->VUIPresent                                                        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->SPS->VUIPresent == true) {
                ParseVideoUsabilityInformation(Dec, BitB);
            }
        }
    }

    void ParseNALSequenceParameterSetExtended(DecodeAVC *Dec, BitBuffer *BitB) { // seq_parameter_set_extension_rbsp?
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SPS->SeqParamSetID                                                     = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            Dec->SPS->AuxiliaryFormatID                                                 = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            if (Dec->SPS->AuxiliaryFormatID != 0) {
                Dec->SPS->AuxiliaryBitDepth                                             = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 8;
                Dec->SPS->AlphaIncrFlag                                                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                Dec->SPS->AlphaOpaqueValue                                              = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Dec->SPS->AuxiliaryBitDepth + 9);
                Dec->SPS->AlphaTransparentValue                                         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Dec->SPS->AuxiliaryBitDepth + 9);
            }
            Dec->SPS->AdditionalExtensionFlag                                           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            BitBuffer_Align(BitB, 1); // rbsp_trailing_bits
        }
    }

    void ParseNALSubsetSPS(DecodeAVC *Dec, BitBuffer *BitB) { // subset_seq_parameter_set_rbsp
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            ReadSequenceParameterSetData(Dec, BitB);
            if ((Dec->SPS->ProfileIDC == Scalable_Constrained_Baseline_Profile) || (Dec->SPS->ProfileIDC == Scalable_High_Intra_Profile)) { // Scalable Video mode
                ParseNALSequenceParameterSetSVC(Dec, BitB);
                if (Dec->SPS->VUIPresent == true) {
                    ParseSVCVUIExtension(Dec, BitB);
                }
            } else if ((Dec->SPS->ProfileIDC == MultiView_High_Profile) || (Dec->SPS->ProfileIDC == Stereo_High_Profile) || (Dec->SPS->ProfileIDC == 134)) {
                // Multi View Coding
                BitBuffer_Seek(BitB, 1);
                ParseSPSMultiViewCodingExtension(Dec, BitB); // seq_parameter_set_mvc_extension
                Dec->SPS->MVCVUIParamsPresent                                           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (Dec->SPS->MVCVUIParamsPresent == true) {
                    ParseMVCVUI(Dec, BitB);
                }
            } else if (Dec->SPS->ProfileIDC == MultiView_Depth_High_Profile) {
                // MVCD
                BitBuffer_Seek(BitB, 1);
                ParseSPSMVCDExtension(Dec, BitB);
            } else if (Dec->SPS->ProfileIDC == 139) {
                // MVCD && 3DAVC
                BitBuffer_Seek(BitB, 1);
                ParseSPSMVCDExtension(Dec, BitB);
                ParseSPS3DAVCExtension(Dec, BitB);
            }
            Dec->SPS->AdditionalExtension2                                              = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->SPS->AdditionalExtension2 == true) {
                while (more_rbsp_data()) {
                    Dec->SPS->AdditionalExtension2DataFlag                              = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                }
            }
            BitBuffer_Align(BitB, 1); // rbsp_trailing_bits
        }
    }

    void ParseNALSequenceParameterSet(DecodeAVC *Dec, BitBuffer *BitB) { // seq_parameter_set_rbsp
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            ParseSequenceParameterSetData(Dec, BitB);                        // seq_parameter_set_data
            BitBuffer_Align(BitB, 1);                                         // rbsp_trailing_bits();
        }
    }

    /* Video Usability Information */
    void ParseVideoUsabilityInformation(DecodeAVC *Dec, BitBuffer *BitB) { // Video Usability Information; ParseVUIParameters
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->VUI->AspectRatioInfoPresent                                            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->VUI->AspectRatioInfoPresent == true) {
                Dec->VUI->AspectRatioIDC                                                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 8);
                if (Dec->VUI->AspectRatioIDC == 255) { // Extended_SAR = 255
                    Dec->VUI->SARWidth                                                  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
                    Dec->VUI->SAWHeight                                                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
                }
            }
            Dec->VUI->OverscanInfoPresent                                               = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->VUI->OverscanInfoPresent == true) {
                Dec->VUI->DisplayInOverscan                                             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            }
            Dec->VUI->VideoSignalTypePresent                                            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->VUI->VideoSignalTypePresent == true) {
                Dec->VUI->VideoType                                                     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 3);
                Dec->VUI->FullRange                                                     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                Dec->VUI->ColorDescriptionPresent                                       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (Dec->VUI->ColorDescriptionPresent == true) {
                    Dec->VUI->ColorPrimaries                                            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 8);
                    Dec->VUI->TransferProperties                                        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 8);
                    Dec->VUI->MatrixCoefficients                                        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 8);
                }
            }
            Dec->VUI->ChromaLocationPresent                                             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->VUI->ChromaLocationPresent == true) {
                Dec->VUI->ChromaSampleLocationTop                                       = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                Dec->VUI->ChromaSampleLocationBottom                                    = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            }
            Dec->VUI->TimingInfoPresent[0]                                              = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->VUI->TimingInfoPresent[0] == true) {
                Dec->VUI->UnitsPerTick[0]                                               = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 32);
                Dec->VUI->TimeScale[0]                                                  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 32);
                Dec->VUI->FixedFrameRateFlag[0]                                         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            }
            Dec->VUI->NALHrdParamsPresent[0]                                            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->VUI->NALHrdParamsPresent[0] == true) {
                ParseHypotheticalReferenceDecoder(Dec, BitB);
            }
            Dec->VUI->VCLHrdParamsPresent[0]                                            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->VUI->VCLHrdParamsPresent[0] == true) {
                ParseHypotheticalReferenceDecoder(Dec, BitB); // wat
            }
            if ((Dec->VUI->NALHrdParamsPresent[0] || Dec->VUI->VCLHrdParamsPresent[0]) == true) {
                //Dec->StreamIsLowDelay                                                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            }
            Dec->VUI->PicStructPresent[0]                                               = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            Dec->VUI->BitStreamRestricted                                               = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->VUI->BitStreamRestricted == true) {
                Dec->VUI->MotionVectorsOverPicBoundaries[0]                             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                Dec->VUI->MaxBytesPerPicDenom[0]                                        = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                Dec->VUI->MaxBitsPerMarcoBlockDenom[0]                                  = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                Dec->VUI->MaxMotionVectorLength[0]                                      = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                Dec->VUI->MaxMotionVectorHeight[0]                                      = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                Dec->VUI->MaxReorderFrames[0]                                           = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                Dec->VUI->MaxFrameBuffer[0]                                             = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            }
        }
    }

    void ParseMVCDVUIParametersExtension(DecodeAVC *Dec, BitBuffer *BitB) { // mvcd_vui_parameters_extension
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->VUI->VUIMVCDNumOpPoints                                                = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
            for (uint8_t MVCDOpPoint = 0; MVCDOpPoint < Dec->VUI->VUIMVCDNumOpPoints; MVCDOpPoint++) {
                Dec->VUI->VUIMVCDTemporalID[MVCDOpPoint]                                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 3);
                Dec->VUI->VUIMVCDNumTargetOutputViews[MVCDOpPoint]                      = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                for (uint16_t VUIMVCDView = 0; VUIMVCDView < Dec->VUI->VUIMVCDNumTargetOutputViews[MVCDOpPoint]; VUIMVCDView++) {
                    Dec->VUI->VUIMVCDViewID[MVCDOpPoint][VUIMVCDView]                   = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    Dec->VUI->VUIMVCDDepthFlag[MVCDOpPoint][VUIMVCDView]                = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    Dec->VUI->VUIMVCDTextureFlag[MVCDOpPoint][VUIMVCDView]              = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                }
                Dec->VUI->VUIMVCDTimingInfoPresent[MVCDOpPoint]                         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (Dec->VUI->VUIMVCDTimingInfoPresent[MVCDOpPoint] == true) {
                    Dec->VUI->VUIMVCDNumUnitsInTick[MVCDOpPoint]                        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 32);
                    Dec->VUI->VUIMVCDTimeScale[MVCDOpPoint]                             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 32);
                    Dec->VUI->VUIMVCDFixedFrameRateFlag[MVCDOpPoint]                    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                }
                Dec->VUI->VUIMVCDNalHRDParametersPresent[MVCDOpPoint]                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (Dec->VUI->VUIMVCDNalHRDParametersPresent[MVCDOpPoint] == true) {
                    hrd_parameters();
                }
                Dec->VUI->VUIMVCDVclHRDParametersPresent[MVCDOpPoint]                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (Dec->VUI->VUIMVCDVclHRDParametersPresent[MVCDOpPoint] == true) {
                    hrd_parameters();
                }
                if ((Dec->VUI->VUIMVCDNalHRDParametersPresent[MVCDOpPoint] || Dec->VUI->VUIMVCDVclHRDParametersPresent[MVCDOpPoint]) == true) {
                    Dec->VUI->VUIMVCDHRDLowDelayPresent[MVCDOpPoint]                    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                }
                Dec->VUI->VUIMVCDPicStructPresent[MVCDOpPoint]                          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            }
        }
    }

    void ParseMVCVUIParametersExtension(DecodeAVC *Dec, BitBuffer *BitB) { // mvc_vui_parameters_extension
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->VUI->MVCNumOpertionPoints                                              = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
            for (uint16_t Operation = 0; Operation < Dec->VUI->MVCNumOpertionPoints; Operation++) {
                Dec->VUI->MVCTemporalID[0][Operation]                                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 3);
                Dec->VUI->MVCNumTargetViews[Operation]                                  = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                for (uint16_t OutputView = 0; OutputView < Dec->VUI->MVCNumTargetViews[Operation]; OutputView++) {
                    Dec->VUI->MVCViewID[Operation][OutputView]                          = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                }
                Dec->VUI->TimingInfoPresent[Operation]                                  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (Dec->VUI->TimingInfoPresent[Operation] == true) {
                    Dec->VUI->MVCUnitsInTick[Operation]                                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 32);
                    Dec->VUI->MVCTimeScale[Operation]                                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 32);
                    Dec->VUI->MVCFixedFrameRate[Operation]                              = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                }
                Dec->VUI->MVCNALHRDParamsPresent[Operation]                             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (Dec->VUI->MVCNALHRDParamsPresent[Operation] == true) {
                    ParseHypotheticalReferenceDecoder(Dec, BitB);
                }
                Dec->VUI->MVCVCLHRDParamsPresent[Operation]                             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (Dec->VUI->MVCVCLHRDParamsPresent[Operation] == true) {
                    ParseHypotheticalReferenceDecoder(Dec, BitB);
                }
                if ((Dec->VUI->MVCNALHRDParamsPresent[Operation] || Dec->VUI->MVCVCLHRDParamsPresent[Operation]) == true) {
                    Dec->VUI->MVCLowDelayFlag[Operation]                                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                }
                Dec->VUI->PicStructPresent[Operation]                                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            }
        }
    }

    void ParseSVCVUIExtension(DecodeAVC *Dec, BitBuffer *BitB) { // svc_vui_parameters_extension
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->VUI->VUIExtNumEntries                                                  = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
            for (uint8_t VUIExtEntry = 0; VUIExtEntry < Dec->VUI->VUIExtNumEntries; VUIExtEntry++) {
                Dec->VUI->VUIExtDependencyID[VUIExtEntry]                               = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 3);
                Dec->VUI->VUIExtQualityID[VUIExtEntry]                                  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 4);
                Dec->VUI->VUIExtTemporalID[VUIExtEntry]                                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 3);
                Dec->VUI->VUIExtTimingInfoPresentFlag[VUIExtEntry]                      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (Dec->VUI->VUIExtTimingInfoPresentFlag[VUIExtEntry] == true) {
                    Dec->VUI->VUIExtNumUnitsInTick[VUIExtEntry]                         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 32);
                    Dec->VUI->VUIExtTimeScale[VUIExtEntry]                              = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 32);
                    Dec->VUI->VUIExtFixedFrameRateFlag[VUIExtEntry]                     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                }
                Dec->VUI->VUIExtNALHRDPresentFlag[VUIExtEntry]                          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (Dec->VUI->VUIExtNALHRDPresentFlag[VUIExtEntry] == true) {
                    ParseHypotheticalReferenceDecoder(Dec, BitB);
                }
                Dec->VUI->VUIExtVCLHRDPresentFlag[VUIExtEntry]                          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (Dec->VUI->VUIExtVCLHRDPresentFlag[VUIExtEntry] == true) {
                    ParseHypotheticalReferenceDecoder(Dec, BitB);
                }
                if ((Dec->VUI->VUIExtNALHRDPresentFlag[VUIExtEntry] == true) || (Dec->VUI->VUIExtVCLHRDPresentFlag[VUIExtEntry] == true)) {
                    Dec->VUI->VUIExtLowDelayHRDFlag[VUIExtEntry]                        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                }
                Dec->VUI->VUIExtPicStructPresentFlag[VUIExtEntry]                       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            }
        }
    }

    /* Picture Parameter Set */
    void ParseNALPictureParameterSet(DecodeAVC *Dec, BitBuffer *BitB) { // pic_parameter_set_rbsp
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->PPS->PicParamSetID                                                     = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false); // 3?
            Dec->SPS->SeqParamSetID                                                     = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false); // 2
            Dec->PPS->EntropyCodingMode                                                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1); // bit 9, 0
            Dec->PPS->BottomPicFieldOrderInSliceFlag                                    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1); // bit 8, 0
            Dec->PPS->SliceGroups                                                       = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1; // 2
            if (Dec->PPS->SliceGroups > 0) {
                Dec->PPS->SliceGroupMapType                                             = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false); // 48?
                if (Dec->PPS->SliceGroupMapType == 0) {
                    for (uint8_t SliceGroup = 0; SliceGroup <= Dec->PPS->SliceGroups; SliceGroup++) {
                        Dec->PPS->RunLength[SliceGroup]                                 = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                    }
                } else if (Dec->PPS->SliceGroupMapType == 2) {
                    for (uint8_t SliceGroup = 0; SliceGroup <= Dec->PPS->SliceGroups; SliceGroup++) {
                        Dec->PPS->TopLeft[SliceGroup]                                   = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                        Dec->PPS->BottomRight[SliceGroup]                               = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    }
                } else if ((Dec->PPS->SliceGroupMapType == 3) || (Dec->PPS->SliceGroupMapType == 4) || (Dec->PPS->SliceGroupMapType == 5)) {
                    for (uint8_t SliceGroup = 0; SliceGroup <= Dec->PPS->SliceGroups; SliceGroup++) {
                        Dec->PPS->SliceGroupChangeDirection                             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                        Dec->PPS->SliceGroupChangeRate                                  = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                    }
                } else if (Dec->PPS->SliceGroupMapType == 6) {
                    Dec->PPS->PicSizeInMapUnits                                         = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                    for (uint64_t MapUnit = 0; MapUnit <= Dec->PPS->PicSizeInMapUnits; MapUnit++) {
                        Dec->PPS->SliceGroupID[MapUnit]                                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Ceili(log2(Dec->PPS->SliceGroups)));
                    }
                }
            }
            Dec->PPS->RefIndex[0]                                                       = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
            Dec->PPS->RefIndex[1]                                                       = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
            Dec->PPS->WeightedPrediction                                                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            Dec->PPS->WeightedBiPrediction                                              = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            Dec->PPS->InitialSliceQP                                                    = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true) + 26;
            Dec->PPS->InitialSliceQS                                                    = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true) + 26;
            Dec->PPS->ChromaQPOffset                                                    = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
            Dec->PPS->DeblockingFilterFlag                                              = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            Dec->PPS->ConstrainedIntraFlag                                              = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            Dec->PPS->RedundantPictureFlag                                              = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (more_rbsp_data() == true) {
                Dec->PPS->TransformIs8x8                                                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                Dec->PPS->SeperateScalingMatrix                                         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (Dec->PPS->SeperateScalingMatrix == true) {
                    for (uint8_t i = 0; i < 6 + ((Dec->SPS->ChromaFormatIDC != Chroma444) ? 2 : 6) * Dec->PPS->TransformIs8x8; i++) {
                        Dec->PPS->PicScalingList[i]                                     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                        if (Dec->PPS->PicScalingList[i] == true) {
                            if (i < 6) {
                                ScalingList(Dec, BitB, ScalingList4x4[i], 16, Dec->PPS->DefaultScalingMatrix4x4[i]);
                            } else {
                                ScalingList(Dec, BitB, ScalingList8x8[i - 6], 64, Dec->PPS->DefaultScalingMatrix8x8[i - 6]);
                            }
                        }
                    }
                    Dec->PPS->ChromaQPOffset                                            = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                }
                BitBuffer_Align(BitB, 1);
            }
        }
    }

    /* Scalable Video Coding */
    void ParseNALSVCExtension(DecodeAVC *Dec, BitBuffer *BitB) { // nal_unit_header_svc_extension
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->NAL->IDRFlag                                                           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            Dec->NAL->PriorityID[0]                                                     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 6);
            Dec->NAL->NoInterLayerPredictionFlag                                        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            Dec->NAL->DependencyID[0]                                                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 3);
            Dec->NAL->QualityID[0][0]                                                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 4);
            Dec->NAL->TemporalID[0]                                                     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 3);
            Dec->NAL->UseReferenceBasePictureFlag                                       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            Dec->NAL->IsDisposable[0]                                                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            Dec->NAL->OutputFlag                                                        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            BitBuffer_Seek(BitB, 2); // reserved_three_2bits
        }
    }

    void ParseNALSequenceParameterSetSVC(DecodeAVC *Dec, BitBuffer *BitB) { // seq_parameter_set_svc_extension
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SVC->InterLayerDeblockingFilterPresent                                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            Dec->SVC->ExtendedSpatialScalabilityIDC                                     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 2);
            if ((Dec->SPS->ChromaFormatIDC == Chroma420) || (Dec->SPS->ChromaFormatIDC == Chroma422)) {
                Dec->SVC->ChromaPhaseXFlag                                              = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            }
            if (Dec->SPS->ChromaFormatIDC == Chroma420) {
                Dec->SVC->ChromaPhaseY                                                  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 2);
            }
            if (Dec->SVC->ExtendedSpatialScalabilityIDC == 1) {
                if (Dec->SPS->ChromaFormatIDC != ChromaBW) {
                    Dec->SVC->SeqRefLayerChromaPhaseX                                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                    Dec->SVC->SeqRefLayerChromaPhaseY                                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 2);
                }
                Dec->SVC->RefLayerLeftOffset                                            = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                Dec->SVC->RefLayerTopOffset                                             = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                Dec->SVC->RefLayerRightOffset                                           = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                Dec->SVC->RefLayerBottomOffset                                          = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
            }
            Dec->SVC->SequenceCoeffLevelPresent                                         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->SVC->SequenceCoeffLevelPresent == true) {
                Dec->SVC->AdaptiveCoeffsPresent                                         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            }
            Dec->SVC->SliceHeaderRestricted                                             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
        }
    }

    void ParseNALPrefixUnitSVC(DecodeAVC *Dec, BitBuffer *BitB) { // prefix_nal_unit_svc
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            if (Dec->NAL->NALRefIDC != 0) {
                Dec->Slice->StoreRefBasePicFlag                                         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (((Dec->NAL->UseReferenceBasePictureFlag) || (Dec->Slice->StoreRefBasePicFlag)) && (!Dec->NAL->IDRFlag)) {
                    ParseReferenceBasePictureSyntax(Dec, BitB); // dec_ref_base_pic_marking();
                }
                Dec->NAL->AdditionalPrefixNALExtensionFlag                              = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (Dec->NAL->AdditionalPrefixNALExtensionFlag == true) {
                    while (more_rbsp_data()) {
                        Dec->NAL->AdditionalPrefixNALExtensionDataFlag                  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                    }
                }
                BitBuffer_Align(BitB, 1); // rbsp_trailing_bits()
            } else if (more_rbsp_data()) {
                while (more_rbsp_data()) {
                    Dec->NAL->AdditionalPrefixNALExtensionDataFlag                      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                }
            }
            BitBuffer_Align(BitB, 1); // rbsp_trailing_bits()
        }
    }

    /* Multi-View Coding */
    void ParseNALMVCExtension(DecodeAVC *Dec, BitBuffer *BitB) { // nal_unit_header_mvc_extension
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->NAL->NonIDRFlag                                                        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            Dec->NAL->PriorityID[0]                                                     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 6);
            Dec->SPS->ViewID[0][0]                                                      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 10);
            Dec->NAL->TemporalID[0]                                                     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 3);
            Dec->NAL->IsAnchorPicture                                                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            Dec->NAL->InterViewFlag                                                     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            BitBuffer_Seek(BitB, 1);
        }
    }

    void ParseSPSMVCDExtension(DecodeAVC *Dec, BitBuffer *BitB) { // seq_parameter_set_mvcd_extension
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SPS->ViewCount                                                         = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
            for (uint16_t View = 0; View < Dec->SPS->ViewCount; View++) {
                Dec->SPS->ViewID[View][0]                                               = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                Dec->SPS->DepthViewPresent[View]                                        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                Dec->SPS->DepthViewID[View]                                             = Dec->SPS->ViewID[View][0];
                Dec->DPS->NumDepthViews                                                += Dec->SPS->DepthViewPresent[View];
                Dec->SPS->TextureViewPresent[View]                                      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            }
            for (uint16_t View = 1; View < Dec->SPS->ViewCount; View++) {
                if (Dec->SPS->DepthViewPresent[View] == true) {
                    Dec->SPS->AnchorRefsCount[0][View]                                  = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    for (uint8_t AnchorRef = 0; AnchorRef < Dec->SPS->AnchorRefsCount[0][View]; AnchorRef++) {
                        Dec->SPS->AnchorRef[0][View][AnchorRef]                         = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    }
                    Dec->SPS->AnchorRefsCount[1][View]                                  = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    for (uint8_t AnchorRef = 0; AnchorRef < Dec->SPS->AnchorRefsCount[1][View]; AnchorRef++) {
                        Dec->SPS->AnchorRef[1][View][AnchorRef]                         = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    }
                }
            }
            for (uint16_t View = 1; View < Dec->SPS->ViewCount; View++) {
                if (Dec->SPS->DepthViewPresent[View] == true) {
                    Dec->SPS->NonAnchorRefCount[0][View]                                = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    for (uint16_t NonAnchorRef = 0; NonAnchorRef < Dec->SPS->NonAnchorRefCount[0][View]; NonAnchorRef++) {
                        Dec->SPS->NonAnchorRef[0][View][NonAnchorRef]                   = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    }
                    Dec->SPS->NonAnchorRefCount[1][View]                                = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    for (uint16_t NonAnchorRef = 0; NonAnchorRef < Dec->SPS->NonAnchorRefCount[0][View]; NonAnchorRef++) {
                        Dec->SPS->NonAnchorRef[1][View][NonAnchorRef]                   = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    }
                }
            }
            Dec->SPS->NumLevelValues                                                    = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
            for (uint8_t Level = 0; Level < Dec->SPS->NumLevelValues; Level++) {
                Dec->SPS->LevelIDC[Level]                                               = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                Dec->SPS->NumApplicableOps[Level]                                       = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                for (uint16_t AppOp = 0; AppOp < Dec->SPS->NumApplicableOps[Level]; AppOp++) {
                    Dec->SPS->AppOpTemporalID[Level][AppOp]                             = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    Dec->SPS->AppOpNumTargetViews[Level][AppOp]                         = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                    for (uint16_t AppOpTargetView = 0; AppOpTargetView < Dec->SPS->AppOpNumTargetViews[Level][AppOp]; AppOpTargetView++) {
                        Dec->SPS->AppOpTargetViewID[Level][AppOp][AppOpTargetView]      = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                        Dec->SPS->AppOpDepthFlag[Level][AppOp][AppOpTargetView]         = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                        Dec->SPS->AppOpTextureFlag[Level][AppOp][AppOpTargetView]       = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    }
                    Dec->SPS->AppOpTextureViews[Level][AppOp]                           = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                    Dec->SPS->AppOpNumDepthViews[Level][AppOp]                          = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                }
            }
            Dec->SPS->MVCDVUIParametersPresent                                          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->SPS->MVCDVUIParametersPresent == true) {
                ParseMVCDVUIParametersExtension(Dec, BitB); // mvcd_vui_parameters_extension();
            }
            Dec->SPS->MVCDTextureVUIParametersPresent                                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->SPS->MVCDTextureVUIParametersPresent == true) {
                ParseMVCVUIParametersExtension(Dec, BitB); //mvc_vui_parameters_extension();
            }
        }
    }

    void ParseNALDepthParameterSet(DecodeAVC *Dec, BitBuffer *BitB) { // depth_parameter_set_rbsp
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->DPS->DepthParameterSetID                                               = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            Dec->DPS->PredictionDirection                                               = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            if ((Dec->DPS->PredictionDirection == 0) || (Dec->DPS->PredictionDirection == 1)) {
                Dec->DPS->ReferenceDPSID[0]                                             = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                Dec->DPS->PredictedWeight0                                              = 64;
            }
            if (Dec->DPS->PredictionDirection == 0) {
                Dec->DPS->ReferenceDPSID[1]                                             = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                Dec->DPS->PredictedWeight0                                              = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 6);
            }
            Dec->DPS->NumDepthViews                                                     = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
            depth_ranges(BitB, Dec->DPS->NumDepthViews, Dec->DPS->PredictionDirection, Dec->DPS->DepthParameterSetID);
            Dec->DPS->VSPParamFlag                                                      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->DPS->VSPParamFlag == true) {
                vsp_param(Dec, BitB, Dec->DPS->NumDepthViews, Dec->DPS->PredictionDirection, Dec->DPS->DepthParameterSetID);
            }
            Dec->DPS->AdditionalExtensionFlag                                           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            Dec->DPS->DepthMappingValues                                                = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            for (uint8_t i = 1; i <= Dec->DPS->DepthMappingValues; i++) {
                Dec->DPS->DepthRepresentationModel[i]                                   = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            }
            if (Dec->DPS->AdditionalExtensionFlag == true) {
                while (more_rbsp_data() == true) {
                    Dec->DPS->AdditionalExtensionFlag                                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                }
            }
            BitBuffer_Align(BitB, 1); // rbsp_trailing_bits
        }
    }

    void ParseSPS3DAVCExtension(DecodeAVC *Dec, BitBuffer *BitB) { // seq_parameter_set_3davc_extension
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            if (Dec->DPS->NumDepthViews > 0) {
                Dec->SPS->AVC3DAcquisitionIDC                                           = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                for (uint8_t View = 0; View < Dec->DPS->NumDepthViews; View++) {
                    Dec->SPS->AVC3DViewID[View]                                         = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                }
                if (Dec->SPS->AVC3DAcquisitionIDC > 0) {
                    DepthRanges(BitB, Dec->DPS->NumDepthViews, 2, 0);
                    vsp_param(Dec, BitB, Dec->DPS->NumDepthViews, 2, 0);
                }
                Dec->SPS->ReducedResolutionFlag                                         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (Dec->SPS->ReducedResolutionFlag == true) {
                    Dec->SPS->DepthPicWidthInMacroBlocks                                = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                    Dec->SPS->DepthPicHeightInMapUnits                                  = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                    Dec->SPS->DepthHorizontalDisparity                                  = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                    Dec->SPS->DepthVerticalDisparity                                    = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                    Dec->SPS->DepthHorizontalRSH                                        = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    Dec->SPS->DepthVerticalRSH                                          = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                } else {
                    Dec->SPS->DepthHorizontalDisparity                                  = 1;
                    Dec->SPS->DepthVerticalDisparity                                    = 1;
                    Dec->SPS->DepthHorizontalRSH                                        = 0;
                    Dec->SPS->DepthVerticalRSH                                          = 0;
                }
                Dec->SPS->DepthFrameCroppingFlag                                        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (Dec->SPS->DepthFrameCroppingFlag == true) {
                    Dec->SPS->DepthFrameLeftCropOffset                                  = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    Dec->SPS->DepthFrameRightCropOffset                                 = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    Dec->SPS->DepthFrameTopCropOffset                                   = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    Dec->SPS->DepthFrameBottomCropOffset                                = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                }
                Dec->SPS->GridPosViewCount                                              = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                for (uint8_t TextureView = 0; TextureView < Dec->SPS->GridPosViewCount; TextureView++) {
                    Dec->SPS->GridPosViewID[TextureView]                                = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    Dec->SPS->GridPosX[TextureView]                                     = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                    Dec->SPS->GridPosY[TextureView]                                     = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                }
                Dec->SPS->SlicePrediction                                               = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                Dec->Slice->SeqViewSynthesisFlag                                        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            }
            Dec->SPS->ALCSpsEnableFlag                                                  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            Dec->SPS->EnableRLESkipFlag                                                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            bool AllViewsPairedFlag                                                     = AreAllViewsPaired(Dec);
            if (AllViewsPairedFlag == false) {
                for (uint8_t View = 0; View < Dec->SPS->ViewCount; View++) {
                    if (Dec->SPS->TextureViewPresent[View] == true) {
                        Dec->SPS->AnchorRefsCount[0][View]                              = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                        for (uint16_t AnchorViewL0 = 0; AnchorViewL0 < Dec->SPS->AnchorRefsCount[0][View]; AnchorViewL0++) {
                            Dec->SPS->AnchorRef[0][View][AnchorViewL0]                  = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                        }
                        Dec->SPS->AnchorRefsCount[1][View]                              = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                        for (uint16_t AnchorViewL1 = 0; AnchorViewL1 < Dec->SPS->AnchorRefsCount[1][View]; AnchorViewL1++) {
                            Dec->SPS->AnchorRef[1][View][AnchorViewL1]                  = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                        }
                    }
                }
                for (uint16_t View = 0; View <= Dec->SPS->ViewCount; View++) {
                    if (Dec->SPS->TextureViewPresent[View] == true) {
                        Dec->SPS->NonAnchorRefCount[0][View]                            = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                        for (uint16_t NonAnchorRefL0 = 0; NonAnchorRefL0 < Dec->SPS->NonAnchorRefCount[0][View]; NonAnchorRefL0++) {
                            Dec->SPS->NonAnchorRef[0][View][NonAnchorRefL0]             = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                        }
                        Dec->SPS->NonAnchorRefCount[1][View]                            = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                        for (uint16_t NonAnchorRefL1 = 0; NonAnchorRefL1 < Dec->SPS->NonAnchorRefCount[1][View]; NonAnchorRefL1++) {
                            Dec->SPS->NonAnchorRef[1][View][NonAnchorRefL1]             = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                        }
                    }
                }
            }
        }
    }

    void ParseNAL3DAVCExtension(DecodeAVC *Dec, BitBuffer *BitB) { // nal_unit_header_3davc_extension
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->NAL->ViewIndex                                                         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 8);
            Dec->NAL->DepthFlag                                                         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            Dec->NAL->NonIDRFlag                                                        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            Dec->NAL->TemporalID[0]                                                     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 3);
            Dec->NAL->IsAnchorPicture                                                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            Dec->NAL->InterViewFlag                                                     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
        }
    }

    /* Hypothetical Reference Decoder */
    void ParseHypotheticalReferenceDecoder(DecodeAVC *Dec, BitBuffer *BitB) { // hrd_parameters
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->HRD->NumCodedPictureBuffers                                            = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
            Dec->HRD->BitRateScale                                                      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 4);
            Dec->HRD->CodedPictureBufferScale                                           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 4);
            for (uint8_t SchedSelIdx = 0; SchedSelIdx < Dec->HRD->NumCodedPictureBuffers; SchedSelIdx++) {
                Dec->HRD->BitRate[SchedSelIdx]                                          = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                Dec->HRD->CodedPictureSize[SchedSelIdx]                                 = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                Dec->HRD->IsConstantBitRate[SchedSelIdx]                                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1) + 1; // FIXME: is +1 correct
            }
            Dec->HRD->InitialCPBDelayLength                                             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 5) + 1;
            Dec->HRD->CBPDelay                                                          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 5) + 1;
            Dec->HRD->DBPDelay                                                          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 5) + 1;
            Dec->HRD->TimeOffsetSize                                                    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 5);
        }
    }

    /* Generic */
    void ParseNALSliceHeader(DecodeAVC *Dec, BitBuffer *BitB) { // slice_header
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->Slice->FirstMacroBlockInSlice                                          = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false); // 0
            Dec->Slice->Type                                                            = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false); // 0, 34 0s remaining
            Dec->PPS->PicParamSetID                                                     = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false); // 0, 26 0s remaining

            if (Dec->SPS->SeperateColorPlane == true) {
                Dec->Slice->ColorPlaneID                                                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 2);
            }

            Dec->Slice->FrameNumber                                                     = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false); // FIXME: Should I use BitBuffer_ReadBits?
            if (Dec->SPS->OnlyMacroBlocksInFrame == false) {
                Dec->Slice->SliceIsInterlaced                                           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (Dec->Slice->SliceIsInterlaced == true) {
                    Dec->Slice->SliceIsBottomField                                      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                }
            }
            Dec->Slice->SliceIsIDR                                                      = ((Dec->NAL->NALUnitType == NAL_IDRSliceNonPartitioned) ? true : false);
            if (Dec->Slice->SliceIsIDR == true) {
                Dec->Slice->IDRPicID                                                    = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            }
            if (Dec->SPS->PicOrderCount == 0) {
                Dec->Slice->PictureOrderCountLSB                                        = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                if ((Dec->PPS->BottomPicFieldOrderInSliceFlag == true) && (Dec->Slice->SliceIsInterlaced == false)) {
                    Dec->Slice->DeltaPicOrderCount[0]                                   = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                }
            }
            if (Dec->SPS->PicOrderCount == true && Dec->SPS->DeltaPicOrder == false) {
                Dec->Slice->DeltaPicOrderCount[0]                                       = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                if ((Dec->PPS->BottomPicFieldOrderInSliceFlag == true) && (Dec->Slice->SliceIsInterlaced == false)) {
                    Dec->Slice->DeltaPicOrderCount[1]                                   = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                }
            }
            if (Dec->PPS->RedundantPictureFlag == true) {
                Dec->PPS->RedundantPictureCount                                         = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            }
            if ((Dec->Slice->Type == SliceB1) || (Dec->Slice->Type == SliceB2)) {
                Dec->Slice->DirectSpatialMVPredictionFlag                               = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);

            }
            if ((Dec->Slice->Type == SliceP1)  || (Dec->Slice->Type == SliceP2)  ||
                (Dec->Slice->Type == SliceSP1) || (Dec->Slice->Type == SliceSP2) ||
                (Dec->Slice->Type == SliceB1)  || (Dec->Slice->Type == SliceB2)) {
                Dec->Slice->NumRefIDXActiveOverrideFlag                                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (Dec->Slice->NumRefIDXActiveOverrideFlag == true) {
                    Dec->MacroBlock->NumRefIndexActiveLevel0                            = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1; // num_ref_idx_l0_active_minus1
                    if ((Dec->Slice->Type == SliceB1) || (Dec->Slice->Type == SliceB2)) {
                        Dec->MacroBlock->NumRefIndexActiveLevel1                        = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1; // num_ref_idx_l1_active_minus1
                    }
                }
            }
            if ((Dec->NAL->NALUnitType == NAL_AuxiliarySliceExtension) || (Dec->NAL->NALUnitType == NAL_MVCDepthView)) {
                RefPicListMVCMod(Dec, BitB);
            } else {
                RefPicListMod(Dec, BitB);
            }
            if ((Dec->PPS->WeightedPrediction == true)
                && (
                    (Dec->Slice->Type == SliceP1)  ||
                    (Dec->Slice->Type == SliceP2)  ||
                    (Dec->Slice->Type == SliceSP1) ||
                    (Dec->Slice->Type == SliceSP2) ||
                    (Dec->Slice->Type == SliceB1)  ||
                    (Dec->Slice->Type == SliceB2)
                    ) && (Dec->PPS->WeightedBiPrediction == true)) {
                        pred_weight_table(Dec, BitB);
                    }
            if (Dec->NAL->NALRefIDC != 0) {
                DecodeRefPicMarking(Dec, BitB);
            }
            if ((Dec->PPS->EntropyCodingMode  == true) && (((Dec->Slice->Type != SliceI1) || (Dec->Slice->Type != SliceI2) || (Dec->Slice->Type != SliceSI1) || (Dec->Slice->Type != SliceSI2)))) {
                Dec->Slice->CabacInitIDC                                                = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
            }
            Dec->Slice->SliceQPDelta                                                    = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
            if (
                (Dec->Slice->Type == SliceSP1) || (Dec->Slice->Type == SliceSP2) ||
                (Dec->Slice->Type == SliceSI1) || (Dec->Slice->Type == SliceSI2)) {

                    if ((Dec->Slice->Type == SliceSP1) || (Dec->Slice->Type == SliceSP2)) {
                        Dec->Slice->DecodePMBAsSPSlice                                      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                    }
                    Dec->Slice->SliceQSDelta                                                = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                }
            if (Dec->PPS->DeblockingFilterFlag == true) {
                Dec->Slice->DisableDeblockingFilterIDC                                  = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                if (Dec->Slice->DisableDeblockingFilterIDC  != true) {
                    Dec->Slice->SliceAlphaOffsetC0                                      = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                    Dec->Slice->SliceBetaOffset                                         = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                }
            }
            if (Dec->PPS->SliceGroups > 0 && (Dec->PPS->SliceGroupMapType >= 3 && Dec->PPS->SliceGroupMapType <= 5)) {
                uint64_t Bits                                                           = Ceili(log2(Dec->PPS->PicSizeInMapUnits / Dec->PPS->SliceGroupChangeRate));
                Dec->Slice->SliceGroupChangeCycle                                       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Ceili(log2(Dec->PPS->PicSizeInMapUnits /  Dec->Slice->SliceGroupChangeRate)));
            }
        }
    }

    void ParseNALSliceData(DecodeAVC *Dec, BitBuffer *BitB, uint8_t Category) { // slice_data
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            if (Dec->PPS->EntropyCodingMode == Arithmetic) {
                while (IsBitBufferAligned(BitB, 1) == false) {
                    BitBuffer_Seek(BitB, 1); // cabac_alignment_one_bit
                }
                uint64_t CurrentMacroBlockAddress                                       = Dec->Slice->FirstMacroBlockInSlice * (Dec->Slice->MbaffFrameFlag + 1);
                bool     MoreDataFlag                                                   = true;
                Dec->Slice->PreviousMacroBlockSkipped                                   = false;
                if ((Dec->Slice->Type != SliceI1) || (Dec->Slice->Type != SliceI2) || (Dec->Slice->Type != SliceSI1) || (Dec->Slice->Type != SliceSI2)) {
                    if (Dec->PPS->EntropyCodingMode == ExpGolomb) {
                        Dec->Slice->MacroBlockSkipRun                                   = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                        Dec->Slice->PreviousMacroBlockSkipped                           = (Dec->Slice->MacroBlockSkipRun > 0);
                        for (uint8_t SkippedMacroBlock = 0; SkippedMacroBlock < Dec->Slice->MacroBlockSkipRun; SkippedMacroBlock++) {
                            CurrentMacroBlockAddress                                    = NextMacroBlockAddress(Dec, CurrentMacroBlockAddress);
                        }
                    }
                }
            }
        }
    }

    void ParseNALSlicePartitionA(DecodeAVC *Dec, BitBuffer *BitB) { // slice_data_partition_a_layer_rbsp
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            ParseSliceHeader(Dec, BitB);
            uint64_t SliceID                                                            = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            ParseSliceData(Dec, BitB, 2); /* only category 2 parts of slice_data() syntax */
            rbsp_slice_trailing_bits(Dec, BitB); // BitBuffer_Align(BitB, 1);
        }
    }

    void ParseNALSlicePartitionB(DecodeAVC *Dec, BitBuffer *BitB) { // slice_data_partition_b_layer_rbsp
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            uint8_t SliceID                                                             = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            if (Dec->SPS->SeperateColorPlane == true) {
                uint8_t ColorPlaneID                                                    = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            }
            if (Dec->PPS->RedundantPictureFlag == true) {
                Dec->PPS->RedundantPictureCount                                         = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            }
            ParseSliceData(Dec, BitB, 3);
            rbsp_slice_trailing_bits(Dec, BitB); // BitBuffer_Align(BitB, 1);
        }
    }

    void ParseNALSlicePartitionC(DecodeAVC *Dec, BitBuffer *BitB) { // slice_data_partition_c_layer_rbsp
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            uint8_t SliceID                                                             = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            if (Dec->SPS->SeperateColorPlane == true) {
                uint8_t ColorPlaneID                                                    = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            }
            if (Dec->PPS->RedundantPictureFlag == true) {
                Dec->PPS->RedundantPictureCount                                         = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            }
            ParseSliceData(Dec, BitB, 4);
            rbsp_slice_trailing_bits(Dec, BitB); // BitBuffer_Align(BitB, 1);
        }
    }

    void ParseNALSliceNonPartitioned(DecodeAVC *Dec, BitBuffer *BitB) { // slice_layer_without_partitioning_rbsp
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            ParseNALSliceHeader(Dec, BitB);
            ParseNALSliceData(Dec, BitB, 0); // TODO: Fix category

            BitBuffer_Align(BitB, 1); // rbsp_slice_trailing_bits();
        }
    }

    void ParseNALFillerData(DecodeAVC *Dec, BitBuffer *BitB) { // filler_data_rbsp
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            while (BitBuffer_PeekBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8) == 0xFF) {
                BitBuffer_Seek(BitB, 8);
            }
        }
    }

    void ParseNALPrefixUnit(DecodeAVC *Dec, BitBuffer *BitB) { // prefix_nal_unit_rbsp
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            if (Dec->NAL->SVCExtensionFlag == true) {
                ParseNALPrefixUnitSVC(Dec, BitB);
            }
        }
    }

    void ParseNALAccessUnitDelimiter(DecodeAVC *Dec, BitBuffer *BitB) { // access_unit_delimiter_rbsp
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->Slice->PictureType                                                     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 3);
            BitBuffer_Align(BitB, 1);
        }
    }

    /* Supplemental Enhancement Information */
    void ParseSEIBufferingPeriod(DecodeAVC *Dec, BitBuffer *BitB) { // buffering_period
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SPS->SeqParamSetID                                                     = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            if (Dec->SEI->NalHrdBpPresentFlag == true) {
                for (uint8_t SchedSelIdx = 0; SchedSelIdx <= Dec->HRD->NumCodedPictureBuffers; SchedSelIdx++) {
                    Dec->SEI->InitialCPBRemovalDelay[SchedSelIdx]                       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Dec->HRD->InitialCPBDelayLength);
                    Dec->SEI->InitialCPBRemovalDelayOffset[SchedSelIdx]                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Dec->HRD->InitialCPBDelayLength);
                }
            }
            Dec->HRD->VclHrdBpPresentFlag                                               = Dec->VUI->VCLHrdParamsPresent > 0 ? 1 : 0;
            if (Dec->HRD->VclHrdBpPresentFlag == true) {
                for (uint8_t SchedSelIdx = 0; SchedSelIdx <= Dec->HRD->NumCodedPictureBuffers; SchedSelIdx++) {
                    Dec->SEI->InitialCPBRemovalDelay[SchedSelIdx]                       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Dec->HRD->InitialCPBDelayLength);
                    Dec->SEI->InitialCPBRemovalDelayOffset[SchedSelIdx]                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Dec->HRD->InitialCPBDelayLength);
                }
            }
        }
    }

    uint8_t GetClockTS(uint8_t PicStruct) {
        uint8_t ClockTS                                                             = 0;
        if ((PicStruct == 0) || (PicStruct == 1) || (PicStruct == 2)) {
            ClockTS                                                                 = 1;
        } else if ((PicStruct == 3) || (PicStruct == 4) || (PicStruct == 7)) {
            ClockTS                                                                 = 2;
        } else {
            ClockTS                                                                 = 3;
        }
        return ClockTS;
    }

    void ParseSEIPictureTiming(DecodeAVC *Dec, BitBuffer *BitB) { // pic_timing
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            if (Dec->SEI->CpbDpbDelaysPresentFlag == true) {
                Dec->SEI->CPBRemovalDelay                                               = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Dec->HRD->CBPDelay);
                Dec->SEI->DPBOutputDelay                                                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Dec->HRD->DBPDelay);
            }
            if (Dec->VUI->PicStructPresent[0] == true) {
                Dec->SEI->PicStruct                                                     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 4);
                uint8_t NumClockTS = GetClockTS(Dec->SEI->PicStruct);
                for (uint8_t Time = 0; Time < NumClockTS; Time++) {
                    Dec->SEI->ClockTimestampFlag[Time]                                  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                    if (Dec->SEI->ClockTimestampFlag[Time] == true) {
                        Dec->SEI->CTType                                                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 2);
                        Dec->SEI->NuitFieldBasedFlag                                    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                        Dec->SEI->CountingType                                          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 5);
                        Dec->SEI->FullTimestampFlag                                     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                        Dec->SEI->CountDroppedFlag                                      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                        Dec->SEI->NFrames                                               = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 8);
                        if (Dec->SEI->FullTimestampFlag == true) {
                            Dec->SEI->Seconds                                           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 6);
                            Dec->SEI->Minutes                                           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 6);
                            Dec->SEI->Hours                                             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 5);
                        } else {
                            Dec->SEI->SecondsFlag                                       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                            if (Dec->SEI->SecondsFlag == true) {
                                Dec->SEI->Seconds                                       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 6);
                                Dec->SEI->MinutesFlag                                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                                if (Dec->SEI->MinutesFlag == true) {
                                    Dec->SEI->Minutes                                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 6);
                                    Dec->SEI->HoursFlag                                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                                    if (Dec->SEI->HoursFlag == true) {
                                        Dec->SEI->Hours                                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 5);
                                    }
                                }
                            }
                            if (Dec->HRD->TimeOffsetSize > 0) {
                                Dec->SEI->TimeOffset                                    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Dec->HRD->TimeOffsetSize);
                            }
                        }
                    }
                }
            }
        }
    }

    void ParseSEIPanScan(DecodeAVC *Dec, BitBuffer *BitB) { // pan_scan_rect
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->PanScanID                                                         = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            Dec->SEI->DisablePanScanFlag                                                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->SEI->DisablePanScanFlag == false) {
                Dec->SEI->PanScanCount                                                  = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                for (uint8_t PanScan = 0; PanScan < Dec->SEI->PanScanCount; PanScan++) {
                    Dec->SEI->PanScanOffsetLeft[PanScan]                                = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                    Dec->SEI->PanScanOffsetRight[PanScan]                               = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                    Dec->SEI->PanScanOffsetTop[PanScan]                                 = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                    Dec->SEI->PanScanOffsetBottom[PanScan]                              = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                }
                Dec->SEI->PanScanRepitionPeriod                                         = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            }
        }
    }

    void ParseSEIFiller(DecodeAVC *Dec, BitBuffer *BitB) { // filler_payload
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            BitBuffer_Seek(BitB, Bytes2Bits(Dec->SEI->SEISize));
        }
    }

    void ParseSEIRegisteredUserData(DecodeAVC *Dec, BitBuffer *BitB) { // user_data_registered_itu_t_t35
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            uint8_t CountryCodeSize = 0;
            Dec->SEI->CountryCode                                                       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 8);
            if (Dec->SEI->CountryCode != 0xFF) {
                CountryCodeSize                                                        += 1;
            } else {
                CountryCodeSize                                                        += 2;
                Dec->SEI->CountryCode                                                 <<= 8;
                Dec->SEI->CountryCode                                                  += BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 8);
            }
        }
    }

    void ParseSEIUnregisteredUserData(DecodeAVC *Dec, BitBuffer *BitB) { // user_data_unregistered
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->UnregisteredUserDataUUID = ReadUUID(BitB); // DC45E9BD-E6D9-48B7-962C-D820D923EEEF, x264 UserID.
            BitBuffer_Seek(BitB, Bytes2Bits(Dec->SEI->SEISize - BitIOBinaryUUIDSize));
        }
    }

    void ParseSEIRecoveryPoint(DecodeAVC *Dec, BitBuffer *BitB) { // recovery_point
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->RecoveryFrameCount                                                = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            Dec->SEI->ExactMatchFlag                                                    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            Dec->SEI->BrokenLinkFlag                                                    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            Dec->SEI->ChangingSliceGroupIDC                                             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 2);
        }
    }

    void ParseSEIRepetitiveReferencePicture(DecodeAVC *Dec, BitBuffer *BitB) { // dec_ref_pic_marking_repetition
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->OriginalIDRFlag                                                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            Dec->SEI->OriginalFrameNum                                                  = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            if (Dec->SPS->OnlyMacroBlocksInFrame == false) {
                Dec->SEI->OriginalFieldPicFlag                                          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (Dec->SEI->OriginalFieldPicFlag == true) {
                    Dec->SEI->OriginalBottomFieldFlag                                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                }
            }
            DecodeRefPicMarking(Dec, BitB); // dec_ref_pic_marking();
        }
    }

    void ParseSEISparePicture(DecodeAVC *Dec, BitBuffer *BitB) { // spare_pic
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            uint8_t MapUnitCount = 0;

            Dec->SEI->TargetFrameNum                                                    = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            Dec->SEI->SpareFieldFlag                                                    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->SEI->SpareFieldFlag == true) {
                Dec->SEI->TargetBottomFieldFlag                                         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            }
            Dec->SEI->NumSparePics                                                      = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
            for (uint8_t SparePic = 0; SparePic < Dec->SEI->NumSparePics; SparePic++) {
                Dec->SEI->DeltaSpareFrameNum[SparePic]                                  = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                if (Dec->SEI->SpareFieldFlag == true) {
                    Dec->SEI->SpareBottomFieldFlag[SparePic]                            = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                }
                Dec->SEI->SpareAreaIDC[SparePic]                                        = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                if (Dec->SEI->SpareAreaIDC[SparePic] == 1) {
                    for (uint8_t MapUnit = 0; MapUnit < Dec->PPS->PicSizeInMapUnits; MapUnit++) {
                        Dec->SEI->SpareUnitFlag[SparePic][MapUnit]                      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                    }
                } else if (Dec->SEI->SpareAreaIDC[SparePic] == 2) {
                    while (MapUnitCount < Dec->PPS->PicSizeInMapUnits) {
                        Dec->SEI->ZeroRunLength[SparePic][MapUnitCount]                 = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                        MapUnitCount += Dec->SEI->ZeroRunLength[SparePic][MapUnitCount] + 1;
                    }
                }
            }
        }
    }

    void ParseSEISceneInfo(DecodeAVC *Dec, BitBuffer *BitB) { // scene_info
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->SceneInfoPresentFlag                                              = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->SEI->SceneInfoPresentFlag == true) {
                Dec->SEI->SceneID                                                       = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                Dec->SEI->SceneTransitionType                                           = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                if (Dec->SEI->SceneTransitionType > 3) {
                    Dec->SEI->SecondSceneID                                             = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                }
            }
        }
    }

    void ParseSEISubSequenceInfo(DecodeAVC *Dec, BitBuffer *BitB) { // sub_seq_info
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->SubSequenceLayerNum                                                = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            Dec->SEI->SubSequenceID                                                      = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            Dec->SEI->FirstRefPicFlag                                                    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            Dec->SEI->LeadingNonRefPicFlag                                               = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            Dec->SEI->LastPicFlag                                                        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            Dec->SEI->SubSeqFrameNumFlag                                                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->SEI->SubSeqFrameNumFlag  == true) {
                Dec->SEI->SubSeqFrameNum                                                 = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            }
        }
    }

    void ParseSEISubSequenceLayerProperties(DecodeAVC *Dec, BitBuffer *BitB) { // sub_seq_layer_characteristics
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->NumSubSeqLayers                                                    = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
            for (uint8_t Layer = 0; Layer < Dec->SEI->NumSubSeqLayers; Layer++) {
                Dec->SEI->AccurateStatisticsFlag                                         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                Dec->SEI->AverageBitRate                                                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
                Dec->SEI->AverageFrameRate                                               = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
            }
        }
    }

    void ParseSEISubSequenceProperties(DecodeAVC *Dec, BitBuffer *BitB) { // sub_seq_characteristics
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->SubSequenceLayerNum                                                = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            Dec->SEI->SubSequenceID                                                      = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            Dec->SEI->DurationFlag                                                       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->SEI->DurationFlag == true) {
                Dec->SEI->SubSeqDuration                                                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 32);
            }
            Dec->SEI->AverageRateFlag                                                    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->SEI->AverageRateFlag == true) {
                Dec->SEI->AccurateStatisticsFlag                                         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                Dec->SEI->AverageBitRate                                                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
                Dec->SEI->AverageFrameRate                                               = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
            }
            Dec->SEI->NumReferencedSubSeqs                                               = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            for (uint16_t SubSequence = 0; SubSequence < Dec->SEI->NumReferencedSubSeqs; SubSequence++) {
                Dec->SEI->RefSubSeqLayerNum                                              = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                Dec->SEI->RefSubSeqID                                                    = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                Dec->SEI->RefSubSeqDirection                                             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            }
        }
    }

    void ParseSEIFullFrameFreeze(DecodeAVC *Dec, BitBuffer *BitB) { // full_frame_freeze
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->FullFrameFreezeRepitionPeriod                                      = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
        }
    }

    void ParseSEIFullFrameFreezeRelease(DecodeAVC *Dec, BitBuffer *BitB) { // full_frame_freeze_release
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            BitBuffer_Seek(BitB, Bytes2Bits(Dec->SEI->SEISize));
        }
    }

    void ParseSEIFullFrameSnapshot(DecodeAVC *Dec, BitBuffer *BitB) { // full_frame_snapshot
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->SnapshotID                                                         = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
        }
    }

    void ParseSEIProgressiveRefinementSegmentStart(DecodeAVC *Dec, BitBuffer *BitB) { // progressive_refinement_segment_start
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->ProgressiveRefinementID                                            = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            Dec->SEI->NumRefinementSteps                                                 = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
        }
    }

    void ParseSEIProgressiveRefinementSegmentEnd(DecodeAVC *Dec, BitBuffer *BitB) { // progressive_refinement_segment_end
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->ProgressiveRefinementID                                            = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
        }
    }

    void ParseSEIMotionConstrainedSliceGroupSet(DecodeAVC *Dec, BitBuffer *BitB) { // motion_constrained_slice_group_set
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->NumSliceGroupsInSet                                                = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
            if (Dec->SEI->NumSliceGroupsInSet > 1) {
                for (uint16_t SliceGroup = 0; SliceGroup < Dec->SEI->NumSliceGroupsInSet; SliceGroup++) {
                    Dec->PPS->SliceGroupID[SliceGroup]                                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Ceili(log2(Dec->PPS->SliceGroups)));
                }
            }
            Dec->SEI->ExactSampleValueMatchFlag[0]                                       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            Dec->SEI->PanScanRectFlag                                                    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->SEI->PanScanRectFlag == true) {
                Dec->SEI->PanScanID                                                      = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            }
        }
    }

    void ParseSEIFilmGrainCharacteristics(DecodeAVC *Dec, BitBuffer *BitB) { // film_grain_characteristics
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->FilmGrainCharactisticsCancelFlag                                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->SEI->FilmGrainCharactisticsCancelFlag == false) {
                Dec->SEI->FilmGrainModelID                                               = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 2);
                Dec->SEI->SeperateColorDescriptionFlag                                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (Dec->SEI->SeperateColorDescriptionFlag == true) {
                    Dec->SEI->FilmGrainBitDepthLuma                                      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 3) + 8;
                    Dec->SEI->FilmGrainBitDepthChroma                                    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 3) + 8;
                    Dec->SEI->FilmGrainFullRangeFlag                                     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                    Dec->SEI->FilmGrainColorPrimaries                                    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 8);
                    Dec->SEI->FilmGrainTransferCharacteristics                           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 8);
                    Dec->SEI->FilmGrainMatrixCoefficents                                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 8);
                }
                Dec->SEI->BlendingModeID                                                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 2);
                Dec->SEI->Scalefactor                                                    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 4);
                for (uint8_t Channel = 0; Channel < 3; Channel++) {
                    Dec->SEI->CompModelPresent[Channel]                                  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                }
                for (uint8_t Channel = 0; Channel < 3; Channel++) {
                    if (Dec->SEI->CompModelPresent[Channel] == true) {
                        Dec->SEI->NumIntensityIntervals[Channel]                         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 8) + 1;
                        Dec->SEI->NumModelValues[Channel]                                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 3) + 1;
                        for (uint16_t Intensity = 0; Intensity < Dec->SEI->NumIntensityIntervals[Channel]; Intensity++) {
                            Dec->SEI->IntensityIntervalLowerBound[Channel][Intensity]    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 8);
                            Dec->SEI->IntensityIntervalUpperBound[Channel][Intensity]    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 8);
                            for (uint8_t ModelValue = 0; ModelValue < Dec->SEI->NumModelValues[Channel]; ModelValue++) {
                                Dec->SEI->CompModelValue[Channel][Intensity][ModelValue] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                            }
                        }
                    }
                }
                Dec->SEI->FilmGrainCharacteristicsRepetitionPeriod                       = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            }
        }
    }

    void ParseSEIDeblockingFilterDisplayPreference(DecodeAVC *Dec, BitBuffer *BitB) { // deblocking_filter_display_preference
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->DeblockingDisplayPreferenceCancelFlag           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->SEI->DeblockingDisplayPreferenceCancelFlag == false) {
                Dec->SEI->DisplayBeforeDeblockingFlag                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                Dec->SEI->DecodeFrameBufferingConstraintFlag          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                Dec->SEI->DeblockingDisplayPreferenceRepetitionPeriod = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            }
        }
    }

    void ParseSEIStereoVideoInfo(DecodeAVC *Dec, BitBuffer *BitB) { // stereo_video_info
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->FieldViewsFlag               = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->SEI->FieldViewsFlag == true) {
                Dec->SEI->TopFieldIsLeftViewFlag       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            } else {
                Dec->SEI->CurrentFrameIsLeftViewFlag   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                Dec->SEI->NextFrameIsDependentViewFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            }
            Dec->SEI->LeftViewSelfContainedFlag    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            Dec->SEI->RightViewSelfContainedFlag   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
        }
    }

    void ParseSEIPostFilterHint(DecodeAVC *Dec, BitBuffer *BitB) { // post_filter_hint
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->FilterHintSizeY     = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            Dec->SEI->FilterHintSizeX     = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            Dec->SEI->FilterHintType      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 2);
            for (uint8_t Color = 0; Color < 3; Color++) {
                for (uint8_t CY = 0; CY < Dec->SEI->FilterHintSizeY; CY++) {
                    for (uint8_t CX = 0; CX < Dec->SEI->FilterHintSizeX; CX++) {
                        Dec->SEI->FilterHint[Color][CY][CX] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                    }
                }
            }
            Dec->SPS->AdditionalExtensionFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
        }
    }

    void ParseSEIToneMappingInfo(DecodeAVC *Dec, BitBuffer *BitB) { // tone_mapping_info
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, , "Pointer to BitBuffer is NULL");
        } else {
            Dec->SEI->ToneMapID  = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            Dec->SEI->ToneMapCancelFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->SEI->ToneMapCancelFlag == false) {
                Dec->SEI->ToneMapRepetitionPeriod = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                Dec->SEI->CodedDataBitDepth       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 8);
                Dec->SEI->TargetBitDepth   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 8);
                Dec->SEI->ToneMapModelID = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                if (Dec->SEI->ToneMapModelID == 0) {
                    Dec->SEI->ToneMinValue = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 32);
                    Dec->SEI->ToneMaxValue = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 32);
                } else if (Dec->SEI->ToneMapModelID == 1) {
                    Dec->SEI->SigmoidMidpoint = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 32);
                    Dec->SEI->SigmoidWidth    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 32);
                } else if (Dec->SEI->ToneMapModelID == 2) {
                    for (uint16_t I = 0; I < (1 << Dec->SEI->TargetBitDepth); I++) {
                        Dec->SEI->StartOfCodedInterval[I] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, ((Dec->SEI->CodedDataBitDepth + 7) >> 3) << 3);
                    }
                } else if (Dec->SEI->ToneMapModelID == 3) {
                    Dec->SEI->NumPivots = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
                    for (uint16_t I = 0; I < Dec->SEI->NumPivots; I++) {
                        Dec->SEI->CodedPivotValue[I] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, ((Dec->SEI->CodedDataBitDepth + 7) >> 3) << 3);
                        Dec->SEI->TargetPivotValue[I] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, ((Dec->SEI->TargetBitDepth + 7) >> 3) << 3);
                    }
                } else if (Dec->SEI->ToneMapModelID == 4) {
                    Dec->SEI->CameraISOSpeedIDC = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 8);
                    if (Dec->SEI->CameraISOSpeedIDC == ExtendedISO) {
                        Dec->SEI->CameraISOSpeed = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 32);
                    }
                    Dec->SEI->ExposureIndexIDC = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 8);
                    if (Dec->SEI->ExposureIndexIDC == ExtendedISO) {
                        Dec->SEI->ExposureIndexValue = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 32);
                    }
                    Dec->SEI->ExposureCompensationSignFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                    Dec->SEI->ExposureCompensationNumerator = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
                    Dec->SEI->ExposureCompensationDenominatorIDC = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
                    Dec->SEI->RefScreenLuminanceWhite = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 32);
                    Dec->SEI->ExtendedRangeWhiteLevel = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 32);
                    Dec->SEI->NominalBlackLumaCode    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
                    Dec->SEI->NominalWhiteLumaCode    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
                    Dec->SEI->ExtendedWhiteLumaCode   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
                }
            }
        }
    }

    void ParseSEIScalabilityInfo(DecodeAVC *Dec, BitBuffer *BitB) { // scalability_info
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->TemporalIDNestingFlag                      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            Dec->SEI->PriorityLayerInfoPresent                   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            Dec->SEI->PriorityIDSettingFlag                      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            Dec->SEI->NumLayers                                  = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
            for (uint8_t Layer = 0; Layer < Dec->SEI->NumLayers; Layer++) {
                Dec->SEI->LayerID[Layer]                         = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                Dec->NAL->PriorityID[Layer]                      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 6);
                Dec->NAL->IsDisposable[Layer]                    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                Dec->NAL->DependencyID[Layer]                    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 3);
                Dec->NAL->QualityID[0][Layer]                    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 4); // FIXME; is this correct?
                Dec->NAL->TemporalID[Layer]                      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 3);
                Dec->SEI->SubPicLayerFlag[Layer]                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                Dec->SEI->SubRegionLayerFlag[Layer]              = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                Dec->SEI->IROIDivisionFlag[Layer]                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                Dec->SEI->ProfileLevelInfoPresentFlag[Layer]     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                Dec->SEI->BitRateInfoPresent[Layer]              = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                Dec->SEI->FrameRateInfoPresent[Layer]            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                Dec->SEI->FrameSizeInfoPresentFlag[Layer]        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                Dec->SEI->LayerDependencyInfoPresent[Layer]      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                Dec->SEI->ParameterSetsInfoPresent[Layer]        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                Dec->SEI->BitstreamRestrictionInfoPresent[Layer] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                Dec->SEI->ExactInterLayerPredFlag[Layer]         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if ((Dec->SEI->SubPicLayerFlag[Layer] == true) || (Dec->SEI->IROIDivisionFlag[Layer] == true)) {
                    Dec->SEI->ExactSampleValueMatchFlag[Layer]   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                }
                Dec->SEI->LayerConversionFlag[Layer]             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                Dec->SEI->LayerOutputFlag[Layer]                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (Dec->SEI->ProfileLevelInfoPresentFlag[Layer] == true) {
                    Dec->SEI->LayerProfileLevelID[Layer]         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 24);
                }
                if (Dec->SEI->BitRateInfoPresent[Layer] == true) {
                    Dec->SEI->AvgBitRate[Layer]                  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
                    Dec->SEI->MaxBitRate[Layer]                  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
                    Dec->SEI->MaxBitRateRepresentation[Layer]    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
                    Dec->SEI->MaxBitRateWindow[Layer]            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
                }
                if (Dec->SEI->FrameRateInfoPresent[Layer] == true) {
                    Dec->SEI->ConstantFrameRateIDC[Layer]        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 2);
                    Dec->SEI->AvgFrameRate[Layer]                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
                }
                if ((Dec->SEI->FrameSizeInfoPresentFlag[Layer] == true) || (Dec->SEI->IROIDivisionFlag[Layer] == true)) {
                    Dec->SEI->FrameWidthInMacroBlocks[Layer]     = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                    Dec->SEI->FrameHeightInMacroBlocks[Layer]    = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                }
                if (Dec->SEI->SubRegionLayerFlag[Layer] == true) {
                    Dec->SEI->BaseRegionLayerID[Layer]           = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    Dec->SEI->DynamicRectFlag[Layer]             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                    if (Dec->SEI->DynamicRectFlag[Layer] == false) {
                        Dec->SEI->HorizontalOffset[Layer]        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
                        Dec->SEI->VerticalOffset[Layer]          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
                        Dec->SEI->RegionWidth[Layer]             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
                        Dec->SEI->RegionHeight[Layer]            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
                    }
                }
                if (Dec->SEI->SubPicLayerFlag[Layer] == true) {
                    Dec->SEI->ROIID[Layer]                       = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                }
                if (Dec->SEI->IROIDivisionFlag[Layer] == true) {
                    Dec->SEI->IROIGridFlag[Layer]                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                    if (Dec->SEI->IROIGridFlag[Layer] == true) {
                        Dec->SEI->GridWidthInMacroBlocks[Layer]  = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                        Dec->SEI->GridHeightInMacroBlocks[Layer] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                    } else {
                        Dec->SEI->NumROIs[Layer]                       = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                        for (uint8_t ROI = 0; ROI < Dec->SEI->NumROIs[Layer]; ROI++) {
                            Dec->SEI->FirstMacroBlockInROI[Layer][ROI] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                            Dec->SEI->ROIWidthInMacroBlock[Layer][ROI] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                            Dec->SEI->ROIHeightInMacroBlock[Layer][ROI] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;

                        }
                    }
                }
                if (Dec->SEI->LayerDependencyInfoPresent[Layer] == true) {
                    Dec->SEI->NumDependentLayers[Layer]          = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    for (uint8_t DependentLayer = 0; DependentLayer < Dec->SEI->NumDependentLayers[Layer]; DependentLayer++) {
                        Dec->SEI->DirectlyDependentLayerIDDelta[Layer][DependentLayer] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                    }
                } else {
                    Dec->SEI->LayerDepInfoSourceLayerIDDelta[Layer] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                    if (Dec->SEI->ParameterSetsInfoPresent[Layer] == true) {
                        Dec->SEI->NumSequenceParameterSets[Layer] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                        for (uint8_t SPS = 0; SPS < Dec->SEI->NumSequenceParameterSets[Layer]; SPS++) {
                            Dec->SEI->SPSIDDelta[Layer][SPS] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                        }
                        Dec->SEI->NumSubsetSPS[Layer]    = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                        for (uint8_t SubsetSPS = 0; SubsetSPS < Dec->SEI->NumSubsetSPS[Layer]; SubsetSPS++) {
                            Dec->SEI->SubsetSPSIDDelta[Layer][SubsetSPS] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                        }
                        Dec->SEI->NumPicParameterSets[Layer] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                        for (uint8_t PicParameterSet = 0; PicParameterSet < Dec->SEI->NumPicParameterSets[Layer]; PicParameterSet++) {
                            Dec->SEI->PicParameterSetIDDelta[Layer][PicParameterSet] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                        }
                    } else {
                        Dec->SEI->PPSInfoSrcLayerIDDelta[Layer] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    }
                    if (Dec->SEI->BitstreamRestrictionInfoPresent[Layer] == true) {
                        Dec->VUI->MotionVectorsOverPicBoundaries[Layer] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                        Dec->VUI->MaxBytesPerPicDenom[Layer]            = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                        Dec->VUI->MaxBitsPerMarcoBlockDenom[Layer]      = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                        Dec->VUI->MaxMotionVectorLength[Layer]          = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                        Dec->VUI->MaxMotionVectorHeight[Layer]          = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                        Dec->VUI->MaxReorderFrames[Layer]               = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                        Dec->VUI->MaxFrameBuffer[Layer]                 = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    }
                    if (Dec->SEI->LayerConversionFlag[Layer] == true) {
                        Dec->SEI->ConversionTypeIDC[Layer]              = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                        for (uint8_t J = 0; J < 2; J++) {
                            Dec->SEI->RewritingInfoFlag[Layer][J]       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                            if (Dec->SEI->RewritingInfoFlag[Layer][J] == true) {
                                Dec->SEI->RewritingProfileLevelIDC[Layer][J] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 24);
                                Dec->SEI->RewritingAverageBitrate[Layer][J]  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
                                Dec->SEI->RewritingMaxBitrate[Layer][J]      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
                            }
                        }
                    }
                }
            }
            if (Dec->SEI->PriorityLayerInfoPresent == true) {
                Dec->SEI->NumDependencyLayersForPriorityLayer = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1) + 1;
                for (uint8_t DependentLayer = 0; DependentLayer < Dec->SEI->NumDependencyLayersForPriorityLayer; DependentLayer++) {
                    Dec->SEI->PriorityDependencyID[DependentLayer] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 3);
                    Dec->SEI->NumPriorityLayers[DependentLayer]    = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                    for (uint8_t PriorityLayer = 0; PriorityLayer < Dec->SEI->NumPriorityLayers[DependentLayer]; PriorityLayer++) {
                        Dec->SEI->PriorityLayerID[DependentLayer][PriorityLayer]   = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                        Dec->SEI->PriorityLevelIDC[DependentLayer][PriorityLayer]  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 24);
                        Dec->SEI->PriorityLevelAvgBitrate[DependentLayer][PriorityLayer]  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
                        Dec->SEI->PriorityLevelMaxBitrate[DependentLayer][PriorityLayer]  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
                    }
                }
            }
            if (Dec->SEI->PriorityIDSettingFlag == true) {
                Dec->SEI->PriorityIDURIIndex = 0;
                Dec->SEI->PriorityIDSettingURI = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 8);
                for (uint8_t StringByte = Dec->SEI->PriorityIDSettingURI; StringByte > 0; StringByte--) {
                    Dec->SEI->PriorityIDSettingURIString[StringByte] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 8);
                }
            }
        }
    }

    void ParseSEISubPictureScalableLayer(DecodeAVC *Dec, BitBuffer *BitB) { // sub_pic_scalable_layer
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->LayerID[0] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
        }
    }

    void ParseSEINonRequiredLayerRep(DecodeAVC *Dec, BitBuffer *BitB) { // non_required_layer_rep
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->NumInfoEntries = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
            for (uint32_t InfoEntry = 0; InfoEntry < Dec->SEI->NumInfoEntries; InfoEntry++) {
                Dec->SEI->EntryDependencyID[InfoEntry] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 3);
                Dec->SEI->NumNonRequiredLayerReps[InfoEntry] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                for (uint8_t LayerRep = 0; LayerRep < Dec->SEI->NumNonRequiredLayerReps[InfoEntry]; LayerRep++) {
                    Dec->SEI->NonRequiredLayerRepDependencyID[InfoEntry][LayerRep] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 3);
                    Dec->SEI->NonRequiredLayerRepQualityID[InfoEntry][LayerRep] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 4);
                }
            }
        }
    }

    void ParseSEIPriorityLayerInfo(DecodeAVC *Dec, BitBuffer *BitB) { // priority_layer_info
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->PriorityDependencyID[0] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 3);
            Dec->SEI->NumPriorityIDs          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 4);
            for (uint8_t PriorityID = 0; PriorityID < Dec->SEI->NumPriorityIDs; PriorityID++) {
                Dec->SEI->AltPriorityIDs[PriorityID] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 6);
            }
        }
    }

    void ParseSEILayersNotPresent(DecodeAVC *Dec, BitBuffer *BitB) { // layers_not_present
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->NumLayersNotPresent = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            for (uint8_t Layer = 0; Layer < Dec->SEI->NumLayersNotPresent; Layer++) {
                Dec->SEI->LayerID[Layer] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            }
        }
    }

    void ParseSEILayerDependencyChange(DecodeAVC *Dec, BitBuffer *BitB) { // layer_dependency_change
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->NumLayers = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
            for (uint8_t Layer = 0; Layer < Dec->SEI->NumLayers; Layer++) {
                Dec->SEI->LayerID[Layer] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                Dec->SEI->LayerDependencyInfoPresent[Layer] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (Dec->SEI->LayerDependencyInfoPresent[Layer] == true) {
                    Dec->SEI->NumDependentLayers[Layer] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    for (uint8_t DependentLayer = 0; DependentLayer < Dec->SEI->NumDependentLayers[Layer]; DependentLayer++) {
                        Dec->SEI->DirectlyDependentLayerIDDelta[Layer][DependentLayer] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                    }
                } else {
                    Dec->SEI->LayerDepInfoSourceLayerIDDelta[Layer] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                }
            }
        }
    }

    void ParseSEIScalableNesting(DecodeAVC *Dec, BitBuffer *BitB) { // scalable_nesting
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->AllLayerRepresentationsInAUFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->SEI->AllLayerRepresentationsInAUFlag == false) {
                Dec->SEI->NumLayerRepresentations = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                for (uint8_t LayerRepresentation = 0; LayerRepresentation < Dec->SEI->NumLayerRepresentations; LayerRepresentation++) {
                    Dec->SEI->SEIDependencyID[LayerRepresentation] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 3);
                    Dec->SEI->SEIQualityID[LayerRepresentation]    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 4);
                }
                Dec->SEI->SEITemporalID[0] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 3);
            }
            BitBuffer_Align(BitB, 1);
            ParseSEIMessage(Dec, BitB); // sei_message();
        }
    }

    void ParseSEIBaseLayerTemporalHRD(DecodeAVC *Dec, BitBuffer *BitB) { // base_layer_temporal_hrd
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->NumTemporalLayersInBaseLayer                 = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
            for (uint8_t TemporalLayer = 0; TemporalLayer < Dec->SEI->NumTemporalLayersInBaseLayer; TemporalLayer++) {
                Dec->SEI->SEITemporalID[TemporalLayer]             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 3);
                Dec->SEI->SEITimingInfoPresent[TemporalLayer]      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (Dec->SEI->SEITimingInfoPresent[TemporalLayer] == true) {
                    Dec->SEI->SEIUnitsInTick[TemporalLayer]        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 32);
                    Dec->SEI->SEITimeScale[TemporalLayer]          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 32);
                    Dec->SEI->SEIFixedFrameRateFlag[TemporalLayer] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                }
                Dec->SEI->SEINALHRDParamsPresentFlag[TemporalLayer] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (Dec->SEI->SEINALHRDParamsPresentFlag[TemporalLayer] == true) {
                    ParseHypotheticalReferenceDecoder(Dec, BitB);
                }
                Dec->SEI->SEIVCLHRDParamsPresentFlag[TemporalLayer] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (Dec->SEI->SEIVCLHRDParamsPresentFlag[TemporalLayer] == true) {
                    ParseHypotheticalReferenceDecoder(Dec, BitB);
                }
                if ((Dec->SEI->SEINALHRDParamsPresentFlag[TemporalLayer] == true) || (Dec->SEI->SEIVCLHRDParamsPresentFlag[TemporalLayer] == true)) {
                    Dec->SEI->SEILowDelayHRDFlag[TemporalLayer] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                }
                Dec->SEI->SEIPicStructPresentFlag[TemporalLayer] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            }
        }
    }

    void ParseSEIQualityLayerIntegrityCheck(DecodeAVC *Dec, BitBuffer *BitB) { // quality_layer_integrity_check
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->NumInfoEntries = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
            for (uint8_t IntegrityCheck = 0; IntegrityCheck < Dec->SEI->NumInfoEntries; IntegrityCheck++) {
                Dec->SEI->EntryDependencyID[IntegrityCheck] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 3);
                Dec->SEI->SEIQualityLayerCRC[IntegrityCheck] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
            }
        }
    }

    void ParseSEIRedundantPicProperty(DecodeAVC *Dec, BitBuffer *BitB) { // redundant_pic_property
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->NumDependencyIDs = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
            for (uint8_t DependencyID = 0; DependencyID < Dec->SEI->NumDependencyIDs; DependencyID++) {
                Dec->NAL->DependencyID[DependencyID]  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 3);
                Dec->SEI->NumQualityIDs[DependencyID] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                for (uint8_t QualityID = 0; QualityID < Dec->SEI->NumQualityIDs[DependencyID]; QualityID++) {
                    Dec->NAL->QualityID[DependencyID][QualityID] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 4);
                    Dec->SEI->NumRedundantPics[DependencyID][QualityID] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                    for (uint8_t RedundantPic = 0; RedundantPic < Dec->SEI->NumRedundantPics[DependencyID][QualityID]; RedundantPic++) {
                        Dec->SEI->RedundantPicCount[DependencyID][QualityID][RedundantPic] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                        Dec->SEI->RedundantPicsMatch[DependencyID][QualityID][RedundantPic] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                        if (Dec->SEI->RedundantPicsMatch[DependencyID][QualityID][RedundantPic] == false) {
                            Dec->SEI->MBTypeMatchFlag[DependencyID][QualityID][RedundantPic] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                            Dec->SEI->MotionMatchFlag[DependencyID][QualityID][RedundantPic] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                            Dec->SEI->ResidualMatchFlag[DependencyID][QualityID][RedundantPic] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                            Dec->SEI->IntraSamplesMatchFlag[DependencyID][QualityID][RedundantPic] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                        }
                    }
                }
            }
        }
    }

    void ParseSEITemporalDependencyRepresentationIndex(DecodeAVC *Dec, BitBuffer *BitB) { // tl0_dep_rep_index
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->TemporalDependencyRepresentationIndexLevel0 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 8);
            Dec->SEI->EffectiveIDRPicID                           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
        }
    }

    void ParseSEITemporalLevelSwitchingPoint(DecodeAVC *Dec, BitBuffer *BitB) { // tl_switching_point
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->DeltaFrameNum = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
        }
    }

    void ParseSEIParallelDecodingInfo(DecodeAVC *Dec, BitBuffer *BitB) { // parallel_decoding_info
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SPS->SeqParamSetID = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            for (uint8_t View = 0; View <= Dec->SPS->ViewCount; View++) {
                if (Dec->NAL->IsAnchorPicture == true) {
                    for (uint8_t AnchorRef = 0; AnchorRef < Dec->SPS->AnchorRefsCount[0][View]; AnchorRef++) {
                        Dec->SEI->PDIInitDelayAnchor[0][View][AnchorRef] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 2;
                    }
                    for (uint8_t AnchorRef = 0; AnchorRef < Dec->SPS->AnchorRefsCount[1][View]; AnchorRef++) {
                        Dec->SEI->PDIInitDelayAnchor[1][View][AnchorRef] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 2;
                    }
                } else {
                    for (uint8_t NonAnchorRef = 0; NonAnchorRef < Dec->SPS->NonAnchorRefCount[0][View]; NonAnchorRef++) {
                        Dec->SEI->PDIInitDelayNonAnchor[0][View][NonAnchorRef] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 2;
                    }
                    for (uint8_t NonAnchorRef = 0; NonAnchorRef < Dec->SPS->NonAnchorRefCount[1][View]; NonAnchorRef++) {
                        Dec->SEI->PDIInitDelayNonAnchor[1][View][NonAnchorRef] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 2;
                    }
                }
            }
        }
    }

    void ParseSEIMVCScalableNesting(DecodeAVC *Dec, BitBuffer *BitB) { // mvc_scalable_nesting
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->OperationPointFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->SEI->OperationPointFlag == false) {
                Dec->SEI->AllViewComponentsInAUFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (Dec->SEI->AllViewComponentsInAUFlag == false) {
                    Dec->SEI->NumViewComponents = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                    for (uint8_t ViewComponent = 0; ViewComponent < Dec->SEI->NumViewComponents; ViewComponent++) {
                        Dec->SEI->SEIViewID[ViewComponent] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 10);
                    }
                }
            } else {
                Dec->SEI->NumViewComponentsOp = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                for (uint8_t ViewComponent = 0; ViewComponent < Dec->SEI->NumViewComponentsOp; ViewComponent++) {
                    Dec->SEI->SEIOpViewID[ViewComponent] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 10);
                }
                Dec->SEI->SEIOpTemporalID = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 3);
            }
            BitBuffer_Align(BitB, 1);
            ParseSEIMessage(Dec, BitB); // sei_message();
        }
    }

    void ParseSEIViewScalabilityInfo(DecodeAVC *Dec, BitBuffer *BitB) { // view_scalability_info FIXME: FINISH THIS FUNCTION!!!
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->NumOperationPoints                         = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
            for (uint8_t OperationPoint = 0; OperationPoint < Dec->SEI->NumOperationPoints; OperationPoint++) {
                Dec->SEI->OperationPointID[OperationPoint]       = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                Dec->NAL->PriorityID[OperationPoint]             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 5);
                Dec->NAL->TemporalID[OperationPoint]             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 3);
                Dec->SEI->NumTargetOutputViews[OperationPoint]   = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                for (uint8_t OutputView = 0; OutputView < Dec->SEI->NumTargetOutputViews[OperationPoint]; OutputView++) {
                    Dec->SPS->ViewID[OperationPoint][OutputView] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                }
                Dec->SEI->ProfileLevelInfoPresentFlag[0]         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                Dec->SEI->BitRateInfoPresent[0]                  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                Dec->SEI->FrameRateInfoPresent[0]                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (Dec->SEI->NumTargetOutputViews[OperationPoint] == false) {
                    Dec->SEI->ViewDependencyInfoPresent[OperationPoint] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                }
                Dec->SEI->ParameterSetsInfoPresent[OperationPoint]        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                Dec->SEI->BitstreamRestrictionInfoPresent[OperationPoint] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (Dec->SEI->ProfileLevelInfoPresentFlag[0] == true) {
                    Dec->SEI->OpProfileLevelIDC[OperationPoint]           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 24);
                }
                if (Dec->SEI->BitstreamRestrictionInfoPresent[OperationPoint] == true) {
                    Dec->SEI->AvgBitRate[OperationPoint]                  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
                    Dec->SEI->MaxBitRate[OperationPoint]                  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
                    Dec->SEI->MaxBitRateWindow[OperationPoint]            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
                }
                if (Dec->SEI->FrameRateInfoPresent[0] == true) {
                    Dec->SEI->ConstantFrameRateIDC[OperationPoint]        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 2);
                    Dec->SEI->AvgFrameRate[OperationPoint]                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
                }
                if (Dec->SEI->ViewDependencyInfoPresent[OperationPoint] == true) {
                    Dec->SEI->NumDirectlyDependentViews[OperationPoint]   = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    for (uint8_t DirectDependentView = 0; DirectDependentView < Dec->SEI->NumDirectlyDependentViews[OperationPoint]; DirectDependentView++) {
                        Dec->SEI->DirectlyDependentViewID[OperationPoint][DirectDependentView] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    }
                } else {
                    Dec->SEI->ViewDependencyInfoSrcOpID[OperationPoint]   = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                }
                if (Dec->SEI->ParameterSetsInfoPresent[OperationPoint] == true) {
                    Dec->SEI->NumSequenceParameterSets[OperationPoint] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    for (uint8_t SPS = 0; SPS < Dec->SEI->NumSequenceParameterSets[OperationPoint]; SPS++) {
                        Dec->SEI->SPSIDDelta[OperationPoint][SPS] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    }
                    Dec->SEI->NumSubsetSPS[OperationPoint] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    for (uint8_t SubsetSPS = 0; SubsetSPS < Dec->SEI->NumSubsetSPS[OperationPoint]; SubsetSPS++) {
                        Dec->SEI->SubsetSPSIDDelta[OperationPoint][SubsetSPS] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    }
                    Dec->SEI->NumPicParameterSets[OperationPoint] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                    for (uint8_t PPS = 0; PPS < Dec->SEI->NumPicParameterSets[OperationPoint]; PPS++) {
                        Dec->SEI->PicParameterSetIDDelta[OperationPoint][PPS] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    }
                } else {
                    Dec->SEI->PPSInfoSrcLayerIDDelta[OperationPoint] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                }
                if (Dec->SEI->BitstreamRestrictionInfoPresent[OperationPoint] == true) {
                    Dec->VUI->MotionVectorsOverPicBoundaries[OperationPoint]  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                    Dec->VUI->MaxBytesPerPicDenom[OperationPoint]             = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    Dec->VUI->MaxBitsPerMarcoBlockDenom[OperationPoint]       = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    Dec->VUI->MaxMotionVectorLength[OperationPoint]           = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    Dec->VUI->MaxMotionVectorHeight[OperationPoint]           = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    Dec->VUI->MaxReorderFrames[OperationPoint]                = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    Dec->VUI->MaxFrameBuffer[OperationPoint]                  = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                }
                if (Dec->SEI->LayerConversionFlag[OperationPoint] == true) {
                    Dec->SEI->ConversionTypeIDC[OperationPoint] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    for (uint8_t J = 0; J < 2; J++) {
                        Dec->SEI->RewritingInfoFlag[OperationPoint][J] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                        for (uint8_t RewriteInfo = 0; RewriteInfo < Dec->SEI->RewritingInfoFlag[OperationPoint][J]; RewriteInfo++) {
                            Dec->SEI->RewritingProfileLevelIDC[OperationPoint][J] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 24);
                            Dec->SEI->RewritingAverageBitrate[OperationPoint][J]  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
                            Dec->SEI->RewritingMaxBitrate[OperationPoint][J]      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
                        }
                    }
                }
            }
            if (Dec->SEI->PriorityLayerInfoPresent == true) {
                Dec->SEI->NumDependencyLayersForPriorityLayer                             = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                for (uint8_t DependencyLayer = 0; DependencyLayer < Dec->SEI->NumDependencyLayersForPriorityLayer; DependencyLayer++) {
                    Dec->SEI->PriorityDependencyID[DependencyLayer]                       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 3);
                    Dec->SEI->NumPriorityLayers[DependencyLayer]                          = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                    for (uint8_t PriorityLayer = 0; PriorityLayer < Dec->SEI->NumPriorityLayers[DependencyLayer]; PriorityLayer++) {
                        Dec->SEI->PriorityLayerID[DependencyLayer][PriorityLayer]         = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                        Dec->SEI->PriorityLevelIDC[DependencyLayer][PriorityLayer]        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 24);
                        Dec->SEI->PriorityLevelAvgBitrate[DependencyLayer][PriorityLayer] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
                        Dec->SEI->PriorityLevelMaxBitrate[DependencyLayer][PriorityLayer] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
                    }
                }
            }
            if (Dec->SEI->PriorityIDSettingFlag == true) {
                Dec->SEI->PriorityIDURIIndex = 0;
                Dec->SEI->PriorityIDSettingURI = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 8);
                for (uint8_t StringByte = Dec->SEI->PriorityIDSettingURI; StringByte > 0; StringByte--) {
                    Dec->SEI->PriorityIDSettingURIString[StringByte] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 8);
                }
            }
        }
    }

    void ParseSEIMVCSceneInfo(DecodeAVC *Dec, BitBuffer *BitB) { // multiview_scene_info
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->MaxDisparity = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
        }
    }

    void ParseSEIMVCAcquisitionInfo(DecodeAVC *Dec, BitBuffer *BitB) { // multiview_acquisition_info
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SPS->ViewCount            = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
            Dec->SEI->IntrinsicParamFlag   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            Dec->SEI->ExtrinsicParamFlag   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->SEI->IntrinsicParamFlag == true) {
                Dec->SEI->IntrinsicParamsEqual = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                Dec->SEI->PrecFocalLength      = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                Dec->SEI->PrecPrincipalPoint   = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                Dec->SEI->PrecSkewFactor       = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                if (Dec->SEI->IntrinsicParamsEqual == true) {
                    Dec->SEI->NumParamSets     = 1;
                } else {
                    Dec->SEI->NumParamSets     = Dec->SPS->ViewCount;
                }
                for (uint8_t ParamSet = 0; ParamSet < Dec->SEI->NumParamSets; ParamSet++) { // FIXME: BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 0)
                    Dec->SEI->SignFocalLength[0][ParamSet]        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                    Dec->SEI->ExponentFocalLength[0][ParamSet]    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 6);
                    if (Dec->SEI->ExponentFocalLength[0][ParamSet] == 0) {
                        Dec->SEI->MantissaFocalLength[0][ParamSet] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Max(0, Dec->SEI->PrecFocalLength - 30));
                    } else {
                        Dec->SEI->MantissaFocalLength[0][ParamSet] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Max(0, Dec->SEI->ExponentFocalLength[0][ParamSet] + Dec->SEI->PrecFocalLength - 31));
                    }
                    Dec->SEI->SignFocalLength[1][ParamSet]        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                    Dec->SEI->ExponentFocalLength[1][ParamSet]    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 6);
                    if (Dec->SEI->ExponentFocalLength[1][ParamSet] == 0) {
                        Dec->SEI->MantissaFocalLength[1][ParamSet] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Max(0, Dec->SEI->PrecFocalLength - 30));
                    } else {
                        Dec->SEI->MantissaFocalLength[1][ParamSet] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Max(0, Dec->SEI->ExponentFocalLength[1][ParamSet] + Dec->SEI->PrecFocalLength - 31));
                    }
                    Dec->SEI->SignPrincipalPoint[0][ParamSet]     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                    Dec->SEI->ExponentPrincipalPoint[0][ParamSet] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 6);
                    if (Dec->SEI->ExponentFocalLength[1][ParamSet] == 0) {
                        Dec->SEI->MantissaPrincipalPoint[0][ParamSet] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Max(0, Dec->SEI->PrecPrincipalPoint - 30));
                    } else {
                        Dec->SEI->MantissaPrincipalPoint[0][ParamSet] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Max(0, Dec->SEI->ExponentPrincipalPoint[0][ParamSet] + Dec->SEI->PrecPrincipalPoint - 31));
                    }
                    Dec->SEI->SignPrincipalPoint[1][ParamSet]     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                    Dec->SEI->ExponentPrincipalPoint[1][ParamSet] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 6);
                    if (Dec->SEI->ExponentFocalLength[1][ParamSet] == 0) {
                        Dec->SEI->MantissaPrincipalPoint[1][ParamSet] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Max(0, Dec->SEI->PrecPrincipalPoint - 30));
                    } else {
                        Dec->SEI->MantissaPrincipalPoint[1][ParamSet] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Max(0, Dec->SEI->ExponentPrincipalPoint[1][ParamSet] + Dec->SEI->PrecPrincipalPoint - 31));
                    }
                    Dec->SEI->SignSkewFactor[ParamSet]            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                    Dec->SEI->ExponentSkewFactor[ParamSet]        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 6);
                    if (Dec->SEI->ExponentSkewFactor[ParamSet] == 0) {
                        Dec->SEI->MantissaSkewFactor[ParamSet] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Max(0, Dec->SEI->PrecSkewFactor - 30));
                    } else {
                        Dec->SEI->MantissaSkewFactor[ParamSet] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Max(0, Dec->SEI->ExponentSkewFactor[ParamSet] + Dec->SEI->PrecSkewFactor - 31));
                    }
                }
            }
            if (Dec->SEI->ExtrinsicParamFlag == true) {
                for (uint8_t View = 0; View < Dec->SPS->ViewCount; View++) {
                    for (uint8_t Row = 1; Row <= 3; Row++) {
                        for (uint8_t Column = 1; Column <= 3; Column++) {
                            Dec->SEI->SignR[View][Row][Column]     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                            Dec->SEI->ExponentR[View][Row][Column] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 6);
                            Dec->SEI->MantissaR[View][Row][Column] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 0);
                        }
                        Dec->SEI->SignT[View][Row] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                        Dec->SEI->ExponentT[View][Row] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 6);
                        Dec->SEI->MantissaT[View][Row] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 0);
                    }
                }
            }
        }
    }

    void ParseSEINonRequiredViewComponent(DecodeAVC *Dec, BitBuffer *BitB) { // non_required_view_component
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->NumInfoEntries                               = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
            for (uint8_t InfoEntry = 0; InfoEntry < Dec->SEI->NumInfoEntries; InfoEntry++) {
                Dec->SEI->ViewOrderIndex[InfoEntry]                = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                Dec->SEI->NumNonRequiredViewComponents[InfoEntry]  = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                for (uint8_t ViewComponent = 0; ViewComponent < Dec->SEI->NumNonRequiredViewComponents[InfoEntry]; ViewComponent++) {
                    Dec->SEI->IndexDelta[InfoEntry][ViewComponent] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                }
            }
        }
    }

    void ParseSEIViewDependencyChange(DecodeAVC *Dec, BitBuffer *BitB) { // view_dependency_change
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SPS->SeqParamSetID       = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            Dec->SEI->AnchorUpdateFlag    = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            Dec->SEI->NonAnchorUpdateFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->SEI->AnchorUpdateFlag == true) {
                for (uint8_t View = 0; View < Dec->SPS->ViewCount; View++) {
                    for (uint8_t AnchorRef = 0; AnchorRef < Dec->SPS->AnchorRefsCount[0][View]; AnchorRef++) {
                        Dec->SEI->AnchorRefFlag[0][View][AnchorRef] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                    }
                    for (uint8_t AnchorRef = 0; AnchorRef < Dec->SPS->AnchorRefsCount[1][View]; AnchorRef++) {
                        Dec->SEI->AnchorRefFlag[1][View][AnchorRef] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                    }
                }
            }
            if (Dec->SEI->NonAnchorUpdateFlag == true) {
                for (uint8_t View = 0; View < Dec->SPS->ViewCount; View++) {
                    for (uint8_t NonAnchorRef = 0; NonAnchorRef < Dec->SPS->NonAnchorRefCount[0][View][NonAnchorRef]; NonAnchorRef++) {
                        Dec->SEI->NonAnchorRefFlag[0][View][NonAnchorRef] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                    }
                    for (uint8_t NonAnchorRef = 0; NonAnchorRef < Dec->SPS->NonAnchorRefCount[1][View][NonAnchorRef]; NonAnchorRef++) {
                        Dec->SEI->NonAnchorRefFlag[1][View][NonAnchorRef] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                    }
                }
            }
        }
    }

    void ParseSEIOperationPointNotPresent(DecodeAVC *Dec, BitBuffer *BitB) { // operation_point(s)_not_present
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->NumOperationPoints = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
            for (uint8_t OperationPoint = 0; OperationPoint < Dec->SEI->NumOperationPoints; OperationPoint++) {
                Dec->SEI->OperationPointNotPresentID[OperationPoint] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            }
        }
    }

    void ParseSEIBaseViewTemporalHRD(DecodeAVC *Dec, BitBuffer *BitB) { // base_view_temporal_hrd
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->NumTemporalLayersInBaseView = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
            for (uint8_t TemporalLayer = 0; TemporalLayer < Dec->SEI->NumTemporalLayersInBaseView; TemporalLayer++) {
                Dec->SEI->SEIMVCTemporalID[TemporalLayer] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 3);
            }
        }
    }

    void ParseSEIFramePackingArrangement(DecodeAVC *Dec, BitBuffer *BitB) { // frame_packing_arrangement
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->FramePackingArrangementID = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            Dec->SEI->FramePackingArrangementCancelFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->SEI->FramePackingArrangementCancelFlag == false) {
                Dec->SEI->FramePackingArrangementType = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 7);
                Dec->SEI->QuincunxSamplingFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                Dec->SEI->ContentIntrepretationType = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 6);
                Dec->SEI->SpatialFlippingFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                Dec->SEI->FlippedFlagFrame0 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                Dec->SEI->FieldViewsFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                Dec->SEI->TheCurrentFrameIsFrame0 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                Dec->SEI->Frame0IsSelfContainedFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                Dec->SEI->Frame1IsSelfContainedFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if ((Dec->SEI->QuincunxSamplingFlag == false) && (Dec->SEI->FramePackingArrangementType != 5)) {
                    Dec->SEI->Frame0GridPositionX = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 4);
                    Dec->SEI->Frame0GridPositionY = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 4);
                    Dec->SEI->Frame1GridPositionX = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 4);
                    Dec->SEI->Frame1GridPositionY = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 4);
                }
                Dec->SEI->FramePackingArrangementReservedByte = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 8);
                Dec->SEI->FramePackingArrangementRepetitionPeriod = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            }
            Dec->SEI->FramePackingArrangementExtensionFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
        }
    }

    void ParseSEIMVCViewPosition(DecodeAVC *Dec, BitBuffer *BitB) { // multiview_view_position
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SPS->ViewCount = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
            for (uint8_t View = 0; View < Dec->SPS->ViewCount; View++) {
                Dec->SEI->ViewPosition[View] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            }
            Dec->SEI->MVCViewPositionExtensionFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
        }
    }

    void ParseSEIDisplayOrientation(DecodeAVC *Dec, BitBuffer *BitB) { // display_orientation
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->DisplayOrientationCancelFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->SEI->DisplayOrientationCancelFlag == false) {
                Dec->SEI->HorizontalFlip = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                Dec->SEI->VerticalFlip   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                Dec->SEI->CounterClockwiseRotation = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
                Dec->SEI->DisplayOrientationRepetitionPeriod = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                Dec->SEI->DisplayOrientationExtensionFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            }
        }
    }

    void ParseSEIDepthRepresentationInformation(DecodeAVC *Dec, BitBuffer *BitB) { // depth_representation_info
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->AllViewsEqual                                     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->SEI->AllViewsEqual == true) {
                Dec->SPS->ViewCount                                     = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
            } else {
                Dec->SPS->ViewCount                                     = 1;
            }
            Dec->SEI->ZNearFlag                                         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            Dec->SEI->ZFarFlag                                          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if ((Dec->SEI->ZNearFlag == true) || (Dec->SEI->ZFarFlag == true)) {
                Dec->SEI->ZAxisEqualFlag                                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (Dec->SEI->ZAxisEqualFlag == true) {
                    Dec->SEI->CommonZAxisReferenceView                  = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                }
            }
            Dec->SEI->DMinFlag                                          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            Dec->SEI->DMaxFlag                                          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            Dec->SEI->DepthRepresentationType                           = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            for (uint8_t View = 0; View < Dec->SPS->ViewCount; View++) {
                Dec->SEI->DepthInfoViewID[View]                         = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                if ((Dec->SEI->ZNearFlag == true || Dec->SEI->ZFarFlag == true) && (Dec->SEI->ZAxisEqualFlag == false)) {
                    Dec->SEI->ZAxisReferenceView[View]                  = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                }
                if (Dec->SEI->DMinFlag == true || Dec->SEI->DMaxFlag == true) {
                    Dec->SEI->DisparityReferenceView[View]              = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                }
                if (Dec->SEI->ZNearFlag == true) {
                    //depth_representation_sei_element(ZNearSign, ZNearExp, ZNearMantissa, ZNearManLen);
                    //ParseSEIDepthRepresentationElement(BitB, Dec->SEI->ZNearSign, Dec->SEI->ZNearExp, Dec->SEI->ZNearMantissa, Dec->SEI->ZNear)
                }
                if (Dec->SEI->ZFarFlag == true) {
                    //depth_representation_sei_element(ZFarSign, ZFarExp, ZFarMantissa, ZFarManLen);
                }
                if (Dec->SEI->DMinFlag == true) {
                    //depth_representation_sei_element(DMinSign, DMinExp, DMinMantissa, DMinManLen);
                }
                if (Dec->SEI->DMaxFlag == true) {
                    //depth_representation_sei_element(DMaxSign, DMaxExp, DMaxMantissa, DMaxManLen);
                }
            }
            if (Dec->SEI->DepthRepresentationType == 3) {
                Dec->SEI->DepthNonlinearRepresentation                  = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                for (uint8_t Index= 0; Index < Dec->SEI->DepthNonlinearRepresentation; Index++) {
                    Dec->SEI->DepthNonlinearRepresentationModel[Index]  = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                }
            }
        }
    }

    void ParseSEI3DReferenceDisplayInfo(DecodeAVC *Dec, BitBuffer *BitB) { // three_dimensional_reference_displays_info
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->TruncationErrorExponent                           = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            Dec->SEI->TruncatedWidthExponent                            = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            Dec->SEI->ReferenceViewingDistanceFlag                      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);

            if (Dec->SEI->ReferenceViewingDistanceFlag == true) {
                Dec->SEI->TruncatedReferenveViewingDistanceExponent     = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            }
            Dec->SEI->NumReferenceDisplays                              = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
            for (uint8_t Display= 0; Display < Dec->SEI->NumReferenceDisplays; Display++) {
                Dec->SEI->ReferenceBaselineExponent[Display]            = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                Dec->SEI->ReferenceBaselineMantissa[Display]            = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                Dec->SEI->ReferenceDisplayWidthExponent[Display]        = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                Dec->SEI->ReferenceDisplayWidthMantissa[Display]        = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                if (Dec->SEI->ReferenceViewingDistanceFlag == true) {
                    Dec->SEI->ReferenceViewingDistanceExponent[Display] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    Dec->SEI->ReferenceViewingDistanceMantissa[Display] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                }
                Dec->SEI->ShiftPresentFlag[Display]                     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (Dec->SEI->ShiftPresentFlag[Display] == true) {
                    Dec->SEI->SampleShift[Display]                      = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) - 512;
                }
            }
            Dec->SEI->ReferenceDisplays3DFlag                           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
        }
    }

    void ParseSEIDepthTiming(DecodeAVC *Dec, BitBuffer *BitB) { // depth_timing
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->PerViewDepthTimingFlag                              = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);

            if (Dec->SEI->PerViewDepthTimingFlag == true) {
                for (uint8_t View = 0; View < Dec->DPS->NumDepthViews; View++) {
                    Dec->SEI->OffsetLength[View]                          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 5) + 1;
                    Dec->SEI->DepthDisplayDelayOffsetFP[View]             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Dec->SEI->OffsetLength[View]);
                    Dec->SEI->DepthDisplayDelayOffsetDP[View]             = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Dec->SEI->OffsetLength[View]);
                }
            }
        }
    }

    void ParseSEIDepthGridPosition(DecodeAVC *Dec, BitBuffer *BitB) { // depth_grid_position()
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->DepthGridPositionXFP                                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 20);
            Dec->SEI->DepthGridPositionXDP                                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 4);
            Dec->SEI->DepthGridPositionXSignFlag                          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            Dec->SEI->DepthGridPositionYFP                                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 20);
            Dec->SEI->DepthGridPositionYDP                                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 4);
            Dec->SEI->DepthGridPositionYSignFlag                          = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
        }
    }

    void ParseSEIDepthSamplingInfo(DecodeAVC *Dec, BitBuffer *BitB) { // depth_sampling_info
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->DepthSampleWidthMul                                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
            Dec->SEI->DepthSampleWidthDP                                  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 4);
            Dec->SEI->DepthSampleHeightMul                                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
            Dec->SEI->DepthSampleHeightDP                                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 4);
            Dec->SEI->PerViewDepthTimingFlag                              = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->SEI->PerViewDepthTimingFlag == true) {
                Dec->SEI->NumDepthGridViews                               = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                for (uint8_t DepthGrid = 0; DepthGrid < Dec->SEI->NumDepthGridViews; DepthGrid++) {
                    Dec->SEI->DepthInfoViewID[DepthGrid]                  = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    ParseSEIDepthGridPosition(Dec, BitB);
                }
            } else {
                ParseSEIDepthGridPosition(Dec, BitB);
            }
        }
    }

    void ParseSEIConstrainedDepthParameterSetID(DecodeAVC *Dec, BitBuffer *BitB) { // constrained_depth_parameter_set_identifier
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->MaxDPSID                                            = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            Dec->SEI->MaxDPSIDDiff                                        = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
        }
    }

    void ParseSEIMeteringDisplayColorVolume(DecodeAVC *Dec, BitBuffer *BitB) { // mastering_display_color_volume
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            for (uint8_t Color = 0; Color < 3; Color++) {
                Dec->SEI->DisplayPrimariesX[Color]                        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
                Dec->SEI->DisplayPrimariesY[Color]                        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
            }
            Dec->SEI->WhitePointX                                         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
            Dec->SEI->WhitePointY                                         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 16);
            Dec->SEI->MaxDisplayMasteringLuminance                        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 32);
            Dec->SEI->MinDisplayMasteringLuminance                        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 32);
        }
    }

    void ParseSEIMVCDScalableNesting(DecodeAVC *Dec, BitBuffer *BitB) { // mvcd_scalable_nesting
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            Dec->SEI->OperationPointFlag                                  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->SEI->OperationPointFlag == false) {
                Dec->SEI->AllViewComponentsInAUFlag                       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (Dec->SEI->AllViewComponentsInAUFlag == false) {
                    Dec->SEI->NumViewComponents                           = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                    for (uint8_t ViewComponent = 0; ViewComponent < Dec->SEI->NumViewComponents; ViewComponent++) {
                        Dec->SEI->SEIViewID[ViewComponent]                = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 10);
                        Dec->SEI->SEIViewApplicabilityFlag[ViewComponent] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                    }
                }
            } else {
                Dec->SEI->SEIOpTextureOnlyFlag                            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                Dec->SEI->NumViewComponents                               = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                for (uint8_t ViewComponent = 0; ViewComponent < Dec->SEI->NumViewComponents; ViewComponent++) {
                    Dec->SEI->SEIOpViewID[ViewComponent]                  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 10);
                    if (Dec->SEI->SEIOpTextureOnlyFlag == false) {
                        Dec->SEI->SEIOpDepthFlag[ViewComponent]           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                        Dec->SEI->SEIOpTextureFlag[ViewComponent]         = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                    }
                }
                Dec->SEI->SEIOpTemporalID                                 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            }
            BitBuffer_Align(BitB, 1);
            ParseSEIMessage(Dec, BitB); // sei_message();
        }
    }

    void ParseSEIDepthRepresentationElement(BitBuffer *BitB, uint8_t OutSign, uint8_t OutExp, uint8_t OutMantissa, uint8_t OutManLen) { // depth_representation_sei_element
        if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            bool     DASignFlag                                           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1); // da_sign_flag
            uint8_t  DAExponent                                           = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 7); // da_exponent
            uint8_t  DAMatissaSize                                        = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 5) + 1;
            uint64_t DAMatissa                                            = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, DAMatissaSize);
        }
    }

    void SkipSEIReservedMessage(BitBuffer *BitB, size_t SEISize) { // reserved_sei_message
        if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            BitBuffer_Seek(BitB, Bytes2Bits(SEISize));
        }
    }

    void ParseSEIMessage(DecodeAVC *Dec, BitBuffer *BitB) { // sei_message
        if (Dec == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to DecodeAVC is NULL"));
        } else if (BitB == NULL) {
            Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Pointer to BitBuffer is NULL"));
        } else {
            while (BitBuffer_PeekBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8) == 0xFF) {
                BitBuffer_Seek(BitB, 8);
                Dec->SEI->SEIType += 255;
            }
            Dec->SEI->SEIType += BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 8); // last_payload_type_byte, 5

            while (BitBuffer_PeekBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, 8) == 0xFF) {
                BitBuffer_Seek(BitB, 8);
                Dec->SEI->SEISize    += 255;
            }
            Dec->SEI->SEISize += BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 8); // last_payload_size_byte, 692, emulation prevention bytes not included, but these fields are.

            switch (Dec->SEI->SEIType) { // sei_payload
                case SEI_BufferingPeriod:                         // 0
                    ParseSEIBufferingPeriod(Dec, BitB);
                    break;
                case SEI_PictureTiming:                           // 1
                    ParseSEIPictureTiming(Dec, BitB);
                    break;
                case SEI_PanScan:                                 // 2
                    ParseSEIPanScan(Dec, BitB);
                    break;
                case SEI_Filler:                                  // 3
                    ParseSEIFiller(Dec, BitB);
                    break;
                case SEI_RegisteredUserData:                      // 4
                    ParseSEIRegisteredUserData(Dec, BitB);
                    break;
                case SEI_UnregisteredUserData:                    // 5
                    ParseSEIUnregisteredUserData(Dec, BitB);
                    break;
                case SEI_RecoveryPoint:                           // 6
                    ParseSEIRecoveryPoint(Dec, BitB);
                    break;
                case SEI_RepetitiveReferencePicture:              // 7
                    ParseSEIRepetitiveReferencePicture(Dec, BitB);
                    break;
                case SEI_SparePicture:                            // 8
                    ParseSEISparePicture(Dec, BitB);
                    break;
                case SEI_SceneInformation:                        // 9
                    ParseSEISceneInfo(Dec, BitB);
                    break;
                case SEI_SubSequenceInformation:                  // 10
                    ParseSEISubSequenceInfo(Dec, BitB);
                    break;
                case SEI_SubSequenceLayerProperties:              // 11
                    ParseSEISubSequenceLayerProperties(Dec, BitB);
                    break;
                case SEI_SubSequenceProperties:                   // 12
                    ParseSEISubSequenceProperties(Dec, BitB);
                    break;
                case SEI_FullFrameFreeze:                         // 13
                    ParseSEIFullFrameFreeze(Dec, BitB);
                    break;
                case SEI_FullFrameFreezeRelease:                  // 14
                    ParseSEIFullFrameFreezeRelease(Dec, BitB);
                    break;
                case SEI_FullFrameSnapshot:                       // 15
                    ParseSEIFullFrameSnapshot(Dec, BitB);
                    break;
                case SEI_ProgressiveRefinementSegmentStart:       // 16
                    ParseSEIProgressiveRefinementSegmentStart(Dec, BitB);
                    break;
                case SEI_ProgressiveRefinementSegmentEnd:         // 17
                    ParseSEIProgressiveRefinementSegmentEnd(Dec, BitB);
                    break;
                case SEI_MotionConstrainedSliceGroup:             // 18
                    ParseSEIMotionConstrainedSliceGroupSet(Dec, BitB);
                    break;
                case SEI_FilmGrainCharacteristics:                // 19
                    ParseSEIFilmGrainCharacteristics(Dec, BitB);
                    break;
                case SEI_DeblockingFilterDisplayPreferences:      // 20
                    ParseSEIDeblockingFilterDisplayPreference(Dec, BitB);
                    break;
                case SEI_StereoVideoInformation:                  // 21
                    ParseSEIStereoVideoInfo(Dec, BitB);
                    break;
                case SEI_PostFilterHint:                          // 22
                    ParseSEIPostFilterHint(Dec, BitB);
                    break;
                case SEI_ToneMappingInformation:                  // 23
                    ParseSEIToneMappingInfo(Dec, BitB);
                    break;
                case SEI_ScalabilityInformation:                  // 24
                    ParseSEIScalabilityInfo(Dec, BitB);
                    break;
                case SEI_SubPictureScalableLayer:                 // 25
                    ParseSEISubPictureScalableLayer(Dec, BitB);
                    break;
                case SEI_NonRequiredLayerRep:                     // 26
                    ParseSEINonRequiredLayerRep(Dec, BitB);
                    break;
                case SEI_PriorityLayerInformation:                // 27
                    ParseSEIPriorityLayerInfo(Dec, BitB);
                    break;
                case SEI_LayersNotPresent:                        // 28
                    ParseSEILayersNotPresent(Dec, BitB);
                    break;
                case SEI_LayerDependencyChange:                   // 29
                    ParseSEILayerDependencyChange(Dec, BitB);
                    break;
                case SEI_ScalableNesting:                         // 30
                    ParseSEIScalableNesting(Dec, BitB);
                    break;
                case SEI_BaseLayerTemporalHRD:                    // 31
                    ParseSEIBaseLayerTemporalHRD(Dec, BitB);
                    break;
                case SEI_QualityLayerIntegrityCheck:              // 32
                    ParseSEIQualityLayerIntegrityCheck(Dec, BitB);
                    break;
                case SEI_RedundantPictureProperty:                // 33
                    ParseSEIRedundantPicProperty(Dec, BitB);
                    break;
                case SEI_TemporalL0DependencyRepresentationIndex: // 34
                    ParseSEITemporalDependencyRepresentationIndex(Dec, BitB);
                    break;
                case SEI_TemporalLevelSwitchingPoint:             // 35
                    ParseSEITemporalLevelSwitchingPoint(Dec, BitB);
                    break;
                case SEI_ParallelDecodingInformation:             // 36
                    ParseSEIParallelDecodingInfo(Dec, BitB);
                    break;
                case SEI_MVCScalableNesting:                      // 37
                    ParseSEIMVCScalableNesting(Dec, BitB);
                    break;
                case SEI_ViewScalabilityInformation:              // 38
                    ParseSEIViewScalabilityInfo(Dec, BitB);
                    break;
                case SEI_MVCSceneInformation:                     // 39
                    ParseSEIMVCSceneInfo(Dec, BitB);
                    break;
                case SEI_MVCAquisitionInformation:                // 40
                    ParseSEIMVCAcquisitionInfo(Dec, BitB);
                    break;
                case SEI_NonRequiredViewComponent:                // 41
                    ParseSEINonRequiredViewComponent(Dec, BitB);
                    break;
                case SEI_ViewDependencyChange:                    // 42
                    ParseSEIViewDependencyChange(Dec, BitB);
                    break;
                case SEI_OperationPointsNotPresent:               // 43
                    ParseSEIOperationPointNotPresent(Dec, BitB);
                    break;
                case SEI_BaseViewTemporalHRD:                     // 44
                    ParseSEIBaseViewTemporalHRD(Dec, BitB);
                    break;
                case SEI_FramePackingArrangement:                 // 45
                    ParseSEIFramePackingArrangement(Dec, BitB);
                    break;
                case SEI_MVCViewPosition:                         // 46
                    ParseSEIMVCViewPosition(Dec, BitB);
                    break;
                case SEI_DisplayOrientation:                      // 47
                    ParseSEIDisplayOrientation(Dec, BitB);
                    break;
                case SEI_MVCDScalableNesting:                     // 48
                    ParseSEIMVCDScalableNesting(Dec, BitB);
                    break;
                case SEI_MVCDViewScalabilityInformation:          // 49
                    ParseSEIViewScalabilityInfo(Dec, BitB);
                    break;
                case SEI_DepthRepresentationInformation:          // 50
                    ParseSEIDepthRepresentationInformation(Dec, BitB);
                    break;
                case SEI_3DReferenceDisplaysInformation:          // 51
                    ParseSEI3DReferenceDisplayInfo(Dec, BitB);
                    break;
                case SEI_DepthTiming:                             // 52
                    ParseSEIDepthTiming(Dec, BitB);
                    break;
                case SEI_DepthSamplingInformation:                // 53
                    ParseSEIDepthSamplingInfo(Dec, BitB);
                    break;
                case SEI_MVCConstrainedDPSIdentifier:             // 54
                    ParseSEIConstrainedDepthParameterSetID(Dec, BitB);
                    break;
                case SEI_MasteringDisplayColorVolume:             // 137
                    ParseSEIMeteringDisplayColorVolume(Dec, BitB);
                    break;
                default:
                    Log(Severity_DEBUG, PlatformIO_FunctionName, u8"Unrecognized SEIType: %d", Dec->SEI->SEIType);
                    BitBuffer_Seek(BitB, Dec->SEI->SEISize);
                    break;
            }
        }
    }

    void ParseAVC3DSliceDataExtension(DecodeAVC *Dec, BitBuffer *BitB) { // slice_data_in_3davc_extension
        if (Dec->PPS->EntropyCodingMode == Arithmetic) {
            BitBuffer_Align(BitB, 1);
            Dec->Slice->CurrentMacroBlockAddress = Dec->Slice->FirstMacroBlockInSlice * (Dec->Slice->MbaffFrameFlag + 1);
            bool     MoreDataFlag = true;
            Dec->Slice->PreviousMacroBlockSkipped = false;
            bool     RunLength = 0;
            while (MoreDataFlag == true) {
                if ((Dec->Slice->Type != SliceI1) || (Dec->Slice->Type != SliceI2) || (Dec->Slice->Type != SliceSI1) || (Dec->Slice->Type != SliceSI2)) {
                    if (Dec->PPS->EntropyCodingMode == ExpGolomb) {
                        Dec->Slice->MacroBlockSkipRun = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                        Dec->Slice->PreviousMacroBlockSkipped = (Dec->Slice->MacroBlockSkipRun > 0);
                        for (uint8_t SkippedMacroBlock = 0; SkippedMacroBlock < Dec->Slice->MacroBlockSkipRun; SkippedMacroBlock++) {
                            Dec->Slice->CurrentMacroBlockAddress = NextMacroBlockAddress(Dec, Dec->Slice->CurrentMacroBlockAddress);
                        }
                    }
                }
            }
        }
    }

    void ParseAVC3DSlice(DecodeAVC *Dec, BitBuffer *BitB) { // slice_header_in_3davc_extension
        Dec->Slice->FirstMacroBlockInSlice     = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
        Dec->Slice->Type                       = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
        Dec->PPS->PicParamSetID                = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
        if ((Dec->NAL->AVC3DExtensionFlag == true) && (Dec->NAL->AVC3DExtensionFlag == true)) {
            Dec->Slice->PreSliceHeaderSrc      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 2);
            if ((Dec->Slice->Type == SliceP1) || (Dec->Slice->Type == SliceP2) || (Dec->Slice->Type == SliceSP1) || (Dec->Slice->Type == SliceSP2) || (Dec->Slice->Type == SliceB1) || (Dec->Slice->Type == SliceB2)) {
                Dec->Slice->PreRefListSrc      = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 2);
                if (Dec->Slice->PreRefListSrc == 0) {
                    Dec->Slice->NumRefIDXActiveOverrideFlag= BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                    if (Dec->Slice->NumRefIDXActiveOverrideFlag == true) {
                        Dec->MacroBlock->NumRefIndexActiveLevel0 = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                    }
                    if ((Dec->Slice->Type == SliceB1) || (Dec->Slice->Type == SliceB2)) {
                        Dec->MacroBlock->NumRefIndexActiveLevel1 = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                    }
                    RefPicListMVCMod(Dec, BitB);
                }
            }
            if ((Dec->PPS->WeightedPrediction == true && ((Dec->Slice->Type == SliceP1) || (Dec->Slice->Type == SliceP2) || (Dec->Slice->Type == SliceSP1) || (Dec->Slice->Type == SliceSP2))) || ((Dec->PPS->WeightedBiPrediction == 1 && ((Dec->Slice->Type == SliceB1) || (Dec->Slice->Type == SliceB2))))) {

                Dec->Slice->PrePredictionWeightTableSrc = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 2);
                if (Dec->Slice->PrePredictionWeightTableSrc == false) {
                    pred_weight_table(Dec, BitB);
                }
            }
            if (Dec->NAL->NALRefIDC != 0) {
                Dec->Slice->PreDecReferencePictureMarkingSrc = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 2);
                if (Dec->Slice->PreDecReferencePictureMarkingSrc == 0) {
                    DecodeRefPicMarking(Dec, BitB);
                }
            }
            Dec->Slice->SliceQPDelta = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
        } else {
            if (Dec->SPS->SeperateColorPlane == true) {
                Dec->Slice->ColorPlaneID   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 2);
            }
            Dec->Slice->FrameNumber     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 0); // FIXME: READ VARIABLE BITS frame_num
            if (Dec->SPS->OnlyMacroBlocksInFrame == false) {
                Dec->Slice->SliceIsInterlaced = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (Dec->Slice->SliceIsInterlaced == true) {
                    Dec->Slice->SliceIsBottomField  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                }
            }
            if (Dec->Slice->IDRPicID == true) {
                Dec->Slice->IDRPicID = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            }
            if (Dec->SPS->PicOrderCount == false) {
                Dec->Slice->PictureOrderCountLSB = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Dec->SPS->MaxPicOrder);
                if ((Dec->PPS->BottomPicFieldOrderInSliceFlag == true) && (Dec->Slice->SliceIsInterlaced == false)) {
                    Dec->Slice->DeltaPictureOrderCountBottom[1] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                }
            }
            if ((Dec->SPS->PicOrderCount == true) && (Dec->SPS->DeltaPicOrder == false)) {
                Dec->Slice->DeltaPictureOrderCountBottom[0] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                if ((Dec->PPS->BottomPicFieldOrderInSliceFlag == true) && (Dec->Slice->SliceIsInterlaced == false)) {
                    Dec->Slice->DeltaPictureOrderCountBottom[1] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                }
            }
            if (Dec->PPS->RedundantPictureFlag == true) {
                Dec->PPS->RedundantPictureCount = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
            }
            if ((Dec->Slice->Type == SliceB1) || (Dec->Slice->Type == SliceB2)) {
                Dec->Slice->DirectSpatialMVPredictionFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            }
            if ((Dec->Slice->Type == SliceP1) || (Dec->Slice->Type == SliceP2) || (Dec->Slice->Type == SliceSP1) || (Dec->Slice->Type == SliceSP2) || (Dec->Slice->Type == SliceB1) || (Dec->Slice->Type == SliceB2)) {
                Dec->Slice->NumRefIDXActiveOverrideFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (Dec->Slice->NumRefIDXActiveOverrideFlag == true) {
                    Dec->MacroBlock->NumRefIndexActiveLevel0 = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                    if ((Dec->Slice->Type == SliceB1) || (Dec->Slice->Type == SliceB2)) {
                        Dec->MacroBlock->NumRefIndexActiveLevel1 = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                    }
                }
            }
            if ((Dec->NAL->NALRefIDC == NAL_AuxiliarySliceExtension) || (Dec->NAL->NALRefIDC == NAL_MVCDepthView)) {
                RefPicListMVCMod(Dec, BitB); /* specified in Annex H */
            } else {
                RefPicListMod(Dec, BitB);
            }
            if (((Dec->Slice->DepthWeightedPredictionFlag == true) && ((Dec->Slice->Type == SliceP1) || (Dec->Slice->Type == SliceP2))) || ((Dec->PPS->WeightedBiPrediction == true && (Dec->Slice->Type == SliceB1)) || (Dec->Slice->Type == SliceB2))) {
                pred_weight_table(Dec, BitB);
            }
            if (Dec->NAL->NALRefIDC != NAL_Unspecified0) {
                DecodeRefPicMarking(Dec, BitB);
            }
            if ((Dec->PPS->EntropyCodingMode == ExpGolomb) && ((Dec->Slice->Type != SliceI1) || (Dec->Slice->Type != SliceI2))) {
                Dec->Slice->CabacInitIDC = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                Dec->Slice->SliceQPDelta = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
            }
            // FIXME: Should SliceQPDelta be here?
            if ((Dec->Slice->Type == SliceSP1) || (Dec->Slice->Type == SliceSP2) || (Dec->Slice->Type == SliceSI1) || (Dec->Slice->Type == SliceSI2)) {
                if ((Dec->Slice->Type == SliceSP1) || (Dec->Slice->Type == SliceSP2)) {
                    Dec->Slice->DecodePMBAsSPSlice = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                    Dec->Slice->SliceQSDelta = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                }
                if (Dec->PPS->DeblockingFilterFlag == true) {
                    Dec->Slice->DisableDeblockingFilterIDC = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                    if (Dec->Slice->DisableDeblockingFilterIDC == false) {
                        Dec->Slice->SliceAlphaOffsetC0 = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                        Dec->Slice->SliceBetaOffset    = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                    }
                }
                if ((Dec->PPS->SliceGroups > 0) && (Dec->PPS->SliceGroupMapType >= 3) && (Dec->PPS->SliceGroupMapType <= 5)) {
                    Dec->Slice->SliceGroupChangeCycle = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Ceili(log2(Dec->PPS->PicSizeInMapUnits /  Dec->Slice->SliceGroupChangeRate)));
                }
                if (
                    (Dec->NAL->NALUnitType == NAL_MVCDepthView) &&
                    (
                     (Dec->Slice->Type == SliceI1)  ||
                     (Dec->Slice->Type == SliceI2)  ||
                     (Dec->Slice->Type == SliceSI1) ||
                     (Dec->Slice->Type == SliceSI2)
                     )
                    ) {
                        if (Dec->NAL->DepthFlag == true) {
                            Dec->Slice->DepthWeightedPredictionFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                        } else if (Dec->NAL->AVC3DExtensionFlag == true) {
                            Dec->Slice->DMVPFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                            if (Dec->Slice->SeqViewSynthesisFlag == true) {
                                Dec->Slice->SliceVSPFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                            }
                        }
                        if (Dec->SPS->AVC3DAcquisitionIDC == false && ((Dec->Slice->DepthWeightedPredictionFlag == true) || (Dec->Slice->DMVPFlag == true))) {
                            Dec->Slice->DepthParamSetID = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                        }
                    }
            }
        }
    }

    /* Scalable Video Coding */
    void ParseScalableSlice(DecodeAVC *Dec, BitBuffer *BitB) { // slice_header_in_scalable_extension
        Dec->Slice->FirstMacroBlockInSlice = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
        Dec->Slice->Type                   = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
        Dec->PPS->PicParamSetID            = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
        if (Dec->Slice->SeperateColorPlaneFlag == true) {
            Dec->Slice->ColorPlaneID       = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 2);
        }
        Dec->Slice->FrameNumber            = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
        if (Dec->SPS->OnlyMacroBlocksInFrame == true) {
            Dec->Slice->SliceIsInterlaced  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->Slice->SliceIsInterlaced == true) {
                Dec->Slice->SliceIsBottomField = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            }
        }
        if (Dec->NAL->IDRFlag == NAL_NonIDRSlice) {
            Dec->Slice->IDRPicID = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
        }
        if (Dec->SPS->PicOrderCount == 0) {
            Dec->Slice->PictureOrderCountLSB = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Dec->SPS->MaxPicOrder);
            if ((Dec->PPS->BottomPicFieldOrderInSliceFlag == true) && (Dec->Slice->SliceIsInterlaced == false)) {
                Dec->Slice->DeltaPictureOrderCountBottom[0] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
            }
        } else if ((Dec->SPS->PicOrderCount == 1) && (Dec->SPS->DeltaPicOrder == 0)) {
            Dec->Slice->DeltaPicOrderCount[0] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
            if ((Dec->PPS->BottomPicFieldOrderInSliceFlag == true) && (Dec->Slice->SliceIsInterlaced == false)) {
                Dec->Slice->DeltaPicOrderCount[1] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
            }
        }
        if (Dec->PPS->RedundantPictureFlag == true) {
            Dec->PPS->RedundantPictureCount = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
        }
        if (Dec->NAL->QualityID == 0) {
            if ((Dec->NAL->NALUnitType == NAL_AuxiliarySliceExtension) && ((Dec->Slice->Type == SliceEB1) || (Dec->Slice->Type == SliceEB2))) {
                Dec->Slice->DirectSpatialMVPredictionFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            }
            if ((Dec->NAL->NALUnitType == NAL_AuxiliarySliceExtension) && ((Dec->Slice->Type == SliceEP1) || (Dec->Slice->Type == SliceEP2) || (Dec->Slice->Type == SliceEB1) || (Dec->Slice->Type == SliceEB2))) {
                Dec->Slice->NumRefIDXActiveOverrideFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (Dec->Slice->NumRefIDXActiveOverrideFlag == true) {
                    Dec->MacroBlock->NumRefIndexActiveLevel0 = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                    if (Dec->Slice->Type == SliceEB1 || Dec->Slice->Type == SliceEB2) {
                        Dec->MacroBlock->NumRefIndexActiveLevel1 = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
                    }
                }
            }
            RefPicListMod(Dec, BitB);
            if ((Dec->NAL->NALUnitType == NAL_AuxiliarySliceExtension) && (((Dec->PPS->WeightedPrediction == true) &&((Dec->Slice->Type == SliceEP1)  || (Dec->Slice->Type == SliceEP2))) || ((Dec->PPS->WeightedBiPrediction == 1) && ((Dec->Slice->Type == SliceEB1) || (Dec->Slice->Type == SliceEB2))))) {
                if (Dec->NAL->NoInterLayerPredictionFlag == true) {
                    Dec->Slice->BasePredictionWeightTable = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                }
                if ((Dec->NAL->NoInterLayerPredictionFlag == false) || (Dec->Slice->BasePredictionWeightTable == false)) {
                    pred_weight_table(Dec, BitB);
                }
            }
            if (Dec->NAL->NALRefIDC != NAL_Unspecified0) {
                DecodeRefPicMarking(Dec, BitB);
                if (Dec->SVC->SliceHeaderRestricted == false) {
                    Dec->Slice->StoreRefBasePicFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                    if ((Dec->NAL->UseReferenceBasePictureFlag == true) || ((Dec->Slice->StoreRefBasePicFlag == true) && (Dec->NAL->IDRFlag == false))) {
                        ParseReferenceBasePictureSyntax(Dec, BitB);
                    }
                }
            }
        }
        if ((Dec->PPS->EntropyCodingMode == true) && ((Dec->Slice->Type != SliceEI1) || (Dec->Slice->Type != SliceEI2))) {
            Dec->Slice->CabacInitIDC = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
        }
        Dec->Slice->SliceQPDelta = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
        if (Dec->PPS->DeblockingFilterFlag == true) {
            Dec->Slice->DisableDeblockingFilterIDC = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            if (Dec->Slice->DisableDeblockingFilterIDC != 1) {
                Dec->Slice->SliceAlphaOffsetC0 = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                Dec->Slice->SliceBetaOffset    = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
            }
        }
        if ((Dec->PPS->SliceGroups > 0) && ((Dec->PPS->SliceGroupMapType >= 3) && (Dec->PPS->SliceGroupMapType <= 5))) {
            Dec->Slice->SliceGroupChangeCycle = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Ceili(log2(Dec->PPS->PicSizeInMapUnits /  Dec->Slice->SliceGroupChangeRate)));
        }
        if ((Dec->NAL->NoInterLayerPredictionFlag == true) && (Dec->NAL->QualityID == 0)) {
            Dec->Slice->RefLayerDQID = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            if (Dec->SVC->InterLayerDeblockingFilterPresent == true) {
                Dec->Slice->DisableInterLayerDeblocking = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                if (Dec->Slice->DisableInterLayerDeblocking != 1) {
                    Dec->Slice->InterLayerSliceAplhaC0Offset = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                    Dec->Slice->InterLayerSliceBetaOffset    = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                }
            }
            Dec->Slice->ConstrainedIntraResamplingFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->SVC->ExtendedSpatialScalabilityIDC == 2) {
                if (Dec->SPS->SeperateColorPlane == false) {
                    Dec->SPS->ChromaArrayType = Dec->SPS->ChromaFormatIDC;
                } else {
                    Dec->SPS->ChromaArrayType = ChromaBW;
                }
                if (Dec->SPS->ChromaArrayType > ChromaBW) {
                    BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                    Dec->Slice->RefLayerChromaPhaseXFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                    Dec->Slice->RefLayerChromaPhaseY     = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 2) - 1;
                }
                Dec->Slice->ScaledRefLayerLeftOffset     = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                Dec->Slice->ScaledRefLayerTopOffset      = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                Dec->Slice->ScaledRefLayerRightOffset    = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                Dec->Slice->ScaledRefLayerBottomOffset   = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
            }
        }
        if (Dec->NAL->NoInterLayerPredictionFlag == true) {
            Dec->Slice->SkipSliceFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            if (Dec->Slice->SkipSliceFlag == true) {
                Dec->Slice->MacroBlocksInSlice = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false) + 1;
            } else {
                Dec->Slice->AdaptiveBaseModeFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                if (Dec->Slice->AdaptiveBaseModeFlag == false) {
                    Dec->Slice->DefaultBaseModeFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                }
                if (Dec->Slice->DefaultBaseModeFlag == false) {
                    Dec->Slice->AdaptiveMotionPredictionFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                    if (Dec->Slice->AdaptiveMotionPredictionFlag == false) {
                        Dec->Slice->DefaultMotionPredictionFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                    }
                }
                if (Dec->Slice->AdaptiveResidualPredictionFlag == false) {
                    Dec->Slice->DefaultResidualPredictionFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                }
            }
            if (Dec->SVC->AdaptiveCoeffsPresent == true) {
                Dec->Slice->TCoefficentPredictionFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            }
        }
        if ((Dec->SVC->SliceHeaderRestricted == false) && (Dec->Slice->SkipSliceFlag == false)) {
            Dec->Slice->ScanIndexStart = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 4);
            Dec->Slice->ScanIndexEnd   = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 4);
        }
    }

    void ParseNALAuxiliarySliceExtension(DecodeAVC *Dec, BitBuffer *BitB) { // slice_layer_extension_rbsp
        if (Dec->NAL->SVCExtensionFlag == true) {
            ParseScalableSlice(Dec, BitB); // slice_header_in_scalable_extension()
            if (Dec->Slice->SkipSliceFlag == true) {
                ParseScalableSliceData(Dec, BitB); // slice_data_in_scalable_extension()
            }
        } else if (Dec->NAL->AVC3DExtensionFlag == true) {
            ParseAVC3DSlice(Dec, BitB); // specified in Annex J
        } else {
            ParseSliceHeader(Dec, BitB); // slice_header();
            ParseSliceData(Dec, BitB, (2|3|4)); // slice_data(); FIXME: Unknown Category
        }
        BitBuffer_Align(BitB, 1); // rbsp_slice_trailing_bits()
    }

    void ParseScalableSliceData(DecodeAVC *Dec, BitBuffer *BitB) { // slice_data_in_scalable_extension
        bool moreDataFlag;
        if (Dec->PPS->EntropyCodingMode == Arithmetic) {
            BitBuffer_Align(BitB, 1);
        }
        Dec->Slice->MbaffFrameFlag = (Dec->SPS->AdaptiveFrameFlag && !Dec->Slice->SliceIsInterlaced);
        Dec->Slice->CurrentMacroBlockAddress = Dec->Slice->FirstMacroBlockInSlice * (Dec->Slice->MbaffFrameFlag + 1);
        moreDataFlag = 1;
        if ((Dec->Slice->Type != SliceEI1) || (Dec->Slice->Type != SliceEI2)) {
            if (Dec->PPS->EntropyCodingMode == ExpGolomb) {
                Dec->Slice->MacroBlockSkipRun = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                Dec->Slice->PreviousMacroBlockSkipped = (Dec->Slice->MacroBlockSkipRun > 0);
                for (uint8_t MB = 0; MB < Dec->Slice->MacroBlockSkipRun; MB++) {
                    Dec->Slice->CurrentMacroBlockAddress = NextMacroBlockAddress(Dec, Dec->Slice->CurrentMacroBlockAddress);
                }
                if (Dec->Slice->MacroBlockSkipRun > 0) {
                    moreDataFlag = more_rbsp_data();
                }
            } else {
                Dec->Slice->SkipMacroBlock = ReadArithmetic(BitB, NULL, NULL, NULL, NULL); // FIXME: Arthmetic encoded variable
                moreDataFlag = !Dec->Slice->SkipMacroBlock;
            }
        }
        if (moreDataFlag == true) {
            if (Dec->Slice->MbaffFrameFlag && ((Dec->Slice->CurrentMacroBlockAddress % 2) == 0 || (((Dec->Slice->CurrentMacroBlockAddress % 2) == 1) && (Dec->Slice->PreviousMacroBlockSkipped == true)))) {
                Dec->Slice->MacroBlockFieldDecodingFlag  = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
            }
            ParseMacroBlockLayerInSVC(Dec, BitB); // macroblock_layer_in_scalable_extension();
            if (Dec->PPS->EntropyCodingMode == ExpGolomb) {
                moreDataFlag = more_rbsp_data();
            } else {
                if ((Dec->Slice->Type != SliceEI1) || (Dec->Slice->Type != SliceEI2)) {
                    Dec->Slice->PreviousMacroBlockSkipped = Dec->Slice->SkipMacroBlock;
                }
                if (Dec->Slice->MbaffFrameFlag && (Dec->Slice->CurrentMacroBlockAddress % 2) == 0) {
                    moreDataFlag = 1;
                } else {
                    Dec->Slice->IsEndOfSlice = ReadArithmetic(BitB, NULL, NULL, NULL, NULL); // arthmetic coded variable
                    moreDataFlag = !Dec->Slice->IsEndOfSlice;
                    while (Dec->Slice->IsEndOfSlice == false) {
                        BitBuffer_Seek(BitB, 1); // FIXME: Not sure f it should be 1 bit or 8 or what?
                    }
                }
            }
            Dec->Slice->CurrentMacroBlockAddress = NextMacroBlockAddress(Dec, Dec->Slice->CurrentMacroBlockAddress);
        }
    }

    void ParseUnpartitionedSliceLayer(DecodeAVC *Dec, BitBuffer *BitB) { // slice_layer_without_partitioning_rbsp
                                                                         //ParseSliceHeader(Dec, BitB);
        ParseSliceData(Dec, BitB, 0);
        rbsp_slice_trailing_bits(Dec, BitB);
    }

    void ParseMacroBlockLayerInSVC(DecodeAVC *Dec, BitBuffer *BitB) { // macroblock_layer_in_scalable_extension
        if (Dec->PPS->EntropyCodingMode == ExpGolomb) {
            if (InCropWindow(Dec, Dec->Slice->CurrentMacroBlockAddress) && (Dec->Slice->AdaptiveBaseModeFlag == true)) {
                Dec->MacroBlock->BaseModeFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1); // base_mode_flag
            }
            if (Dec->MacroBlock->BaseModeFlag == false) {
                Dec->MacroBlock->Type = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, BitB, CountUnary, AVCStopBit);
            }
            if (Dec->MacroBlock->Type == I_PCM) {
                BitBuffer_Seek(BitB, 1); // pcm_alignment_zero_bit
                CalculateMacroBlockDimensions(Dec);
                for (uint16_t Sample = 0; Sample < MacroBlockMaxPixels; Sample++) {
                    Dec->MacroBlock->PCMLumaSample[Sample] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Dec->SPS->LumaBitDepthMinus8);
                }
                for (uint8_t Channel = 0; Channel < 2; Channel++) {
                    for (uint8_t Sample = 0; Sample < (2 * (Dec->SPS->MacroBlockWidthChroma * Dec->SPS->MacroBlockHeightChroma)); Sample++) {
                        Dec->MacroBlock->PCMChromaSample[Channel][Sample] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Dec->SPS->ChromaBitDepthMinus8);
                    }
                }
            } else {
                if (Dec->MacroBlock->BaseModeFlag == false) {
                    Dec->MacroBlock->NoMBSmallerThan8x8Flag = 1;
                    if ((Dec->MacroBlock->Type != I_NxN) && (MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) != Intra_16x16) && NumMacroBlockPartitions(Dec->MacroBlock->Type) == 4) {
                        ParseSubMacroBlockPredictionInSVC(Dec, BitB);
                        for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) {
                            if (Dec->MacroBlock->SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8) {
                                if (GetNumSubMacroBlockPartitions(Dec->MacroBlock->SubMacroBlockType[MacroBlockPiece]) > 1) {
                                    Dec->MacroBlock->NoMBSmallerThan8x8Flag = 0;
                                }
                            } else if (Dec->SPS->Inference8x8 == false) {
                                Dec->MacroBlock->NoMBSmallerThan8x8Flag = 0;
                            }
                        }
                    } else {
                        if (Dec->PPS->TransformIs8x8 == true && Dec->MacroBlock->Type == I_NxN) {
                            Dec->MacroBlock->TransformSizeIs8x8 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                        }
                        ParseMBPredictionInSVC(Dec, BitB);
                    }
                }
                if ((Dec->Slice->AdaptiveResidualPredictionFlag == true) && ((Dec->Slice->Type != SliceEI1) || (Dec->Slice->Type != SliceEI2)) && InCropWindow(Dec, Dec->Slice->CurrentMacroBlockAddress) && (Dec->MacroBlock->BaseModeFlag || (MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) != Intra_16x16 && MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) != Intra_8x8 && MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) != Intra_4x4))) {
                    Dec->Slice->ResidualPredictionFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                }
                if (Dec->Slice->ScanIndexEnd >= Dec->Slice->ScanIndexStart) {
                    if (Dec->MacroBlock->BaseModeFlag || MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) != Intra_16x16) {
                        Dec->MacroBlock->BlockPattern = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, BitB, CountUnary, AVCStopBit); // MappedExp
                        if (Dec->MacroBlock->BlockPatternLuma > 0 && Dec->PPS->TransformIs8x8 && (Dec->MacroBlock->BaseModeFlag || (Dec->MacroBlock->Type != I_NxN && Dec->MacroBlock->NoMBSmallerThan8x8Flag && (Dec->MacroBlock->Type != B_Direct_16x16 || Dec->SPS->Inference8x8)))) {
                            Dec->MacroBlock->TransformSizeIs8x8 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                        }
                    }
                    if (Dec->MacroBlock->BlockPatternLuma > 0 || Dec->MacroBlock->BlockPatternChroma > 0 || MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) == Intra_16x16) {
                        Dec->MacroBlock->QPDelta = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, BitB, CountUnary, AVCStopBit);
                        residual(Dec->Slice->ScanIndexStart, Dec->Slice->ScanIndexEnd);
                    }
                }
            }
        } else if (Dec->PPS->EntropyCodingMode == Arithmetic) {
            if (InCropWindow(Dec, Dec->Slice->CurrentMacroBlockAddress) && (Dec->Slice->AdaptiveBaseModeFlag == true)) {
                Dec->MacroBlock->BaseModeFlag = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
            }
            if (Dec->MacroBlock->BaseModeFlag == false) {
                Dec->MacroBlock->Type = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
            }
            if (Dec->MacroBlock->Type == I_PCM) {
                BitBuffer_Seek(BitB, 1);
                CalculateMacroBlockDimensions(Dec);
                for (uint16_t Sample = 0; Sample < MacroBlockMaxPixels; Sample++) {
                    Dec->MacroBlock->PCMLumaSample[Sample] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Dec->SPS->LumaBitDepthMinus8);
                }
                for (uint8_t Channel = 0; Channel < 2; Channel++) {
                    for (uint8_t Sample = 0; Sample < (2 * (Dec->SPS->MacroBlockWidthChroma * Dec->SPS->MacroBlockHeightChroma)); Sample++) {
                        Dec->MacroBlock->PCMChromaSample[Channel][Sample] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Dec->SPS->ChromaBitDepthMinus8);
                    }
                }
            } else {
                if (Dec->MacroBlock->BaseModeFlag == false) {
                    Dec->MacroBlock->NoMBSmallerThan8x8Flag = 1;
                    if ((Dec->MacroBlock->Type != I_NxN) && (MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) != Intra_16x16) && NumMacroBlockPartitions(Dec->MacroBlock->Type) == 4) {
                        ParseSubMacroBlockPredictionInSVC(Dec, BitB);
                        for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) {
                            if (Dec->MacroBlock->SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8) {
                                if (GetNumSubMacroBlockPartitions(Dec->MacroBlock->SubMacroBlockType[MacroBlockPiece]) > 1) {
                                    Dec->MacroBlock->NoMBSmallerThan8x8Flag = 0;
                                }
                            } else if (Dec->SPS->Inference8x8 == false) {
                                Dec->MacroBlock->NoMBSmallerThan8x8Flag = 0;
                            }
                        }
                    } else {
                        if (Dec->PPS->TransformIs8x8 == true && Dec->MacroBlock->Type == I_NxN) {
                            Dec->MacroBlock->TransformSizeIs8x8 = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                        ParseMBPredictionInSVC(Dec, BitB);
                    }
                }
                if ((Dec->Slice->AdaptiveResidualPredictionFlag == true) && ((Dec->Slice->Type != SliceEI1) || (Dec->Slice->Type != SliceEI2)) && InCropWindow(Dec, Dec->Slice->CurrentMacroBlockAddress) && (Dec->MacroBlock->BaseModeFlag || (MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) != Intra_16x16 && MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) != Intra_8x8 && MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) != Intra_4x4))) {
                    Dec->Slice->ResidualPredictionFlag = Dec->PPS->EntropyCodingMode = ExpGolomb ? ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, BitB, CountUnary, AVCStopBit) : ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                }
                if (Dec->Slice->ScanIndexEnd >= Dec->Slice->ScanIndexStart) {
                    if (Dec->MacroBlock->BaseModeFlag || MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) != Intra_16x16) {
                        Dec->MacroBlock->BlockPattern = ReadArithmetic(BitB, NULL, NULL, NULL, NULL); // ME or Arithmetic
                        if (Dec->MacroBlock->BlockPatternLuma > 0 && Dec->PPS->TransformIs8x8 && (Dec->MacroBlock->BaseModeFlag || (Dec->MacroBlock->Type != I_NxN && Dec->MacroBlock->NoMBSmallerThan8x8Flag && (Dec->MacroBlock->Type != B_Direct_16x16 || Dec->SPS->Inference8x8)))) {
                            Dec->MacroBlock->TransformSizeIs8x8 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1); // OR Arithmetic
                        }
                    }
                    if (Dec->MacroBlock->BlockPatternLuma > 0 || Dec->MacroBlock->BlockPatternChroma > 0 || MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) == Intra_16x16) {
                        Dec->MacroBlock->QPDelta = Dec->PPS->EntropyCodingMode = ExpGolomb ? ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true) : ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        residual(Dec->Slice->ScanIndexStart, Dec->Slice->ScanIndexEnd);
                    }
                }
            }
        }
    }

    void CalculateMacroBlockDimensions(DecodeAVC *Dec) {
        if ((Dec->SPS->ChromaFormatIDC == ChromaBW) && (Dec->Slice->SeperateColorPlaneFlag == false)) {
            Dec->SPS->MacroBlockWidthChroma  = 0;
            Dec->SPS->MacroBlockHeightChroma = 0;
        } else if ((Dec->SPS->ChromaFormatIDC == Chroma420) && (Dec->Slice->SeperateColorPlaneFlag == false)) {
            Dec->SPS->MacroBlockWidthChroma  = 8;
            Dec->SPS->MacroBlockHeightChroma = 8;
        } else if ((Dec->SPS->ChromaFormatIDC == Chroma422) && (Dec->Slice->SeperateColorPlaneFlag == false)) {
            Dec->SPS->MacroBlockWidthChroma  = 8;
            Dec->SPS->MacroBlockHeightChroma = 16;
        } else if ((Dec->SPS->ChromaFormatIDC == Chroma444) && (Dec->Slice->SeperateColorPlaneFlag == false)) {
            Dec->SPS->MacroBlockWidthChroma  = 16;
            Dec->SPS->MacroBlockHeightChroma = 16;
        } else if (Dec->Slice->SeperateColorPlaneFlag == true) {
            // Samples need to be handled differently.
        }
    }

    void ParseMBPredictionInSVC(DecodeAVC *Dec, BitBuffer *BitB) { // mb_pred_in_scalable_extension
        if (Dec->PPS->EntropyCodingMode == ExpGolomb) {
            if ((MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) == Intra_4x4) || (MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) == Intra_8x8) || (MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) == Intra_16x16)) {
                if (MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) == Intra_4x4) {
                    for (uint8_t luma4x4BlkIdx = 0; luma4x4BlkIdx < 16; luma4x4BlkIdx++) {
                        Dec->MacroBlock->PreviousIntra4x4PredictionFlag[luma4x4BlkIdx] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                        if (!Dec->MacroBlock->PreviousIntra4x4PredictionFlag[luma4x4BlkIdx]) {
                            Dec->MacroBlock->RemIntra4x4PredictionMode[luma4x4BlkIdx] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 3);
                        }
                    }
                }
                if (MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) == Intra_8x8) {
                    for (uint8_t luma8x8BlkIdx = 0; luma8x8BlkIdx < 4; luma8x8BlkIdx++) {
                        Dec->MacroBlock->PreviousIntra8x8PredictionFlag[luma8x8BlkIdx] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                        if (!Dec->MacroBlock->PreviousIntra8x8PredictionFlag[luma8x8BlkIdx]) {
                            Dec->MacroBlock->RemIntra8x8PredictionMode[luma8x8BlkIdx] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 3);
                        }
                    }
                }
                if (Dec->SPS->ChromaArrayType != 0) {
                    Dec->MacroBlock->IntraChromaPredictionMode = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsFarthest, BitB, CountUnary, AVCStopBit);
                }
            } else if (MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) != Direct) {
                if (InCropWindow(Dec, Dec->Slice->CurrentMacroBlockAddress) && Dec->Slice->AdaptiveMotionPredictionFlag) {
                    for (uint32_t MacroBlockPiece = 0; MacroBlockPiece < NumMacroBlockPartitions(Dec->MacroBlock->Type); MacroBlockPiece++) {
                        if (MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) != Pred_L1) {
                            Dec->MacroBlock->MotionPredictionFlagLevel0[MacroBlockPiece] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                        }
                        if (MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) != Pred_L0) {
                            Dec->MacroBlock->MotionPredictionFlagLevel1[MacroBlockPiece] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                        }
                    }
                }
                for (uint32_t MacroBlockPiece = 0; MacroBlockPiece < NumMacroBlockPartitions(Dec->MacroBlock->Type); MacroBlockPiece++) {
                    if ((Dec->MacroBlock->NumRefIndexActiveLevel0 > 0 || Dec->Slice->MacroBlockFieldDecodingFlag != Dec->Slice->SliceIsInterlaced) && MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 1) != Pred_L1 && !Dec->MacroBlock->MotionPredictionFlagLevel0[MacroBlockPiece]) {
                        Dec->MacroBlock->RefIndexLevel0[MacroBlockPiece] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                    }
                    if ((Dec->MacroBlock->NumRefIndexActiveLevel1 > 0 || Dec->Slice->MacroBlockFieldDecodingFlag != Dec->Slice->SliceIsInterlaced) && MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 1) != Pred_L0 && !Dec->MacroBlock->MotionPredictionFlagLevel1[MacroBlockPiece]) {
                        Dec->MacroBlock->RefIndexLevel1[MacroBlockPiece] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                    }
                    if (MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, MacroBlockPiece) != Pred_L1) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            Dec->MacroBlock->MVDLevel0[MacroBlockPiece][0][compIdx] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                        }
                    }
                    if (MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 1) != Pred_L0) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            Dec->MacroBlock->MVDLevel1[MacroBlockPiece][0][compIdx] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                        }
                    }
                }
            }
        } else if (Dec->PPS->EntropyCodingMode == Arithmetic) {
            if ((MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) == Intra_4x4) || (MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) == Intra_8x8) || (MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) == Intra_16x16)) {
                if (MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) == Intra_4x4) {
                    for (uint8_t luma4x4BlkIdx = 0; luma4x4BlkIdx < 16; luma4x4BlkIdx++) {
                        Dec->MacroBlock->PreviousIntra4x4PredictionFlag[luma4x4BlkIdx] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        if (!Dec->MacroBlock->PreviousIntra4x4PredictionFlag[luma4x4BlkIdx]) {
                            Dec->MacroBlock->RemIntra4x4PredictionMode[luma4x4BlkIdx] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                }
                if (MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) == Intra_8x8) {
                    for (uint8_t luma8x8BlkIdx = 0; luma8x8BlkIdx < 4; luma8x8BlkIdx++) {
                        Dec->MacroBlock->PreviousIntra8x8PredictionFlag[luma8x8BlkIdx] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        if (Dec->MacroBlock->PreviousIntra8x8PredictionFlag[luma8x8BlkIdx] == false) {
                            Dec->MacroBlock->RemIntra8x8PredictionMode[luma8x8BlkIdx] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                }
                if (Dec->SPS->ChromaArrayType != 0) {
                    Dec->MacroBlock->IntraChromaPredictionMode = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                }
            } else if (MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) != Direct) {
                if (InCropWindow(Dec, Dec->Slice->CurrentMacroBlockAddress) && Dec->Slice->AdaptiveMotionPredictionFlag) {
                    for (uint32_t MacroBlockPiece = 0; MacroBlockPiece < NumMacroBlockPartitions(Dec->MacroBlock->Type); MacroBlockPiece++) {
                        if (MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 1) != Pred_L1) {
                            Dec->MacroBlock->MotionPredictionFlagLevel0[MacroBlockPiece] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                        if (MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 1) != Pred_L0) {
                            Dec->MacroBlock->MotionPredictionFlagLevel1[MacroBlockPiece] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                }
                for (uint32_t MacroBlockPiece = 0; MacroBlockPiece < NumMacroBlockPartitions(Dec->MacroBlock->Type); MacroBlockPiece++) {
                    if ((Dec->MacroBlock->NumRefIndexActiveLevel0 > 0 || Dec->Slice->MacroBlockFieldDecodingFlag != Dec->Slice->SliceIsInterlaced) && MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 1) != Pred_L1 && !Dec->MacroBlock->MotionPredictionFlagLevel0[MacroBlockPiece]) {
                        Dec->MacroBlock->RefIndexLevel0[MacroBlockPiece] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                    }
                    if ((Dec->MacroBlock->NumRefIndexActiveLevel1 > 0 || Dec->Slice->MacroBlockFieldDecodingFlag != Dec->Slice->SliceIsInterlaced) &&
                        MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 1) != Pred_L0 && !Dec->MacroBlock->MotionPredictionFlagLevel0[MacroBlockPiece]) {
                        Dec->MacroBlock->RefIndexLevel1[MacroBlockPiece] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                    }
                    if (MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, MacroBlockPiece) != Pred_L1) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            Dec->MacroBlock->MVDLevel0[MacroBlockPiece][0][compIdx] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                    if (MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) != Pred_L0) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            Dec->MacroBlock->MVDLevel1[MacroBlockPiece][0][compIdx] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                }
            }
        }
    }

    void ParseAVC3DMacroBlockPredictionExtension(DecodeAVC *Dec, BitBuffer *BitB) {
        if (MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) == Intra_4x4) {

        } else if (MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) == Intra_8x8) {

        } else if (MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) == Intra_16x16) {

        }
    }

    uint8_t NumMacroBlockPartitions(uint8_t MacroBlockType) { // NumMbPart
        if (MacroBlockType == 0) {
            return 1;
        } else if ((MacroBlockType == 1) || (MacroBlockType == 2)) {
            return 2;
        } else if ((MacroBlockType == 3) || (MacroBlockType == 4)) {
            return 4;
        } else {
            return 1;
        }
    }

    void MacroBlockLayer(DecodeAVC *Dec, BitBuffer *BitB) { // macroblock_layer
        if (Dec->PPS->EntropyCodingMode == ExpGolomb) {
            Dec->MacroBlock->Type = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
            if (Dec->MacroBlock->Type == I_PCM) { // I_PCM
                BitBuffer_Align(BitB, 1);
                for (uint16_t i = 0; i < 256; i++) {
                    Dec->MacroBlock->PCMLumaSample[i] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Dec->SPS->LumaBitDepthMinus8);
                    for (uint8_t j = 0; j < 2; j++) { // FIXME: 2 needs to be a variable of the number of channels
                        Dec->MacroBlock->PCMChromaSample[j][i] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Dec->SPS->ChromaBitDepthMinus8);
                    }
                }
            } else { // NOT I_PCM
                Dec->MacroBlock->NoMBSmallerThan8x8Flag = true;
                uint8_t NumMacroBlockPart = NumMacroBlockPartitions(Dec->MacroBlock->Type);
                if ((Dec->MacroBlock->Type != I_NxN) && (MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) != Intra_16x16) && (NumMacroBlockPart == 4)) {
                    ParseSubMacroBlockPrediction(Dec, BitB, Dec->MacroBlock->Type);
                    for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) {
                        if (Dec->MacroBlock->SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8) {
                            if (GetNumSubMacroBlockPartitions(Dec->MacroBlock->SubMacroBlockType[MacroBlockPiece]) > 1) {
                                Dec->MacroBlock->NoMBSmallerThan8x8Flag = 0;
                            }
                        } else if (Dec->SPS->Inference8x8 == false) {
                            Dec->MacroBlock->NoMBSmallerThan8x8Flag = 0;
                        }
                    }
                } else {
                    if (Dec->PPS->TransformIs8x8 && (Dec->MacroBlock->Type == I_NxN)) {
                        Dec->MacroBlock->TransformSizeIs8x8 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                    }
                    mb_pred(Dec, BitB, Dec->MacroBlock->Type);
                }
                if (MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) != Intra_16x16) {
                    Dec->MacroBlock->BlockPattern = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false); // TODO: Add table lookup
                    if (Dec->MacroBlock->BlockPattern == true && Dec->PPS->TransformIs8x8 && (Dec->MacroBlock->Type != I_NxN) && Dec->MacroBlock->NoMBSmallerThan8x8Flag && (Dec->MacroBlock->Type != B_Direct_16x16 || Dec->SPS->Inference8x8)) {
                        Dec->MacroBlock->TransformSizeIs8x8 = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                    }
                    if (Dec->MacroBlock->BlockPatternLuma > 0 || Dec->MacroBlock->BlockPatternChroma > 0 || MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) == Intra_16x16) {
                        Dec->MacroBlock->QPDelta = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                        residual(0, 15);
                    }
                }
            }
        } else if (Dec->PPS->EntropyCodingMode == Arithmetic) {
            Dec->MacroBlock->Type = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
            if (Dec->MacroBlock->Type == I_PCM) { // I_PCM
                BitBuffer_Align(BitB, 1);
                for (uint16_t i = 0; i < 256; i++) {
                    Dec->MacroBlock->PCMLumaSample[i] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Dec->SPS->LumaBitDepthMinus8);
                    for (uint8_t j = 0; j < 2; j++) { // FIXME: 2 needs to be a variable of the number of channels
                        Dec->MacroBlock->PCMChromaSample[j][i] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, Dec->SPS->ChromaBitDepthMinus8);
                    }
                }
            } else { // NOT I_PCM
                Dec->MacroBlock->NoMBSmallerThan8x8Flag = true;
                uint8_t NumMacroBlockPart = NumMacroBlockPartitions(Dec->MacroBlock->Type);
                if ((Dec->MacroBlock->Type != I_NxN) && (MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) != Intra_16x16) && (NumMacroBlockPart == 4)) {
                    ParseSubMacroBlockPrediction(Dec, BitB, Dec->MacroBlock->Type);
                    for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) {
                        if (Dec->MacroBlock->SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8) {
                            if (GetNumSubMacroBlockPartitions(Dec->MacroBlock->SubMacroBlockType[MacroBlockPiece]) > 1) {
                                Dec->MacroBlock->NoMBSmallerThan8x8Flag = 0;
                            }
                        } else if (Dec->SPS->Inference8x8 == false) {
                            Dec->MacroBlock->NoMBSmallerThan8x8Flag = 0;
                        }
                    }
                } else {
                    if (Dec->PPS->TransformIs8x8 && (Dec->MacroBlock->Type == I_NxN)) {
                        Dec->MacroBlock->TransformSizeIs8x8 = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                    }
                    mb_pred(Dec, BitB, Dec->MacroBlock->Type);
                }
                if (MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) != Intra_16x16) {
                    Dec->MacroBlock->BlockPattern = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                    if (Dec->MacroBlock->BlockPattern == true && Dec->PPS->TransformIs8x8 && (Dec->MacroBlock->Type != I_NxN) && Dec->MacroBlock->NoMBSmallerThan8x8Flag && (Dec->MacroBlock->Type != B_Direct_16x16 || Dec->SPS->Inference8x8)) {
                        Dec->MacroBlock->TransformSizeIs8x8 = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                    }
                    if (Dec->MacroBlock->BlockPatternLuma > 0 || Dec->MacroBlock->BlockPatternChroma > 0 || MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) == Intra_16x16) {
                        Dec->MacroBlock->QPDelta = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        residual(0, 15);
                    }
                }
            }
        }
    }

    void mb_pred(DecodeAVC *Dec, BitBuffer *BitB, uint8_t mb_type) { // mb_pred
        if (Dec->PPS->EntropyCodingMode == ExpGolomb) {
            uint8_t MacroBlockPredictionMode = MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0);
            if ((MacroBlockPredictionMode == Intra_4x4) || (MacroBlockPredictionMode == Intra_8x8) || (MacroBlockPredictionMode == Intra_16x16)) {
                if (MacroBlockPredictionMode == Intra_4x4) {
                    for (uint8_t Luma4x4Block = 0; Luma4x4Block < 16; Luma4x4Block++) { // luma4x4BlkIdx
                        Dec->MacroBlock->PreviousIntra4x4PredictionFlag[Luma4x4Block] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                        if (Dec->MacroBlock->PreviousIntra4x4PredictionFlag[Luma4x4Block] == false) {
                            Dec->MacroBlock->RemIntra4x4PredictionMode[Luma4x4Block] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 3);
                        }
                    }
                }
                if (MacroBlockPredictionMode == Intra_8x8) {
                    for (uint8_t Luma8x8Block = 0; Luma8x8Block < 4; Luma8x8Block++) { // luma8x8BlkIdx
                        Dec->MacroBlock->PreviousIntra8x8PredictionFlag[Luma8x8Block] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                        if (Dec->MacroBlock->PreviousIntra8x8PredictionFlag[Luma8x8Block] == false) {
                            Dec->MacroBlock->RemIntra8x8PredictionMode[Luma8x8Block] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 3);
                        }
                    }
                }
                if ((Dec->SPS->ChromaArrayType == Chroma420) || (Dec->SPS->ChromaArrayType == Chroma422)) {
                    Dec->MacroBlock->IntraChromaPredictionMode = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, false);
                }
            } else if (MacroBlockPredictionMode != Direct) {
                for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < NumMacroBlockPartitions(Dec->MacroBlock->Type); MacroBlockPiece++) {
                    if ((Dec->MacroBlock->NumRefIndexActiveLevel0 > 0 || Dec->Slice->MacroBlockFieldDecodingFlag != Dec->Slice->SliceIsInterlaced) && MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) != Pred_L1) {
                        Dec->MacroBlock->RefIndexLevel0[MacroBlockPiece] = ReadRICE(BitB, true, 0);
                    }
                }
                for (uint32_t MacroBlockPiece = 0; MacroBlockPiece < NumMacroBlockPartitions(Dec->MacroBlock->Type); MacroBlockPiece++) {
                    if ((Dec->MacroBlock->NumRefIndexActiveLevel1 > 0 || Dec->Slice->MacroBlockFieldDecodingFlag != Dec->Slice->SliceIsInterlaced) && MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) != Pred_L0) {
                        Dec->MacroBlock->RefIndexLevel1[MacroBlockPiece] = ReadRICE(BitB, true, 0);
                    }
                }
                for (uint32_t MacroBlockPiece = 0; MacroBlockPiece < NumMacroBlockPartitions(Dec->MacroBlock->Type); MacroBlockPiece++) {
                    if (MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) != Pred_L1) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            Dec->MacroBlock->MVDLevel0[MacroBlockPiece][0][compIdx] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                        }
                    }
                }
                for (uint32_t MacroBlockPiece = 0; MacroBlockPiece < NumMacroBlockPartitions(Dec->MacroBlock->Type); MacroBlockPiece++) {
                    if (MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) != Pred_L0) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            Dec->MacroBlock->MVDLevel1[MacroBlockPiece][0][compIdx] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                        }
                    }
                }
            }
        } else if (Dec->PPS->EntropyCodingMode == Arithmetic) {
            uint8_t MacroBlockPredictionMode = MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0);
            if ((MacroBlockPredictionMode == Intra_4x4) || (MacroBlockPredictionMode == Intra_8x8) || (MacroBlockPredictionMode == Intra_16x16)) {
                if (MacroBlockPredictionMode == Intra_4x4) {
                    for (uint8_t Luma4x4Block = 0; Luma4x4Block < 16; Luma4x4Block++) { // luma4x4BlkIdx
                        Dec->MacroBlock->PreviousIntra4x4PredictionFlag[Luma4x4Block] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        if (Dec->MacroBlock->PreviousIntra4x4PredictionFlag[Luma4x4Block] == false) {
                            Dec->MacroBlock->RemIntra4x4PredictionMode[Luma4x4Block] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                }
                if (MacroBlockPredictionMode == Intra_8x8) {
                    for (uint8_t Luma8x8Block = 0; Luma8x8Block < 4; Luma8x8Block++) { // luma8x8BlkIdx
                        Dec->MacroBlock->PreviousIntra8x8PredictionFlag[Luma8x8Block] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        if (Dec->MacroBlock->PreviousIntra8x8PredictionFlag[Luma8x8Block] == false) {
                            Dec->MacroBlock->RemIntra8x8PredictionMode[Luma8x8Block] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                }
                if ((Dec->SPS->ChromaArrayType == Chroma420) || (Dec->SPS->ChromaArrayType == Chroma422)) {
                    Dec->MacroBlock->IntraChromaPredictionMode = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                }
            } else if (MacroBlockPredictionMode != Direct) {
                for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < NumMacroBlockPartitions(Dec->MacroBlock->Type); MacroBlockPiece++) {
                    if ((Dec->MacroBlock->NumRefIndexActiveLevel0 > 0 || Dec->Slice->MacroBlockFieldDecodingFlag != Dec->Slice->SliceIsInterlaced) && MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) != Pred_L1) {
                        Dec->MacroBlock->RefIndexLevel0[MacroBlockPiece] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                    }
                }
                for (uint32_t MacroBlockPiece = 0; MacroBlockPiece < NumMacroBlockPartitions(Dec->MacroBlock->Type); MacroBlockPiece++) {
                    if ((Dec->MacroBlock->NumRefIndexActiveLevel1 > 0 || Dec->Slice->MacroBlockFieldDecodingFlag != Dec->Slice->SliceIsInterlaced) && MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) != Pred_L0) {
                        Dec->MacroBlock->RefIndexLevel1[MacroBlockPiece] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                    }
                }
                for (uint32_t MacroBlockPiece = 0; MacroBlockPiece < NumMacroBlockPartitions(Dec->MacroBlock->Type); MacroBlockPiece++) {
                    if (MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) != Pred_L1) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            Dec->MacroBlock->MVDLevel0[MacroBlockPiece][0][compIdx] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                }
                for (uint32_t MacroBlockPiece = 0; MacroBlockPiece < NumMacroBlockPartitions(Dec->MacroBlock->Type); MacroBlockPiece++) {
                    if (MacroBlockPartitionPredictionMode(Dec, Dec->MacroBlock->Type, 0) != Pred_L0) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            Dec->MacroBlock->MVDLevel1[MacroBlockPiece][0][compIdx] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                }
            }
        }
        Dec->Slice->PicHeightInMapUnits      = Dec->SPS->PicHeightInMapUnitsMinus1 + 1;
        Dec->Slice->FrameHeightInMacroBlocks = (2 - Dec->SPS->OnlyMacroBlocksInFrame) * Dec->Slice->PicHeightInMapUnits;
        Dec->Slice->PicWidthInMacroBlocks    = Dec->SPS->PicWidthInMacroBlocksMinus1 + 1;
        Dec->Slice->PicWidthInSamplesLuma    = Dec->Slice->PicWidthInMacroBlocks * 16;
        Dec->Slice->PicWidthInSamplesChroma  = Dec->Slice->PicWidthInMacroBlocks * Dec->SPS->MacroBlockWidthChroma;
        Dec->Slice->PicHeightInMacroBlocks   = Dec->Slice->FrameHeightInMacroBlocks / (Dec->Slice->SliceIsInterlaced + 1);
        Dec->Slice->RawMacroBlockSizeInBits  = 256 * Dec->SPS->BitDepthLuma + 2 * Dec->SPS->MacroBlockWidthChroma * Dec->SPS->MacroBlockHeightChroma * Dec->SPS->BitDepthChroma;
        Dec->SPS->QPBDOffsetChroma           = Dec->SPS->ChromaBitDepthMinus8 * 6;
    }

    void DecodeMacroBlock(DecodeAVC *Dec, uint8_t *MacroBlock, size_t MacroBlockSize) {
        if (Dec->SPS->ChromaFormatIDC == ChromaBW) { // black and white

        } else if (Dec->SPS->ChromaFormatIDC == Chroma420) { // 4:2:0

        } else if (Dec->SPS->ChromaFormatIDC == Chroma422) { // 4:2:2

        } else if (Dec->SPS->ChromaFormatIDC == Chroma444) { // 4:4:4

        }
    }

    void ParseSubMacroBlockPredictionInSVC(DecodeAVC *Dec, BitBuffer *BitB) { // sub_mb_pred_in_scalable_extension
        if (Dec->PPS->EntropyCodingMode == ExpGolomb) {
            for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) { // MacroBlockPiece
                Dec->MacroBlock->SubMacroBlockType[MacroBlockPiece] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, CountUnary, AVCStopBit);
            }
            if ((InCropWindow(Dec, Dec->Slice->CurrentMacroBlockAddress) == true) && (Dec->Slice->AdaptiveMotionPredictionFlag == true)) {
                for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) {
                    if (SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Direct && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L1) {
                        Dec->MacroBlock->MotionPredictionFlagLevel0[MacroBlockPiece] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                    }
                    if (SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Direct && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L0) {
                        Dec->MacroBlock->MotionPredictionFlagLevel1[MacroBlockPiece] = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
                    }
                }
            }
            for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) { // MacroBlockPiece
                if ((Dec->MacroBlock->NumRefIndexActiveLevel0 > 0 || Dec->Slice->MacroBlockFieldDecodingFlag != Dec->Slice->SliceIsInterlaced) && Dec->MacroBlock->Type != P_8x8ref0 && SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L1 && !motion_prediction_flag_l0[MacroBlockPiece]) {

                    Dec->SPS->RefIndexLevel0[MacroBlockPiece] = ReadRICE(BitB, true, 0) + 1;
                }
                if ((Dec->MacroBlock->NumRefIndexActiveLevel1 > 0 || Dec->Slice->MacroBlockFieldDecodingFlag != Dec->Slice->SliceIsInterlaced) && SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L0 && !motion_prediction_flag_l1[MacroBlockPiece]) {

                    Dec->SPS->RefIndexLevel1[MacroBlockPiece] = ReadRICE(BitB, true, 0) + 1;

                }

                if (SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L1) {
                    for (uint32_t SubMacroBlockPiece = 0; SubMacroBlockPiece < NumSubMbPart(SubMacroBlockType[MacroBlockPiece]); SubMacroBlockPiece++) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            Dec->MacroBlock->MVDLevel0[MacroBlockPiece][SubMacroBlockPiece][compIdx] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, WholeUnary, AVCStopBit);
                        }
                    }
                }

                if (SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L0) {
                    for (uint32_t SubMacroBlockPiece = 0; SubMacroBlockPiece < NumSubMbPart(SubMacroBlockType[MacroBlockPiece]); SubMacroBlockPiece++) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            Dec->MacroBlock->MVDLevel1[MacroBlockPiece][SubMacroBlockPiece][compIdx] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, WholeUnary, AVCStopBit);
                        }
                    }
                }
            }
        } else if (Dec->PPS->EntropyCodingMode == Arithmetic) {
            for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) { // MacroBlockPiece
                Dec->MacroBlock->SubMacroBlockType[MacroBlockPiece] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
            }
            if ((InCropWindow(Dec, Dec->Slice->CurrentMacroBlockAddress) == true) && (Dec->Slice->AdaptiveMotionPredictionFlag == true)) {
                for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) {
                    if (SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Direct && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L1) {
                        Dec->MacroBlock->MotionPredictionFlagLevel0[MacroBlockPiece] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                    }
                    if (SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Direct && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L0) {
                        Dec->MacroBlock->MotionPredictionFlagLevel1[MacroBlockPiece] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                    }
                }
            }
            for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) { // MacroBlockPiece
                if ((Dec->MacroBlock->NumRefIndexActiveLevel0 > 0 || Dec->Slice->MacroBlockFieldDecodingFlag != Dec->Slice->SliceIsInterlaced) && Dec->MacroBlock->Type != P_8x8ref0 && SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L1 && !motion_prediction_flag_l0[MacroBlockPiece]) {

                    Dec->SPS->RefIndexLevel0[MacroBlockPiece] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL) + 1;

                }
                if ((Dec->MacroBlock->NumRefIndexActiveLevel1 > 0 || Dec->Slice->MacroBlockFieldDecodingFlag != Dec->Slice->SliceIsInterlaced) && SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L0 && !motion_prediction_flag_l1[MacroBlockPiece]) {

                    Dec->SPS->RefIndexLevel1[MacroBlockPiece] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL) + 1;

                }

                if (SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L1) {
                    for (uint32_t SubMacroBlockPiece = 0; SubMacroBlockPiece < NumSubMbPart(SubMacroBlockType[MacroBlockPiece]); SubMacroBlockPiece++) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            Dec->MacroBlock->MVDLevel0[MacroBlockPiece][SubMacroBlockPiece][compIdx] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                }

                if (SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L0) {
                    for (uint32_t SubMacroBlockPiece = 0; SubMacroBlockPiece < NumSubMbPart(SubMacroBlockType[MacroBlockPiece]); SubMacroBlockPiece++) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            Dec->MacroBlock->MVDLevel1[MacroBlockPiece][SubMacroBlockPiece][compIdx] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                }
            }
        }
    }

    void ParseReferenceBasePictureSyntax(DecodeAVC *Dec, BitBuffer *BitB) { // dec_ref_base_pic_marking
        Dec->SVC->AdaptiveMarkingModeFlag = BitBuffer_ReadBits(BitB, ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, 1);
        if (Dec->SVC->AdaptiveMarkingModeFlag == true) {
            Dec->SVC->BaseControlOperation = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, CountUnary, AVCStopBit);
            if (Dec->SVC->BaseControlOperation == 1) {
                Dec->SVC->NumBasePicDiff = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, CountUnary, AVCStopBit) + 1;
            } else if (Dec->SVC->BaseControlOperation == 2) {
                Dec->SVC->LongTermBasePicNum = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, CountUnary, AVCStopBit);
            } while(Dec->SVC->BaseControlOperation != 0) {

            }
        }
    }

    void ParseSubMacroBlockPrediction(DecodeAVC *Dec, BitBuffer *BitB, uint8_t mb_type) { // sub_mb_pred
        if (Dec->PPS->EntropyCodingMode == ExpGolomb) {
            for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) {
                SubMacroBlockType[MacroBlockPiece] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, CountUnary, AVCStopBit);
            }
            for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) {
                if ((Dec->MacroBlock->NumRefIndexActiveLevel0 > 0 || Dec->Slice->MacroBlockFieldDecodingFlag != Dec->Slice->SliceIsInterlaced) && Dec->MacroBlock->Type != P_8x8ref0 && SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L1) {
                    Dec->MacroBlock->RefIndexLevel0[MacroBlockPiece] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true) + 1;
                }
            }
            for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) {
                if ((Dec->MacroBlock->NumRefIndexActiveLevel1 > 0 || Dec->Slice->MacroBlockFieldDecodingFlag != Dec->Slice->SliceIsInterlaced) && SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L0) {
                    Dec->MacroBlock->RefIndexLevel1[MacroBlockPiece] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true) + 1;
                }
            }
            for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) {
                if (SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) !=Pred_L1) {
                    for (uint32_t SubMacroBlockPiece = 0; SubMacroBlockPiece < NumSubMbPart(SubMacroBlockType[MacroBlockPiece]);SubMacroBlockPiece++) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            Dec->MacroBlock->MVDLevel0[MacroBlockPiece][SubMacroBlockPiece][compIdx] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                        }
                    }
                }
            }
            for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) {
                if (SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L0) {
                    for (uint32_t SubMacroBlockPiece = 0; SubMacroBlockPiece < NumSubMbPart(SubMacroBlockType[MacroBlockPiece]);SubMacroBlockPiece++) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            Dec->MacroBlock->MVDLevel1[MacroBlockPiece][SubMacroBlockPiece][compIdx] = ReadExpGolomb(ByteOrder_LSByteIsFarthest, BitOrder_LSBitIsNearest, BitB, true);
                        }
                    }
                }
            }
        } else if (Dec->PPS->EntropyCodingMode == Arithmetic) {
            for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) {
                SubMacroBlockType[MacroBlockPiece] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
            }
            for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) {
                if ((Dec->MacroBlock->NumRefIndexActiveLevel0 > 0 || Dec->Slice->MacroBlockFieldDecodingFlag != Dec->Slice->SliceIsInterlaced) && Dec->MacroBlock->Type != P_8x8ref0 && SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L1) {
                    Dec->MacroBlock->RefIndexLevel0[MacroBlockPiece] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL) + 1;
                }
            }
            for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) {
                if ((Dec->MacroBlock->NumRefIndexActiveLevel1 > 0 || Dec->Slice->MacroBlockFieldDecodingFlag != Dec->Slice->SliceIsInterlaced) && SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L0) {
                    Dec->MacroBlock->RefIndexLevel1[MacroBlockPiece] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL) + 1;
                }
            }
            for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) {
                if (SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) !=Pred_L1) {
                    for (uint32_t SubMacroBlockPiece = 0; SubMacroBlockPiece < NumSubMbPart(SubMacroBlockType[MacroBlockPiece]);SubMacroBlockPiece++) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            Dec->MacroBlock->MVDLevel0[MacroBlockPiece][SubMacroBlockPiece][compIdx] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                }
            }
            for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) {
                if (SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L0) {
                    for (uint32_t SubMacroBlockPiece = 0; SubMacroBlockPiece < NumSubMbPart(SubMacroBlockType[MacroBlockPiece]);SubMacroBlockPiece++) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            Dec->MacroBlock->MVDLevel1[MacroBlockPiece][SubMacroBlockPiece][compIdx] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                }
            }
        }
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
