cmake_minimum_required(VERSION 3.8)

project(audiopp)
include(CTest)

option(BUILD_AUDIOPP_SHARED "Build as a shared library." ON)
option(BUILD_AUDIOPP_TESTS "Build the tests" ON)

if(BUILD_AUDIOPP_TESTS)
	if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
	endif()
    if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
    endif()
	if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
	endif()

    # Second, for multi-config builds (e.g. msvc)
    foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
        string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
        set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )
        set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} )
        set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
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
    add_subdirectory(tests)
    
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
    include(InstallRequiredSystemLibraries)

    if(MINGW)
        get_filename_component( Mingw_Path ${CMAKE_CXX_COMPILER} PATH )
        file(GLOB MINGW_SYSTEM_RUNTIME_LIBS "${Mingw_Path}/*.dll")
        set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} ${MINGW_SYSTEM_RUNTIME_LIBS})
    endif()
    message(STATUS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS})


    if(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
    add_custom_target(copy_compiler_dependencies ALL
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    endif()

    if(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
        install( PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )
    endif()
endif()
