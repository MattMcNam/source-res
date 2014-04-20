cmake_minimum_required(VERSION 2.6)

project(SourceRes)

set(PACKAGE "SourceRes")
set(PACKAGE_VERSION "1.0")
set(PACKAGE_BUGREPORT "https://github.com/MattMcNam/SourceRes/issues")
set(PACKAGE_NAME "${PACKAGE}")
set(PACKAGE_STRING "${PACKAGE} ${PACKAGE_VERSION}")
set(PACKAGE_TARNAME "${PACKAGE}_${PACKAGE_VERSION}")
set(PACKAGE_URL "https://github.com/MattMcNam/SourceRes")
set(VERSION "${PACKAGE_VERSION}")

include_directories(${PROJECT_BINARY_DIR})
# Put final product in 'bin' folder
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

# Include source and hl2sdk
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${HL2SDK}/common)
include_directories(${HL2SDK}/public)
include_directories(${HL2SDK}/public/tier0)
include_directories(${HL2SDK}/public/tier1)
include_directories(${HL2SDK}/game/client)
include_directories(${HL2SDK}/game/shared)

set(HDR_PUBLIC
	src/sourceres.h
	src/stdafx.h
	)

set(SOURCES
	src/sourceres.cpp
	)

source_group("Header Files" FILES ${HDR_PUBLIC})
source_group("Source Files" FILES ${SOURCES})

# Unix not yet supported
if(UNIX)
	message( FATAL_ERROR "Mac and Linux are not yet supported" )
endif()

# Force use of static libs
# Credit SteveL & brofield on StackOverflow
foreach(flag_var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINOFO)
	string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
endforeach()

add_definitions(/D_CRT_SECURE_NO_DEPRECATE /D_CRT_SECURE_NO_WARNINGS /D_CRT_NONSTDC_NO_DEPRECATE)
set(COMPILER_FLAGS "/EHsc /DR- /W3 /nologo /Zi /TP")
set(LINKER_FLAGS "/MACHINE:X86 /subsystem:windows /NODEFAULTLIB:libc /NODEFAULTLIB:libcd")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:libcmt")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /NODEFAULTLIB:libcmtd")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILER_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${LINKER_FLAGS}")

# Create the plugin
add_library(SourceRes SHARED
	${HDR_PUBLIC}
	${SOURCES})

# Remove 'lib' prefix
set_target_properties(SourceRes PROPERTIES PREFIX "")

# Link sdk libraries
target_link_libraries(SourceRes ${HL2SDK}/lib/public/tier0.lib)
target_link_libraries(SourceRes ${HL2SDK}/lib/public/tier1.lib)
