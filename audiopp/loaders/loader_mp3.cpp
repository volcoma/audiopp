#include "../logger.h"
#include "loader.h"
#include "minimp3_ex.h"

#include <cstring>
#include <iostream>
namespace audio
{

bool load_mp3_from_memory(const std::uint8_t* data, std::size_t data_size, sound_data& result,
						  std::string& err)
{
	if(!data)
	{
		err = "ERROR : No data to load from.";
		return false;
	}
	if(!data_size)
	{
		err = "ERROR : No data to load from.";
		return false;
	}

	mp3dec_t mp3d;
	mp3dec_file_info_t info;
	mp3dec_load_buf(&mp3d, data, data_size, &info, nullptr, nullptr);

	if(info.samples == 0)
	{
		err = "ERROR : mp3: could not read any samples";
		return false;
	}

	result.info.channels = std::uint8_t(info.channels);
	result.info.sample_rate = std::uint32_t(info.hz);
	result.info.bytes_per_sample = sizeof(mp3d_sample_t);

	std::size_t data_bytes = info.samples * result.info.bytes_per_sample;

	using duration_rep = sound_info::duration_t::rep;
	auto seconds = (duration_rep(info.samples) / duration_rep(info.channels)) / duration_rep(info.hz);

	result.info.duration = sound_info::duration_t(seconds);
	result.data.resize(data_bytes, 0);

	std::memcpy(result.data.data(), info.buffer, result.data.size());

	err = {};
	return true;
}
} // namespace audio
