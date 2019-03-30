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
static auto get_format(const sound_info& info) -> ALenum
{
	ALenum format = 0;
	switch(info.channels)
	{
		case 1:
		{
			switch(info.bytes_per_sample)
			{
				case 1:
					format = AL_FORMAT_MONO8;
					break;
				case 2:
					format = AL_FORMAT_MONO16;
					break;
				default:
					log_error("Unsupported bytes per sample count : " +
							  std::to_string(info.bytes_per_sample));
					break;
			}
		}
		break;

		case 2:
		{
			switch(info.bytes_per_sample)
			{
				case 1:
					format = AL_FORMAT_STEREO8;
					break;
				case 2:
					format = AL_FORMAT_STEREO16;
					break;
				default:
					log_error("Unsupported bytes per sample count : " +
							  std::to_string(info.bytes_per_sample));
					break;
			}
		}
		break;

		default:
			log_error("Unsupported channel count : " + std::to_string(info.channels));
			break;
	}

	return format;
}

} // namespace detail

using namespace std::chrono_literals;

sound_impl::sound_impl() = default;
sound_impl::sound_impl(std::vector<std::uint8_t>&& buffer, const sound_info& info, bool stream /*= false*/)
	: data_(std::move(buffer))
	, info_(info)
	, stream_(stream)
{
}

sound_impl::~sound_impl()
{
	unbind_from_all_sources();

	if(!handles_.empty())
	{
		al_check(alDeleteBuffers(ALsizei(handles_.size()), handles_.data()));
	}
}

auto sound_impl::upload_chunk() -> bool
{
	if(stream_)
	{
		return upload_chunk(get_byte_size_for(1s));
	}

	return upload_chunk(data_.size());
}

auto sound_impl::upload_chunk(size_t desired_size) -> bool
{
	if(data_.empty())
	{
		return false;
	}

	// get the actual chunk size depending on how much is left in the buffer
	auto chunk_size = std::min(data_.size() - data_offset_, desired_size);
	auto format = detail::get_format(info_);

	native_handle_type h{0};
	al_check(alGenBuffers(1, &h));
	al_check(alBufferData(h, format, data_.data() + data_offset_, ALsizei(chunk_size),
						  ALsizei(info_.sample_rate)));

	// add the handle for bookkeeping
	handles_.emplace_back(h);

	data_offset_ += chunk_size;

	log_info("loaded : " + std::to_string(chunk_size));

	{
		// enqueue the newly created buffer
		// to all the bound sources
		std::lock_guard<std::mutex> lock(mutex_);
		log_info("updating : " + std::to_string(bound_to_sources_.size()) + " sources");

		for(auto source : bound_to_sources_)
		{
			source->enqueue_buffers(&handles_.back(), 1);
		}
	}

	if(data_offset_ == data_.size())
	{
		// force deallocate and clear out the data
		std::vector<uint8_t>().swap(data_);
		data_offset_ = 0;
	}

	return true;
}

auto sound_impl::get_info() const -> const sound_info&
{
	return info_;
}

auto sound_impl::get_byte_size_for(sound_info::duration_t desired_duration) const -> size_t
{
	constexpr static const size_t min_chunk = 4096 * 4;
	return std::max<size_t>(
		min_chunk, static_cast<size_t>(desired_duration.count() *
									   double(info_.sample_rate * info_.channels * info_.bytes_per_sample)));
}

auto sound_impl::append_chunk(const std::vector<uint8_t>& data) -> bool
{
	if(data.empty())
	{
		return false;
	}
	auto offset = data_.size();
	data_.resize(data_.size() + data.size());
	std::memcpy(data_.data() + offset, data.data(), data.size());

	return true;
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
