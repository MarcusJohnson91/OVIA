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
    "${OVIA_Library_Dir}/Internal/src/CodecIO.c"
    "${OVIA_Library_Dir}/Internal/src/EntropyIO.c"
    "${OVIA_Library_Dir}/Internal/src/MediaIO.c"
    "${OVIA_Library_Dir}/Internal/src/StreamIO.c"
    "${OVIA_Library_Dir}/Internal/src/TagIO.c"
    "${OVIA_Library_Dir}/Internal/src/CodecIO/AVCCodec.c"
    "${OVIA_Library_Dir}/Internal/src/CodecIO/AVCDecoder.c"
    "${OVIA_Library_Dir}/Internal/src/CodecIO/AVCEncoder.c"
    "${OVIA_Library_Dir}/Internal/src/CodecIO/DNGCodec.c"
    "${OVIA_Library_Dir}/Internal/src/CodecIO/DNGDecoder.c"
    "${OVIA_Library_Dir}/Internal/src/CodecIO/DNGEncoder.c"
    "${OVIA_Library_Dir}/Internal/src/CodecIO/FLACCodec.c"
    "${OVIA_Library_Dir}/Internal/src/CodecIO/FLACDecoder.c"
    "${OVIA_Library_Dir}/Internal/src/CodecIO/FLACEncoder.c"
    "${OVIA_Library_Dir}/Internal/src/CodecIO/JPEGCodec.c"
    "${OVIA_Library_Dir}/Internal/src/CodecIO/JPEGDecoder.c"
    "${OVIA_Library_Dir}/Internal/src/CodecIO/JPEGEncoder.c"
    "${OVIA_Library_Dir}/Internal/src/CodecIO/PNGCodec.c"
    "${OVIA_Library_Dir}/Internal/src/CodecIO/PNGDecoder.c"
    "${OVIA_Library_Dir}/Internal/src/CodecIO/PNGEncoder.c"
    "${OVIA_Library_Dir}/Internal/src/CodecIO/DNGCodec.c"
    "${OVIA_Library_Dir}/Internal/src/CodecIO/DNGDecoder.c"
    "${OVIA_Library_Dir}/Internal/src/CodecIO/DNGEncoder.c"
    "${OVIA_Library_Dir}/Internal/src/EntropyIO/Arithmetic.c"
    "${OVIA_Library_Dir}/Internal/src/EntropyIO/Flate.c"
    "${OVIA_Library_Dir}/Internal/src/EntropyIO/Huffman.c"
    "${OVIA_Library_Dir}/Internal/src/EntropyIO/Rice.c"
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
    "${OVIA_Library_Dir}/Internal/src/TagIO/VorbisTags.c"
    "${OVIA_Library_Dir}/Internal/src/TagIO/WAVTags.c"
    "${OVIA_Library_Dir}/Internal/src/TransformIO/RCT.c"
    "${OVIA_Library_Dir}/Internal/src/TransformIO/YCoCgR.c"
    "${LIVC_SOURCE}"
)

set(LIVCPath "")
include("${OVIA_Library_Dir}/External/LIVC/LIVC.cmake" OPTIONAL RESULT_VARIABLE LIVCPath)

include(${FoundationIO_Dir}/Projects/GetVersion.cmake)
GetVersionString("${OVIA_Library_Dir}/OVIA.h" OVIA_VERSION)

configure_file("${OVIA_Resource_Dir}/OVIA.pc.in" "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Resources/OVIA.pc")
configure_file("${OVIA_Resource_Dir}/OVIA.rc.in" "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Resources/OVIA.rc")

option(BUILD_AS_SHARED_LIBRARY "Should OVIA be compiled as a dynamic library? (.dylib/.dll/.so)" ON)

if(BUILD_AS_SHARED_LIBRARY MATCHES ON)
    add_library(OVIA SHARED ${OVIA_SOURCE})
elseif(BUILD_AS_SHARED_LIBRARY MATCHES OFF)
    add_library(OVIA STATIC ${OVIA_SOURCE})
endif(BUILD_AS_SHARED_LIBRARY MATCHES ON)

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

if(DEFINED CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RELWITHDEBINFO"))
include(CTest)
endif(DEFINED CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RELWITHDEBINFO"))

if(BUILD_TESTING)
    enable_testing()
    include("${FoundationIO_Dir}/Projects/RegisterTests.cmake")

    RegisterTest("${OVIA_CodecIO_Tests_Dir}" AVCCodec OVIA)
	RegisterTest("${OVIA_CodecIO_Tests_Dir}" DNGCodec OVIA)
	RegisterTest("${OVIA_CodecIO_Tests_Dir}" FLACCodec OVIA)
	RegisterTest("${OVIA_CodecIO_Tests_Dir}" JPEGCodec OVIA)
	RegisterTest("${OVIA_CodecIO_Tests_Dir}" PNGCodec OVIA)

    RegisterTest("${OVIA_EntropyIO_Tests_Dir}" Arithmetic OVIA)
	RegisterTest("${OVIA_EntropyIO_Tests_Dir}" Huffman OVIA)
	RegisterTest("${OVIA_EntropyIO_Tests_Dir}" Flate OVIA)
	RegisterTest("${OVIA_EntropyIO_Tests_Dir}" Rice OVIA)

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
	RegisterTest("${OVIA_TagIO_Tests_Dir}" VorbisTags OVIA)
	RegisterTest("${OVIA_TagIO_Tests_Dir}" WAVTags OVIA)

	RegisterTest("${OVIA_TransformIO_Tests_Dir}" RCT OVIA)
	RegisterTest("${OVIA_TransformIO_Tests_Dir}" YCoCgR OVIA)
endif(BUILD_TESTING)

install(TARGETS OVIA DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES ${OVIA_PUBLICHEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

add_subdirectory("${FoundationIO_Dir}/Projects" "${CMAKE_OUTPUT_DIRECTORY}")

set_target_properties(OVIA PROPERTIES PREFIX "")

target_link_libraries(OVIA PUBLIC FoundationIO)

include("${FoundationIO_Dir}/Projects/BuildSettings.cmake")
