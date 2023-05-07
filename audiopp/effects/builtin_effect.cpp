#include "../impl/effects/builtin_effect_impl.h"

namespace audio
{

namespace
{

struct make_shared_enabler : public builtin_effect
{
    make_shared_enabler() = default;
};

template<typename T>
static std::shared_ptr<builtin_effect> create_impl(const T& param)
{
    auto effect = std::make_shared<make_shared_enabler>();
    effect->set_param(param);
    return effect;
}

}

builtin_effect::builtin_effect()
    : impl_(std::make_unique<detail::builtin_effect_impl>())
{
}

builtin_effect::builtin_effect(builtin_effect&&) noexcept = default;
builtin_effect& builtin_effect::operator=(builtin_effect&&) noexcept = default;
builtin_effect::~builtin_effect() = default;

builtin_effect::ptr builtin_effect::create(const builtin_effect::eaxreverb& param){ return create_impl(param); }
builtin_effect::ptr builtin_effect::create(const builtin_effect::reverb& param){ return create_impl(param); }
builtin_effect::ptr builtin_effect::create(const builtin_effect::chorus& param){ return create_impl(param); }
builtin_effect::ptr builtin_effect::create(const builtin_effect::distortion& param){ return create_impl(param); }
builtin_effect::ptr builtin_effect::create(const builtin_effect::echo& param){ return create_impl(param); }
builtin_effect::ptr builtin_effect::create(const builtin_effect::flanger& param){ return create_impl(param); }
builtin_effect::ptr builtin_effect::create(const builtin_effect::frequency_shifter& param){ return create_impl(param); }
builtin_effect::ptr builtin_effect::create(const builtin_effect::vocal_morpher& param){ return create_impl(param); }
builtin_effect::ptr builtin_effect::create(const builtin_effect::pitch_shifter& param){ return create_impl(param); }
builtin_effect::ptr builtin_effect::create(const builtin_effect::ring_modulator& param){ return create_impl(param); }
builtin_effect::ptr builtin_effect::create(const builtin_effect::autowah& param){ return create_impl(param); }
builtin_effect::ptr builtin_effect::create(const builtin_effect::compressor& param){ return create_impl(param); }
builtin_effect::ptr builtin_effect::create(const builtin_effect::equalizer& param){ return create_impl(param); }

void builtin_effect::set_param(const eaxreverb& param){ impl_->set_param(param); }
void builtin_effect::set_param(const reverb& param){ impl_->set_param(param); }
void builtin_effect::set_param(const chorus& param){ impl_->set_param(param); }
void builtin_effect::set_param(const distortion& param){ impl_->set_param(param); }
void builtin_effect::set_param(const echo& param){ impl_->set_param(param); }
void builtin_effect::set_param(const flanger& param){ impl_->set_param(param); }
void builtin_effect::set_param(const frequency_shifter& param){ impl_->set_param(param); }
void builtin_effect::set_param(const vocal_morpher& param){ impl_->set_param(param); }
void builtin_effect::set_param(const pitch_shifter& param){ impl_->set_param(param); }
void builtin_effect::set_param(const ring_modulator& param){ impl_->set_param(param); }
void builtin_effect::set_param(const autowah& param){ impl_->set_param(param); }
void builtin_effect::set_param(const compressor& param){ impl_->set_param(param); }
void builtin_effect::set_param(const equalizer& param){ impl_->set_param(param); }

auto builtin_effect::is_valid() const noexcept -> bool
{
    return impl_ && impl_->is_valid();
}

auto builtin_effect::apply() noexcept -> bool
{
    return impl_ && impl_->apply();
}

auto builtin_effect::bind_impl(source& src) noexcept -> bool
{
    return impl_ && impl_->bind(src);
}

void builtin_effect::unbind_impl(source& src) noexcept
{
    if(impl_)
    {
        impl_->unbind(src);
    }
}

void builtin_effect::unbind_all_impl() noexcept
{
    if(impl_)
    {
        impl_->unbind_all();
    }
}

} //end of namespace audio
