#pragma once

#include "../sound.h"
#include "al.h"
#include <mutex>

namespace audio
{
namespace detail
{
class source_impl;

class sound_impl
{
public:
	using native_handle_type = ALuint;

	sound_impl();
	~sound_impl();
	sound_impl(std::vector<std::uint8_t>&& buffer, sound_info&& info, bool stream = false);

	sound_impl(sound_impl&& rhs) = delete;
	sound_impl& operator=(sound_impl&& rhs) = delete;
	sound_impl(const sound_impl& rhs) = delete;
	sound_impl& operator=(const sound_impl& rhs) = delete;

	auto is_valid() const -> bool;
	auto native_handles() const -> const std::vector<native_handle_type>&;
	auto upload_chunk() -> bool;
	auto append_chunk(std::vector<uint8_t>&& data) -> bool;
	auto get_info() const -> const sound_info&;
	auto get_byte_size_for(sound_info::duration_t desired_duration) const -> size_t;

private:
	friend class source_impl;

	auto upload_chunk(size_t desired_size) -> bool;
	void bind_to_source(source_impl* source);
	void unbind_from_source(source_impl* source);
	void unbind_from_all_sources();
	/// created buffer handles
	std::vector<native_handle_type> handles_;

	/// transient data valid until the audio is being streamed from memory
	std::vector<std::uint8_t> data_;
	/// offset into the data buffer to upload from
	size_t data_offset_{0};

	/// the sound info
	sound_info info_;
	/// openal doesn't let us destroy sounds that are
	/// binded, so we have to keep this bookkeeping
	std::mutex mutex_;
	std::vector<source_impl*> bound_to_sources_;

	bool stream_{};
};
} // namespace detail
} // namespace audio
