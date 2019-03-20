#include "sound_impl.h"
#include "../logger.h"
#include "check.h"
#include "source_impl.h"
#include <algorithm>

namespace audio
{
namespace detail
{

namespace detail
{
static ALenum get_format_for_channels(std::uint32_t channels, std::uint32_t bytes_per_sample)
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

static const size_t CHUNK_SIZE = 64 * 1024; // size of buffer if streaming

sound_impl::sound_impl(std::vector<std::uint8_t>&& buffer, const sound_info& info, bool stream /*= false*/)
	: buf_(std::move(buffer))
	, buf_info_(info)
{
	if(buf_.empty())
	{
		return;
	}

	buf_size_ = buf_.size() - buf_offset_;

	if(stream)
	{
		load_buffer();
	}
	else
	{
		load_buffer(buf_.size());
	}
}

bool sound_impl::load_buffer()
{
	return load_buffer(CHUNK_SIZE);
}

bool sound_impl::load_buffer(size_t chunk_size)
{
	if(buf_.empty())
	{
		return false;
	}

	ALenum format = detail::get_format_for_channels(buf_info_.channels, buf_info_.bytes_per_sample);

	size_t size = std::min(buf_.size() - buf_offset_, chunk_size);

	native_handle_type h = 0;
	al_check(alGenBuffers(1, &h));
	al_check(
		alBufferData(h, format, buf_.data() + buf_offset_, ALsizei(size), ALsizei(buf_info_.sample_rate)));
	handles_.push_back(h);

	buf_offset_ += size;

	{
		std::lock_guard<std::mutex> lock(mutex_);
		for(auto source : bound_to_sources_)
		{
			// enqueue the newly created buffer
			source->enqueue_buffers(&handles_.back(), 1);
		}
	}

	if(buf_offset_ == buf_.size())
	{
		// force deallocate
		std::vector<std::uint8_t>().swap(buf_);
	}

	return true;
}

sound_impl::sound_impl() = default;

sound_impl::~sound_impl()
{
	unbind_from_all_sources();

	if(!handles_.empty())
	{
		al_check(alDeleteBuffers(ALsizei(handles_.size()), handles_.data()));
	}
}

bool sound_impl::is_valid() const
{
	return !handles_.empty();
}

const std::vector<sound_impl::native_handle_type>& sound_impl::native_handles() const
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
