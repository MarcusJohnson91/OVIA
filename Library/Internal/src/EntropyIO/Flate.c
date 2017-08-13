#include "../../include/Private/Common/libModernPNGFlate.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    void EncodeLZ77(const BitBuffer *RawBuffer, const BitBuffer *LZ77Buffer, const uint64_t WindowSize, const uint64_t DistanceLength, const uint64_t SymbolSize) {
        // The dictionary is simply the current buffer, at the current buffer position -(WindowSize / 2) +(WindowSize / 2)
        // So, the first thing you write is the distance from the cursor to the previous string.
        // Then you write the length of the largest match you can find.
        // Then write the next byte in the stream.
        // Then move the cursor the length of the longest match + 1
        // When we're at the start of the match, simply record the byte and the length of the match (1).
        
        
        // We need to have a BitBuffer to read from, and a BitBuffer to write to.
        if (RawBuffer == NULL) {
            Log(LOG_ERR, "libBitIO", "EncodeLZ77", "The pointer to the raw buffer is NULL");
        } else if (LZ77Buffer == NULL) {
            Log(LOG_ERR, "libBitIO", "EncodeLZ77", "The pointer to the LZ77 buffer is NULL");
        } else {
            uint64_t WindowBits   = NumBits2ReadSymbols(WindowSize);
            uint64_t DistanceBits = NumBits2ReadSymbols(DistanceLength);
            
            uint64_t CurrentSymbol = 0;
            uint64_t PreviousSymbol = 0;
            
            while (GetBitBufferPosition(RawBuffer) != GetBitBufferSize(RawBuffer)) {
                CurrentSymbol = ReadBits(RawBuffer, SymbolSize, true);
                if (CurrentSymbol == PreviousSymbol) {
                    // find the largest string of symbols that matches the current one
                }
            }
            
            
            
            /* ------------OLD SHIT----------------
             
             // Now, we just need to read the RawBuffer (which must contain ALL the data to be encoded) with ReadBits(SymbolSize)
             // then start writing LZ77Buffer with our discoveries.
             // So, loop over RawBuffer, if RawByte == 0, just code the longest string you can, or the first 3 bytes (if they're all different)
             for (uint64_t RawByte = 0; RawByte < RawBuffer->BitsUnavailable; RawByte++) {
             if (RawByte == 0) {
             if (RawBuffer->Buffer[RawByte] == RawBuffer->Buffer[RawByte + 1] || RawBuffer->Buffer[RawByte + 1] == RawBuffer->Buffer[RawByte + 2]) {
             
             }
             }
             }
             */
        }
    }
    
#ifdef __cplusplus
}
#endif
