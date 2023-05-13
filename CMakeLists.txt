cmake_minimum_required(VERSION 3.8)

project(audiopp)

option(BUILD_AUDIOPP_SHARED "Build as a shared library." ON)
option(BUILD_AUDIOPP_TESTS "Build the tests" ON)

if(BUILD_AUDIOPP_TESTS)
	if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
	endif()
	if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
	endif()
	if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
    endif()

    # Second, for multi-config builds (e.g. msvc)
    foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
        string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
        set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )
        set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
        set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} )
    endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

    message(STATUS "Enabled ${PROJECT_NAME} tests.")

endif()

if(BUILD_AUDIOPP_SHARED)
	set(BUILD_SHARED_LIBS ON)
endif()

include(CheckFunctionExists)
include(CheckLibraryExists)
include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckSymbolExists)
include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)
include(CheckCSourceCompiles)
include(CheckCXXSourceCompiles)
include(CheckStructHasMember)

add_subdirectory(3rdparty)
add_subdirectory(audiopp)

if(BUILD_AUDIOPP_TESTS)
    include(CTest)
    add_subdirectory(tests)
endif()
