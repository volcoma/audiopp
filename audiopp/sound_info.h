#pragma once

#include "types.h"

#include <cstdint>
#include <sstream>
#include <string>

namespace audio
{

struct sound_info
{
    /// id of the sound
    std::string id{};

    /// duration of the sound in seconds
    duration_t duration{};

    /// sample rate (samples per second) of the sound (hz)
    std::uint32_t sample_rate{};

    /// bytes per sample (sample size)
    std::uint8_t bits_per_sample{};

    /// channel count of the sound. e.g mono/stereo
    std::uint8_t channels{};

    /// frames count (samples per channel)
    std::uint64_t frames{};
};

} // namespace audio

inline auto to_string(const audio::sound_info& info) -> std::string
{
    std::stringstream ss;
    ss << "id          : " << info.id;
    ss << "\n";

    ss << "sample size : " << uint32_t(info.bits_per_sample) << " bits";
    ss << "\n";

    ss << "sample rate : " << info.sample_rate << " hz";
    ss << "\n";

    ss << "channels    : " << uint32_t(info.channels);
    ss << "\n";

    ss << "frames      : " << info.frames;
    ss << "\n";

    ss << "duration    : " << info.duration.count() << " seconds";

    return ss.str();
}

inline auto operator==(const audio::sound_info& lhs, const audio::sound_info& rhs) -> bool
{
    return lhs.id == rhs.id && lhs.sample_rate == rhs.sample_rate &&
           lhs.bits_per_sample == rhs.bits_per_sample && lhs.channels == rhs.channels &&
           lhs.duration == rhs.duration;
}

inline auto operator!=(const audio::sound_info& lhs, const audio::sound_info& rhs) -> bool
{
    return !(lhs == rhs);
}
