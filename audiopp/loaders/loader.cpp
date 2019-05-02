#include "loader.h"
#include "../logger.h"
#include <fstream>

namespace audio
{
namespace detail
{
template <typename Container = std::string, typename CharT = char, typename Traits = std::char_traits<char>>
auto read_stream_into_container(std::basic_istream<CharT, Traits>& in,
								typename Container::allocator_type alloc = {}) -> Container
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

	if(!in.seekg(0, std::ios_base::end))
	{
		throw std::ios_base::failure{"error"};
	};

	auto const end_pos = in.tellg();

	if(std::streamsize(-1) == end_pos)
	{
		throw std::ios_base::failure{"error"};
	};

	auto const char_count = end_pos - start_pos;

	if(!in.seekg(start_pos))
	{
		throw std::ios_base::failure{"error"};
	};

	Container container(std::move(alloc));
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

auto read_stream(std::istream& stream) -> byte_array_t
{
	return detail::read_stream_into_container<byte_array_t>(stream);
}

auto get_extension(const std::string& path) -> std::string
{
	auto idx = path.rfind('.');

	if(idx != std::string::npos)
	{
		return path.substr(idx + 1);
	}
	return {};
}

auto load_file(const std::string& path, byte_array_t& buffer) -> bool
{
	std::ifstream stream(path, std::ios::in | std::ios::binary);

	if(!stream.is_open())
	{
		return false;
	}

	buffer = read_stream(stream);
	return true;
}

auto load_from_memory(const uint8_t* data, size_t size, sound_data& result, std::string& err) -> bool
{
	bool success = false;
	if(!success)
	{
		success |= load_wav_from_memory(data, size, result, err);
	}
	if(!success)
	{
		success |= load_ogg_from_memory(data, size, result, err);
	}
	if(!success)
	{
		success |= load_mp3_from_memory(data, size, result, err);
	}
	if(!success)
	{
		success |= load_flac_from_memory(data, size, result, err);
	}
	return success;
}

auto load_ogg_from_file(const std::string& path, sound_data& result, std::string& err) -> bool
{
	err = {};

	byte_array_t buffer;
	if(!load_file(path, buffer))
	{
		err = "Failed to load file : " + path;
		return false;
	}

	if(!load_ogg_from_memory(buffer.data(), buffer.size(), result, err))
	{
		return false;
	}

	result.info.id = path;
	return true;
}
auto load_wav_from_file(const std::string& path, sound_data& result, std::string& err) -> bool
{
	err = {};

	byte_array_t buffer;
	if(!load_file(path, buffer))
	{
		err = "Failed to load file : " + path;
		return false;
	}

	if(!load_wav_from_memory(buffer.data(), buffer.size(), result, err))
	{
		return false;
	}

	result.info.id = path;
	return true;
}

auto load_mp3_from_file(const std::string& path, sound_data& result, std::string& err) -> bool
{
	err = {};

	byte_array_t buffer;
	if(!load_file(path, buffer))
	{
		err = "Failed to load file : " + path;
		return false;
	}

	if(!load_mp3_from_memory(buffer.data(), buffer.size(), result, err))
	{
		return false;
	}

	result.info.id = path;
	return true;
}

auto load_flac_from_file(const std::string& path, sound_data& result, std::string& err) -> bool
{
	err = {};

	byte_array_t buffer;
	if(!load_file(path, buffer))
	{
		err = "Failed to load file : " + path;
		return false;
	}

	if(!load_flac_from_memory(buffer.data(), buffer.size(), result, err))
	{
		return false;
	}

	result.info.id = path;

	return true;
}

auto load_from_file(const std::string& path, sound_data& result, std::string& err) -> bool
{
	auto ext = get_extension(path);
	if(ext == "wav")
	{
		return load_wav_from_file(path, result, err);
	}
	else if(ext == "ogg")
	{
		return load_ogg_from_file(path, result, err);
	}
	else if(ext == "mp3")
	{
		return load_mp3_from_file(path, result, err);
	}
	else if(ext == "flac")
	{
		return load_flac_from_file(path, result, err);
	}
	err = "Unsupported audio file format : " + ext;
	return false;
}

} // namespace audio
