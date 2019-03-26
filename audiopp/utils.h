#pragma once

#include <cstdint>
#include <vector>

namespace audio
{

namespace utils
{

//-----------------------------------------------------------------------------
/// Converts an input buffer to mono
//-----------------------------------------------------------------------------
auto convert_to_mono(const std::vector<std::uint8_t>& stereo_samples, std::uint8_t bytes_per_sample)
	-> std::vector<std::uint8_t>;

//-----------------------------------------------------------------------------
/// Converts an input buffer to stereo
//-----------------------------------------------------------------------------
auto convert_to_stereo(const std::vector<std::uint8_t>& mono_samples, std::uint8_t bytes_per_sample)
	-> std::vector<std::uint8_t>;
} // namespace utils
} // namespace audio
