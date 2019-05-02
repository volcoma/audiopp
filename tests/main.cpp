#include "audiopp/library.h"
#include "audiopp/loaders/loader.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

using namespace std::chrono_literals;

void add_expected_info(std::vector<audio::sound_info>& infos, const std::string& file, uint32_t sample_rate,
					   uint8_t bits_per_sample, uint8_t channels)
{
	audio::sound_info expected;
	expected.id = file;
	expected.sample_rate = sample_rate;
	expected.bits_per_sample = bits_per_sample;
	expected.channels = channels;
	infos.emplace_back(expected);
}

int main() try
{
	audio::set_info_logger([](const std::string& msg) { std::cout << msg << std::endl; });
	audio::set_error_logger([](const std::string& msg) { std::cout << msg << std::endl; });

	audio::info() << audio::device::enumerate_devices();

	// initialize the audio device
	audio::device device;

	std::vector<audio::sound_info> infos;

	// wav loader will force 2 bytes per sample
	add_expected_info(infos, DATA "pcm0822m.wav", 22050, 16, 1);
	add_expected_info(infos, DATA "pcm1622m.wav", 22050, 16, 1);
	add_expected_info(infos, DATA "pcm0822s.wav", 22050, 16, 2);
	add_expected_info(infos, DATA "pcm1622s.wav", 22050, 16, 2);
	add_expected_info(infos, DATA "pcm0844m.wav", 44100, 16, 1);
	add_expected_info(infos, DATA "pcm1644m.wav", 44100, 16, 1);
	add_expected_info(infos, DATA "pcm0844s.wav", 44100, 16, 2);
	add_expected_info(infos, DATA "pcm1644s.wav", 44100, 16, 2);

	// ogg loader will force 2 bytes per sample
	add_expected_info(infos, DATA "pcm0822m.ogg", 22050, 16, 1);
	add_expected_info(infos, DATA "pcm1622m.ogg", 22050, 16, 1);
	add_expected_info(infos, DATA "pcm0822s.ogg", 22050, 16, 2);
	add_expected_info(infos, DATA "pcm1622s.ogg", 22050, 16, 2);
	add_expected_info(infos, DATA "pcm0844m.ogg", 44100, 16, 1);
	add_expected_info(infos, DATA "pcm1644m.ogg", 44100, 16, 1);
	add_expected_info(infos, DATA "pcm0844s.ogg", 44100, 16, 2);
	add_expected_info(infos, DATA "pcm1644s.ogg", 44100, 16, 2);

	// mp3 loader will force 2 bytes per sample
	add_expected_info(infos, DATA "pcm0822m.mp3", 22050, 16, 1);
	add_expected_info(infos, DATA "pcm1622m.mp3", 22050, 16, 1);
	add_expected_info(infos, DATA "pcm0822s.mp3", 22050, 16, 2);
	add_expected_info(infos, DATA "pcm1622s.mp3", 22050, 16, 2);
	add_expected_info(infos, DATA "pcm0844m.mp3", 44100, 16, 1);
	add_expected_info(infos, DATA "pcm1644m.mp3", 44100, 16, 1);
	add_expected_info(infos, DATA "pcm0844s.mp3", 44100, 16, 2);
	add_expected_info(infos, DATA "pcm1644s.mp3", 44100, 16, 2);

	// flac loader will force 2 bytes per sample
	add_expected_info(infos, DATA "pcm0822m.flac", 22050, 16, 1);
	add_expected_info(infos, DATA "pcm1622m.flac", 22050, 16, 1);
	add_expected_info(infos, DATA "pcm0822s.flac", 22050, 16, 2);
	add_expected_info(infos, DATA "pcm1622s.flac", 22050, 16, 2);
	add_expected_info(infos, DATA "pcm0844m.flac", 44100, 16, 1);
	add_expected_info(infos, DATA "pcm1644m.flac", 44100, 16, 1);
	add_expected_info(infos, DATA "pcm0844s.flac", 44100, 16, 2);
	add_expected_info(infos, DATA "pcm1644s.flac", 44100, 16, 2);

	std::vector<audio::sound_data> loaded_sounds;

	for(const auto& expected : infos)
	{
		// Try to load the sound data
		std::string err;
		audio::sound_data data;
		if(!audio::load_from_file(expected.id, data, err))
		{
			audio::error() << "Failed to load sound : " << err;
			continue;
		}

		if(expected.bits_per_sample != data.info.bits_per_sample ||
		   expected.sample_rate != data.info.sample_rate || expected.channels != data.info.channels)
		{
			audio::error() << "[FAIL]";
			audio::error() << "expected : ";
			audio::error() << to_string(expected);
			audio::error() << "loaded : ";
			audio::error() << to_string(data.info);

			continue;
		}
		audio::info() << "------------------------------------------";
		audio::info() << "loaded";
		audio::info() << to_string(data.info);

		data.convert_to_opposite();
		data.convert_to_opposite();
		loaded_sounds.emplace_back(std::move(data));
	}
	audio::info() << "------------------------------------------";
	audio::info() << "Playing sounds";

	for(auto& data : loaded_sounds)
	{
		audio::info() << "------------------------------------------";
		audio::info() << "now playing";
		audio::info() << to_string(data.info);

		// creating large internal buffer and uploading it at once
		// can be slow so we can stream it in chunks if we want to
		bool stream = false;
		audio::sound sound(std::move(data), stream);

		audio::source source;
		source.bind(sound);
		source.play();

		while(source.is_playing())
		{
			std::this_thread::sleep_for(16067us);

			// you can also append more data to the sound at any time
			// std::vector<uint8_t> some_next_chunk = ;
			// sound.append_chunk(std::move(some_next_chunk));

			source.update_stream();
			audio::info() << source.get_playback_position().count();
			audio::info() << source.get_playback_position().count() / source.get_playback_duration().count();
		}
	}

	return 0;
}
catch(const audio::exception& e)
{
	audio::error() << e.what();
}
