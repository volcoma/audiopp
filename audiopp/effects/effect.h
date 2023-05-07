#pragma once

#include <functional>
#include <unordered_set>

#include "../types.h"

namespace audio
{

class source;

namespace detail
{
class source_impl;
}

class effect
{
public:
    using callback = std::function<void(source&)>;

    effect() noexcept = default;
    effect(const effect&) = delete;
    effect& operator=(const effect&) = delete;
    effect(effect&& rhs) noexcept;
    effect& operator=(effect&& rhs) noexcept;
    virtual ~effect();

    //-----------------------------------------------------------------------------
    /// Binds a source to the effect.
    //-----------------------------------------------------------------------------
    auto bind(source& src) noexcept -> bool;

    //-----------------------------------------------------------------------------
    /// Binds a source to the effect.
    //-----------------------------------------------------------------------------
    void unbind(source& src) noexcept;

    //-----------------------------------------------------------------------------
    /// Unbinds all bound sources.
    //-----------------------------------------------------------------------------
    void unbind_all() noexcept;

    //-----------------------------------------------------------------------------
    /// Checks whether the source is bound to the effect.
    //-----------------------------------------------------------------------------
    virtual auto has_bound(const source& src) const noexcept -> bool;

    //-----------------------------------------------------------------------------
    /// Returns true if there is any source bound to the effect.
    //-----------------------------------------------------------------------------
    virtual auto has_bound_source() const noexcept -> bool;

    //-----------------------------------------------------------------------------
    /// Updates the effect's state for the corresponding source.
    //-----------------------------------------------------------------------------
    virtual void update(source&, const duration_t&) noexcept;

private:
    virtual auto bind_impl(source& src) noexcept -> bool = 0;
    virtual void unbind_impl(source& src) noexcept = 0;
    virtual void unbind_all_impl() noexcept = 0;

    void move(effect&& rhs) noexcept;

    friend class source;

    // calling two request methods (on same source) without waiting for the ready callback inbetween the calls
    // leads to undefined behavior
    virtual void request_play(source& src, const callback& ready_to_play) noexcept;
    virtual void request_stop(source& src, const callback& ready_to_stop) noexcept;
    virtual void request_pause(source& src, const callback& ready_to_pause) noexcept;
    virtual void request_resume(source& src, const callback& ready_to_resume) noexcept;

    virtual void loop_state_changes(source& src, bool new_loop_state) noexcept;
    virtual void playback_position_changed(source& src) noexcept;
    virtual void sound_changed(source& src) noexcept;
    virtual void force_stop(source& src) noexcept;
    void remove_dead_source(source& src) noexcept;
    void change_source_address(source* from, source* to) noexcept;
    virtual void change_source_address_impl(source*, source*) noexcept;

    template <typename F>
    void bound_sources_visitor(F&& func) noexcept(noexcept(std::forward<F>(func)(std::declval<source*>())));

    std::unordered_set<source*> bound_sources_;
    bool is_iterating_bound_sources_{};
};
}
