#include "source_impl.h"

#include "../exception.h"
#include "check.h"
#include "sound_impl.h"

namespace audio
{
namespace detail
{
source_impl::source_impl()
{
	if(!create())
	{
		throw audio::exception("Cannot create source.");
	}
}

source_impl::~source_impl()
{
	purge();
}

auto source_impl::create() -> bool
{
	if(handle_ != 0u)
	{
		return true;
	}

	al_check(alGenSources(1, &handle_));

	return is_valid();
}

auto source_impl::bind(sound_impl* sound) -> bool
{
	if(sound == nullptr)
	{
		return false;
	}

	unbind();

	bind_sound(sound);

	const auto& handles = sound->native_handles();

	al_check(alSourcei(handle_, AL_SOURCE_RELATIVE, AL_FALSE));
	al_check(alSourcei(handle_, AL_BUFFER, 0));

	enqueue_buffers(handles.data(), handles.size());

	return true;
}

auto source_impl::has_binded_sound() const -> bool
{
	ALint buffer = 0;
	al_check(alGetSourcei(handle_, AL_BUFFERS_QUEUED, &buffer));
	return buffer != 0;
}

void source_impl::unbind()
{
	if(!is_bound())
	{
		return;
	}
	stop();

	ALint queued = 0;
	al_check(alGetSourcei(handle_, AL_BUFFERS_QUEUED, &queued));
	while(queued--)
	{
		ALuint buffer = 0;
		al_check(alSourceUnqueueBuffers(handle_, 1, &buffer));
	}

	al_check(alSourcei(handle_, AL_BUFFER, 0));

	unbind_sound();
}

void source_impl::purge()
{
	if(handle_ == 0u)
	{
		return;
	}

	unbind();

	al_check(alDeleteSources(1, &handle_));

	handle_ = 0;
}

void source_impl::set_playing_offset(float seconds)
{
	// temporary load the whole sound here
	// until we figure out a good way to load until the position we need it
	while(update_stream())
		;

	al_check(alSourcef(handle_, AL_SEC_OFFSET, seconds));
}

auto source_impl::get_playing_offset() const -> float
{
	ALfloat seconds = 0.0f;
	al_check(alGetSourcef(handle_, AL_SEC_OFFSET, &seconds));
	return static_cast<float>(seconds);
}

auto source_impl::get_playing_duration() const -> float
{
	if(bound_sound_)
	{
		return float(bound_sound_->get_info().duration.count());
	}
	return 0.0f;
}

void source_impl::play() const
{
	al_check(alSourcePlay(handle_));
}

void source_impl::stop() const
{
	set_loop(false);
	al_check(alSourceStop(handle_));
}

void source_impl::pause() const
{
	al_check(alSourcePause(handle_));
}

auto source_impl::is_playing() const -> bool
{
	ALint state = AL_INITIAL;
	al_check(alGetSourcei(handle_, AL_SOURCE_STATE, &state));
	return (state == AL_PLAYING);
}

auto source_impl::is_paused() const -> bool
{
	ALint state = AL_INITIAL;
	al_check(alGetSourcei(handle_, AL_SOURCE_STATE, &state));
	return (state == AL_PAUSED);
}

auto source_impl::is_stopped() const -> bool
{
	ALint state = AL_INITIAL;
	al_check(alGetSourcei(handle_, AL_SOURCE_STATE, &state));
	return (state == AL_STOPPED);
}

auto source_impl::is_bound() const -> bool
{
	ALint buffer = 0;
	al_check(alGetSourcei(handle_, AL_BUFFER, &buffer));
	return (buffer != 0);
}

void source_impl::set_loop(bool on) const
{
	al_check(alSourcei(handle_, AL_LOOPING, on ? AL_TRUE : AL_FALSE));
}

void source_impl::set_volume(float volume) const
{
	al_check(alSourcef(handle_, AL_GAIN, volume));
}

/* pitch, speed stretching */
void source_impl::set_pitch(float pitch) const
{
	// if pitch == 0.f pitch = 0.0001f;
	al_check(alSourcef(handle_, AL_PITCH, pitch));
}

void source_impl::set_position(const float3& position) const
{
	al_check(alSourcefv(handle_, AL_POSITION, position.data()));
}

void source_impl::set_velocity(const float3& velocity) const
{
	al_check(alSourcefv(handle_, AL_VELOCITY, velocity.data()));
}

void source_impl::set_orientation(const float3& direction, const float3& up) const
{
	float orientation6[] = {-direction[0], -direction[1], -direction[2], up[0], up[1], up[2]};
	al_check(alSourcefv(handle_, AL_ORIENTATION, orientation6));
}

void source_impl::set_volume_rolloff(float rolloff) const
{
	al_check(alSourcef(handle_, AL_ROLLOFF_FACTOR, rolloff));
}

void source_impl::set_distance(float mind, float maxd) const
{

	// The distance that the source will be the loudest (if the listener is
	// closer, it won't be any louder than if they were at this distance)
	al_check(alSourcef(handle_, AL_REFERENCE_DISTANCE, mind));

	// The distance that the source will be the quietest (if the listener is
	// farther, it won't be any quieter than if they were at this distance)
	al_check(alSourcef(handle_, AL_MAX_DISTANCE, maxd));
}

auto source_impl::is_valid() const -> bool
{
	return handle_ != 0;
}

auto source_impl::is_looping() const -> bool
{
	ALint loop;
	al_check(alGetSourcei(handle_, AL_LOOPING, &loop));
	return loop != 0;
}

auto source_impl::update_stream() -> bool
{
	if(bound_sound_)
	{
		return bound_sound_->load_chunk();
	}

	return false;
}

auto source_impl::native_handle() const -> native_handle_type
{
	return handle_;
}

auto source_impl::get_bound_sound_uid() const -> uintptr_t
{
	return reinterpret_cast<uintptr_t>(bound_sound_);
}

void source_impl::enqueue_buffers(const native_handle_type* handles, size_t count) const
{
	al_check(alSourceQueueBuffers(handle_, ALsizei(count), handles));
}

void source_impl::bind_sound(sound_impl* sound)
{
	if(bound_sound_ == sound)
	{
		return;
	}

	bound_sound_ = sound;
	bound_sound_->bind_to_source(this);
}

void source_impl::unbind_sound()
{
	if(bound_sound_ != nullptr)
	{
		bound_sound_->unbind_from_source(this);
		bound_sound_ = nullptr;
	}
}
} // namespace detail
} // namespace audio
