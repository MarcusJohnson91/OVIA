#include "../../include/StreamIO/MPEG2Stream.h"

#include "../../../../Dependencies/FoundationIO/Library/include/AssertIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/BufferIO.h"
#include "../../../../Dependencies/FoundationIO/Library/include/MathIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
    MPEG2Options *MPEG2Options_Init(void) {
        MPEG2Options *Options                         = calloc(1, sizeof(MPEG2Options));
        Options->Packet                               = calloc(1, sizeof(TransportStreamPacket));
        Options->Adaptation                           = calloc(1, sizeof(TSAdaptationField));
        Options->PES                                  = calloc(1, sizeof(PacketizedElementaryStream));
        Options->Program                              = calloc(1, sizeof(ProgramAssociatedSection));
        Options->Condition                            = calloc(1, sizeof(ConditionalAccessSection));
        Options->PSP                                  = calloc(1, sizeof(ProgramStreamPacket));
        return Options;
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
