#pragma once

#include <chrono>
#include <cstdint>

namespace audio
{

struct sound_info
{
	using duration_t = std::chrono::duration<double>;

	/// duration of the sound in seconds
	duration_t duration = duration_t(0);

	/// sample rate of the sound
	std::uint32_t sample_rate = 0;

	/// bytes per sample
	std::uint8_t bytes_per_sample = 2;

	/// channel count of the sound. e.g mono/stereo
	std::uint8_t channels = 0;

};
} // namespace audio
