#include "../logger.h"
#include "loader.h"
#include <decoders/decoder_vorbis.h>
#include <memory>
namespace audio
{

auto load_from_memory_ogg(const std::uint8_t* data, std::size_t data_size, sound_data& result,
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
	static const auto deleter = [](stb_vorbis* decoder) { stb_vorbis_close(decoder); };
	using decoder_t = std::unique_ptr<stb_vorbis, decltype(deleter)>;

	int vorb_err = 0;
	decoder_t decoder(stb_vorbis_open_memory(data, static_cast<int>(data_size), &vorb_err, nullptr), deleter);
	if(!decoder)
	{
		auto decoded_err = STBVorbisError(vorb_err);
		err = "Vorbis error code : " + std::to_string(decoded_err);
		return false;
	}
	stb_vorbis_info decoded_info = stb_vorbis_get_info(decoder.get());

	using duration_t = sound_info::duration_t;
	auto& info = result.info;
	info.channels = std::uint8_t(decoded_info.channels);
	info.sample_rate = std::uint32_t(decoded_info.sample_rate);
	info.bits_per_sample = 16;
	info.frames = std::uint64_t(stb_vorbis_stream_length_in_samples(decoder.get()));
	info.duration = duration_t(duration_t::rep(info.frames) / duration_t::rep(info.sample_rate));

	auto num_samples = std::size_t(info.frames * info.channels);
	auto data_bytes = std::size_t(num_samples * (info.bits_per_sample / 8u));
	result.data.resize(data_bytes);

	auto frames_read = std::uint64_t(stb_vorbis_get_samples_short_interleaved(
		decoder.get(), info.channels, reinterpret_cast<std::int16_t*>(result.data.data()), int(num_samples)));

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
