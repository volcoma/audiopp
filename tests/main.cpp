#include <audiopp/library.h>
#include <audiopp/loaders/loader.h>
#include <suitepp/suitepp.hpp>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <map>
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

	const std::string& data_path = DATA;

	const std::vector<std::pair<std::string, std::vector<std::string>>> formats = {

		{"wav", {"pcm08", "pcm16", "pcm24", "pcm32", "ulaw", "alaw", "sngl", "dbl", "ima", "ms"}},
		{"ogg", {"pcm08", "pcm16", "pcm24", "pcm32"}},
		{"mp3", {"pcm08", "pcm16", "pcm24", "pcm32"}},
		{"flac", {"pcm08", "pcm16", "pcm24", "pcm32"}}

	};

	const std::vector<std::pair<uint32_t, std::string>> sample_rates = {

		{8000u, "08"}, {11025u, "11"}, {22050u, "22"}, {44100u, "44"}

	};

	const std::vector<std::pair<uint8_t, std::string>> channels = {

		{uint8_t(1), "m"}, {uint8_t(2), "s"}

	};

	// all loaders will convert to 16 bits per sample
	const uint32_t bps = 16;

	for(const auto& format_entry : formats)
	{
		const auto& format = format_entry.first;
		const auto& sub_formats = format_entry.second;

		for(const auto& sub_format : sub_formats)
		{
			std::string entry_path;
			entry_path.append(data_path);
			entry_path.append(format);
			entry_path.append("/");
			entry_path.append(sub_format);

			for(const auto& rate_entry : sample_rates)
			{
				const auto& rate_num = rate_entry.first;
				const auto& rate = rate_entry.second;

				for(const auto& channel_entry : channels)
				{
					const auto& channel_num = channel_entry.first;
					const auto& channel = channel_entry.second;

					std::string path;
					path.append(entry_path);
					path.append(rate);
					path.append(channel);
					path.append(".");
					path.append(format);

					add_expected_info(infos, path, rate_num, bps, channel_num);
				}
			}
		}
	}

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
			audio::sound_data loaded;
			EXPECT(audio::load_from_file(expected.id, loaded, err));

			EXPECT(loaded.info.bits_per_sample == expected.bits_per_sample);
			EXPECT(loaded.info.sample_rate == expected.sample_rate);
			EXPECT(loaded.info.channels == expected.channels);

			if(err.empty())
			{
				// audio::info() << to_string(data.info);
				loaded_sounds.emplace_back(std::move(loaded));
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
