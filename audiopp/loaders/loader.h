#pragma once

#include <string>
namespace audio
{

struct sound_data;

auto load_from_memory_ogg(const std::uint8_t* data, std::size_t data_size, sound_data& result,
                          std::string& err) -> bool;
auto load_from_memory_wav(const std::uint8_t* data, std::size_t data_size, sound_data& result,
                          std::string& err) -> bool;
auto load_from_memory_mp3(const std::uint8_t* data, std::size_t data_size, sound_data& result,
                          std::string& err) -> bool;
auto load_from_memory_flac(const std::uint8_t* data, std::size_t data_size, sound_data& result,
                           std::string& err) -> bool;
auto load_from_memory(const std::uint8_t* data, std::size_t data_size, sound_data& result, std::string& err)
    -> bool;

auto load_from_file_ogg(const std::string& path, sound_data& result, std::string& err) -> bool;
auto load_from_file_wav(const std::string& path, sound_data& result, std::string& err) -> bool;
auto load_from_file_mp3(const std::string& path, sound_data& result, std::string& err) -> bool;
auto load_from_file_flac(const std::string& path, sound_data& result, std::string& err) -> bool;
auto load_from_file(const std::string& path, sound_data& result, std::string& err) -> bool;
} // namespace audio
