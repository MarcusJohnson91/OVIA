#include "../Common/libModernAVC_Types.h"

#pragma once

#ifndef LIBMODERNAVC_ParseNAL_H
#define LIBMODERNAVC_ParseNAL_H

#ifdef __cplusplus
extern "C" {
#endif
    
    void ParseSequenceParameterSetData(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseNALSequenceParameterSetExtended(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseNALSubsetSPS(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseNALSequenceParameterSet(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseNALSequenceParameterSetSVC(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseNALSliceHeader(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseNALSliceData(DecodeAVC *Dec, BitBuffer *BitB, uint8_t Category);
    
    void ParseNALSlicePartitionA(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseNALSlicePartitionB(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseNALSlicePartitionC(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseVideoUsabilityInformation(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSVCVUIExtension(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSPSMVCDExtension(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSPS3DAVCExtension(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseHypotheticalReferenceDecoder(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseNALMVCExtension(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseNALSVCExtension(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseNAL3DAVCExtension(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseNALDepthParameterSet(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseNALSliceNonPartitioned(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseNALFillerData(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseNALPrefixUnit(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseNALAccessUnitDelimiter(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseMVCDVUIParametersExtension(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseMVCVUIParametersExtension(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseNALPictureParameterSet(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseNALPrefixUnitSVC(DecodeAVC *Dec, BitBuffer *BitB);
    
    /* Supplementary EnhancementInformation SEI */
    void ParseSEIBufferingPeriod(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIPictureTiming(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIPanScan(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIFiller(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIRegisteredUserData(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIUnregisteredUserData(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIRecoveryPoint(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIRepetitiveReferencePicture(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEISparePicture(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEISceneInfo(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEISubSequenceInfo(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEISubSequenceLayerProperties(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEISubSequenceProperties(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIFullFrameFreeze(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIFullFrameFreezeRelease(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIFullFrameSnapshot(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIProgressiveRefinementSegmentStart(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIProgressiveRefinementSegmentEnd(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIMotionConstrainedSliceGroupSet(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIFilmGrainCharacteristics(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIDeblockingFilterDisplayPreference(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIStereoVideoInfo(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIPostFilterHint(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIToneMappingInfo(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIScalabilityInfo(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEISubPictureScalableLayer(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEINonRequiredLayerRep(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIPriorityLayerInfo(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEILayersNotPresent(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEILayerDependencyChange(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIScalableNesting(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIBaseLayerTemporalHRD(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIQualityLayerIntegrityCheck(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIRedundantPicProperty(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEITemporalDependencyRepresentationIndex(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEITemporalLevelSwitchingPoint(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIParallelDecodingInfo(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIMVCScalableNesting(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIViewScalabilityInfo(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIMVCSceneInfo(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIMVCAcquisitionInfo(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEINonRequiredViewComponent(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIViewDependencyChange(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIOperationPointNotPresent(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIBaseViewTemporalHRD(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIFramePackingArrangement(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIMVCViewPosition(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIDisplayOrientation(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIDepthRepresentationInformation(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEI3DReferenceDisplayInfo(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIDepthTiming(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIDepthSamplingInfo(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIConstrainedDepthParameterSetID(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIMeteringDisplayColorVolume(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIMVCDScalableNesting(DecodeAVC *Dec, BitBuffer *BitB);
    
    /* Why are these here? */
    void ParseSEIDepthGridPosition(DecodeAVC *Dec, BitBuffer *BitB);
    
    void ParseSEIDepthRepresentationElement(BitBuffer *BitB, uint8_t OutSign, uint8_t OutExp, uint8_t OutMantissa, uint8_t OutManLen);
    
    void SkipSEIReservedMessage(BitBuffer *BitB, size_t SEISize);
    
    void ParseSEIMessage(DecodeAVC *Dec, BitBuffer *BitB);
    
#ifdef __cplusplus
}
#endif

#endif /* LIBMODERNAVC_ParseNAL_H */
