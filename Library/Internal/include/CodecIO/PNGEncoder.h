#include <stdbool.h>
#include <stdint.h>
#include "../libModernPNG.h"

#pragma once

#ifndef LIBMODERNPNG_ENCODEPNG_H
#define LIBMODERNPNG_ENCODEPNG_H

#ifdef __cplusplus
extern "C" {
#endif
    
    void WriteIHDRChunk(BitOutput *BitO, EncodePNG *Enc);
    
    void WriteACTLChunk(BitOutput *BitO, EncodePNG *Enc);
    
    void WriteFCTLChunk(BitOutput *BitO, EncodePNG *Enc);
    
    void WriteFDATChunk(BitOutput *BitO, EncodePNG *Enc, uint8_t *DeflatedFrameData, uint32_t DeflatedFrameDataSize);
    
    void WriteSTERChunk(BitOutput *BitO, EncodePNG *Enc);
    
    void WriteBKGDChunk(BitOutput *BitO, EncodePNG *Enc);
    
    void WriteCHRMChunk(BitOutput *BitO, EncodePNG *Enc);
    
    void WriteGAMAChunk(BitOutput *BitO, EncodePNG *Enc);
    
    void WriteOFFSChunk(BitOutput *BitO, EncodePNG *Enc);
    
    void WriteICCPChunk(BitOutput *BitO, EncodePNG *Enc);
    
    void WriteSBITChunk(BitOutput *BitO, EncodePNG *Enc);
    
    void WriteSRGBChunk(BitOutput *BitO, EncodePNG *Enc);
    
    void WritePHYSChunk(BitOutput *BitO, EncodePNG *Enc);
    
    void WritePCALChunk(BitOutput *BitO, EncodePNG *Enc);
    
    void WriteSCALChunk(BitOutput *BitO, EncodePNG *Enc);
    
    void PNGEncodeFilterPaeth(EncodePNG *Enc, uint8_t *Line, size_t LineSize);
    
    void PNGEncodeFilterSub(EncodePNG *Enc, uint8_t *Line, size_t NumPixels);
    
    void PNGEncodeAdam7(EncodePNG *Enc, uint8_t ****RawFrame);
    
    void OptimizeAdam7(EncodePNG *Enc, uint8_t ****Image);
    
    /*!
     @abstract       "Optimizes the image, by trying the 5 filters on each line, and keeping the best."
     */
    void OptimizePNG(EncodePNG *Enc, uint8_t ****Image);
    
    void PNGEncodeImage(EncodePNG *Enc, BitOutput *BitO);
    
    
    
#ifdef __cplusplus
}
#endif

#endif /* LIBMODERNPNG_ENCODEPNG_H */
