#include "../../../include/Private/Image/PNMCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    static void PNMWritePAMHeader(OVIA *Ovia, BitBuffer *BitB) {
        
    }
    
    static void PNMWriteBinaryPNMHeader(OVIA *Ovia, BitBuffer *BitB) {
        
    }
    
    static void PNMWriteASCIIPNMHeader(OVIA *Ovia, BitBuffer *BitB) {
        
    }
    
    void PNMWriteHeader(PNMTypes PNMType, BitBuffer *BitB) {
        if (PNMType == PAMPNM) {
            PNMWritePAMHeader(Ovia, BitB);
        } else if (PNMType == BinaryPNM) {
            PNMWriteBinaryPNMHeader(Ovia, BitB);
        } else if (PNMType == ASCIIPNM) {
            PNMWriteASCIIPNMHeader(Ovia, BitB);
        }
    }
    
    void PNMInsertImage(OVIA *Ovia, BitBuffer *BitB, ImageContainer *Image) {
        if (Ovia != NULL && BitB != NULL && Image != NULL) {
            uint64_t ChannelCount = OVIA_GetNumChannels(Ovia);
            uint64_t Width        = ImageContainer_GetWidth(Image);
            uint64_t Height       = ImageContainer_GetHeight(Image);
            Image_Types Type      = ImageContainer_GetType(Image);
            if (Type == ImageContainer_2DSInteger8) {
                int8_t ****Array  = (int8_t****) ImageContainer_GetArray(Image);
                for (uint64_t W = 0ULL; W < Width; W++) {
                    for (uint64_t H = 0ULL; H < Height; H++) {
                        for (uint16_t Channel = 0; Channel < ChannelCount; Channel++) {
                            WriteBits(MSByteFirst, MSBitFirst, BitB, ChannelCount, Array[0][W][H][Channel]);
                        }
                    }
                }
            } else if (Type == ImageContainer_2DUInteger8) {
                uint8_t ****Array  = (uint8_t****) ImageContainer_GetArray(Image);
                for (uint64_t W = 0ULL; W < Width; W++) {
                    for (uint64_t H = 0ULL; H < Height; H++) {
                        for (uint16_t Channel = 0; Channel < ChannelCount; Channel++) {
                            WriteBits(MSByteFirst, MSBitFirst, BitB, ChannelCount, Array[0][W][H][Channel]);
                        }
                    }
                }
            } else if (Type == ImageContainer_2DSInteger16) {
                int16_t ****Array  = (int16_t****) ImageContainer_GetArray(Image);
                for (uint64_t W = 0ULL; W < Width; W++) {
                    for (uint64_t H = 0ULL; H < Height; H++) {
                        for (uint16_t Channel = 0; Channel < ChannelCount; Channel++) {
                            WriteBits(MSByteFirst, MSBitFirst, BitB, ChannelCount, Array[0][W][H][Channel]);
                        }
                    }
                }
            } else if (Type == ImageContainer_2DUInteger16) {
                uint16_t ****Array  = (uint16_t****) ImageContainer_GetArray(Image);
                for (uint64_t W = 0ULL; W < Width; W++) {
                    for (uint64_t H = 0ULL; H < Height; H++) {
                        for (uint16_t Channel = 0; Channel < ChannelCount; Channel++) {
                            WriteBits(MSByteFirst, MSBitFirst, BitB, ChannelCount, Array[0][W][H][Channel]);
                        }
                    }
                }
            }
        } else if (Ovia == NULL) {
            Log(Log_ERROR, __func__, U8("OVIA Pointer is NULL"));
        } else if (BitB == NULL) {
            Log(Log_ERROR, __func__, U8("BitBuffer Pointer is NULL"));
        } else if (Image == NULL) {
            Log(Log_ERROR, __func__, U8("Pixels2Write Pointer is NULL"));
        }
    }
    
#ifdef __cplusplus
}
#endif
