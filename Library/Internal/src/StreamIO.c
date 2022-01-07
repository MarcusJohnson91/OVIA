#include "../../../include/Private/StreamIO/StreamIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

#ifdef    OVIA_StreamIO_AIF
#include "../../../include/Private/StreamIO/AIFStream.h"
#ifdef    OVIA_StreamIO_Mux
StreamIO_RegisterMuxer(AIFMuxer)
#endif /* OVIA_StreamIO_Mux */
#ifdef    OVIA_StreamIO_Demux
StreamIO_RegisterDemuxer(AIFDemuxer)
#endif /* OVIA_StreamIO_Demux */
#endif /* OVIA_StreamIO_AIF */

#ifdef    OVIA_StreamIO_BMP
#include "../../../include/Private/StreamIO/BMPStream.h"
#ifdef    OVIA_StreamIO_Mux
StreamIO_RegisterMuxer(BMPMuxer)
#endif /* OVIA_StreamIO_Mux */
#ifdef    OVIA_StreamIO_Demux
StreamIO_RegisterDemuxer(BMPDemuxer)
#endif /* OVIA_StreamIO_Demux */
#endif /* OVIA_StreamIO_BMP */

#ifdef    OVIA_StreamIO_PNM
#include "../../../include/Private/StreamIO/PNMStream.h"
#ifdef    OVIA_StreamIO_Mux
StreamIO_RegisterMuxer(PNMMuxer)
#endif /* OVIA_StreamIO_Mux */
#ifdef    OVIA_StreamIO_Demux
StreamIO_RegisterDemuxer(PNMDemuxer)
#endif /* OVIA_StreamIO_Demux */
#endif /* OVIA_StreamIO_PNM */

#ifdef    OVIA_StreamIO_RIFF
#include "../../../include/Private/StreamIO/RIFFStream.h"
#ifdef    OVIA_StreamIO_Mux
StreamIO_RegisterMuxer(RIFFMuxer)
#endif /* OVIA_StreamIO_Mux */
#ifdef    OVIA_StreamIO_Demux
StreamIO_RegisterDemuxer(RIFFDemuxer)
#endif /* OVIA_StreamIO_Demux */
#endif /* OVIA_StreamIO_RIFF */

#ifdef    OVIA_StreamIO_OGG
#include "../../../include/Private/StreamIO/OGGStream.h"
#ifdef    OVIA_StreamIO_Mux
StreamIO_RegisterMuxer(OGGMuxer)
#endif /* OVIA_StreamIO_Mux */
#ifdef    OVIA_StreamIO_Demux
StreamIO_RegisterDemuxer(OGGDemuxer)
#endif /* OVIA_StreamIO_Demux */
#endif /* OVIA_StreamIO_OGG */

#ifdef    OVIA_StreamIO_FLAC
#include "../../../include/Private/StreamIO/FLACNativeStream.h"
#ifdef    OVIA_StreamIO_Mux
StreamIO_RegisterMuxer(FLACMuxer)
#endif /* OVIA_StreamIO_Mux */
#ifdef    OVIA_StreamIO_Demux
StreamIO_RegisterDemuxer(FLACDemuxer)
#endif /* OVIA_StreamIO_Demux */
#endif /* OVIA_StreamIO_FLAC */

#ifdef    OVIA_StreamIO_TIFF
#include "../../../include/Private/StreamIO/TIFFStream.h"
#ifdef    OVIA_StreamIO_Mux
StreamIO_RegisterMuxer(TIFFMuxer)
#endif /* OVIA_StreamIO_Mux */
#ifdef    OVIA_StreamIO_Demux
StreamIO_RegisterDemuxer(TIFFDemuxer)
#endif /* OVIA_StreamIO_Demux */
#endif /* OVIA_StreamIO_TIFF */

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif /* Extern C */
