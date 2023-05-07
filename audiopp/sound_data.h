#pragma once

#include "sound_info.h"
#include <cstdint>
#include <vector>

namespace audio
{

struct sound_data
{
    //-----------------------------------------------------------------------------
    /// Converts internal data to mono/1 channel. Ideal for 3d positional sounds.
    //-----------------------------------------------------------------------------
    void convert_to_mono();

    //-----------------------------------------------------------------------------
    /// Converts internal data to stereo/2 channels. These will not be affected by
    /// 3d attenuation.
    //-----------------------------------------------------------------------------
    void convert_to_stereo();

    //-----------------------------------------------------------------------------
    /// Converts internal data to mono or stereo depending on its type.
    //-----------------------------------------------------------------------------
    void convert_to_opposite();

    /// data buffer of pcm sound stored in uint8_t buffer
    std::vector<std::uint8_t> data;

    /// info about the sound
    sound_info info;
};
} // namespace audio
