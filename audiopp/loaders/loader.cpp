#include "loader.h"
#include "../logger.h"
#include <fstream>

namespace audio
{
namespace detail
{

template <typename Container = std::string, typename CharT = char, typename Traits = std::char_traits<char>>
auto read_stream(std::basic_istream<CharT, Traits>& in, Container& container) -> bool
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
		return false;
	};

	if(!in.seekg(0, std::ios_base::end))
	{
		return false;
	};

	auto const end_pos = in.tellg();

	if(std::streamsize(-1) == end_pos)
	{
		return false;
	};

	auto const char_count = end_pos - start_pos;

	if(!in.seekg(start_pos))
	{
		return false;
	};

	container.resize(static_cast<std::size_t>(char_count));

	if(!container.empty())
	{
		if(!in.read(reinterpret_cast<CharT*>(&container[0]), char_count))
		{
			return false;
		};
	}

	return true;
}
} // namespace detail
using byte_array_t = std::vector<uint8_t>;

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

	return detail::read_stream(stream, buffer);
}

auto load_from_memory(const uint8_t* data, size_t size, sound_data& result, std::string& err) -> bool
{
	bool success = false;
	if(!success)
	{
		success = load_from_memory_wav(data, size, result, err);
	}
	if(!success)
	{
		success = load_from_memory_ogg(data, size, result, err);
	}
	if(!success)
	{
		success = load_from_memory_mp3(data, size, result, err);
	}
	if(!success)
	{
		success = load_from_memory_flac(data, size, result, err);
	}
	return success;
}

auto load_from_file_ogg(const std::string& path, sound_data& result, std::string& err) -> bool
{
	byte_array_t buffer;
	if(!load_file(path, buffer))
	{
		err = "Failed to load file : " + path;
		return false;
	}

	if(!load_from_memory_ogg(buffer.data(), buffer.size(), result, err))
	{
		return false;
	}

	result.info.id = path;
	return true;
}
auto load_from_file_wav(const std::string& path, sound_data& result, std::string& err) -> bool
{
	byte_array_t buffer;
	if(!load_file(path, buffer))
	{
		err = "Failed to load file : " + path;
		return false;
	}

	if(!load_from_memory_wav(buffer.data(), buffer.size(), result, err))
	{
		return false;
	}

	result.info.id = path;
	return true;
}

auto load_from_file_mp3(const std::string& path, sound_data& result, std::string& err) -> bool
{
	byte_array_t buffer;
	if(!load_file(path, buffer))
	{
		err = "Failed to load file : " + path;
		return false;
	}

	if(!load_from_memory_mp3(buffer.data(), buffer.size(), result, err))
	{
		return false;
	}

	result.info.id = path;
	return true;
}

auto load_from_file_flac(const std::string& path, sound_data& result, std::string& err) -> bool
{
	byte_array_t buffer;
	if(!load_file(path, buffer))
	{
		err = "Failed to load file : " + path;
		return false;
	}

	if(!load_from_memory_flac(buffer.data(), buffer.size(), result, err))
	{
		return false;
	}

	result.info.id = path;
	return true;
}

auto load_from_file(const std::string& path, sound_data& result, std::string& err) -> bool
{
	auto ext = get_extension(path);

	if(ext == "wav" || ext == "wave")
	{
		return load_from_file_wav(path, result, err);
	}
	else if(ext == "ogg")
	{
		return load_from_file_ogg(path, result, err);
	}
	else if(ext == "flac")
	{
		return load_from_file_flac(path, result, err);
	}
	else if(ext == "mp3")
	{
		return load_from_file_mp3(path, result, err);
	}

	err = "Unsupported audio file format : " + ext;
	return false;
}

} // namespace audio
