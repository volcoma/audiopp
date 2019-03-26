#include "sound_impl.h"
#include "../logger.h"
#include "check.h"
#include "source_impl.h"
#include <algorithm>
#include <cstring>

namespace audio
{
namespace detail
{

namespace detail
{
static auto get_format_for_channels(std::uint32_t channels, std::uint32_t bytes_per_sample) -> ALenum
{
	ALenum format = 0;
	switch(channels)
	{
		case 1:
		{
			switch(bytes_per_sample)
			{
				case 1:
					format = AL_FORMAT_MONO8;
					break;
				case 2:
					format = AL_FORMAT_MONO16;
					break;
				default:
					log_error("Unsupported bytes per sample count : " + std::to_string(bytes_per_sample));
					break;
			}
		}
		break;

		case 2:
		{
			switch(bytes_per_sample)
			{
				case 1:
					format = AL_FORMAT_STEREO8;
					break;
				case 2:
					format = AL_FORMAT_STEREO16;
					break;
				default:
					log_error("Unsupported bytes per sample count : " + std::to_string(bytes_per_sample));
					break;
			}
		}
		break;

		default:
			log_error("Unsupported channel count : " + std::to_string(channels));
			break;
	}

	return format;
}
} // namespace detail

constexpr static const size_t CHUNK_SIZE = 64 * 1024; // size of buffer if streaming

sound_impl::sound_impl() = default;
sound_impl::sound_impl(std::vector<std::uint8_t>&& buffer, const sound_info& info, bool stream /*= false*/)
	: data_(std::move(buffer))
	, info_(info)
	, stream_(stream)
{
	if(data_.empty())
	{
		return;
	}

	load_chunk();
}

sound_impl::~sound_impl()
{
	unbind_from_all_sources();

	if(!handles_.empty())
	{
		al_check(alDeleteBuffers(ALsizei(handles_.size()), handles_.data()));
	}
}

auto sound_impl::load_chunk() -> bool
{
	if(stream_)
	{
		return load_chunk(CHUNK_SIZE);
	}

	return load_chunk(data_.size());
}

auto sound_impl::load_chunk(size_t chunk_size) -> bool
{
	if(data_.empty())
	{
		return false;
	}
	size_t actual_size = std::min(data_.size() - data_offset_, chunk_size);

	ALenum format = detail::get_format_for_channels(info_.channels, info_.bytes_per_sample);

	native_handle_type h = 0;
	al_check(alGenBuffers(1, &h));
	al_check(alBufferData(h, format, data_.data() + data_offset_, ALsizei(actual_size),
						  ALsizei(info_.sample_rate)));
	handles_.emplace_back(h);

	data_offset_ += actual_size;

	{
		std::lock_guard<std::mutex> lock(mutex_);
		for(auto source : bound_to_sources_)
		{
			// enqueue the newly created buffer
			source->enqueue_buffers(&handles_.back(), 1);
		}
	}

	if(data_offset_ == data_.size())
	{
		// force deallocate
		std::vector<uint8_t>().swap(data_);
		data_offset_ = 0;
	}

	return true;
}

auto sound_impl::get_info() const -> const sound_info&
{
	return info_;
}

void sound_impl::append_chunk(const std::vector<uint8_t>& data)
{
	auto offset = data_.size();
	data_.resize(data_.size() + data.size());
	std::memcpy(data_.data() + offset, data.data(), data.size());
}

auto sound_impl::is_valid() const -> bool
{
	return !handles_.empty();
}

auto sound_impl::native_handles() const -> const std::vector<native_handle_type>&
{
	return handles_;
}

void sound_impl::bind_to_source(source_impl* source)
{
	std::lock_guard<std::mutex> lock(mutex_);
	bound_to_sources_.push_back(source);
}

void sound_impl::unbind_from_source(source_impl* source)
{
	std::lock_guard<std::mutex> lock(mutex_);
	bound_to_sources_.erase(std::remove_if(std::begin(bound_to_sources_), std::end(bound_to_sources_),
										   [source](const auto& item) { return item == source; }),
							std::end(bound_to_sources_));
}

void sound_impl::unbind_from_all_sources()
{
	auto all_sources = [&]() {
		std::lock_guard<std::mutex> lock(mutex_);
		return std::move(bound_to_sources_);
	}();

	for(auto& source : all_sources)
	{
		if(source != nullptr)
		{
			source->unbind();
		}
	}
}

} // namespace detail
} // namespace audio
