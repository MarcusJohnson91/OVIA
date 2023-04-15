cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

# Ok, so new structure: Globals, followed by Options, followed by config-specific variables.

project(OVIA C)

if(DEFINED CMAKE_BUILD_TYPE AND CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Debug")
elseif(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif(DEFINED CMAKE_BUILD_TYPE AND CMAKE_BUILD_TYPE STREQUAL "")

include("${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/FoundationIO/Projects/DetectArchitecture.cmake")
FindTargetArchitecture(TargetArchitecture)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../BUILD/${TargetArchitecture}/${CMAKE_BUILD_TYPE}")
set(CMAKE_OUTPUT_DIRECTORY         "${CMAKE_CURRENT_SOURCE_DIR}/../BUILD/${TargetArchitecture}/${CMAKE_BUILD_TYPE}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../BUILD/${TargetArchitecture}/${CMAKE_BUILD_TYPE}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../BUILD/${TargetArchitecture}/${CMAKE_BUILD_TYPE}")
set(OVIA_Library_Dir               "${CMAKE_CURRENT_SOURCE_DIR}/../Library")
set(OVIA_Resource_Dir              "${CMAKE_CURRENT_SOURCE_DIR}/../Resources")
set(OVIA_StreamIO_Tests_Dir        "${OVIA_Library_Dir}/Internal/src/StreamIO/Tests" CACHE PATH INTERNAL FORCE)
set(OVIA_CodecIO_Tests_Dir         "${OVIA_Library_Dir}/Internal/src/CodecIO/Tests" CACHE PATH INTERNAL FORCE)
set(OVIA_EntropyIO_Tests_Dir       "${OVIA_Library_Dir}/Internal/src/EntropyIO/Tests" CACHE PATH INTERNAL FORCE)
set(OVIA_TagIO_Tests_Dir           "${OVIA_Library_Dir}/Internal/src/TagIO/Tests" CACHE PATH INTERNAL FORCE)
set(OVIA_TransformIO_Tests_Dir     "${OVIA_Library_Dir}/Internal/src/TransformIO/Tests" CACHE PATH INTERNAL FORCE)
set(FoundationIO_Dir               "${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/FoundationIO")

if(CMAKE_HOST_UNIX)
    add_definitions(-D_LARGEFILE_SOURCE)
    add_definitions(-D_LARGEFILE64_SOURCE)
    add_definitions(-D_FILE_OFFSET_BITS=64)
    add_definitions(-D__STDC_WANT_LIB_EXT1__)
    add_definitions(-D__TIMESIZE=64)
elseif(CMAKE_HOST_WIN32)
    add_definitions(-DUNICODE)
    add_definitions(-D_UNICODE)
    add_definitions(-D_ATL_ALLOW_CHAR_UNSIGNED)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif(CMAKE_HOST_UNIX)

set(OVIA_SOURCE
    "${OVIA_Library_Dir}/OVIA.c"
    "${OVIA_Library_Dir}/OVIA/src/CodecIO.c"
    "${OVIA_Library_Dir}/OVIA/src/EntropyIO.c"
    "${OVIA_Library_Dir}/OVIA/src/MediaIO.c"
    "${OVIA_Library_Dir}/OVIA/src/StreamIO.c"
    "${OVIA_Library_Dir}/OVIA/src/TagIO.c"
    "${OVIA_Library_Dir}/Internal/src/CodecIO/FLACCodec.c"
    "${OVIA_Library_Dir}/Internal/src/CodecIO/FLACDecoder.c"
    "${OVIA_Library_Dir}/Internal/src/CodecIO/FLACEncoder.c"
    "${OVIA_Library_Dir}/Internal/src/CodecIO/JPEGCodec.c"
    "${OVIA_Library_Dir}/Internal/src/CodecIO/JPEGDecoder.c"
    "${OVIA_Library_Dir}/Internal/src/CodecIO/JPEGEncoder.c"
    "${OVIA_Library_Dir}/Internal/src/CodecIO/PNGCodec.c"
    "${OVIA_Library_Dir}/Internal/src/CodecIO/PNGDecoder.c"
    "${OVIA_Library_Dir}/Internal/src/CodecIO/PNGEncoder.c"
    "${OVIA_Library_Dir}/Internal/src/EntropyIO/Arithmetic.c"
    "${OVIA_Library_Dir}/Internal/src/EntropyIO/Flate.c"
    "${OVIA_Library_Dir}/Internal/src/EntropyIO/Huffman.c"
    "${OVIA_Library_Dir}/Internal/src/StreamIO/AIFStream.c"
    "${OVIA_Library_Dir}/Internal/src/StreamIO/AIFDemuxer.c"
    "${OVIA_Library_Dir}/Internal/src/StreamIO/AIFMuxer.c"
    "${OVIA_Library_Dir}/Internal/src/StreamIO/BMPStream.c"
    "${OVIA_Library_Dir}/Internal/src/StreamIO/BMPDemuxer.c"
    "${OVIA_Library_Dir}/Internal/src/StreamIO/BMPMuxer.c"
    "${OVIA_Library_Dir}/Internal/src/StreamIO/PNMStream.c"
    "${OVIA_Library_Dir}/Internal/src/StreamIO/PNMDemuxer.c"
    "${OVIA_Library_Dir}/Internal/src/StreamIO/PNMMuxer.c"
    "${OVIA_Library_Dir}/Internal/src/StreamIO/W64Stream.c"
    "${OVIA_Library_Dir}/Internal/src/StreamIO/W64Demuxer.c"
    "${OVIA_Library_Dir}/Internal/src/StreamIO/W64Muxer.c"
    "${OVIA_Library_Dir}/Internal/src/StreamIO/TIFStream.c"
    "${OVIA_Library_Dir}/Internal/src/StreamIO/TIFDemuxer.c"
    "${OVIA_Library_Dir}/Internal/src/StreamIO/TIFMuxer.c"
    "${OVIA_Library_Dir}/Internal/src/TagIO/AIFTags.c"
    "${OVIA_Library_Dir}/Internal/src/TagIO/APETags.c"
    "${OVIA_Library_Dir}/Internal/src/TagIO/ID3Tags.c"
    "${OVIA_Library_Dir}/Internal/src/TagIO/RIFFTags.c"
    "${OVIA_Library_Dir}/Internal/src/TagIO/VorbisTags.c"
    "${OVIA_Library_Dir}/Internal/src/TransformIO/RCT.c"
    "${OVIA_Library_Dir}/Internal/src/TransformIO/YCoCgR.c"
    "${LIVC_SOURCE}"
    "${OVIA_Library_Dir}/Internal/src/CodecIO/AVCCodec.c"
    "${OVIA_Library_Dir}/Internal/src/CodecIO/AVCDecoder.c"
    "${OVIA_Library_Dir}/Internal/src/CodecIO/AVCEncoder.c"
)

set(LIVCPath "")
include("${OVIA_Library_Dir}/External/LIVC/" OPTIONAL RESULT_VARIABLE LIVCPath)

if(EXISTS "${LIVCPath}/LIVC.cmake")
include("${LIVCPath}/LIVC.cmake")
set(OVIA_SOURCE "${OVIA_SOURCE} ${LIVC_SOURCE}")
endif(EXISTS "${LIVCPath}/LIVC.cmake")

include("${FoundationIO_Dir}/Projects/GetVersion.cmake")
GetVersionString("${OVIA_Library_Dir}/OVIA.h" OVIA_VERSION)

configure_file("${OVIA_Resource_Dir}/OVIA.pc.in" "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Resources/OVIA.pc")
configure_file("${OVIA_Resource_Dir}/OVIA.rc.in" "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Resources/OVIA.rc")

option(BUILD_AS_SHARED_LIBRARY "Should OVIA be compiled as a dynamic library? (.dylib/.dll/.so)" ON)
option(BUILD_CODECIO_AVC "Should OVIA be compiled with support for AVC/H.264?" ON)
option(BUILD_CODECIO_JPG "Should OVIA be compiled with support for JPEG?" ON)
option(BUILD_CODECIO_PNG "Should OVIA be compiled with support for PNG/MNG/JNG (JNG requires JPG)?" ON)
option(BUILD_CODECIO_FLAC "Should OVIA be compiled with support for FLAC audio?" ON)
option(BUILD_STREAMIO_AIF "Should OVIA be compiled with support for AIFF audio?" ON)
option(BUILD_STREAMIO_BMP "Should OVIA be compiled with support for BMP images?" ON)
option(BUILD_STREAMIO_WAV "Should OVIA be compiled with support for WAV audio?" ON)
option(BUILD_STREAMIO_W64 "Should OVIA be compiled with support for W64 audio?" ON)
option(BUILD_STREAMIO_MPEG2 "Should OVIA be compiled with support for MPEG2 Transport/Program Stream?" ON)
option(BUILD_STREAMIO_PNM "Should OVIA be compiled with support for PNM images?" ON)
option(BUILD_STREAMIO_TIF "Should OVIA be compiled with support for TIFF (DNG) images?" ON)
option(BUILD_STREAMIO_MKV "Should OVIA be compiled with support for MKV (mkv/mk3d/mka/mks/webm)?" ON)
option(BUILD_STREAMIO_OGG "Should OVIA be compiled with support for OGG (FLAC/opus/ogg/oga/ogv)?" ON)


if(BUILD_AS_SHARED_LIBRARY MATCHES ON)
    add_library(OVIA SHARED ${OVIA_SOURCE})
elseif(BUILD_AS_SHARED_LIBRARY MATCHES OFF)
    add_library(OVIA STATIC ${OVIA_SOURCE})
endif(BUILD_AS_SHARED_LIBRARY MATCHES ON)

IF(BUILD_CODECIO_AVC)
    ADD_DEFINITIONS(-DOVIA_CodecIO_FLAC)
    ADD_DEFINITIONS(-DOVIA_CodecIO_Encode)
    ADD_DEFINITIONS(-DOVIA_CodecIO_Decode)
ENDIF(BUILD_CODECIO_AVC)

IF(BUILD_CODECIO_JPG)
    ADD_DEFINITIONS(-DOVIA_CodecIO_JPEG)
    ADD_DEFINITIONS(-DOVIA_CodecIO_Encode)
    ADD_DEFINITIONS(-DOVIA_CodecIO_Decode)
ENDIF(BUILD_CODECIO_JPG)

IF(BUILD_CODECIO_PNG)
    ADD_DEFINITIONS(-DOVIA_CodecIO_PNG)
    ADD_DEFINITIONS(-DOVIA_CodecIO_Encode)
    ADD_DEFINITIONS(-DOVIA_CodecIO_Decode)
ENDIF(BUILD_CODECIO_PNG)

IF(BUILD_CODECIO_FLAC)
    ADD_DEFINITIONS(-DOVIA_CodecIO_FLAC)
    ADD_DEFINITIONS(-DOVIA_StreamIO_FLAC)
    ADD_DEFINITIONS(-DOVIA_CodecIO_Encode)
    ADD_DEFINITIONS(-DOVIA_CodecIO_Decode)
ENDIF(BUILD_CODECIO_FLAC)

IF(BUILD_STREAMIO_AIF)
    ADD_DEFINITIONS(-DOVIA_StreamIO_AIF)
    ADD_DEFINITIONS(-DOVIA_CodecIO_Encode)
    ADD_DEFINITIONS(-DOVIA_CodecIO_Decode)
ENDIF(BUILD_STREAMIO_AIF)

IF(BUILD_STREAMIO_BMP)
    ADD_DEFINITIONS(-DOVIA_StreamIO_BMP)
    ADD_DEFINITIONS(-DOVIA_CodecIO_Encode)
    ADD_DEFINITIONS(-DOVIA_CodecIO_Decode)
ENDIF(BUILD_STREAMIO_BMP)

IF(BUILD_STREAMIO_WAV)
    ADD_DEFINITIONS(-DOVIA_StreamIO_WAV)
    ADD_DEFINITIONS(-DOVIA_CodecIO_Encode)
    ADD_DEFINITIONS(-DOVIA_CodecIO_Decode)
ENDIF(BUILD_STREAMIO_WAV)

IF(BUILD_STREAMIO_W64)
    ADD_DEFINITIONS(-DOVIA_StreamIO_W64)
    ADD_DEFINITIONS(-DOVIA_CodecIO_Encode)
    ADD_DEFINITIONS(-DOVIA_CodecIO_Decode)
ENDIF(BUILD_STREAMIO_W64)

IF(BUILD_STREAMIO_MKV)
    ADD_DEFINITIONS(-DOVIA_StreamIO_MKV)
    ADD_DEFINITIONS(-DOVIA_CodecIO_Encode)
    ADD_DEFINITIONS(-DOVIA_CodecIO_Decode)
ENDIF(BUILD_STREAMIO_MKV)

IF(BUILD_STREAMIO_MPEG2)
    ADD_DEFINITIONS(-DOVIA_StreamIO_MPEG2)
    ADD_DEFINITIONS(-DOVIA_CodecIO_Encode)
    ADD_DEFINITIONS(-DOVIA_CodecIO_Decode)
ENDIF(BUILD_STREAMIO_MPEG2)

IF(BUILD_STREAMIO_OGG)
    ADD_DEFINITIONS(-DOVIA_StreamIO_OGG)
    ADD_DEFINITIONS(-DOVIA_CodecIO_Encode)
    ADD_DEFINITIONS(-DOVIA_CodecIO_Decode)
ENDIF(BUILD_STREAMIO_OGG)

IF(BUILD_STREAMIO_PNM)
    ADD_DEFINITIONS(-DOVIA_StreamIO_PNM)
    ADD_DEFINITIONS(-DOVIA_CodecIO_Encode)
    ADD_DEFINITIONS(-DOVIA_CodecIO_Decode)
ENDIF(BUILD_STREAMIO_PNM)

IF(BUILD_STREAMIO_TIF)
    ADD_DEFINITIONS(-DOVIA_StreamIO_TIFF)
    ADD_DEFINITIONS(-DOVIA_CodecIO_Encode)
    ADD_DEFINITIONS(-DOVIA_CodecIO_Decode)
ENDIF(BUILD_STREAMIO_TIF)

set_target_properties(OVIA PROPERTIES PREFIX "")

target_include_directories(OVIA PUBLIC "${OVIA_Library_Dir}/include")

if(NOT DEFINED CMAKE_INSTALL_PREFIX AND CMAKE_HOST_UNIX)
    set(CMAKE_INSTALL_PREFIX "/usr/local")
elseif(NOT DEFINED CMAKE_INSTALL_PREFIX AND CMAKE_HOST_WIN32)
    set(CMAKE_INSTALL_PREFIX "%ProgramFiles%\OVIA")
endif(NOT DEFINED CMAKE_INSTALL_PREFIX AND CMAKE_HOST_UNIX)

install(TARGETS OVIA                          DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES  ${OVIA_PUBLIC_HEADERS}         DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
install(FILES "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Resources/OVIA.pc" DESTINATION "${CMAKE_INSTALL_PREFIX}/share/pkgconfig")

if(NOT DEFINED CMAKE_BUILD_TYPE OR (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RELWITHDEBINFO"))
include(CTest)
endif(NOT DEFINED CMAKE_BUILD_TYPE OR (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RELWITHDEBINFO"))

if(BUILD_TESTING)
    enable_testing()
    include("${FoundationIO_Dir}/Projects/RegisterTests.cmake")

    add_custom_target(OVIA_Tests Test DEPENDS ${OVIA_TEST_TARGETS})

    RegisterTest("${OVIA_CodecIO_Tests_Dir}" AVCCodec OVIA)
    RegisterTest("${OVIA_CodecIO_Tests_Dir}" FLACCodec OVIA)
    RegisterTest("${OVIA_CodecIO_Tests_Dir}" JPEGCodec OVIA)
    RegisterTest("${OVIA_CodecIO_Tests_Dir}" PNGCodec OVIA)

    RegisterTest("${OVIA_EntropyIO_Tests_Dir}" Arithmetic OVIA)
    RegisterTest("${OVIA_EntropyIO_Tests_Dir}" Huffman OVIA)
    RegisterTest("${OVIA_EntropyIO_Tests_Dir}" Flate OVIA)

    RegisterTest("${OVIA_StreamIO_Tests_Dir}" AIFStream OVIA)
    RegisterTest("${OVIA_StreamIO_Tests_Dir}" BMPStream OVIA)
    RegisterTest("${OVIA_StreamIO_Tests_Dir}" MPEG2Stream OVIA)
    RegisterTest("${OVIA_StreamIO_Tests_Dir}" PNMStream OVIA)
    RegisterTest("${OVIA_StreamIO_Tests_Dir}" TIFStream OVIA)
    RegisterTest("${OVIA_StreamIO_Tests_Dir}" W64Stream OVIA)
    RegisterTest("${OVIA_StreamIO_Tests_Dir}" WAVStream OVIA)

    RegisterTest("${OVIA_TagIO_Tests_Dir}" AIFTags OVIA)
    RegisterTest("${OVIA_TagIO_Tests_Dir}" APETags OVIA)
    RegisterTest("${OVIA_TagIO_Tests_Dir}" ID3Tags OVIA)
    RegisterTest("${OVIA_TagIO_Tests_Dir}" RIFFTags OVIA)
    RegisterTest("${OVIA_TagIO_Tests_Dir}" VorbisTags OVIA)

    RegisterTest("${OVIA_TransformIO_Tests_Dir}" RCT OVIA)
    RegisterTest("${OVIA_TransformIO_Tests_Dir}" YCoCgR OVIA)
endif(BUILD_TESTING)

install(TARGETS OVIA DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES ${OVIA_PUBLICHEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

add_subdirectory("${FoundationIO_Dir}/Projects" "${CMAKE_OUTPUT_DIRECTORY}")

set_target_properties(OVIA PROPERTIES PREFIX "")

target_link_libraries(OVIA PUBLIC FoundationIO)

include("${FoundationIO_Dir}/Projects/BuildSettings.cmake")
