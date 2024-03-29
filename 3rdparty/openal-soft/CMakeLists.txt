cmake_minimum_required(VERSION 3.8)
cmake_policy(SET CMP0067 NEW)

set( ALSOFT_UTILS OFF CACHE BOOL "Build and install utility programs" FORCE)
set( ALSOFT_EXAMPLES OFF CACHE BOOL "Build and install example programs" FORCE)
set( ALSOFT_TESTS OFF CACHE BOOL "Build and install test programs" FORCE)
set( ALSOFT_INSTALL_CONFIG OFF CACHE BOOL "Install alsoft.conf sample configuration file" FORCE)
set( ALSOFT_INSTALL_HRTF_DATA OFF CACHE BOOL "Install HRTF definition files" FORCE)
set( ALSOFT_INSTALL_AMBDEC_PRESETS OFF CACHE BOOL "Install AmbDec preset files" FORCE)
set( ALSOFT_INSTALL OFF CACHE BOOL "Install headers and libraries" FORCE)
set( ALSOFT_BUILD_IMPORT_LIB OFF CACHE BOOL "Build an import .lib using dlltool (requires sed)" FORCE)

#if(MINGW)
#	set( ALSOFT_STATIC_LIBGCC ON CACHE BOOL "Force -static-libgcc for static GCC runtimes" FORCE)
#endif()

add_subdirectory( openal-soft )

target_include_directories (OpenAL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/openal-soft/include/AL)

if(MSVC)
    target_compile_options (OpenAL PUBLIC /wd4018 /wd4267 /wd4244)
else()
    target_compile_options (OpenAL PUBLIC -Wno-type-limits -Wno-switch -Wno-format-extra-args -Wno-format -Wno-conversion -Wno-attributes)
endif()

