#include "builtin_effect_impl.h"
//#define AL_ALEXT_PROTOTYPES
#include <efx.h>

#include "../../exception.h"
#include "../../source.h"
#include "../check.h"
#include "../source_impl.h"

#include <cassert>

namespace audio
{
namespace detail
{
namespace
{
/* Effect object functions */
static LPALGENEFFECTS alGenEffects;
static LPALDELETEEFFECTS alDeleteEffects;
static LPALISEFFECT alIsEffect;
static LPALEFFECTI alEffecti;
static LPALEFFECTIV alEffectiv;
static LPALEFFECTF alEffectf;
static LPALEFFECTFV alEffectfv;
static LPALGETEFFECTI alGetEffecti;
static LPALGETEFFECTIV alGetEffectiv;
static LPALGETEFFECTF alGetEffectf;
static LPALGETEFFECTFV alGetEffectfv;

/* Auxiliary Effect Slot object functions */
static LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots;
static LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots;
static LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot;
static LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti;
static LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv;
static LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf;
static LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv;
static LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti;
static LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv;
static LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf;
static LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv;

/* C doesn't allow casting between function and non-function pointer types, so
 * with C99 we need to use a union to reinterpret the pointer type. Pre-C99
 * still needs to use a normal cast and live with the warning (C++ is fine with
 * a regular reinterpret_cast).
 */
#if __STDC_VERSION__ >= 199901L
#define FUNCTION_CAST(T, ptr) (union{void *p; T f;}){ptr}.f
#else
#define FUNCTION_CAST(T, ptr) (T)(ptr)
#endif

bool loadEffects()
{
    /* Define a macro to help load the function pointers. */
#define LOAD_PROC(T, x)  ((x) = FUNCTION_CAST(T, alGetProcAddress(#x)))
    LOAD_PROC(LPALGENEFFECTS, alGenEffects);
    LOAD_PROC(LPALDELETEEFFECTS, alDeleteEffects);
    LOAD_PROC(LPALISEFFECT, alIsEffect);
    LOAD_PROC(LPALEFFECTI, alEffecti);
    LOAD_PROC(LPALEFFECTIV, alEffectiv);
    LOAD_PROC(LPALEFFECTF, alEffectf);
    LOAD_PROC(LPALEFFECTFV, alEffectfv);
    LOAD_PROC(LPALGETEFFECTI, alGetEffecti);
    LOAD_PROC(LPALGETEFFECTIV, alGetEffectiv);
    LOAD_PROC(LPALGETEFFECTF, alGetEffectf);
    LOAD_PROC(LPALGETEFFECTFV, alGetEffectfv);

    LOAD_PROC(LPALGENAUXILIARYEFFECTSLOTS, alGenAuxiliaryEffectSlots);
    LOAD_PROC(LPALDELETEAUXILIARYEFFECTSLOTS, alDeleteAuxiliaryEffectSlots);
    LOAD_PROC(LPALISAUXILIARYEFFECTSLOT, alIsAuxiliaryEffectSlot);
    LOAD_PROC(LPALAUXILIARYEFFECTSLOTI, alAuxiliaryEffectSloti);
    LOAD_PROC(LPALAUXILIARYEFFECTSLOTIV, alAuxiliaryEffectSlotiv);
    LOAD_PROC(LPALAUXILIARYEFFECTSLOTF, alAuxiliaryEffectSlotf);
    LOAD_PROC(LPALAUXILIARYEFFECTSLOTFV, alAuxiliaryEffectSlotfv);
    LOAD_PROC(LPALGETAUXILIARYEFFECTSLOTI, alGetAuxiliaryEffectSloti);
    LOAD_PROC(LPALGETAUXILIARYEFFECTSLOTIV, alGetAuxiliaryEffectSlotiv);
    LOAD_PROC(LPALGETAUXILIARYEFFECTSLOTF, alGetAuxiliaryEffectSlotf);
    LOAD_PROC(LPALGETAUXILIARYEFFECTSLOTFV, alGetAuxiliaryEffectSlotfv);
#undef LOAD_PROC
    return true;
}

struct openal_module
{
    openal_module()
    {
        if(!loadEffects())
        {
            throw exception("Could not load openal module");
        }
    }
    ~openal_module()
    {
    }
} module;

} // namespace

builtin_effect_impl::builtin_effect_impl()
    : effect_id_(0)
    , aux_slot_id_(0)
{
    alGenEffects(1, &effect_id_);
    if(alGetError() != AL_NO_ERROR)
    {
        throw exception("Unable to create al effect object");
    }

    alGenAuxiliaryEffectSlots(1, &aux_slot_id_);
    if(alGetError() != AL_NO_ERROR)
    {
        al_check(alDeleteEffects(1, &effect_id_));
        throw exception("Unable to create al aux effect slots object");
    }
}

builtin_effect_impl::~builtin_effect_impl()
{
    al_check(alDeleteEffects(1, &effect_id_));

    bound_sources_visitor([this](source_impl* src) { src->unbind_source_aux_slot(this); });

    al_check(alDeleteAuxiliaryEffectSlots(1, &aux_slot_id_));
}

auto builtin_effect_impl::bind(source& src) -> bool
{
    const auto src_impl = get_source_impl(src);
    assert(src_impl);

    if(has_bound(src_impl))
    {
        return true;
    }

    if(src_impl->bind_source_aux_slot_to_effect(this))
    {
        assert(!is_iterating_bound_sources_);
        bound_sources_.insert(src_impl);
        return true;
    }

    return false;
}

void builtin_effect_impl::unbind(source& src)
{
    auto src_impl = get_source_impl(src);
    if(has_bound(src_impl))
    {
        assert(src_impl);
        src_impl->unbind_source_aux_slot(this);

        assert(!is_iterating_bound_sources_);
        bound_sources_.erase(src_impl);
    }
}

void builtin_effect_impl::unbind_all()
{
    bound_sources_visitor([this](source_impl* src) { src->unbind_source_aux_slot(this); });
    assert(!is_iterating_bound_sources_);
    bound_sources_.clear();
}

auto builtin_effect_impl::has_bound(const source& src) const -> bool
{
    auto src_impl = get_source_impl(const_cast<source&>(src));
    return bound_sources_.find(src_impl) != bound_sources_.end();
}

auto builtin_effect_impl::has_bound(source_impl* src_impl) const -> bool
{
    return bound_sources_.find(src_impl) != bound_sources_.end();
}

auto builtin_effect_impl::is_valid() const -> bool
{
    return effect_id_ != 0 && aux_slot_id_ != 0;
}

auto builtin_effect_impl::apply() -> bool
{
    alAuxiliaryEffectSloti(aux_slot_id_, AL_EFFECTSLOT_EFFECT, static_cast<ALint>(effect_id_));
    return alGetError() == AL_NO_ERROR;
}

auto builtin_effect_impl::get_aux_id() -> ALuint
{
    return aux_slot_id_;
}

auto builtin_effect_impl::get_source_impl(source& src) -> source_impl*
{
    return src.impl_.get();
}

template <typename F>
void builtin_effect_impl::bound_sources_visitor(F&& func) noexcept(noexcept(std::forward<F>(func)(std::declval<source_impl*>())))
{
    assert(!is_iterating_bound_sources_);
    is_iterating_bound_sources_ = true;
    for(auto& src : bound_sources_)
    {
        std::forward<F>(func)(src);
    }
    is_iterating_bound_sources_ = false;
}

void builtin_effect_impl::set_effect_type(ALenum effect_type)
{
    alEffecti(effect_id_, AL_EFFECT_TYPE, effect_type);
    if(alGetError() != AL_NO_ERROR)
    {
        al_check(alDeleteEffects(1, &effect_id_));
        al_check(alDeleteAuxiliaryEffectSlots(1, &aux_slot_id_));
        throw exception("Unable to create an effect of that type");
    }
}

void builtin_effect_impl::set_effect_value(ALenum effect_type, const effect_param<float>& value)
{
    alEffectf(effect_id_, effect_type, value.get());
}

void builtin_effect_impl::set_effect_value(ALenum effect_type, const effect_param<std::array<float, 3>>& value)
{
    alEffectfv(effect_id_, effect_type, (ALfloat*)value.get().data());
}

void builtin_effect_impl::set_effect_value_impl(ALenum effect_type, ALint value)
{
    alEffecti(effect_id_, effect_type, value);
    check_for_error();
}

void builtin_effect_impl::check_for_error()
{
    auto error = alGetError();
    switch(error)
    {
        case AL_INVALID_NAME:
        {
            throw std::runtime_error("AL_INVALID_NAME");
        }
        case AL_INVALID_ENUM:
        {
            throw std::runtime_error("AL_INVALID_ENUM");
        }
        case AL_INVALID_OPERATION:
        {
            throw std::runtime_error("AL_INVALID_OPERATION");
        }
        case AL_INVALID_VALUE:
        {
            throw std::runtime_error("AL_INVALID_VALUE");
        }
        default: break;
    }
}

void builtin_effect_impl::set_param(const builtin_effect::eaxreverb& param)
{
    set_effect_type(AL_EFFECT_EAXREVERB);

    set_effect_value(AL_EAXREVERB_DENSITY, param.density);
    set_effect_value(AL_EAXREVERB_DIFFUSION, param.diffusion);
    set_effect_value(AL_EAXREVERB_GAIN, param.gain);
    set_effect_value(AL_EAXREVERB_GAINHF, param.gainhf);
    set_effect_value(AL_EAXREVERB_GAINLF, param.gainlf);
    set_effect_value(AL_EAXREVERB_DECAY_TIME, param.decay_time);
    set_effect_value(AL_EAXREVERB_DECAY_HFRATIO, param.decay_hfratio);
    set_effect_value(AL_EAXREVERB_DECAY_LFRATIO, param.decay_lfratio);
    set_effect_value(AL_EAXREVERB_REFLECTIONS_GAIN, param.reflections_gain);
    set_effect_value(AL_EAXREVERB_REFLECTIONS_DELAY, param.reflections_delay);
    set_effect_value(AL_EAXREVERB_REFLECTIONS_PAN, param.reflections_pan);
    set_effect_value(AL_EAXREVERB_LATE_REVERB_GAIN, param.late_reverb_gain);
    set_effect_value(AL_EAXREVERB_LATE_REVERB_DELAY, param.late_reverb_delay);
    set_effect_value(AL_EAXREVERB_LATE_REVERB_PAN, param.late_reverb_pan);
    set_effect_value(AL_EAXREVERB_ECHO_TIME, param.echo_time);
    set_effect_value(AL_EAXREVERB_ECHO_DEPTH, param.echo_depth);
    set_effect_value(AL_EAXREVERB_MODULATION_TIME, param.modulation_time);
    set_effect_value(AL_EAXREVERB_MODULATION_DEPTH, param.modulation_depth);
    set_effect_value(AL_EAXREVERB_AIR_ABSORPTION_GAINHF, param.air_absorption_gainhf);
    set_effect_value(AL_EAXREVERB_HFREFERENCE, param.hfreference);
    set_effect_value(AL_EAXREVERB_LFREFERENCE, param.lfreference);
    set_effect_value(AL_EAXREVERB_ROOM_ROLLOFF_FACTOR, param.room_rolloff_factor);
}

void builtin_effect_impl::set_param(const builtin_effect::reverb& param)
{
    set_effect_type(AL_EFFECT_REVERB);

    set_effect_value(AL_REVERB_DENSITY, param.density);
    set_effect_value(AL_REVERB_DIFFUSION, param.diffusion);
    set_effect_value(AL_REVERB_GAIN, param.gain);
    set_effect_value(AL_REVERB_GAINHF, param.gainhf);
    set_effect_value(AL_REVERB_DECAY_TIME, param.decay_time);
    set_effect_value(AL_REVERB_DECAY_HFRATIO, param.decay_hfratio);
    set_effect_value(AL_REVERB_REFLECTIONS_GAIN, param.reflections_gain);
    set_effect_value(AL_REVERB_REFLECTIONS_DELAY, param.reflections_delay);
    set_effect_value(AL_REVERB_LATE_REVERB_GAIN, param.late_reverb_gain);
    set_effect_value(AL_REVERB_LATE_REVERB_DELAY, param.late_reverb_delay);
    set_effect_value(AL_REVERB_AIR_ABSORPTION_GAINHF, param.air_absorption_gainhf);
    set_effect_value(AL_REVERB_ROOM_ROLLOFF_FACTOR, param.room_rolloff_factor);
    set_effect_value(AL_REVERB_DECAY_HFLIMIT, param.decay_hflimit);
}

void builtin_effect_impl::set_param(const builtin_effect::chorus& param)
{
    set_effect_type(AL_EFFECT_CHORUS);

    set_effect_value(AL_CHORUS_WAVEFORM, param.waveform);
    set_effect_value(AL_CHORUS_PHASE, param.phase);
    set_effect_value(AL_CHORUS_RATE, param.rate);
    set_effect_value(AL_CHORUS_DEPTH, param.depth);
    set_effect_value(AL_CHORUS_FEEDBACK, param.feedback);
    set_effect_value(AL_CHORUS_DELAY, param.delay);
}

void builtin_effect_impl::set_param(const builtin_effect::distortion& param)
{
    set_effect_type(AL_EFFECT_DISTORTION);

    set_effect_value(AL_DISTORTION_EDGE, param.edge);
    set_effect_value(AL_DISTORTION_GAIN, param.gain);
    set_effect_value(AL_DISTORTION_LOWPASS_CUTOFF, param.lowpass_cutoff);
    set_effect_value(AL_DISTORTION_EQCENTER, param.eqcenter);
    set_effect_value(AL_DISTORTION_EQBANDWIDTH, param.eqbandwidth);
}

void builtin_effect_impl::set_param(const builtin_effect::echo& param)
{
    set_effect_type(AL_EFFECT_ECHO);

    set_effect_value(AL_ECHO_DELAY, param.delay);
    set_effect_value(AL_ECHO_LRDELAY, param.lrdelay);
    set_effect_value(AL_ECHO_DAMPING, param.damping);
    set_effect_value(AL_ECHO_FEEDBACK, param.feedback);
    set_effect_value(AL_ECHO_SPREAD, param.spread);
}

void builtin_effect_impl::set_param(const builtin_effect::flanger& param)
{
    set_effect_type(AL_EFFECT_FLANGER);

    set_effect_value(AL_FLANGER_WAVEFORM, param.waveform);
    set_effect_value(AL_FLANGER_PHASE, param.phase);
    set_effect_value(AL_FLANGER_RATE, param.rate);
    set_effect_value(AL_FLANGER_DEPTH, param.depth);
    set_effect_value(AL_FLANGER_FEEDBACK, param.feedback);
    set_effect_value(AL_FLANGER_DELAY, param.delay);
}

void builtin_effect_impl::set_param(const builtin_effect::frequency_shifter& param)
{
    set_effect_type(AL_EFFECT_FREQUENCY_SHIFTER);

    set_effect_value(AL_FREQUENCY_SHIFTER_FREQUENCY, param.frequency);
    set_effect_value(AL_FREQUENCY_SHIFTER_LEFT_DIRECTION, param.left_direction);
    set_effect_value(AL_FREQUENCY_SHIFTER_RIGHT_DIRECTION, param.right_direction);
}

void builtin_effect_impl::set_param(const builtin_effect::vocal_morpher& param)
{
    set_effect_type(AL_EFFECT_VOCAL_MORPHER);

    set_effect_value(AL_VOCAL_MORPHER_PHONEMEA, param.phonemea);
    set_effect_value(AL_VOCAL_MORPHER_PHONEMEB, param.phonemeb);
    set_effect_value(AL_VOCAL_MORPHER_PHONEMEA_COARSE_TUNING, param.phonemea_coarse_tuning);
    set_effect_value(AL_VOCAL_MORPHER_PHONEMEB_COARSE_TUNING, param.phonemeb_coarse_tuning);
    set_effect_value(AL_VOCAL_MORPHER_WAVEFORM, param.waveform);
    set_effect_value(AL_VOCAL_MORPHER_RATE, param.rate);
}

void builtin_effect_impl::set_param(const builtin_effect::pitch_shifter& param)
{
    set_effect_type(AL_EFFECT_PITCH_SHIFTER);

    set_effect_value(AL_PITCH_SHIFTER_COARSE_TUNE, param.coarse_tune);
    set_effect_value(AL_PITCH_SHIFTER_FINE_TUNE, param.fine_tune);
}

void builtin_effect_impl::set_param(const builtin_effect::ring_modulator& param)
{
    set_effect_type(AL_EFFECT_RING_MODULATOR);

    set_effect_value(AL_RING_MODULATOR_FREQUENCY, param.frequency);
    set_effect_value(AL_RING_MODULATOR_HIGHPASS_CUTOFF, param.highpass_cutoff);
    set_effect_value(AL_RING_MODULATOR_WAVEFORM, param.waveform);
}

void builtin_effect_impl::set_param(const builtin_effect::autowah& param)
{
    set_effect_type(AL_EFFECT_AUTOWAH);

    set_effect_value(AL_AUTOWAH_ATTACK_TIME, param.attack_time);
    set_effect_value(AL_AUTOWAH_RELEASE_TIME, param.release_time);
    set_effect_value(AL_AUTOWAH_RESONANCE, param.resonance);
    set_effect_value(AL_AUTOWAH_PEAK_GAIN, param.peak_gain);
}

void builtin_effect_impl::set_param(const builtin_effect::compressor& param)
{
    set_effect_type(AL_EFFECT_COMPRESSOR);
    set_effect_value(AL_COMPRESSOR_ONOFF, param.onoff);
}

void builtin_effect_impl::set_param(const builtin_effect::equalizer& param)
{
    set_effect_type(AL_EFFECT_EQUALIZER);

    set_effect_value(AL_EQUALIZER_LOW_GAIN, param.low_gain);
    set_effect_value(AL_EQUALIZER_LOW_CUTOFF, param.low_cutoff);
    set_effect_value(AL_EQUALIZER_MID1_GAIN, param.mid1_gain);
    set_effect_value(AL_EQUALIZER_MID1_CENTER, param.mid1_center);
    set_effect_value(AL_EQUALIZER_MID1_WIDTH, param.mid1_width);
    set_effect_value(AL_EQUALIZER_MID2_GAIN, param.mid2_gain);
    set_effect_value(AL_EQUALIZER_MID2_CENTER, param.mid2_center);
    set_effect_value(AL_EQUALIZER_MID2_WIDTH, param.mid2_width);
    set_effect_value(AL_EQUALIZER_HIGH_GAIN, param.high_gain);
    set_effect_value(AL_EQUALIZER_HIGH_CUTOFF, param.high_cutoff);
}

} //end of namespace detail
} //end of namespace audio
