#include "../../../Dependencies/BitIO/libBitIO/include/BitIO.h"

#include "../../include/Encode/EncodeAVC.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    /*
     So, the frontend will handle decoding input PNGs, and all we need to do is take in the command line info as well as the raw PNG
     So we need to create a struct to hod the extracted options from the command line?
     */
    
    
    /*
     void ConvertRGB2YCgCo(AVCFile *AVC, uint8_t NumChannels, uint8_t NumPixels, uint16_t *DecodedMB[3][256]) {
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
