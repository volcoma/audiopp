message(STATUS "Enabled tests.")

file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_executable(tests ${libsrc})

target_link_libraries(tests PUBLIC audiopp)
target_compile_definitions(tests PUBLIC DATA="${CMAKE_CURRENT_SOURCE_DIR}/../tests_data/")

set_target_properties(tests PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

include(target_warning_support)
set_warning_level(tests ultra)

include(target_code_style_support)
set_code_style(tests lower_case check_headers "${extra_flags}")
