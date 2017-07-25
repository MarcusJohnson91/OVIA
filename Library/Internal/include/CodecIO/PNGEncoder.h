#include "../libModernPNG.h"

#pragma once

#ifndef LIBMODERNPNG_ENCODEPNG_H
#define LIBMODERNPNG_ENCODEPNG_H

#ifdef __cplusplus
extern "C" {
#endif
    
    void WriteIHDRChunk(EncodePNG *Enc, BitBuffer *OutputPNG);
    
    void WriteACTLChunk(EncodePNG *Enc, BitBuffer *OutputPNG);
    
    void WriteFCTLChunk(EncodePNG *Enc, BitBuffer *OutputPNG);
    
    void WriteFDATChunk(EncodePNG *Enc, BitBuffer *OutputPNG, uint8_t *DeflatedFrameData, uint32_t DeflatedFrameDataSize);
    
    void WriteSTERChunk(EncodePNG *Enc, BitBuffer *OutputPNG);
    
    void WriteBKGDChunk(EncodePNG *Enc, BitBuffer *OutputPNG);
    
    void WriteCHRMChunk(EncodePNG *Enc, BitBuffer *OutputPNG);
    
    void WriteGAMAChunk(EncodePNG *Enc, BitBuffer *OutputPNG);
    
    void WriteOFFSChunk(EncodePNG *Enc, BitBuffer *OutputPNG);
    
    void WriteICCPChunk(EncodePNG *Enc, BitBuffer *OutputPNG);
    
    void WriteSBITChunk(EncodePNG *Enc, BitBuffer *OutputPNG);
    
    void WriteSRGBChunk(EncodePNG *Enc, BitBuffer *OutputPNG);
    
    void WritePHYSChunk(EncodePNG *Enc, BitBuffer *OutputPNG);
    
    void WritePCALChunk(EncodePNG *Enc, BitBuffer *OutputPNG);
    
    void WriteSCALChunk(EncodePNG *Enc, BitBuffer *OutputPNG);
    
    void PNGEncodeFilterPaeth(EncodePNG *Enc, uint8_t *Line, size_t LineSize);
    
    void PNGEncodeFilterSub(EncodePNG *Enc, uint8_t *Line, size_t NumPixels);
    
    void PNGEncodeAdam7(EncodePNG *Enc, uint8_t ****RawFrame);
    
    void OptimizeAdam7(EncodePNG *Enc, uint8_t ****Image);
    
    /*!
     @abstract       "Optimizes the image, by trying the 5 filters on each line, and keeping the best."
     */
    void OptimizePNG(EncodePNG *Enc, uint8_t ****Image);
    
    void PNGEncodeImage(EncodePNG *Enc, BitBuffer *OutputPNG);
    
    
    
#ifdef __cplusplus
}
#endif

#endif /* LIBMODERNPNG_ENCODEPNG_H */
