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

if(MSVC)
    add_compile_options("/wd4018")
else()
    add_compile_options("-Wno-type-limits")
endif()

add_subdirectory( openal-soft )

