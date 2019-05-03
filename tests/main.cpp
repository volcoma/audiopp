#include <audiopp/library.h>
#include <audiopp/loaders/loader.h>
#include <suitepp/suitepp.hpp>

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
	audio::set_trace_logger([](const std::string& msg) { std::cout << msg << std::endl; });
	audio::set_error_logger([](const std::string& msg) { std::cout << "[error] " << msg << std::endl; });

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

	suitepp::test("device init", [&] {
		audio::device::print_devices();
		// initialize the audio device
		EXPECT_NOTHROWS(audio::device device);
	});

	for(const auto& expected : infos)
	{
		suitepp::test("loading " + expected.id, [&] {
			std::string err;
			audio::sound_data data;
			EXPECT(audio::load_from_file(expected.id, data, err));

			EXPECT(data.info.id.empty() == false);
			EXPECT(data.info.bits_per_sample == expected.bits_per_sample);
			EXPECT(data.info.sample_rate == expected.sample_rate);
			EXPECT(data.info.channels == expected.channels);
			EXPECT(data.info.frames != 0u);
			EXPECT(data.info.duration.count() != 0.0);

			if(err.empty())
			{
				audio::info() << to_string(data.info);
				loaded_sounds.emplace_back(std::move(data));
			}
		});
	}

//	audio::device device;
//	for(auto& data : loaded_sounds)
//	{
//		suitepp::test("playback " + data.info.id, [&] {
//			auto code = [&]() {
//				// creating large internal buffer and uploading it at once
//				// can be slow so we can stream it in chunks if we want to
//				bool stream = false;
//				audio::sound sound(std::move(data), stream);

//				audio::source source;
//				source.bind(sound);
//				source.play();

//				while(source.is_playing())
//				{
//					std::this_thread::sleep_for(16067us);

//					// you can also append more data to the sound at any time
//					// std::vector<uint8_t> some_next_chunk = ;
//					// sound.append_chunk(std::move(some_next_chunk));

//					source.update_stream();
//				}
//			};

//			EXPECT_NOTHROWS(code());
//		});
//	}
	return 0;
}
catch(const audio::exception& e)
{
	audio::error() << e.what();
}
