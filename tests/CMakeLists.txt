message(STATUS "Enabled tests.")

set(target_name audiopp_test)

file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_executable(${target_name} ${libsrc})

target_link_libraries(${target_name} PUBLIC audiopp suitepp)
target_compile_definitions(${target_name} PUBLIC DATA="${CMAKE_CURRENT_SOURCE_DIR}/tests_data/")

set_target_properties(${target_name} PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
enable_testing()
add_test(NAME ${target_name} COMMAND ${target_name})
