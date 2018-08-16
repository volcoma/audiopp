#include "audiopp/library.h"
#include "audiopp/loaders/loader.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

namespace detail
{
template <typename Container = std::string, typename CharT = char, typename Traits = std::char_traits<char>>
auto read_stream_into_container(std::basic_istream<CharT, Traits>& in,
								typename Container::allocator_type alloc = {})
{
	static_assert(
		// Allow only strings...
		std::is_same<Container,
					 std::basic_string<CharT, Traits, typename Container::allocator_type>>::value ||
			// ... and vectors of the plain, signed, and
			// unsigned flavours of CharT.
			std::is_same<Container, std::vector<CharT, typename Container::allocator_type>>::value ||
			std::is_same<Container, std::vector<std::make_unsigned_t<CharT>,
												typename Container::allocator_type>>::value ||
			std::is_same<Container,
						 std::vector<std::make_signed_t<CharT>, typename Container::allocator_type>>::value,
		"only strings and vectors of ((un)signed) CharT allowed");

	auto const start_pos = in.tellg();
	if(std::streamsize(-1) == start_pos)
	{
		throw std::ios_base::failure{"error"};
	};

	if(!in.ignore(std::numeric_limits<std::streamsize>::max()))
	{
		throw std::ios_base::failure{"error"};
	};
	auto const char_count = in.gcount();

	if(!in.seekg(start_pos))
	{
		throw std::ios_base::failure{"error"};
	};

	auto container = Container(std::move(alloc));
	container.resize(static_cast<std::size_t>(char_count));

	if(!container.empty())
	{
		if(!in.read(reinterpret_cast<CharT*>(&container[0]), char_count))
		{
			throw std::ios_base::failure{"error"};
		};
	}

	return container;
}
} // namespace detail
using byte_array_t = std::vector<uint8_t>;

byte_array_t read_stream(std::istream& stream)
{
	return detail::read_stream_into_container<byte_array_t>(stream);
}

using namespace std::chrono_literals;

struct test_info
{
	std::string file;
	audio::sound_info expected;
};

void add_expected_info(std::vector<test_info>& infos, const std::string& file, uint32_t sample_rate,
			  uint8_t bytes_per_sample, uint8_t channels)
{
    test_info info;
    info.file = file;
    info.expected.sample_rate = sample_rate;
    info.expected.bytes_per_sample = bytes_per_sample;
    info.expected.channels = channels;
    infos.emplace_back(info);
}

int main()
{
	audio::set_info_logger([](const std::string& msg) { std::cout << msg << std::endl; });
	audio::set_error_logger([](const std::string& msg) { std::cout << msg << std::endl; });
	try
	{
		// initialize the audio device
		audio::device device;

		std::vector<test_info> infos;
        add_expected_info(infos, DATA "pcm0822m.wav", 22050, 1, 1);
        add_expected_info(infos, DATA "pcm1622m.wav", 22050, 2, 1);
        add_expected_info(infos, DATA "pcm0822s.wav", 22050, 1, 2);
        add_expected_info(infos, DATA "pcm1622s.wav", 22050, 2, 2);
        add_expected_info(infos, DATA "pcm0844m.wav", 44100, 1, 1);
        add_expected_info(infos, DATA "pcm1644m.wav", 44100, 2, 1);
        add_expected_info(infos, DATA "pcm0844s.wav", 44100, 1, 2);
        add_expected_info(infos, DATA "pcm1644s.wav", 44100, 2, 2);

        // ogg loader will force 2 bytes per sample
        add_expected_info(infos, DATA "pcm0822m.ogg", 22050, 2, 1);
        add_expected_info(infos, DATA "pcm1622m.ogg", 22050, 2, 1);
        add_expected_info(infos, DATA "pcm0822s.ogg", 22050, 2, 2);
        add_expected_info(infos, DATA "pcm1622s.ogg", 22050, 2, 2);
        add_expected_info(infos, DATA "pcm0844m.ogg", 44100, 2, 1);
        add_expected_info(infos, DATA "pcm1644m.ogg", 44100, 2, 1);
        add_expected_info(infos, DATA "pcm0844s.ogg", 44100, 2, 2);
        add_expected_info(infos, DATA "pcm1644s.ogg", 44100, 2, 2);

		std::vector<audio::sound_data> loaded_sounds;

		for(const auto& info : infos)
		{
			std::ifstream stream(info.file, std::ios::in | std::ios::binary);

			if(!stream.is_open())
			{
				audio::log_error("Failed to load file " + info.file);
				continue;
			}

			auto buffer = read_stream(stream);

			// Try to load the sound data
			std::string err;
			audio::sound_data data;
			bool success = false;
			if(!success)
			{
				success |= audio::load_wav_from_memory(buffer.data(), buffer.size(), data, err);
			}
			if(!success)
			{
				success |= audio::load_ogg_from_memory(buffer.data(), buffer.size(), data, err);
			}
			if(!success)
			{
				audio::log_error("Failed to load sound data");
				continue;
			}

			if(info.expected.bytes_per_sample != data.info.bytes_per_sample ||

			   info.expected.sample_rate != data.info.sample_rate ||

			   info.expected.channels != data.info.channels)
			{
				audio::log_error(info.file);
				audio::log_error("loaded with wrong expectations");
				continue;
			}
			audio::log_info("------------------------------------------");
			audio::log_info("loaded");
			audio::log_info("bytes_per_sample : " + std::to_string(data.info.bytes_per_sample));
			audio::log_info("sample_rate : " + std::to_string(data.info.sample_rate));
			audio::log_info("channels : " + std::to_string(data.info.channels));

			loaded_sounds.emplace_back(std::move(data));
		}
        audio::log_info("------------------------------------------");

		for(auto& data : loaded_sounds)
		{
			audio::sound sound(std::move(data));
			audio::source source;

			source.bind(sound);
			source.play();
			while(source.is_playing())
			{
				// source.update_stream();
				std::this_thread::sleep_for(1667us);
			}
		}
	}
	catch(const audio::exception& e)
	{
		audio::log_error(e.what());
	}

	return 0;
}
