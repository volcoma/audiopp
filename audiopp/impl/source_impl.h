#pragma once

#include "../types.h"
#include "al.h"
#include <cstdint>
#include <mutex>
#include <vector>

namespace audio
{
namespace detail
{
class sound_impl;

class source_impl
{
public:
	using native_handle_type = ALuint;
	source_impl();
	~source_impl();

	source_impl(source_impl&& rhs) = delete;
	source_impl& operator=(source_impl&& rhs) = delete;
	source_impl(const source_impl& rhs) = delete;
	source_impl& operator=(const source_impl& rhs) = delete;

	bool create();
	bool bind(sound_impl* sound);
	bool has_binded_sound() const;
	void unbind();
	void purge();

	void set_loop(bool on) const;
	void set_volume(float volume) const;
	void set_pitch(float pitch) const;
	void set_position(const float3& position) const;
	void set_velocity(const float3& velocity) const;
	void set_orientation(const float3& direction, const float3& up) const;

	void set_volume_rolloff(float rolloff) const;
	void set_distance(float mind, float maxd) const;
	void set_playing_offset(float seconds);
	float get_playing_offset() const;
	float get_playing_duration() const;

	void play() const;
	void stop() const;
	void pause() const;
	bool is_playing() const;
	bool is_paused() const;
	bool is_stopped() const;
	bool is_bound() const;
	bool is_valid() const;
	bool is_looping() const;

	void update_stream();
	void enqueue_buffers(const native_handle_type* handles, size_t count) const;

	native_handle_type native_handle() const;

	uintptr_t get_bound_sound_uid() const;

private:
	void bind_sound(sound_impl* sound);
	void unbind_sound();

	native_handle_type handle_ = 0;

	/// non owning
	sound_impl* bound_sound_ = nullptr;
};
} // namespace detail
} // namespace audio
