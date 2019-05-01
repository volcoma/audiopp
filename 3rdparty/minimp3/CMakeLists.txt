file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.inl *.c *.cc *.m)

add_library (minimp3 STATIC ${libsrc})
target_include_directories (minimp3 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(minimp3 PUBLIC _CRT_SECURE_NO_WARNINGS=1)

set_target_properties(minimp3 PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
