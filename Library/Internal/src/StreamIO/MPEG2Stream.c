#include "../../include/StreamIO/MPEG2Stream.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    MPEG2TransportStream *InitMPEGTransportStream(void) {
        MPEG2TransportStream *TransportStream                 = calloc(1, sizeof(MPEG2TransportStream));
        TransportStream->Packet                               = calloc(1, sizeof(TransportStreamPacket));
        TransportStream->Adaptation                           = calloc(1, sizeof(TSAdaptationField));
        TransportStream->PES                                  = calloc(1, sizeof(PacketizedElementaryStream));
        TransportStream->Program                              = calloc(1, sizeof(ProgramAssociatedSection));
        TransportStream->Condition                            = calloc(1, sizeof(ConditionalAccessSection));
        return TransportStream;
    }
    
    MPEG2ProgramStream *InitMPEGProgramStream(void) {
        MPEG2ProgramStream *ProgramStream                     = calloc(1, sizeof(MPEG2ProgramStream));
        ProgramStream->PSP                                    = calloc(1, sizeof(ProgramStreamPacket));
        ProgramStream->PES                                    = calloc(1, sizeof(PacketizedElementaryStream));
        return ProgramStream;
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
