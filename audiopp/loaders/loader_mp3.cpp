#include "../logger.h"
#include "decoder_mp3.h"
#include "loader.h"

#include <cstring>
namespace audio
{

auto load_from_memory_mp3(const std::uint8_t* data, std::size_t data_size, sound_data& result,
						  std::string& err) -> bool
{
	if(!data)
	{
		err = "No data to load from.";
		return false;
	}
	if(!data_size)
	{
		err = "No data to load from.";
		return false;
	}

	mp3dec_t mp3d;
	mp3dec_file_info_t decoded_info;
	mp3dec_load_buf(&mp3d, data, data_size, &decoded_info, nullptr, nullptr);

	if(decoded_info.samples == 0)
	{
		err = "No frames loaded.";
		return false;
	}

	using duration_t = sound_info::duration_t;
	auto& info = result.info;
	info.channels = std::uint8_t(decoded_info.channels);
	info.sample_rate = std::uint32_t(decoded_info.hz);
	info.bits_per_sample = 16;
	info.frames = std::uint64_t(decoded_info.samples) / std::uint64_t(decoded_info.channels);
	info.duration = duration_t(duration_t::rep(info.frames) / duration_t::rep(info.sample_rate));

	auto data_bytes = std::size_t(info.frames * info.channels * (info.bits_per_sample / 8u));
	result.data.resize(data_bytes);

	std::memcpy(result.data.data(), decoded_info.buffer, data_bytes);
	err = {};
	return true;
}
} // namespace audio
