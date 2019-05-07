#include "../logger.h"
#include "loader.h"
#include <decoders/decoder_flac.h>

#include <memory>
namespace audio
{

auto load_from_memory_flac(const std::uint8_t* data, std::size_t data_size, sound_data& result,
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

	static const auto deleter = [](drflac* decoder) { drflac_close(decoder); };
	using decoder_t = std::unique_ptr<drflac, decltype(deleter)>;

	decoder_t decoder(drflac_open_memory(data, data_size), deleter);
	if(!decoder)
	{
		err = "Incorrect flac header.";
		return false;
	}

	if(decoder->totalPCMFrameCount == 0)
	{
		err = "No frames loaded.";
		return false;
	}

	using duration_t = sound_info::duration_t;
	auto& info = result.info;
	info.channels = std::uint8_t(decoder->channels);
	info.sample_rate = std::uint32_t(decoder->sampleRate);
	info.bits_per_sample = 16;
	info.frames = std::uint64_t(decoder->totalPCMFrameCount);
	info.duration = duration_t(duration_t::rep(info.frames) / duration_t::rep(info.sample_rate));

	auto num_samples = std::size_t(info.frames * info.channels);
	auto data_bytes = std::size_t(num_samples * (info.bits_per_sample / 8u));
	result.data.resize(data_bytes);

	auto frames_read = drflac_read_pcm_frames_s16(decoder.get(), info.frames,
												  reinterpret_cast<std::int16_t*>(result.data.data()));

	if(frames_read != info.frames)
	{
		err = "Could not read all the frames. Read " + std::to_string(frames_read) + "/" +
			  std::to_string(info.frames);
		result = {};
		return false;
	}

	err = {};
	return true;
}
} // namespace audio
