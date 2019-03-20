#include "loader.h"
#include "../logger.h"
#include <fstream>

namespace audio
{
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

std::string get_extension(const std::string& path)
{
	auto idx = path.rfind('.');

	if(idx != std::string::npos)
	{
		return path.substr(idx + 1);
	}
	return {};
}

bool load_file(const std::string& path, byte_array_t& buffer)
{
	std::ifstream stream(path, std::ios::in | std::ios::binary);

	if(!stream.is_open())
	{
		return false;
	}

	buffer = read_stream(stream);
	return true;
}

bool load_from_memory(const uint8_t* data, size_t size, sound_data& result, std::string& err)
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
	return success;
}

bool load_ogg_from_file(const std::string& path, sound_data& result, std::string& err)
{
	byte_array_t buffer;
	if(!load_file(path, buffer))
	{
		log_error("Failed to load file " + path);
		return false;
	}

	return load_ogg_from_memory(buffer.data(), buffer.size(), result, err);
}
bool load_wav_from_file(const std::string& path, sound_data& result, std::string& err)
{
	byte_array_t buffer;
	if(!load_file(path, buffer))
	{
		log_error("Failed to load file " + path);
		return false;
	}

	return load_wav_from_memory(buffer.data(), buffer.size(), result, err);
}

bool load_from_file(const std::string& path, sound_data& result, std::string& err)
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
	return false;
}

} // namespace audio
