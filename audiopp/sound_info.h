#pragma once

#include <chrono>
#include <cstdint>
#include <sstream>
#include <string>
namespace audio
{

struct sound_info
{
	using duration_t = std::chrono::duration<double>;

	/// id of the sound
	std::string id{};
	/// duration of the sound in seconds
	duration_t loading = duration_t(0);

	/// duration of the sound in seconds
	duration_t duration = duration_t(0);

	/// sample rate (samples per second) of the sound (hz)
	std::uint32_t sample_rate = 0;

	/// bytes per sample (sample size)
	std::uint8_t bytes_per_sample = 2;

	/// channel count of the sound. e.g mono/stereo
	std::uint8_t channels = 0;
};

} // namespace audio

inline auto to_string(const audio::sound_info& info) -> std::string
{
	std::stringstream ss;
	ss << "id          : " << info.id;
	ss << "\n";

	ss << "sample size : " << uint32_t(info.bytes_per_sample) << " byte"
	   << (info.bytes_per_sample > 1 ? "s" : "") << " (" << uint32_t(info.bytes_per_sample * 8) << " bits)";
	ss << "\n";

	ss << "sample rate : " << info.sample_rate << " hz";
	ss << "\n";

	ss << "channels    : " << uint32_t(info.channels);
	ss << "\n";

	ss << "duration    : " << info.duration.count() << " seconds";
	ss << "\n";
	ss << "loaded for  : " << info.loading.count() << " seconds";

	return ss.str();
}

inline auto operator==(const audio::sound_info& lhs, const audio::sound_info& rhs) -> bool
{
	return lhs.id == rhs.id && lhs.sample_rate == rhs.sample_rate &&
		   lhs.bytes_per_sample == rhs.bytes_per_sample && lhs.channels == rhs.channels &&
		   lhs.duration == rhs.duration;
}

inline auto operator!=(const audio::sound_info& lhs, const audio::sound_info& rhs) -> bool
{
	return !(lhs == rhs);
}
