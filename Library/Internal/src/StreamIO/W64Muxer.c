#include "../../Dependencies/BitIO/libBitIO/include/BitIO.h"

#include "../include/libPCM.h"
#include "../include/PCMTypes.h"
#include "../include/W64Common.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    // To encode W64 i'll need to read the values from the struct and write it to the file
    
    void WriteW64FMT(W64Header *W64, BitBuffer *BitB) {
        WriteBits(BitB, W64->FormatType, 16, false);
        WriteBits(BitB, W64->Channels,   16, false);
        WriteBits(BitB, W64->SampleRate, 32, false);
        WriteBits(BitB, W64->ByteRate,   32, false);
        WriteBits(BitB, W64->BlockAlign, 16, false);
        WriteBits(BitB, W64->BitDepth,   16, false);
    }
    
    void WriteW64Header(W64Header *W64, BitBuffer *BitB) {
        WriteUUID(BitB, RIFF_UUID);
        // Write the size of the file including all header fields
        uint64_t W64Size = (W64->NumSamples * W64->Channels * W64->BitDepth) + W64HeaderSize;
        WriteBits(BitB, SwapEndian64(W64Size), 64, false); // little endian field
        WriteUUID(BitB, WAVE_UUID);
        WriteUUID(BitB, FMT_UUID);
        uint64_t FMTSize = 40;
        WriteBits(BitB, SwapEndian64(FMTSize), 64, false); // little endian
        WriteW64FMT(W64, BitB);
        WriteUUID(BitB, DATA_UUID);
        WriteBits(BitB, SwapEndian64(W64->NumSamples), 64, false); // little endian
    }
    
#ifdef __cplusplus
}
#endif
