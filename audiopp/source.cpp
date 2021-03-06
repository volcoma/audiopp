#include "source.h"
#include "impl/source_impl.h"

namespace audio
{
source::source()
	: impl_(std::make_unique<detail::source_impl>())
{
}

source::~source() = default;

source::source(source&&) noexcept = default;
source& source::operator=(source&&) noexcept = default;

void source::play()
{
	if(is_valid())
	{
		impl_->play();
	}
}

void source::stop()
{
	if(is_valid())
	{
		impl_->stop();
	}
}

void source::pause()
{
	if(is_valid())
	{
		impl_->pause();
	}
}

auto source::is_playing() const -> bool
{
	if(is_valid())
	{
		return impl_->is_playing();
	}
	return false;
}

auto source::is_paused() const -> bool
{
	if(is_valid())
	{
		return impl_->is_paused();
	}
	return false;
}

auto source::is_stopped() const -> bool
{
	if(is_valid())
	{
		return impl_->is_stopped();
	}
	return true;
}

auto source::is_looping() const -> bool
{
	if(is_valid())
	{
		return impl_->is_looping();
	}
	return false;
}

void source::set_loop(bool on) const
{
	if(is_valid())
	{
		impl_->set_loop(on);
	}
}

void source::set_volume(float volume) const
{
	if(is_valid())
	{
		impl_->set_volume(volume);
	}
}

/* pitch, speed stretching */
void source::set_pitch(float pitch) const
{
	// if pitch == 0.f pitch = 0.0001f;
	if(is_valid())
	{
		impl_->set_pitch(pitch);
	}
}

void source::set_position(const float3& position) const
{
	if(is_valid())
	{
		impl_->set_position(position);
	}
}

void source::set_velocity(const float3& velocity) const
{
	if(is_valid())
	{
		impl_->set_velocity(velocity);
	}
}

void source::set_orientation(const float3& direction, const float3& up) const
{
	if(is_valid())
	{
		impl_->set_orientation(direction, up);
	}
}

void source::set_volume_rolloff(float rolloff) const
{
	if(is_valid())
	{
		impl_->set_volume_rolloff(rolloff);
	}
}

void source::set_distance(float mind, float maxd) const
{
	if(is_valid())
	{
		impl_->set_distance(mind, maxd);
	}
}

void source::set_playback_position(sound_info::duration_t offset)
{
	if(is_valid())
	{
		impl_->set_playback_position(float(offset.count()));
	}
}

auto source::get_playback_position() const -> sound_info::duration_t
{
	if(is_valid())
	{
		return sound_info::duration_t(impl_->get_playback_position());
	}
	return sound_info::duration_t::zero();
}

auto source::get_playback_duration() const -> sound_info::duration_t
{
	if(is_valid())
	{
		return sound_info::duration_t(impl_->get_playback_duration());
	}
	return sound_info::duration_t::zero();
}

auto source::is_valid() const -> bool
{
	return impl_ && impl_->is_valid();
}

void source::update_stream()
{
	if(is_valid())
	{
		impl_->update_stream();
	}
}

auto source::get_bound_sound_uid() const -> uintptr_t
{
	return impl_ ? impl_->get_bound_sound_uid() : 0;
}

void source::bind(const sound& snd)
{
	if(is_valid())
	{
		impl_->bind(snd.impl_.get());
	}
}

void source::unbind()
{
	if(is_valid())
	{
		impl_->unbind();
	}
}

auto source::has_bound_sound() const -> bool
{
	if(is_valid())
	{
		return impl_->has_bound_sound();
	}

	return false;
}
} // namespace audio
