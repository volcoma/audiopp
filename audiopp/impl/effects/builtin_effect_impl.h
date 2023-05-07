#pragma once

#include "../../effects/builtin_effect.h"
#include <al.h>
#include <efx.h>

#include <unordered_set>

namespace audio
{

class source;

namespace detail
{

class source_impl;

class builtin_effect_impl
{
public:
    using callback = builtin_effect::callback;

    builtin_effect_impl();

    builtin_effect_impl(const builtin_effect_impl&) = delete;
    builtin_effect_impl& operator=(const builtin_effect_impl&) = delete;
    builtin_effect_impl(builtin_effect_impl&&) = delete;
    builtin_effect_impl& operator=(builtin_effect_impl&&) = delete;
    ~builtin_effect_impl();

    auto bind(source& src) -> bool;
    void unbind(source& src);
    void unbind_all();
    auto has_bound(const source& src) const -> bool;
    auto has_bound(source_impl* src) const -> bool;

    void set_param(const builtin_effect::eaxreverb& param);
    void set_param(const builtin_effect::reverb& param);
    void set_param(const builtin_effect::chorus& param);
    void set_param(const builtin_effect::distortion& param);
    void set_param(const builtin_effect::echo& param);
    void set_param(const builtin_effect::flanger& param);
    void set_param(const builtin_effect::frequency_shifter& param);
    void set_param(const builtin_effect::vocal_morpher& param);
    void set_param(const builtin_effect::pitch_shifter& param);
    void set_param(const builtin_effect::ring_modulator& param);
    void set_param(const builtin_effect::autowah& param);
    void set_param(const builtin_effect::compressor& param);
    void set_param(const builtin_effect::equalizer& param);

    auto apply() -> bool;

    auto is_valid() const -> bool;

    auto get_aux_id() -> ALuint;

private:
    static auto get_source_impl(source& src) -> source_impl*;

    template <typename F>
    void
    bound_sources_visitor(F&& func) noexcept(noexcept(std::forward<F>(func)(std::declval<source_impl*>())));

    void set_effect_type(ALenum effect_type);

    void set_effect_value(ALenum effect_type, const effect_param<float>& value);
    void set_effect_value(ALenum effect_type, const effect_param<std::array<float, 3>>& value);

    template<typename T>
    void set_effect_value(ALenum effect_type, const T& value);

    void set_effect_value_impl(ALenum effect_type, ALint value);

    void check_for_error();

    ALuint effect_id_;
    ALuint aux_slot_id_;
    bool is_iterating_bound_sources_{};
    std::unordered_set<source_impl*> bound_sources_;
};

template<typename T>
void builtin_effect_impl::set_effect_value(ALenum effect_type, const T& value)
{
    int vi = static_cast<int>(value);
    set_effect_value_impl(effect_type, vi);
}

} //end of namespace detail
} //end of namespace audio
