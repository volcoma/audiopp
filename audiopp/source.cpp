#include "source.h"
#include "impl/source_impl.h"
#include <cassert>
#include "logger.h"

namespace audio
{
source::source()
    : impl_(std::make_unique<detail::source_impl>())
{
}

source::~source()
{
    bound_effects_visitor([this](effect* e)
    {
        e->remove_dead_source(*this);
    });
}

source::source(source&& rhs) noexcept
{
    move(std::move(rhs));
}

source& source::operator=(source&& rhs) noexcept
{
    if (this != &rhs)
    {
        bound_effects_visitor([this](effect* e)
        {
            e->remove_dead_source(*this);
        });
        move(std::move(rhs));
    }

    return *this;
}

void source::play()
{
    if (is_waiting())
    {
        return;
    }
    request(&detail::source_impl::play, &effect::request_play);
}

void source::stop()
{
    if (is_waiting())
    {
        return;
    }
    request(&detail::source_impl::stop, &effect::request_stop);
}

void source::force_stop()
{
    if(is_valid())
    {
        bound_effects_visitor([this](effect* e)
        {
            e->force_stop(*this);
        });
        assert(!is_waiting());
        impl_->stop();
    }
}

void source::pause()
{
    if (is_waiting())
    {
        return;
    }

    request(&detail::source_impl::pause, &effect::request_pause);
}

void source::resume()
{
    if (is_waiting())
    {
        return;
    }

    request(&detail::source_impl::play, &effect::request_resume);
}

void source::mute()
{
    if(is_valid())
    {
        impl_->mute();
    }
}

void source::unmute()
{
    if(is_valid())
    {
        impl_->unmute();
    }
}

auto source::is_muted() const -> bool
{
    if(is_valid())
    {
        return impl_->is_muted();
    }
    return false;
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

auto source::is_waiting() const -> bool
{
    return has_pending_request_;
}

void source::set_loop(bool on)
{
    if(is_valid())
    {
        bound_effects_visitor([this, on](effect* e)
        {
            e->loop_state_changes(*this, on);
        });
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

void source::set_playback_position(duration_t offset)
{
    if(is_valid())
    {
        impl_->set_playback_position(float(offset.count()));

        bound_effects_visitor([this](effect* e)
        {
            e->playback_position_changed(*this);
        });
    }
}

auto source::get_playback_position() const -> duration_t
{
    if(is_valid())
    {
        return duration_t(impl_->get_playback_position());
    }
    return duration_t::zero();
}

auto source::get_playback_duration() const -> duration_t
{
    if(is_valid())
    {
        return duration_t(impl_->get_playback_duration());
    }
    return duration_t::zero();
}

auto source::get_volume() const -> float
{
    return is_valid() ? impl_->get_volume() : 0.f;
}

auto source::get_pitch() const -> float
{
    return is_valid() ? impl_->get_pitch() : 0.f;
}

auto source::is_valid() const -> bool
{
    return impl_ && impl_->is_valid();
}

auto source::has_bound(effect& e) const -> bool
{
    return bound_effects_.find(&e) != bound_effects_.end();
}

auto source::has_bound_effect() const -> bool
{
    return !bound_effects_.empty();
}

void source::update(duration_t dt)
{
    if (is_valid())
    {
        impl_->update_stream();

        bound_effects_visitor([this, dt](effect* e)
        {
            e->update(*this, dt);
        });
    }
}

auto source::get_bound_sound_uid() const -> uintptr_t
{
    return impl_ ? impl_->get_bound_sound_uid() : 0;
}

auto source::get_uid() const -> uintptr_t
{
    return reinterpret_cast<uintptr_t>(impl_.get());
}

auto source::get_bound_sound_info() const -> const sound_info&
{
    static const sound_info empty{};
    return is_valid() ? impl_->get_bound_sound_info() : empty;
}

void source::bind(const sound& snd)
{
    if(is_valid())
    {
        if(impl_->bind(snd.impl_.get()))
        {
            bound_effects_visitor([this](effect* e)
            {
                e->sound_changed(*this);
            });
        }
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

void source::move(source&& rhs) noexcept
{
    rhs.bound_effects_visitor([this, rhs_addr = &rhs](effect* e)
    {
        e->change_source_address(rhs_addr, this);
    });

    impl_ = std::move(rhs.impl_);

    assert(!is_iterating_bound_effects_ && !rhs.is_iterating_bound_effects_);
    bound_effects_ = std::move(rhs.bound_effects_);
    has_pending_request_ = rhs.has_pending_request_;
    ready_effects_count_ = rhs.ready_effects_count_;
    is_iterating_bound_effects_ = rhs.is_iterating_bound_effects_;
}

void source::bind_effect(effect* e) noexcept
{
    assert(e);
    assert(!is_iterating_bound_effects_);
    bound_effects_.insert(e);
}

void source::unbind_effect(effect* e) noexcept
{
    assert(!is_iterating_bound_effects_);
    bound_effects_.erase(e);
}

void source::change_effect_address(effect* from, effect* to) noexcept
{
    assert(from && to);
    auto it = bound_effects_.find(from);
    if (it != bound_effects_.end())
    {
        assert(!is_iterating_bound_effects_);
        bound_effects_.erase(it);
        assert(bound_effects_.find(to) == bound_effects_.end());
        bound_effects_.insert(to);
    }
}

void source::remove_dead_effect(effect* e) noexcept
{
    assert(!is_iterating_bound_effects_);
    bound_effects_.erase(e);
}

void source::request(void (detail::source_impl::*request)() const,
                     void (effect::*effect_request)(source &, const effect::callback &)) noexcept
{
    assert(request && effect_request);
    if (has_pending_request_)
    {
        audio::error() << "Calling play/stop/pause/resume on source which is already waiting for such operation to finish";
        return;
    }

    if(is_valid())
    {
        if (bound_effects_.empty())
        {
            (impl_.get()->*request)();
            return;
        }

        ready_effects_count_ = 0;
        has_pending_request_ = true;

        auto ready = [request, effects_count = bound_effects_.size()](source& src) {
            // do not capture 'this'
            // because it could be called later and the source might have been moved
            ++src.ready_effects_count_;
            if (src.ready_effects_count_ >= effects_count)
            {
                if (src.is_valid())
                {
                    (src.impl_.get()->*request)();
                }

                src.has_pending_request_ = false;
            }
        };

        bound_effects_visitor([this, ready, effect_request](effect* e)
        {
            (e->*effect_request)(*this, ready);
        });
    }
}

template<typename F>
void source::bound_effects_visitor(F&& func) noexcept(noexcept(std::forward<F>(func)(std::declval<effect*>())))
{
    assert(!is_iterating_bound_effects_);
    is_iterating_bound_effects_ = true;
    for (auto& effect : bound_effects_)
    {
        std::forward<F>(func)(effect);
    }
    is_iterating_bound_effects_ = false;
}

} // namespace audio
