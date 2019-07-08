#include "../../include/Private/PNGCommon.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    PNGOptions *PNGOptions_Init(void) {
        PNGOptions *PNG  = calloc(1, sizeof(PNGOptions));
        if (PNG != NULL) {
            PNG->sPLT[0] = calloc(1, sizeof(sPLTChunk)); // Realloc?
            PNG->iHDR    = calloc(1, sizeof(iHDRChunk));
            PNG->cHRM    = calloc(1, sizeof(cHRMChunk));
            PNG->gAMA    = calloc(1, sizeof(gAMAChunk));
            PNG->iCCP    = calloc(1, sizeof(iCCPChunk));
            PNG->sBIT    = calloc(1, sizeof(sBITChunk));
            PNG->sRGB    = calloc(1, sizeof(sRGBChunk));
            PNG->PLTE    = calloc(1, sizeof(PLTEChunk));
            PNG->bkGD    = calloc(1, sizeof(bkGDChunk));
            PNG->hIST    = calloc(1, sizeof(hISTChunk));
            PNG->tRNS    = calloc(1, sizeof(tRNSChunk));
            PNG->pHYs    = calloc(1, sizeof(pHYsChunk));
            PNG->oFFs    = calloc(1, sizeof(oFFsChunk));
            PNG->sCAL    = calloc(1, sizeof(sCALChunk));
            PNG->pCAL    = calloc(1, sizeof(pCALChunk));
            PNG->tIMe    = calloc(1, sizeof(tIMeChunk));
            PNG->Text    = calloc(1, sizeof(TextChunk));
            PNG->DAT     = calloc(1, sizeof(DATChunk));
            PNG->sTER    = calloc(1, sizeof(sTERChunk));
            PNG->acTL    = calloc(1, sizeof(acTLChunk));
            PNG->fcTL    = calloc(1, sizeof(fcTLChunk));
        } else {
            Log(Log_DEBUG, __func__, U8("Couldn't allocate PNGOptions"));
        }
    }
    
    HuffmanTree *HuffmanTree_BuildTree(uint16_t *BitLengths, uint64_t NumBitLengths, uint8_t MaxCodeLengthInBits) { // HuffmanTree_makeFromLengths2
        HuffmanTree *Tree = calloc(1, sizeof(HuffmanTree));
        if (BitLengths != NULL && Tree != NULL) {
            // So we do what, exactly?
            // We need both length and distance codes to make a proper Huffman tree, right?
            
            // Create a histogram from the codelengths
            uint16_t *Histogram  = calloc(NumBitLengths, sizeof(uint16_t));
            uint32_t *Histogram2 = calloc(NumBitLengths, sizeof(uint32_t));
            if (Histogram != NULL) {
                for (uint64_t Index = 0ULL; Index < NumBitLengths; Index++) { // Count the frequency of each bitlength
                    Histogram[BitLengths[Index]] += 1; // Histogram = blcount.data
                }
                for (uint64_t Index = 1ULL; Index < NumBitLengths; Index++) {
                    Histogram2[Index] = (Histogram2[Index - 1] + Histogram[Index - 1]) << 1;
                }
                for (uint64_t Index = 0ULL; Index < NumBitLengths; Index++) { // Generate the symbols
                    if (BitLengths[Index] != 0) {
                        Histogram2[BitLengths[Index]] += 1;
                        Tree->HuffmanSymbol            = Histogram2[BitLengths[Index]];
                    }
                }
            } else {
                Log(Log_DEBUG, __func__, U8("Couldn't allocate Histogram"));
            }
        } else if (BitLengths == NULL) {
            Log(Log_DEBUG, __func__, U8("BitLengths array is null"));
        } else if (Tree == NULL) {
            Log(Log_DEBUG, __func__, U8("Couldn't allocate HuffmanTree"));
        }
        return Tree;
    }
    
    void PNGOptions_Deinit(PNGOptions *PNG) {
        if (PNG != NULL) {
            free(PNG->sPLT);
            free(PNG->iHDR);
            free(PNG->cHRM);
            free(PNG->gAMA);
            free(PNG->iCCP);
            free(PNG->sBIT);
            free(PNG->sRGB);
            free(PNG->PLTE);
            free(PNG->bkGD);
            free(PNG->hIST);
            free(PNG->tRNS);
            free(PNG->pHYs);
            free(PNG->oFFs);
            free(PNG->sCAL);
            free(PNG->pCAL);
            free(PNG->tIMe);
            free(PNG->Text);
            free(PNG->sTER);
            free(PNG->acTL);
            free(PNG->fcTL);
            free(PNG->DAT);
            free(PNG);
        } else {
            Log(Log_DEBUG, __func__, U8("Couldn't allocate PNGOptions"));
        }
    }
    
#ifdef __cplusplus
}
#endif
