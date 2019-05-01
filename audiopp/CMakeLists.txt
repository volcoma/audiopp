file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_library(audiopp ${libsrc})

target_include_directories(audiopp
    PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/..
)
set_target_properties(audiopp PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    POSITION_INDEPENDENT_CODE ON
    WINDOWS_EXPORT_ALL_SYMBOLS ON
)

find_package(OpenAL REQUIRED)

target_link_libraries(audiopp PRIVATE stb_vorbis)
target_link_libraries(audiopp PRIVATE minimp3)
target_link_libraries(audiopp PRIVATE ${OPENAL_LIBRARY})
target_include_directories(audiopp PRIVATE ${OPENAL_INCLUDE_DIR})


include(target_warning_support)
set_warning_level(audiopp ultra)

include(target_code_style_support)
set_code_style(audiopp lower_case check_headers "${extra_flags}")
