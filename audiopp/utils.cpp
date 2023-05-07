#include "utils.h"
#include "logger.h"
#include <cstring>
namespace audio
{
namespace utils
{

template <typename SampleType>
auto convert_to_mono(const std::vector<std::uint8_t>& stereo_samples, std::uint8_t bits_per_sample)
    -> std::vector<std::uint8_t>
{
    size_t bytes_per_sample = bits_per_sample / 8u;
    if(bytes_per_sample > 2)
    {
        error() << "Sound buffer is not 8/16 bits per sample. Unsupported";
        return stereo_samples;
    }

    if(sizeof(SampleType) != bytes_per_sample)
    {
        error() << "Requested sample type is not the proper size";
        return stereo_samples;
    }

    const auto input_size = stereo_samples.size();
    if(input_size % bytes_per_sample != 0)
    {
        error() << "Sound buffer is not the proper size";
        return stereo_samples;
    }

    std::vector<std::uint8_t> output;
    output.reserve(input_size / 2);

    for(std::size_t i = 0; i < input_size; i += 2 * bytes_per_sample)
    {
        const auto left_sample = *reinterpret_cast<const SampleType*>(&stereo_samples[i]);
        const auto right_sample = *reinterpret_cast<const SampleType*>(&stereo_samples[i + bytes_per_sample]);
        const auto stereo_sample = std::int32_t(left_sample) + std::int32_t(right_sample);
        const auto mono_sample = SampleType(stereo_sample / 2);

        for(std::size_t j = 0; j < bytes_per_sample; ++j)
        {
            const auto sample_part = std::uint8_t((mono_sample >> (j * 8)) & 0xff);
            output.emplace_back(sample_part);
        }
    }

    return output;
}

auto convert_to_mono(const std::vector<std::uint8_t>& stereo_samples, std::uint8_t bits_per_sample)
    -> std::vector<std::uint8_t>
{
    if(bits_per_sample == 8)
    {
        return convert_to_mono<std::uint8_t>(stereo_samples, bits_per_sample);
    }
    else if(bits_per_sample == 16)
    {
        return convert_to_mono<std::int16_t>(stereo_samples, bits_per_sample);
    }
    return stereo_samples;
}

auto convert_to_stereo(const std::vector<std::uint8_t>& mono_samples, std::uint8_t bits_per_sample)
    -> std::vector<std::uint8_t>
{
    size_t bytes_per_sample = bits_per_sample / 8u;

    if(bytes_per_sample > 2)
    {
        error() << "Sound buffer is not 8/16 bits per sample";
        return mono_samples;
    }

    const auto input_size = mono_samples.size();
    if(input_size % bytes_per_sample != 0)
    {
        error() << "Sound buffer is not the proper size";
        return mono_samples;
    }

    std::vector<std::uint8_t> output;
    output.resize(input_size * 2);

    for(std::size_t i = 0; i < input_size; i += bytes_per_sample)
    {
        const std::size_t dst_idx = i * 2;
        const std::uint8_t* mono_sample = mono_samples.data() + i;
        std::uint8_t* dst_part1 = output.data() + dst_idx;
        std::uint8_t* dst_part2 = dst_part1 + bytes_per_sample;

        std::memcpy(dst_part1, mono_sample, bytes_per_sample);
        std::memcpy(dst_part2, mono_sample, bytes_per_sample);
    }

    return output;
}
} // namespace utils
} // namespace audio
