file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.inl *.c *.cc *.m)

add_library (decoders STATIC ${libsrc})
target_include_directories (decoders PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_compile_definitions(decoders PUBLIC _CRT_SECURE_NO_WARNINGS=1)

set_target_properties(decoders PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
