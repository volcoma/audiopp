#pragma once

#include "../sound_data.h"
#include <string>
namespace audio
{

auto load_ogg_from_memory(const std::uint8_t* data, std::size_t data_size, sound_data& result,
						  std::string& err) -> bool;
auto load_wav_from_memory(const std::uint8_t* data, std::size_t data_size, sound_data& result,
						  std::string& err) -> bool;
auto load_mp3_from_memory(const std::uint8_t* data, std::size_t data_size, sound_data& result,
						  std::string& err) -> bool;
auto load_from_memory(const std::uint8_t* data, std::size_t data_size, sound_data& result, std::string& err)
	-> bool;

auto load_ogg_from_file(const std::string& path, sound_data& result, std::string& err) -> bool;
auto load_wav_from_file(const std::string& path, sound_data& result, std::string& err) -> bool;
auto load_mp3_from_file(const std::string& path, sound_data& result, std::string& err) -> bool;

auto load_from_file(const std::string& path, sound_data& result, std::string& err) -> bool;
} // namespace audio
