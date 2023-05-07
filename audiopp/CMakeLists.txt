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

target_link_libraries(audiopp PRIVATE decoders)
target_link_libraries(audiopp PRIVATE OpenAL::OpenAL)


include(target_warning_support)
set_warning_level(audiopp ultra)

include(target_code_style_support)
set_code_style(audiopp lower_case check_headers "${extra_flags}")
