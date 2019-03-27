#include "audiopp/library.h"
#include "audiopp/loaders/loader.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

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

void check_conversions(audio::sound_data& data)
{
	auto og = data.data;
	data.convert_to_opposite();
	data.convert_to_opposite();
	std::vector<uint8_t> diff;
	std::set_difference(og.begin(), og.end(), data.data.begin(), data.data.end(),
						std::inserter(diff, diff.begin()));
	if(!diff.empty())
	{
		audio::log_info("different after convert");
	}
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
		add_expected_info(infos, DATA "17.wav", 44100, 2, 1);
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
			// Try to load the sound data
			std::string err;
			audio::sound_data data;
			if(!audio::load_from_file(info.file, data, err))
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

			// check_conversions(data);

			loaded_sounds.emplace_back(std::move(data));
		}
		audio::log_info("------------------------------------------");
		audio::log_info("Playing sounds");

		for(auto& data : loaded_sounds)
		{
			audio::log_info("------------------------------------------");
			audio::log_info("playing");
			audio::log_info("bytes_per_sample : " + std::to_string(data.info.bytes_per_sample));
			audio::log_info("sample_rate : " + std::to_string(data.info.sample_rate));
			audio::log_info("channels : " + std::to_string(data.info.channels));

			audio::sound sound(std::move(data), true);
			audio::source source;

			source.bind(sound);
			source.play();
			while(source.is_playing())
			{
				source.update_stream();
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
