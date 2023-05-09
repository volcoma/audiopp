file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.inl *.c *.cc *.m)

add_library (decoders STATIC ${libsrc})
target_include_directories (decoders PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_compile_definitions(decoders PUBLIC _CRT_SECURE_NO_WARNINGS=1)

if(MSVC)
    target_compile_options(decoders PUBLIC /wd4244 /wd4456 /wd4457 /wd4100 /wd4245 /wd4701)
else()

    if(CMAKE_COMPILER_IS_GNUCC)
        target_compile_options(decoders PUBLIC -Wno-maybe-uninitialized)
    endif()

    target_compile_options(decoders PUBLIC -Wno-unused-variable -Wno-unused-parameter -Wno-deprecated-declarations)
endif()


set_target_properties(decoders PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
