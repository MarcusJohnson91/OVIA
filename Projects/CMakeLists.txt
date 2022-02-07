cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

# Ok, so new structure: Globals, followed by Options, followed by config-specific variables.

project(OVIA C)

if(DEFINED CMAKE_BUILD_TYPE AND CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Debug")
elseif(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif(DEFINED CMAKE_BUILD_TYPE AND CMAKE_BUILD_TYPE STREQUAL "")

include(../Dependencies/FoundationIO/Projects/DetectArchitecture.cmake)
FindTargetArchitecture(TargetArchitecture)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../BUILD/${TargetArchitecture}/${CMAKE_BUILD_TYPE}")
set(CMAKE_OUTPUT_DIRECTORY         "${CMAKE_CURRENT_SOURCE_DIR}/../BUILD/${TargetArchitecture}/${CMAKE_BUILD_TYPE}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../BUILD/${TargetArchitecture}/${CMAKE_BUILD_TYPE}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../BUILD/${TargetArchitecture}/${CMAKE_BUILD_TYPE}")
set(OVIA_Library_Dir               "${CMAKE_CURRENT_SOURCE_DIR}/../Library")
set(OVIA_Test_Dir                  "${CMAKE_CURRENT_SOURCE_DIR}/../Tests") # CHANGE THIS
set(OVIA_Resource_Dir              "${CMAKE_CURRENT_SOURCE_DIR}/../Resources")

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
)

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

include(../Dependencies/FoundationIO/Projects/GetVersion.cmake) # Honestly, I want this to get the version from the main header
GetVersionString(VERSION_STRING "/Library")

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

	macro(register_test ComponentName TestName)
        string(CONCAT Test_Name "Test_${TestName}")

        add_executable(${Test_Name} "Library/Internal/src/${ComponentName}/Tests/${Test_Name}.c")

        target_link_libraries(${Test_Name} OVIA)

        set(UNIT_TEST_TARGETS ${UNIT_TEST_TARGETS} ${Test_Name})

        add_test(${Test_Name} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${Test_Name}")

        set_tests_properties(${Test_Name} PROPERTIES TIMEOUT 15)
    endmacro(register_test)

	register_test(StreamIO AIFStream)
	register_test(StreamIO BMPStream)
	register_test(StreamIO MPEG2Stream)
	register_test(StreamIO PNMStream)
	register_test(StreamIO TIFStream)
	register_test(StreamIO W64Stream)
	register_test(StreamIO WAVStream)

	register_test(CodecIO AVCCodec)
	register_test(CodecIO DNGCodec)
	register_test(CodecIO FLACCodec)
	register_test(CodecIO JPEGCodec)
	register_test(CodecIO PNGCodec)

	register_test(TagIO AIFTags)
	register_test(TagIO APETags)
	register_test(TagIO ID3Tags)
	register_test(TagIO VorbisTags)
	register_test(TagIO WAVTags)

	register_test(EntropyIO Arithmetic)
	register_test(EntropyIO Huffman)
	register_test(EntropyIO Flate)
	register_test(EntropyIO Rice)

	register_test(TransformIO RCT)
	register_test(TransformIO YCoCgR)

    add_custom_target(OVIA_Tests Test DEPENDS ${UNIT_TEST_TARGETS})
    add_custom_command(TARGET OVIA_Tests COMMENT "Run tests" POST_BUILD COMMAND ctest ARGS --output-on-failure WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif(BUILD_TESTING)

install(TARGETS OVIA DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES ${OVIA_PUBLICHEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

add_subdirectory(../Dependencies/FoundationIO/Projects "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/../Dependencies/FoundationIO/BUILD")

set_target_properties(OVIA PROPERTIES PREFIX "")

target_link_libraries(OVIA PUBLIC FoundationIO)

if(${CMAKE_C_COMPILER_ID} STREQUAL "AppleClang" OR ${CMAKE_C_COMPILER_ID} STREQUAL "Clang")

set(CMAKE_C_FLAGS " \
${CMAKE_C_FLAGS} \
-fPIC \
-fshort-enums \
-funsigned-char \
-std=c18 \
-Weverything \
-Wall \
-Wextra \
-Wpedantic \
-Wno-assign-enum \
-Wno-c99-compat \
-Wno-ignored-attributes \
-Wno-incompatible-pointer-types-discards-qualifiers \
-Wno-padded \
-Wno-pointer-sign \
-Wno-reserved-id-macro \
-Wno-sign-conversion \
-Wsuggest-attribute=const \
-Wenum-conversion \
-v \
")

set(CMAKE_C_FLAGS_DEBUG " \
${CMAKE_C_FLAGS} \
-fsanitize=address,undefined \
-fno-omit-frame-pointer \
-DDEBUG=1 \
-g \
-O1 \
")

set(CMAKE_C_FLAGS_RELEASE " \
${CMAKE_C_FLAGS} \
-Ofast \
")

set(CMAKE_C_FLAGS_RELWITHDEBINFO " \
${CMAKE_C_FLAGS} \
-fsanitize=address,undefined \
-fno-omit-frame-pointer \
-DDEBUG=1 \
-g \
-Ofast \
")

set(CMAKE_C_FLAGS_MINSIZEREL " \
${CMAKE_C_FLAGS} \
-Oz \
")

set(LINK_FLAGS ${LINK_FLAGS} " \
-flto=full \
-lm \
")

if(CMAKE_HOST_APPLE)
set(LINK_FLAGS ${LINK_FLAGS} " \
-dead_strip_dylibs
")
endif(CMAKE_HOST_APPLE)

set(LINK_FLAGS_DEBUG " \
${LINK_FLAGS} \
")

set(LINK_FLAGS_RELWITHDEBINFO " \
${LINK_FLAGS} \
")

set(LINK_FLAGS_RELEASE " \
${LINK_FLAGS} \
")

set(LINK_FLAGS_MINSIZEREL " \
${LINK_FLAGS} \
")

elseif(${CMAKE_C_COMPILER_ID} STREQUAL "GNU")

set(CMAKE_C_FLAGS " \
${CMAKE_C_FLAGS} \
-fPIC \
-fshort-enums \
-funroll-loops \
-funsigned-char \
-std=c2x \
-Wall \
-Wextra \
-Wpedantic \
-Wno-ignored-attributes \
-Wno-padded \
-Wno-pointer-sign \
-Wno-sign-conversion \
-Wno-varargs \
-Wsuggest-attribute=const \
-Wsuggest-attribute=pure \
-Wsuggest-attribute=malloc \
-Wsuggest-attribute=noreturn \
-Wsuggest-attribute=format \
-Wmissing-format-attribute \
-Wmissing-noreturn \
")

set(CMAKE_C_FLAGS_DEBUG " \
${CMAKE_C_FLAGS} \
-fsanitize=address,undefined \
-DDEBUG=1 \
-g \
-O1 \
")

set(CMAKE_C_FLAGS_RELEASE " \
${CMAKE_C_FLAGS} \
-Ofast \
")

set(CMAKE_C_FLAGS_RELWITHDEBINFO " \
${CMAKE_C_FLAGS} \
-fsanitize=address,undefined \
-fno-omit-frame-pointer \
-DDEBUG=1 \
-g \
-Ofast \
")

set(CMAKE_C_FLAGS_MINSIZEREL " \
${CMAKE_C_FLAGS} \
-Os \
")

set(LINK_FLAGS ${LINK_FLAGS} " \
-flto=full \
-lm \
")

if(CMAKE_HOST_APPLE)
set(LINK_FLAGS ${LINK_FLAGS} " \
-dead_strip_dylibs
")
endif(CMAKE_HOST_APPLE)

set(LINK_FLAGS_DEBUG " \
${LINK_FLAGS} \
")

set(LINK_FLAGS_RELWITHDEBINFO " \
${LINK_FLAGS} \
")

set(LINK_FLAGS_RELEASE " \
${LINK_FLAGS} \
")

set(LINK_FLAGS_MINSIZEREL " \
${LINK_FLAGS} \
")

elseif(${CMAKE_C_COMPILER_ID} STREQUAL "MSVC")

set(CMAKE_C_FLAGS " \
${CMAKE_C_FLAGS} \
/bigobj \
/FAu \
/GF \
/Gm- \
/GT \
/Gy \
/JMC \
/J \
/MP \
/MT \
/Ob2 \
/openmp \
/p:CharacterSet=Unicode \
/p:FunctionLevelLinking=true \
/p:PrecompiledHeader=NotUsing \
/p:RuntimeLibrary=MultiThreaded \
/p:UseOfMfc=false \
/p:WarningLevel=Level3 \
/Qpar \
/sdl- \
/std:c++17 \
/W3 \
/Zc:rvalueCast \
/Zc:wchar_t \
/Wall \
")

set(CMAKE_C_FLAGS_DEBUG " \
${CMAKE_C_FLAGS} \
/DDEBUG=1 \
/D_DEBUG=1 \
/p:UseDebugLibraries=true \
/O1 \
")

set(CMAKE_C_FLAGS_RELEASE " \
${CMAKE_C_FLAGS} \
/p:UseDebugLibraries=false \
/Ox \
")

set(CMAKE_C_FLAGS_RELWITHDEBINFO " \
${CMAKE_C_FLAGS} \
/DDEBUG=1 \
/D_DEBUG=1 \
/p:UseDebugLibraries=true \
-Ox \
")

set(CMAKE_C_FLAGS_MINSIZEREL " \
${CMAKE_C_FLAGS} \
/p:UseDebugLibraries=false \
-Os \
")

set(LINK_FLAGS " \
${LINK_FLAGS} \
/GL \
/LTCG:INCREMENTAL
/OPT:REF
/flto \
")

set(LINK_FLAGS_DEBUG " \
${LINK_FLAGS} \
/DEBUG \
")


set(LINK_FLAGS_RELWITHDEBINFO " \
${LINK_FLAGS} \
/DEBUG \
/RELEASE \
")

set(LINK_FLAGS_RELEASE " \
${LINK_FLAGS} \
/RELEASE \
")

set(LINK_FLAGS_MINSIZEREL " \
${LINK_FLAGS} \
/RELEASE \
")

endif(${CMAKE_C_COMPILER_ID} STREQUAL "AppleClang" OR ${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
