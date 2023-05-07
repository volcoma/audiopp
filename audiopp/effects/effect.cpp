#include "effect.h"
#include "../impl/source_impl.h"
#include "../source.h"

#include <cassert>

namespace audio
{

effect::effect(effect&& rhs) noexcept
{
    move(std::move(rhs));
}

effect& effect::operator=(effect&& rhs) noexcept
{
    if(this != &rhs)
    {
        bound_sources_visitor([this](source* src) { src->remove_dead_effect(this); });
        move(std::move(rhs));
    }
    return *this;
}

effect::~effect()
{
    bound_sources_visitor([this](source* src) { src->remove_dead_effect(this); });
}

auto effect::bind(source& src) noexcept -> bool
{
    auto it = bound_sources_.find(&src);
    if(it != bound_sources_.end())
    {
        return true;
    }

    if(!bind_impl(src))
    {
        return false;
    }

    src.bind_effect(this);
    assert(!is_iterating_bound_sources_);
    bound_sources_.insert(&src);

    return true;
}

void effect::unbind(source& src) noexcept
{
    auto it = bound_sources_.find(&src);
    if(it != bound_sources_.end())
    {
        unbind_impl(src);

        src.unbind_effect(this);
        assert(!is_iterating_bound_sources_);
        bound_sources_.erase(it);
    }
}

void effect::unbind_all() noexcept
{
    unbind_all_impl();

    bound_sources_visitor([this](source* src) { src->unbind_effect(this); });
    assert(!is_iterating_bound_sources_);
    bound_sources_.clear();
}

auto effect::has_bound(const source& src) const noexcept -> bool
{
    return bound_sources_.find(&const_cast<source&>(src)) != bound_sources_.end();
}

auto effect::has_bound_source() const noexcept -> bool
{
    return !bound_sources_.empty();
}

void effect::update(source&, const duration_t&) noexcept
{
}

void effect::move(effect&& rhs) noexcept
{
    assert(!is_iterating_bound_sources_ && !rhs.is_iterating_bound_sources_);
    bound_sources_ = std::move(rhs.bound_sources_);

    bound_sources_visitor(
        [ this, rhs_addr = &rhs ](source * src) { src->change_effect_address(rhs_addr, this); });
}

void effect::request_play(source& src, const callback& ready_to_play) noexcept
{
    if(ready_to_play)
    {
        ready_to_play(src);
    }
}

void effect::request_stop(source& src, const callback& ready_to_stop) noexcept
{
    if(ready_to_stop)
    {
        ready_to_stop(src);
    }
}

void effect::request_pause(source& src, const callback& ready_to_pause) noexcept
{
    if(ready_to_pause)
    {
        ready_to_pause(src);
    }
}

void effect::request_resume(source& src, const callback& ready_to_resume) noexcept
{
    if(ready_to_resume)
    {
        ready_to_resume(src);
    }
}

void effect::loop_state_changes(source&, bool) noexcept
{
}

void effect::playback_position_changed(source&) noexcept
{
}

void effect::sound_changed(source&) noexcept
{
}

void effect::force_stop(source&) noexcept
{
}

void effect::remove_dead_source(source& src) noexcept
{
    unbind_impl(src);
    assert(bound_sources_.find(&src) != bound_sources_.end());
    assert(!is_iterating_bound_sources_);
    bound_sources_.erase(&src);
}

void effect::change_source_address(source* from, source* to) noexcept
{
    assert(from && to);
    auto it = bound_sources_.find(from);
    if(it != bound_sources_.end())
    {
        assert(!is_iterating_bound_sources_);
        bound_sources_.erase(it);
        assert(bound_sources_.find(to) == bound_sources_.end());
        bound_sources_.insert(to);
    }
    change_source_address_impl(from, to);
}

void effect::change_source_address_impl(source*, source*) noexcept
{
}

template <typename F>
void effect::bound_sources_visitor(F&& func) noexcept(
    noexcept(std::forward<F>(func)(std::declval<source*>())))
{
    assert(!is_iterating_bound_sources_);
    is_iterating_bound_sources_ = true;
    for(auto& src : bound_sources_)
    {
        std::forward<F>(func)(src);
    }
    is_iterating_bound_sources_ = false;
}
}
