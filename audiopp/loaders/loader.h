#pragma once

#include "../sound_data.h"
#include <string>
namespace audio
{

bool load_ogg_from_memory(const std::uint8_t* data, std::size_t data_size, sound_data& result,
						  std::string& err);
bool load_wav_from_memory(const std::uint8_t* data, std::size_t data_size, sound_data& result,
						  std::string& err);
bool load_from_memory(const std::uint8_t* data, std::size_t data_size, sound_data& result, std::string& err);

bool load_ogg_from_file(const std::string& path, sound_data& result, std::string& err);
bool load_wav_from_file(const std::string& path, sound_data& result, std::string& err);

bool load_from_file(const std::string& path, sound_data& result, std::string& err);
} // namespace audio
