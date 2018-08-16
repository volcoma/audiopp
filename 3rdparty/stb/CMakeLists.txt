file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.inl *.c *.cc *.m)

add_library (stb_vorbis STATIC ${libsrc})
target_include_directories (stb_vorbis PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(stb_vorbis PUBLIC _CRT_SECURE_NO_WARNINGS=1)
