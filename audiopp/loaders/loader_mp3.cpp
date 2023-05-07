#include "loader.h"
#include "decoders/decoder_mp3.h"
#include "../sound_data.h"
#include "../types.h"

#include <cstring>
#include <memory>
namespace audio
{

auto load_from_memory_mp3(const std::uint8_t* data, std::size_t data_size, sound_data& result,
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

    static const auto deleter = [](mp3dec_file_info_t* map_info) {
        if(map_info->buffer)
            free(map_info->buffer);
    };
    using decoder_t = std::unique_ptr<mp3dec_file_info_t, decltype(deleter)>;

    mp3dec_t mp3d{};
    mp3dec_file_info_t file_info{};
    mp3dec_load_buf(&mp3d, data, data_size, &file_info, nullptr, nullptr);

    decoder_t decoder(&file_info, deleter);
    if(decoder->samples == 0)
    {
        err = "No frames loaded.";
        return false;
    }

    if(!decoder->buffer)
    {
        err = "No frames loaded.";
        return false;
    }

    auto& info = result.info;
    info.channels = std::uint8_t(decoder->channels);
    info.sample_rate = std::uint32_t(decoder->hz);
    info.bits_per_sample = 16;
    info.frames = std::uint64_t(decoder->samples) / std::uint64_t(decoder->channels);
    info.duration = duration_t(duration_t::rep(info.frames) / duration_t::rep(info.sample_rate));

    auto num_samples = std::size_t(info.frames * info.channels);
    auto data_bytes = std::size_t(num_samples * (info.bits_per_sample / 8u));
    result.data.resize(data_bytes);

    std::memcpy(result.data.data(), decoder->buffer, data_bytes);
    err = {};
    return true;
}
} // namespace audio
