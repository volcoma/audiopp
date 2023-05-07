#pragma once

#include <array>
#include <chrono>
#include <cstdint>

namespace audio
{
using float3 = std::array<float, 3>;

using duration_t = std::chrono::duration<double>; // in seconds
}
