/*!
 @header    libMPEGTS
 @author    Marcus Johnson aka BumbleBritches57
 @copyright 2017, released under the BSD 3 clause license
 @version   0.0
 @brief     This library contains a muxer and demuxer for MPEG2-TS aka H.222 streams
 */

#include "/usr/local/Packages/libBitIO/include/BitIO.h"
#include "libMPEG2StreamTables.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

    extern enum TSPIDTable {
        ProgramAssociationTable         =    0, // Program association table
        ConditionalAccesTable           =    1, // Conditional access table
        TransportStreamDescriptionTable =    2, // Transport stream description table
        IPMPControlInfoTable            =    3, // Intellectual Property Management and Protection
        LastReserved                    = 8191,
    } PIDTable;

    extern enum TSAdaptationFieldTable {
        Reserved                        =    0,
        PayloadOnly                     =    1,
        AdaptationFieldOnly             =    2,
        AdaptationThenPayload           =    3,
    } AdaptationFieldTable;

    extern enum TSStreamIDTypes {
        ProgramStreamMap                =  188,
        PrivateStream1                  =  189,
        PaddingStream                   =  190,
        PrivateStream2                  =  191,
        AudioStream                     =  192, // & 0xC0, the 5 least significant bits are the audio stream number.
        VideoStream                     =  224, // & 0xE0, the 4 least significant bits are the video stream number.
        ECMStream                       =  240,
        EMMStream                       =  241,
        AnnexA_DSMCCStream              =  242, // or 13818-1 Annex A
        Stream13522                     =  243,
        TypeAStream                     =  244,
        TypeBStream                     =  245,
        TypeCStream                     =  246,
        TypeDStream                     =  247,
        TypeEStream                     =  248,
        AncillaryStream                 =  249,
        SLPacketizedStream              =  250,
        FlexMuxStream                   =  251,
        MetadataStream                  =  252,
        ExtendedStreamID                =  253,
        ReservedStream                  =  254,
        ProgramStreamFolder             =  255,
    } TSStreamIDTypes;

    extern enum TSTrickModeTypes {
        FastForward = 0,
        SlowMotion  = 1,
        FreezeFrame = 2,
        FastRewind  = 3,
        SlowRewind  = 4,
    } TSTrickModeTypes;

    extern enum TSFieldID {
        TopFieldOnly    = 0,
        BottomFieldOnly = 1,
        DisplayFullPic  = 2,
    } TSFieldID;

    extern enum CoefficentSelection {
        OnlyDCCoeffsAreNonZero     = 0,
        OnlyFirst3CoeffsAreNonZero = 1,
        OnlyFirst6CoeffsAreNonZero = 2,
        AllCoeffsMayBeNonZero      = 3,
    } CoefficentSelection;
    
    // Lets say I'm got a series of PNG images I want to encode to lossless AVC, with a preexisting FLAC audio track.
    // I could use ModernFLAC to decode the audio to PCM via a "DecodeFLACFrame" function, but I think that's a bit too nitty gritty.
    // Could I instead take and have the user create a program that loops over these frames and passes them to a standalone muxer?
    // This way would allow a LOT more flexability...
    
    typedef struct Packet2Mux Packet2Mux;
    
    /*!
     @abstract     Main muxing function
     */
    void MuxMPEGTransportStream(BitInput *BitO, Packet2Mux *Packet);
    
    void MuxAVC2MPEGTransportStream(BitInput *BitO, Packet2Mux *Packet);
    
    typedef struct TSAdaptationField TSAdaptationField;
    
    typedef struct TransportStreamPacket TransportStreamPacket;
    
    typedef struct PacketizedElementaryStream PacketizedElementaryStream;
    
    typedef struct ProgramAssociatedSection ProgramAssociatedSection;
    
    typedef struct ConditionalAccessSection ConditionalAccessSection;
    
    typedef struct ProgramStream ProgramStream;
    
    typedef struct MPEG2TransportStream {
        TransportStreamPacket      *Packet;
        TSAdaptationField          *Adaptation;
        PacketizedElementaryStream *PES;
        ProgramAssociatedSection   *Program;
        ConditionalAccessSection   *Condition;
    } MPEG2TransportStream;
    
    typedef struct MPEG2ProgramStream {
        ProgramStream              *PS;
        PacketizedElementaryStream *PES;
    } MPEG2ProgramStream;
    
    /*!
     @abstract     Main Demuxing function
     */
    void DemuxMPEGTransportStream(BitInput *BitI, MPEG2TransportStream *Stream);
    
#ifdef __cplusplus
}
#endif
