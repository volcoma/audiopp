#pragma once

#include "sound.h"
#include "types.h"

namespace audio
{
namespace detail
{
class source_impl;
}

//-----------------------------------------------------------------------------
/// Emitter of a sound. It can control 3d spatial and emitter properties like
/// 3d position, 3d orientation, pitch, volume etc.
//-----------------------------------------------------------------------------
class source
{
public:
	source();
	~source();
	source(source&& rhs) noexcept;
	source& operator=(source&& rhs) noexcept;

	source(const source& rhs) = delete;
	source& operator=(const source& rhs) = delete;

	//-----------------------------------------------------------------------------
	/// Specifies the buffer to provide sound samples.
	//-----------------------------------------------------------------------------
	void bind(const sound& snd);

	//-----------------------------------------------------------------------------
	/// Checks whether this source has a sound binded to it.
	//-----------------------------------------------------------------------------
	bool has_binded_sound() const;

	//-----------------------------------------------------------------------------
	/// Specifies whether source is looping.
	//-----------------------------------------------------------------------------
	void set_loop(bool on);

	//-----------------------------------------------------------------------------
	/// A value of 1.0 means unattenuated. Each division by 2 equals an attenuation
	/// of about -6dB. Each multiplicaton by 2 equals an amplification of about +6dB.
	/// A value of 0.0 is meaningless with respect to a logarithmic scale; it is
	/// silent.
	//-----------------------------------------------------------------------------
	void set_volume(float volume);

	//-----------------------------------------------------------------------------
	/// A multiplier for the frequency (sample rate) of the source's buffer.
	//-----------------------------------------------------------------------------
	void set_pitch(float pitch);

	//-----------------------------------------------------------------------------
	/// The source location in three dimensional space.
	//-----------------------------------------------------------------------------
	void set_position(const float3& position);

	//-----------------------------------------------------------------------------
	/// Specifies the current velocity in local space.
	//-----------------------------------------------------------------------------
	void set_velocity(const float3& velocity);

	//-----------------------------------------------------------------------------
	/// Effectively two three dimensional vectors. The first vector is the front (or
	/// "at") and the second is the top (or "up").
	//-----------------------------------------------------------------------------
	void set_orientation(const float3& direction, const float3& up);

	//-----------------------------------------------------------------------------
	/// Multiplier to exaggerate or diminish distance attenuation.
	//-----------------------------------------------------------------------------
	void set_volume_rolloff(float rolloff);

	//-----------------------------------------------------------------------------
	/// Min = The distance in units that no attenuation occurs.
	/// Max = The distance above which the source is not attenuated any further with a
	/// clamped distance model, or where attenuation reaches 0.0 gain for linear
	/// distance models with a default rolloff factor.
	//-----------------------------------------------------------------------------
	void set_distance(float mind, float maxd);

	//-----------------------------------------------------------------------------
	/// Sets the source buffer position, in seconds.
	//-----------------------------------------------------------------------------
	void set_playing_offset(sound_info::duration_t offset);

	//-----------------------------------------------------------------------------
	/// Gets the source buffer position, in seconds.
	//-----------------------------------------------------------------------------
	sound_info::duration_t get_playing_offset() const;

	//-----------------------------------------------------------------------------
	/// Gets the source buffer length, in seconds.
	//-----------------------------------------------------------------------------
	sound_info::duration_t get_playing_duration() const;

	//-----------------------------------------------------------------------------
	/// Play, replay, or resume a Source.
	//-----------------------------------------------------------------------------
	void play();

	//-----------------------------------------------------------------------------
	/// Stop a Source.
	//-----------------------------------------------------------------------------
	void stop();

	//-----------------------------------------------------------------------------
	/// Pause a Source.
	//-----------------------------------------------------------------------------
	void pause();

	//-----------------------------------------------------------------------------
	/// Checks whether a source is currently playing.
	//-----------------------------------------------------------------------------
	bool is_playing() const;

	//-----------------------------------------------------------------------------
	/// Checks whether a source is currently paused.
	//-----------------------------------------------------------------------------
	bool is_paused() const;

	//-----------------------------------------------------------------------------
	/// Checks whether a source is currently stopped.
	//-----------------------------------------------------------------------------
	bool is_stopped() const;

	//-----------------------------------------------------------------------------
	/// Checks whether a source is currently looping.
	//-----------------------------------------------------------------------------
	bool is_looping() const;

	//-----------------------------------------------------------------------------
	/// Checks whether a source is valid.
	//-----------------------------------------------------------------------------
	bool is_valid() const;

	//-----------------------------------------------------------------------------
	/// Updates the stream. Does nothing if there is no streaming
	//-----------------------------------------------------------------------------
	void update_stream();

	//-----------------------------------------------------------------------------
	/// Gets the bound sound uid or 0 if none exists
	//-----------------------------------------------------------------------------
	uintptr_t get_bound_sound_uid() const;

private:
	/// pimpl idiom
	std::unique_ptr<detail::source_impl> impl_;
};
} // namespace audio
