#pragma once

#include "effects/builtin_effect_impl.h"
#include "../types.h"
#include "../sound_info.h"
#include "al.h"
#include <cstdint>
#include <unordered_map>
#include <set>

namespace audio
{

class effect;

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

    auto bind(sound_impl* sound) -> bool;
    auto has_bound_sound() const -> bool;
    void unbind();

    void set_loop(bool on) const;
    void set_volume(float volume) const;
    void set_pitch(float pitch) const;
    void set_position(const float3& position) const;
    void set_velocity(const float3& velocity) const;
    void set_orientation(const float3& direction, const float3& up) const;

    void set_volume_rolloff(float rolloff) const;
    void set_distance(float mind, float maxd) const;
    void set_playback_position(float seconds);
    auto get_volume() const -> float;
    auto get_pitch() const -> float;
    auto get_playback_position() const -> float;
    auto get_playback_duration() const -> float;

    void play() const;
    void stop() const;
    void pause() const;
    auto is_playing() const -> bool;
    auto is_paused() const -> bool;
    auto is_stopped() const -> bool;
    auto is_valid() const -> bool;
    auto is_looping() const -> bool;

    void mute() const;
    void unmute() const;
    auto is_muted() const -> bool;

    auto update_stream() -> bool;
    auto native_handle() const -> native_handle_type;
    auto get_bound_sound_uid() const -> uintptr_t;
    auto get_bound_sound_info() const -> const sound_info&;

    void enqueue_buffers(const native_handle_type* handles, size_t count) const;
    void unqueue_buffers() const;

    auto get_queued_buffers() const -> size_t;

    auto bind_source_aux_slot_to_effect(builtin_effect_impl* effect) -> bool;
    void unbind_source_aux_slot(builtin_effect_impl* effect);
private:
    void bind_sound(sound_impl* sound);
    void unbind_sound();

    using slot_type = ALint;

    auto request_slot() -> slot_type;

    std::set<slot_type> free_slots_;
    slot_type next_free_slot_{};
    std::unordered_map<builtin_effect_impl*, slot_type> effect_to_slot_;

    /// non owning
    sound_impl* bound_sound_ = nullptr;

    native_handle_type handle_ = 0;
    mutable float muted_volume_{1.0f};
    mutable bool muted_{};
};
} // namespace detail
} // namespace audio
