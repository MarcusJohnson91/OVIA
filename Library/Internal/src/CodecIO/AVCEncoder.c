#include "../../include/Private/Common/libModernAVC_Types.h"
#include "../../include/Private/Encode/libModernAVC_Encode.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    EncodeAVC *InitAVCEncoder(void) {
        EncodeAVC *Enc                = (EncodeAVC*) calloc(1, sizeof(EncodeAVC));
        if (Enc == NULL) {
            BitIOLog(LOG_ERROR, "libModernAVC", "InitAVCEncoder", "Not enough memory to allocate EncodeAVC");
        } else {
            Enc->NAL                  = (NALHeader*)                    calloc(1, sizeof(NALHeader));
            Enc->SPS                  = (SequenceParameterSet*)         calloc(1, sizeof(SequenceParameterSet));
            Enc->PPS                  = (PictureParameterSet*)          calloc(1, sizeof(PictureParameterSet));
            Enc->VUI                  = (VideoUsabilityInformation*)    calloc(1, sizeof(VideoUsabilityInformation));
            Enc->HRD                  = (HypotheticalReferenceDecoder*) calloc(1, sizeof(HypotheticalReferenceDecoder));
            Enc->SEI                  = (SupplementalEnhancementInfo*)  calloc(1, sizeof(SupplementalEnhancementInfo));
            Enc->Slice                = (Slice*)                        calloc(1, sizeof(Slice));
            Enc->SVC                  = (ScalableVideoCoding*)          calloc(1, sizeof(ScalableVideoCoding));
            Enc->DPS                  = (DepthParameterSet*)            calloc(1, sizeof(DepthParameterSet));
            Enc->MacroBlock           = (MacroBlock*)                   calloc(1, sizeof(MacroBlock));
        }
        return Enc;
    }
    
    bool AreAllViewsPaired(EncodeAVC *Enc) {
        if (Enc == NULL) {
            BitIOLog(LOG_ERROR, "libModernAVC", "ResidualLuma", "Pointer to EncodeAVC is NULL");
        } else {
            bool AllViewsPairedFlag = false;
            for (uint8_t View = 0; View < Enc->SPS->ViewCount; View++) {
                AllViewsPairedFlag = (1 && Enc->SPS->DepthViewPresent[View] && Enc->SPS->TextureViewPresent[View]);
            }
        }
        return AllViewsPairedFlag;
    }
    
    void ParseTransformCoeffs(EncodeAVC *Enc, uint8_t i16x16DC, uint8_t i16x16AC, uint8_t Level4x4, uint8_t Level8x8, uint8_t StartIndex, uint8_t EndIndex) { // ParseTransformCoeffs
        if (Enc == NULL) {
            BitIOLog(LOG_ERROR, "libModernAVC", "ResidualLuma", "Pointer to EncodeAVC is NULL");
        } else if (BitB == NULL) {
            BitIOLog(LOG_ERROR, "libModernAVC", "ResidualLuma", "Pointer to BitBuffer is NULL");
        } else {
            uint8_t Intra16x16DCLevel = i16x16DC, Intra16x16ACLevel = i16x16AC, LumaLevel4x4 = Level4x4, LumaLevel8x8 = Level8x8;
            // Return the first 4 variables
            if (Enc->PPS->EntropyCodingMode == ExpGolomb) {
                // Read ExpGolomb
            } else {
                // Read Arithmetic
            }
            
            if (Enc->SPS->ChromaArrayType == Chroma420 || Enc->SPS->ChromaArrayType == Chroma422) {
                for (uint8_t iCbCr = 0; iCbCr < 1; iCbCr++) {
                    ChromaDCLevel[iCbCr] = 4 * NumC8x8;
                    for (uint8_t ChromaBlock = 0; ChromaBlock < i4x4; ChromaBlock++) {
                        ChromaACLevel[iCbCr][(i8x8 * 4) + i4x4] = 0;
                    }
                }
            } else if (Enc->SPS->ChromaArrayType == Chroma444) {
                
            }
        }
    }
    
    void ResidualLuma(EncodeAVC *Enc, BitBuffer *BitB, int i16x16DClevel, int i16x16AClevel, int level4x4,
                      int level8x8, int startIdx, int endIdx) { // residual_luma
        if (Enc == NULL) {
            BitIOLog(LOG_ERROR, "libModernAVC", "ResidualLuma", "Pointer to EncodeAVC is NULL");
        } else if (BitB == NULL) {
            BitIOLog(LOG_ERROR, "libModernAVC", "ResidualLuma", "Pointer to BitBuffer is NULL");
        } else {
            if (startIdx == 0 && MacroBlockPartitionPredictionMode(Enc, Enc->MacroBlock->Type, 0) == Intra_16x16) {
                ParseTransformCoeffs(Enc, i16x16DClevel, 0, 15, 16);
            }
            for (uint8_t i8x8 = 0; i8x8 < 4; i8x8++) {
                if (Enc->MacroBlock->TransformSizeIs8x8 == false || Enc->PPS->EntropyCodingMode == ExpGolomb) {
                    for (uint8_t i4x4 = 0; i4x4 < 4; i4x4++) {
                        if (Enc->MacroBlock->BlockPatternLuma & (1 << i8x8)) {
                            if (MacroBlockPartitionPredictionMode(Enc, Enc->MacroBlock->Type, 0) == Intra_16x16) {
                                ParseTransformCoeffs(i16x16AClevel[i8x8 * 4 + i4x4], Max(0, startIdx - 1), endIdx - 1, 15);
                            } else {
                                ParseTransformCoeffs(level4x4[i8x8 * 4 + i4x4], startIdx, endIdx, 16);
                            }
                        } else if (MacroBlockPartitionPredictionMode(Enc, Enc->MacroBlock->Type, 0) == Intra_16x16) {
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
                if (Enc->PPS->EntropyCodingMode == ExpGolomb && Enc->MacroBlock->TransformSizeIs8x8) {
                    for (uint8_t i = 0; i < 16; i++) {
                        level8x8[i8x8][4 * i + i4x4] = level4x4[i8x8 * 4 + i4x4][i];
                    }
                }
                else if (Enc->MacroBlock->BlockPatternLuma & (1 << i8x8)) {
                    ParseTransformCoeffs(level8x8[i8x8], 4 * startIdx, 4 * endIdx + 3, 64);
                } else {
                    for (uint8_t i = 0; i < 64; i++) {
                        level8x8[i8x8][i] = 0;
                    }
                }
            }
        }
    }
    
    int8_t MacroBlock2SliceGroupMap(EncodeAVC *Enc, uint8_t CurrentMacroBlock) { // MbToSliceGroupMap
        if (Enc == NULL) {
            BitIOLog(LOG_ERROR, "libModernAVC", "MacroBlock2SliceGroupMap", "Pointer to EncodeAVC is NULL");
        } else {
            if (Enc->PPS->SliceGroups == 1 && (Enc->PPS->SliceGroupMapType == 3 || Enc->PPS->SliceGroupMapType == 4 || Enc->PPS->SliceGroupMapType == 5)) {
                if (Enc->PPS->SliceGroupMapType == 3) {
                    if (Enc->PPS->SliceGroupChangeDirection == false) {
                        return BoxOutClockwise;
                    } else if (Enc->PPS->SliceGroupChangeDirection == true) {
                        return BoxOutCounterClockwise;
                    }
                } else if (Enc->PPS->SliceGroupMapType == 4) {
                    if (Enc->PPS->SliceGroupChangeDirection == false) {
                        return RasterScan;
                    } else if (Enc->PPS->SliceGroupChangeDirection == true) {
                        return ReverseRasterScan;
                    }
                } else if (Enc->PPS->SliceGroupMapType == 5) {
                    if (Enc->PPS->SliceGroupChangeDirection == false) {
                        return WipeRight;
                    } else if (Enc->PPS->SliceGroupChangeDirection == true) {
                        return WipeLeft;
                    }
                }
            }
        }
        return -1; // failure
    }
    
    void rbsp_slice_trailing_bits(EncodeAVC *Enc, BitBuffer *BitB) {
        if (Enc == NULL) {
            BitIOLog(LOG_ERROR, "libModernAVC", "GetSizeOfNALUnit", "Pointer to EncodeAVC is NULL");
        } else {
            AlignBitBuffer(BitB, 1); // rbsp_trailing_bits();
            if (Enc->PPS->EntropyCodingMode == Arithmetic) {
                while (more_rbsp_trailing_data()) {
                    uint16_t CABACZeroWord = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16); /* equal to 0x0000 */
                }
            }
        }
    }
    
    uint8_t MacroBlockPartitionPredictionMode(EncodeAVC *Enc, uint8_t MacroBlockType, uint8_t PartitionNumber) {  // MbPartPredMode
        uint8_t ReturnValue = 0;
        if (Enc == NULL) {
            BitIOLog(LOG_ERROR, "libModernAVC", "GetSizeOfNALUnit", "Pointer to EncodeAVC is NULL");
        } else {
            if (MacroBlockType == 0) {
                if (Enc->MacroBlock->TransformSizeIs8x8 == true) {
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
    
    uint64_t NextMacroBlockAddress(EncodeAVC *Enc, uint64_t CurrentMacroBlockAddress) { // NextMbAddress
        if (Enc == NULL) {
            BitIOLog(LOG_ERROR, "libModernAVC", "GetSizeOfNALUnit", "Pointer to EncodeAVC is NULL");
        } else {
            while (CurrentMacroBlockAddress + 1 < Enc->Slice->PicSizeInMacroBlocks && MacroBlock2SliceGroupMap(Enc, CurrentMacroBlockAddress + 1) != MacroBlock2SliceGroupMap(Enc, CurrentMacroBlockAddress)) {
                i++; nextMbAddress = I
            }
            // aka
            for (uint64_t I = CurrentMacroBlockAddress + 1; I < Enc->Slice->PicSizeInMacroBlocks && MbToSliceGroups[I]) {
                MacroBlock2SliceGroupMap(Enc, CurrentMacroBlockAddress);
            }
        }
    }
    
    size_t GetSizeOfNALUnit(EncodeAVC *Enc, BitBuffer *BitB) {
        if (Enc == NULL) {
            BitIOLog(LOG_ERROR, "libModernAVC", "GetSizeOfNALUnit", "Pointer to EncodeAVC is NULL");
        } else if (BitB == NULL) {
            BitIOLog(LOG_ERROR, "libModernAVC", "GetSizeOfNALUnit", "Pointer to BitBuffer is NULL");
        } else {
            
        }
        return 0;
    }
    
    uint8_t CalculateNumberOfTimeStamps(EncodeAVC *Enc) { // PicOrderCount
        if (Enc == NULL) {
            BitIOLog(LOG_ERROR, "libModernAVC", "CalculateNumberOfTimeStamps", "Pointer to EncodeAVC is NULL");
        } else {
            uint8_t NumTimeStamps = 0;
            if ((Enc->Slice->SliceIsInterlaced == false) && (Enc->Slice->TopFieldOrderCount == Enc->Slice->BottomFieldOrderCount)) {
                NumTimeStamps = 1;
            } else if (0 == 1) {
                
            }
        }
        return 0;
    }
    
    /*
     So, the frontend will handle decoding input PNGs, and all we need to do is take in the command line info as well as the raw PNG
     So we need to create a struct to hod the extracted options from the command line?
     */
    
    
    /*
     void ConvertRGB2YCgCo(EncodeAVC *Enc, uint8_t NumChannels, uint8_t NumPixels, uint16_t *DecodedMB[3][256]) {
     uint16_t Temp, Green[NumChannels][NumPixels], Red[NumChannels][NumPixels], Blue[NumChannels][NumPixels];
     
     // Y  = DecodedMB[0]
     // Cb = DecodedMB[1]
     // Cr = DecodedMB[2]
     
     for (uint8_t Channel = 0; Channel < 3; Channel++) {
     for (uint8_t Pixel = 0; Pixel < NumPixels; Pixel++) {
     Temp                  = DecodedMB[0][Pixel] − (DecodedMB[1][Pixel] − (1 << (BitDepthC −1)));
     Green[Channel][Pixel] = Clip1Y(DecodedMB[0][Pixel] + (DecodedMB[1][Pixel] − (1 << (BitDepthC − 1))));
     Blue[Channel][Pixel]  = Clip1Y(Temp − (DecodedMB[2][Pixel] − (1 << (BitDepthC − 1))));
     Red[Channel][Pixel]   = Clip1Y(Temp + (DecodedMB[2][Pixel] − (1 << (BitDepthC − 1))));
     }
     }
     }
     */
    
    // When you encode AVC, make sure that you center the image, and add any black pixels to the edges you need in order to support any resolution.
    
    // Step 1: Decode the input PNG
    // Step 2: Optimize the frame layout for macroblock count.
    // Step 3: Measure the similarity between the frames to tell when we should make new IDR frames.
    
    // Also, on the encode side, after optimizing the Macroblock placement, we should scan through each macroblock to figure out how complex each macroblock is. if it's all black or something, quantized the ever loving fuck outta it, to reduce the bitrate.
    
    uint8_t MeasureMacroBlockSimilarity(uint16_t *PreviousRawPixels[64], uint16_t *CurrentRawPixels[64]) {
        return 0;
    }
    
    uint8_t MeasureMacroBlockEntropy(uint16_t *RawPixels[64]) {
        // lets say all pixels are 0xFF
        // oxFF * 64 = 16,320
        // 16320 / 64 = 0xFF, we need to decouple the absolute value from the similarity.
        // Well, what if I average it then compared the average to the smallest and largest values, and recorded that score as the similarity?
        
        // So 255, the smallest is equal to the largest, therefore the similarity is 100, and the difference is 0.
        
        // 1 Problem: not a whole damn lot happens in 16x16 pixels, so it'll have to be extremely precise to be worthwhile.
        
        // Test data time:
        // 16x16 pixel array.
        // RGB 16x16 array.
        /*
         D0F8DED0 F8DED1F8 DED1F8DE D1F7DDD0 E9DDF7DE CEF7DDCE F9DFD2F8 DED1F8DD
         3B445C4C 46588A72 7CDBBDBA FFEBE3FF D1F8DFD1 F8DED1F8 DED0F8DE D1F8DED1
         E5DDFDE6 DAF8DFD1 F8DED0F8 DED2F8DE 2B3D543B 3F516255 6193727B D4AFB0FE
         D0F8DED1 F8DED1F8 DED1F9E0 D3F8DED1 B1B4FFEA E2FFECE1 F7E0D1F8 DFD0F8DE
         2B3F5A2D 374C3C38 48584454 86666ED1 CFF8DFD0 F8DFD1F8 DFD1F8DE D1F8DDD0
         6975CAB3 B2FCE9E1 FBE6DAF9 E3D5F7DE 384B6C28 324C2E31 463F3A4D 5D4D5E80
         D4F8E0D1 F8DED0F8 DFD2F7DD D0F7DDD0 485B826E 7BC1A9AB F9E5DEFF ECE1F9E1
         52638929 36522D35 4C363549 3F3C4E56 DAF9E1D3 F8DFD1F9 E0D3F9DF D2F7DDD0
         4053584D 61745E6D C1A5A9FB E4DEFDE6 647BA126 35502532 4930364B 35354742
         DDFFECE1 F9E2D4F8 E0D3F8DF D3F8DFD2 35473D37 4C513F56 775C6BBB 9EA0FBE4
         4C65842B 41592133 46273347 30374A35 9BF5E0D7 FCE5D8FB E5D9F9E0 D4F7DFD2
         394E3733 4842344C 553E546F 5461B59B 334D5E31 4A5C2338 48223243 2B364935
         5EA18C8A E6D3C7FE E9DEFCE5 DAFAE2D6 34493434 4A3A3348 42344C4D 3C516755
         2B47592E 495D3149 5E27384D 2632472E 5060575D 9B8B88DD C9C1F8E0 D5FFE8DF
         31452D33 473A3B50 3C384E41 394E4A42 1F3C5521 3C552F47 612F425B 27354C27
         4E443E4D 524C538A 7B7ADCC8 C2F9E3DB 354A2C37 4B31394E 34374B36 384B3F3D
         32587728 49662E46 65354762 29375227 4C3A3C4C 3E394647 434B8374 77B9A1A1
         39542637 4C2D3A4E 303A4E34 3E50353C 80AFDD4B 7296334D 6B3D4E6D 32425F2B
         4C333949 393A493C 3646443F 4C6D5C6A 4261243C 56293F55 2A3A4D2C 3A4C303C
         D5EDFFA9 D0FA4264 8D2F4C6D 2D4A6D27 4F344051 353A4B3D 3B4E3A34 49484159
         567A223E 5C234059 22374B26 384B2D3E A0D2F4C8 EAFF96C4 F65A87B6 5783B22C
         57303D50 2A304231 32463430 4639324B A3CF224E 6D21425F 20385126 3C532F42
         3D65866A 9FCEA0D8 FFB5E3FF C4ECFF6C 572D3E53 262D4027 2B3E2D2D 42343047
         E8FF507C A828496D 1F365528 415D2A40 5173973E 6B9157A5 CF9BD5FF CEEEFFB9
         */
        
        // YCoCg algorithm:
        /*
         Co = R - B
         t = B + (Co >> 1)
         Cg = G - t
         Y = t + (Cg >> 1)
         */
        
        return 0;
    }
    
#ifdef __cplusplus
}
#endif
