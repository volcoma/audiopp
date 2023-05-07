#pragma once

#include "effects/effect.h"
#include "sound.h"
#include "types.h"

#include <unordered_set>

namespace audio
{

namespace detail
{
class source_impl;
class builtin_effect_impl;
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
    /// Unbinds the currently bound sound, making the source available
    /// to bind other sounds.
    //-----------------------------------------------------------------------------
    void unbind();

    //-----------------------------------------------------------------------------
    /// Checks whether this source has a sound binded to it.
    //-----------------------------------------------------------------------------
    auto has_bound_sound() const -> bool;

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
    void set_volume(float volume) const;

    //-----------------------------------------------------------------------------
    /// A multiplier for the frequency (sample rate) of the source's buffer.
    //-----------------------------------------------------------------------------
    void set_pitch(float pitch) const;

    //-----------------------------------------------------------------------------
    /// The source location in three dimensional space.
    //-----------------------------------------------------------------------------
    void set_position(const float3& position) const;

    //-----------------------------------------------------------------------------
    /// Specifies the current velocity in local space.
    //-----------------------------------------------------------------------------
    void set_velocity(const float3& velocity) const;

    //-----------------------------------------------------------------------------
    /// Effectively two three dimensional vectors. The first vector is the front (or
    /// "at") and the second is the top (or "up").
    //-----------------------------------------------------------------------------
    void set_orientation(const float3& direction, const float3& up) const;

    //-----------------------------------------------------------------------------
    /// Multiplier to exaggerate or diminish distance attenuation.
    //-----------------------------------------------------------------------------
    void set_volume_rolloff(float rolloff) const;

    //-----------------------------------------------------------------------------
    /// Min = The distance in units that no attenuation occurs.
    /// Max = The distance above which the source is not attenuated any further with a
    /// clamped distance model, or where attenuation reaches 0.0 gain for linear
    /// distance models with a default rolloff factor.
    //-----------------------------------------------------------------------------
    void set_distance(float mind, float maxd) const;

    //-----------------------------------------------------------------------------
    /// Sets the source buffer position, in seconds.
    //-----------------------------------------------------------------------------
    void set_playback_position(duration_t offset);

    //-----------------------------------------------------------------------------
    /// Gets the source buffer position, in seconds.
    //-----------------------------------------------------------------------------
    auto get_playback_position() const -> duration_t;

    //-----------------------------------------------------------------------------
    /// Gets the source buffer length, in seconds.
    //-----------------------------------------------------------------------------
    auto get_playback_duration() const -> duration_t;

    //-----------------------------------------------------------------------------
    /// Gets the source volume in range [0, 1]
    //-----------------------------------------------------------------------------
    auto get_volume() const -> float;

    //-----------------------------------------------------------------------------
    /// Gets the source multiplier for the frequency
    /// (sample rate) of the source's buffer.
    //-----------------------------------------------------------------------------
    auto get_pitch() const -> float;

    //-----------------------------------------------------------------------------
    /// Play or replay a Source.
    //-----------------------------------------------------------------------------
    void play();

    //-----------------------------------------------------------------------------
    /// Stop a Source.
    //-----------------------------------------------------------------------------
    void stop();

    //-----------------------------------------------------------------------------
    /// Stop a Source immediately without notifying the effects.
    //-----------------------------------------------------------------------------
    void force_stop();

    //-----------------------------------------------------------------------------
    /// Pause a Source.
    //-----------------------------------------------------------------------------
    void pause();

    //-----------------------------------------------------------------------------
    /// Resumes a Source.
    //-----------------------------------------------------------------------------
    void resume();

    //-----------------------------------------------------------------------------
    /// Mutes a Source. get_volume/set_volume will continue to get/set the values.
    //-----------------------------------------------------------------------------
    void mute();

    //-----------------------------------------------------------------------------
    /// Unmutes a Source.
    //-----------------------------------------------------------------------------
    void unmute();

    //-----------------------------------------------------------------------------
    /// Checks whether a source is currently muted.
    //-----------------------------------------------------------------------------
    auto is_muted() const -> bool;

    //-----------------------------------------------------------------------------
    /// Checks whether a source is currently playing.
    //-----------------------------------------------------------------------------
    auto is_playing() const -> bool;

    //-----------------------------------------------------------------------------
    /// Checks whether a source is currently paused.
    //-----------------------------------------------------------------------------
    auto is_paused() const -> bool;

    //-----------------------------------------------------------------------------
    /// Checks whether a source is currently stopped.
    //-----------------------------------------------------------------------------
    auto is_stopped() const -> bool;

    //-----------------------------------------------------------------------------
    /// Checks whether a source is currently looping.
    //-----------------------------------------------------------------------------
    auto is_looping() const -> bool;

    //-----------------------------------------------------------------------------
    /// Checks whether a source is valid.
    //-----------------------------------------------------------------------------
    auto is_valid() const -> bool;

    //-----------------------------------------------------------------------------
    /// Checks whether a source is waiting for
    /// start/stop/pause/resume operation to complete.
    //-----------------------------------------------------------------------------
    auto is_waiting() const -> bool;

    //-----------------------------------------------------------------------------
    /// Checks if it's bound to the effect.
    //-----------------------------------------------------------------------------
    auto has_bound(effect& e) const -> bool;

    //-----------------------------------------------------------------------------
    /// Checks if it's bound to any effect.
    //-----------------------------------------------------------------------------
    auto has_bound_effect() const -> bool;

    //-----------------------------------------------------------------------------
    /// Updates the source's, e.g. the attached effects.
    /// Updates the sound stream.
    //-----------------------------------------------------------------------------
    void update(duration_t dt);

    //-----------------------------------------------------------------------------
    /// Gets the bound sound uid or 0 if none exists
    //-----------------------------------------------------------------------------
    auto get_bound_sound_uid() const -> uintptr_t;

    //-----------------------------------------------------------------------------
    /// Gets the source uid.
    //-----------------------------------------------------------------------------
    auto get_uid() const -> uintptr_t;

    //-----------------------------------------------------------------------------
    /// Gets the bound sound's info.
    /// If there is no bound sound returns an empty info.
    //-----------------------------------------------------------------------------
    auto get_bound_sound_info() const -> const sound_info&;

private:
    friend class detail::builtin_effect_impl;
    friend class effect;

    void move(source&& rhs) noexcept;

    void bind_effect(effect* effect) noexcept;
    void unbind_effect(effect* effect) noexcept;
    void change_effect_address(effect* from, effect* to) noexcept;
    void remove_dead_effect(effect* e) noexcept;

    void request(void (detail::source_impl::*request)() const,
                 void (effect::*effect_request)(source&, const effect::callback&)) noexcept;

    template<typename F>
    void bound_effects_visitor(F&& func) noexcept(noexcept(std::forward<F>(func)(std::declval<effect*>())));

    bool has_pending_request_{};
    bool is_iterating_bound_effects_{};
    size_t ready_effects_count_{};
    std::unordered_set<effect*> bound_effects_;

    /// pimpl idiom
    std::unique_ptr<detail::source_impl> impl_;
};
} // namespace audio
