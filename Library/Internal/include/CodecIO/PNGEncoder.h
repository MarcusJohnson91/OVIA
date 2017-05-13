#include "../libModernPNG.h"

#pragma once

#ifndef LIBMODERNPNG_ENCODEPNG_H
#define LIBMODERNPNG_ENCODEPNG_H

#ifdef __cplusplus
extern "C" {
#endif
    
    void WriteIHDRChunk(BitBuffer *BitB, EncodePNG *Enc);
    
    void WriteACTLChunk(BitBuffer *BitB, EncodePNG *Enc);
    
    void WriteFCTLChunk(BitBuffer *BitB, EncodePNG *Enc);
    
    void WriteFDATChunk(BitBuffer *BitB, EncodePNG *Enc, uint8_t *DeflatedFrameData, uint32_t DeflatedFrameDataSize);
    
    void WriteSTERChunk(BitBuffer *BitB, EncodePNG *Enc);
    
    void WriteBKGDChunk(BitBuffer *BitB, EncodePNG *Enc);
    
    void WriteCHRMChunk(BitBuffer *BitB, EncodePNG *Enc);
    
    void WriteGAMAChunk(BitBuffer *BitB, EncodePNG *Enc);
    
    void WriteOFFSChunk(BitBuffer *BitB, EncodePNG *Enc);
    
    void WriteICCPChunk(BitBuffer *BitB, EncodePNG *Enc);
    
    void WriteSBITChunk(BitBuffer *BitB, EncodePNG *Enc);
    
    void WriteSRGBChunk(BitBuffer *BitB, EncodePNG *Enc);
    
    void WritePHYSChunk(BitBuffer *BitB, EncodePNG *Enc);
    
    void WritePCALChunk(BitBuffer *BitB, EncodePNG *Enc);
    
    void WriteSCALChunk(BitBuffer *BitB, EncodePNG *Enc);
    
    void PNGEncodeFilterPaeth(EncodePNG *Enc, uint8_t *Line, size_t LineSize);
    
    void PNGEncodeFilterSub(EncodePNG *Enc, uint8_t *Line, size_t NumPixels);
    
    void PNGEncodeAdam7(EncodePNG *Enc, uint8_t ****RawFrame);
    
    void OptimizeAdam7(EncodePNG *Enc, uint8_t ****Image);
    
    /*!
     @abstract       "Optimizes the image, by trying the 5 filters on each line, and keeping the best."
     */
    void OptimizePNG(EncodePNG *Enc, uint8_t ****Image);
    
    void PNGEncodeImage(EncodePNG *Enc, BitBuffer *BitB);
    
    
    
#ifdef __cplusplus
}
#endif

#endif /* LIBMODERNPNG_ENCODEPNG_H */
