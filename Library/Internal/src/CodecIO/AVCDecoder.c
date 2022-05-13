/*!
 @header          AVCCodec.h
 @author          Marcus Johnson
 @copyright       2017+
 @version         1.0.0
 @brief           AVC fils are a collection of NAL-Units
 2.264 NAL-Units: {28 bytes, 536 bytes, }
 */

#include "../../include/CodecIO/AVCCodec.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h" /* Included for Assertions */
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/TextIO/LogIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    void AVC_ParseNAL(AVCOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);

        // left to right, first bit should be zero followed by nal_ref_idc aka 2 bits then nal_unit_type aka 5 bits

        /*
         Maybe we should try to make byte and bit orde more intuitive for like the 5th time?

         The Most Significant, aka most valuable bit is the left-most, or the right-most?

         ByteOrder_MSByteIsLeft
         ByteOrder_MSByteIsRight

         That's what the question really is...
         */
        if (BitBuffer_PeekBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsLeft, 1) == 0) { // NAL, probably; forbidden_zero_bit
            BitBuffer_Seek(BitB, 1); // Skip the peeked bit
            Options->NAL->NALRefIDC   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsLeft, 2); // 0b11
            Options->NAL->NALUnitType = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsLeft, 5); // 0b00111
            switch (Options->NAL->NALUnitType) {
                case NAL_NonIDRSlice: // 1
                    ParseNALSliceNonPartitioned(Options, BitB); // slice_layer_without_partitioning_rbsp
                    break;
                case NAL_SlicePartitionA: // 2
                    ParseNALSlicePartitionA(Options, BitB); // slice_data_partition_a_layer_rbsp
                    break;
                case NAL_SlicePartitionB: // 3
                    ParseNALSlicePartitionB(Options, BitB); // slice_data_partition_b_layer_rbsp
                    break;
                case NAL_SlicePartitionC: // 4
                    ParseNALSlicePartitionC(Options, BitB); // slice_data_partition_c_layer_rbsp
                    break;
                case NAL_IDRSliceNonPartitioned: // 5
                    ParseNALSliceNonPartitioned(Options, BitB); // slice_layer_without_partitioning_rbsp
                    break;
                case NAL_SupplementalEnhancementInfo: // 6
                    ParseSEIMessage(Options, BitB); // sei_rbsp
                    break;
                case NAL_SequenceParameterSet: // 7
                    ParseNALSequenceParameterSet(Options, BitB); // seq_parameter_set_rbsp
                    break;
                case NAL_PictureParameterSet: // 8
                    ParseNALPictureParameterSet(Options, BitB); // pic_parameter_set_rbsp
                    break;
                case NAL_AccessUnitDelimiter: // 9
                    ParseNALAccessUnitDelimiter(Options, BitB); // access_unit_delimiter_rbsp()
                    break;
                case NAL_EndOfSequence: // 10
                                        //ParseNALEndOfSequence(Options, BitB);       // End of Sequence aka Rescan end_of_seq_rbsp
                case NAL_EndOfStream: // 11
                                      //ParseNALEndOfStream(Options, BitB);        // End of Stream NAL.
                case NAL_FillerData: // 12
                    ParseNALFillerData(Options, BitB); // filler_data_rbsp
                case NAL_SequenceParameterSetExtended: // 13
                    ParseNALSequenceParameterSetExtended(Options, BitB);
                    break;
                case NAL_PrefixUnit: // 14
                    ParseNALPrefixUnit(Options, BitB); // prefix_nal_unit_rbsp
                    break;
                case NAL_SubsetSequenceParameterSet: // 15
                    ParseNALSubsetSPS(Options, BitB);
                    break;
                case NAL_DepthParameterSet: // 16
                    ParseNALDepthParameterSet(Options, BitB);
                    break;
                case NAL_AuxiliarySliceNonPartitioned: // 19
                    ParseNALIDRSliceNonPartitioned(Options, BitB);
                    break;
                case NAL_AuxiliarySliceExtension: // 20
                    ParseNALAuxiliarySliceExtension(Options, BitB);
                    // slice_layer_extension_rbsp
                    break;
                case NAL_MVCDepthView: // 21
                    ParseNALAuxiliarySliceExtension(Options, BitB);
                    // slice_layer_extension_rbsp
                    break;
                default:
                    Log(Severity_WARNING, PlatformIO_FunctionName, UTF8String("NAL ID 0x%X is not supported, seeking to next NAL\n"), Options->NAL->NALUnitType);
                    break;
            }
        }

        bool StartBitShouldBeZero = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsLeft, 1); // 0b0
        if (StartBitShouldBeZero == 0) {
            Options->NAL->RefIDC      = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsLeft, 2); // 0b11 aka 3
            Options->NAL->NALUnitType = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsLeft, 5); // 0b00111 aka 7
            
            switch (Options->NAL->NALUnitType) { // nal_unit_type

            }
        }
        
    }
    
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
     size_t FindNALSize(AVCOptions *Options, BitBuffer *BitB) {
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
     
     void ParseAVCFile(AVCOptions *Options, BitBuffer *BitB) { // byte_stream_nal_unit
     
     // Found a start code.
     if (BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 32) == AVCMagic && GetBitBufferPosition(BitB) == 0) {
     while (GetBitBufferPosition(BitB) + Bits2Bytes(BitB->BitsUnavailable, true) < BitB->FileSize) {
     ParseAVCHeader(Options, BitB);
     FindNALSize(Options, BitB);
     
     ScanNALUnits(Options, BitB);
     }
     } else {
     BitBuffer_Seek(BitB, 8);
     }
     Options->CurrentFilePosition = GetBitBufferPosition(BitB) + Bits2Bytes(BitB->BitsUnavailable, false);
     }
     
     void ParseAVCHeader(AVCOptions *Options, BitBuffer *BitB) { // nal_unit
     size_t BytesInNALUnit                 = 1; // nalUnitHeaderBytes
     if (BitBuffer_PeekBits(BitB, 1, true) == 0) {
     BitBuffer_Seek(BitB, 1);
     Options->NAL->NALRefIDC               = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 2); // 0
     Options->NAL->NALUnitType             = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 5); // 6
     
     int     NumBytesInRBSP = 0;
     int nalUnitHeaderBytes = 1;
     
     if ((Options->NAL->NALUnitType == NAL_PrefixUnit) || (Options->NAL->NALUnitType == NAL_AuxiliarySliceExtension) ||(Options->NAL->NALUnitType == NAL_MVCDepthView) ) {
     if (Options->NAL->NALUnitType == NAL_MVCDepthView) {
     Options->NAL->AVC3DExtensionFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
     } else {
     Options->NAL->SVCExtensionFlag   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
     }
     
     if (Options->NAL->SVCExtensionFlag == true) {
     ParseNALSVCExtension(Options, BitB);
     BytesInNALUnit += 3;
     } else if (Options->NAL->AVC3DExtensionFlag == true) {
     ParseNAL3DAVCExtension(Options, BitB);
     BytesInNALUnit += 2;
     } else {
     ParseNALMVCExtension(Options, BitB);
     BytesInNALUnit += 3;
     }
     }
     for (uint64_t NALByte = BytesInNALUnit; NALByte < Options->NAL->NALUnitSize; NALByte++) {
     if (NALByte + 2 < Options->NAL->NALUnitSize) {
     uint32_t NALID = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 24);
     if (NALID == 0x000003) {
     uint8_t NALID2 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 8);
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
     void ExtractNALFromByteStream(AVCOptions *Options, BitBuffer *BitB, size_t NALSize) {
     // Make sure the stream is byte algined by verifying there are 4 the data = 0x0000001
     // Once you've got that, you've got byte alignment.
     
     uint32_t ByteAlignment = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 32);
     if (ByteAlignment == 0x00000001) { // Bytestream is aligned
     uint32_t StreamType = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 24);
     if (StreamType == 0x000001) { // Start code prefix
     
     } else if (StreamType == 0x000003) { // emulation_prevention_three_byte
     
     }
     
     if ((BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 24) == 0x000000) && (IsStreamByteAligned(BitB->BitsUnavailable, 1) == true)) {
     
     
     }
     }
     }
     */
    
    void ParseTransformCoeffs(AVCOptions *Options, uint8_t i16x16DC, uint8_t i16x16AC, uint8_t Level4x4, uint8_t Level8x8, uint8_t StartIndex, uint8_t EndIndex) { // residual_block
        uint8_t Intra16x16DCLevel = i16x16DC, Intra16x16ACLevel = i16x16AC, LumaLevel4x4 = Level4x4, LumaLevel8x8 = Level8x8;
        // Return the first 4 variables
        if (Options->PPS->EntropyCodingMode == ExpGolomb) {
            // Read ExpGolomb
        } else {
            // Read Arithmetic
        }
        
        if (Options->SPS->ChromaArrayType == AVCChroma_420 || Options->SPS->ChromaArrayType == AVCChroma_422) {
            for (uint8_t iCbCr = 0; iCbCr < 1; iCbCr++) {
                ChromaDCLevel[iCbCr] = 4 * NumC8x8;
                for (uint8_t ChromaBlock = 0; ChromaBlock < i4x4; ChromaBlock++) {
                    ChromaACLevel[iCbCr][(i8x8 * 4) + i4x4] = 0;
                }
            }
        } else if (Options->SPS->ChromaArrayType == AVCChroma_444) {
            
        }
    }
    
    void ResidualLuma(AVCOptions *Options, BitBuffer *BitB, int i16x16DClevel, int i16x16AClevel, int level4x4,
                      int level8x8, int startIdx, int endIdx) { // residual_luma
        if (startIdx == 0 && MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) == Intra_16x16) {
            residual_block(i16x16DClevel, 0, 15, 16);
        }
        for (uint8_t i8x8 = 0; i8x8 < 4; i8x8++) {
            if (Options->MacroBlock->TransformSizeIs8x8 == false || Options->PPS->EntropyCodingMode == ExpGolomb) {
                for (uint8_t i4x4 = 0; i4x4 < 4; i4x4++) {
                    if (Options->MacroBlock->BlockPatternLuma & (1 << i8x8)) {
                        if (MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) == Intra_16x16) {
                            //residual_block(i16x16AClevel[i8x8 * 4 + i4x4], Maximum(0, startIdx - 1), endIdx - 1, 15);
                        } else {
                            //residual_block(level4x4[i8x8 * 4 + i4x4], startIdx, endIdx, 16);
                        }
                    } else if (MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) == Intra_16x16) {
                        for (uint8_t i = 0; i < 15; i++) {
                            //i16x16AClevel[i8x8 * 4 + i4x4][i] = 0;
                        }
                    } else {
                        for (uint8_t i = 0; i < 16; i++) {
                            //level4x4[i8x8 * 4 + i4x4][i] = 0;
                        }
                    }
                }
            }
            
            if (Options->PPS->EntropyCodingMode == ExpGolomb && Options->MacroBlock->TransformSizeIs8x8) {
                for (uint8_t i = 0; i < 16; i++) {
                    //level8x8[i8x8][4 * i + i4x4] = level4x4[i8x8 * 4 + i4x4][i];
                }
            } else if (Options->MacroBlock->BlockPatternLuma & (1 << i8x8)) {
                // residual_block(level8x8[i8x8], 4 * startIdx, 4 * endIdx + 3, 64);
            } else {
                for (uint8_t i = 0; i < 64; i++) {
                    level8x8[i8x8][i] = 0;
                }
            }
        }
    }
    
    int8_t MacroBlock2SliceGroupMap(AVCOptions *Options, uint8_t CurrentMacroBlock) { // MbToSliceGroupMap
        AssertIO(Options != NULL);
        if (Options->PPS->SliceGroups == 1 && (Options->PPS->SliceGroupMapType == 3 || Options->PPS->SliceGroupMapType == 4 || Options->PPS->SliceGroupMapType == 5)) {
            if (Options->PPS->SliceGroupMapType == 3) {
                if (Options->PPS->SliceGroupChangeDirection == false) {
                    return BoxOutClockwise;
                } else if (Options->PPS->SliceGroupChangeDirection == true) {
                    return BoxOutCounterClockwise;
                }
            } else if (Options->PPS->SliceGroupMapType == 4) {
                if (Options->PPS->SliceGroupChangeDirection == false) {
                    return RasterScan;
                } else if (Options->PPS->SliceGroupChangeDirection == true) {
                    return ReverseRasterScan;
                }
            } else if (Options->PPS->SliceGroupMapType == 5) {
                if (Options->PPS->SliceGroupChangeDirection == false) {
                    return WipeRight;
                } else if (Options->PPS->SliceGroupChangeDirection == true) {
                    return WipeLeft;
                }
            }
        }
        return -1; // failure
    }
    
    void rbsp_slice_trailing_bits(AVCOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        BitBuffer_Align(BitB, 1); // rbsp_trailing_bits();
        if (Options->PPS->EntropyCodingMode == Arithmetic) {
            while (more_rbsp_trailing_data()) {
                uint64_t CABACZeroWord = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16); /* equal to 0x0000 */
            }
        }
    }
    
    uint8_t MacroBlockPartitionPredictionMode(AVCOptions *Options, uint8_t MacroBlockType, uint8_t PartitionNumber) {  // MbPartPredMode
        AssertIO(Options != NULL);
        uint8_t ReturnValue = 0;
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
    
    uint64_t NextMacroBlockAddress(AVCOptions *Options, uint64_t CurrentMacroBlockAddress) { // NextMbAddress
        AssertIO(Options != NULL);
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
    
    bool InCropWindow(AVCOptions *Options, uint64_t MacroBlockAddress) { // mbAddr
        AssertIO(Options != NULL);
        uint64_t mbX     = ((MacroBlockAddress / (1 + Options->Slice->MbaffFrameFlag)) % Options->Slice->PicWidthInMacroBlocks);
        uint64_t mbY0    = 0;
        uint64_t mbY1    = 0;
        uint8_t  scalMbH  = 0; // Max = 32
        bool     Status   = 0;
        
        Options->Slice->ScaledRefLayerPicHeightInSamplesL = Options->Slice->PicHeightInMacroBlocks * 16 - (Options->Slice->ScaledRefLayerTopOffset + Options->Slice->ScaledRefLayerBottomOffset) / (1 + Options->Slice->SliceIsInterlaced);
        
        if (Options->Slice->MbaffFrameFlag == false) {
            mbY0 = (MacroBlockAddress / Options->Slice->PicWidthInMacroBlocks);
            mbY1 = (MacroBlockAddress / Options->Slice->PicWidthInMacroBlocks);
        } else if (Options->Slice->MbaffFrameFlag == true) {
            mbY0 = (2 * ((MacroBlockAddress / Options->Slice->PicWidthInMacroBlocks) / 2));
            mbY1 = mbY0 + 1;
        }
        scalMbH = (16 * (1 + Options->Slice->SliceIsInterlaced));
        
        if (Options->NAL->NoInterLayerPredictionFlag == true && (mbX >= ((Options->Slice->ScaledRefLayerLeftOffset + 15) / 16) && mbX < ((Options->Slice->ScaledRefLayerLeftOffset + ScaledRefLayerPicWidthInSamplesL) / 16)) && mbY0 >= ((Options->Slice->ScaledRefLayerTopOffset + scalMbH - 1) / scalMbH) && mbY1 < ((Options->Slice->ScaledRefLayerTopOffset + Options->Slice->ScaledRefLayerPicHeightInSamplesL) / scalMbH)) {
            Status = true;
        } else {
            Status = false;
        }
        
        return Status;
    }
    
    uint64_t ScalingList(AVCOptions *Options, BitBuffer *BitB, uint8_t *scalingList, size_t ScalingListSize, bool UseDefaultScalingMatrixFlag) { // scaling_list
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        uint8_t LastScale  = 8;
        uint8_t NextScale  = 8;
        uint8_t DeltaScale = 0;
        
        uint8_t ScalingList[ScalingListSize];
        
        for (uint64_t J = 0; J < ScalingListSize; J++) {
            if (NextScale != 0) {
                DeltaScale = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
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
    
    void AcquisitionElement3DV(AVCOptions *Options, BitBuffer *BitB, uint8_t numViews, uint8_t DeltaFlag, uint8_t PredDirection, uint8_t precMode, uint8_t expLen, uint8_t OutSign[MVCMaxViews], uint8_t OutExp, uint8_t OutMantissa, uint8_t OutManLen[MVCMaxViews]) { // 3dv_acquisition_element
        
        uint8_t element_equal_flag, numValues, matissa_len_minus1, prec, sign0, exponent0, mantissa0, skip_flag, sign1, exponent_skip_flag, manLen;
        uint8_t mantissaPred, mantissa_diff, exponent1;
        
        if (numViews - DeltaFlag > 1) {
            element_equal_flag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        }
        if (element_equal_flag == false) {
            numValues = numViews - DeltaFlag;
        } else {
            numValues = 1;
        }
        for (uint8_t Value = 0; Value < numValues; Value++) {
            if ((PredDirection == 2) && (Value = 0)) {
                if (precMode == 0) {
                    matissa_len_minus1 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 5) + 1;
                    OutManLen[index, Value] = manLen = matissa_len_minus1;
                } else {
                    prec = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 5);
                }
            }
            if (PredDirection == 2) {
                sign0 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                OutSign[index, Value] = sign0;
                exponent0 = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                
                OutExp[index, Value] = exponent0;
                if (precMode == 1) {
                    if (exponent0 == 0) {
                        OutManLen[index, Value] = manLen = Maximum(0, prec - 30);
                    } else {
                        OutManLen[index, Value] = manLen = Maximum(0, exponent0 + prec - 31);
                    }
                }
                mantissa0 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, manLen);
                OutMantissa[index, Value] = mantissa0;
            } else {
                skip_flag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                if (skip_flag == 0) {
                    sign1 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                    OutSign[index, Value] = sign1;
                    exponent_skip_flag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                    if (exponent_skip_flag == 0) {
                        exponent1 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, expLen);
                        OutExp[index, Value] = exponent1;
                    } else {
                        OutExp[index, Value] = OutExp[Options->DPS->ReferenceDPSID[1], Value];
                    }
                    mantissa_diff = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
                    mantissaPred = ((OutMantissa[Options->DPS->ReferenceDPSID[1], Value] * Options->DPS->PredictedWeight0 + OutMantissa[Options->DPS->ReferenceDPSID[1], Value] * (64 - Options->DPS->PredictedWeight0) + 32) >> 6);
                    OutMantissa[index, Value] = mantissaPred + mantissa_diff;
                    OutManLen[index, Value] = OutManLen[Options->DPS->ReferenceDPSID[1], Value];
                } else {
                    OutSign[index, Value] = OutSign[Options->DPS->ReferenceDPSID[1], Value];
                    OutExp[index, Value] = OutExp[Options->DPS->ReferenceDPSID[1], Value];
                    OutMantissa[index, Value] = OutMantissa[Options->DPS->ReferenceDPSID[1], Value];
                    OutManLen[index, Value] = OutManLen[Options->DPS->ReferenceDPSID[1], Value];
                }
            }
        }
        if (element_equal_flag == 1) {
            for (uint8_t View = 0; View < Options->SPS->ViewCount; View++) {
                OutSign[index, View] = OutSign[index, 0];
                OutExp[index, View] = OutExp[index, 0];
                OutMantissa[index, View] = OutMantissa[index, 0];
                OutManLen[index, View] = OutManLen[index, 0];
            }
        }
    }
    
    uint64_t DepthRanges(BitBuffer *BitB, uint64_t NumberOfViews, bool PredDirection, uint64_t Index) { // depth_ranges
        bool NearFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        bool FarFlag  = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        
        if (NearFlag == true) {
            AcquisitionElement3DV(NumberOfViews, 0, PredDirection, 7, 0, ZNearSign, ZNearExp, ZNearMantissa, ZNearManLen)
        } else if (FarFlag == true) {
            AcquisitionElement3DV(NumberOfViews, 0, PredDirection, 7, 0, ZFarSign, ZFarExp, ZFarMantissa, ZFarManLen);
        }
        
        return 0;
    }
    
    void vsp_param(AVCOptions *Options, BitBuffer *BitB, uint8_t numViews, uint8_t Direction, uint8_t DepthPS) { // vsp_param
        uint64_t disparity_diff_wji[numViews][numViews];
        uint64_t disparity_diff_oji[numViews][numViews];
        uint64_t disparity_diff_wij[numViews][numViews];
        uint64_t disparity_diff_oij[numViews][numViews];
        
        for (uint8_t View = 0; View < numViews; View++) { // Param
            for (uint8_t J = 0; J < View; J++) { // Wat
                disparity_diff_wji[J][View] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                disparity_diff_oji[J][View] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                disparity_diff_wij[View][J] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                disparity_diff_oij[View][J] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                if (Direction == 2) { /* Not 100% sure about the if loop part, but it makes more sense than for alone */
                    Options->DisparityScale[DepthPS][J][View] = disparity_diff_wji[J][View];
                    Options->DisparityOffset[DepthPS][J][View] = disparity_diff_oji[J][View];
                    Options->DisparityScale[DepthPS][View][J] = disparity_diff_wij[View][J] - disparity_diff_wji[J][View];
                    Options->DisparityOffset[DepthPS][View][J] = disparity_diff_oij[View][J] - disparity_diff_oji[J][View];
                } else {
                    Options->DisparityScale[DepthPS][J][View] = disparity_diff_wji[J][View] + (Options->DisparityScale[Options->DPS->ReferenceDPSID[0]][J][View] * Options->DPS->PredictedWeight0 + Options->DisparityScale[Options->DPS->ReferenceDPSID[1]][J][View] * (64 - Options->DPS->PredictedWeight0) + 32) >> 6;
                    Options->DisparityOffset[DepthPS][J][View] = disparity_diff_oji[J][View] + (Options->DisparityOffset[Options->DPS->ReferenceDPSID[0]][J][View] * Options->DPS->PredictedWeight0 + Options->DisparityOffset[Options->DPS->ReferenceDPSID[1]][J][View] * (64 - Options->DPS->PredictedWeight0) + 32) >> 6;
                    Options->DisparityScale[DepthPS][View][J] = disparity_diff_wij[View][J] + (Options->DisparityScale[Options->DPS->ReferenceDPSID[0]][View][J] * Options->DPS->PredictedWeight0 + Options->DisparityScale[Options->DPS->ReferenceDPSID[1]][View][J] * (64 - Options->DPS->PredictedWeight0) + 32) >> 6;
                    Options->DisparityOffset[DepthPS][View][J] = disparity_diff_oij[View][J] + (Options->DisparityOffset[Options->DPS->ReferenceDPSID[0]][View][J] * Options->DPS->PredictedWeight0 + Options->DisparityOffset[Options->DPS->ReferenceDPSID[1]][View][J] * (64 - Options->DPS->PredictedWeight0) + 32) >> 6;
                }
            }
        }
    }
    
    void RefPicListMVCMod(AVCOptions *Options, BitBuffer *BitB) { // ref_pic_list_mvc_modification
        if (((Options->Slice->Type % 5) != 2) && ((Options->Slice->Type % 5) != 4)) {
            Options->Slice->RefPicListModFlag[0] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->Slice->RefPicListModFlag[0] == true) {
                Options->Slice->ModPicNumsIDC = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                if ((Options->Slice->ModPicNumsIDC == 0) || (Options->Slice->ModPicNumsIDC == 1)) {
                    Options->Slice->AbsDiffPicNum = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true) + 1;
                } else if (Options->Slice->ModPicNumsIDC == 2) {
                    Options->Slice->LongTermPicNum = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                } else if ((Options->Slice->ModPicNumsIDC == 4) || (Options->Slice->ModPicNumsIDC == 5)) {
                    Options->Slice->AbsDiffViewIdx = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
                }
            }
        }
        if ((Options->Slice->Type % 5) == 1) {
            Options->Slice->RefPicListModFlag[1] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->Slice->RefPicListModFlag[1] == true) {
                Options->Slice->ModPicNumsIDC = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                if ((Options->Slice->ModPicNumsIDC == 0) || (Options->Slice->ModPicNumsIDC == 1)) {
                    Options->Slice->AbsDiffPicNum = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true) + 1;
                } else if (Options->Slice->ModPicNumsIDC == 2) {
                    Options->Slice->LongTermPicNum = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                } else if ((Options->Slice->ModPicNumsIDC == 4) || (Options->Slice->ModPicNumsIDC == 5)) {
                    Options->Slice->AbsDiffViewIdx = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
                }
            }
        }
    }
    
    void RefPicListMod(AVCOptions *Options, BitBuffer *BitB) { // ref_pic_list_modification
        if (((Options->Slice->Type % 5) != 2) && ((Options->Slice->Type % 5) != 4)) {
            Options->Slice->RefPicListModFlag[0] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->Slice->RefPicListModFlag[0] == true) {
                Options->Slice->ModPicNumsIDC = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                if ((Options->Slice->ModPicNumsIDC == 0) || (Options->Slice->ModPicNumsIDC == 1)) {
                    Options->Slice->AbsDiffPicNum = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true) + 1;
                } else if (Options->Slice->ModPicNumsIDC == 2) {
                    Options->Slice->LongTermPicNum = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                } else if ((Options->Slice->ModPicNumsIDC == 4) || (Options->Slice->ModPicNumsIDC == 5)) {
                    Options->Slice->AbsDiffViewIdx = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
                }
            }
        }
        if ((Options->Slice->Type % 5) == 1) {
            Options->Slice->RefPicListModFlag[1] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->Slice->RefPicListModFlag[1] == true) {
                Options->Slice->ModPicNumsIDC = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                if ((Options->Slice->ModPicNumsIDC == 0) || (Options->Slice->ModPicNumsIDC == 1)) {
                    Options->Slice->AbsDiffPicNum = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true) + 1;
                } else if (Options->Slice->ModPicNumsIDC == 2) {
                    Options->Slice->LongTermPicNum = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                } else if ((Options->Slice->ModPicNumsIDC == 4) || (Options->Slice->ModPicNumsIDC == 5)) {
                    Options->Slice->AbsDiffViewIdx = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
                }
            }
        }
    }
    
    void pred_weight_table(AVCOptions *Options, BitBuffer *BitB) { // pred_weight_table
        Options->Slice->LumaWeightDenom = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
        if (Options->SPS->ChromaArrayType != AVCChroma_Gray) {
            Options->Slice->ChromaWeightDenom = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        }
        for (uint8_t i = 0; i <= Options->MacroBlock->NumRefIndexActiveLevel0; i++) {
            Options->Slice->LumaWeightFlag[0] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->Slice->LumaWeightFlag[0] == true) {
                Options->Slice->LumaWeight[0][i]  = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
                Options->Slice->LumaOffset[0][i]  = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
            }
            if (Options->SPS->ChromaArrayType != AVCChroma_Gray) {
                Options->Slice->ChromaWeightFlag[0] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                if (Options->Slice->ChromaWeightFlag[0] == true) {
                    for (int J = 0; J < 2; J++) {
                        Options->Slice->ChromaWeight[0][i][J] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
                        Options->Slice->ChromaOffset[0][i][J] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
                    }
                }
            }
        }
        if ((Options->Slice->Type % 5) == 1) {
            for (uint8_t i = 0; i <= Options->MacroBlock->NumRefIndexActiveLevel1; i++) {
                Options->Slice->LumaWeightFlag[1] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                if (Options->Slice->LumaWeightFlag[1] == true) {
                    Options->Slice->LumaWeight[1][i]  = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
                    Options->Slice->LumaOffset[1][i]  = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
                }
                if (Options->SPS->ChromaArrayType != AVCChroma_Gray) {
                    Options->Slice->ChromaWeightFlag[1] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                    if (Options->Slice->ChromaWeightFlag[1] == true) {
                        for (uint8_t J = 0; J < 2; J++) {
                            Options->Slice->ChromaWeight[1][i][J] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
                            Options->Slice->ChromaOffset[1][i][J] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
                        }
                    }
                }
            }
        }
    }
    
    void DecodeRefPicMarking(AVCOptions *Options, BitBuffer *BitB) { // dec_ref_pic_marking
        if (Options->Slice->SliceIsIDR == true) {
            Options->NAL->EmptyPictureBufferBeforeDisplay = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->NAL->FrameIsLongTermReference        = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        } else {
            Options->NAL->AdaptiveRefPicMarkingModeFlag   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->NAL->AdaptiveRefPicMarkingModeFlag == true) {
                Options->NAL->MemManControlOp = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                if ((Options->NAL->MemManControlOp == 1) || (Options->NAL->MemManControlOp == 3)) {
                    Options->NAL->PicNumDiff = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
                }
                if (Options->NAL->MemManControlOp == 2) {
                    Options->Slice->LongTermPicNum  = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                }
                if ((Options->NAL->MemManControlOp == 3) || (Options->NAL->MemManControlOp == 6)) {
                    Options->NAL->LongTermFrameIndex = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                }
                if (Options->NAL->MemManControlOp == 4) {
                    Options->NAL->MaxLongTermFrameIndex = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) - 1;
                }
            }
        }
    }
    
    void Residual(AVCOptions *Options, BitBuffer *BitB, uint64_t StartIdx, uint64_t endIdx) {
        if (Options->PPS->EntropyCodingMode == false) {
            residual_block = ExpGolomb; // ParseResidualTransformCoefficentExpGolomb
        } else {
            residual_block = Arithmetic; // residual_block_cabac
        }
        ResidualLuma(i16x16DClevel, i16x16AClevel, level4x4, level8x8, startIdx, endIdx);
        Intra16x16DCLevel = i16x16DClevel;
        Intra16x16ACLevel = i16x16AClevel;
        LumaLevel4x4 = level4x4;
        LumaLevel8x8 = level8x8;
        if ((Options->SPS->ChromaArrayType == AVCChroma_420) || (Options->SPS->ChromaArrayType == AVCChroma_422)) {
            NumC8x8 = (4 / (SubWidthC * SubHeightC));
            for (uint8_t iCbCr = 0; iCbCr < 2; iCbCr++) {
                if ((Options->MacroBlock->BlockPatternChroma & 3) && (startIdx == 0)) { /* chroma DC residual present */
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
                        if (Options->MacroBlock->BlockPatternChroma & 2) { /* chroma AC residual present */
                            residual_block(ChromaACLevel[iCbCr][i8x8 * 4 + i4x4], Maximum(0, startIdx - 1), endIdx - 1, 15);
                        } else {
                            for (int i = 0; i < 15; i++) {
                                ChromaACLevel[iCbCr][i8x8 * 4 + i4x4][i] = 0;
                            }
                        }
                    }
                }
            }
        } else if (Options->SPS->ChromaArrayType == AVCChroma_444) {
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
    
    void ParseResidualTransformCoefficentExpGolomb(AVCOptions *Options, BitBuffer *BitB, uint64_t CoefficentLevel, uint64_t StartIndex, uint64_t EndIndex, uint64_t MaxCoefficents) { // residual_block_cavlc
                                                                                                                                                                                      // int coeffLevel, int startIdx, int endIdx, int maxNumCoeff
        
        uint64_t coeff_token = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
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
                    trailing_ones_sign_flag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                    levelVal[Coefficent] = 1 - 2 * trailing_ones_sign_flag;
                } else {
                    level_prefix = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                    levelCode = (Minimum(15, level_prefix) << suffixLength);
                    if ((suffixLength > 0) || (level_prefix >= 14)) {
                        level_suffix = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 0);
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
                    total_zeros = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                    zerosLeft = total_zeros;
                } else {
                    zerosLeft = 0;
                }
                for (uint64_t i = 0; i < TotalCoeff(coeff_token) - 1; i++) {
                    if (zerosLeft > 0) {
                        run_before = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
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
    
    void ScanNALUnits(AVCOptions *Options, BitBuffer *BitB) {
        
    }
    
    void RescanSync(BitBuffer *BitB) {
        // search for 0x000001, or 0x000003 for the next NAL.
        
    }
    
    void rbsp_trailing_bits(AVCOptions *Options, BitBuffer *BitB) { // rbsp_trailing_bits
        bool rbsp_stop_one_bit = 0;
        rbsp_stop_one_bit = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        BitBuffer_Align(BitB, 1); // while( !byte_aligned( ) )
                                  // rbsp_alignment_zero_bit
    }
    
    uint8_t GetCodedBlockPattern(AVCOptions *Options, uint8_t CodeNum) {
        uint8_t Value = 0;
        if (Options->MacroBlock->BlockPattern == 1 || Options->MacroBlock->BlockPattern == 2) {
            if (Options->SPS->ChromaArrayType == 1) {
                // Intra
                Value = CBPCAT12Intra[CodeNum];
            } else if (Options->SPS->ChromaArrayType == 2) {
                // Inter
                Value = CBPCAT12Inter[CodeNum];
            }
        }
        else if (Options->MacroBlock->BlockPattern == 0 || Options->MacroBlock->BlockPattern == 3) {
            if (Options->SPS->ChromaArrayType == 0) {
                // Intra
                Value = CBPCAT03Intra[CodeNum];
            } else if (Options->SPS->ChromaArrayType == 3) {
                // Inter
                Value = CBPCAT03Inter[CodeNum];
            }
        }
        return Value;
    }
    
    uint8_t MacroBlockPartitionWidth(AVCOptions *Options, uint8_t MacroBlockType) { // MbPartWidth
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
    
    uint8_t SubMacroBlockPartitionWidth(AVCOptions *Options) { // SubMbPartWidth
        
    }
    
    uint8_t MacroBlockPartitionHeight(AVCOptions *Options, uint8_t MacroBlockType) { // MbPartHeight
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
    
    BitBuffer *ExtractNAL2Packet(AVCOptions *Options, BitBuffer *AVCStream) { // NAL here means any NAL, packet here means NAL with any substituting required performed.
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
    void ParseSequenceParameterSetData(AVCOptions *Options, BitBuffer *BitB) { // seq_parameter_set_data
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        
        Options->SPS->ProfileIDC                                                        = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 8); // 244
        Options->SPS->ConstraintFlag0                                                   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1); // 0
        Options->SPS->ConstraintFlag1                                                   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1); // 0
        Options->SPS->ConstraintFlag2                                                   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1); // 0
        Options->SPS->ConstraintFlag3                                                   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1); // 0
        Options->SPS->ConstraintFlag4                                                   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1); // 0
        Options->SPS->ConstraintFlag5                                                   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1); // 0
        BitBuffer_Seek(BitB, 2); // Zero bits.
        Options->SPS->LevelIDC[0]                                                       = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 8); // 62
        Options->SPS->SeqParamSetID                                                     = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false); // 1
        
        if ((Options->SPS->ProfileIDC == 44)  ||
            (Options->SPS->ProfileIDC == 83)  ||
            (Options->SPS->ProfileIDC == 86)  ||
            (Options->SPS->ProfileIDC == 100) ||
            (Options->SPS->ProfileIDC == 110) ||
            (Options->SPS->ProfileIDC == 118) ||
            (Options->SPS->ProfileIDC == 122) ||
            (Options->SPS->ProfileIDC == 128) ||
            (Options->SPS->ProfileIDC == 134) ||
            (Options->SPS->ProfileIDC == 138) ||
            (Options->SPS->ProfileIDC == 139) ||
            (Options->SPS->ProfileIDC == 244)) {
            ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            Options->SPS->ChromaFormatIDC                                               = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false); // 1 aka 420
            if (Options->SPS->ChromaFormatIDC == AVCChroma_444) {
                Options->SPS->SeperateColorPlane                                        = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            }
            Options->SPS->LumaBitDepthMinus8                                            = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false); // 8
            Options->SPS->ChromaBitDepthMinus8                                          = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            Options->SPS->QPPrimeBypassFlag                                             = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1); // qpprime_y_zero_transform_bypass_flag
            Options->SPS->ScalingMatrixFlag                                             = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            
            if (Options->SPS->ScalingMatrixFlag == true) {
                for (uint8_t i = 0; i < ((Options->SPS->ChromaFormatIDC != AVCChroma_444) ? 8 : 12); i++) {
                    Options->SPS->ScalingListFlag[i]                                    = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                    if (Options->SPS->ScalingListFlag[i] == true) {
                        if (i < 6) {
                            ScalingList(Options, BitB, ScalingList4x4[i], 16, Options->PPS->DefaultScalingMatrix4x4[i]);
                        } else {
                            ScalingList(Options, BitB, ScalingList8x8[i - 6], 64, Options->PPS->DefaultScalingMatrix8x8[i - 6]);
                        }
                    }
                }
            }
        }
        Options->SPS->MaxFrameNumMinus4                                                 = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 4; // 3
        Options->SPS->PicOrderCount                                                     = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        if (Options->SPS->PicOrderCount == 0) {
            Options->SPS->MaxPicOrder                                                   = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 4;
        } else {
            Options->SPS->DeltaPicOrder                                                 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->SPS->OffsetNonRefPic                                               = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
            Options->SPS->OffsetTop2Bottom                                              = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
            Options->SPS->RefFramesInPicOrder                                           = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            for (uint8_t i = 0; i < Options->SPS->RefFramesInPicOrder; i++) {
                Options->SPS->RefFrameOffset[i]                                         = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
            }
            
        }
        Options->SPS->MaxRefFrames                                                      = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        Options->SPS->GapsInFrameNumAllowed                                             = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        Options->SPS->PicWidthInMacroBlocksMinus1                                       = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        Options->SPS->PicHeightInMapUnitsMinus1                                         = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        Options->SPS->OnlyMacroBlocksInFrame                                            = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (Options->SPS->OnlyMacroBlocksInFrame == false) {
            Options->SPS->AdaptiveFrameFlag                                             = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        }
        Options->SPS->Inference8x8                                                      = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        Options->SPS->FrameCroppingFlag                                                 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (Options->SPS->FrameCroppingFlag == true) {
            Options->SPS->FrameCropLeft                                                 = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            Options->SPS->FrameCropRight                                                = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            Options->SPS->FrameCropTop                                                  = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            Options->SPS->FrameCropBottom                                               = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        }
        Options->SPS->VUIPresent                                                        = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (Options->SPS->VUIPresent == true) {
            ParseVideoUsabilityInformation(Options, BitB);
        }
    }
    
    void ParseNALSequenceParameterSetExtended(AVCOptions *Options, BitBuffer *BitB) { // seq_parameter_set_extension_rbsp?
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SPS->SeqParamSetID                                                     = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        Options->SPS->AuxiliaryFormatID                                                 = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        if (Options->SPS->AuxiliaryFormatID != 0) {
            Options->SPS->AuxiliaryBitDepth                                             = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 8;
            Options->SPS->AlphaIncrFlag                                                 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->SPS->AlphaOpaqueValue                                              = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, Options->SPS->AuxiliaryBitDepth + 9);
            Options->SPS->AlphaTransparentValue                                         = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, Options->SPS->AuxiliaryBitDepth + 9);
        }
        Options->SPS->AdditionalExtensionFlag                                           = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        BitBuffer_Align(BitB, 1); // rbsp_trailing_bits
    }
    
    void ParseNALSubsetSPS(AVCOptions *Options, BitBuffer *BitB) { // subset_seq_parameter_set_rbsp
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        ReadSequenceParameterSetData(Options, BitB);
        if ((Options->SPS->ProfileIDC == Scalable_Constrained_Baseline_Profile) || (Options->SPS->ProfileIDC == Scalable_High_Intra_Profile)) { // Scalable Video mode
            ParseNALSequenceParameterSetSVC(Options, BitB);
            if (Options->SPS->VUIPresent == true) {
                ParseSVCVUIExtension(Options, BitB);
            }
        } else if ((Options->SPS->ProfileIDC == MultiView_High_Profile) || (Options->SPS->ProfileIDC == Stereo_High_Profile) || (Options->SPS->ProfileIDC == 134)) {
            // Multi View Coding
            BitBuffer_Seek(BitB, 1);
            ParseSPSMultiViewCodingExtension(Options, BitB); // seq_parameter_set_mvc_extension
            Options->SPS->MVCVUIParamsPresent                                           = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->SPS->MVCVUIParamsPresent == true) {
                ParseMVCVUI(Options, BitB);
            }
        } else if (Options->SPS->ProfileIDC == MultiView_Depth_High_Profile) {
            // MVCD
            BitBuffer_Seek(BitB, 1);
            ParseSPSMVCDExtension(Options, BitB);
        } else if (Options->SPS->ProfileIDC == 139) {
            // MVCD && 3DAVC
            BitBuffer_Seek(BitB, 1);
            ParseSPSMVCDExtension(Options, BitB);
            ParseSPS3DAVCExtension(Options, BitB);
        }
        Options->SPS->AdditionalExtension2                                              = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (Options->SPS->AdditionalExtension2 == true) {
            while (more_rbsp_data()) {
                Options->SPS->AdditionalExtension2DataFlag                              = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            }
        }
        BitBuffer_Align(BitB, 1); // rbsp_trailing_bits
    }
    
    void ParseNALSequenceParameterSet(AVCOptions *Options, BitBuffer *BitB) { // seq_parameter_set_rbsp
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        
        ParseSequenceParameterSetData(Options, BitB);                        // seq_parameter_set_data
        BitBuffer_Align(BitB, 1);                                         // rbsp_trailing_bits();
    }
    
    /* Video Usability Information */
    void ParseVideoUsabilityInformation(AVCOptions *Options, BitBuffer *BitB) { // Video Usability Information; ParseVUIParameters
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->VUI->AspectRatioInfoPresent                                            = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (Options->VUI->AspectRatioInfoPresent == true) {
            Options->VUI->AspectRatioIDC                                                = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 8);
            if (Options->VUI->AspectRatioIDC == 255) { // Extended_SAR = 255
                Options->VUI->SARWidth                                                  = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
                Options->VUI->SAWHeight                                                 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
            }
        }
        Options->VUI->OverscanInfoPresent                                               = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (Options->VUI->OverscanInfoPresent == true) {
            Options->VUI->DisplayInOverscan                                             = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        }
        Options->VUI->VideoSignalTypePresent                                            = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (Options->VUI->VideoSignalTypePresent == true) {
            Options->VUI->VideoType                                                     = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 3);
            Options->VUI->FullRange                                                     = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->VUI->ColorDescriptionPresent                                       = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->VUI->ColorDescriptionPresent == true) {
                Options->VUI->ColorPrimaries                                            = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 8);
                Options->VUI->TransferProperties                                        = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 8);
                Options->VUI->MatrixCoefficients                                        = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 8);
            }
        }
        Options->VUI->ChromaLocationPresent                                             = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (Options->VUI->ChromaLocationPresent == true) {
            Options->VUI->ChromaSampleLocationTop                                       = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            Options->VUI->ChromaSampleLocationBottom                                    = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        }
        Options->VUI->TimingInfoPresent[0]                                              = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (Options->VUI->TimingInfoPresent[0] == true) {
            Options->VUI->UnitsPerTick[0]                                               = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 32);
            Options->VUI->TimeScale[0]                                                  = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 32);
            Options->VUI->FixedFrameRateFlag[0]                                         = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        }
        Options->VUI->NALHrdParamsPresent[0]                                            = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (Options->VUI->NALHrdParamsPresent[0] == true) {
            ParseHypotheticalReferenceDecoder(Options, BitB);
        }
        Options->VUI->VCLHrdParamsPresent[0]                                            = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (Options->VUI->VCLHrdParamsPresent[0] == true) {
            ParseHypotheticalReferenceDecoder(Options, BitB); // wat
        }
        if ((Options->VUI->NALHrdParamsPresent[0] || Options->VUI->VCLHrdParamsPresent[0]) == true) {
            //Options->StreamIsLowDelay                                                   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        }
        Options->VUI->PicStructPresent[0]                                               = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        Options->VUI->BitStreamRestricted                                               = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (Options->VUI->BitStreamRestricted == true) {
            Options->VUI->MotionVectorsOverPicBoundaries[0]                             = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->VUI->MaxBytesPerPicDenom[0]                                        = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            Options->VUI->MaxBitsPerMarcoBlockDenom[0]                                  = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            Options->VUI->MaxMotionVectorLength[0]                                      = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            Options->VUI->MaxMotionVectorHeight[0]                                      = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            Options->VUI->MaxReorderFrames[0]                                           = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            Options->VUI->MaxFrameBuffer[0]                                             = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        }
    }
    
    void ParseMVCDVUIParametersExtension(AVCOptions *Options, BitBuffer *BitB) { // mvcd_vui_parameters_extension
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->VUI->VUIMVCDNumOpPoints                                                = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
        for (uint8_t MVCDOpPoint = 0; MVCDOpPoint < Options->VUI->VUIMVCDNumOpPoints; MVCDOpPoint++) {
            Options->VUI->VUIMVCDTemporalID[MVCDOpPoint]                                = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 3);
            Options->VUI->VUIMVCDNumTargetOutputViews[MVCDOpPoint]                      = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
            for (uint16_t VUIMVCDView = 0; VUIMVCDView < Options->VUI->VUIMVCDNumTargetOutputViews[MVCDOpPoint]; VUIMVCDView++) {
                Options->VUI->VUIMVCDViewID[MVCDOpPoint][VUIMVCDView]                   = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                Options->VUI->VUIMVCDDepthFlag[MVCDOpPoint][VUIMVCDView]                = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                Options->VUI->VUIMVCDTextureFlag[MVCDOpPoint][VUIMVCDView]              = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            }
            Options->VUI->VUIMVCDTimingInfoPresent[MVCDOpPoint]                         = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->VUI->VUIMVCDTimingInfoPresent[MVCDOpPoint] == true) {
                Options->VUI->VUIMVCDNumUnitsInTick[MVCDOpPoint]                        = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 32);
                Options->VUI->VUIMVCDTimeScale[MVCDOpPoint]                             = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 32);
                Options->VUI->VUIMVCDFixedFrameRateFlag[MVCDOpPoint]                    = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            }
            Options->VUI->VUIMVCDNalHRDParametersPresent[MVCDOpPoint]                   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->VUI->VUIMVCDNalHRDParametersPresent[MVCDOpPoint] == true) {
                hrd_parameters();
            }
            Options->VUI->VUIMVCDVclHRDParametersPresent[MVCDOpPoint]                   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->VUI->VUIMVCDVclHRDParametersPresent[MVCDOpPoint] == true) {
                hrd_parameters();
            }
            if ((Options->VUI->VUIMVCDNalHRDParametersPresent[MVCDOpPoint] || Options->VUI->VUIMVCDVclHRDParametersPresent[MVCDOpPoint]) == true) {
                Options->VUI->VUIMVCDHRDLowDelayPresent[MVCDOpPoint]                    = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            }
            Options->VUI->VUIMVCDPicStructPresent[MVCDOpPoint]                          = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        }
    }
    
    void ParseMVCVUIParametersExtension(AVCOptions *Options, BitBuffer *BitB) { // mvc_vui_parameters_extension
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->VUI->MVCNumOpertionPoints                                              = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
        for (uint16_t Operation = 0; Operation < Options->VUI->MVCNumOpertionPoints; Operation++) {
            Options->VUI->MVCTemporalID[0][Operation]                                   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 3);
            Options->VUI->MVCNumTargetViews[Operation]                                  = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
            for (uint16_t OutputView = 0; OutputView < Options->VUI->MVCNumTargetViews[Operation]; OutputView++) {
                Options->VUI->MVCViewID[Operation][OutputView]                          = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            }
            Options->VUI->TimingInfoPresent[Operation]                                  = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->VUI->TimingInfoPresent[Operation] == true) {
                Options->VUI->MVCUnitsInTick[Operation]                                 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 32);
                Options->VUI->MVCTimeScale[Operation]                                   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 32);
                Options->VUI->MVCFixedFrameRate[Operation]                              = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            }
            Options->VUI->MVCNALHRDParamsPresent[Operation]                             = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->VUI->MVCNALHRDParamsPresent[Operation] == true) {
                ParseHypotheticalReferenceDecoder(Options, BitB);
            }
            Options->VUI->MVCVCLHRDParamsPresent[Operation]                             = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->VUI->MVCVCLHRDParamsPresent[Operation] == true) {
                ParseHypotheticalReferenceDecoder(Options, BitB);
            }
            if ((Options->VUI->MVCNALHRDParamsPresent[Operation] || Options->VUI->MVCVCLHRDParamsPresent[Operation]) == true) {
                Options->VUI->MVCLowDelayFlag[Operation]                                = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            }
            Options->VUI->PicStructPresent[Operation]                                   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        }
    }
    
    void ParseSVCVUIExtension(AVCOptions *Options, BitBuffer *BitB) { // svc_vui_parameters_extension
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->VUI->VUIExtNumEntries                                                  = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
        for (uint8_t VUIExtEntry = 0; VUIExtEntry < Options->VUI->VUIExtNumEntries; VUIExtEntry++) {
            Options->VUI->VUIExtDependencyID[VUIExtEntry]                               = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 3);
            Options->VUI->VUIExtQualityID[VUIExtEntry]                                  = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 4);
            Options->VUI->VUIExtTemporalID[VUIExtEntry]                                 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 3);
            Options->VUI->VUIExtTimingInfoPresentFlag[VUIExtEntry]                      = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->VUI->VUIExtTimingInfoPresentFlag[VUIExtEntry] == true) {
                Options->VUI->VUIExtNumUnitsInTick[VUIExtEntry]                         = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 32);
                Options->VUI->VUIExtTimeScale[VUIExtEntry]                              = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 32);
                Options->VUI->VUIExtFixedFrameRateFlag[VUIExtEntry]                     = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            }
            Options->VUI->VUIExtNALHRDPresentFlag[VUIExtEntry]                          = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->VUI->VUIExtNALHRDPresentFlag[VUIExtEntry] == true) {
                ParseHypotheticalReferenceDecoder(Options, BitB);
            }
            Options->VUI->VUIExtVCLHRDPresentFlag[VUIExtEntry]                          = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->VUI->VUIExtVCLHRDPresentFlag[VUIExtEntry] == true) {
                ParseHypotheticalReferenceDecoder(Options, BitB);
            }
            if ((Options->VUI->VUIExtNALHRDPresentFlag[VUIExtEntry] == true) || (Options->VUI->VUIExtVCLHRDPresentFlag[VUIExtEntry] == true)) {
                Options->VUI->VUIExtLowDelayHRDFlag[VUIExtEntry]                        = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            }
            Options->VUI->VUIExtPicStructPresentFlag[VUIExtEntry]                       = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        }
    }
    
    /* Picture Parameter Set */
    void ParseNALPictureParameterSet(AVCOptions *Options, BitBuffer *BitB) { // pic_parameter_set_rbsp
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->PPS->PicParamSetID                                                     = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false); // 3?
        Options->SPS->SeqParamSetID                                                     = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false); // 2
        Options->PPS->EntropyCodingMode                                                 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1); // bit 9, 0
        Options->PPS->BottomPicFieldOrderInSliceFlag                                    = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1); // bit 8, 0
        Options->PPS->SliceGroups                                                       = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1; // 2
        if (Options->PPS->SliceGroups > 0) {
            Options->PPS->SliceGroupMapType                                             = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false); // 48?
            if (Options->PPS->SliceGroupMapType == 0) {
                for (uint8_t SliceGroup = 0; SliceGroup <= Options->PPS->SliceGroups; SliceGroup++) {
                    Options->PPS->RunLength[SliceGroup]                                 = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
                }
            } else if (Options->PPS->SliceGroupMapType == 2) {
                for (uint8_t SliceGroup = 0; SliceGroup <= Options->PPS->SliceGroups; SliceGroup++) {
                    Options->PPS->TopLeft[SliceGroup]                                   = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                    Options->PPS->BottomRight[SliceGroup]                               = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                }
            } else if ((Options->PPS->SliceGroupMapType == 3) || (Options->PPS->SliceGroupMapType == 4) || (Options->PPS->SliceGroupMapType == 5)) {
                for (uint8_t SliceGroup = 0; SliceGroup <= Options->PPS->SliceGroups; SliceGroup++) {
                    Options->PPS->SliceGroupChangeDirection                             = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                    Options->PPS->SliceGroupChangeRate                                  = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
                }
            } else if (Options->PPS->SliceGroupMapType == 6) {
                Options->PPS->PicSizeInMapUnits                                         = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
                for (uint64_t MapUnit = 0; MapUnit <= Options->PPS->PicSizeInMapUnits; MapUnit++) {
                    Options->PPS->SliceGroupID[MapUnit]                                 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, Ceili(log2(Options->PPS->SliceGroups)));
                }
            }
        }
        Options->PPS->RefIndex[0]                                                       = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
        Options->PPS->RefIndex[1]                                                       = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
        Options->PPS->WeightedPrediction                                                = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        Options->PPS->WeightedBiPrediction                                              = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        Options->PPS->InitialSliceQP                                                    = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true) + 26;
        Options->PPS->InitialSliceQS                                                    = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true) + 26;
        Options->PPS->ChromaQPOffset                                                    = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
        Options->PPS->DeblockingFilterFlag                                              = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        Options->PPS->ConstrainedIntraFlag                                              = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        Options->PPS->RedundantPictureFlag                                              = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (more_rbsp_data() == true) {
            Options->PPS->TransformIs8x8                                                = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->PPS->SeperateScalingMatrix                                         = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->PPS->SeperateScalingMatrix == true) {
                for (uint8_t i = 0; i < 6 + ((Options->SPS->ChromaFormatIDC != AVCChroma_444) ? 2 : 6) * Options->PPS->TransformIs8x8; i++) {
                    Options->PPS->PicScalingList[i]                                     = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                    if (Options->PPS->PicScalingList[i] == true) {
                        if (i < 6) {
                            ScalingList(Options, BitB, ScalingList4x4[i], 16, Options->PPS->DefaultScalingMatrix4x4[i]);
                        } else {
                            ScalingList(Options, BitB, ScalingList8x8[i - 6], 64, Options->PPS->DefaultScalingMatrix8x8[i - 6]);
                        }
                    }
                }
                Options->PPS->ChromaQPOffset                                            = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
            }
            BitBuffer_Align(BitB, 1);
        }
    }
    
    /* Scalable Video Coding */
    void ParseNALSVCExtension(AVCOptions *Options, BitBuffer *BitB) { // nal_unit_header_svc_extension
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->NAL->IDRFlag                                                           = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        Options->NAL->PriorityID[0]                                                     = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 6);
        Options->NAL->NoInterLayerPredictionFlag                                        = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        Options->NAL->DependencyID[0]                                                   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 3);
        Options->NAL->QualityID[0][0]                                                   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 4);
        Options->NAL->TemporalID[0]                                                     = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 3);
        Options->NAL->UseReferenceBasePictureFlag                                       = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        Options->NAL->IsDisposable[0]                                                   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        Options->NAL->OutputFlag                                                        = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        BitBuffer_Seek(BitB, 2); // reserved_three_2bits
    }
    
    void ParseNALSequenceParameterSetSVC(AVCOptions *Options, BitBuffer *BitB) { // seq_parameter_set_svc_extension
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SVC->InterLayerDeblockingFilterPresent                                 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        Options->SVC->ExtendedSpatialScalabilityIDC                                     = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 2);
        if ((Options->SPS->ChromaFormatIDC == AVCChroma_420) || (Options->SPS->ChromaFormatIDC == AVCChroma_422)) {
            Options->SVC->ChromaPhaseXFlag                                              = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        }
        if (Options->SPS->ChromaFormatIDC == AVCChroma_420) {
            Options->SVC->ChromaPhaseY                                                  = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 2);
        }
        if (Options->SVC->ExtendedSpatialScalabilityIDC == 1) {
            if (Options->SPS->ChromaFormatIDC != AVCChroma_Gray) {
                Options->SVC->SeqRefLayerChromaPhaseX                                   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                Options->SVC->SeqRefLayerChromaPhaseY                                   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 2);
            }
            Options->SVC->RefLayerLeftOffset                                            = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
            Options->SVC->RefLayerTopOffset                                             = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
            Options->SVC->RefLayerRightOffset                                           = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
            Options->SVC->RefLayerBottomOffset                                          = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
        }
        Options->SVC->SequenceCoeffLevelPresent                                         = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (Options->SVC->SequenceCoeffLevelPresent == true) {
            Options->SVC->AdaptiveCoeffsPresent                                         = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        }
        Options->SVC->SliceHeaderRestricted                                             = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
    }
    
    void ParseNALPrefixUnitSVC(AVCOptions *Options, BitBuffer *BitB) { // prefix_nal_unit_svc
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        if (Options->NAL->NALRefIDC != 0) {
            Options->Slice->StoreRefBasePicFlag                                         = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (((Options->NAL->UseReferenceBasePictureFlag) || (Options->Slice->StoreRefBasePicFlag)) && (!Options->NAL->IDRFlag)) {
                ParseReferenceBasePictureSyntax(Options, BitB); // dec_ref_base_pic_marking();
            }
            Options->NAL->AdditionalPrefixNALExtensionFlag                              = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->NAL->AdditionalPrefixNALExtensionFlag == true) {
                while (more_rbsp_data()) {
                    Options->NAL->AdditionalPrefixNALExtensionDataFlag                  = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                }
            }
            BitBuffer_Align(BitB, 1); // rbsp_trailing_bits()
        } else if (more_rbsp_data()) {
            while (more_rbsp_data()) {
                Options->NAL->AdditionalPrefixNALExtensionDataFlag                      = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            }
        }
        BitBuffer_Align(BitB, 1); // rbsp_trailing_bits()
    }
    
    /* Multi-View Coding */
    void ParseNALMVCExtension(AVCOptions *Options, BitBuffer *BitB) { // nal_unit_header_mvc_extension
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->NAL->NonIDRFlag                                                        = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        Options->NAL->PriorityID[0]                                                     = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 6);
        Options->SPS->ViewID[0][0]                                                      = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 10);
        Options->NAL->TemporalID[0]                                                     = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 3);
        Options->NAL->IsAnchorPicture                                                   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        Options->NAL->InterViewFlag                                                     = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        BitBuffer_Seek(BitB, 1);
    }
    
    void ParseSPSMVCDExtension(AVCOptions *Options, BitBuffer *BitB) { // seq_parameter_set_mvcd_extension
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SPS->ViewCount                                                         = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
        for (uint16_t View = 0; View < Options->SPS->ViewCount; View++) {
            Options->SPS->ViewID[View][0]                                               = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            Options->SPS->DepthViewPresent[View]                                        = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->SPS->DepthViewID[View]                                             = Options->SPS->ViewID[View][0];
            Options->DPS->NumDepthViews                                                += Options->SPS->DepthViewPresent[View];
            Options->SPS->TextureViewPresent[View]                                      = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        }
        for (uint16_t View = 1; View < Options->SPS->ViewCount; View++) {
            if (Options->SPS->DepthViewPresent[View] == true) {
                Options->SPS->AnchorRefsCount[0][View]                                  = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                for (uint8_t AnchorRef = 0; AnchorRef < Options->SPS->AnchorRefsCount[0][View]; AnchorRef++) {
                    Options->SPS->AnchorRef[0][View][AnchorRef]                         = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                }
                Options->SPS->AnchorRefsCount[1][View]                                  = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                for (uint8_t AnchorRef = 0; AnchorRef < Options->SPS->AnchorRefsCount[1][View]; AnchorRef++) {
                    Options->SPS->AnchorRef[1][View][AnchorRef]                         = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                }
            }
        }
        for (uint16_t View = 1; View < Options->SPS->ViewCount; View++) {
            if (Options->SPS->DepthViewPresent[View] == true) {
                Options->SPS->NonAnchorRefCount[0][View]                                = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                for (uint16_t NonAnchorRef = 0; NonAnchorRef < Options->SPS->NonAnchorRefCount[0][View]; NonAnchorRef++) {
                    Options->SPS->NonAnchorRef[0][View][NonAnchorRef]                   = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                }
                Options->SPS->NonAnchorRefCount[1][View]                                = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                for (uint16_t NonAnchorRef = 0; NonAnchorRef < Options->SPS->NonAnchorRefCount[0][View]; NonAnchorRef++) {
                    Options->SPS->NonAnchorRef[1][View][NonAnchorRef]                   = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                }
            }
        }
        Options->SPS->NumLevelValues                                                    = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
        for (uint8_t Level = 0; Level < Options->SPS->NumLevelValues; Level++) {
            Options->SPS->LevelIDC[Level]                                               = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            Options->SPS->NumApplicableOps[Level]                                       = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
            for (uint16_t AppOp = 0; AppOp < Options->SPS->NumApplicableOps[Level]; AppOp++) {
                Options->SPS->AppOpTemporalID[Level][AppOp]                             = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                Options->SPS->AppOpNumTargetViews[Level][AppOp]                         = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
                for (uint16_t AppOpTargetView = 0; AppOpTargetView < Options->SPS->AppOpNumTargetViews[Level][AppOp]; AppOpTargetView++) {
                    Options->SPS->AppOpTargetViewID[Level][AppOp][AppOpTargetView]      = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                    Options->SPS->AppOpDepthFlag[Level][AppOp][AppOpTargetView]         = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                    Options->SPS->AppOpTextureFlag[Level][AppOp][AppOpTargetView]       = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                }
                Options->SPS->AppOpTextureViews[Level][AppOp]                           = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
                Options->SPS->AppOpNumDepthViews[Level][AppOp]                          = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            }
        }
        Options->SPS->MVCDVUIParametersPresent                                          = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (Options->SPS->MVCDVUIParametersPresent == true) {
            ParseMVCDVUIParametersExtension(Options, BitB); // mvcd_vui_parameters_extension();
        }
        Options->SPS->MVCDTextureVUIParametersPresent                                   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (Options->SPS->MVCDTextureVUIParametersPresent == true) {
            ParseMVCVUIParametersExtension(Options, BitB); //mvc_vui_parameters_extension();
        }
    }
    
    void ParseNALDepthParameterSet(AVCOptions *Options, BitBuffer *BitB) { // depth_parameter_set_rbsp
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->DPS->DepthParameterSetID                                               = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        Options->DPS->PredictionDirection                                               = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        if ((Options->DPS->PredictionDirection == 0) || (Options->DPS->PredictionDirection == 1)) {
            Options->DPS->ReferenceDPSID[0]                                             = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            Options->DPS->PredictedWeight0                                              = 64;
        }
        if (Options->DPS->PredictionDirection == 0) {
            Options->DPS->ReferenceDPSID[1]                                             = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            Options->DPS->PredictedWeight0                                              = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 6);
        }
        Options->DPS->NumDepthViews                                                     = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
        depth_ranges(BitB, Options->DPS->NumDepthViews, Options->DPS->PredictionDirection, Options->DPS->DepthParameterSetID);
        Options->DPS->VSPParamFlag                                                      = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (Options->DPS->VSPParamFlag == true) {
            vsp_param(Options, BitB, Options->DPS->NumDepthViews, Options->DPS->PredictionDirection, Options->DPS->DepthParameterSetID);
        }
        Options->DPS->AdditionalExtensionFlag                                           = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        Options->DPS->DepthMappingValues                                                = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        for (uint8_t i = 1; i <= Options->DPS->DepthMappingValues; i++) {
            Options->DPS->DepthRepresentationModel[i]                                   = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        }
        if (Options->DPS->AdditionalExtensionFlag == true) {
            while (more_rbsp_data() == true) {
                Options->DPS->AdditionalExtensionFlag                                   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            }
        }
        BitBuffer_Align(BitB, 1); // rbsp_trailing_bits
    }
    
    void ParseSPS3DAVCExtension(AVCOptions *Options, BitBuffer *BitB) { // seq_parameter_set_3davc_extension
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        
        if (Options->DPS->NumDepthViews > 0) {
            Options->SPS->AVC3DAcquisitionIDC                                           = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            for (uint8_t View = 0; View < Options->DPS->NumDepthViews; View++) {
                Options->SPS->AVC3DViewID[View]                                         = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            }
            if (Options->SPS->AVC3DAcquisitionIDC > 0) {
                DepthRanges(BitB, Options->DPS->NumDepthViews, 2, 0);
                vsp_param(Options, BitB, Options->DPS->NumDepthViews, 2, 0);
            }
            Options->SPS->ReducedResolutionFlag                                         = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->SPS->ReducedResolutionFlag == true) {
                Options->SPS->DepthPicWidthInMacroBlocks                                = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
                Options->SPS->DepthPicHeightInMapUnits                                  = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
                Options->SPS->DepthHorizontalDisparity                                  = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
                Options->SPS->DepthVerticalDisparity                                    = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
                Options->SPS->DepthHorizontalRSH                                        = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                Options->SPS->DepthVerticalRSH                                          = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            } else {
                Options->SPS->DepthHorizontalDisparity                                  = 1;
                Options->SPS->DepthVerticalDisparity                                    = 1;
                Options->SPS->DepthHorizontalRSH                                        = 0;
                Options->SPS->DepthVerticalRSH                                          = 0;
            }
            Options->SPS->DepthFrameCroppingFlag                                        = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->SPS->DepthFrameCroppingFlag == true) {
                Options->SPS->DepthFrameLeftCropOffset                                  = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                Options->SPS->DepthFrameRightCropOffset                                 = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                Options->SPS->DepthFrameTopCropOffset                                   = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                Options->SPS->DepthFrameBottomCropOffset                                = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            }
            Options->SPS->GridPosViewCount                                              = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            for (uint8_t TextureView = 0; TextureView < Options->SPS->GridPosViewCount; TextureView++) {
                Options->SPS->GridPosViewID[TextureView]                                = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                Options->SPS->GridPosX[TextureView]                                     = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
                Options->SPS->GridPosY[TextureView]                                     = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
            }
            Options->SPS->SlicePrediction                                               = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->Slice->SeqViewSynthesisFlag                                        = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        }
        Options->SPS->ALCSpsEnableFlag                                                  = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        Options->SPS->EnableRLESkipFlag                                                 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        bool AllViewsPairedFlag                                                     = AreAllViewsPaired(Options);
        if (AllViewsPairedFlag == false) {
            for (uint8_t View = 0; View < Options->SPS->ViewCount; View++) {
                if (Options->SPS->TextureViewPresent[View] == true) {
                    Options->SPS->AnchorRefsCount[0][View]                              = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                    for (uint16_t AnchorViewL0 = 0; AnchorViewL0 < Options->SPS->AnchorRefsCount[0][View]; AnchorViewL0++) {
                        Options->SPS->AnchorRef[0][View][AnchorViewL0]                  = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                    }
                    Options->SPS->AnchorRefsCount[1][View]                              = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                    for (uint16_t AnchorViewL1 = 0; AnchorViewL1 < Options->SPS->AnchorRefsCount[1][View]; AnchorViewL1++) {
                        Options->SPS->AnchorRef[1][View][AnchorViewL1]                  = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                    }
                }
            }
            for (uint16_t View = 0; View <= Options->SPS->ViewCount; View++) {
                if (Options->SPS->TextureViewPresent[View] == true) {
                    Options->SPS->NonAnchorRefCount[0][View]                            = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                    for (uint16_t NonAnchorRefL0 = 0; NonAnchorRefL0 < Options->SPS->NonAnchorRefCount[0][View]; NonAnchorRefL0++) {
                        Options->SPS->NonAnchorRef[0][View][NonAnchorRefL0]             = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                    }
                    Options->SPS->NonAnchorRefCount[1][View]                            = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                    for (uint16_t NonAnchorRefL1 = 0; NonAnchorRefL1 < Options->SPS->NonAnchorRefCount[1][View]; NonAnchorRefL1++) {
                        Options->SPS->NonAnchorRef[1][View][NonAnchorRefL1]             = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                    }
                }
            }
        }
    }
    
    void ParseNAL3DAVCExtension(AVCOptions *Options, BitBuffer *BitB) { // nal_unit_header_3davc_extension
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->NAL->ViewIndex                                                         = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 8);
        Options->NAL->DepthFlag                                                         = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        Options->NAL->NonIDRFlag                                                        = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        Options->NAL->TemporalID[0]                                                     = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 3);
        Options->NAL->IsAnchorPicture                                                   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        Options->NAL->InterViewFlag                                                     = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
    }
    
    /* Hypothetical Reference Decoder */
    void ParseHypotheticalReferenceDecoder(AVCOptions *Options, BitBuffer *BitB) { // hrd_parameters
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->HRD->NumCodedPictureBuffers                                            = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
        Options->HRD->BitRateScale                                                      = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 4);
        Options->HRD->CodedPictureBufferScale                                           = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 4);
        for (uint8_t SchedSelIdx = 0; SchedSelIdx < Options->HRD->NumCodedPictureBuffers; SchedSelIdx++) {
            Options->HRD->BitRate[SchedSelIdx]                                          = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
            Options->HRD->CodedPictureSize[SchedSelIdx]                                 = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
            Options->HRD->IsConstantBitRate[SchedSelIdx]                                = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1) + 1; // FIXME: is +1 correct
        }
        Options->HRD->InitialCPBDelayLength                                             = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 5) + 1;
        Options->HRD->CBPDelay                                                          = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 5) + 1;
        Options->HRD->DBPDelay                                                          = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 5) + 1;
        Options->HRD->TimeOffsetSize                                                    = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 5);
    }
    
    /* Generic */
    void ParseNALSliceHeader(AVCOptions *Options, BitBuffer *BitB) { // slice_header
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->Slice->FirstMacroBlockInSlice                                          = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false); // 0
        Options->Slice->Type                                                            = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false); // 0, 34 0s remaining
        Options->PPS->PicParamSetID                                                     = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false); // 0, 26 0s remaining
        
        if (Options->SPS->SeperateColorPlane == true) {
            Options->Slice->ColorPlaneID                                                = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 2);
        }
        
        Options->Slice->FrameNumber                                                     = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false); // FIXME: Should I use BitBuffer_ReadBits?
        if (Options->SPS->OnlyMacroBlocksInFrame == false) {
            Options->Slice->SliceIsInterlaced                                           = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->Slice->SliceIsInterlaced == true) {
                Options->Slice->SliceIsBottomField                                      = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            }
        }
        Options->Slice->SliceIsIDR                                                      = ((Options->NAL->NALUnitType == NAL_IDRSliceNonPartitioned) ? true : false);
        if (Options->Slice->SliceIsIDR == true) {
            Options->Slice->IDRPicID                                                    = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        }
        if (Options->SPS->PicOrderCount == 0) {
            Options->Slice->PictureOrderCountLSB                                        = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            if ((Options->PPS->BottomPicFieldOrderInSliceFlag == true) && (Options->Slice->SliceIsInterlaced == false)) {
                Options->Slice->DeltaPicOrderCount[0]                                   = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
            }
        }
        if (Options->SPS->PicOrderCount == true && Options->SPS->DeltaPicOrder == false) {
            Options->Slice->DeltaPicOrderCount[0]                                       = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
            if ((Options->PPS->BottomPicFieldOrderInSliceFlag == true) && (Options->Slice->SliceIsInterlaced == false)) {
                Options->Slice->DeltaPicOrderCount[1]                                   = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
            }
        }
        if (Options->PPS->RedundantPictureFlag == true) {
            Options->PPS->RedundantPictureCount                                         = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        }
        if ((Options->Slice->Type == SliceB1) || (Options->Slice->Type == SliceB2)) {
            Options->Slice->DirectSpatialMVPredictionFlag                               = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            
        }
        if ((Options->Slice->Type == SliceP1)  || (Options->Slice->Type == SliceP2)  ||
            (Options->Slice->Type == SliceSP1) || (Options->Slice->Type == SliceSP2) ||
            (Options->Slice->Type == SliceB1)  || (Options->Slice->Type == SliceB2)) {
            Options->Slice->NumRefIDXActiveOverrideFlag                                 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->Slice->NumRefIDXActiveOverrideFlag == true) {
                Options->MacroBlock->NumRefIndexActiveLevel0                            = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1; // num_ref_idx_l0_active_minus1
                if ((Options->Slice->Type == SliceB1) || (Options->Slice->Type == SliceB2)) {
                    Options->MacroBlock->NumRefIndexActiveLevel1                        = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1; // num_ref_idx_l1_active_minus1
                }
            }
        }
        if ((Options->NAL->NALUnitType == NAL_AuxiliarySliceExtension) || (Options->NAL->NALUnitType == NAL_MVCDepthView)) {
            RefPicListMVCMod(Options, BitB);
        } else {
            RefPicListMod(Options, BitB);
        }
        if ((Options->PPS->WeightedPrediction == true)
            && (
                (Options->Slice->Type == SliceP1)  ||
                (Options->Slice->Type == SliceP2)  ||
                (Options->Slice->Type == SliceSP1) ||
                (Options->Slice->Type == SliceSP2) ||
                (Options->Slice->Type == SliceB1)  ||
                (Options->Slice->Type == SliceB2)
                ) && (Options->PPS->WeightedBiPrediction == true)) {
                    pred_weight_table(Options, BitB);
                }
        if (Options->NAL->NALRefIDC != 0) {
            DecodeRefPicMarking(Options, BitB);
        }
        if ((Options->PPS->EntropyCodingMode  == true) && (((Options->Slice->Type != SliceI1) || (Options->Slice->Type != SliceI2) || (Options->Slice->Type != SliceSI1) || (Options->Slice->Type != SliceSI2)))) {
            Options->Slice->CabacInitIDC                                                = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
        }
        Options->Slice->SliceQPDelta                                                    = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
        if (
            (Options->Slice->Type == SliceSP1) || (Options->Slice->Type == SliceSP2) ||
            (Options->Slice->Type == SliceSI1) || (Options->Slice->Type == SliceSI2)) {
                
                if ((Options->Slice->Type == SliceSP1) || (Options->Slice->Type == SliceSP2)) {
                    Options->Slice->DecodePMBAsSPSlice                                      = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                }
                Options->Slice->SliceQSDelta                                                = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
            }
        if (Options->PPS->DeblockingFilterFlag == true) {
            Options->Slice->DisableDeblockingFilterIDC                                  = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            if (Options->Slice->DisableDeblockingFilterIDC  != true) {
                Options->Slice->SliceAlphaOffsetC0                                      = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
                Options->Slice->SliceBetaOffset                                         = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
            }
        }
        if (Options->PPS->SliceGroups > 0 && (Options->PPS->SliceGroupMapType >= 3 && Options->PPS->SliceGroupMapType <= 5)) {
            uint64_t Bits                                                           = Ceili(log2(Options->PPS->PicSizeInMapUnits / Options->PPS->SliceGroupChangeRate));
            Options->Slice->SliceGroupChangeCycle                                       = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, Ceili(log2(Options->PPS->PicSizeInMapUnits /  Options->Slice->SliceGroupChangeRate)));
        }
    }
    
    void ParseNALSliceData(AVCOptions *Options, BitBuffer *BitB, uint8_t Category) { // slice_data
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        if (Options->PPS->EntropyCodingMode == Arithmetic) {
            while (IsBitBufferAligned(BitB, 1) == false) {
                BitBuffer_Seek(BitB, 1); // cabac_alignment_one_bit
            }
            uint64_t CurrentMacroBlockAddress                                       = Options->Slice->FirstMacroBlockInSlice * (Options->Slice->MbaffFrameFlag + 1);
            bool     MoreDataFlag                                                   = true;
            Options->Slice->PreviousMacroBlockSkipped                                   = false;
            if ((Options->Slice->Type != SliceI1) || (Options->Slice->Type != SliceI2) || (Options->Slice->Type != SliceSI1) || (Options->Slice->Type != SliceSI2)) {
                if (Options->PPS->EntropyCodingMode == ExpGolomb) {
                    Options->Slice->MacroBlockSkipRun                                   = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                    Options->Slice->PreviousMacroBlockSkipped                           = (Options->Slice->MacroBlockSkipRun > 0);
                    for (uint8_t SkippedMacroBlock = 0; SkippedMacroBlock < Options->Slice->MacroBlockSkipRun; SkippedMacroBlock++) {
                        CurrentMacroBlockAddress                                    = NextMacroBlockAddress(Options, CurrentMacroBlockAddress);
                    }
                }
            }
        }
    }
    
    void ParseNALSlicePartitionA(AVCOptions *Options, BitBuffer *BitB) { // slice_data_partition_a_layer_rbsp
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        ParseSliceHeader(Options, BitB);
        uint64_t SliceID                                                            = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        ParseSliceData(Options, BitB, 2); /* only category 2 parts of slice_data() syntax */
        rbsp_slice_trailing_bits(Options, BitB); // BitBuffer_Align(BitB, 1);
    }
    
    void ParseNALSlicePartitionB(AVCOptions *Options, BitBuffer *BitB) { // slice_data_partition_b_layer_rbsp
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        uint8_t SliceID                                                             = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        if (Options->SPS->SeperateColorPlane == true) {
            uint8_t ColorPlaneID                                                    = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        }
        if (Options->PPS->RedundantPictureFlag == true) {
            Options->PPS->RedundantPictureCount                                         = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        }
        ParseSliceData(Options, BitB, 3);
        rbsp_slice_trailing_bits(Options, BitB); // BitBuffer_Align(BitB, 1);
    }
    
    void ParseNALSlicePartitionC(AVCOptions *Options, BitBuffer *BitB) { // slice_data_partition_c_layer_rbsp
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        uint8_t SliceID                                                             = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        if (Options->SPS->SeperateColorPlane == true) {
            uint8_t ColorPlaneID                                                    = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        }
        if (Options->PPS->RedundantPictureFlag == true) {
            Options->PPS->RedundantPictureCount                                         = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        }
        ParseSliceData(Options, BitB, 4);
        rbsp_slice_trailing_bits(Options, BitB); // BitBuffer_Align(BitB, 1);
    }
    
    void ParseNALSliceNonPartitioned(AVCOptions *Options, BitBuffer *BitB) { // slice_layer_without_partitioning_rbsp
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        ParseNALSliceHeader(Options, BitB);
        ParseNALSliceData(Options, BitB, 0); // TODO: Fix category
        
        BitBuffer_Align(BitB, 1); // rbsp_slice_trailing_bits();
    }
    
    void ParseNALFillerData(AVCOptions *Options, BitBuffer *BitB) { // filler_data_rbsp
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        while (BitBuffer_PeekBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 8) == 0xFF) {
            BitBuffer_Seek(BitB, 8);
        }
    }
    
    void ParseNALPrefixUnit(AVCOptions *Options, BitBuffer *BitB) { // prefix_nal_unit_rbsp
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        if (Options->NAL->SVCExtensionFlag == true) {
            ParseNALPrefixUnitSVC(Options, BitB);
        }
    }
    
    void ParseNALAccessUnitDelimiter(AVCOptions *Options, BitBuffer *BitB) { // access_unit_delimiter_rbsp
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->Slice->PictureType                                                     = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 3);
        BitBuffer_Align(BitB, 1);
    }
    
    /* Supplemental Enhancement Information */
    void ParseSEIBufferingPeriod(AVCOptions *Options, BitBuffer *BitB) { // buffering_period
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SPS->SeqParamSetID                                                     = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        if (Options->SEI->NalHrdBpPresentFlag == true) {
            for (uint8_t SchedSelIdx = 0; SchedSelIdx <= Options->HRD->NumCodedPictureBuffers; SchedSelIdx++) {
                Options->SEI->InitialCPBRemovalDelay[SchedSelIdx]                       = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, Options->HRD->InitialCPBDelayLength);
                Options->SEI->InitialCPBRemovalDelayOffset[SchedSelIdx]                 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, Options->HRD->InitialCPBDelayLength);
            }
        }
        Options->HRD->VclHrdBpPresentFlag                                               = Options->VUI->VCLHrdParamsPresent > 0 ? 1 : 0;
        if (Options->HRD->VclHrdBpPresentFlag == true) {
            for (uint8_t SchedSelIdx = 0; SchedSelIdx <= Options->HRD->NumCodedPictureBuffers; SchedSelIdx++) {
                Options->SEI->InitialCPBRemovalDelay[SchedSelIdx]                       = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, Options->HRD->InitialCPBDelayLength);
                Options->SEI->InitialCPBRemovalDelayOffset[SchedSelIdx]                 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, Options->HRD->InitialCPBDelayLength);
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
    
    void ParseSEIPictureTiming(AVCOptions *Options, BitBuffer *BitB) { // pic_timing
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        if (Options->SEI->CpbDpbDelaysPresentFlag == true) {
            Options->SEI->CPBRemovalDelay                                               = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, Options->HRD->CBPDelay);
            Options->SEI->DPBOutputDelay                                                = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, Options->HRD->DBPDelay);
        }
        if (Options->VUI->PicStructPresent[0] == true) {
            Options->SEI->PicStruct                                                     = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 4);
            uint8_t NumClockTS = GetClockTS(Options->SEI->PicStruct);
            for (uint8_t Time = 0; Time < NumClockTS; Time++) {
                Options->SEI->ClockTimestampFlag[Time]                                  = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                if (Options->SEI->ClockTimestampFlag[Time] == true) {
                    Options->SEI->CTType                                                = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 2);
                    Options->SEI->NuitFieldBasedFlag                                    = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                    Options->SEI->CountingType                                          = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 5);
                    Options->SEI->FullTimestampFlag                                     = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                    Options->SEI->CountDroppedFlag                                      = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                    Options->SEI->NFrames                                               = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 8);
                    if (Options->SEI->FullTimestampFlag == true) {
                        Options->SEI->Seconds                                           = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 6);
                        Options->SEI->Minutes                                           = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 6);
                        Options->SEI->Hours                                             = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 5);
                    } else {
                        Options->SEI->SecondsFlag                                       = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                        if (Options->SEI->SecondsFlag == true) {
                            Options->SEI->Seconds                                       = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 6);
                            Options->SEI->MinutesFlag                                   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                            if (Options->SEI->MinutesFlag == true) {
                                Options->SEI->Minutes                                   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 6);
                                Options->SEI->HoursFlag                                 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                                if (Options->SEI->HoursFlag == true) {
                                    Options->SEI->Hours                                 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 5);
                                }
                            }
                        }
                        if (Options->HRD->TimeOffsetSize > 0) {
                            Options->SEI->TimeOffset                                    = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, Options->HRD->TimeOffsetSize);
                        }
                    }
                }
            }
        }
    }
    
    void ParseSEIPanScan(AVCOptions *Options, BitBuffer *BitB) { // pan_scan_rect
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->PanScanID                                                         = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        Options->SEI->DisablePanScanFlag                                                = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (Options->SEI->DisablePanScanFlag == false) {
            Options->SEI->PanScanCount                                                  = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
            for (uint8_t PanScan = 0; PanScan < Options->SEI->PanScanCount; PanScan++) {
                Options->SEI->PanScanOffsetLeft[PanScan]                                = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
                Options->SEI->PanScanOffsetRight[PanScan]                               = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
                Options->SEI->PanScanOffsetTop[PanScan]                                 = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
                Options->SEI->PanScanOffsetBottom[PanScan]                              = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
            }
            Options->SEI->PanScanRepitionPeriod                                         = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        }
    }
    
    void ParseSEIFiller(AVCOptions *Options, BitBuffer *BitB) { // filler_payload
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        BitBuffer_Seek(BitB, Bytes2Bits(Options->SEI->SEISize));
    }
    
    void ParseSEIRegisteredUserData(AVCOptions *Options, BitBuffer *BitB) { // user_data_registered_itu_t_t35
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        uint8_t CountryCodeSize = 0;
        Options->SEI->CountryCode                                                       = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 8);
        if (Options->SEI->CountryCode != 0xFF) {
            CountryCodeSize                                                        += 1;
        } else {
            CountryCodeSize                                                        += 2;
            Options->SEI->CountryCode                                                 <<= 8;
            Options->SEI->CountryCode                                                  += BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 8);
        }
    }
    
    void ParseSEIUnregisteredUserData(AVCOptions *Options, BitBuffer *BitB) { // user_data_unregistered
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->UnregisteredUserDataUUID = ReadUUID(BitB); // DC45E9BD-E6D9-48B7-962C-D820D923EEEF, x264 UserID.
        BitBuffer_Seek(BitB, Bytes2Bits(Options->SEI->SEISize - BitIOBinaryUUIDSize));
    }
    
    void ParseSEIRecoveryPoint(AVCOptions *Options, BitBuffer *BitB) { // recovery_point
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->RecoveryFrameCount                                                = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        Options->SEI->ExactMatchFlag                                                    = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        Options->SEI->BrokenLinkFlag                                                    = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        Options->SEI->ChangingSliceGroupIDC                                             = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 2);
    }
    
    void ParseSEIRepetitiveReferencePicture(AVCOptions *Options, BitBuffer *BitB) { // dec_ref_pic_marking_repetition
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->OriginalIDRFlag                                                   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        Options->SEI->OriginalFrameNum                                                  = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        if (Options->SPS->OnlyMacroBlocksInFrame == false) {
            Options->SEI->OriginalFieldPicFlag                                          = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->SEI->OriginalFieldPicFlag == true) {
                Options->SEI->OriginalBottomFieldFlag                                   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            }
        }
        DecodeRefPicMarking(Options, BitB); // dec_ref_pic_marking();
    }
    
    void ParseSEISparePicture(AVCOptions *Options, BitBuffer *BitB) { // spare_pic
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        uint8_t MapUnitCount = 0;
        
        Options->SEI->TargetFrameNum                                                    = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        Options->SEI->SpareFieldFlag                                                    = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (Options->SEI->SpareFieldFlag == true) {
            Options->SEI->TargetBottomFieldFlag                                         = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        }
        Options->SEI->NumSparePics                                                      = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
        for (uint8_t SparePic = 0; SparePic < Options->SEI->NumSparePics; SparePic++) {
            Options->SEI->DeltaSpareFrameNum[SparePic]                                  = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            if (Options->SEI->SpareFieldFlag == true) {
                Options->SEI->SpareBottomFieldFlag[SparePic]                            = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            }
            Options->SEI->SpareAreaIDC[SparePic]                                        = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            if (Options->SEI->SpareAreaIDC[SparePic] == 1) {
                for (uint8_t MapUnit = 0; MapUnit < Options->PPS->PicSizeInMapUnits; MapUnit++) {
                    Options->SEI->SpareUnitFlag[SparePic][MapUnit]                      = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                }
            } else if (Options->SEI->SpareAreaIDC[SparePic] == 2) {
                while (MapUnitCount < Options->PPS->PicSizeInMapUnits) {
                    Options->SEI->ZeroRunLength[SparePic][MapUnitCount]                 = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                    MapUnitCount += Options->SEI->ZeroRunLength[SparePic][MapUnitCount] + 1;
                }
            }
        }
    }
    
    void ParseSEISceneInfo(AVCOptions *Options, BitBuffer *BitB) { // scene_info
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->SceneInfoPresentFlag                                              = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (Options->SEI->SceneInfoPresentFlag == true) {
            Options->SEI->SceneID                                                       = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            Options->SEI->SceneTransitionType                                           = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            if (Options->SEI->SceneTransitionType > 3) {
                Options->SEI->SecondSceneID                                             = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            }
        }
    }
    
    void ParseSEISubSequenceInfo(AVCOptions *Options, BitBuffer *BitB) { // sub_seq_info
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->SubSequenceLayerNum                                                = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        Options->SEI->SubSequenceID                                                      = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        Options->SEI->FirstRefPicFlag                                                    = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        Options->SEI->LeadingNonRefPicFlag                                               = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        Options->SEI->LastPicFlag                                                        = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        Options->SEI->SubSeqFrameNumFlag                                                 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (Options->SEI->SubSeqFrameNumFlag  == true) {
            Options->SEI->SubSeqFrameNum                                                 = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        }
    }
    
    void ParseSEISubSequenceLayerProperties(AVCOptions *Options, BitBuffer *BitB) { // sub_seq_layer_characteristics
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->NumSubSeqLayers                                                    = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
        for (uint8_t Layer = 0; Layer < Options->SEI->NumSubSeqLayers; Layer++) {
            Options->SEI->AccurateStatisticsFlag                                         = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->SEI->AverageBitRate                                                 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
            Options->SEI->AverageFrameRate                                               = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
        }
    }
    
    void ParseSEISubSequenceProperties(AVCOptions *Options, BitBuffer *BitB) { // sub_seq_characteristics
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->SubSequenceLayerNum                                                = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        Options->SEI->SubSequenceID                                                      = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        Options->SEI->DurationFlag                                                       = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (Options->SEI->DurationFlag == true) {
            Options->SEI->SubSeqDuration                                                 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 32);
        }
        Options->SEI->AverageRateFlag                                                    = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (Options->SEI->AverageRateFlag == true) {
            Options->SEI->AccurateStatisticsFlag                                         = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->SEI->AverageBitRate                                                 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
            Options->SEI->AverageFrameRate                                               = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
        }
        Options->SEI->NumReferencedSubSeqs                                               = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        for (uint16_t SubSequence = 0; SubSequence < Options->SEI->NumReferencedSubSeqs; SubSequence++) {
            Options->SEI->RefSubSeqLayerNum                                              = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            Options->SEI->RefSubSeqID                                                    = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            Options->SEI->RefSubSeqDirection                                             = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        }
    }
    
    void ParseSEIFullFrameFreeze(AVCOptions *Options, BitBuffer *BitB) { // full_frame_freeze
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->FullFrameFreezeRepitionPeriod                                      = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
    }
    
    void ParseSEIFullFrameFreezeRelease(AVCOptions *Options, BitBuffer *BitB) { // full_frame_freeze_release
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        BitBuffer_Seek(BitB, Bytes2Bits(Options->SEI->SEISize));
    }
    
    void ParseSEIFullFrameSnapshot(AVCOptions *Options, BitBuffer *BitB) { // full_frame_snapshot
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->SnapshotID = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
    }
    
    void ParseSEIProgressiveRefinementSegmentStart(AVCOptions *Options, BitBuffer *BitB) { // progressive_refinement_segment_start
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->ProgressiveRefinementID                                            = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        Options->SEI->NumRefinementSteps                                                 = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
    }
    
    void ParseSEIProgressiveRefinementSegmentEnd(AVCOptions *Options, BitBuffer *BitB) { // progressive_refinement_segment_end
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->ProgressiveRefinementID                                            = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
    }
    
    void ParseSEIMotionConstrainedSliceGroupSet(AVCOptions *Options, BitBuffer *BitB) { // motion_constrained_slice_group_set
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->NumSliceGroupsInSet                                                = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
        if (Options->SEI->NumSliceGroupsInSet > 1) {
            for (uint16_t SliceGroup = 0; SliceGroup < Options->SEI->NumSliceGroupsInSet; SliceGroup++) {
                Options->PPS->SliceGroupID[SliceGroup]                                   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, Ceili(log2(Options->PPS->SliceGroups)));
            }
        }
        Options->SEI->ExactSampleValueMatchFlag[0]                                       = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        Options->SEI->PanScanRectFlag                                                    = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (Options->SEI->PanScanRectFlag == true) {
            Options->SEI->PanScanID                                                      = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        }
    }
    
    void ParseSEIFilmGrainCharacteristics(AVCOptions *Options, BitBuffer *BitB) { // film_grain_characteristics
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->FilmGrainCharactisticsCancelFlag                                   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (Options->SEI->FilmGrainCharactisticsCancelFlag == false) {
            Options->SEI->FilmGrainModelID                                               = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 2);
            Options->SEI->SeperateColorDescriptionFlag                                   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->SEI->SeperateColorDescriptionFlag == true) {
                Options->SEI->FilmGrainBitDepthLuma                                      = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 3) + 8;
                Options->SEI->FilmGrainBitDepthChroma                                    = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 3) + 8;
                Options->SEI->FilmGrainFullRangeFlag                                     = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                Options->SEI->FilmGrainColorPrimaries                                    = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 8);
                Options->SEI->FilmGrainTransferCharacteristics                           = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 8);
                Options->SEI->FilmGrainMatrixCoefficents                                 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 8);
            }
            Options->SEI->BlendingModeID                                                 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 2);
            Options->SEI->Scalefactor                                                    = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 4);
            for (uint8_t Channel = 0; Channel < 3; Channel++) {
                Options->SEI->CompModelPresent[Channel]                                  = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            }
            for (uint8_t Channel = 0; Channel < 3; Channel++) {
                if (Options->SEI->CompModelPresent[Channel] == true) {
                    Options->SEI->NumIntensityIntervals[Channel]                         = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 8) + 1;
                    Options->SEI->NumModelValues[Channel]                                = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 3) + 1;
                    for (uint16_t Intensity = 0; Intensity < Options->SEI->NumIntensityIntervals[Channel]; Intensity++) {
                        Options->SEI->IntensityIntervalLowerBound[Channel][Intensity]    = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 8);
                        Options->SEI->IntensityIntervalUpperBound[Channel][Intensity]    = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 8);
                        for (uint8_t ModelValue = 0; ModelValue < Options->SEI->NumModelValues[Channel]; ModelValue++) {
                            Options->SEI->CompModelValue[Channel][Intensity][ModelValue] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
                        }
                    }
                }
            }
            Options->SEI->FilmGrainCharacteristicsRepetitionPeriod                       = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        }
    }
    
    void ParseSEIDeblockingFilterDisplayPreference(AVCOptions *Options, BitBuffer *BitB) { // deblocking_filter_display_preference
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->DeblockingDisplayPreferenceCancelFlag           = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (Options->SEI->DeblockingDisplayPreferenceCancelFlag == false) {
            Options->SEI->DisplayBeforeDeblockingFlag                 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->SEI->DecodeFrameBufferingConstraintFlag          = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->SEI->DeblockingDisplayPreferenceRepetitionPeriod = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        }
    }
    
    void ParseSEIStereoVideoInfo(AVCOptions *Options, BitBuffer *BitB) { // stereo_video_info
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->FieldViewsFlag               = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (Options->SEI->FieldViewsFlag == true) {
            Options->SEI->TopFieldIsLeftViewFlag       = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        } else {
            Options->SEI->CurrentFrameIsLeftViewFlag   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->SEI->NextFrameIsDependentViewFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        }
        Options->SEI->LeftViewSelfContainedFlag    = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        Options->SEI->RightViewSelfContainedFlag   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
    }
    
    void ParseSEIPostFilterHint(AVCOptions *Options, BitBuffer *BitB) { // post_filter_hint
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->FilterHintSizeY     = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        Options->SEI->FilterHintSizeX     = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        Options->SEI->FilterHintType      = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 2);
        for (uint8_t Color = 0; Color < 3; Color++) {
            for (uint8_t CY = 0; CY < Options->SEI->FilterHintSizeY; CY++) {
                for (uint8_t CX = 0; CX < Options->SEI->FilterHintSizeX; CX++) {
                    Options->SEI->FilterHint[Color][CY][CX] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
                }
            }
        }
        Options->SPS->AdditionalExtensionFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
    }
    
    void ParseSEIToneMappingInfo(AVCOptions *Options, BitBuffer *BitB) { // tone_mapping_info
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->ToneMapID  = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        Options->SEI->ToneMapCancelFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (Options->SEI->ToneMapCancelFlag == false) {
            Options->SEI->ToneMapRepetitionPeriod = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            Options->SEI->CodedDataBitDepth       = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 8);
            Options->SEI->TargetBitDepth   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 8);
            Options->SEI->ToneMapModelID = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            if (Options->SEI->ToneMapModelID == 0) {
                Options->SEI->ToneMinValue = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 32);
                Options->SEI->ToneMaxValue = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 32);
            } else if (Options->SEI->ToneMapModelID == 1) {
                Options->SEI->SigmoidMidpoint = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 32);
                Options->SEI->SigmoidWidth    = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 32);
            } else if (Options->SEI->ToneMapModelID == 2) {
                for (uint16_t I = 0; I < (1 << Options->SEI->TargetBitDepth); I++) {
                    Options->SEI->StartOfCodedInterval[I] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, ((Options->SEI->CodedDataBitDepth + 7) >> 3) << 3);
                }
            } else if (Options->SEI->ToneMapModelID == 3) {
                Options->SEI->NumPivots = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
                for (uint16_t I = 0; I < Options->SEI->NumPivots; I++) {
                    Options->SEI->CodedPivotValue[I] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, ((Options->SEI->CodedDataBitDepth + 7) >> 3) << 3);
                    Options->SEI->TargetPivotValue[I] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, ((Options->SEI->TargetBitDepth + 7) >> 3) << 3);
                }
            } else if (Options->SEI->ToneMapModelID == 4) {
                Options->SEI->CameraISOSpeedIDC = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 8);
                if (Options->SEI->CameraISOSpeedIDC == ExtendedISO) {
                    Options->SEI->CameraISOSpeed = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 32);
                }
                Options->SEI->ExposureIndexIDC = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 8);
                if (Options->SEI->ExposureIndexIDC == ExtendedISO) {
                    Options->SEI->ExposureIndexValue = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 32);
                }
                Options->SEI->ExposureCompensationSignFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                Options->SEI->ExposureCompensationNumerator = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
                Options->SEI->ExposureCompensationDenominatorIDC = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
                Options->SEI->RefScreenLuminanceWhite = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 32);
                Options->SEI->ExtendedRangeWhiteLevel = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 32);
                Options->SEI->NominalBlackLumaCode    = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
                Options->SEI->NominalWhiteLumaCode    = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
                Options->SEI->ExtendedWhiteLumaCode   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
            }
        }
    }
    
    void ParseSEIScalabilityInfo(AVCOptions *Options, BitBuffer *BitB) { // scalability_info
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->TemporalIDNestingFlag                      = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        Options->SEI->PriorityLayerInfoPresent                   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        Options->SEI->PriorityIDSettingFlag                      = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        Options->SEI->NumLayers                                  = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
        for (uint8_t Layer = 0; Layer < Options->SEI->NumLayers; Layer++) {
            Options->SEI->LayerID[Layer]                         = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            Options->NAL->PriorityID[Layer]                      = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 6);
            Options->NAL->IsDisposable[Layer]                    = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->NAL->DependencyID[Layer]                    = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 3);
            Options->NAL->QualityID[0][Layer]                    = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 4); // FIXME; is this correct?
            Options->NAL->TemporalID[Layer]                      = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 3);
            Options->SEI->SubPicLayerFlag[Layer]                 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->SEI->SubRegionLayerFlag[Layer]              = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->SEI->IROIDivisionFlag[Layer]                = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->SEI->ProfileLevelInfoPresentFlag[Layer]     = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->SEI->BitRateInfoPresent[Layer]              = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->SEI->FrameRateInfoPresent[Layer]            = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->SEI->FrameSizeInfoPresentFlag[Layer]        = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->SEI->LayerDependencyInfoPresent[Layer]      = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->SEI->ParameterSetsInfoPresent[Layer]        = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->SEI->BitstreamRestrictionInfoPresent[Layer] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->SEI->ExactInterLayerPredFlag[Layer]         = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if ((Options->SEI->SubPicLayerFlag[Layer] == true) || (Options->SEI->IROIDivisionFlag[Layer] == true)) {
                Options->SEI->ExactSampleValueMatchFlag[Layer]   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            }
            Options->SEI->LayerConversionFlag[Layer]             = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->SEI->LayerOutputFlag[Layer]                 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->SEI->ProfileLevelInfoPresentFlag[Layer] == true) {
                Options->SEI->LayerProfileLevelID[Layer]         = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 24);
            }
            if (Options->SEI->BitRateInfoPresent[Layer] == true) {
                Options->SEI->AvgBitRate[Layer]                  = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
                Options->SEI->MaxBitRate[Layer]                  = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
                Options->SEI->MaxBitRateRepresentation[Layer]    = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
                Options->SEI->MaxBitRateWindow[Layer]            = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
            }
            if (Options->SEI->FrameRateInfoPresent[Layer] == true) {
                Options->SEI->ConstantFrameRateIDC[Layer]        = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 2);
                Options->SEI->AvgFrameRate[Layer]                = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
            }
            if ((Options->SEI->FrameSizeInfoPresentFlag[Layer] == true) || (Options->SEI->IROIDivisionFlag[Layer] == true)) {
                Options->SEI->FrameWidthInMacroBlocks[Layer]     = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
                Options->SEI->FrameHeightInMacroBlocks[Layer]    = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
            }
            if (Options->SEI->SubRegionLayerFlag[Layer] == true) {
                Options->SEI->BaseRegionLayerID[Layer]           = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                Options->SEI->DynamicRectFlag[Layer]             = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                if (Options->SEI->DynamicRectFlag[Layer] == false) {
                    Options->SEI->HorizontalOffset[Layer]        = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
                    Options->SEI->VerticalOffset[Layer]          = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
                    Options->SEI->RegionWidth[Layer]             = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
                    Options->SEI->RegionHeight[Layer]            = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
                }
            }
            if (Options->SEI->SubPicLayerFlag[Layer] == true) {
                Options->SEI->ROIID[Layer]                       = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            }
            if (Options->SEI->IROIDivisionFlag[Layer] == true) {
                Options->SEI->IROIGridFlag[Layer]                = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                if (Options->SEI->IROIGridFlag[Layer] == true) {
                    Options->SEI->GridWidthInMacroBlocks[Layer]  = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
                    Options->SEI->GridHeightInMacroBlocks[Layer] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
                } else {
                    Options->SEI->NumROIs[Layer]                       = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
                    for (uint8_t ROI = 0; ROI < Options->SEI->NumROIs[Layer]; ROI++) {
                        Options->SEI->FirstMacroBlockInROI[Layer][ROI] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                        Options->SEI->ROIWidthInMacroBlock[Layer][ROI] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
                        Options->SEI->ROIHeightInMacroBlock[Layer][ROI] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
                        
                    }
                }
            }
            if (Options->SEI->LayerDependencyInfoPresent[Layer] == true) {
                Options->SEI->NumDependentLayers[Layer]          = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                for (uint8_t DependentLayer = 0; DependentLayer < Options->SEI->NumDependentLayers[Layer]; DependentLayer++) {
                    Options->SEI->DirectlyDependentLayerIDDelta[Layer][DependentLayer] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
                }
            } else {
                Options->SEI->LayerDepInfoSourceLayerIDDelta[Layer] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
                if (Options->SEI->ParameterSetsInfoPresent[Layer] == true) {
                    Options->SEI->NumSequenceParameterSets[Layer] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                    for (uint8_t SPS = 0; SPS < Options->SEI->NumSequenceParameterSets[Layer]; SPS++) {
                        Options->SEI->SPSIDDelta[Layer][SPS] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                    }
                    Options->SEI->NumSubsetSPS[Layer]    = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                    for (uint8_t SubsetSPS = 0; SubsetSPS < Options->SEI->NumSubsetSPS[Layer]; SubsetSPS++) {
                        Options->SEI->SubsetSPSIDDelta[Layer][SubsetSPS] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                    }
                    Options->SEI->NumPicParameterSets[Layer] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
                    for (uint8_t PicParameterSet = 0; PicParameterSet < Options->SEI->NumPicParameterSets[Layer]; PicParameterSet++) {
                        Options->SEI->PicParameterSetIDDelta[Layer][PicParameterSet] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                    }
                } else {
                    Options->SEI->PPSInfoSrcLayerIDDelta[Layer] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                }
                if (Options->SEI->BitstreamRestrictionInfoPresent[Layer] == true) {
                    Options->VUI->MotionVectorsOverPicBoundaries[Layer] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                    Options->VUI->MaxBytesPerPicDenom[Layer]            = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                    Options->VUI->MaxBitsPerMarcoBlockDenom[Layer]      = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                    Options->VUI->MaxMotionVectorLength[Layer]          = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                    Options->VUI->MaxMotionVectorHeight[Layer]          = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                    Options->VUI->MaxReorderFrames[Layer]               = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                    Options->VUI->MaxFrameBuffer[Layer]                 = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                }
                if (Options->SEI->LayerConversionFlag[Layer] == true) {
                    Options->SEI->ConversionTypeIDC[Layer]              = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                    for (uint8_t J = 0; J < 2; J++) {
                        Options->SEI->RewritingInfoFlag[Layer][J]       = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                        if (Options->SEI->RewritingInfoFlag[Layer][J] == true) {
                            Options->SEI->RewritingProfileLevelIDC[Layer][J] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 24);
                            Options->SEI->RewritingAverageBitrate[Layer][J]  = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
                            Options->SEI->RewritingMaxBitrate[Layer][J]      = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
                        }
                    }
                }
            }
        }
        if (Options->SEI->PriorityLayerInfoPresent == true) {
            Options->SEI->NumDependencyLayersForPriorityLayer = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1) + 1;
            for (uint8_t DependentLayer = 0; DependentLayer < Options->SEI->NumDependencyLayersForPriorityLayer; DependentLayer++) {
                Options->SEI->PriorityDependencyID[DependentLayer] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 3);
                Options->SEI->NumPriorityLayers[DependentLayer]    = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
                for (uint8_t PriorityLayer = 0; PriorityLayer < Options->SEI->NumPriorityLayers[DependentLayer]; PriorityLayer++) {
                    Options->SEI->PriorityLayerID[DependentLayer][PriorityLayer]   = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                    Options->SEI->PriorityLevelIDC[DependentLayer][PriorityLayer]  = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 24);
                    Options->SEI->PriorityLevelAvgBitrate[DependentLayer][PriorityLayer]  = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
                    Options->SEI->PriorityLevelMaxBitrate[DependentLayer][PriorityLayer]  = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
                }
            }
        }
        if (Options->SEI->PriorityIDSettingFlag == true) {
            Options->SEI->PriorityIDURIIndex = 0;
            Options->SEI->PriorityIDSettingURI = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 8);
            for (uint8_t StringByte = Options->SEI->PriorityIDSettingURI; StringByte > 0; StringByte--) {
                Options->SEI->PriorityIDSettingURIString[StringByte] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 8);
            }
        }
    }
    
    void ParseSEISubPictureScalableLayer(AVCOptions *Options, BitBuffer *BitB) { // sub_pic_scalable_layer
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->LayerID[0] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
    }
    
    void ParseSEINonRequiredLayerRep(AVCOptions *Options, BitBuffer *BitB) { // non_required_layer_rep
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->NumInfoEntries = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
        for (uint32_t InfoEntry = 0; InfoEntry < Options->SEI->NumInfoEntries; InfoEntry++) {
            Options->SEI->EntryDependencyID[InfoEntry] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 3);
            Options->SEI->NumNonRequiredLayerReps[InfoEntry] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
            for (uint8_t LayerRep = 0; LayerRep < Options->SEI->NumNonRequiredLayerReps[InfoEntry]; LayerRep++) {
                Options->SEI->NonRequiredLayerRepDependencyID[InfoEntry][LayerRep] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 3);
                Options->SEI->NonRequiredLayerRepQualityID[InfoEntry][LayerRep] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 4);
            }
        }
    }
    
    void ParseSEIPriorityLayerInfo(AVCOptions *Options, BitBuffer *BitB) { // priority_layer_info
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->PriorityDependencyID[0] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 3);
        Options->SEI->NumPriorityIDs          = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 4);
        for (uint8_t PriorityID = 0; PriorityID < Options->SEI->NumPriorityIDs; PriorityID++) {
            Options->SEI->AltPriorityIDs[PriorityID] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 6);
        }
    }
    
    void ParseSEILayersNotPresent(AVCOptions *Options, BitBuffer *BitB) { // layers_not_present
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->NumLayersNotPresent = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        for (uint8_t Layer = 0; Layer < Options->SEI->NumLayersNotPresent; Layer++) {
            Options->SEI->LayerID[Layer] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        }
    }
    
    void ParseSEILayerDependencyChange(AVCOptions *Options, BitBuffer *BitB) { // layer_dependency_change
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->NumLayers = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
        for (uint8_t Layer = 0; Layer < Options->SEI->NumLayers; Layer++) {
            Options->SEI->LayerID[Layer] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            Options->SEI->LayerDependencyInfoPresent[Layer] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->SEI->LayerDependencyInfoPresent[Layer] == true) {
                Options->SEI->NumDependentLayers[Layer] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                for (uint8_t DependentLayer = 0; DependentLayer < Options->SEI->NumDependentLayers[Layer]; DependentLayer++) {
                    Options->SEI->DirectlyDependentLayerIDDelta[Layer][DependentLayer] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
                }
            } else {
                Options->SEI->LayerDepInfoSourceLayerIDDelta[Layer] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
            }
        }
    }
    
    void ParseSEIScalableNesting(AVCOptions *Options, BitBuffer *BitB) { // scalable_nesting
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->AllLayerRepresentationsInAUFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (Options->SEI->AllLayerRepresentationsInAUFlag == false) {
            Options->SEI->NumLayerRepresentations = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
            for (uint8_t LayerRepresentation = 0; LayerRepresentation < Options->SEI->NumLayerRepresentations; LayerRepresentation++) {
                Options->SEI->SEIDependencyID[LayerRepresentation] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 3);
                Options->SEI->SEIQualityID[LayerRepresentation]    = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 4);
            }
            Options->SEI->SEITemporalID[0] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 3);
        }
        BitBuffer_Align(BitB, 1);
        ParseSEIMessage(Options, BitB); // sei_message();
    }
    
    void ParseSEIBaseLayerTemporalHRD(AVCOptions *Options, BitBuffer *BitB) { // base_layer_temporal_hrd
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->NumTemporalLayersInBaseLayer                 = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
        for (uint8_t TemporalLayer = 0; TemporalLayer < Options->SEI->NumTemporalLayersInBaseLayer; TemporalLayer++) {
            Options->SEI->SEITemporalID[TemporalLayer]             = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 3);
            Options->SEI->SEITimingInfoPresent[TemporalLayer]      = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->SEI->SEITimingInfoPresent[TemporalLayer] == true) {
                Options->SEI->SEIUnitsInTick[TemporalLayer]        = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 32);
                Options->SEI->SEITimeScale[TemporalLayer]          = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 32);
                Options->SEI->SEIFixedFrameRateFlag[TemporalLayer] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            }
            Options->SEI->SEINALHRDParamsPresentFlag[TemporalLayer] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->SEI->SEINALHRDParamsPresentFlag[TemporalLayer] == true) {
                ParseHypotheticalReferenceDecoder(Options, BitB);
            }
            Options->SEI->SEIVCLHRDParamsPresentFlag[TemporalLayer] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->SEI->SEIVCLHRDParamsPresentFlag[TemporalLayer] == true) {
                ParseHypotheticalReferenceDecoder(Options, BitB);
            }
            if ((Options->SEI->SEINALHRDParamsPresentFlag[TemporalLayer] == true) || (Options->SEI->SEIVCLHRDParamsPresentFlag[TemporalLayer] == true)) {
                Options->SEI->SEILowDelayHRDFlag[TemporalLayer] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            }
            Options->SEI->SEIPicStructPresentFlag[TemporalLayer] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        }
    }
    
    void ParseSEIQualityLayerIntegrityCheck(AVCOptions *Options, BitBuffer *BitB) { // quality_layer_integrity_check
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->NumInfoEntries = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
        for (uint8_t IntegrityCheck = 0; IntegrityCheck < Options->SEI->NumInfoEntries; IntegrityCheck++) {
            Options->SEI->EntryDependencyID[IntegrityCheck] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 3);
            Options->SEI->SEIQualityLayerCRC[IntegrityCheck] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
        }
    }
    
    void ParseSEIRedundantPicProperty(AVCOptions *Options, BitBuffer *BitB) { // redundant_pic_property
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->NumDependencyIDs = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
        for (uint8_t DependencyID = 0; DependencyID < Options->SEI->NumDependencyIDs; DependencyID++) {
            Options->NAL->DependencyID[DependencyID]  = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 3);
            Options->SEI->NumQualityIDs[DependencyID] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
            for (uint8_t QualityID = 0; QualityID < Options->SEI->NumQualityIDs[DependencyID]; QualityID++) {
                Options->NAL->QualityID[DependencyID][QualityID] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 4);
                Options->SEI->NumRedundantPics[DependencyID][QualityID] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
                for (uint8_t RedundantPic = 0; RedundantPic < Options->SEI->NumRedundantPics[DependencyID][QualityID]; RedundantPic++) {
                    Options->SEI->RedundantPicCount[DependencyID][QualityID][RedundantPic] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
                    Options->SEI->RedundantPicsMatch[DependencyID][QualityID][RedundantPic] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                    if (Options->SEI->RedundantPicsMatch[DependencyID][QualityID][RedundantPic] == false) {
                        Options->SEI->MBTypeMatchFlag[DependencyID][QualityID][RedundantPic] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                        Options->SEI->MotionMatchFlag[DependencyID][QualityID][RedundantPic] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                        Options->SEI->ResidualMatchFlag[DependencyID][QualityID][RedundantPic] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                        Options->SEI->IntraSamplesMatchFlag[DependencyID][QualityID][RedundantPic] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                    }
                }
            }
        }
    }
    
    void ParseSEITemporalDependencyRepresentationIndex(AVCOptions *Options, BitBuffer *BitB) { // tl0_dep_rep_index
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->TemporalDependencyRepresentationIndexLevel0 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 8);
        Options->SEI->EffectiveIDRPicID                           = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
    }
    
    void ParseSEITemporalLevelSwitchingPoint(AVCOptions *Options, BitBuffer *BitB) { // tl_switching_point
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->DeltaFrameNum = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
    }
    
    void ParseSEIParallelDecodingInfo(AVCOptions *Options, BitBuffer *BitB) { // parallel_decoding_info
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SPS->SeqParamSetID = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        for (uint8_t View = 0; View <= Options->SPS->ViewCount; View++) {
            if (Options->NAL->IsAnchorPicture == true) {
                for (uint8_t AnchorRef = 0; AnchorRef < Options->SPS->AnchorRefsCount[0][View]; AnchorRef++) {
                    Options->SEI->PDIInitDelayAnchor[0][View][AnchorRef] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 2;
                }
                for (uint8_t AnchorRef = 0; AnchorRef < Options->SPS->AnchorRefsCount[1][View]; AnchorRef++) {
                    Options->SEI->PDIInitDelayAnchor[1][View][AnchorRef] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 2;
                }
            } else {
                for (uint8_t NonAnchorRef = 0; NonAnchorRef < Options->SPS->NonAnchorRefCount[0][View]; NonAnchorRef++) {
                    Options->SEI->PDIInitDelayNonAnchor[0][View][NonAnchorRef] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 2;
                }
                for (uint8_t NonAnchorRef = 0; NonAnchorRef < Options->SPS->NonAnchorRefCount[1][View]; NonAnchorRef++) {
                    Options->SEI->PDIInitDelayNonAnchor[1][View][NonAnchorRef] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 2;
                }
            }
        }
    }
    
    void ParseSEIMVCScalableNesting(AVCOptions *Options, BitBuffer *BitB) { // mvc_scalable_nesting
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->OperationPointFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (Options->SEI->OperationPointFlag == false) {
            Options->SEI->AllViewComponentsInAUFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->SEI->AllViewComponentsInAUFlag == false) {
                Options->SEI->NumViewComponents = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
                for (uint8_t ViewComponent = 0; ViewComponent < Options->SEI->NumViewComponents; ViewComponent++) {
                    Options->SEI->SEIViewID[ViewComponent] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 10);
                }
            }
        } else {
            Options->SEI->NumViewComponentsOp = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
            for (uint8_t ViewComponent = 0; ViewComponent < Options->SEI->NumViewComponentsOp; ViewComponent++) {
                Options->SEI->SEIOpViewID[ViewComponent] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 10);
            }
            Options->SEI->SEIOpTemporalID = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 3);
        }
        BitBuffer_Align(BitB, 1);
        ParseSEIMessage(Options, BitB); // sei_message();
    }
    
    void ParseSEIViewScalabilityInfo(AVCOptions *Options, BitBuffer *BitB) { // view_scalability_info FIXME: FINISH THIS FUNCTION!!!
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->NumOperationPoints                         = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
        for (uint8_t OperationPoint = 0; OperationPoint < Options->SEI->NumOperationPoints; OperationPoint++) {
            Options->SEI->OperationPointID[OperationPoint]       = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            Options->NAL->PriorityID[OperationPoint]             = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 5);
            Options->NAL->TemporalID[OperationPoint]             = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 3);
            Options->SEI->NumTargetOutputViews[OperationPoint]   = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
            for (uint8_t OutputView = 0; OutputView < Options->SEI->NumTargetOutputViews[OperationPoint]; OutputView++) {
                Options->SPS->ViewID[OperationPoint][OutputView] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            }
            Options->SEI->ProfileLevelInfoPresentFlag[0]         = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->SEI->BitRateInfoPresent[0]                  = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->SEI->FrameRateInfoPresent[0]                = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->SEI->NumTargetOutputViews[OperationPoint] == false) {
                Options->SEI->ViewDependencyInfoPresent[OperationPoint] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            }
            Options->SEI->ParameterSetsInfoPresent[OperationPoint]        = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->SEI->BitstreamRestrictionInfoPresent[OperationPoint] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->SEI->ProfileLevelInfoPresentFlag[0] == true) {
                Options->SEI->OpProfileLevelIDC[OperationPoint]           = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 24);
            }
            if (Options->SEI->BitstreamRestrictionInfoPresent[OperationPoint] == true) {
                Options->SEI->AvgBitRate[OperationPoint]                  = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
                Options->SEI->MaxBitRate[OperationPoint]                  = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
                Options->SEI->MaxBitRateWindow[OperationPoint]            = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
            }
            if (Options->SEI->FrameRateInfoPresent[0] == true) {
                Options->SEI->ConstantFrameRateIDC[OperationPoint]        = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 2);
                Options->SEI->AvgFrameRate[OperationPoint]                = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
            }
            if (Options->SEI->ViewDependencyInfoPresent[OperationPoint] == true) {
                Options->SEI->NumDirectlyDependentViews[OperationPoint]   = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                for (uint8_t DirectDependentView = 0; DirectDependentView < Options->SEI->NumDirectlyDependentViews[OperationPoint]; DirectDependentView++) {
                    Options->SEI->DirectlyDependentViewID[OperationPoint][DirectDependentView] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                }
            } else {
                Options->SEI->ViewDependencyInfoSrcOpID[OperationPoint]   = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            }
            if (Options->SEI->ParameterSetsInfoPresent[OperationPoint] == true) {
                Options->SEI->NumSequenceParameterSets[OperationPoint] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                for (uint8_t SPS = 0; SPS < Options->SEI->NumSequenceParameterSets[OperationPoint]; SPS++) {
                    Options->SEI->SPSIDDelta[OperationPoint][SPS] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                }
                Options->SEI->NumSubsetSPS[OperationPoint] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                for (uint8_t SubsetSPS = 0; SubsetSPS < Options->SEI->NumSubsetSPS[OperationPoint]; SubsetSPS++) {
                    Options->SEI->SubsetSPSIDDelta[OperationPoint][SubsetSPS] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                }
                Options->SEI->NumPicParameterSets[OperationPoint] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
                for (uint8_t PPS = 0; PPS < Options->SEI->NumPicParameterSets[OperationPoint]; PPS++) {
                    Options->SEI->PicParameterSetIDDelta[OperationPoint][PPS] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                }
            } else {
                Options->SEI->PPSInfoSrcLayerIDDelta[OperationPoint] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            }
            if (Options->SEI->BitstreamRestrictionInfoPresent[OperationPoint] == true) {
                Options->VUI->MotionVectorsOverPicBoundaries[OperationPoint]  = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                Options->VUI->MaxBytesPerPicDenom[OperationPoint]             = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                Options->VUI->MaxBitsPerMarcoBlockDenom[OperationPoint]       = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                Options->VUI->MaxMotionVectorLength[OperationPoint]           = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                Options->VUI->MaxMotionVectorHeight[OperationPoint]           = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                Options->VUI->MaxReorderFrames[OperationPoint]                = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                Options->VUI->MaxFrameBuffer[OperationPoint]                  = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            }
            if (Options->SEI->LayerConversionFlag[OperationPoint] == true) {
                Options->SEI->ConversionTypeIDC[OperationPoint] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                for (uint8_t J = 0; J < 2; J++) {
                    Options->SEI->RewritingInfoFlag[OperationPoint][J] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                    for (uint8_t RewriteInfo = 0; RewriteInfo < Options->SEI->RewritingInfoFlag[OperationPoint][J]; RewriteInfo++) {
                        Options->SEI->RewritingProfileLevelIDC[OperationPoint][J] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 24);
                        Options->SEI->RewritingAverageBitrate[OperationPoint][J]  = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
                        Options->SEI->RewritingMaxBitrate[OperationPoint][J]      = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
                    }
                }
            }
        }
        if (Options->SEI->PriorityLayerInfoPresent == true) {
            Options->SEI->NumDependencyLayersForPriorityLayer                             = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
            for (uint8_t DependencyLayer = 0; DependencyLayer < Options->SEI->NumDependencyLayersForPriorityLayer; DependencyLayer++) {
                Options->SEI->PriorityDependencyID[DependencyLayer]                       = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 3);
                Options->SEI->NumPriorityLayers[DependencyLayer]                          = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
                for (uint8_t PriorityLayer = 0; PriorityLayer < Options->SEI->NumPriorityLayers[DependencyLayer]; PriorityLayer++) {
                    Options->SEI->PriorityLayerID[DependencyLayer][PriorityLayer]         = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                    Options->SEI->PriorityLevelIDC[DependencyLayer][PriorityLayer]        = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 24);
                    Options->SEI->PriorityLevelAvgBitrate[DependencyLayer][PriorityLayer] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
                    Options->SEI->PriorityLevelMaxBitrate[DependencyLayer][PriorityLayer] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
                }
            }
        }
        if (Options->SEI->PriorityIDSettingFlag == true) {
            Options->SEI->PriorityIDURIIndex = 0;
            Options->SEI->PriorityIDSettingURI = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 8);
            for (uint8_t StringByte = Options->SEI->PriorityIDSettingURI; StringByte > 0; StringByte--) {
                Options->SEI->PriorityIDSettingURIString[StringByte] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 8);
            }
        }
    }
    
    void ParseSEIMVCSceneInfo(AVCOptions *Options, BitBuffer *BitB) { // multiview_scene_info
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->MaxDisparity = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
    }
    
    void ParseSEIMVCAcquisitionInfo(AVCOptions *Options, BitBuffer *BitB) { // multiview_acquisition_info
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SPS->ViewCount            = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
        Options->SEI->IntrinsicParamFlag   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        Options->SEI->ExtrinsicParamFlag   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (Options->SEI->IntrinsicParamFlag == true) {
            Options->SEI->IntrinsicParamsEqual = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->SEI->PrecFocalLength      = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            Options->SEI->PrecPrincipalPoint   = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            Options->SEI->PrecSkewFactor       = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            if (Options->SEI->IntrinsicParamsEqual == true) {
                Options->SEI->NumParamSets     = 1;
            } else {
                Options->SEI->NumParamSets     = Options->SPS->ViewCount;
            }
            for (uint8_t ParamSet = 0; ParamSet < Options->SEI->NumParamSets; ParamSet++) { // FIXME: BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 0)
                Options->SEI->SignFocalLength[0][ParamSet]        = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                Options->SEI->ExponentFocalLength[0][ParamSet]    = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 6);
                if (Options->SEI->ExponentFocalLength[0][ParamSet] == 0) {
                    Options->SEI->MantissaFocalLength[0][ParamSet] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, Maximum(0, Options->SEI->PrecFocalLength - 30));
                } else {
                    Options->SEI->MantissaFocalLength[0][ParamSet] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, Maximum(0, Options->SEI->ExponentFocalLength[0][ParamSet] + Options->SEI->PrecFocalLength - 31));
                }
                Options->SEI->SignFocalLength[1][ParamSet]        = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                Options->SEI->ExponentFocalLength[1][ParamSet]    = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 6);
                if (Options->SEI->ExponentFocalLength[1][ParamSet] == 0) {
                    Options->SEI->MantissaFocalLength[1][ParamSet] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, Maximum(0, Options->SEI->PrecFocalLength - 30));
                } else {
                    Options->SEI->MantissaFocalLength[1][ParamSet] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, Maximum(0, Options->SEI->ExponentFocalLength[1][ParamSet] + Options->SEI->PrecFocalLength - 31));
                }
                Options->SEI->SignPrincipalPoint[0][ParamSet]     = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                Options->SEI->ExponentPrincipalPoint[0][ParamSet] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 6);
                if (Options->SEI->ExponentFocalLength[1][ParamSet] == 0) {
                    Options->SEI->MantissaPrincipalPoint[0][ParamSet] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, Maximum(0, Options->SEI->PrecPrincipalPoint - 30));
                } else {
                    Options->SEI->MantissaPrincipalPoint[0][ParamSet] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, Maximum(0, Options->SEI->ExponentPrincipalPoint[0][ParamSet] + Options->SEI->PrecPrincipalPoint - 31));
                }
                Options->SEI->SignPrincipalPoint[1][ParamSet]     = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                Options->SEI->ExponentPrincipalPoint[1][ParamSet] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 6);
                if (Options->SEI->ExponentFocalLength[1][ParamSet] == 0) {
                    Options->SEI->MantissaPrincipalPoint[1][ParamSet] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, Maximum(0, Options->SEI->PrecPrincipalPoint - 30));
                } else {
                    Options->SEI->MantissaPrincipalPoint[1][ParamSet] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, Maximum(0, Options->SEI->ExponentPrincipalPoint[1][ParamSet] + Options->SEI->PrecPrincipalPoint - 31));
                }
                Options->SEI->SignSkewFactor[ParamSet]            = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                Options->SEI->ExponentSkewFactor[ParamSet]        = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 6);
                if (Options->SEI->ExponentSkewFactor[ParamSet] == 0) {
                    Options->SEI->MantissaSkewFactor[ParamSet] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, Maximum(0, Options->SEI->PrecSkewFactor - 30));
                } else {
                    Options->SEI->MantissaSkewFactor[ParamSet] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, Maximum(0, Options->SEI->ExponentSkewFactor[ParamSet] + Options->SEI->PrecSkewFactor - 31));
                }
            }
        }
        if (Options->SEI->ExtrinsicParamFlag == true) {
            for (uint8_t View = 0; View < Options->SPS->ViewCount; View++) {
                for (uint8_t Row = 1; Row <= 3; Row++) {
                    for (uint8_t Column = 1; Column <= 3; Column++) {
                        Options->SEI->SignR[View][Row][Column]     = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                        Options->SEI->ExponentR[View][Row][Column] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 6);
                        Options->SEI->MantissaR[View][Row][Column] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 0);
                    }
                    Options->SEI->SignT[View][Row] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                    Options->SEI->ExponentT[View][Row] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 6);
                    Options->SEI->MantissaT[View][Row] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 0);
                }
            }
        }
    }
    
    void ParseSEINonRequiredViewComponent(AVCOptions *Options, BitBuffer *BitB) { // non_required_view_component
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->NumInfoEntries                               = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
        for (uint8_t InfoEntry = 0; InfoEntry < Options->SEI->NumInfoEntries; InfoEntry++) {
            Options->SEI->ViewOrderIndex[InfoEntry]                = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            Options->SEI->NumNonRequiredViewComponents[InfoEntry]  = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
            for (uint8_t ViewComponent = 0; ViewComponent < Options->SEI->NumNonRequiredViewComponents[InfoEntry]; ViewComponent++) {
                Options->SEI->IndexDelta[InfoEntry][ViewComponent] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
            }
        }
    }
    
    void ParseSEIViewDependencyChange(AVCOptions *Options, BitBuffer *BitB) { // view_dependency_change
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SPS->SeqParamSetID       = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        Options->SEI->AnchorUpdateFlag    = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        Options->SEI->NonAnchorUpdateFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (Options->SEI->AnchorUpdateFlag == true) {
            for (uint8_t View = 0; View < Options->SPS->ViewCount; View++) {
                for (uint8_t AnchorRef = 0; AnchorRef < Options->SPS->AnchorRefsCount[0][View]; AnchorRef++) {
                    Options->SEI->AnchorRefFlag[0][View][AnchorRef] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                }
                for (uint8_t AnchorRef = 0; AnchorRef < Options->SPS->AnchorRefsCount[1][View]; AnchorRef++) {
                    Options->SEI->AnchorRefFlag[1][View][AnchorRef] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                }
            }
        }
        if (Options->SEI->NonAnchorUpdateFlag == true) {
            for (uint8_t View = 0; View < Options->SPS->ViewCount; View++) {
                for (uint8_t NonAnchorRef = 0; NonAnchorRef < Options->SPS->NonAnchorRefCount[0][View][NonAnchorRef]; NonAnchorRef++) {
                    Options->SEI->NonAnchorRefFlag[0][View][NonAnchorRef] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                }
                for (uint8_t NonAnchorRef = 0; NonAnchorRef < Options->SPS->NonAnchorRefCount[1][View][NonAnchorRef]; NonAnchorRef++) {
                    Options->SEI->NonAnchorRefFlag[1][View][NonAnchorRef] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                }
            }
        }
    }
    
    void ParseSEIOperationPointNotPresent(AVCOptions *Options, BitBuffer *BitB) { // operation_point(s)_not_present
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->NumOperationPoints = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
        for (uint8_t OperationPoint = 0; OperationPoint < Options->SEI->NumOperationPoints; OperationPoint++) {
            Options->SEI->OperationPointNotPresentID[OperationPoint] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        }
    }
    
    void ParseSEIBaseViewTemporalHRD(AVCOptions *Options, BitBuffer *BitB) { // base_view_temporal_hrd
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->NumTemporalLayersInBaseView = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
        for (uint8_t TemporalLayer = 0; TemporalLayer < Options->SEI->NumTemporalLayersInBaseView; TemporalLayer++) {
            Options->SEI->SEIMVCTemporalID[TemporalLayer] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 3);
        }
    }
    
    void ParseSEIFramePackingArrangement(AVCOptions *Options, BitBuffer *BitB) { // frame_packing_arrangement
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->FramePackingArrangementID = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        Options->SEI->FramePackingArrangementCancelFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (Options->SEI->FramePackingArrangementCancelFlag == false) {
            Options->SEI->FramePackingArrangementType = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 7);
            Options->SEI->QuincunxSamplingFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->SEI->ContentIntrepretationType = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 6);
            Options->SEI->SpatialFlippingFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->SEI->FlippedFlagFrame0 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->SEI->FieldViewsFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->SEI->TheCurrentFrameIsFrame0 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->SEI->Frame0IsSelfContainedFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->SEI->Frame1IsSelfContainedFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if ((Options->SEI->QuincunxSamplingFlag == false) && (Options->SEI->FramePackingArrangementType != 5)) {
                Options->SEI->Frame0GridPositionX = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 4);
                Options->SEI->Frame0GridPositionY = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 4);
                Options->SEI->Frame1GridPositionX = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 4);
                Options->SEI->Frame1GridPositionY = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 4);
            }
            Options->SEI->FramePackingArrangementReservedByte = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 8);
            Options->SEI->FramePackingArrangementRepetitionPeriod = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        }
        Options->SEI->FramePackingArrangementExtensionFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
    }
    
    void ParseSEIMVCViewPosition(AVCOptions *Options, BitBuffer *BitB) { // multiview_view_position
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SPS->ViewCount = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
        for (uint8_t View = 0; View < Options->SPS->ViewCount; View++) {
            Options->SEI->ViewPosition[View] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        }
        Options->SEI->MVCViewPositionExtensionFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
    }
    
    void ParseSEIDisplayOrientation(AVCOptions *Options, BitBuffer *BitB) { // display_orientation
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->DisplayOrientationCancelFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (Options->SEI->DisplayOrientationCancelFlag == false) {
            Options->SEI->HorizontalFlip = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->SEI->VerticalFlip   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->SEI->CounterClockwiseRotation = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
            Options->SEI->DisplayOrientationRepetitionPeriod = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            Options->SEI->DisplayOrientationExtensionFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        }
    }
    
    void ParseSEIDepthRepresentationInformation(AVCOptions *Options, BitBuffer *BitB) { // depth_representation_info
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->AllViewsEqual                                     = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (Options->SEI->AllViewsEqual == true) {
            Options->SPS->ViewCount                                     = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
        } else {
            Options->SPS->ViewCount                                     = 1;
        }
        Options->SEI->ZNearFlag                                         = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        Options->SEI->ZFarFlag                                          = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if ((Options->SEI->ZNearFlag == true) || (Options->SEI->ZFarFlag == true)) {
            Options->SEI->ZAxisEqualFlag                                = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->SEI->ZAxisEqualFlag == true) {
                Options->SEI->CommonZAxisReferenceView                  = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            }
        }
        Options->SEI->DMinFlag                                          = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        Options->SEI->DMaxFlag                                          = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        Options->SEI->DepthRepresentationType                           = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        for (uint8_t View = 0; View < Options->SPS->ViewCount; View++) {
            Options->SEI->DepthInfoViewID[View]                         = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            if ((Options->SEI->ZNearFlag == true || Options->SEI->ZFarFlag == true) && (Options->SEI->ZAxisEqualFlag == false)) {
                Options->SEI->ZAxisReferenceView[View]                  = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            }
            if (Options->SEI->DMinFlag == true || Options->SEI->DMaxFlag == true) {
                Options->SEI->DisparityReferenceView[View]              = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            }
            if (Options->SEI->ZNearFlag == true) {
                //depth_representation_sei_element(ZNearSign, ZNearExp, ZNearMantissa, ZNearManLen);
                //ParseSEIDepthRepresentationElement(BitB, Options->SEI->ZNearSign, Options->SEI->ZNearExp, Options->SEI->ZNearMantissa, Options->SEI->ZNear)
            }
            if (Options->SEI->ZFarFlag == true) {
                //depth_representation_sei_element(ZFarSign, ZFarExp, ZFarMantissa, ZFarManLen);
            }
            if (Options->SEI->DMinFlag == true) {
                //depth_representation_sei_element(DMinSign, DMinExp, DMinMantissa, DMinManLen);
            }
            if (Options->SEI->DMaxFlag == true) {
                //depth_representation_sei_element(DMaxSign, DMaxExp, DMaxMantissa, DMaxManLen);
            }
        }
        if (Options->SEI->DepthRepresentationType == 3) {
            Options->SEI->DepthNonlinearRepresentation                  = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
            for (uint8_t Index= 0; Index < Options->SEI->DepthNonlinearRepresentation; Index++) {
                Options->SEI->DepthNonlinearRepresentationModel[Index]  = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            }
        }
    }
    
    void ParseSEI3DReferenceDisplayInfo(AVCOptions *Options, BitBuffer *BitB) { // three_dimensional_reference_displays_info
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->TruncationErrorExponent                           = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        Options->SEI->TruncatedWidthExponent                            = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        Options->SEI->ReferenceViewingDistanceFlag                      = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        
        if (Options->SEI->ReferenceViewingDistanceFlag == true) {
            Options->SEI->TruncatedReferenveViewingDistanceExponent     = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        }
        Options->SEI->NumReferenceDisplays                              = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
        for (uint8_t Display= 0; Display < Options->SEI->NumReferenceDisplays; Display++) {
            Options->SEI->ReferenceBaselineExponent[Display]            = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            Options->SEI->ReferenceBaselineMantissa[Display]            = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            Options->SEI->ReferenceDisplayWidthExponent[Display]        = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            Options->SEI->ReferenceDisplayWidthMantissa[Display]        = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            if (Options->SEI->ReferenceViewingDistanceFlag == true) {
                Options->SEI->ReferenceViewingDistanceExponent[Display] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                Options->SEI->ReferenceViewingDistanceMantissa[Display] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            }
            Options->SEI->ShiftPresentFlag[Display]                     = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->SEI->ShiftPresentFlag[Display] == true) {
                Options->SEI->SampleShift[Display]                      = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) - 512;
            }
        }
        Options->SEI->ReferenceDisplays3DFlag                           = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
    }
    
    void ParseSEIDepthTiming(AVCOptions *Options, BitBuffer *BitB) { // depth_timing
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->PerViewDepthTimingFlag                              = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        
        if (Options->SEI->PerViewDepthTimingFlag == true) {
            for (uint8_t View = 0; View < Options->DPS->NumDepthViews; View++) {
                Options->SEI->OffsetLength[View]                          = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 5) + 1;
                Options->SEI->DepthDisplayDelayOffsetFP[View]             = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, Options->SEI->OffsetLength[View]);
                Options->SEI->DepthDisplayDelayOffsetDP[View]             = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, Options->SEI->OffsetLength[View]);
            }
        }
    }
    
    void ParseSEIDepthGridPosition(AVCOptions *Options, BitBuffer *BitB) { // depth_grid_position()
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->DepthGridPositionXFP                                = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 20);
        Options->SEI->DepthGridPositionXDP                                = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 4);
        Options->SEI->DepthGridPositionXSignFlag                          = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        Options->SEI->DepthGridPositionYFP                                = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 20);
        Options->SEI->DepthGridPositionYDP                                = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 4);
        Options->SEI->DepthGridPositionYSignFlag                          = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
    }
    
    void ParseSEIDepthSamplingInfo(AVCOptions *Options, BitBuffer *BitB) { // depth_sampling_info
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->DepthSampleWidthMul                                 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
        Options->SEI->DepthSampleWidthDP                                  = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 4);
        Options->SEI->DepthSampleHeightMul                                = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
        Options->SEI->DepthSampleHeightDP                                 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 4);
        Options->SEI->PerViewDepthTimingFlag                              = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (Options->SEI->PerViewDepthTimingFlag == true) {
            Options->SEI->NumDepthGridViews                               = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
            for (uint8_t DepthGrid = 0; DepthGrid < Options->SEI->NumDepthGridViews; DepthGrid++) {
                Options->SEI->DepthInfoViewID[DepthGrid]                  = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                ParseSEIDepthGridPosition(Options, BitB);
            }
        } else {
            ParseSEIDepthGridPosition(Options, BitB);
        }
    }
    
    void ParseSEIConstrainedDepthParameterSetID(AVCOptions *Options, BitBuffer *BitB) { // constrained_depth_parameter_set_identifier
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->MaxDPSID                                            = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        Options->SEI->MaxDPSIDDiff                                        = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
    }
    
    void ParseSEIMeteringDisplayColorVolume(AVCOptions *Options, BitBuffer *BitB) { // mastering_display_color_volume
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        for (uint8_t Color = 0; Color < 3; Color++) {
            Options->SEI->DisplayPrimariesX[Color]                        = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
            Options->SEI->DisplayPrimariesY[Color]                        = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
        }
        Options->SEI->WhitePointX                                         = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
        Options->SEI->WhitePointY                                         = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 16);
        Options->SEI->MaxDisplayMasteringLuminance                        = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 32);
        Options->SEI->MinDisplayMasteringLuminance                        = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 32);
    }
    
    void ParseSEIMVCDScalableNesting(AVCOptions *Options, BitBuffer *BitB) { // mvcd_scalable_nesting
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SEI->OperationPointFlag                                  = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (Options->SEI->OperationPointFlag == false) {
            Options->SEI->AllViewComponentsInAUFlag                       = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->SEI->AllViewComponentsInAUFlag == false) {
                Options->SEI->NumViewComponents                           = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
                for (uint8_t ViewComponent = 0; ViewComponent < Options->SEI->NumViewComponents; ViewComponent++) {
                    Options->SEI->SEIViewID[ViewComponent]                = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 10);
                    Options->SEI->SEIViewApplicabilityFlag[ViewComponent] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                }
            }
        } else {
            Options->SEI->SEIOpTextureOnlyFlag                            = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            Options->SEI->NumViewComponents                               = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
            for (uint8_t ViewComponent = 0; ViewComponent < Options->SEI->NumViewComponents; ViewComponent++) {
                Options->SEI->SEIOpViewID[ViewComponent]                  = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 10);
                if (Options->SEI->SEIOpTextureOnlyFlag == false) {
                    Options->SEI->SEIOpDepthFlag[ViewComponent]           = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                    Options->SEI->SEIOpTextureFlag[ViewComponent]         = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                }
            }
            Options->SEI->SEIOpTemporalID                                 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        }
        BitBuffer_Align(BitB, 1);
        ParseSEIMessage(Options, BitB); // sei_message();
    }
    
    void ParseSEIDepthRepresentationElement(BitBuffer *BitB, uint8_t OutSign, uint8_t OutExp, uint8_t OutMantissa, uint8_t OutManLen) { // depth_representation_sei_element
        AssertIO(BitB != NULL);
        bool     DASignFlag                                           = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1); // da_sign_flag
        uint8_t  DAExponent                                           = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 7); // da_exponent
        uint8_t  DAMatissaSize                                        = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 5) + 1;
        uint64_t DAMatissa                                            = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, DAMatissaSize);
    }
    
    void SkipSEIReservedMessage(BitBuffer *BitB, size_t SEISize) { // reserved_sei_message
        AssertIO(BitB != NULL);
        BitBuffer_Seek(BitB, Bytes2Bits(SEISize));
    }
    
    void ParseSEIMessage(AVCOptions *Options, BitBuffer *BitB) { // sei_message
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        while (BitBuffer_PeekBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 8) == 0xFF) {
            BitBuffer_Seek(BitB, 8);
            Options->SEI->SEIType += 255;
        }
        Options->SEI->SEIType += BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 8); // last_payload_type_byte, 5
        
        while (BitBuffer_PeekBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, 8) == 0xFF) {
            BitBuffer_Seek(BitB, 8);
            Options->SEI->SEISize    += 255;
        }
        Options->SEI->SEISize += BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 8); // last_payload_size_byte, 692, emulation prevention bytes not included, but these fields are.
        
        switch (Options->SEI->SEIType) { // sei_payload
            case SEI_BufferingPeriod:                         // 0
                ParseSEIBufferingPeriod(Options, BitB);
                break;
            case SEI_PictureTiming:                           // 1
                ParseSEIPictureTiming(Options, BitB);
                break;
            case SEI_PanScan:                                 // 2
                ParseSEIPanScan(Options, BitB);
                break;
            case SEI_Filler:                                  // 3
                ParseSEIFiller(Options, BitB);
                break;
            case SEI_RegisteredUserData:                      // 4
                ParseSEIRegisteredUserData(Options, BitB);
                break;
            case SEI_UnregisteredUserData:                    // 5
                ParseSEIUnregisteredUserData(Options, BitB);
                break;
            case SEI_RecoveryPoint:                           // 6
                ParseSEIRecoveryPoint(Options, BitB);
                break;
            case SEI_RepetitiveReferencePicture:              // 7
                ParseSEIRepetitiveReferencePicture(Options, BitB);
                break;
            case SEI_SparePicture:                            // 8
                ParseSEISparePicture(Options, BitB);
                break;
            case SEI_SceneInformation:                        // 9
                ParseSEISceneInfo(Options, BitB);
                break;
            case SEI_SubSequenceInformation:                  // 10
                ParseSEISubSequenceInfo(Options, BitB);
                break;
            case SEI_SubSequenceLayerProperties:              // 11
                ParseSEISubSequenceLayerProperties(Options, BitB);
                break;
            case SEI_SubSequenceProperties:                   // 12
                ParseSEISubSequenceProperties(Options, BitB);
                break;
            case SEI_FullFrameFreeze:                         // 13
                ParseSEIFullFrameFreeze(Options, BitB);
                break;
            case SEI_FullFrameFreezeRelease:                  // 14
                ParseSEIFullFrameFreezeRelease(Options, BitB);
                break;
            case SEI_FullFrameSnapshot:                       // 15
                ParseSEIFullFrameSnapshot(Options, BitB);
                break;
            case SEI_ProgressiveRefinementSegmentStart:       // 16
                ParseSEIProgressiveRefinementSegmentStart(Options, BitB);
                break;
            case SEI_ProgressiveRefinementSegmentEnd:         // 17
                ParseSEIProgressiveRefinementSegmentEnd(Options, BitB);
                break;
            case SEI_MotionConstrainedSliceGroup:             // 18
                ParseSEIMotionConstrainedSliceGroupSet(Options, BitB);
                break;
            case SEI_FilmGrainCharacteristics:                // 19
                ParseSEIFilmGrainCharacteristics(Options, BitB);
                break;
            case SEI_DeblockingFilterDisplayPreferences:      // 20
                ParseSEIDeblockingFilterDisplayPreference(Options, BitB);
                break;
            case SEI_StereoVideoInformation:                  // 21
                ParseSEIStereoVideoInfo(Options, BitB);
                break;
            case SEI_PostFilterHint:                          // 22
                ParseSEIPostFilterHint(Options, BitB);
                break;
            case SEI_ToneMappingInformation:                  // 23
                ParseSEIToneMappingInfo(Options, BitB);
                break;
            case SEI_ScalabilityInformation:                  // 24
                ParseSEIScalabilityInfo(Options, BitB);
                break;
            case SEI_SubPictureScalableLayer:                 // 25
                ParseSEISubPictureScalableLayer(Options, BitB);
                break;
            case SEI_NonRequiredLayerRep:                     // 26
                ParseSEINonRequiredLayerRep(Options, BitB);
                break;
            case SEI_PriorityLayerInformation:                // 27
                ParseSEIPriorityLayerInfo(Options, BitB);
                break;
            case SEI_LayersNotPresent:                        // 28
                ParseSEILayersNotPresent(Options, BitB);
                break;
            case SEI_LayerDependencyChange:                   // 29
                ParseSEILayerDependencyChange(Options, BitB);
                break;
            case SEI_ScalableNesting:                         // 30
                ParseSEIScalableNesting(Options, BitB);
                break;
            case SEI_BaseLayerTemporalHRD:                    // 31
                ParseSEIBaseLayerTemporalHRD(Options, BitB);
                break;
            case SEI_QualityLayerIntegrityCheck:              // 32
                ParseSEIQualityLayerIntegrityCheck(Options, BitB);
                break;
            case SEI_RedundantPictureProperty:                // 33
                ParseSEIRedundantPicProperty(Options, BitB);
                break;
            case SEI_TemporalL0DependencyRepresentationIndex: // 34
                ParseSEITemporalDependencyRepresentationIndex(Options, BitB);
                break;
            case SEI_TemporalLevelSwitchingPoint:             // 35
                ParseSEITemporalLevelSwitchingPoint(Options, BitB);
                break;
            case SEI_ParallelDecodingInformation:             // 36
                ParseSEIParallelDecodingInfo(Options, BitB);
                break;
            case SEI_MVCScalableNesting:                      // 37
                ParseSEIMVCScalableNesting(Options, BitB);
                break;
            case SEI_ViewScalabilityInformation:              // 38
                ParseSEIViewScalabilityInfo(Options, BitB);
                break;
            case SEI_MVCSceneInformation:                     // 39
                ParseSEIMVCSceneInfo(Options, BitB);
                break;
            case SEI_MVCAquisitionInformation:                // 40
                ParseSEIMVCAcquisitionInfo(Options, BitB);
                break;
            case SEI_NonRequiredViewComponent:                // 41
                ParseSEINonRequiredViewComponent(Options, BitB);
                break;
            case SEI_ViewDependencyChange:                    // 42
                ParseSEIViewDependencyChange(Options, BitB);
                break;
            case SEI_OperationPointsNotPresent:               // 43
                ParseSEIOperationPointNotPresent(Options, BitB);
                break;
            case SEI_BaseViewTemporalHRD:                     // 44
                ParseSEIBaseViewTemporalHRD(Options, BitB);
                break;
            case SEI_FramePackingArrangement:                 // 45
                ParseSEIFramePackingArrangement(Options, BitB);
                break;
            case SEI_MVCViewPosition:                         // 46
                ParseSEIMVCViewPosition(Options, BitB);
                break;
            case SEI_DisplayOrientation:                      // 47
                ParseSEIDisplayOrientation(Options, BitB);
                break;
            case SEI_MVCDScalableNesting:                     // 48
                ParseSEIMVCDScalableNesting(Options, BitB);
                break;
            case SEI_MVCDViewScalabilityInformation:          // 49
                ParseSEIViewScalabilityInfo(Options, BitB);
                break;
            case SEI_DepthRepresentationInformation:          // 50
                ParseSEIDepthRepresentationInformation(Options, BitB);
                break;
            case SEI_3DReferenceDisplaysInformation:          // 51
                ParseSEI3DReferenceDisplayInfo(Options, BitB);
                break;
            case SEI_DepthTiming:                             // 52
                ParseSEIDepthTiming(Options, BitB);
                break;
            case SEI_DepthSamplingInformation:                // 53
                ParseSEIDepthSamplingInfo(Options, BitB);
                break;
            case SEI_MVCConstrainedDPSIdentifier:             // 54
                ParseSEIConstrainedDepthParameterSetID(Options, BitB);
                break;
            case SEI_MasteringDisplayColorVolume:             // 137
                ParseSEIMeteringDisplayColorVolume(Options, BitB);
                break;
            default:
                Log(Severity_DEBUG, PlatformIO_FunctionName, UTF8String("Unrecognized SEIType: %d"), Options->SEI->SEIType);
                BitBuffer_Seek(BitB, Options->SEI->SEISize);
                break;
        }
    }
    
    void ParseAVC3DSliceDataExtension(AVCOptions *Options, BitBuffer *BitB) { // slice_data_in_3davc_extension
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        if (Options->PPS->EntropyCodingMode == Arithmetic) {
            BitBuffer_Align(BitB, 1);
            Options->Slice->CurrentMacroBlockAddress = Options->Slice->FirstMacroBlockInSlice * (Options->Slice->MbaffFrameFlag + 1);
            bool     MoreDataFlag = true;
            Options->Slice->PreviousMacroBlockSkipped = false;
            bool     RunLength = 0;
            while (MoreDataFlag == true) {
                if ((Options->Slice->Type != SliceI1) || (Options->Slice->Type != SliceI2) || (Options->Slice->Type != SliceSI1) || (Options->Slice->Type != SliceSI2)) {
                    if (Options->PPS->EntropyCodingMode == ExpGolomb) {
                        Options->Slice->MacroBlockSkipRun = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                        Options->Slice->PreviousMacroBlockSkipped = (Options->Slice->MacroBlockSkipRun > 0);
                        for (uint8_t SkippedMacroBlock = 0; SkippedMacroBlock < Options->Slice->MacroBlockSkipRun; SkippedMacroBlock++) {
                            Options->Slice->CurrentMacroBlockAddress = NextMacroBlockAddress(Options, Options->Slice->CurrentMacroBlockAddress);
                        }
                    }
                }
            }
        }
    }
    
    void ParseAVC3DSlice(AVCOptions *Options, BitBuffer *BitB) { // slice_header_in_3davc_extension
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->Slice->FirstMacroBlockInSlice     = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        Options->Slice->Type                       = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        Options->PPS->PicParamSetID                = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        if ((Options->NAL->AVC3DExtensionFlag == true) && (Options->NAL->AVC3DExtensionFlag == true)) {
            Options->Slice->PreSliceHeaderSrc      = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 2);
            if ((Options->Slice->Type == SliceP1) || (Options->Slice->Type == SliceP2) || (Options->Slice->Type == SliceSP1) || (Options->Slice->Type == SliceSP2) || (Options->Slice->Type == SliceB1) || (Options->Slice->Type == SliceB2)) {
                Options->Slice->PreRefListSrc      = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 2);
                if (Options->Slice->PreRefListSrc == 0) {
                    Options->Slice->NumRefIDXActiveOverrideFlag= BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                    if (Options->Slice->NumRefIDXActiveOverrideFlag == true) {
                        Options->MacroBlock->NumRefIndexActiveLevel0 = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
                    }
                    if ((Options->Slice->Type == SliceB1) || (Options->Slice->Type == SliceB2)) {
                        Options->MacroBlock->NumRefIndexActiveLevel1 = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
                    }
                    RefPicListMVCMod(Options, BitB);
                }
            }
            if ((Options->PPS->WeightedPrediction == true && ((Options->Slice->Type == SliceP1) || (Options->Slice->Type == SliceP2) || (Options->Slice->Type == SliceSP1) || (Options->Slice->Type == SliceSP2))) || ((Options->PPS->WeightedBiPrediction == 1 && ((Options->Slice->Type == SliceB1) || (Options->Slice->Type == SliceB2))))) {
                
                Options->Slice->PrePredictionWeightTableSrc = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 2);
                if (Options->Slice->PrePredictionWeightTableSrc == false) {
                    pred_weight_table(Options, BitB);
                }
            }
            if (Options->NAL->NALRefIDC != 0) {
                Options->Slice->PreDecReferencePictureMarkingSrc = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 2);
                if (Options->Slice->PreDecReferencePictureMarkingSrc == 0) {
                    DecodeRefPicMarking(Options, BitB);
                }
            }
            Options->Slice->SliceQPDelta = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
        } else {
            if (Options->SPS->SeperateColorPlane == true) {
                Options->Slice->ColorPlaneID   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 2);
            }
            Options->Slice->FrameNumber     = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 0); // FIXME: READ VARIABLE BITS frame_num
            if (Options->SPS->OnlyMacroBlocksInFrame == false) {
                Options->Slice->SliceIsInterlaced = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                if (Options->Slice->SliceIsInterlaced == true) {
                    Options->Slice->SliceIsBottomField  = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                }
            }
            if (Options->Slice->IDRPicID == true) {
                Options->Slice->IDRPicID = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            }
            if (Options->SPS->PicOrderCount == false) {
                Options->Slice->PictureOrderCountLSB = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, Options->SPS->MaxPicOrder);
                if ((Options->PPS->BottomPicFieldOrderInSliceFlag == true) && (Options->Slice->SliceIsInterlaced == false)) {
                    Options->Slice->DeltaPictureOrderCountBottom[1] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
                }
            }
            if ((Options->SPS->PicOrderCount == true) && (Options->SPS->DeltaPicOrder == false)) {
                Options->Slice->DeltaPictureOrderCountBottom[0] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
                if ((Options->PPS->BottomPicFieldOrderInSliceFlag == true) && (Options->Slice->SliceIsInterlaced == false)) {
                    Options->Slice->DeltaPictureOrderCountBottom[1] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
                }
            }
            if (Options->PPS->RedundantPictureFlag == true) {
                Options->PPS->RedundantPictureCount = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
            }
            if ((Options->Slice->Type == SliceB1) || (Options->Slice->Type == SliceB2)) {
                Options->Slice->DirectSpatialMVPredictionFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            }
            if ((Options->Slice->Type == SliceP1) || (Options->Slice->Type == SliceP2) || (Options->Slice->Type == SliceSP1) || (Options->Slice->Type == SliceSP2) || (Options->Slice->Type == SliceB1) || (Options->Slice->Type == SliceB2)) {
                Options->Slice->NumRefIDXActiveOverrideFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                if (Options->Slice->NumRefIDXActiveOverrideFlag == true) {
                    Options->MacroBlock->NumRefIndexActiveLevel0 = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
                    if ((Options->Slice->Type == SliceB1) || (Options->Slice->Type == SliceB2)) {
                        Options->MacroBlock->NumRefIndexActiveLevel1 = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
                    }
                }
            }
            if ((Options->NAL->NALRefIDC == NAL_AuxiliarySliceExtension) || (Options->NAL->NALRefIDC == NAL_MVCDepthView)) {
                RefPicListMVCMod(Options, BitB); /* specified in Annex H */
            } else {
                RefPicListMod(Options, BitB);
            }
            if (((Options->Slice->DepthWeightedPredictionFlag == true) && ((Options->Slice->Type == SliceP1) || (Options->Slice->Type == SliceP2))) || ((Options->PPS->WeightedBiPrediction == true && (Options->Slice->Type == SliceB1)) || (Options->Slice->Type == SliceB2))) {
                pred_weight_table(Options, BitB);
            }
            if (Options->NAL->NALRefIDC != NAL_Unspecified0) {
                DecodeRefPicMarking(Options, BitB);
            }
            if ((Options->PPS->EntropyCodingMode == ExpGolomb) && ((Options->Slice->Type != SliceI1) || (Options->Slice->Type != SliceI2))) {
                Options->Slice->CabacInitIDC = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                Options->Slice->SliceQPDelta = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
            }
            // FIXME: Should SliceQPDelta be here?
            if ((Options->Slice->Type == SliceSP1) || (Options->Slice->Type == SliceSP2) || (Options->Slice->Type == SliceSI1) || (Options->Slice->Type == SliceSI2)) {
                if ((Options->Slice->Type == SliceSP1) || (Options->Slice->Type == SliceSP2)) {
                    Options->Slice->DecodePMBAsSPSlice = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                    Options->Slice->SliceQSDelta = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
                }
                if (Options->PPS->DeblockingFilterFlag == true) {
                    Options->Slice->DisableDeblockingFilterIDC = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                    if (Options->Slice->DisableDeblockingFilterIDC == false) {
                        Options->Slice->SliceAlphaOffsetC0 = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
                        Options->Slice->SliceBetaOffset    = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
                    }
                }
                if ((Options->PPS->SliceGroups > 0) && (Options->PPS->SliceGroupMapType >= 3) && (Options->PPS->SliceGroupMapType <= 5)) {
                    Options->Slice->SliceGroupChangeCycle = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, Ceili(log2(Options->PPS->PicSizeInMapUnits /  Options->Slice->SliceGroupChangeRate)));
                }
                if (
                    (Options->NAL->NALUnitType == NAL_MVCDepthView) &&
                    (
                     (Options->Slice->Type == SliceI1)  ||
                     (Options->Slice->Type == SliceI2)  ||
                     (Options->Slice->Type == SliceSI1) ||
                     (Options->Slice->Type == SliceSI2)
                     )
                    ) {
                        if (Options->NAL->DepthFlag == true) {
                            Options->Slice->DepthWeightedPredictionFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                        } else if (Options->NAL->AVC3DExtensionFlag == true) {
                            Options->Slice->DMVPFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                            if (Options->Slice->SeqViewSynthesisFlag == true) {
                                Options->Slice->SliceVSPFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                            }
                        }
                        if (Options->SPS->AVC3DAcquisitionIDC == false && ((Options->Slice->DepthWeightedPredictionFlag == true) || (Options->Slice->DMVPFlag == true))) {
                            Options->Slice->DepthParamSetID = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                        }
                    }
            }
        }
    }
    
    /* Scalable Video Coding */
    void ParseScalableSlice(AVCOptions *Options, BitBuffer *BitB) { // slice_header_in_scalable_extension
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->Slice->FirstMacroBlockInSlice = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        Options->Slice->Type                   = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        Options->PPS->PicParamSetID            = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        if (Options->Slice->SeperateColorPlaneFlag == true) {
            Options->Slice->ColorPlaneID       = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 2);
        }
        Options->Slice->FrameNumber            = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        if (Options->SPS->OnlyMacroBlocksInFrame == true) {
            Options->Slice->SliceIsInterlaced  = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->Slice->SliceIsInterlaced == true) {
                Options->Slice->SliceIsBottomField = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            }
        }
        if (Options->NAL->IDRFlag == NAL_NonIDRSlice) {
            Options->Slice->IDRPicID = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        }
        if (Options->SPS->PicOrderCount == 0) {
            Options->Slice->PictureOrderCountLSB = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, Options->SPS->MaxPicOrder);
            if ((Options->PPS->BottomPicFieldOrderInSliceFlag == true) && (Options->Slice->SliceIsInterlaced == false)) {
                Options->Slice->DeltaPictureOrderCountBottom[0] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
            }
        } else if ((Options->SPS->PicOrderCount == 1) && (Options->SPS->DeltaPicOrder == 0)) {
            Options->Slice->DeltaPicOrderCount[0] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
            if ((Options->PPS->BottomPicFieldOrderInSliceFlag == true) && (Options->Slice->SliceIsInterlaced == false)) {
                Options->Slice->DeltaPicOrderCount[1] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
            }
        }
        if (Options->PPS->RedundantPictureFlag == true) {
            Options->PPS->RedundantPictureCount = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        }
        if (Options->NAL->QualityID == 0) {
            if ((Options->NAL->NALUnitType == NAL_AuxiliarySliceExtension) && ((Options->Slice->Type == SliceEB1) || (Options->Slice->Type == SliceEB2))) {
                Options->Slice->DirectSpatialMVPredictionFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            }
            if ((Options->NAL->NALUnitType == NAL_AuxiliarySliceExtension) && ((Options->Slice->Type == SliceEP1) || (Options->Slice->Type == SliceEP2) || (Options->Slice->Type == SliceEB1) || (Options->Slice->Type == SliceEB2))) {
                Options->Slice->NumRefIDXActiveOverrideFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                if (Options->Slice->NumRefIDXActiveOverrideFlag == true) {
                    Options->MacroBlock->NumRefIndexActiveLevel0 = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
                    if (Options->Slice->Type == SliceEB1 || Options->Slice->Type == SliceEB2) {
                        Options->MacroBlock->NumRefIndexActiveLevel1 = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
                    }
                }
            }
            RefPicListMod(Options, BitB);
            if ((Options->NAL->NALUnitType == NAL_AuxiliarySliceExtension) && (((Options->PPS->WeightedPrediction == true) &&((Options->Slice->Type == SliceEP1)  || (Options->Slice->Type == SliceEP2))) || ((Options->PPS->WeightedBiPrediction == 1) && ((Options->Slice->Type == SliceEB1) || (Options->Slice->Type == SliceEB2))))) {
                if (Options->NAL->NoInterLayerPredictionFlag == true) {
                    Options->Slice->BasePredictionWeightTable = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                }
                if ((Options->NAL->NoInterLayerPredictionFlag == false) || (Options->Slice->BasePredictionWeightTable == false)) {
                    pred_weight_table(Options, BitB);
                }
            }
            if (Options->NAL->NALRefIDC != NAL_Unspecified0) {
                DecodeRefPicMarking(Options, BitB);
                if (Options->SVC->SliceHeaderRestricted == false) {
                    Options->Slice->StoreRefBasePicFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                    if ((Options->NAL->UseReferenceBasePictureFlag == true) || ((Options->Slice->StoreRefBasePicFlag == true) && (Options->NAL->IDRFlag == false))) {
                        ParseReferenceBasePictureSyntax(Options, BitB);
                    }
                }
            }
        }
        if ((Options->PPS->EntropyCodingMode == true) && ((Options->Slice->Type != SliceEI1) || (Options->Slice->Type != SliceEI2))) {
            Options->Slice->CabacInitIDC = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
        }
        Options->Slice->SliceQPDelta = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
        if (Options->PPS->DeblockingFilterFlag == true) {
            Options->Slice->DisableDeblockingFilterIDC = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            if (Options->Slice->DisableDeblockingFilterIDC != 1) {
                Options->Slice->SliceAlphaOffsetC0 = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
                Options->Slice->SliceBetaOffset    = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
            }
        }
        if ((Options->PPS->SliceGroups > 0) && ((Options->PPS->SliceGroupMapType >= 3) && (Options->PPS->SliceGroupMapType <= 5))) {
            Options->Slice->SliceGroupChangeCycle = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, Ceili(log2(Options->PPS->PicSizeInMapUnits /  Options->Slice->SliceGroupChangeRate)));
        }
        if ((Options->NAL->NoInterLayerPredictionFlag == true) && (Options->NAL->QualityID == 0)) {
            Options->Slice->RefLayerDQID = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            if (Options->SVC->InterLayerDeblockingFilterPresent == true) {
                Options->Slice->DisableInterLayerDeblocking = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                if (Options->Slice->DisableInterLayerDeblocking != 1) {
                    Options->Slice->InterLayerSliceAplhaC0Offset = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
                    Options->Slice->InterLayerSliceBetaOffset    = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
                }
            }
            Options->Slice->ConstrainedIntraResamplingFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->SVC->ExtendedSpatialScalabilityIDC == 2) {
                if (Options->SPS->SeperateColorPlane == false) {
                    Options->SPS->ChromaArrayType = Options->SPS->ChromaFormatIDC;
                } else {
                    Options->SPS->ChromaArrayType = AVCChroma_Gray;
                }
                if (Options->SPS->ChromaArrayType > AVCChroma_Gray) {
                    BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                    Options->Slice->RefLayerChromaPhaseXFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                    Options->Slice->RefLayerChromaPhaseY     = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 2) - 1;
                }
                Options->Slice->ScaledRefLayerLeftOffset     = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
                Options->Slice->ScaledRefLayerTopOffset      = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
                Options->Slice->ScaledRefLayerRightOffset    = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
                Options->Slice->ScaledRefLayerBottomOffset   = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
            }
        }
        if (Options->NAL->NoInterLayerPredictionFlag == true) {
            Options->Slice->SkipSliceFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            if (Options->Slice->SkipSliceFlag == true) {
                Options->Slice->MacroBlocksInSlice = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false) + 1;
            } else {
                Options->Slice->AdaptiveBaseModeFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                if (Options->Slice->AdaptiveBaseModeFlag == false) {
                    Options->Slice->DefaultBaseModeFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                }
                if (Options->Slice->DefaultBaseModeFlag == false) {
                    Options->Slice->AdaptiveMotionPredictionFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                    if (Options->Slice->AdaptiveMotionPredictionFlag == false) {
                        Options->Slice->DefaultMotionPredictionFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                    }
                }
                if (Options->Slice->AdaptiveResidualPredictionFlag == false) {
                    Options->Slice->DefaultResidualPredictionFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                }
            }
            if (Options->SVC->AdaptiveCoeffsPresent == true) {
                Options->Slice->TCoefficentPredictionFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            }
        }
        if ((Options->SVC->SliceHeaderRestricted == false) && (Options->Slice->SkipSliceFlag == false)) {
            Options->Slice->ScanIndexStart = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 4);
            Options->Slice->ScanIndexEnd   = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 4);
        }
    }
    
    void ParseNALAuxiliarySliceExtension(AVCOptions *Options, BitBuffer *BitB) { // slice_layer_extension_rbsp
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        if (Options->NAL->SVCExtensionFlag == true) {
            ParseScalableSlice(Options, BitB); // slice_header_in_scalable_extension()
            if (Options->Slice->SkipSliceFlag == true) {
                ParseScalableSliceData(Options, BitB); // slice_data_in_scalable_extension()
            }
        } else if (Options->NAL->AVC3DExtensionFlag == true) {
            ParseAVC3DSlice(Options, BitB); // specified in Annex J
        } else {
            ParseSliceHeader(Options, BitB); // slice_header();
            ParseSliceData(Options, BitB, (2|3|4)); // slice_data(); FIXME: Unknown Category
        }
        BitBuffer_Align(BitB, 1); // rbsp_slice_trailing_bits()
    }
    
    void ParseScalableSliceData(AVCOptions *Options, BitBuffer *BitB) { // slice_data_in_scalable_extension
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        bool moreDataFlag;
        if (Options->PPS->EntropyCodingMode == Arithmetic) {
            BitBuffer_Align(BitB, 1);
        }
        Options->Slice->MbaffFrameFlag = (Options->SPS->AdaptiveFrameFlag && !Options->Slice->SliceIsInterlaced);
        Options->Slice->CurrentMacroBlockAddress = Options->Slice->FirstMacroBlockInSlice * (Options->Slice->MbaffFrameFlag + 1);
        moreDataFlag = 1;
        if ((Options->Slice->Type != SliceEI1) || (Options->Slice->Type != SliceEI2)) {
            if (Options->PPS->EntropyCodingMode == ExpGolomb) {
                Options->Slice->MacroBlockSkipRun = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                Options->Slice->PreviousMacroBlockSkipped = (Options->Slice->MacroBlockSkipRun > 0);
                for (uint8_t MB = 0; MB < Options->Slice->MacroBlockSkipRun; MB++) {
                    Options->Slice->CurrentMacroBlockAddress = NextMacroBlockAddress(Options, Options->Slice->CurrentMacroBlockAddress);
                }
                if (Options->Slice->MacroBlockSkipRun > 0) {
                    moreDataFlag = more_rbsp_data();
                }
            } else {
                Options->Slice->SkipMacroBlock = ReadArithmetic(BitB, NULL, NULL, NULL, NULL); // FIXME: Arthmetic encoded variable
                moreDataFlag = !Options->Slice->SkipMacroBlock;
            }
        }
        if (moreDataFlag == true) {
            if (Options->Slice->MbaffFrameFlag && ((Options->Slice->CurrentMacroBlockAddress % 2) == 0 || (((Options->Slice->CurrentMacroBlockAddress % 2) == 1) && (Options->Slice->PreviousMacroBlockSkipped == true)))) {
                Options->Slice->MacroBlockFieldDecodingFlag  = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
            }
            ParseMacroBlockLayerInSVC(Options, BitB); // macroblock_layer_in_scalable_extension();
            if (Options->PPS->EntropyCodingMode == ExpGolomb) {
                moreDataFlag = more_rbsp_data();
            } else {
                if ((Options->Slice->Type != SliceEI1) || (Options->Slice->Type != SliceEI2)) {
                    Options->Slice->PreviousMacroBlockSkipped = Options->Slice->SkipMacroBlock;
                }
                if (Options->Slice->MbaffFrameFlag && (Options->Slice->CurrentMacroBlockAddress % 2) == 0) {
                    moreDataFlag = 1;
                } else {
                    Options->Slice->IsEndOfSlice = ReadArithmetic(BitB, NULL, NULL, NULL, NULL); // arthmetic coded variable
                    moreDataFlag = !Options->Slice->IsEndOfSlice;
                    while (Options->Slice->IsEndOfSlice == false) {
                        BitBuffer_Seek(BitB, 1); // FIXME: Not sure f it should be 1 bit or 8 or what?
                    }
                }
            }
            Options->Slice->CurrentMacroBlockAddress = NextMacroBlockAddress(Options, Options->Slice->CurrentMacroBlockAddress);
        }
    }
    
    void ParseUnpartitionedSliceLayer(AVCOptions *Options, BitBuffer *BitB) { // slice_layer_without_partitioning_rbsp
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        //ParseSliceHeader(Options, BitB);
        ParseSliceData(Options, BitB, 0);
        rbsp_slice_trailing_bits(Options, BitB);
    }
    
    void ParseMacroBlockLayerInSVC(AVCOptions *Options, BitBuffer *BitB) { // macroblock_layer_in_scalable_extension
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        if (Options->PPS->EntropyCodingMode == ExpGolomb) {
            if (InCropWindow(Options, Options->Slice->CurrentMacroBlockAddress) && (Options->Slice->AdaptiveBaseModeFlag == true)) {
                Options->MacroBlock->BaseModeFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1); // base_mode_flag
            }
            if (Options->MacroBlock->BaseModeFlag == false) {
                Options->MacroBlock->Type = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsLeft, BitB, CountUnary, AVCStopBit);
            }
            if (Options->MacroBlock->Type == I_PCM) {
                BitBuffer_Seek(BitB, 1); // pcm_alignment_zero_bit
                CalculateMacroBlockDimensions(Options);
                for (uint16_t Sample = 0; Sample < MacroBlockMaxPixels; Sample++) {
                    Options->MacroBlock->PCMLumaSample[Sample] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, Options->SPS->LumaBitDepthMinus8);
                }
                for (uint8_t Channel = 0; Channel < 2; Channel++) {
                    for (uint8_t Sample = 0; Sample < (2 * (Options->SPS->MacroBlockWidthChroma * Options->SPS->MacroBlockHeightChroma)); Sample++) {
                        Options->MacroBlock->PCMChromaSample[Channel][Sample] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, Options->SPS->ChromaBitDepthMinus8);
                    }
                }
            } else {
                if (Options->MacroBlock->BaseModeFlag == false) {
                    Options->MacroBlock->NoMBSmallerThan8x8Flag = 1;
                    if ((Options->MacroBlock->Type != I_NxN) && (MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) != Intra_16x16) && NumMacroBlockPartitions(Options->MacroBlock->Type) == 4) {
                        ParseSubMacroBlockPredictionInSVC(Options, BitB);
                        for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) {
                            if (Options->MacroBlock->SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8) {
                                if (GetNumSubMacroBlockPartitions(Options->MacroBlock->SubMacroBlockType[MacroBlockPiece]) > 1) {
                                    Options->MacroBlock->NoMBSmallerThan8x8Flag = 0;
                                }
                            } else if (Options->SPS->Inference8x8 == false) {
                                Options->MacroBlock->NoMBSmallerThan8x8Flag = 0;
                            }
                        }
                    } else {
                        if (Options->PPS->TransformIs8x8 == true && Options->MacroBlock->Type == I_NxN) {
                            Options->MacroBlock->TransformSizeIs8x8 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                        }
                        ParseMBPredictionInSVC(Options, BitB);
                    }
                }
                if ((Options->Slice->AdaptiveResidualPredictionFlag == true) && ((Options->Slice->Type != SliceEI1) || (Options->Slice->Type != SliceEI2)) && InCropWindow(Options, Options->Slice->CurrentMacroBlockAddress) && (Options->MacroBlock->BaseModeFlag || (MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) != Intra_16x16 && MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) != Intra_8x8 && MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) != Intra_4x4))) {
                    Options->Slice->ResidualPredictionFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                }
                if (Options->Slice->ScanIndexEnd >= Options->Slice->ScanIndexStart) {
                    if (Options->MacroBlock->BaseModeFlag || MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) != Intra_16x16) {
                        Options->MacroBlock->BlockPattern = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsLeft, BitB, CountUnary, AVCStopBit); // MappedExp
                        if (Options->MacroBlock->BlockPatternLuma > 0 && Options->PPS->TransformIs8x8 && (Options->MacroBlock->BaseModeFlag || (Options->MacroBlock->Type != I_NxN && Options->MacroBlock->NoMBSmallerThan8x8Flag && (Options->MacroBlock->Type != B_Direct_16x16 || Options->SPS->Inference8x8)))) {
                            Options->MacroBlock->TransformSizeIs8x8 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                        }
                    }
                    if (Options->MacroBlock->BlockPatternLuma > 0 || Options->MacroBlock->BlockPatternChroma > 0 || MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) == Intra_16x16) {
                        Options->MacroBlock->QPDelta = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsLeft, BitB, CountUnary, AVCStopBit);
                        residual(Options->Slice->ScanIndexStart, Options->Slice->ScanIndexEnd);
                    }
                }
            }
        } else if (Options->PPS->EntropyCodingMode == Arithmetic) {
            if (InCropWindow(Options, Options->Slice->CurrentMacroBlockAddress) && (Options->Slice->AdaptiveBaseModeFlag == true)) {
                Options->MacroBlock->BaseModeFlag = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
            }
            if (Options->MacroBlock->BaseModeFlag == false) {
                Options->MacroBlock->Type = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
            }
            if (Options->MacroBlock->Type == I_PCM) {
                BitBuffer_Seek(BitB, 1);
                CalculateMacroBlockDimensions(Options);
                for (uint16_t Sample = 0; Sample < MacroBlockMaxPixels; Sample++) {
                    Options->MacroBlock->PCMLumaSample[Sample] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, Options->SPS->LumaBitDepthMinus8);
                }
                for (uint8_t Channel = 0; Channel < 2; Channel++) {
                    for (uint8_t Sample = 0; Sample < (2 * (Options->SPS->MacroBlockWidthChroma * Options->SPS->MacroBlockHeightChroma)); Sample++) {
                        Options->MacroBlock->PCMChromaSample[Channel][Sample] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, Options->SPS->ChromaBitDepthMinus8);
                    }
                }
            } else {
                if (Options->MacroBlock->BaseModeFlag == false) {
                    Options->MacroBlock->NoMBSmallerThan8x8Flag = 1;
                    if ((Options->MacroBlock->Type != I_NxN) && (MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) != Intra_16x16) && NumMacroBlockPartitions(Options->MacroBlock->Type) == 4) {
                        ParseSubMacroBlockPredictionInSVC(Options, BitB);
                        for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) {
                            if (Options->MacroBlock->SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8) {
                                if (GetNumSubMacroBlockPartitions(Options->MacroBlock->SubMacroBlockType[MacroBlockPiece]) > 1) {
                                    Options->MacroBlock->NoMBSmallerThan8x8Flag = 0;
                                }
                            } else if (Options->SPS->Inference8x8 == false) {
                                Options->MacroBlock->NoMBSmallerThan8x8Flag = 0;
                            }
                        }
                    } else {
                        if (Options->PPS->TransformIs8x8 == true && Options->MacroBlock->Type == I_NxN) {
                            Options->MacroBlock->TransformSizeIs8x8 = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                        ParseMBPredictionInSVC(Options, BitB);
                    }
                }
                if ((Options->Slice->AdaptiveResidualPredictionFlag == true) && ((Options->Slice->Type != SliceEI1) || (Options->Slice->Type != SliceEI2)) && InCropWindow(Options, Options->Slice->CurrentMacroBlockAddress) && (Options->MacroBlock->BaseModeFlag || (MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) != Intra_16x16 && MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) != Intra_8x8 && MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) != Intra_4x4))) {
                    Options->Slice->ResidualPredictionFlag = Options->PPS->EntropyCodingMode = ExpGolomb ? ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsLeft, BitB, CountUnary, AVCStopBit) : ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                }
                if (Options->Slice->ScanIndexEnd >= Options->Slice->ScanIndexStart) {
                    if (Options->MacroBlock->BaseModeFlag || MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) != Intra_16x16) {
                        Options->MacroBlock->BlockPattern = ReadArithmetic(BitB, NULL, NULL, NULL, NULL); // ME or Arithmetic
                        if (Options->MacroBlock->BlockPatternLuma > 0 && Options->PPS->TransformIs8x8 && (Options->MacroBlock->BaseModeFlag || (Options->MacroBlock->Type != I_NxN && Options->MacroBlock->NoMBSmallerThan8x8Flag && (Options->MacroBlock->Type != B_Direct_16x16 || Options->SPS->Inference8x8)))) {
                            Options->MacroBlock->TransformSizeIs8x8 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1); // OR Arithmetic
                        }
                    }
                    if (Options->MacroBlock->BlockPatternLuma > 0 || Options->MacroBlock->BlockPatternChroma > 0 || MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) == Intra_16x16) {
                        Options->MacroBlock->QPDelta = Options->PPS->EntropyCodingMode = ExpGolomb ? ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true) : ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        residual(Options->Slice->ScanIndexStart, Options->Slice->ScanIndexEnd);
                    }
                }
            }
        }
    }
    
    void CalculateMacroBlockDimensions(AVCOptions *Options) {
        AssertIO(Options != NULL);
        if ((Options->SPS->ChromaFormatIDC == AVCChroma_Gray) && (Options->Slice->SeperateColorPlaneFlag == false)) {
            Options->SPS->MacroBlockWidthChroma  = 0;
            Options->SPS->MacroBlockHeightChroma = 0;
        } else if ((Options->SPS->ChromaFormatIDC == AVCChroma_420) && (Options->Slice->SeperateColorPlaneFlag == false)) {
            Options->SPS->MacroBlockWidthChroma  = 8;
            Options->SPS->MacroBlockHeightChroma = 8;
        } else if ((Options->SPS->ChromaFormatIDC == AVCChroma_422) && (Options->Slice->SeperateColorPlaneFlag == false)) {
            Options->SPS->MacroBlockWidthChroma  = 8;
            Options->SPS->MacroBlockHeightChroma = 16;
        } else if ((Options->SPS->ChromaFormatIDC == AVCChroma_444) && (Options->Slice->SeperateColorPlaneFlag == false)) {
            Options->SPS->MacroBlockWidthChroma  = 16;
            Options->SPS->MacroBlockHeightChroma = 16;
        } else if (Options->Slice->SeperateColorPlaneFlag == true) {
            // Samples need to be handled differently.
        }
    }
    
    void ParseMBPredictionInSVC(AVCOptions *Options, BitBuffer *BitB) { // mb_pred_in_scalable_extension
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        if (Options->PPS->EntropyCodingMode == ExpGolomb) {
            if ((MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) == Intra_4x4) || (MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) == Intra_8x8) || (MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) == Intra_16x16)) {
                if (MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) == Intra_4x4) {
                    for (uint8_t luma4x4BlkIdx = 0; luma4x4BlkIdx < 16; luma4x4BlkIdx++) {
                        Options->MacroBlock->PreviousIntra4x4PredictionFlag[luma4x4BlkIdx] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                        if (!Options->MacroBlock->PreviousIntra4x4PredictionFlag[luma4x4BlkIdx]) {
                            Options->MacroBlock->RemIntra4x4PredictionMode[luma4x4BlkIdx] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 3);
                        }
                    }
                }
                if (MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) == Intra_8x8) {
                    for (uint8_t luma8x8BlkIdx = 0; luma8x8BlkIdx < 4; luma8x8BlkIdx++) {
                        Options->MacroBlock->PreviousIntra8x8PredictionFlag[luma8x8BlkIdx] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                        if (!Options->MacroBlock->PreviousIntra8x8PredictionFlag[luma8x8BlkIdx]) {
                            Options->MacroBlock->RemIntra8x8PredictionMode[luma8x8BlkIdx] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 3);
                        }
                    }
                }
                if (Options->SPS->ChromaArrayType != 0) {
                    Options->MacroBlock->IntraChromaPredictionMode = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsLeft, BitB, CountUnary, AVCStopBit);
                }
            } else if (MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) != Direct) {
                if (InCropWindow(Options, Options->Slice->CurrentMacroBlockAddress) && Options->Slice->AdaptiveMotionPredictionFlag) {
                    for (uint32_t MacroBlockPiece = 0; MacroBlockPiece < NumMacroBlockPartitions(Options->MacroBlock->Type); MacroBlockPiece++) {
                        if (MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) != Pred_L1) {
                            Options->MacroBlock->MotionPredictionFlagLevel0[MacroBlockPiece] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                        }
                        if (MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) != Pred_L0) {
                            Options->MacroBlock->MotionPredictionFlagLevel1[MacroBlockPiece] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                        }
                    }
                }
                for (uint32_t MacroBlockPiece = 0; MacroBlockPiece < NumMacroBlockPartitions(Options->MacroBlock->Type); MacroBlockPiece++) {
                    if ((Options->MacroBlock->NumRefIndexActiveLevel0 > 0 || Options->Slice->MacroBlockFieldDecodingFlag != Options->Slice->SliceIsInterlaced) && MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 1) != Pred_L1 && !Options->MacroBlock->MotionPredictionFlagLevel0[MacroBlockPiece]) {
                        Options->MacroBlock->RefIndexLevel0[MacroBlockPiece] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
                    }
                    if ((Options->MacroBlock->NumRefIndexActiveLevel1 > 0 || Options->Slice->MacroBlockFieldDecodingFlag != Options->Slice->SliceIsInterlaced) && MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 1) != Pred_L0 && !Options->MacroBlock->MotionPredictionFlagLevel1[MacroBlockPiece]) {
                        Options->MacroBlock->RefIndexLevel1[MacroBlockPiece] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
                    }
                    if (MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, MacroBlockPiece) != Pred_L1) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            Options->MacroBlock->MVDLevel0[MacroBlockPiece][0][compIdx] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
                        }
                    }
                    if (MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 1) != Pred_L0) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            Options->MacroBlock->MVDLevel1[MacroBlockPiece][0][compIdx] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
                        }
                    }
                }
            }
        } else if (Options->PPS->EntropyCodingMode == Arithmetic) {
            if ((MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) == Intra_4x4) || (MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) == Intra_8x8) || (MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) == Intra_16x16)) {
                if (MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) == Intra_4x4) {
                    for (uint8_t luma4x4BlkIdx = 0; luma4x4BlkIdx < 16; luma4x4BlkIdx++) {
                        Options->MacroBlock->PreviousIntra4x4PredictionFlag[luma4x4BlkIdx] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        if (!Options->MacroBlock->PreviousIntra4x4PredictionFlag[luma4x4BlkIdx]) {
                            Options->MacroBlock->RemIntra4x4PredictionMode[luma4x4BlkIdx] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                }
                if (MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) == Intra_8x8) {
                    for (uint8_t luma8x8BlkIdx = 0; luma8x8BlkIdx < 4; luma8x8BlkIdx++) {
                        Options->MacroBlock->PreviousIntra8x8PredictionFlag[luma8x8BlkIdx] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        if (Options->MacroBlock->PreviousIntra8x8PredictionFlag[luma8x8BlkIdx] == false) {
                            Options->MacroBlock->RemIntra8x8PredictionMode[luma8x8BlkIdx] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                }
                if (Options->SPS->ChromaArrayType != 0) {
                    Options->MacroBlock->IntraChromaPredictionMode = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                }
            } else if (MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) != Direct) {
                if (InCropWindow(Options, Options->Slice->CurrentMacroBlockAddress) && Options->Slice->AdaptiveMotionPredictionFlag) {
                    for (uint32_t MacroBlockPiece = 0; MacroBlockPiece < NumMacroBlockPartitions(Options->MacroBlock->Type); MacroBlockPiece++) {
                        if (MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 1) != Pred_L1) {
                            Options->MacroBlock->MotionPredictionFlagLevel0[MacroBlockPiece] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                        if (MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 1) != Pred_L0) {
                            Options->MacroBlock->MotionPredictionFlagLevel1[MacroBlockPiece] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                }
                for (uint32_t MacroBlockPiece = 0; MacroBlockPiece < NumMacroBlockPartitions(Options->MacroBlock->Type); MacroBlockPiece++) {
                    if ((Options->MacroBlock->NumRefIndexActiveLevel0 > 0 || Options->Slice->MacroBlockFieldDecodingFlag != Options->Slice->SliceIsInterlaced) && MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 1) != Pred_L1 && !Options->MacroBlock->MotionPredictionFlagLevel0[MacroBlockPiece]) {
                        Options->MacroBlock->RefIndexLevel0[MacroBlockPiece] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                    }
                    if ((Options->MacroBlock->NumRefIndexActiveLevel1 > 0 || Options->Slice->MacroBlockFieldDecodingFlag != Options->Slice->SliceIsInterlaced) &&
                        MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 1) != Pred_L0 && !Options->MacroBlock->MotionPredictionFlagLevel0[MacroBlockPiece]) {
                        Options->MacroBlock->RefIndexLevel1[MacroBlockPiece] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                    }
                    if (MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, MacroBlockPiece) != Pred_L1) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            Options->MacroBlock->MVDLevel0[MacroBlockPiece][0][compIdx] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                    if (MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) != Pred_L0) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            Options->MacroBlock->MVDLevel1[MacroBlockPiece][0][compIdx] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                }
            }
        }
    }
    
    void ParseAVC3DMacroBlockPredictionExtension(AVCOptions *Options, BitBuffer *BitB) {
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        if (MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) == Intra_4x4) {
            
        } else if (MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) == Intra_8x8) {
            
        } else if (MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) == Intra_16x16) {
            
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
    
    void MacroBlockLayer(AVCOptions *Options, BitBuffer *BitB) { // macroblock_layer
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        if (Options->PPS->EntropyCodingMode == ExpGolomb) {
            Options->MacroBlock->Type = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
            if (Options->MacroBlock->Type == I_PCM) { // I_PCM
                BitBuffer_Align(BitB, 1);
                for (uint16_t i = 0; i < 256; i++) {
                    Options->MacroBlock->PCMLumaSample[i] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, Options->SPS->LumaBitDepthMinus8);
                    for (uint8_t j = 0; j < 2; j++) { // FIXME: 2 needs to be a variable of the number of channels
                        Options->MacroBlock->PCMChromaSample[j][i] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, Options->SPS->ChromaBitDepthMinus8);
                    }
                }
            } else { // NOT I_PCM
                Options->MacroBlock->NoMBSmallerThan8x8Flag = true;
                uint8_t NumMacroBlockPart = NumMacroBlockPartitions(Options->MacroBlock->Type);
                if ((Options->MacroBlock->Type != I_NxN) && (MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) != Intra_16x16) && (NumMacroBlockPart == 4)) {
                    ParseSubMacroBlockPrediction(Options, BitB, Options->MacroBlock->Type);
                    for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) {
                        if (Options->MacroBlock->SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8) {
                            if (GetNumSubMacroBlockPartitions(Options->MacroBlock->SubMacroBlockType[MacroBlockPiece]) > 1) {
                                Options->MacroBlock->NoMBSmallerThan8x8Flag = 0;
                            }
                        } else if (Options->SPS->Inference8x8 == false) {
                            Options->MacroBlock->NoMBSmallerThan8x8Flag = 0;
                        }
                    }
                } else {
                    if (Options->PPS->TransformIs8x8 && (Options->MacroBlock->Type == I_NxN)) {
                        Options->MacroBlock->TransformSizeIs8x8 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                    }
                    mb_pred(Options, BitB, Options->MacroBlock->Type);
                }
                if (MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) != Intra_16x16) {
                    Options->MacroBlock->BlockPattern = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false); // TODO: Add table lookup
                    if (Options->MacroBlock->BlockPattern == true && Options->PPS->TransformIs8x8 && (Options->MacroBlock->Type != I_NxN) && Options->MacroBlock->NoMBSmallerThan8x8Flag && (Options->MacroBlock->Type != B_Direct_16x16 || Options->SPS->Inference8x8)) {
                        Options->MacroBlock->TransformSizeIs8x8 = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                    }
                    if (Options->MacroBlock->BlockPatternLuma > 0 || Options->MacroBlock->BlockPatternChroma > 0 || MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) == Intra_16x16) {
                        Options->MacroBlock->QPDelta = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
                        residual(0, 15);
                    }
                }
            }
        } else if (Options->PPS->EntropyCodingMode == Arithmetic) {
            Options->MacroBlock->Type = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
            if (Options->MacroBlock->Type == I_PCM) { // I_PCM
                BitBuffer_Align(BitB, 1);
                for (uint16_t i = 0; i < 256; i++) {
                    Options->MacroBlock->PCMLumaSample[i] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, Options->SPS->LumaBitDepthMinus8);
                    for (uint8_t j = 0; j < 2; j++) { // FIXME: 2 needs to be a variable of the number of channels
                        Options->MacroBlock->PCMChromaSample[j][i] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, Options->SPS->ChromaBitDepthMinus8);
                    }
                }
            } else { // NOT I_PCM
                Options->MacroBlock->NoMBSmallerThan8x8Flag = true;
                uint8_t NumMacroBlockPart = NumMacroBlockPartitions(Options->MacroBlock->Type);
                if ((Options->MacroBlock->Type != I_NxN) && (MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) != Intra_16x16) && (NumMacroBlockPart == 4)) {
                    ParseSubMacroBlockPrediction(Options, BitB, Options->MacroBlock->Type);
                    for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) {
                        if (Options->MacroBlock->SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8) {
                            if (GetNumSubMacroBlockPartitions(Options->MacroBlock->SubMacroBlockType[MacroBlockPiece]) > 1) {
                                Options->MacroBlock->NoMBSmallerThan8x8Flag = 0;
                            }
                        } else if (Options->SPS->Inference8x8 == false) {
                            Options->MacroBlock->NoMBSmallerThan8x8Flag = 0;
                        }
                    }
                } else {
                    if (Options->PPS->TransformIs8x8 && (Options->MacroBlock->Type == I_NxN)) {
                        Options->MacroBlock->TransformSizeIs8x8 = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                    }
                    mb_pred(Options, BitB, Options->MacroBlock->Type);
                }
                if (MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) != Intra_16x16) {
                    Options->MacroBlock->BlockPattern = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                    if (Options->MacroBlock->BlockPattern == true && Options->PPS->TransformIs8x8 && (Options->MacroBlock->Type != I_NxN) && Options->MacroBlock->NoMBSmallerThan8x8Flag && (Options->MacroBlock->Type != B_Direct_16x16 || Options->SPS->Inference8x8)) {
                        Options->MacroBlock->TransformSizeIs8x8 = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                    }
                    if (Options->MacroBlock->BlockPatternLuma > 0 || Options->MacroBlock->BlockPatternChroma > 0 || MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) == Intra_16x16) {
                        Options->MacroBlock->QPDelta = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        residual(0, 15);
                    }
                }
            }
        }
    }
    
    void mb_pred(AVCOptions *Options, BitBuffer *BitB, uint8_t mb_type) { // mb_pred
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        if (Options->PPS->EntropyCodingMode == ExpGolomb) {
            uint8_t MacroBlockPredictionMode = MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0);
            if ((MacroBlockPredictionMode == Intra_4x4) || (MacroBlockPredictionMode == Intra_8x8) || (MacroBlockPredictionMode == Intra_16x16)) {
                if (MacroBlockPredictionMode == Intra_4x4) {
                    for (uint8_t Luma4x4Block = 0; Luma4x4Block < 16; Luma4x4Block++) { // luma4x4BlkIdx
                        Options->MacroBlock->PreviousIntra4x4PredictionFlag[Luma4x4Block] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                        if (Options->MacroBlock->PreviousIntra4x4PredictionFlag[Luma4x4Block] == false) {
                            Options->MacroBlock->RemIntra4x4PredictionMode[Luma4x4Block] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 3);
                        }
                    }
                }
                if (MacroBlockPredictionMode == Intra_8x8) {
                    for (uint8_t Luma8x8Block = 0; Luma8x8Block < 4; Luma8x8Block++) { // luma8x8BlkIdx
                        Options->MacroBlock->PreviousIntra8x8PredictionFlag[Luma8x8Block] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                        if (Options->MacroBlock->PreviousIntra8x8PredictionFlag[Luma8x8Block] == false) {
                            Options->MacroBlock->RemIntra8x8PredictionMode[Luma8x8Block] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 3);
                        }
                    }
                }
                if ((Options->SPS->ChromaArrayType == AVCChroma_420) || (Options->SPS->ChromaArrayType == AVCChroma_422)) {
                    Options->MacroBlock->IntraChromaPredictionMode = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, false);
                }
            } else if (MacroBlockPredictionMode != Direct) {
                for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < NumMacroBlockPartitions(Options->MacroBlock->Type); MacroBlockPiece++) {
                    if ((Options->MacroBlock->NumRefIndexActiveLevel0 > 0 || Options->Slice->MacroBlockFieldDecodingFlag != Options->Slice->SliceIsInterlaced) && MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) != Pred_L1) {
                        Options->MacroBlock->RefIndexLevel0[MacroBlockPiece] = ReadRICE(BitB, true, 0);
                    }
                }
                for (uint32_t MacroBlockPiece = 0; MacroBlockPiece < NumMacroBlockPartitions(Options->MacroBlock->Type); MacroBlockPiece++) {
                    if ((Options->MacroBlock->NumRefIndexActiveLevel1 > 0 || Options->Slice->MacroBlockFieldDecodingFlag != Options->Slice->SliceIsInterlaced) && MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) != Pred_L0) {
                        Options->MacroBlock->RefIndexLevel1[MacroBlockPiece] = ReadRICE(BitB, true, 0);
                    }
                }
                for (uint32_t MacroBlockPiece = 0; MacroBlockPiece < NumMacroBlockPartitions(Options->MacroBlock->Type); MacroBlockPiece++) {
                    if (MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) != Pred_L1) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            Options->MacroBlock->MVDLevel0[MacroBlockPiece][0][compIdx] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
                        }
                    }
                }
                for (uint32_t MacroBlockPiece = 0; MacroBlockPiece < NumMacroBlockPartitions(Options->MacroBlock->Type); MacroBlockPiece++) {
                    if (MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) != Pred_L0) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            Options->MacroBlock->MVDLevel1[MacroBlockPiece][0][compIdx] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
                        }
                    }
                }
            }
        } else if (Options->PPS->EntropyCodingMode == Arithmetic) {
            uint8_t MacroBlockPredictionMode = MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0);
            if ((MacroBlockPredictionMode == Intra_4x4) || (MacroBlockPredictionMode == Intra_8x8) || (MacroBlockPredictionMode == Intra_16x16)) {
                if (MacroBlockPredictionMode == Intra_4x4) {
                    for (uint8_t Luma4x4Block = 0; Luma4x4Block < 16; Luma4x4Block++) { // luma4x4BlkIdx
                        Options->MacroBlock->PreviousIntra4x4PredictionFlag[Luma4x4Block] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        if (Options->MacroBlock->PreviousIntra4x4PredictionFlag[Luma4x4Block] == false) {
                            Options->MacroBlock->RemIntra4x4PredictionMode[Luma4x4Block] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                }
                if (MacroBlockPredictionMode == Intra_8x8) {
                    for (uint8_t Luma8x8Block = 0; Luma8x8Block < 4; Luma8x8Block++) { // luma8x8BlkIdx
                        Options->MacroBlock->PreviousIntra8x8PredictionFlag[Luma8x8Block] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        if (Options->MacroBlock->PreviousIntra8x8PredictionFlag[Luma8x8Block] == false) {
                            Options->MacroBlock->RemIntra8x8PredictionMode[Luma8x8Block] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                }
                if ((Options->SPS->ChromaArrayType == AVCChroma_420) || (Options->SPS->ChromaArrayType == AVCChroma_422)) {
                    Options->MacroBlock->IntraChromaPredictionMode = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                }
            } else if (MacroBlockPredictionMode != Direct) {
                for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < NumMacroBlockPartitions(Options->MacroBlock->Type); MacroBlockPiece++) {
                    if ((Options->MacroBlock->NumRefIndexActiveLevel0 > 0 || Options->Slice->MacroBlockFieldDecodingFlag != Options->Slice->SliceIsInterlaced) && MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) != Pred_L1) {
                        Options->MacroBlock->RefIndexLevel0[MacroBlockPiece] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                    }
                }
                for (uint32_t MacroBlockPiece = 0; MacroBlockPiece < NumMacroBlockPartitions(Options->MacroBlock->Type); MacroBlockPiece++) {
                    if ((Options->MacroBlock->NumRefIndexActiveLevel1 > 0 || Options->Slice->MacroBlockFieldDecodingFlag != Options->Slice->SliceIsInterlaced) && MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) != Pred_L0) {
                        Options->MacroBlock->RefIndexLevel1[MacroBlockPiece] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                    }
                }
                for (uint32_t MacroBlockPiece = 0; MacroBlockPiece < NumMacroBlockPartitions(Options->MacroBlock->Type); MacroBlockPiece++) {
                    if (MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) != Pred_L1) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            Options->MacroBlock->MVDLevel0[MacroBlockPiece][0][compIdx] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                }
                for (uint32_t MacroBlockPiece = 0; MacroBlockPiece < NumMacroBlockPartitions(Options->MacroBlock->Type); MacroBlockPiece++) {
                    if (MacroBlockPartitionPredictionMode(Options, Options->MacroBlock->Type, 0) != Pred_L0) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            Options->MacroBlock->MVDLevel1[MacroBlockPiece][0][compIdx] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                }
            }
        }
        Options->Slice->PicHeightInMapUnits      = Options->SPS->PicHeightInMapUnitsMinus1 + 1;
        Options->Slice->FrameHeightInMacroBlocks = (2 - Options->SPS->OnlyMacroBlocksInFrame) * Options->Slice->PicHeightInMapUnits;
        Options->Slice->PicWidthInMacroBlocks    = Options->SPS->PicWidthInMacroBlocksMinus1 + 1;
        Options->Slice->PicWidthInSamplesLuma    = Options->Slice->PicWidthInMacroBlocks * 16;
        Options->Slice->PicWidthInSamplesChroma  = Options->Slice->PicWidthInMacroBlocks * Options->SPS->MacroBlockWidthChroma;
        Options->Slice->PicHeightInMacroBlocks   = Options->Slice->FrameHeightInMacroBlocks / (Options->Slice->SliceIsInterlaced + 1);
        Options->Slice->RawMacroBlockSizeInBits  = 256 * Options->SPS->BitDepthLuma + 2 * Options->SPS->MacroBlockWidthChroma * Options->SPS->MacroBlockHeightChroma * Options->SPS->BitDepthChroma;
        Options->SPS->QPBDOffsetChroma           = Options->SPS->ChromaBitDepthMinus8 * 6;
    }
    
    void DecodeMacroBlock(AVCOptions *Options, uint8_t *MacroBlock, size_t MacroBlockSize) {
        AssertIO(Options != NULL);
        AssertIO(MacroBlock != NULL);
        if (Options->SPS->ChromaFormatIDC == AVCChroma_Gray) { // black and white
            
        } else if (Options->SPS->ChromaFormatIDC == AVCChroma_420) { // 4:2:0
            
        } else if (Options->SPS->ChromaFormatIDC == AVCChroma_422) { // 4:2:2
            
        } else if (Options->SPS->ChromaFormatIDC == AVCChroma_444) { // 4:4:4
            
        }
    }
    
    void ParseSubMacroBlockPredictionInSVC(AVCOptions *Options, BitBuffer *BitB) { // sub_mb_pred_in_scalable_extension
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        if (Options->PPS->EntropyCodingMode == ExpGolomb) {
            for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) { // MacroBlockPiece
                Options->MacroBlock->SubMacroBlockType[MacroBlockPiece] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, CountUnary, AVCStopBit);
            }
            if ((InCropWindow(Options, Options->Slice->CurrentMacroBlockAddress) == true) && (Options->Slice->AdaptiveMotionPredictionFlag == true)) {
                for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) {
                    if (SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Direct && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L1) {
                        Options->MacroBlock->MotionPredictionFlagLevel0[MacroBlockPiece] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                    }
                    if (SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Direct && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L0) {
                        Options->MacroBlock->MotionPredictionFlagLevel1[MacroBlockPiece] = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
                    }
                }
            }
            for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) { // MacroBlockPiece
                if ((Options->MacroBlock->NumRefIndexActiveLevel0 > 0 || Options->Slice->MacroBlockFieldDecodingFlag != Options->Slice->SliceIsInterlaced) && Options->MacroBlock->Type != P_8x8ref0 && SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L1 && !motion_prediction_flag_l0[MacroBlockPiece]) {
                    
                    Options->SPS->RefIndexLevel0[MacroBlockPiece] = ReadRICE(BitB, true, 0) + 1;
                }
                if ((Options->MacroBlock->NumRefIndexActiveLevel1 > 0 || Options->Slice->MacroBlockFieldDecodingFlag != Options->Slice->SliceIsInterlaced) && SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L0 && !motion_prediction_flag_l1[MacroBlockPiece]) {
                    
                    Options->SPS->RefIndexLevel1[MacroBlockPiece] = ReadRICE(BitB, true, 0) + 1;
                    
                }
                
                if (SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L1) {
                    for (uint32_t SubMacroBlockPiece = 0; SubMacroBlockPiece < NumSubMbPart(SubMacroBlockType[MacroBlockPiece]); SubMacroBlockPiece++) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            Options->MacroBlock->MVDLevel0[MacroBlockPiece][SubMacroBlockPiece][compIdx] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, WholeUnary, AVCStopBit);
                        }
                    }
                }
                
                if (SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L0) {
                    for (uint32_t SubMacroBlockPiece = 0; SubMacroBlockPiece < NumSubMbPart(SubMacroBlockType[MacroBlockPiece]); SubMacroBlockPiece++) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            Options->MacroBlock->MVDLevel1[MacroBlockPiece][SubMacroBlockPiece][compIdx] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, WholeUnary, AVCStopBit);
                        }
                    }
                }
            }
        } else if (Options->PPS->EntropyCodingMode == Arithmetic) {
            for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) { // MacroBlockPiece
                Options->MacroBlock->SubMacroBlockType[MacroBlockPiece] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
            }
            if ((InCropWindow(Options, Options->Slice->CurrentMacroBlockAddress) == true) && (Options->Slice->AdaptiveMotionPredictionFlag == true)) {
                for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) {
                    if (SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Direct && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L1) {
                        Options->MacroBlock->MotionPredictionFlagLevel0[MacroBlockPiece] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                    }
                    if (SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Direct && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L0) {
                        Options->MacroBlock->MotionPredictionFlagLevel1[MacroBlockPiece] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                    }
                }
            }
            for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) { // MacroBlockPiece
                if ((Options->MacroBlock->NumRefIndexActiveLevel0 > 0 || Options->Slice->MacroBlockFieldDecodingFlag != Options->Slice->SliceIsInterlaced) && Options->MacroBlock->Type != P_8x8ref0 && SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L1 && !motion_prediction_flag_l0[MacroBlockPiece]) {
                    
                    Options->SPS->RefIndexLevel0[MacroBlockPiece] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL) + 1;
                    
                }
                if ((Options->MacroBlock->NumRefIndexActiveLevel1 > 0 || Options->Slice->MacroBlockFieldDecodingFlag != Options->Slice->SliceIsInterlaced) && SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L0 && !motion_prediction_flag_l1[MacroBlockPiece]) {
                    
                    Options->SPS->RefIndexLevel1[MacroBlockPiece] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL) + 1;
                    
                }
                
                if (SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L1) {
                    for (uint32_t SubMacroBlockPiece = 0; SubMacroBlockPiece < NumSubMbPart(SubMacroBlockType[MacroBlockPiece]); SubMacroBlockPiece++) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            Options->MacroBlock->MVDLevel0[MacroBlockPiece][SubMacroBlockPiece][compIdx] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                }
                
                if (SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L0) {
                    for (uint32_t SubMacroBlockPiece = 0; SubMacroBlockPiece < NumSubMbPart(SubMacroBlockType[MacroBlockPiece]); SubMacroBlockPiece++) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            Options->MacroBlock->MVDLevel1[MacroBlockPiece][SubMacroBlockPiece][compIdx] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                }
            }
        }
    }
    
    void ParseReferenceBasePictureSyntax(AVCOptions *Options, BitBuffer *BitB) { // dec_ref_base_pic_marking
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        Options->SVC->AdaptiveMarkingModeFlag = BitBuffer_ReadBits(BitB, ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, 1);
        if (Options->SVC->AdaptiveMarkingModeFlag == true) {
            Options->SVC->BaseControlOperation = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, CountUnary, AVCStopBit);
            if (Options->SVC->BaseControlOperation == 1) {
                Options->SVC->NumBasePicDiff = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, CountUnary, AVCStopBit) + 1;
            } else if (Options->SVC->BaseControlOperation == 2) {
                Options->SVC->LongTermBasePicNum = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, CountUnary, AVCStopBit);
            } while(Options->SVC->BaseControlOperation != 0) {
                
            }
        }
    }
    
    void ParseSubMacroBlockPrediction(AVCOptions *Options, BitBuffer *BitB, uint8_t mb_type) { // sub_mb_pred
        AssertIO(Options != NULL);
        AssertIO(BitB != NULL);
        if (Options->PPS->EntropyCodingMode == ExpGolomb) {
            for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) {
                SubMacroBlockType[MacroBlockPiece] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, CountUnary, AVCStopBit);
            }
            for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) {
                if ((Options->MacroBlock->NumRefIndexActiveLevel0 > 0 || Options->Slice->MacroBlockFieldDecodingFlag != Options->Slice->SliceIsInterlaced) && Options->MacroBlock->Type != P_8x8ref0 && SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L1) {
                    Options->MacroBlock->RefIndexLevel0[MacroBlockPiece] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true) + 1;
                }
            }
            for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) {
                if ((Options->MacroBlock->NumRefIndexActiveLevel1 > 0 || Options->Slice->MacroBlockFieldDecodingFlag != Options->Slice->SliceIsInterlaced) && SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L0) {
                    Options->MacroBlock->RefIndexLevel1[MacroBlockPiece] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true) + 1;
                }
            }
            for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) {
                if (SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) !=Pred_L1) {
                    for (uint32_t SubMacroBlockPiece = 0; SubMacroBlockPiece < NumSubMbPart(SubMacroBlockType[MacroBlockPiece]);SubMacroBlockPiece++) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            Options->MacroBlock->MVDLevel0[MacroBlockPiece][SubMacroBlockPiece][compIdx] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
                        }
                    }
                }
            }
            for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) {
                if (SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L0) {
                    for (uint32_t SubMacroBlockPiece = 0; SubMacroBlockPiece < NumSubMbPart(SubMacroBlockType[MacroBlockPiece]);SubMacroBlockPiece++) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            Options->MacroBlock->MVDLevel1[MacroBlockPiece][SubMacroBlockPiece][compIdx] = ReadExpGolomb(ByteOrder_MSByteIsLeft, BitOrder_MSBitIsRight, BitB, true);
                        }
                    }
                }
            }
        } else if (Options->PPS->EntropyCodingMode == Arithmetic) {
            for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) {
                SubMacroBlockType[MacroBlockPiece] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
            }
            for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) {
                if ((Options->MacroBlock->NumRefIndexActiveLevel0 > 0 || Options->Slice->MacroBlockFieldDecodingFlag != Options->Slice->SliceIsInterlaced) && Options->MacroBlock->Type != P_8x8ref0 && SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L1) {
                    Options->MacroBlock->RefIndexLevel0[MacroBlockPiece] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL) + 1;
                }
            }
            for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) {
                if ((Options->MacroBlock->NumRefIndexActiveLevel1 > 0 || Options->Slice->MacroBlockFieldDecodingFlag != Options->Slice->SliceIsInterlaced) && SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L0) {
                    Options->MacroBlock->RefIndexLevel1[MacroBlockPiece] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL) + 1;
                }
            }
            for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) {
                if (SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) !=Pred_L1) {
                    for (uint32_t SubMacroBlockPiece = 0; SubMacroBlockPiece < NumSubMbPart(SubMacroBlockType[MacroBlockPiece]);SubMacroBlockPiece++) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            Options->MacroBlock->MVDLevel0[MacroBlockPiece][SubMacroBlockPiece][compIdx] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                }
            }
            for (uint8_t MacroBlockPiece = 0; MacroBlockPiece < 4; MacroBlockPiece++) {
                if (SubMacroBlockType[MacroBlockPiece] != B_Direct_8x8 && SubMbPredMode(SubMacroBlockType[MacroBlockPiece]) != Pred_L0) {
                    for (uint32_t SubMacroBlockPiece = 0; SubMacroBlockPiece < NumSubMbPart(SubMacroBlockType[MacroBlockPiece]);SubMacroBlockPiece++) {
                        for (uint8_t compIdx = 0; compIdx < 2; compIdx++) {
                            Options->MacroBlock->MVDLevel1[MacroBlockPiece][SubMacroBlockPiece][compIdx] = ReadArithmetic(BitB, NULL, NULL, NULL, NULL);
                        }
                    }
                }
            }
        }
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
