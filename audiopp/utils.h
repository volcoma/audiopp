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
std::vector<std::uint8_t> convert_to_mono(const std::vector<std::uint8_t>& input,
										  std::uint8_t bytes_per_sample);

//-----------------------------------------------------------------------------
/// Converts an input buffer to stereo
//-----------------------------------------------------------------------------
std::vector<std::uint8_t> convert_to_stereo(const std::vector<std::uint8_t>& input,
											std::uint8_t bytes_per_sample);
} // namespace utils
} // namespace audio
