#include "../include/StreamIO.h" /* Included for our declarations */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

#ifdef    OVIA_StreamIO_AIF
#include "../../Internal/include/StreamIO/AIFStream.h"
#ifdef    OVIA_StreamIO_Mux
StreamIO_RegisterMuxer(AIFMuxer)
#endif /* OVIA_StreamIO_Mux */
#ifdef    OVIA_StreamIO_Demux
StreamIO_RegisterDemuxer(AIFDemuxer)
#endif /* OVIA_StreamIO_Demux */
#endif /* OVIA_StreamIO_AIF */

#ifdef    OVIA_StreamIO_BMP
#include "../../Internal/include/StreamIO/BMPStream.h"
#ifdef    OVIA_StreamIO_Mux
StreamIO_RegisterMuxer(BMPMuxer)
#endif /* OVIA_StreamIO_Mux */
#ifdef    OVIA_StreamIO_Demux
StreamIO_RegisterDemuxer(BMPDemuxer)
#endif /* OVIA_StreamIO_Demux */
#endif /* OVIA_StreamIO_BMP */

#ifdef    OVIA_StreamIO_PNM
#include "../../Internal/include/StreamIO/PNMStream.h"
#ifdef    OVIA_StreamIO_Mux
StreamIO_RegisterMuxer(PNMMuxer)
#endif /* OVIA_StreamIO_Mux */
#ifdef    OVIA_StreamIO_Demux
StreamIO_RegisterDemuxer(PNMDemuxer)
#endif /* OVIA_StreamIO_Demux */
#endif /* OVIA_StreamIO_PNM */

#ifdef    OVIA_StreamIO_W64
#include "../../Internal/include/StreamIO/W64Stream.h"
#ifdef    OVIA_StreamIO_Mux
    StreamIO_RegisterMuxer(W64Muxer)
#endif /* OVIA_StreamIO_Mux */
#ifdef    OVIA_StreamIO_Demux
    StreamIO_RegisterDemuxer(W64Demuxer)
#endif /* OVIA_StreamIO_Demux */
#endif /* OVIA_StreamIO_W64 */

#ifdef    OVIA_StreamIO_WAV
#include "../../Internal/include/StreamIO/WAVStream.h"
#ifdef    OVIA_StreamIO_Mux
StreamIO_RegisterMuxer(RIFFMuxer)
#endif /* OVIA_StreamIO_Mux */
#ifdef    OVIA_StreamIO_Demux
StreamIO_RegisterDemuxer(RIFFDemuxer)
#endif /* OVIA_StreamIO_Demux */
#endif /* OVIA_StreamIO_RIFF */

#ifdef    OVIA_StreamIO_OGG
#include "../../Internal/include/StreamIO/OGGStream.h"
#ifdef    OVIA_StreamIO_Mux
StreamIO_RegisterMuxer(OGGMuxer)
#endif /* OVIA_StreamIO_Mux */
#ifdef    OVIA_StreamIO_Demux
StreamIO_RegisterDemuxer(OGGDemuxer)
#endif /* OVIA_StreamIO_Demux */
#endif /* OVIA_StreamIO_OGG */

#ifdef    OVIA_StreamIO_FLAC
#include "../../Internal/include/StreamIO/FLACStream.h"
#ifdef    OVIA_StreamIO_Mux
StreamIO_RegisterMuxer(FLACMuxer)
#endif /* OVIA_StreamIO_Mux */
#ifdef    OVIA_StreamIO_Demux
StreamIO_RegisterDemuxer(FLACDemuxer)
#endif /* OVIA_StreamIO_Demux */
#endif /* OVIA_StreamIO_FLAC */

#ifdef    OVIA_StreamIO_TIF
#include "../../Internal/include/StreamIO/TIFStream.h"
#ifdef    OVIA_StreamIO_Mux
StreamIO_RegisterMuxer(TIFMuxer)
#endif /* OVIA_StreamIO_Mux */
#ifdef    OVIA_StreamIO_Demux
StreamIO_RegisterDemuxer(TIFDemuxer)
#endif /* OVIA_StreamIO_Demux */
#endif /* OVIA_StreamIO_TIFF */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
