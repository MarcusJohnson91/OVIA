#include <stdlib.h>

#include "../../../Dependencies/libPCM/Dependencies/BitIO/libBitIO/include/StringIO.h"
#include "../../../Dependencies/libPCM/Dependencies/BitIO/libBitIO/include/BitIOMath.h"

#include "../../include/libModernPNG.h"
#include "../../include/Private/libModernPNG_Types.h"

#include "../../include/Private/Decode/libModernPNG_Decode.h"
#include "../../include/Private/Decode/libModernPNG_ReadChunks.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#define strcasecmp _stricmp
#endif
    
    /*
    bool VerifyChunkCRC(BitBuffer *BitB, uint32_t ChunkSize) {
        // So basically we need to read ChunkSize bytes into an array, then read the following 4 bytes as the CRC
        // then run VerifyCRC over the buffer, and finally compare the generated CRC with the extracted one, and return whether they match.
        // Then call SkipBits(BitB, Bytes2Bits(ChunkSize)); to reset to the beginning of the chunk
        uint8_t *Buffer2CRC = calloc(1, ChunkSize * sizeof(uint8_t));
        for (uint32_t Byte = 0; Byte < ChunkSize; Byte++) {
            Buffer2CRC[Byte] = BitB->Buffer[Bits2Bytes(, false)];
            free(Buffer2CRC);
            
        }
        uint32_t ChunkCRC = ReadBits(BitIOMSByteFirst, BitIOLSBitFirst, BitB, 32);
        bool CRCsMatch = VerifyCRC(Buffer2CRC, ChunkSize, 0x82608EDB, 32, 0xFFFFFFFF, ChunkCRC);
        SkipBits(BitB, -Bytes2Bits(ChunkSize));
        return CRCsMatch;
    }
    */
    
    uint8_t PaethPredictor(int64_t Left, int64_t Above, int64_t UpperLeft) {
        int64_t Guess     = Left + Above - UpperLeft;
        int64_t DistanceA = llabs(Guess - Left);
        int64_t DistanceB = llabs(Guess - Above);
        int64_t DistanceC = llabs(Guess - UpperLeft);
        
        uint8_t Output = 0;
        if (DistanceA <= DistanceB && DistanceA < DistanceC) {
            Output = DistanceA;
        } else if (DistanceB < DistanceC) {
            Output = DistanceB;
        } else {
            Output = DistanceC;
        }
        return Output;
    }
    
    void PNGDecodeNonFilter(DecodePNG *Dec, uint8_t ***InflatedData, uint8_t ***DeFilteredData, size_t Line) {
        for (uint8_t StereoView = 0; StereoView < Dec->sTER->StereoType; StereoView++) {
            for (uint32_t Width = 0; Width < Dec->iHDR->Width; Width++) {
                for (size_t Byte = 1; Byte < Bits2Bytes(Dec->iHDR->BitDepth, true); Byte++) {
                    DeFilteredData[StereoView][Width][Byte - 1] = InflatedData[StereoView][Width][Byte]; // Remove filter indicating byte
                }
            }
        }
    }
    
    void PNGDecodeSubFilter(DecodePNG *Dec, uint8_t ***InflatedData, uint8_t ***DeFilteredData, size_t Line) {
        for (uint8_t StereoView = 0; StereoView < Dec->sTER->StereoType; StereoView++) {
            for (uint32_t Width = 0; Width < Dec->iHDR->Width; Width++) {
                for (size_t Byte = 1; Byte < Bits2Bytes(Dec->iHDR->BitDepth, true); Byte++) {
                    DeFilteredData[StereoView][Line][Byte - 1] = (InflatedData[StereoView][Line][Byte] + InflatedData[StereoView][Line][Byte+1]) & 0xFF; // Byte+1 is wrong, needs to be + bitdepth
                }
            }
        }
    }
    
    void PNGDecodeUpFilter(DecodePNG *Dec, uint8_t ***InflatedData, uint8_t ***DeFilteredData, size_t Line) {
        for (uint8_t StereoView = 0; StereoView < Dec->sTER->StereoType; StereoView++) {
            for (uint32_t Width = 0; Width < Dec->iHDR->Width; Width++) {
                for (size_t Byte = 1; Byte < Bits2Bytes(Dec->iHDR->BitDepth, true); Byte++) {
                    DeFilteredData[StereoView][Line][Byte - 1] = InflatedData[StereoView][Line][Byte] + InflatedData[StereoView][Line - 1][Byte] & 0xFF;
                }
            }
        }
    }
    
    void PNGDecodeAverageFilter(DecodePNG *Dec, uint8_t ***InflatedData, uint8_t ***DeFilteredData, size_t Line) {
        uint8_t PixelSize = Bits2Bytes(Dec->iHDR->BitDepth, true);
        for (uint8_t StereoView = 0; StereoView < Dec->sTER->StereoType; StereoView++) {
        for (uint32_t Width = 0; Width < Dec->iHDR->Width; Width++) {
            for (size_t Byte = 1; Byte < Bits2Bytes(Dec->iHDR->BitDepth, true); Byte++) {
                uint8_t Average = floor((InflatedData[StereoView][Line][Byte - (PixelSize)] + InflatedData[StereoView][Line - 1][Byte]) / 2);
                DeFilteredData[StereoView][Line][Byte - 1] = InflatedData[StereoView][Line][Byte] + Average;
            }
        }
        }
    }
    
    void PNGDecodePaethFilter(DecodePNG *Dec, uint8_t ***InflatedData, uint8_t ***DeFilteredData, size_t Line) {
        // Filtering is applied to bytes, not pixels
        uint8_t PixelSize = Bits2Bytes(Dec->iHDR->BitDepth, true);
        for (uint8_t StereoView = 0; StereoView < Dec->sTER->StereoType; StereoView++) {
        for (uint32_t Width = 0; Width < Dec->iHDR->Width; Width++) {
            for (size_t Byte = 1; Byte < Bits2Bytes(Dec->iHDR->BitDepth, true); Byte++) {
                if (Line == 0) { // Assume top and top left = 0
                    DeFilteredData[StereoView][Line][Byte] = PaethPredictor(InflatedData[StereoView][Line][Byte], 0, 0);
                } else {
                    DeFilteredData[StereoView][Line][Byte] = PaethPredictor(InflatedData[StereoView][Line][Byte], InflatedData[StereoView][Line][Byte - PixelSize], InflatedData[StereoView][Line - 1][Byte - PixelSize]);
                }
            }
        }
        }
    }
    
    // These filters need to be operatable on every line in any order, so we need a main PNGDecodeFilteredImage function.
    // That function then needs to read the first byte of each line to see what filter is used
    // then decode each line.
    // ALSO keep in mind concurrency.
    
    libModernPNGFilterTypes ExtractLineFilterType(uint8_t *Line) {
        uint8_t FilterType = Line[0];
        return FilterType;
    }
    
    void PNGDecodeFilteredImage(DecodePNG *Dec, uint8_t ***InflatedBuffer) {
        
		uint8_t ***DeFilteredData = calloc((Dec->iHDR->Height * Dec->iHDR->Width), Bits2Bytes(Dec->iHDR->BitDepth, Yes));
        
        for (uint8_t StereoView = 0; StereoView < IsPNGStereoscopic(Dec); StereoView++) {
            for (size_t Line = 0; Line < Dec->iHDR->Height; Line++) {
                libModernPNGFilterTypes FilterType = ExtractLineFilterType(Line);
                switch (FilterType) {
                    case NotFiltered:
                        // copy the Line except byte 0 (the filter indication byte) to the output buffer.
                        PNGDecodeNonFilter(Dec, InflatedBuffer, DeFilteredData, Line);
                        break;
                    case SubFilter:
                        // SubFilter
                        PNGDecodeSubFilter(Dec, InflatedBuffer, DeFilteredData, Line);
                        break;
                    case UpFilter:
                        // UpFilter
                        PNGDecodeUpFilter(Dec, InflatedBuffer, DeFilteredData, Line);
                        break;
                    case AverageFilter:
                        // AverageFilter
                        PNGDecodeAverageFilter(Dec, InflatedBuffer, DeFilteredData, Line);
                        break;
                    case PaethFilter:
                        // PaethFilter
                        PNGDecodePaethFilter(Dec, InflatedBuffer, DeFilteredData, Line);
                        break;
                    default:
                        BitIOLog(BitIOLog_ERROR, libModernPNGLibraryName, __func__, u8"Filter type: %d is invalid\n", FilterType);
                        break;
                }
            }
        }
        free(DeFilteredData);
    }
    
    enum DEFLATEBlockTypes {
        UncompressedBlock   = 0,
        FixedHuffmanBlock   = 1,
        DynamicHuffmanBlock = 2,
        
    };
    
    void DecodeDEFLATEBlock(DecodePNG *Dec, BitBuffer *DEFLATEBlock, BitBuffer *DecodedBlock) { // LSByteFirst
        // Huffman codes are written MSBit first, everything else is writen LSBit first
        
        // DEFLATE Block header:
        bool     BlockIsFinal = ReadBits(BitIOMSByteFirst, BitIOLSBitFirst, DEFLATEBlock, 1); // BFINAL
        uint8_t  BlockType    = ReadBits(BitIOMSByteFirst, BitIOLSBitFirst, DEFLATEBlock, 2); // BTYPE
        
        if (BlockType == DynamicHuffmanBlock) {
            
        } else if (BlockType == FixedHuffmanBlock) {
            
        } else if (BlockType == UncompressedBlock) {
            
        }
    }
    
    void DecodeIFDATChunk(DecodePNG *Dec, BitBuffer *DAT2Decode, BitBuffer *DecodedDAT, uint64_t DATSize) { // Decodes both fDAT and IDAT chunks
        // well lets go ahead and allocate a DAT block the size of DATSize
        BitBuffer_Init(DATSize);
        // Now we need to go ahead and parse the ZLIB Header.
        // ok so how do we do that? I wrote some notes on the Zlib header last night...
        
        /* Compression Method and Flags byte */
        uint8_t CompressionMethod = ReadBits(BitIOMSByteFirst, BitIOLSBitFirst, DAT2Decode, 4); // 8
        uint8_t CompressionInfo   = ReadBits(BitIOMSByteFirst, BitIOLSBitFirst, DAT2Decode, 4); // 7
        /* Compression Method and Flags byte */
        
        /* FlagByte */
        uint8_t FCHECK            = ReadBits(BitIOMSByteFirst, BitIOLSBitFirst, DAT2Decode, 5); // 1E
        bool    FDICTPresent      = ReadBits(BitIOMSByteFirst, BitIOLSBitFirst, DAT2Decode, 1); // 0
        uint8_t FLEVEL            = ReadBits(BitIOMSByteFirst, BitIOLSBitFirst, DAT2Decode, 2); // 1
        /* FlagByte */
        
        if (FDICTPresent) {
            // Read TableID which is 4 bytes
            uint32_t DICTID       = ReadBits(BitIOMSByteFirst, BitIOLSBitFirst, DAT2Decode, 32);
        }
        
        // Start reading the DEFLATE block?
         
    }
    
    void DecodePNGData(DecodePNG *Dec, BitBuffer *BitB) {
        // read the iDAT/fDAT chunk header, then do the other stuff.
        while (BitBuffer_GetSize(BitB) > 0) { // 12 is the start of IEND
            uint32_t ChunkSize   = ReadBits(BitIOMSByteFirst, BitIOLSBitFirst, BitB, 32);
            uint32_t ChunkID     = ReadBits(BitIOMSByteFirst, BitIOLSBitFirst, BitB, 32);
            
            
            if (ChunkID == acTLMarker) {
                ParseACTL(Dec, BitB, ChunkSize);
            } else if (ChunkID == bKGDMarker) {
                ParseBKGD(Dec, BitB, ChunkSize);
            } else if (ChunkID == cHRMMarker) {
                ParseCHRM(Dec, BitB, ChunkSize);
            } else if (ChunkID == fcTLMarker) {
                ParseFCTL(Dec, BitB, ChunkSize);
            } else if (ChunkID == gAMAMarker) {
                ParseGAMA(Dec, BitB, ChunkSize);
            } else if (ChunkID == hISTMarker) {
                ParseHIST(Dec, BitB, ChunkSize);
            } else if (ChunkID == iCCPMarker) {
                ParseOFFS(Dec, BitB, ChunkSize);
            } else if (ChunkID == IDATMarker || ChunkID == fDATMarker) {
                DecodeIFDATChunk(Dec, BitB, NULL, ChunkSize);
            } else if (ChunkID == iHDRMarker) {
                ParseIHDR(Dec, BitB, ChunkSize);
            } else if (ChunkID == iTXtMarker) {
                ParseITXt(Dec, BitB, ChunkSize);
            } else if (ChunkID == oFFsMarker) {
                ParseOFFS(Dec, BitB, ChunkSize);
            } else if (ChunkID == pCALMarker) {
                ParsePCAL(Dec, BitB, ChunkSize);
            } else if (ChunkID == pHYsMarker) {
                ParsePHYS(Dec, BitB, ChunkSize);
            } else if (ChunkID == PLTEMarker) {
                ParsePLTE(Dec, BitB, ChunkSize);
            } else if (ChunkID == sBITMarker) {
                ParseSBIT(Dec, BitB, ChunkSize);
            } else if (ChunkID == sRGBMarker) {
                ParseSRGB(Dec, BitB, ChunkSize);
            } else if (ChunkID == sTERMarker) {
                ParseSTER(Dec, BitB, ChunkSize);
            } else if (ChunkID == tEXtMarker) {
                ParseTEXt(Dec, BitB, ChunkSize);
            } else if (ChunkID == zTXtMarker) {
                ParseZTXt(Dec, BitB, ChunkSize);
            }
        }
    }
    
    // Let's do this library right, by adding attach and delete functions for the various chunks, and let's also have a fancy function that applies color profiles to the pixels.
    // that's kinda a lot of work tho...
    uint16_t ***DecodePNGImage(DecodePNG *Dec, BitBuffer *PNGFile) {
        return NULL;
    }
    
    BitBuffer *DecodeAdam7(DecodePNG *Dec, BitBuffer *InterlacedImage) { // Returns the interlaced image
        /*
         Ok, so to interlace an image, we need to loop over every pixel in an 8x8 block?
         */
        BitBuffer *ProgressiveImage = NULL;
        return ProgressiveImage;
    }
    
#ifdef __cplusplus
}
#endif
