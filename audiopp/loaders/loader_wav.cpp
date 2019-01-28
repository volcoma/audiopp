#include "loader.h"
#include "../logger.h"
#include <algorithm>
#include <cstring>
namespace audio
{

enum format : uint32_t
{
    pcm = 0x0001,
    ieee_single = 0x0003,
    alaw = 0x0006,
    mulaw = 0x0007,
    extensible = 0xFFFE
};

struct riff_header
{
    constexpr static const std::size_t spec_sz = 12;
    /// Contains the letters "RIFF" in ASCII form
    /// (0x52494646 big-endian form).
    char header[4] = {0};

    /// Size of the wav portion of the file, which follows the first 8 bytes. File size - 8
    std::uint32_t wav_size = 0;

    /// Contains the letters "WAVE"
    /// (0x57415645 big-endian form).
    char wave_header[4] = {0};
};

struct format_header
{
    constexpr static const std::size_t spec_sz = 24;
    /// Contains the letters "fmt " (includes trailing space)
    /// (0x666d7420 big-endian form).
    char header[4] = {0};

    /// 16 for PCM.  This is the size of the
    /// rest of the Subchunk which follows this number.
    /// May be different than 16. This indicates where
    /// the data header starts offsetted from here.
    std::uint32_t fmt_chunk_size = 0;

    /// PCM = 1 (i.e. Linear quantization)
    /// Values other than 1 indicate some
    /// form of compression.
    std::uint16_t audio_format = 0;

    /// Mono = 1, Stereo = 2, etc.
    std::uint16_t num_channels = 0;

    /// 8000, 44100, etc.
    std::uint32_t sample_rate = 0;

    /// Number of bytes per second. sample_rate * num_channels * bytes per sample
    std::uint32_t byte_rate = 0;

    /// num_channels * bytes per sample
    std::uint16_t sample_alignment = 0;

    /// Number of bits per sample
    std::uint16_t bit_depth = 0;
};

struct data_header
{
    constexpr static const std::size_t spec_sz = 8;
    /// Contains the letters "data"
    /// (0x64617461 big-endian form).
    char header[4] = {0};

    /// Number of bytes in data. Number of samples * num_channels * sample byte size
    std::uint32_t data_bytes = 0;
};

/// Wave files have a master RIFF chunk which includes a
/// WAVE identifier followed by sub-chunks.
/// All sub-headers e.g riff.header, riff.wave_header, format.header, data.header
/// are in big-endian byte order.
struct wav_header
{
    //------------------
    // RIFF Header
    //------------------
    riff_header riff;

    //------------------
    // Format Header
    //------------------
    format_header format;

    //------------------
    // Data Header
    //------------------
    data_header data;
};

bool get_chunk_offset(const std::uint8_t* data, size_t size, const std::string& chunk_id, size_t& offset)
{
    auto chunk_id_len = chunk_id.length();
    for(size_t i = 0; i < size - chunk_id_len; i++)
    {
        std::string section(data + i, data + i + chunk_id_len);
        if(section == chunk_id)
        {
            offset = i;
            return true;
        }
    }

    return false;
}

static bool read_header(wav_header& header, const std::uint8_t* data, size_t size, size_t& offset)
{
    std::memcpy(&header.riff, data, riff_header::spec_sz);
    if(std::memcmp(header.riff.header, "RIFF", 4) != 0)
    {
        return false;
    }

    if(!get_chunk_offset(data, size, "fmt ", offset))
    {
        return false;
    }

    std::memcpy(&header.format, data + offset, format_header::spec_sz);

    if(!get_chunk_offset(data, size, "data", offset))
    {
        return false;
    }

    std::memcpy(&header.data, data + offset, data_header::spec_sz);

    offset += data_header::spec_sz;

    (void)data;
    (void)size;

    return true;
}

bool load_wav_from_memory(const std::uint8_t* data, std::size_t data_size, sound_data& result,
                          std::string& err)
{
    if(!data)
    {
        err = "ERROR : No data to load from.";
        return false;
    }

    size_t offset = 0;
    wav_header header;
    if(!read_header(header, data, data_size, offset))
    {
        err = "ERROR : Incorrect wav header";
        return false;
    }

    if(header.data.data_bytes == 0)
    {
        err = "ERROR : No data to load from.";
        return false;
    }

    if(header.format.audio_format != static_cast<uint32_t>(format::pcm))
    {
        err = "ERROR : Not supported compressed wav format.";
        return false;
    }

    result.info.sample_rate = std::uint32_t(header.format.sample_rate);
    result.info.duration = sound_info::duration_t(sound_info::duration_t::rep(header.data.data_bytes) /
                                                  sound_info::duration_t::rep(header.format.byte_rate));

    result.info.bytes_per_sample = std::uint8_t(header.format.bit_depth) / 8;
    result.info.channels = std::uint8_t(header.format.num_channels);

    // offset to the samples
    data += offset;
    result.data.resize(std::size_t(header.data.data_bytes));
    std::memcpy(result.data.data(), data, result.data.size());

    return true;
}
} // namespace audio
