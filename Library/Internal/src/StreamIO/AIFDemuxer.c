#include "../../include/libPCM.h"
#include "../../include/Private/libPCMTypes.h"
#include "../../include/Private/Audio/AIFCommon.h"
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif
    
    /*
     Like WAV, Chunks are padded if their size is not even, and the ChunkSize field contains the real size without the padding
     the ChunkSize field does NOT include the ChunkID or ChunkSize fields.
     */
    
    void AIFParseCOMMChunk(PCMFile *PCM, BitBuffer *BitB) {
        PCM->AUD->NumChannels = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
        PCM->AUD->NumSamples  = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32); // A SampleFrame is simply a single sample from all channels.
        PCM->AUD->BitDepth    = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 16);
        BitBufferSkip(BitB, 16); // Skip the Exponent
        PCM->AUD->SampleRate  = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 64);
    }
    
    void AIFParseNameChunk(PCMFile *PCM, BitBuffer *BitB) {
        uint32_t AIFFNameSize           = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32);
        char *SongTitleTagString        = calloc(1, AIFFNameSize);
        for (uint32_t TagByte = 0UL; TagByte < AIFFNameSize; TagByte++) {
            SongTitleTagString[TagByte] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
        }
        PCM->AUD->Meta->SongTitleTag    = SongTitleTagString;
    }
    
    void AIFParseAuthorChunk(PCMFile *PCM, BitBuffer *BitB) {
        uint32_t AIFFAuthSize           = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32);
        char *AuthorString              = calloc(1, AIFFAuthSize);
        for (uint32_t TagByte = 0UL; TagByte < AIFFAuthSize; TagByte++) {
            AuthorString[TagByte] = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
        }
        PCM->AUD->Meta->ArtistTag       = AuthorString;
    }
    
    void AIFParseAnnotationChunk(PCMFile *PCM, BitBuffer *BitB) {
        uint32_t AIFFAnnoSize           = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32);
        char *AnnotationString          = calloc(1, AIFFAnnoSize);
        for (uint32_t TagByte = 0UL; TagByte < AIFFAnnoSize; TagByte++) {
            AnnotationString[TagByte]   = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 8);
        }
        PCM->AUD->Meta->NumANNOChunks  += 1;
        PCM->AUD->Meta->AnnoChunks[PCM->AUD->Meta->NumANNOChunks - 1] = AnnotationString;
    }
    
    void AIFParseMetadata(PCMFile *PCM, BitBuffer *BitB) {
        PCM->AUD->FileSize                              = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32);
        uint32_t AIFFChunkID                            = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32);
        if (AIFFChunkID != AIF_AIFF || AIFFChunkID != AIF_AIFC) {
            BitIOLog(LOG_ERROR, "libPCM", "AIFParseMetadata", "Invalid ChunkID %s, It should be AIFF or AIFC", AIFFChunkID);
        } else {
            uint32_t AIFFSubChunkID                     = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32);
            uint32_t AIFFSubChunkIDSize                 = ReadBits(BitIOMSByte, BitIOLSBit, BitB, 32);
            if (AIFFSubChunkID == AIF_COMM) {
                AIFParseCOMMChunk(PCM, BitB);
                if (IsOdd(AIFFSubChunkIDSize) == Yes) { // Skip the IFF container padding byte
                    BitBufferSkip(BitB, 8);
                }
            } else if (AIFFSubChunkID == AIF_ID3) {
                BitBufferSkip(BitB, Bytes2Bits(AIFFSubChunkIDSize));
                if (IsOdd(AIFFSubChunkIDSize) == Yes) { // Skip the IFF container padding byte
                    BitBufferSkip(BitB, 8);
                }
            } else if (AIFFSubChunkID == AIF_NAME) {
                AIFParseNameChunk(PCM, BitB);
                if (IsOdd(AIFFSubChunkIDSize) == Yes) { // Skip the IFF container padding byte
                    BitBufferSkip(BitB, 8);
                }
            } else if (AIFFSubChunkID == AIF_AUTH) {
                AIFParseAuthorChunk(PCM, BitB);
                if (IsOdd(AIFFSubChunkIDSize) == Yes) { // Skip the IFF container padding byte
                    BitBufferSkip(BitB, 8);
                }
            } else if (AIFFSubChunkID == AIF_MARK) {
                BitBufferSkip(BitB, Bytes2Bits(AIFFSubChunkIDSize));
                if (IsOdd(AIFFSubChunkIDSize) == Yes) { // Skip the IFF container padding byte
                    BitBufferSkip(BitB, 8);
                }
            } else if (AIFFSubChunkID == AIF_INST) {
                BitBufferSkip(BitB, Bytes2Bits(AIFFSubChunkIDSize));
                if (IsOdd(AIFFSubChunkIDSize) == Yes) { // Skip the IFF container padding byte
                    BitBufferSkip(BitB, 8);
                }
            } else if (AIFFSubChunkID == AIF_MIDI) {
                BitBufferSkip(BitB, Bytes2Bits(AIFFSubChunkIDSize));
                if (IsOdd(AIFFSubChunkIDSize) == Yes) { // Skip the IFF container padding byte
                    BitBufferSkip(BitB, 8);
                }
            } else if (AIFFSubChunkID == AIF_AESD) {
                BitBufferSkip(BitB, Bytes2Bits(AIFFSubChunkIDSize));
                if (IsOdd(AIFFSubChunkIDSize) == Yes) { // Skip the IFF container padding byte
                    BitBufferSkip(BitB, 8);
                }
            } else if (AIFFSubChunkID == AIF_AAPL) {
                BitBufferSkip(BitB, Bytes2Bits(AIFFSubChunkIDSize));
                if (IsOdd(AIFFSubChunkIDSize) == Yes) { // Skip the IFF container padding byte
                    BitBufferSkip(BitB, 8);
                }
            } else if (AIFFSubChunkID == AIF_COMT) {
                BitBufferSkip(BitB, Bytes2Bits(AIFFSubChunkIDSize));
                if (IsOdd(AIFFSubChunkIDSize) == Yes) { // Skip the IFF container padding byte
                    BitBufferSkip(BitB, 8);
                }
            }
            
            
            
            
            else if (AIFFSubChunkID == AIF_SSND) {
                /*
                 How are we gonna skip the audio data in order to make sure we read all the metadata, then skip back?
                 */
                if (IsOdd(AIFFSubChunkIDSize) == Yes) { // Skip the IFF container padding byte
                    BitBufferSkip(BitB, 8);
                }
            }
        }
    }
    
    
    
    uint32_t **AIFExtractSamples(PCMFile *PCM, BitBuffer *BitB, uint64_t NumSamples2Extract) {
        uint64_t   ExtractedSampleSize = NumSamples2Extract * PCM->AUD->NumChannels * PCM->AUD->BitDepth;
        uint32_t **ExtractedSamples    = calloc(1, NumSamples2Extract);
        if (ExtractedSamples == NULL) {
            BitIOLog(LOG_ERROR, "libPCM", "AIFExtractSamples", "Not enough memory to allocate a buffer for the extracted samples, %d", ExtractedSampleSize);
        } else {
            for (uint16_t Channel = 0; Channel < PCM->AUD->NumChannels; Channel++) {
                for (uint32_t Sample = 0UL; Sample < NumSamples2Extract; Sample++) {
                    ExtractedSamples[Channel][Sample] = ReadBits(BitIOMSByte, BitIOMSBit, BitB, PCM->AUD->BitDepth);
                }
            }
        }
        return ExtractedSamples;
        // Now skip padding when you extract samples, and block alignment too.
    }
    
#ifdef __cplusplus
}
#endif
