#include "../logger.h"
#include "decoder_flac.h"
#include "loader.h"

#include <cstring>
#include <iostream>
#include <memory>
namespace audio
{

bool load_flac_from_memory(const std::uint8_t* data, std::size_t data_size, sound_data& result,
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

	auto decoder = drflac_open_memory(data, data_size);
	if(!decoder)
	{
		err = "ERROR : Incorrect wav header";
		return false;
	}

	if(decoder->totalPCMFrameCount == 0)
	{
		err = "ERROR : No data to load from.";
		drflac_close(decoder);
		return false;
	}

	result.info.channels = std::uint8_t(decoder->channels);
	result.info.sample_rate = std::uint32_t(decoder->sampleRate);
	result.info.bytes_per_sample = sizeof(std::int16_t);
	auto total_samples = std::size_t(decoder->totalSampleCount);
	auto data_bytes = std::size_t(total_samples * result.info.bytes_per_sample);

	using duration_rep = sound_info::duration_t::rep;
	auto seconds = (duration_rep(total_samples) / duration_rep(result.info.channels)) /
				   duration_rep(result.info.sample_rate);

	result.info.duration = sound_info::duration_t(seconds);
	result.data.resize(data_bytes);

	auto frames_read = drflac_read_pcm_frames_s16(decoder, std::uint64_t(decoder->totalPCMFrameCount),
												  reinterpret_cast<std::int16_t*>(result.data.data()));

	if(frames_read != std::uint64_t(decoder->totalPCMFrameCount))
	{
		result = {};
	}

	drflac_close(decoder);
	err = {};

	return true;
}
} // namespace audio
