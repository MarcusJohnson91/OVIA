#include "../../include/Private/PNGCommon.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    void *PNGOptions_Init(void) {
        PNGOptions *PNG     = calloc(1, sizeof(PNGOptions));
        if (PNG != NULL) {
            PNG->LZ77Buffer = calloc(32768, sizeof(uint8_t));
            PNG->sPLT[0]    = calloc(1, sizeof(sPLTChunk)); // Realloc?
            PNG->iHDR       = calloc(1, sizeof(iHDRChunk));
            PNG->cHRM       = calloc(1, sizeof(cHRMChunk));
            PNG->gAMA       = calloc(1, sizeof(gAMAChunk));
            PNG->iCCP       = calloc(1, sizeof(iCCPChunk));
            PNG->sBIT       = calloc(1, sizeof(sBITChunk));
            PNG->sRGB       = calloc(1, sizeof(sRGBChunk));
            PNG->PLTE       = calloc(1, sizeof(PLTEChunk));
            PNG->bkGD       = calloc(1, sizeof(bkGDChunk));
            PNG->hIST       = calloc(1, sizeof(hISTChunk));
            PNG->tRNS       = calloc(1, sizeof(tRNSChunk));
            PNG->pHYs       = calloc(1, sizeof(pHYsChunk));
            PNG->oFFs       = calloc(1, sizeof(oFFsChunk));
            PNG->sCAL       = calloc(1, sizeof(sCALChunk));
            PNG->pCAL       = calloc(1, sizeof(pCALChunk));
            PNG->tIMe       = calloc(1, sizeof(tIMeChunk));
            PNG->Text       = calloc(1, sizeof(TextChunk));
            PNG->DAT        = calloc(1, sizeof(DATChunk));
            PNG->sTER       = calloc(1, sizeof(sTERChunk));
            PNG->acTL       = calloc(1, sizeof(acTLChunk));
            PNG->fcTL       = calloc(1, sizeof(fcTLChunk));
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Couldn't allocate PNGOptions"));
        }
        return PNG;
    }
    
    void PNGOptions_Deinit(void *Options) {
        if (Options != NULL) {
            PNGOptions *PNG = Options;
            free(PNG->LZ77Buffer);
            for (uint8_t SPLTChunk = 0; SPLTChunk < PNG->NumSPLTChunks; SPLTChunk++) {
                free(PNG->sPLT[SPLTChunk]);
            }
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
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Couldn't allocate PNGOptions"));
        }
    }
    
    HuffmanTree *HuffmanTree_Init(uint16_t NumSymbols) {
        HuffmanTree *Tree    = calloc(1, sizeof(HuffmanTree));
        if (Tree != NULL) {
            Tree->NumSymbols = NumSymbols;
            Tree->Frequency  = calloc(NumSymbols, sizeof(uint16_t));
            Tree->Symbol     = calloc(NumSymbols, sizeof(uint16_t));
        } else {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Couldn't allocate HuffmanTree"));
        }
        return Tree;
    }
    
    HuffmanTree *PNG_Flate_BuildHuffmanTree(uint16_t *SymbolLengths, uint16_t NumSymbols) {
        HuffmanTree *Tree = HuffmanTree_Init(NumSymbols);
        if (SymbolLengths != NULL) {
            for (uint16_t Symbol = 0; Symbol < NumSymbols; Symbol++) {
                Tree->Frequency[SymbolLengths[Symbol]] += 1;
            }
            
            uint16_t *Offsets = calloc(MaxBitsPerSymbol + 1, sizeof(uint16_t));
            if (Offsets != NULL) {
                for (uint16_t SymbolLength = 1; SymbolLength < MaxBitsPerSymbol; SymbolLength++) {
                    Offsets[SymbolLength + 1] = Offsets[SymbolLength] + Tree->Frequency[SymbolLength];
                }
                
                for (uint16_t Symbol = 0; Symbol < NumSymbols; Symbol++) {
                    if (SymbolLengths[Symbol] != 0) {
                        Tree->Symbol[Offsets[SymbolLengths[Symbol]] + 1] = Symbol;
                    }
                }
                free(Offsets);
            } else {
                Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("Couldn't allocate Offset table"));
            }
        } else if (SymbolLengths == NULL) {
            Log(Severity_DEBUG, UnicodeIOTypes_FunctionName, UTF8String("SymbolLengths is NULL"));
        }
        return Tree;
    }
    
    void HuffmanTree_Deinit(HuffmanTree *Tree) {
        if (Tree != NULL) {
            free(Tree->Frequency);
            free(Tree->Symbol);
        }
        free(Tree);
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
