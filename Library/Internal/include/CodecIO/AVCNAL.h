#include "../../../Dependencies/BitIO/libBitIO/include/BitIO.h"
#include "../libModernAVC.h"

#pragma once

#ifndef LIBMODERNAVC_PARSENAL_H
#define LIBMODERNAVC_PARSENAL_H

#ifdef __cplusplus
extern "C" {
#endif
    
    void ParseSequenceParameterSetData(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseNALSequenceParameterSetExtended(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseNALSubsetSPS(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseNALSequenceParameterSet(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseNALSequenceParameterSetSVC(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseNALSliceHeader(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseNALSliceData(AVCFile *AVC, BitBuffer *BitB, uint8_t Category);
    
    void ParseNALSlicePartitionA(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseNALSlicePartitionB(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseNALSlicePartitionC(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseVideoUsabilityInformation(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSVCVUIExtension(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSPSMVCDExtension(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSPS3DAVCExtension(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseHypotheticalReferenceDecoder(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseNALMVCExtension(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseNALSVCExtension(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseNAL3DAVCExtension(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseNALDepthParameterSet(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseNALSliceNonPartitioned(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseNALFillerData(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseNALPrefixUnit(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseNALAccessUnitDelimiter(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseMVCDVUIParametersExtension(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseMVCVUIParametersExtension(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseNALPictureParameterSet(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseNALPrefixUnitSVC(AVCFile *AVC, BitBuffer *BitB);
    
    /* Supplementary EnhancementInformation SEI */
    void ParseSEIBufferingPeriod(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIPictureTiming(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIPanScan(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIFiller(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIRegisteredUserData(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIUnregisteredUserData(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIRecoveryPoint(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIRepetitiveReferencePicture(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEISparePicture(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEISceneInfo(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEISubSequenceInfo(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEISubSequenceLayerProperties(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEISubSequenceProperties(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIFullFrameFreeze(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIFullFrameFreezeRelease(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIFullFrameSnapshot(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIProgressiveRefinementSegmentStart(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIProgressiveRefinementSegmentEnd(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIMotionConstrainedSliceGroupSet(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIFilmGrainCharacteristics(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIDeblockingFilterDisplayPreference(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIStereoVideoInfo(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIPostFilterHint(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIToneMappingInfo(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIScalabilityInfo(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEISubPictureScalableLayer(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEINonRequiredLayerRep(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIPriorityLayerInfo(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEILayersNotPresent(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEILayerDependencyChange(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIScalableNesting(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIBaseLayerTemporalHRD(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIQualityLayerIntegrityCheck(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIRedundantPicProperty(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEITemporalDependencyRepresentationIndex(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEITemporalLevelSwitchingPoint(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIParallelDecodingInfo(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIMVCScalableNesting(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIViewScalabilityInfo(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIMVCSceneInfo(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIMVCAcquisitionInfo(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEINonRequiredViewComponent(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIViewDependencyChange(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIOperationPointNotPresent(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIBaseViewTemporalHRD(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIFramePackingArrangement(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIMVCViewPosition(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIDisplayOrientation(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIDepthRepresentationInformation(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEI3DReferenceDisplayInfo(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIDepthTiming(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIDepthSamplingInfo(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIConstrainedDepthParameterSetID(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIMeteringDisplayColorVolume(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIMVCDScalableNesting(AVCFile *AVC, BitBuffer *BitB);
    
    /* Why are these here? */
    void ParseSEIDepthGridPosition(AVCFile *AVC, BitBuffer *BitB);
    
    void ParseSEIDepthRepresentationElement(BitBuffer *BitB, uint8_t OutSign, uint8_t OutExp, uint8_t OutMantissa, uint8_t OutManLen);
    
    void SkipSEIReservedMessage(BitBuffer *BitB, size_t SEISize);
    
    void ParseSEIMessage(AVCFile *AVC, BitBuffer *BitB);
    
#ifdef __cplusplus
}
#endif

#endif /* LIBMODERNAVC_PARSENAL_H */
